// =====================================================================
// TOPIC: Singleton pattern -- the Meyer's Singleton (function-local
// static), C++'s closest equivalent to Java's enum-based singleton.
//
// C++ doesn't have an enum-with-instance trick like Java's. The
// closest idiom that gives you "constructed exactly once, thread-safe,
// no manual locking" for free is a function-local `static` variable:
// the C++11 standard *guarantees* that if multiple threads call
// getInstance() concurrently the first time, exactly one of them runs
// the constructor and the rest block until it's done. That guarantee
// is what makes this the simplest thread-safe singleton available in
// C++, the same way the enum trick is the simplest one in Java.
// =====================================================================

#include <iostream>

class DatabaseConnection {
public:
    static DatabaseConnection& getInstance() {
        static DatabaseConnection instance; // constructed exactly once, guaranteed by the standard
        return instance;
    }

    void connect() {
        std::cout << "Connected to DB\n";
    }

    // Not copyable/movable: there must only ever be the one instance.
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

private:
    DatabaseConnection() = default;
};

int main() {
    DatabaseConnection& db1 = DatabaseConnection::getInstance();
    DatabaseConnection& db2 = DatabaseConnection::getInstance();
    db1.connect();
    std::cout << "Same instance? " << (&db1 == &db2 ? "true" : "false") << "\n";

    return 0;
}

// getInstance()'s function-local static is the only instance we ever
// construct. The C++ standard guarantees it is created exactly once,
// even if multiple threads call getInstance() for the first time
// concurrently.
