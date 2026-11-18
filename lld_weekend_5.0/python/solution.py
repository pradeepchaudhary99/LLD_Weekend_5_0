"""
TOPIC: Method Overloading, Abstraction, Inheritance/Polymorphism,
       Interfaces (via ABC), and the Strategy Design Pattern.

NOTE ON OVERLOADING: Python has no native method overloading -- a later
`def add_money(...)` would simply replace an earlier one with the same
name. We emulate Java's three `addMoney` overloads with default/optional
arguments on a single method, which is the idiomatic Python way to get
the same flexibility.
"""

from abc import ABC, abstractmethod
from typing import List, Optional


class User:
    def __init__(self) -> None:
        self._name: Optional[str] = None
        self._money: int = 0

    def set_name(self, suggested_name: str) -> None:
        # Basic validation before accepting external input.
        if not suggested_name or not suggested_name.strip():
            raise ValueError("name cannot be empty")
        self._name = suggested_name

    def add_money(self, num1: int, num2: int = 1, num3: int = 1) -> None:
        # Mirrors Java's addMoney(a) / addMoney(a, b) / addMoney(a, b, c)
        # by giving the extra multipliers a neutral default of 1.
        self._money += num1 * num2 * num3

    @property
    def money(self) -> int:
        return self._money


# =====================================================================
# ABSTRACTION via an abstract base class: callers depend on "Stack",
# never on the concrete data structure behind it.
# =====================================================================

class Stack(ABC):
    @abstractmethod
    def push(self, value: int) -> None: ...

    @abstractmethod
    def pop(self) -> int: ...

    @abstractmethod
    def get_size(self) -> int: ...


class ArrayStack(Stack):
    """List-backed stack (Python lists are dynamic arrays)."""

    def __init__(self) -> None:
        self._data: List[int] = []

    def push(self, value: int) -> None:
        self._data.append(value)

    def pop(self) -> int:
        if not self._data:
            raise IndexError("stack is empty")
        return self._data.pop()

    def get_size(self) -> int:
        return len(self._data)


class _Node:
    __slots__ = ("value", "next")

    def __init__(self, value: int, next_node: Optional["_Node"]) -> None:
        self.value = value
        self.next = next_node


class LinkedListStack(Stack):
    """Linked-list-backed stack: same contract, different representation."""

    def __init__(self) -> None:
        self._head: Optional[_Node] = None
        self._size = 0

    def push(self, value: int) -> None:
        self._head = _Node(value, self._head)
        self._size += 1

    def pop(self) -> int:
        if self._head is None:
            raise IndexError("stack is empty")
        value = self._head.value
        self._head = self._head.next
        self._size -= 1
        return value

    def get_size(self) -> int:
        return self._size


# =====================================================================
# TEMPLATE METHOD pattern: the abstract class fixes the *skeleton* of an
# algorithm (template_loading -> send_notification) while subclasses
# fill in only the varying step.
# =====================================================================

class Notification(ABC):
    def __init__(self, user_id: str, message: str) -> None:
        self.user_id = user_id
        self.message = message

    def template_loading(self) -> None:
        self.message = f"[Notification for {self.user_id}] {self.message}"

    @abstractmethod
    def send_notification(self) -> None: ...

    def send(self) -> None:
        self.template_loading()
        self.send_notification()


class SMSNotification(Notification):
    def send_notification(self) -> None:
        print(f"Sending SMS -> {self.message}")


class EmailNotification(Notification):
    def send_notification(self) -> None:
        print(f"Sending Email -> {self.message}")


# =====================================================================
# INHERITANCE + RUNTIME POLYMORPHISM:
# Python resolves overridden methods dynamically at call time -- there
# is no separate "compile-time" step, so every override here behaves
# like Java's runtime polymorphism.
# =====================================================================

class Animal:
    def speak(self) -> None:
        print("The animal makes a sound")


class Dog(Animal):
    def speak(self) -> None:
        print("The dog barks")


# =====================================================================
# STRATEGY PATTERN: SortingService is configured with a SortingAlgorithm
# and delegates to it. New algorithms can be added without changing
# SortingService at all (Open/Closed Principle).
# =====================================================================

