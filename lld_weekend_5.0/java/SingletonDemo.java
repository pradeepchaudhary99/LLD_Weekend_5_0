import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

// =====================================================================
// TOPIC: Singleton pattern -- lazy initialization with DOUBLE-CHECKED
// LOCKING, contrasted with eager initialization.
//
// THE BUG IN THE ORIGINAL DRAFT: it checked `instance == null` once,
// then entered a `synchronized` block and created the instance WITHOUT
// checking again inside the lock. Picture 5 threads all passing the
// first null-check before any of them acquires the lock -- they would
// then enter the synchronized block one at a time and EACH create a
// new ConfigurationManager, printing "Object created" 5 times. That
// defeats the entire point of a singleton.
//
// The fix is the second null-check *inside* the synchronized block
// ("double-checked locking"): only the very first thread to get the
// lock actually creates the instance; every thread after it sees
// instance != null and skips construction. `volatile` is required too
// -- without it, a thread could see a half-constructed object due to
// instruction reordering by the JIT/CPU.
// =====================================================================

class ConfigurationManager {
    private Map<String, String> configuration = new ConcurrentHashMap<>();

    // `volatile` prevents other threads from observing a
    // partially-constructed instance.
    private static volatile ConfigurationManager instance;

    private ConfigurationManager() {
    }

    public static ConfigurationManager getInstance() {
        // First check (no lock): avoids paying the synchronization
        // cost on every call once the instance already exists.
        if (instance == null) {
            synchronized (ConfigurationManager.class) {
                // Second check (inside the lock): only the thread that
                // wins the race to acquire the lock actually creates
                // the object. Every other thread that was blocked here
                // will see instance != null once it gets the lock.
                if (instance == null) {
                    System.out.println("Object created by " + Thread.currentThread().getName());
                    instance = new ConfigurationManager();
                }
            }
        }
        return instance;
    }
}

// Eager initialization: the instance is created the moment the class
// is loaded by the JVM. The JVM's classloading guarantees make this
// thread-safe for free, with no explicit synchronization needed -- at
// the cost of creating the object even if it's never used.
class ConfigurationManagerWithoutThreadSynchronized {
    private Map<String, String> configuration = new ConcurrentHashMap<>();

    private static final ConfigurationManagerWithoutThreadSynchronized instance =
            new ConfigurationManagerWithoutThreadSynchronized();

    private ConfigurationManagerWithoutThreadSynchronized() {
    }

    public static ConfigurationManagerWithoutThreadSynchronized getInstance() {
        return instance;
    }
}

public class SingletonDemo {
    public static void main(String[] args) throws InterruptedException {
        System.out.println("== Double-checked locking: 100 threads racing getInstance() ==");
        Thread[] threads = new Thread[100];
        for (int i = 0; i < 100; i++) {
            threads[i] = new Thread(ConfigurationManager::getInstance);
            threads[i].start();
        }
        for (Thread t : threads) {
            t.join();
        }
        System.out.println("\"Object created\" should have printed exactly once above.");

        System.out.println("\n== Eager initialization ==");
        ConfigurationManagerWithoutThreadSynchronized eager1 =
                ConfigurationManagerWithoutThreadSynchronized.getInstance();
        ConfigurationManagerWithoutThreadSynchronized eager2 =
                ConfigurationManagerWithoutThreadSynchronized.getInstance();
        System.out.println("Same instance? " + (eager1 == eager2));
    }
}
