"""
TOPIC: Singleton pattern -- the "module-level instance" approach.

Java's enum-based singleton works because the JVM guarantees an enum
constant is instantiated exactly once. Python has no enum trick that
maps onto this directly, but Python modules give you the same
guarantee for free: a module is only ever imported (and therefore only
ever executed) once per process -- Python caches it in `sys.modules`
and every later `import` just hands back the same cached module
object. So a plain instance created at module level IS the singleton,
with no locking, metaclass, or decorator required.

This is the simplest thread-safe singleton available in Python, for
exactly the same reason the enum trick is the simplest one in Java:
the runtime itself enforces "created exactly once".
"""


class _DatabaseConnection:
    def connect(self) -> None:
        print("Connected to DB")


# INSTANCE is the only object of this class we ever create, at import
# time. Every other module that does `from singleton_enum import INSTANCE`
# gets this exact same object back.
INSTANCE = _DatabaseConnection()


def main() -> None:
    db1 = INSTANCE
    db2 = INSTANCE
    db1.connect()
    print(f"Same instance? {db1 is db2}")


if __name__ == "__main__":
    main()

# INSTANCE is the only instance we construct at module scope.
# Python's import system guarantees this module -- and therefore this
# object -- is created exactly once per process.
