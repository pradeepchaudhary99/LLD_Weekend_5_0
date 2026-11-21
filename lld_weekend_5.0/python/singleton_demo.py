"""
TOPIC: Singleton pattern -- lazy initialization with DOUBLE-CHECKED
LOCKING, contrasted with eager initialization.

THE BUG IN THE ORIGINAL JAVA DRAFT this was translated from: it checked
`instance is None` once, then entered a locked section and created the
instance WITHOUT checking again inside the lock. Picture 5 threads all
passing the first None-check before any of them acquires the lock --
they would then enter the locked section one at a time and EACH create
a new ConfigurationManager, printing "Object created" 5 times. That
defeats the entire point of a singleton.

The fix is the second None-check *inside* the lock
("double-checked locking"): only the very first thread to get the lock
actually creates the instance; every thread after it sees
instance is not None and skips construction.

NOTE: CPython's GIL makes a single bytecode-level race less likely to
manifest than in Java, but the lock is still required for correctness
-- do not rely on the GIL as a substitute for explicit synchronization.
"""

import threading


class ConfigurationManager:
    _instance: "ConfigurationManager | None" = None
    _lock = threading.Lock()

    def __init__(self) -> None:
        self.configuration: dict[str, str] = {}

    @classmethod
    def get_instance(cls) -> "ConfigurationManager":
        # First check (no lock): avoids paying the synchronization
        # cost on every call once the instance already exists.
        if cls._instance is None:
            with cls._lock:
                # Second check (inside the lock): only the thread that
                # wins the race to acquire the lock actually creates
                # the object. Every other thread that was blocked here
                # will see _instance is not None once it gets the lock.
                if cls._instance is None:
                    print(f"Object created by {threading.current_thread().name}")
                    cls._instance = cls()
        return cls._instance


class ConfigurationManagerEager:
    """
    Eager initialization: the single instance is created the moment
    the class body is created as part of a (private-by-convention)
    class attribute, before any thread could possibly race to create
    one. No explicit locking is needed, at the cost of creating the
    object even if it's never used.
    """

    _instance: "ConfigurationManagerEager" = None  # filled in right after the class body

    def __init__(self) -> None:
        self.configuration: dict[str, str] = {}

    @classmethod
    def get_instance(cls) -> "ConfigurationManagerEager":
        return cls._instance


ConfigurationManagerEager._instance = ConfigurationManagerEager()


def main() -> None:
    print("== Double-checked locking: 100 threads racing get_instance() ==")
    threads = [threading.Thread(target=ConfigurationManager.get_instance) for _ in range(100)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print('"Object created" should have printed exactly once above.')

    print("\n== Eager initialization ==")
    eager1 = ConfigurationManagerEager.get_instance()
    eager2 = ConfigurationManagerEager.get_instance()
    print(f"Same instance? {eager1 is eager2}")


if __name__ == "__main__":
    main()
