// =====================================================================
// TOPIC: Singleton pattern -- the enum-based approach.
//
// Java guarantees that an enum constant is instantiated exactly once,
// even under reflection or (unlike other singleton styles) across
// deserialization. INSTANCE is the only constant we declare, so it IS
// the singleton -- Java creates it automatically the first time the
// enum class is initialized, and there is no way to create a second
// one.
//
// This is widely considered the simplest thread-safe singleton in
// Java: no manual locking, no eager-vs-lazy trade-off to reason about.
// =====================================================================

enum DatabaseConnection {
    INSTANCE;

    public void connect() {
        System.out.println("Connected to DB");
    }
}

public class SingletonEnum {
    public static void main(String[] args) {
        DatabaseConnection db1 = DatabaseConnection.INSTANCE;
        DatabaseConnection db2 = DatabaseConnection.INSTANCE;
        db1.connect();
        System.out.println("Same instance? " + (db1 == db2));
    }
}

// INSTANCE is the only enum constant we declared.
// Java automatically creates this instance when the enum class is
// initialized -- that single guaranteed instance is the singleton.
