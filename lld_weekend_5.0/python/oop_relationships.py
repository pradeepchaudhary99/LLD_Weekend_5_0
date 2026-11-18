"""
TOPIC: The five class relationships every LLD interview expects you
to name correctly: Association, Aggregation, Composition,
Inheritance, and Dependency.
"""

from typing import List


# =========================================
# 1. ASSOCIATION (Knows-A)
# Student is associated with Teacher.
# Neither owns the other; both can exist independently.
# =========================================

class Teacher:
    def __init__(self, name: str) -> None:
        self.name = name

    def teach(self) -> None:
        print(f"{self.name} is teaching")


class Student:
    def __init__(self, name: str, teacher: Teacher) -> None:
        self.name = name
        self.teacher = teacher

    def attend_class(self) -> None:
        self.teacher.teach()


# =========================================
# 2. AGGREGATION (Weak HAS-A)
# Department contains Teachers, but Teachers can exist
# without the Department (e.g. shared across departments).
# =========================================

class Department:
    def __init__(self, name: str, teachers: List[Teacher]) -> None:
        self.name = name
        self.teachers: List[Teacher] = list(teachers)

    def add_teacher(self, teacher: Teacher) -> None:
        self.teachers.append(teacher)

    def show_teachers(self) -> None:
        for t in self.teachers:
            print(t.name)


# =========================================
# 3. COMPOSITION (Strong HAS-A)
# House owns its Rooms. Rooms have no meaning or lifecycle
# outside the House that created them -- destroy the House
# and the Rooms go with it.
# =========================================

class _Room:
    """Leading underscore signals "private to this module" -- the closest
    Python equivalent of Java's private static inner class."""

    def __init__(self, name: str) -> None:
        self.name = name


class House:
    def __init__(self) -> None:
        self._rooms: List[_Room] = [_Room("Bedroom"), _Room("Kitchen")]

    def show_rooms(self) -> None:
        for room in self._rooms:
            print(room.name)


# =========================================
# 4. INHERITANCE (IS-A)
# Dog IS-A Animal: it reuses Animal's behavior and can be
# substituted anywhere an Animal is expected.
# =========================================

class Animal:
    def eat(self) -> None:
        print("Animal is eating")


class Dog(Animal):
    def bark(self) -> None:
        print("Dog is barking")


# =========================================
# 5. DEPENDENCY (USES-A)
# OrderService temporarily uses a PaymentService that is
# passed in -- it doesn't own or hold a reference to it
# beyond the single method call.
# =========================================

class PaymentService:
    def pay(self) -> None:
        print("Payment successful")


class OrderService:
    # No field here on purpose: OrderService does NOT hold a
    # PaymentService -- it only "uses" one for the duration of
    # a single checkout() call. That's what makes this a
    # dependency rather than association/aggregation/composition.

    def checkout(self, payment_service: PaymentService) -> None:
        payment_service.pay()


def main() -> None:
    print("1. ASSOCIATION")
    teacher = Teacher("Rahul")
    student = Student("Amit", teacher)
    student.attend_class()

    print("\n2. AGGREGATION")
    t1 = Teacher("Rahul")
    t2 = Teacher("Priya")

    dept = Department("Computer Science", [t1, t2])
    dept.show_teachers()

    # Teachers still exist independently of the department.
    print(f"{t1.name} exists independently")

    print("\n3. COMPOSITION")
    house = House()
    house.show_rooms()

    print("\n4. INHERITANCE")
    dog = Dog()
    dog.eat()   # Inherited from Animal
    dog.bark()

    animal: Animal = Dog()  # IS-A relationship
    animal.eat()

    print("\n5. DEPENDENCY")
    order_service = OrderService()
    payment_service = PaymentService()

    order_service.checkout(payment_service)


if __name__ == "__main__":
    main()
