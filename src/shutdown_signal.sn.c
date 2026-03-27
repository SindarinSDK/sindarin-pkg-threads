/* ==============================================================================
 * shutdown_signal.sn.c — Self-pipe trick for cancellable blocking operations
 * ==============================================================================
 * Creates a pipe (POSIX) or loopback socket pair (Windows). Writing to the
 * pipe wakes any select()/poll() that includes the read-end fd.
 * ============================================================================== */

#include "threads_internal.h"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <unistd.h>
    #include <fcntl.h>
#endif

typedef __sn__ShutdownSignal RtShutdownSignal;

/* ============================================================================
 * Platform helpers
 * ============================================================================ */

#ifdef _WIN32
/* Windows: create a loopback TCP socket pair (select only works on sockets) */
static int create_signal_pair(SOCKET pair[2]) {
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(listener);
        return -1;
    }
    if (listen(listener, 1) == SOCKET_ERROR) {
        closesocket(listener);
        return -1;
    }

    int addrlen = sizeof(addr);
    getsockname(listener, (struct sockaddr *)&addr, &addrlen);

    pair[1] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pair[1] == INVALID_SOCKET) {
        closesocket(listener);
        return -1;
    }
    if (connect(pair[1], (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(pair[1]);
        closesocket(listener);
        return -1;
    }

    pair[0] = accept(listener, NULL, NULL);
    closesocket(listener);
    if (pair[0] == INVALID_SOCKET) {
        closesocket(pair[1]);
        return -1;
    }
    return 0;
}
#endif

/* ============================================================================
 * WinSock Initialization (Windows only)
 * ============================================================================ */

#ifdef _WIN32
static int winsock_initialized = 0;

static void ensure_winsock_initialized(void) {
    if (!winsock_initialized) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            fprintf(stderr, "WSAStartup failed: %d\n", result);
            exit(1);
        }
        winsock_initialized = 1;
    }
}
#else
#define ensure_winsock_initialized() ((void)0)
#endif

/* ============================================================================
 * ShutdownSignal creation
 * ============================================================================ */

RtShutdownSignal *sn_shutdown_signal_new(void) {
    ensure_winsock_initialized();
    RtShutdownSignal *sig = __sn__ShutdownSignal__new();
    if (sig == NULL) {
        fprintf(stderr, "ShutdownSignal.new: allocation failed\n");
        exit(1);
    }

    ShutdownSignalInternal *internal = (ShutdownSignalInternal *)calloc(1, sizeof(ShutdownSignalInternal));
    if (internal == NULL) {
        fprintf(stderr, "ShutdownSignal.new: internal allocation failed\n");
        exit(1);
    }

    internal->signalled = 0;

#ifdef _WIN32
    SOCKET pair[2];
    if (create_signal_pair(pair) != 0) {
        free(internal);
        fprintf(stderr, "ShutdownSignal.new: socket pair creation failed\n");
        exit(1);
    }
    internal->pipe_fd[0] = (long long)pair[0];
    internal->pipe_fd[1] = (long long)pair[1];
#else
    if (pipe(internal->pipe_fd) != 0) {
        free(internal);
        fprintf(stderr, "ShutdownSignal.new: pipe creation failed\n");
        exit(1);
    }
    /* Set read end to non-blocking so signal() never blocks */
    int flags = fcntl(internal->pipe_fd[0], F_GETFL, 0);
    fcntl(internal->pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
    /* Set write end to non-blocking too */
    flags = fcntl(internal->pipe_fd[1], F_GETFL, 0);
    fcntl(internal->pipe_fd[1], F_SETFL, flags | O_NONBLOCK);
#endif

    sig->internal = (long long)(uintptr_t)internal;
    return sig;
}

/* ============================================================================
 * ShutdownSignal operations
 * ============================================================================ */

void sn_shutdown_signal_signal(RtShutdownSignal *sig) {
    if (sig == NULL || !sig->internal) return;
    ShutdownSignalInternal *internal = (ShutdownSignalInternal *)(uintptr_t)sig->internal;

    /* Atomic CAS: only write once (idempotent) */
    if (__sync_val_compare_and_swap(&internal->signalled, 0, 1) == 0) {
        char byte = 1;
#ifdef _WIN32
        send((SOCKET)internal->pipe_fd[1], &byte, 1, 0);
#else
        (void)write(internal->pipe_fd[1], &byte, 1);
#endif
    }
}

long long sn_shutdown_signal_fd(RtShutdownSignal *sig) {
    if (sig == NULL || !sig->internal) return -1;
    ShutdownSignalInternal *internal = (ShutdownSignalInternal *)(uintptr_t)sig->internal;
    return (long long)internal->pipe_fd[0];
}

/* ============================================================================
 * ShutdownSignal lifecycle
 * ============================================================================ */

void sn_shutdown_signal_dispose(RtShutdownSignal *sig) {
    if (sig == NULL || !sig->internal) return;
    ShutdownSignalInternal *internal = (ShutdownSignalInternal *)(uintptr_t)sig->internal;

#ifdef _WIN32
    if (internal->pipe_fd[0]) closesocket((SOCKET)internal->pipe_fd[0]);
    if (internal->pipe_fd[1]) closesocket((SOCKET)internal->pipe_fd[1]);
#else
    if (internal->pipe_fd[0] >= 0) close(internal->pipe_fd[0]);
    if (internal->pipe_fd[1] >= 0) close(internal->pipe_fd[1]);
#endif

    free(internal);
    sig->internal = 0;
}
