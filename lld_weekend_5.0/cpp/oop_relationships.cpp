// =====================================================================
// TOPIC: The five class relationships every LLD interview expects you
// to name correctly: Association, Aggregation, Composition,
// Inheritance, and Dependency.
// =====================================================================

#include <iostream>
#include <string>
#include <vector>

// =========================================
// 1. ASSOCIATION (Knows-A)
// Student is associated with Teacher.
// Neither owns the other; both can exist independently.
// =========================================

class Teacher {
public:
    std::string name;

    explicit Teacher(std::string name) : name(std::move(name)) {}

    void teach() const {
        std::cout << name << " is teaching\n";
    }
};

class Student {
public:
    std::string name;
    Teacher& teacher; // a reference: Student knows about a Teacher it does not own

    Student(std::string name, Teacher& teacher)
        : name(std::move(name)), teacher(teacher) {}

    void attendClass() const {
        teacher.teach();
    }
};

// =========================================
// 2. AGGREGATION (Weak HAS-A)
// Department contains Teachers, but Teachers can exist
// without the Department (e.g. shared across departments).
// Storing pointers (not owning objects) is what makes this aggregation.
// =========================================

class Department {
public:
    std::string name;
    std::vector<Teacher*> teachers; // non-owning pointers

    Department(std::string name, std::vector<Teacher*> teachers)
        : name(std::move(name)), teachers(std::move(teachers)) {}

    void addTeacher(Teacher* t) {
        teachers.push_back(t);
    }

    void showTeachers() const {
        for (const Teacher* t : teachers) {
            std::cout << t->name << "\n";
        }
    }
};

// =========================================
// 3. COMPOSITION (Strong HAS-A)
// House owns its Rooms by value. Rooms have no meaning or
// lifecycle outside the House that created them -- destroy the
// House and the Rooms go with it automatically.
// =========================================

class House {
    struct Room {
        std::string name;
    };

    std::vector<Room> rooms_; // owned by value -- destroyed with the House

public:
    House() {
        rooms_.push_back({"Bedroom"});
        rooms_.push_back({"Kitchen"});
    }

    void showRooms() const {
        for (const auto& room : rooms_) {
            std::cout << room.name << "\n";
        }
    }
};

// =========================================
// 4. INHERITANCE (IS-A)
// Dog IS-A Animal: it reuses Animal's behavior and can be
// substituted anywhere an Animal is expected.
// =========================================

class Animal {
public:
    virtual ~Animal() = default;
    void eat() const { std::cout << "Animal is eating\n"; }
};

class Dog : public Animal {
public:
    void bark() const { std::cout << "Dog is barking\n"; }
};

// =========================================
// 5. DEPENDENCY (USES-A)
// OrderService temporarily uses a PaymentService that is
// passed in -- it doesn't own or hold a reference to it
// beyond the single method call.
// =========================================

class PaymentService {
public:
    void pay() const { std::cout << "Payment successful\n"; }
};

class OrderService {
public:
    // No member here on purpose: OrderService does NOT hold a
    // PaymentService -- it only "uses" one for the duration of
    // a single checkout() call. That's what makes this a
    // dependency rather than association/aggregation/composition.

    void checkout(const PaymentService& paymentService) const {
        paymentService.pay();
    }
};

int main() {
    std::cout << "1. ASSOCIATION\n";
    Teacher teacher("Rahul");
    Student student("Amit", teacher);
    student.attendClass();

    std::cout << "\n2. AGGREGATION\n";
    Teacher t1("Rahul");
    Teacher t2("Priya");

    Department dept("Computer Science", {&t1, &t2});
    dept.showTeachers();

    // Teachers still exist independently of the department.
    std::cout << t1.name << " exists independently\n";

    std::cout << "\n3. COMPOSITION\n";
    House house;
    house.showRooms();

    std::cout << "\n4. INHERITANCE\n";
    Dog dog;
    dog.eat();  // Inherited from Animal
    dog.bark();

    Animal& animal = dog; // IS-A relationship
    animal.eat();

    std::cout << "\n5. DEPENDENCY\n";
    OrderService orderService;
    PaymentService paymentService;

    orderService.checkout(paymentService);

    return 0;
}
