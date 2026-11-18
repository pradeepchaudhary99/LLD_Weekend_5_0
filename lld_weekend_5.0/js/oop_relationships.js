// =====================================================================
// TOPIC: The five class relationships every LLD interview expects you
// to name correctly: Association, Aggregation, Composition,
// Inheritance, and Dependency.
// =====================================================================

"use strict";

// =========================================
// 1. ASSOCIATION (Knows-A)
// Student is associated with Teacher.
// Neither owns the other; both can exist independently.
// =========================================

class Teacher {
  constructor(name) {
    this.name = name;
  }

  teach() {
    console.log(`${this.name} is teaching`);
  }
}

class Student {
  constructor(name, teacher) {
    this.name = name;
    this.teacher = teacher;
  }

  attendClass() {
    this.teacher.teach();
  }
}

// =========================================
// 2. AGGREGATION (Weak HAS-A)
// Department contains Teachers, but Teachers can exist
// without the Department (e.g. shared across departments).
// =========================================

class Department {
  constructor(name, teachers) {
    this.name = name;
    this.teachers = [...teachers];
  }

  addTeacher(teacher) {
    this.teachers.push(teacher);
  }

  showTeachers() {
    for (const t of this.teachers) {
      console.log(t.name);
    }
  }
}

// =========================================
// 3. COMPOSITION (Strong HAS-A)
// House owns its Rooms. Rooms have no meaning or lifecycle
// outside the House that created them -- destroy the House
// and the Rooms go with it.
// =========================================

class Room {
  constructor(name) {
    this.name = name;
  }
}

class House {
  #rooms = [new Room("Bedroom"), new Room("Kitchen")]; // private: owned exclusively by House

  showRooms() {
    for (const room of this.#rooms) {
      console.log(room.name);
    }
  }
}

// =========================================
// 4. INHERITANCE (IS-A)
// Dog IS-A Animal: it reuses Animal's behavior and can be
// substituted anywhere an Animal is expected.
// =========================================

class Animal {
  eat() {
    console.log("Animal is eating");
  }
}

class Dog extends Animal {
  bark() {
    console.log("Dog is barking");
  }
}

// =========================================
// 5. DEPENDENCY (USES-A)
// OrderService temporarily uses a PaymentService that is
// passed in -- it doesn't own or hold a reference to it
// beyond the single method call.
// =========================================

class PaymentService {
  pay() {
    console.log("Payment successful");
  }
}

class OrderService {
  // No field here on purpose: OrderService does NOT hold a
  // PaymentService -- it only "uses" one for the duration of
  // a single checkout() call. That's what makes this a
  // dependency rather than association/aggregation/composition.

  checkout(paymentService) {
    paymentService.pay();
  }
}

function main() {
  console.log("1. ASSOCIATION");
  const teacher = new Teacher("Rahul");
  const student = new Student("Amit", teacher);
  student.attendClass();

  console.log("\n2. AGGREGATION");
  const t1 = new Teacher("Rahul");
  const t2 = new Teacher("Priya");

  const dept = new Department("Computer Science", [t1, t2]);
  dept.showTeachers();

  // Teachers still exist independently of the department.
  console.log(`${t1.name} exists independently`);

  console.log("\n3. COMPOSITION");
  const house = new House();
  house.showRooms();

  console.log("\n4. INHERITANCE");
  const dog = new Dog();
  dog.eat(); // Inherited from Animal
  dog.bark();

  const animal = new Dog(); // IS-A relationship
  animal.eat();

  console.log("\n5. DEPENDENCY");
  const orderService = new OrderService();
  const paymentService = new PaymentService();

  orderService.checkout(paymentService);
}

main();
