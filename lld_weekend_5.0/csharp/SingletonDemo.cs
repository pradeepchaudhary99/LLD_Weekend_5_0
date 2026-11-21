// =====================================================================
// TOPIC: Singleton pattern -- lazy initialization with DOUBLE-CHECKED
// LOCKING, contrasted with eager (static-field) initialization.
//
// THE BUG IN THE ORIGINAL JAVA DRAFT this was translated from: it
// checked `instance == null` once, then entered a `lock` and created
// the instance WITHOUT checking again inside the lock. Picture 5
// threads all passing the first null-check before any of them acquires
// the lock -- they would then enter the lock one at a time and EACH
// create a new ConfigurationManager, printing "Object created" 5
// times. That defeats the entire point of a singleton.
//
// The fix is the second null-check *inside* the lock
// ("double-checked locking"): only the very first thread to get the
// lock actually creates the instance; every thread after it sees
// Instance != null and skips construction. `volatile` plays the same
// role it does in Java: it prevents a thread from observing a
// partially-constructed object due to instruction reordering.
// =====================================================================

using System;
using System.Collections.Generic;
using System.Threading;

namespace LldWeekend.SingletonDemo
{
    public sealed class ConfigurationManager
    {
        private static volatile ConfigurationManager _instance;
        private static readonly object SyncRoot = new();

        private readonly Dictionary<string, string> _configuration = new();

        private ConfigurationManager() { }

        public static ConfigurationManager GetInstance()
        {
            // First check (no lock): avoids paying the synchronization
            // cost on every call once the instance already exists.
            if (_instance == null)
            {
                lock (SyncRoot)
                {
                    // Second check (inside the lock): only the thread that
                    // wins the race to acquire the lock actually creates
                    // the object.
                    if (_instance == null)
                    {
                        Console.WriteLine($"Object created by thread {Thread.CurrentThread.ManagedThreadId}");
                        _instance = new ConfigurationManager();
                    }
                }
            }
            return _instance;
        }
    }

    // Eager initialization: the CLR guarantees a static field is
    // initialized before it's first accessed, and does so in a
    // thread-safe way -- no explicit locking needed, at the cost of
    // creating the object even if it's never used.
    public sealed class ConfigurationManagerEager
    {
        private static readonly ConfigurationManagerEager Instance = new();

        private readonly Dictionary<string, string> _configuration = new();

        private ConfigurationManagerEager() { }

        public static ConfigurationManagerEager GetInstance() => Instance;
    }

    public static class SingletonDemoProgram
    {
        public static void Main()
        {
            Console.WriteLine("== Double-checked locking: 100 threads racing GetInstance() ==");
            var threads = new Thread[100];
            for (int i = 0; i < 100; i++)
            {
                threads[i] = new Thread(() => ConfigurationManager.GetInstance());
                threads[i].Start();
            }
            foreach (var t in threads) t.Join();
            Console.WriteLine("\"Object created\" should have printed exactly once above.");

            Console.WriteLine("\n== Eager initialization ==");
            var eager1 = ConfigurationManagerEager.GetInstance();
            var eager2 = ConfigurationManagerEager.GetInstance();
            Console.WriteLine($"Same instance? {eager1 == eager2}");
        }
    }
}