class SortingAlgorithm(ABC):
    @abstractmethod
    def sort(self, array: List[int]) -> None: ...


class MergeSort(SortingAlgorithm):
    def sort(self, array: List[int]) -> None:
        if len(array) < 2:
            return
        self._merge_sort(array, 0, len(array) - 1)

    def _merge_sort(self, array: List[int], left: int, right: int) -> None:
        if left >= right:
            return
        mid = left + (right - left) // 2
        self._merge_sort(array, left, mid)
        self._merge_sort(array, mid + 1, right)
        self._merge(array, left, mid, right)

    def _merge(self, array: List[int], left: int, mid: int, right: int) -> None:
        merged = []
        i, j = left, mid + 1
        while i <= mid and j <= right:
            if array[i] <= array[j]:
                merged.append(array[i])
                i += 1
            else:
                merged.append(array[j])
                j += 1
        merged.extend(array[i:mid + 1])
        merged.extend(array[j:right + 1])
        array[left:right + 1] = merged


class QuickSort(SortingAlgorithm):
    def sort(self, array: List[int]) -> None:
        self._quick_sort(array, 0, len(array) - 1)

    def _quick_sort(self, array: List[int], low: int, high: int) -> None:
        if low >= high:
            return
        pivot = array[high]
        partition_index = low
        for i in range(low, high):
            if array[i] < pivot:
                array[i], array[partition_index] = array[partition_index], array[i]
                partition_index += 1
        array[partition_index], array[high] = array[high], array[partition_index]
        self._quick_sort(array, low, partition_index - 1)
        self._quick_sort(array, partition_index + 1, high)


class SelectionSort(SortingAlgorithm):
    def sort(self, array: List[int]) -> None:
        n = len(array)
        for i in range(n - 1):
            min_index = i
            for j in range(i + 1, n):
                if array[j] < array[min_index]:
                    min_index = j
            array[i], array[min_index] = array[min_index], array[i]


class SortingService:
    def __init__(self) -> None:
        self._sorting_algorithm: Optional[SortingAlgorithm] = None

    def set_sorting_algorithm(self, sorting_algorithm: SortingAlgorithm) -> None:
        self._sorting_algorithm = sorting_algorithm

    def sort_api(self, array: List[int]) -> None:
        if self._sorting_algorithm is None:
            raise RuntimeError("no sorting algorithm configured")
        self._sorting_algorithm.sort(array)


def main() -> None:
    print("== Method overloading (emulated with default args) ==")
    pradeep = User()
    pradeep.set_name("Pradeep")
    pradeep.add_money(123)
    pradeep.add_money(123, 2)
    pradeep.add_money(123, 2, 3)
    print(f"Total money: {pradeep.money}")

    print("\n== Abstraction via Stack interface ==")
    array_stack: Stack = ArrayStack()
    array_stack.push(1)
    array_stack.push(2)
    print(f"ArrayStack size: {array_stack.get_size()}, pop -> {array_stack.pop()}")

    linked_list_stack: Stack = LinkedListStack()
    linked_list_stack.push(10)
    linked_list_stack.push(20)
    print(f"LinkedListStack size: {linked_list_stack.get_size()}, pop -> {linked_list_stack.pop()}")

    print("\n== Template method (Notification) ==")
    SMSNotification("u1", "Your OTP is 4242").send()
    EmailNotification("u1", "Your invoice is ready").send()

    print("\n== Inheritance + runtime polymorphism ==")
    animal: Animal = Dog()  # declared type Animal, actual type Dog
    animal.speak()          # prints Dog's version -> resolved at runtime

    print("\n== Strategy pattern (sorting) ==")
    service = SortingService()
    array = [3, 4, 1, 2]
    service.set_sorting_algorithm(MergeSort())
    service.sort_api(array)
    print(f"After merge sort: {array}")

    array2 = [5, 1, 4, 2, 8]
    service.set_sorting_algorithm(QuickSort())
    service.sort_api(array2)
    print(f"After quick sort: {array2}")

    array3 = [9, 3, 7, 1]
    service.set_sorting_algorithm(SelectionSort())
    service.sort_api(array3)
    print(f"After selection sort: {array3}")


if __name__ == "__main__":
    main()
