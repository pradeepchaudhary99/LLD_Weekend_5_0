// =====================================================================
// TOPIC: Singleton pattern -- lazy initialization with DOUBLE-CHECKED
// LOCKING, contrasted with eager initialization and with C++11's
// thread-safe static-local initialization.
//
// THE BUG IN THE ORIGINAL JAVA DRAFT this was translated from: it
// checked `instance == nullptr` once, then entered a locked section
// and created the instance WITHOUT checking again inside the lock.
// Picture 5 threads all passing the first null-check before any of
// them acquires the lock -- they would then enter the locked section
// one at a time and EACH create a new ConfigurationManager, printing
// "Object created" 5 times. That defeats the entire point of a
// singleton.
//
// The fix is the second null-check *inside* the lock
// ("double-checked locking"): only the very first thread to get the
// lock actually creates the instance; every thread after it sees
// instance != nullptr and skips construction. `std::atomic` plays the
// role Java's `volatile` plays: it prevents a thread from observing a
// partially-constructed object due to instruction reordering.
// =====================================================================

#include <atomic>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class ConfigurationManager {
public:
    static ConfigurationManager* getInstance() {
        // First check (no lock): avoids paying the synchronization
        // cost on every call once the instance already exists.
        ConfigurationManager* tmp = instance_.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            // Second check (inside the lock): only the thread that wins
            // the race to acquire the lock actually creates the object.
            tmp = instance_.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                std::cout << "Object created by thread " << std::this_thread::get_id() << "\n";
                tmp = new ConfigurationManager();
                instance_.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

private:
    ConfigurationManager() = default;

    std::map<std::string, std::string> configuration_;
    static std::atomic<ConfigurationManager*> instance_;
    static std::mutex mutex_;
};

std::atomic<ConfigurationManager*> ConfigurationManager::instance_{nullptr};
std::mutex ConfigurationManager::mutex_;

// Thread-safe static-local initialization (C++11 guarantees this is
// initialized exactly once, even under concurrent first calls) -- the
// modern C++ equivalent of Java's eager/enum singleton: no manual
// locking required, the standard does it for you.
class ConfigurationManagerEager {
public:
    static ConfigurationManagerEager& getInstance() {
        static ConfigurationManagerEager instance; // created once, guaranteed by the standard
        return instance;
    }

private:
    ConfigurationManagerEager() = default;
    std::map<std::string, std::string> configuration_;
};

int main() {
    std::cout << "== Double-checked locking: 100 threads racing getInstance() ==\n";
    std::vector<std::thread> threads;
    for (int i = 0; i < 100; i++) {
        threads.emplace_back([] { ConfigurationManager::getInstance(); });
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "\"Object created\" should have printed exactly once above.\n";

    std::cout << "\n== Thread-safe static-local (eager-style) ==\n";
    ConfigurationManagerEager& eager1 = ConfigurationManagerEager::getInstance();
    ConfigurationManagerEager& eager2 = ConfigurationManagerEager::getInstance();
    std::cout << "Same instance? " << (&eager1 == &eager2 ? "true" : "false") << "\n";

    return 0;
}
