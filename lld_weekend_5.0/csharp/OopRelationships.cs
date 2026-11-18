// =====================================================================
// TOPIC: The five class relationships every LLD interview expects you
// to name correctly: Association, Aggregation, Composition,
// Inheritance, and Dependency.
// =====================================================================

using System;
using System.Collections.Generic;

namespace LldWeekend.OopRelationships
{
    // =========================================
    // 1. ASSOCIATION (Knows-A)
    // Student is associated with Teacher.
    // Neither owns the other; both can exist independently.
    // =========================================

    public class Teacher
    {
        public string Name { get; }

        public Teacher(string name) => Name = name;

        public void Teach() => Console.WriteLine($"{Name} is teaching");
    }

    public class Student
    {
        public string Name { get; }
        private readonly Teacher _teacher;

        public Student(string name, Teacher teacher)
        {
            Name = name;
            _teacher = teacher;
        }

        public void AttendClass() => _teacher.Teach();
    }

    // =========================================
    // 2. AGGREGATION (Weak HAS-A)
    // Department contains Teachers, but Teachers can exist
    // without the Department (e.g. shared across departments).
    // =========================================

    public class Department
    {
        public string Name { get; }
        public List<Teacher> Teachers { get; }

        public Department(string name, List<Teacher> teachers)
        {
            Name = name;
            Teachers = new List<Teacher>(teachers);
        }

        public void AddTeacher(Teacher t) => Teachers.Add(t);

        public void ShowTeachers()
        {
            foreach (var t in Teachers) Console.WriteLine(t.Name);
        }
    }

    // =========================================
    // 3. COMPOSITION (Strong HAS-A)
    // House owns its Rooms. Rooms have no meaning or lifecycle
    // outside the House that created them -- destroy the House
    // and the Rooms go with it.
    // =========================================

    public class House
    {
        // Room is private to House: House controls creation and access.
        private class Room
        {
            public string Name { get; }
            public Room(string name) => Name = name;
        }

        private readonly List<Room> _rooms = new();

        public House()
        {
            _rooms.Add(new Room("Bedroom"));
            _rooms.Add(new Room("Kitchen"));
        }

        public void ShowRooms()
        {
            foreach (var room in _rooms) Console.WriteLine(room.Name);
        }
    }

    // =========================================
    // 4. INHERITANCE (IS-A)
    // Dog IS-A Animal: it reuses Animal's behavior and can be
    // substituted anywhere an Animal is expected.
    // =========================================

    public class Animal
    {
        public void Eat() => Console.WriteLine("Animal is eating");
    }

    public class Dog : Animal
    {
        public void Bark() => Console.WriteLine("Dog is barking");
    }

    // =========================================
    // 5. DEPENDENCY (USES-A)
    // OrderService temporarily uses a PaymentService that is
    // passed in -- it doesn't own or hold a reference to it
    // beyond the single method call.
    // =========================================

    public class PaymentService
    {
        public void Pay() => Console.WriteLine("Payment successful");
    }

    public class OrderService
    {
        // No field here on purpose: OrderService does NOT hold a
        // PaymentService -- it only "uses" one for the duration of
        // a single Checkout() call. That's what makes this a
        // dependency rather than association/aggregation/composition.

        public void Checkout(PaymentService paymentService) => paymentService.Pay();
    }

    public static class OopRelationshipsProgram
    {
        public static void Main()
        {
            Console.WriteLine("1. ASSOCIATION");
            var teacher = new Teacher("Rahul");
            var student = new Student("Amit", teacher);
            student.AttendClass();

            Console.WriteLine("\n2. AGGREGATION");
            var t1 = new Teacher("Rahul");
            var t2 = new Teacher("Priya");

            var dept = new Department("Computer Science", new List<Teacher> { t1, t2 });
            dept.ShowTeachers();

            // Teachers still exist independently of the department.
            Console.WriteLine($"{t1.Name} exists independently");

            Console.WriteLine("\n3. COMPOSITION");
            var house = new House();
            house.ShowRooms();

            Console.WriteLine("\n4. INHERITANCE");
            var dog = new Dog();
            dog.Eat(); // Inherited from Animal
            dog.Bark();

            Animal animal = new Dog(); // IS-A relationship
            animal.Eat();

            Console.WriteLine("\n5. DEPENDENCY");
            var orderService = new OrderService();
            var paymentService = new PaymentService();

            orderService.Checkout(paymentService);
        }
    }
}
