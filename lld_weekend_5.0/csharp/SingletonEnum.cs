// =====================================================================
// TOPIC: Singleton pattern -- the Lazy<T> approach, C#'s closest
// equivalent to Java's enum-based singleton.
//
// C# enums are just named integers and cannot hold instance state the
// way Java enums can, so there's no direct equivalent of Java's
// enum-constant trick. The closest idiom that gives you the same
// "constructed exactly once, thread-safe, no manual locking" property
// is System.Lazy<T>: the BCL guarantees (with the default
// LazyThreadSafetyMode.ExecutionAndPublication) that only one thread
// ever runs the factory delegate, even under concurrent first access.
// =====================================================================

using System;

namespace LldWeekend.SingletonEnum
{
    public sealed class DatabaseConnection
    {
        // The factory delegate runs at most once, no matter how many
        // threads call Instance.Value concurrently for the first time.
        private static readonly Lazy<DatabaseConnection> LazyInstance =
            new(() => new DatabaseConnection());

        public static DatabaseConnection Instance => LazyInstance.Value;

        private DatabaseConnection() { }

        public void Connect() => Console.WriteLine("Connected to DB");
    }

    public static class SingletonEnumProgram
    {
        public static void Main()
        {
            DatabaseConnection db1 = DatabaseConnection.Instance;
            DatabaseConnection db2 = DatabaseConnection.Instance;
            db1.Connect();
            Console.WriteLine($"Same instance? {db1 == db2}");
        }
    }
}

// LazyInstance.Value is the only DatabaseConnection we ever construct.
// Lazy<T>'s default thread-safety mode guarantees it is created
// exactly once, even under concurrent first access.
