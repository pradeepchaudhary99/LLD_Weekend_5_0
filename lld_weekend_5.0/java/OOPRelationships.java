import java.util.*;

// =====================================================================
// TOPIC: The five class relationships every LLD interview expects you
// to name correctly: Association, Aggregation, Composition,
// Inheritance, and Dependency.
// =====================================================================

public class OOPRelationships {

    // =========================================
    // 1. ASSOCIATION (Knows-A)
    // Student is associated with Teacher.
    // Neither owns the other; both can exist independently.
    // =========================================

    static class Teacher {
        String name;

        Teacher(String name) {
            this.name = name;
        }

        void teach() {
            System.out.println(name + " is teaching");
        }
    }

    static class Student {
        String name;
        Teacher teacher;

        Student(String name, Teacher teacher) {
            this.name = name;
            this.teacher = teacher;
        }

        void attendClass() {
            teacher.teach();
        }
    }

    // =========================================
    // 2. AGGREGATION (Weak HAS-A)
    // Department contains Teachers, but Teachers can exist
    // without the Department (e.g. shared across departments).
    // =========================================

    static class Department {
        String name;
        List<Teacher> teachers;

        Department(String name, List<Teacher> teachers) {
            this.name = name;
            this.teachers = new ArrayList<>(teachers);
        }

        void addTeacher(Teacher t) {
            teachers.add(t);
        }

        void showTeachers() {
            for (Teacher t : teachers) {
                System.out.println(t.name);
            }
        }
    }

    // =========================================
    // 3. COMPOSITION (Strong HAS-A)
    // House owns its Rooms. Rooms have no meaning or lifecycle
    // outside the House that created them -- destroy the House
    // and the Rooms go with it.
    // =========================================

    static class House {
        private final List<Room> rooms = new ArrayList<>();

        // Room is private to House: House controls creation and access.
        private static class Room {
            String name;

            Room(String name) {
                this.name = name;
            }
        }

        House() {
            rooms.add(new Room("Bedroom"));
            rooms.add(new Room("Kitchen"));
        }

        void showRooms() {
            for (Room room : rooms) {
                System.out.println(room.name);
            }
        }
    }

    // =========================================
    // 4. INHERITANCE (IS-A)
    // Dog IS-A Animal: it reuses Animal's behavior and can be
    // substituted anywhere an Animal is expected.
    // =========================================

    static class Animal {
        void eat() {
            System.out.println("Animal is eating");
        }
    }

    static class Dog extends Animal {
        void bark() {
            System.out.println("Dog is barking");
        }
    }

    // =========================================
    // 5. DEPENDENCY (USES-A)
    // OrderService temporarily uses a PaymentService that is
    // passed in -- it doesn't own or hold a reference to it
    // beyond the single method call.
    // =========================================

    static class PaymentService {
        void pay() {
            System.out.println("Payment successful");
        }
    }

    static class OrderService {
        // No field here on purpose: OrderService does NOT hold a
        // PaymentService -- it only "uses" one for the duration of
        // a single checkout() call. That's what makes this a
        // dependency rather than association/aggregation/composition.

        void checkout(PaymentService paymentService) {
            paymentService.pay();
        }
    }

    // =========================================
    // MAIN METHOD
    // =========================================

    public static void main(String[] args) {

        System.out.println("1. ASSOCIATION");
        Teacher teacher = new Teacher("Rahul");
        Student student = new Student("Amit", teacher);
        student.attendClass();

        System.out.println("\n2. AGGREGATION");
        Teacher t1 = new Teacher("Rahul");
        Teacher t2 = new Teacher("Priya");

        Department dept = new Department(
            "Computer Science",
            Arrays.asList(t1, t2)
        );
        dept.showTeachers();

        // Teachers still exist independently of the department.
        System.out.println(t1.name + " exists independently");

        System.out.println("\n3. COMPOSITION");
        House house = new House();
        house.showRooms();

        System.out.println("\n4. INHERITANCE");
        Dog dog = new Dog();
        dog.eat();  // Inherited from Animal
        dog.bark();

        Animal animal = new Dog(); // IS-A relationship
        animal.eat();

        System.out.println("\n5. DEPENDENCY");
        OrderService orderService = new OrderService();
        PaymentService paymentService = new PaymentService();

        orderService.checkout(paymentService);
    }
}
