// =====================================================================
// TOPIC: Method Overloading, Abstraction, Inheritance/Polymorphism,
//        Interfaces (duck-typed / enforced by convention), and the
//        Strategy Design Pattern.
//
// NOTE ON OVERLOADING: JavaScript has no native method overloading --
// a later `addMoney(a, b)` would just replace an earlier `addMoney(a)`
// (functions are values, and there's only one slot per name). We
// emulate Java's three overloads with default parameters on one
// method, which is the idiomatic JS way to get the same flexibility.
// =====================================================================

"use strict";

class User {
  #name;
  #money = 0;

  setName(suggestedName) {
    // Basic validation before accepting external input.
    if (!suggestedName || !suggestedName.trim()) {
      throw new Error("name cannot be empty");
    }
    this.#name = suggestedName;
  }

  // Mirrors Java's addMoney(a) / addMoney(a, b) / addMoney(a, b, c)
  // by giving the extra multipliers a neutral default of 1.
  addMoney(num1, num2 = 1, num3 = 1) {
    this.#money += num1 * num2 * num3;
  }

  get money() {
    return this.#money;
  }
}

// =====================================================================
// ABSTRACTION via a "interface" base class that throws if a subclass
// forgets to override a method -- JS has no compiler to enforce this,
// so we enforce it at runtime instead.
// =====================================================================

class Stack {
  push(_value) {
    throw new Error("push() must be implemented");
  }
  pop() {
    throw new Error("pop() must be implemented");
  }
  getSize() {
    throw new Error("getSize() must be implemented");
  }
}

class ArrayStack extends Stack {
  #data = [];

  push(value) {
    this.#data.push(value);
  }

  pop() {
    if (this.#data.length === 0) {
      throw new Error("stack is empty");
    }
    return this.#data.pop();
  }

  getSize() {
    return this.#data.length;
  }
}

class _Node {
  constructor(value, next) {
    this.value = value;
    this.next = next;
  }
}

class LinkedListStack extends Stack {
  #head = null;
  #size = 0;

  push(value) {
    this.#head = new _Node(value, this.#head);
    this.#size++;
  }

  pop() {
    if (this.#head === null) {
      throw new Error("stack is empty");
    }
    const value = this.#head.value;
    this.#head = this.#head.next;
    this.#size--;
    return value;
  }

  getSize() {
    return this.#size;
  }
}

// =====================================================================
// TEMPLATE METHOD pattern: the base class fixes the *skeleton* of an
// algorithm (templateLoading -> sendNotification) while subclasses
// fill in only the varying step.
// =====================================================================

class Notification {
  constructor(userId, message) {
    this.userId = userId;
    this.message = message;
  }

  templateLoading() {
    this.message = `[Notification for ${this.userId}] ${this.message}`;
  }

  sendNotification() {
    throw new Error("sendNotification() must be implemented");
  }

  send() {
    this.templateLoading();
    this.sendNotification();
  }
}

class SMSNotification extends Notification {
  sendNotification() {
    console.log(`Sending SMS -> ${this.message}`);
  }
}

class EmailNotification extends Notification {
  sendNotification() {
    console.log(`Sending Email -> ${this.message}`);
  }
}

// =====================================================================
// INHERITANCE + RUNTIME POLYMORPHISM: JS always dispatches methods
// dynamically based on the actual object (via the prototype chain), so
// every override here behaves like Java's runtime polymorphism.
// =====================================================================

class Animal {
  speak() {
    console.log("The animal makes a sound");
  }
}

class Dog extends Animal {
  speak() {
    console.log("The dog barks");
  }
}

// =====================================================================
// STRATEGY PATTERN: SortingService is configured with a sorting
// strategy and delegates to it. New algorithms can be added without
// changing SortingService at all (Open/Closed Principle).
// =====================================================================

class SortingAlgorithm {
  sort(_array) {
    throw new Error("sort() must be implemented");
  }
}

class MergeSort extends SortingAlgorithm {
  sort(array) {
    if (array.length < 2) return;
    this.#mergeSort(array, 0, array.length - 1);
  }

