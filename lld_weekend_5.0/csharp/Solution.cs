// =====================================================================
// TOPIC: Method Overloading, Abstraction, Inheritance/Polymorphism,
//        Interfaces, and the Strategy Design Pattern.
// =====================================================================

using System;

namespace LldWeekend.Solution
{
    /// <summary>
    /// Demonstrates METHOD OVERLOADING: same method name, different
    /// parameter lists, resolved by the compiler at compile time.
    /// </summary>
    public class User
    {
        private string _name;
        private int _money;

        public void SetName(string suggestedName)
        {
            // Basic validation before accepting external input.
            if (string.IsNullOrWhiteSpace(suggestedName))
            {
                throw new ArgumentException("name cannot be empty");
            }
            _name = suggestedName;
        }

        public void AddMoney(int num1) => _money += num1;
        public void AddMoney(int num1, int num2) => _money += num1 * num2;
        public void AddMoney(int num1, int num2, int num3) => _money += num1 * num2 * num3;

        public int Money => _money;
    }

    // =====================================================================
    // ABSTRACTION via an interface: callers depend on "IStack", never on
    // the concrete data structure behind it.
    // =====================================================================

    public interface IStack
    {
        void Push(int value);
        int Pop();
        int GetSize();
    }

    public class ArrayStack : IStack
    {
        private readonly int[] _data;
        private int _size;

        public ArrayStack(int capacity)
        {
            _data = new int[capacity];
        }

        public void Push(int value)
        {
            if (_size == _data.Length) throw new InvalidOperationException("stack is full");
            _data[_size++] = value;
        }

        public int Pop()
        {
            if (_size == 0) throw new InvalidOperationException("stack is empty");
            return _data[--_size];
        }

        public int GetSize() => _size;
    }

    public class LinkedListStack : IStack
    {
        private class Node
        {
            public int Value;
            public Node Next;

            public Node(int value, Node next)
            {
                Value = value;
                Next = next;
            }
        }

        private Node _head;
        private int _size;

        public void Push(int value)
        {
            _head = new Node(value, _head);
            _size++;
        }

        public int Pop()
        {
            if (_head == null) throw new InvalidOperationException("stack is empty");
            int value = _head.Value;
            _head = _head.Next;
            _size--;
            return value;
        }

        public int GetSize() => _size;
    }

    // =====================================================================
    // TEMPLATE METHOD pattern: the abstract class fixes the *skeleton* of
    // an algorithm (TemplateLoading -> SendNotification) while subclasses
    // fill in only the varying step.
    // =====================================================================

    public abstract class Notification
    {
        protected string Message;
        protected readonly string UserId;

        protected Notification(string userId, string message)
        {
            UserId = userId;
            Message = message;
        }

        private void TemplateLoading()
        {
            Message = $"[Notification for {UserId}] {Message}";
        }

        protected abstract void SendNotification();

        public void Send()
        {
            TemplateLoading();
            SendNotification();
        }
    }

    public class SmsNotification : Notification
    {
        public SmsNotification(string userId, string message) : base(userId, message) { }

        protected override void SendNotification()
        {
            Console.WriteLine($"Sending SMS -> {Message}");
        }
    }

    public class EmailNotification : Notification
    {
        public EmailNotification(string userId, string message) : base(userId, message) { }

        protected override void SendNotification()
        {
            Console.WriteLine($"Sending Email -> {Message}");
        }
    }

    // =====================================================================
    // INHERITANCE + RUNTIME POLYMORPHISM:
    // - Compile-time polymorphism = method overloading (see User above).
    // - Runtime polymorphism      = virtual/override, resolved via the
    //   actual object type at run time even when accessed through a
    //   parent reference (see Speak() below).
    // =====================================================================

    public class Animal
    {
        public virtual void Speak() => Console.WriteLine("The animal makes a sound");
    }

    public class Dog : Animal
    {
        public override void Speak() => Console.WriteLine("The dog barks");
    }

    // =====================================================================
    // STRATEGY PATTERN
    // =====================================================================

    public interface ISortingAlgorithm
    {
        void Sort(int[] array);
    }

    public class MergeSort : ISortingAlgorithm
    {
        public void Sort(int[] array)
        {
            if (array.Length < 2) return;
            DoMergeSort(array, 0, array.Length - 1);
        }

