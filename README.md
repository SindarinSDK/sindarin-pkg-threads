# sindarin-pkg-threads

Threading primitives for the [Sindarin](https://github.com/SindarinSDK/sindarin-compiler) programming language — mutex, semaphore, condition variable, reader-writer lock, wait group, thread scope, and thread pool.

## Installation

Add the package as a dependency in your `sn.yaml`:

```yaml
dependencies:
- name: sindarin-pkg-threads
  git: git@github.com:SindarinSDK/sindarin-pkg-threads.git
  branch: main
```

Then run `sn --install` to fetch the package.

## Quick Start

```sindarin
import "sindarin-pkg-threads/src/mutex"

sync var counter: int = 0

fn worker(m: Mutex): int =>
    m.withLock(fn(): void =>
        counter += 1
    )
    return 1

fn main(): void =>
    var m: Mutex = Mutex.new()

    var t1: int = &worker(m)
    var t2: int = &worker(m)
    var t3: int = &worker(m)

    [t1, t2, t3]!

    print($"counter = {counter}\n")  // counter = 3
    m.dispose()
```

## Modules

| Module | Import | Description |
|--------|--------|-------------|
| [Mutex](#mutex) | `import "sindarin-pkg-threads/src/mutex"` | Mutual exclusion lock |
| [Semaphore](#semaphore) | `import "sindarin-pkg-threads/src/semaphore"` | Counting semaphore |
| [Condition](#condition-variable) | `import "sindarin-pkg-threads/src/condition"` | Condition variable |
| [ReaderWriterLock](#readerwritelock) | `import "sindarin-pkg-threads/src/rwlock"` | Fair reader-writer lock |
| [WaitGroup](#waitgroup) | `import "sindarin-pkg-threads/src/waitgroup"` | Wait for a set of threads to finish |
| [ThreadScope](#threadscope) | `import "sindarin-pkg-threads/src/threadscope"` | Lifetime-safe scoped threads |
| [ThreadPool](#threadpool) | `import "sindarin-pkg-threads/src/threadpool"` | Fixed pool of worker threads |

---

## Mutex

```sindarin
import "sindarin-pkg-threads/src/mutex"
```

A mutual exclusion lock. Only one thread may hold the lock at a time.

| Method | Signature | Description |
|--------|-----------|-------------|
| `new` | `static fn new(): Mutex` | Create a new unlocked mutex |
| `lock` | `fn lock(): void` | Acquire the lock, blocking until available |
| `unlock` | `fn unlock(): void` | Release the lock |
| `tryLock` | `fn tryLock(): bool` | Acquire without blocking; returns `false` if already held |
| `withLock` | `fn withLock(body: fn(): void): void` | Lock, run body, unlock — always releases |
| `tryWithLock` | `fn tryWithLock(body: fn(): void): bool` | Run body only if lock is immediately available |
| `dispose` | `fn dispose(): void` | Free resources |

```sindarin
var m: Mutex = Mutex.new()

// Preferred: withLock guarantees unlock even on early return
m.withLock(fn(): void =>
    sharedData += 1
)

// Manual lock/unlock
m.lock()
sharedData += 1
m.unlock()

m.dispose()
```

---

## Semaphore

```sindarin
import "sindarin-pkg-threads/src/semaphore"
```

A counting semaphore. Threads acquire permits (blocking if none are available) and release them when done.

| Method | Signature | Description |
|--------|-----------|-------------|
| `new` | `static fn new(initial: int): Semaphore` | Create with `initial` permits available |
| `wait` | `fn wait(): void` | Acquire one permit, blocking if none available |
| `post` | `fn post(): void` | Release one permit |
| `tryWait` | `fn tryWait(): bool` | Acquire without blocking; returns `false` if no permits |
| `withPermit` | `fn withPermit(body: fn(): void): void` | Acquire, run body, release |
| `dispose` | `fn dispose(): void` | Free resources |

```sindarin
// Limit concurrency to 3 simultaneous workers
var sem: Semaphore = Semaphore.new(3)

fn worker(): int =>
    sem.withPermit(fn(): void =>
        doExpensiveWork()
    )
    return 1

sem.dispose()
```

---

## Condition Variable

```sindarin
import "sindarin-pkg-threads/src/condition"
```

A condition variable for signalling between threads. Always used together with a `Mutex` to protect the shared state being tested.

| Method | Signature | Description |
|--------|-----------|-------------|
| `new` | `static fn new(): Condition` | Create a new condition variable |
| `wait` | `fn wait(mutex: Mutex): void` | Sleep until signalled; atomically releases and re-acquires mutex |
| `timedWait` | `fn timedWait(mutex: Mutex, ms: int): bool` | Like `wait`, returns `false` on timeout |
| `waitUntil` | `fn waitUntil(mutex: Mutex, predicate: fn(): bool): void` | Sleep until predicate returns `true` |
| `timedWaitUntil` | `fn timedWaitUntil(mutex: Mutex, ms: int, predicate: fn(): bool): bool` | Like `waitUntil`, returns `false` on timeout |
| `signal` | `fn signal(): void` | Wake one waiting thread |
| `broadcast` | `fn broadcast(): void` | Wake all waiting threads |
| `dispose` | `fn dispose(): void` | Free resources |

```sindarin
var m: Mutex  = Mutex.new()
var cv: Condition = Condition.new()
sync var ready: int = 0

// Waiter thread
fn waiter(m: Mutex, cv: Condition): int =>
    m.lock()
    cv.waitUntil(m, fn(): bool => ready == 1)
    m.unlock()
    return 1

// Signaller (main thread)
var t: int = &waiter(m, cv)
m.lock()
ready = 1
cv.signal()
m.unlock()
t!

m.dispose()
cv.dispose()
```

---

## ReaderWriterLock

```sindarin
import "sindarin-pkg-threads/src/rwlock"
```

A fair reader-writer lock. Multiple readers may hold the lock concurrently; writers get exclusive access. Writers are not starved — when a writer arrives it blocks new readers until it has run.

| Method | Signature | Description |
|--------|-----------|-------------|
| `new` | `static fn new(): ReaderWriterLock` | Create a new lock |
| `readLock` | `fn readLock(): void` | Acquire a shared read lock |
| `readUnlock` | `fn readUnlock(): void` | Release a shared read lock |
| `writeLock` | `fn writeLock(): void` | Acquire an exclusive write lock |
| `writeUnlock` | `fn writeUnlock(): void` | Release an exclusive write lock |
| `withReadLock` | `fn withReadLock(body: fn(): void): void` | Acquire read lock, run body, release |
| `withWriteLock` | `fn withWriteLock(body: fn(): void): void` | Acquire write lock, run body, release |
| `dispose` | `fn dispose(): void` | Free resources |

```sindarin
var rw: ReaderWriterLock = ReaderWriterLock.new()

fn readValue(): int =>
    rw.withReadLock(fn(): void =>
        print(sharedValue)
    )
    return 1

fn writeValue(v: int): int =>
    rw.withWriteLock(fn(): void =>
        sharedValue = v
    )
    return 1

rw.dispose()
```

---

## WaitGroup

```sindarin
import "sindarin-pkg-threads/src/waitgroup"
```

A WaitGroup waits for a set of concurrent operations to finish. Call `add(n)` before starting the operations, `done()` inside each one, and `wait()` to block until all have completed.

| Method | Signature | Description |
|--------|-----------|-------------|
| `new` | `static fn new(): WaitGroup` | Create a new wait group with counter = 0 |
| `add` | `fn add(n: int): void` | Increment the counter by `n` |
| `done` | `fn done(): void` | Decrement the counter by 1; unblocks `wait()` when it reaches 0 |
| `wait` | `fn wait(): void` | Block until the counter reaches 0 |
| `dispose` | `fn dispose(): void` | Free resources |

```sindarin
var wg: WaitGroup = WaitGroup.new()

fn worker(wg: WaitGroup): void =>
    doWork()
    wg.done()

wg.add(3)
&worker(wg)
&worker(wg)
&worker(wg)
wg.wait()

wg.dispose()
```

> **Note:** Use `lock(syncVar) => syncVar += 1` inside worker functions to safely modify `sync var` counters shared across threads.

---

## ThreadScope

```sindarin
import "sindarin-pkg-threads/src/threadscope"
```

`ThreadScope.run()` provides lifetime-safe thread spawning. All threads spawned via `scope.spawn()` are automatically joined before `run()` returns, guaranteeing that variables captured by the threads remain alive for their entire duration.

| Method | Signature | Description |
|--------|-----------|-------------|
| `run` | `static fn run(body: fn(ThreadScope): void): void` | Run body; join all spawned threads before returning |
| `spawn` | `fn spawn(task: fn(): void): void` | Spawn a thread within this scope |

```sindarin
sync var counter: int = 0

ThreadScope.run(fn(scope: ThreadScope): void =>
    scope.spawn(fn(): void =>
        lock(counter) =>
            counter += 1
    )
    scope.spawn(fn(): void =>
        lock(counter) =>
            counter += 1
    )
    scope.spawn(fn(): void =>
        lock(counter) =>
            counter += 1
    )
)
// All threads joined here — counter is guaranteed to be 3
print($"counter = {counter}\n")
```

---

## ThreadPool

```sindarin
import "sindarin-pkg-threads/src/threadpool"
```

A fixed pool of worker threads that execute submitted tasks concurrently. Use `using` to automatically drain and dispose the pool when the block exits.

| Method | Signature | Description |
|--------|-----------|-------------|
| `new` | `static fn new(workers: int): ThreadPool` | Create a pool with `workers` threads |
| `submit` | `fn submit(task: fn(): void): void` | Submit a task for execution by a worker thread |
| `shutdown` | `fn shutdown(): void` | Drain the queue and wait for all tasks to finish |
| `dispose` | `fn dispose(): void` | `shutdown()` then free all resources |

```sindarin
var wg: WaitGroup = WaitGroup.new()

using pool = ThreadPool.new(4) =>
    for i in 1..11 =>
        wg.add(1)
        pool.submit(fn(): void =>
            doWork()
            wg.done()
        )
    wg.wait()
// pool.dispose() called automatically — all workers joined

wg.dispose()
```

---

## Thread Safety Notes

- `sync var` declares a module-level variable with an associated mutex. Use `lock(varName) => ...` to protect concurrent read-modify-write operations on it.
- All primitives in this package are safe to pass across thread boundaries as `as ref` (pointer) arguments.
- Always join or wait for threads before their shared data goes out of scope. `ThreadScope.run()` enforces this automatically.

## Development

```bash
make test    # Build and run all tests
make clean   # Remove build artifacts
```