  #mergeSort(array, left, right) {
    if (left >= right) return;
    const mid = left + Math.floor((right - left) / 2);
    this.#mergeSort(array, left, mid);
    this.#mergeSort(array, mid + 1, right);
    this.#merge(array, left, mid, right);
  }

  #merge(array, left, mid, right) {
    const merged = [];
    let i = left;
    let j = mid + 1;
    while (i <= mid && j <= right) {
      merged.push(array[i] <= array[j] ? array[i++] : array[j++]);
    }
    while (i <= mid) merged.push(array[i++]);
    while (j <= right) merged.push(array[j++]);
    for (let k = 0; k < merged.length; k++) {
      array[left + k] = merged[k];
    }
  }
}

class QuickSort extends SortingAlgorithm {
  sort(array) {
    this.#quickSort(array, 0, array.length - 1);
  }

  #quickSort(array, low, high) {
    if (low >= high) return;
    const pivot = array[high];
    let partitionIndex = low;
    for (let i = low; i < high; i++) {
      if (array[i] < pivot) {
        [array[i], array[partitionIndex]] = [array[partitionIndex], array[i]];
        partitionIndex++;
      }
    }
    [array[partitionIndex], array[high]] = [array[high], array[partitionIndex]];
    this.#quickSort(array, low, partitionIndex - 1);
    this.#quickSort(array, partitionIndex + 1, high);
  }
}

class SelectionSort extends SortingAlgorithm {
  sort(array) {
    for (let i = 0; i < array.length - 1; i++) {
      let minIndex = i;
      for (let j = i + 1; j < array.length; j++) {
        if (array[j] < array[minIndex]) {
          minIndex = j;
        }
      }
      [array[i], array[minIndex]] = [array[minIndex], array[i]];
    }
  }
}

class SortingService {
  #sortingAlgorithm;

  setSortingAlgorithm(sortingAlgorithm) {
    this.#sortingAlgorithm = sortingAlgorithm;
  }

  sortAPI(array) {
    if (!this.#sortingAlgorithm) {
      throw new Error("no sorting algorithm configured");
    }
    this.#sortingAlgorithm.sort(array);
  }
}

function main() {
  console.log("== Method overloading (emulated with default params) ==");
  const pradeep = new User();
  pradeep.setName("Pradeep");
  pradeep.addMoney(123);
  pradeep.addMoney(123, 2);
  pradeep.addMoney(123, 2, 3);
  console.log(`Total money: ${pradeep.money}`);

  console.log("\n== Abstraction via Stack interface ==");
  const arrayStack = new ArrayStack();
  arrayStack.push(1);
  arrayStack.push(2);
  console.log(`ArrayStack size: ${arrayStack.getSize()}, pop -> ${arrayStack.pop()}`);

  const linkedListStack = new LinkedListStack();
  linkedListStack.push(10);
  linkedListStack.push(20);
  console.log(`LinkedListStack size: ${linkedListStack.getSize()}, pop -> ${linkedListStack.pop()}`);

  console.log("\n== Template method (Notification) ==");
  new SMSNotification("u1", "Your OTP is 4242").send();
  new EmailNotification("u1", "Your invoice is ready").send();

  console.log("\n== Inheritance + runtime polymorphism ==");
  const animal = new Dog(); // "declared" type Animal, actual type Dog
  animal.speak();           // prints Dog's version -> resolved at runtime

  console.log("\n== Strategy pattern (sorting) ==");
  const service = new SortingService();
  const array = [3, 4, 1, 2];
  service.setSortingAlgorithm(new MergeSort());
  service.sortAPI(array);
  console.log(`After merge sort: [${array}]`);

  const array2 = [5, 1, 4, 2, 8];
  service.setSortingAlgorithm(new QuickSort());
  service.sortAPI(array2);
  console.log(`After quick sort: [${array2}]`);

  const array3 = [9, 3, 7, 1];
  service.setSortingAlgorithm(new SelectionSort());
  service.sortAPI(array3);
  console.log(`After selection sort: [${array3}]`);
}

main();