        private void DoMergeSort(int[] array, int left, int right)
        {
            if (left >= right) return;
            int mid = left + (right - left) / 2;
            DoMergeSort(array, left, mid);
            DoMergeSort(array, mid + 1, right);
            Merge(array, left, mid, right);
        }

        private void Merge(int[] array, int left, int mid, int right)
        {
            int[] merged = new int[right - left + 1];
            int i = left, j = mid + 1, k = 0;
            while (i <= mid && j <= right)
            {
                merged[k++] = array[i] <= array[j] ? array[i++] : array[j++];
            }
            while (i <= mid) merged[k++] = array[i++];
            while (j <= right) merged[k++] = array[j++];
            Array.Copy(merged, 0, array, left, merged.Length);
        }
    }

    public class QuickSort : ISortingAlgorithm
    {
        public void Sort(int[] array)
        {
            DoQuickSort(array, 0, array.Length - 1);
        }

        private void DoQuickSort(int[] array, int low, int high)
        {
            if (low >= high) return;
            int pivot = array[high];
            int partitionIndex = low;
            for (int i = low; i < high; i++)
            {
                if (array[i] < pivot)
                {
                    (array[i], array[partitionIndex]) = (array[partitionIndex], array[i]);
                    partitionIndex++;
                }
            }
            (array[partitionIndex], array[high]) = (array[high], array[partitionIndex]);
            DoQuickSort(array, low, partitionIndex - 1);
            DoQuickSort(array, partitionIndex + 1, high);
        }
    }

    public class SelectionSort : ISortingAlgorithm
    {
        public void Sort(int[] array)
        {
            for (int i = 0; i < array.Length - 1; i++)
            {
                int minIndex = i;
                for (int j = i + 1; j < array.Length; j++)
                {
                    if (array[j] < array[minIndex]) minIndex = j;
                }
                (array[i], array[minIndex]) = (array[minIndex], array[i]);
            }
        }
    }

    public class SortingService
    {
        private ISortingAlgorithm _sortingAlgorithm;

        public void SetSortingAlgorithm(ISortingAlgorithm sortingAlgorithm) => _sortingAlgorithm = sortingAlgorithm;

        public void SortApi(int[] array)
        {
            if (_sortingAlgorithm == null) throw new InvalidOperationException("no sorting algorithm configured");
            _sortingAlgorithm.Sort(array);
        }
    }

    public static class SolutionProgram
    {
        public static void Main()
        {
            Console.WriteLine("== Method overloading ==");
            var pradeep = new User();
            pradeep.SetName("Pradeep");
            pradeep.AddMoney(123);
            pradeep.AddMoney(123, 2);
            pradeep.AddMoney(123, 2, 3);
            Console.WriteLine($"Total money: {pradeep.Money}");

            Console.WriteLine("\n== Abstraction via IStack interface ==");
            IStack arrayStack = new ArrayStack(5);
            arrayStack.Push(1);
            arrayStack.Push(2);
            Console.WriteLine($"ArrayStack size: {arrayStack.GetSize()}, pop -> {arrayStack.Pop()}");

            IStack linkedListStack = new LinkedListStack();
            linkedListStack.Push(10);
            linkedListStack.Push(20);
            Console.WriteLine($"LinkedListStack size: {linkedListStack.GetSize()}, pop -> {linkedListStack.Pop()}");

            Console.WriteLine("\n== Template method (Notification) ==");
            new SmsNotification("u1", "Your OTP is 4242").Send();
            new EmailNotification("u1", "Your invoice is ready").Send();

            Console.WriteLine("\n== Inheritance + runtime polymorphism ==");
            Animal animal = new Dog(); // declared type Animal, actual type Dog
            animal.Speak();            // prints Dog's version -> resolved at runtime

            Console.WriteLine("\n== Strategy pattern (sorting) ==");
            var service = new SortingService();
            int[] array = { 3, 4, 1, 2 };
            service.SetSortingAlgorithm(new MergeSort());
            service.SortApi(array);
            Console.WriteLine($"After merge sort: [{string.Join(", ", array)}]");

            int[] array2 = { 5, 1, 4, 2, 8 };
            service.SetSortingAlgorithm(new QuickSort());
            service.SortApi(array2);
            Console.WriteLine($"After quick sort: [{string.Join(", ", array2)}]");

            int[] array3 = { 9, 3, 7, 1 };
            service.SetSortingAlgorithm(new SelectionSort());
            service.SortApi(array3);
            Console.WriteLine($"After selection sort: [{string.Join(", ", array3)}]");
        }
    }
}
