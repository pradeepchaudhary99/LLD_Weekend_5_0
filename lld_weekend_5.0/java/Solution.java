// =====================================================================
// TOPIC: Method Overloading, Abstraction, Inheritance/Polymorphism,
//        Interfaces, and the Strategy Design Pattern.
// =====================================================================

/**
 * Demonstrates METHOD OVERLOADING: same method name, different parameter
 * lists, resolved by the compiler at compile time (a form of
 * "compile-time polymorphism").
 */
class User {
    private String name;
    private String rollNumber;
    private int money;

    void setName(String suggestedName) {
        // Basic validation before accepting external input.
        if (suggestedName == null || suggestedName.isBlank()) {
            throw new IllegalArgumentException("name cannot be empty");
        }
        this.name = suggestedName;
    }

    void addMoney(int num1) {
        this.money += num1;
    }

    void addMoney(int num1, int num2) {
        this.money += num1 * num2;
    }

    void addMoney(int num1, int num2, int num3) {
        this.money += num1 * num2 * num3;
    }

    int getMoney() {
        return money;
    }
}

// =====================================================================
// ABSTRACTION via an interface: callers depend on "Stack", never on the
// concrete data structure behind it. Either implementation can be swapped
// in without touching calling code.
// =====================================================================

interface Stack {
    void push(int value);
    int pop();
    int getSize();
}

/** Array-backed stack. Renamed from "Arrays" so it doesn't shadow java.util.Arrays. */
class ArrayStack implements Stack {
    private final int[] data;
    private int size = 0;

    ArrayStack(int capacity) {
        this.data = new int[capacity];
    }

    @Override
    public void push(int value) {
        if (size == data.length) {
            throw new IllegalStateException("stack is full");
        }
        data[size++] = value;
    }

    @Override
    public int pop() {
        if (size == 0) {
            throw new IllegalStateException("stack is empty");
        }
        return data[--size];
    }

    @Override
    public int getSize() {
        return size;
    }
}

/** Linked-list-backed stack: same contract, different internal representation. */
class LinkedListStack implements Stack {
    private static class Node {
        int value;
        Node next;

        Node(int value, Node next) {
            this.value = value;
            this.next = next;
        }
    }

    private Node head;
    private int size = 0;

    @Override
    public void push(int value) {
        head = new Node(value, head);
        size++;
    }

    @Override
    public int pop() {
        if (head == null) {
            throw new IllegalStateException("stack is empty");
        }
        int value = head.value;
        head = head.next;
        size--;
        return value;
    }

    @Override
    public int getSize() {
        return size;
    }
}

// =====================================================================
// TEMPLATE METHOD pattern: the abstract class fixes the *skeleton* of an
// algorithm (templateLoading -> sendNotification) while subclasses fill
// in only the varying step.
// =====================================================================

abstract class Notification {
    String message;
    String userId;

    Notification(String userId, String message) {
        this.userId = userId;
        this.message = message;
    }

    /** Shared by every notification type: build the final text from a template. */
    void templateLoading() {
        this.message = "[Notification for " + userId + "] " + message;
    }

    /** The one step every subclass must specialize. */
    abstract void sendNotification();

    /** Template method: fixed order of steps, reused by every subclass. */
    final void send() {
        templateLoading();
        sendNotification();
    }
}

class SMSNotification extends Notification {
    SMSNotification(String userId, String message) {
        super(userId, message);
    }

    @Override
    void sendNotification() {
        System.out.println("Sending SMS -> " + message);
    }
}

class EmailNotification extends Notification {
    EmailNotification(String userId, String message) {
        super(userId, message);
    }

    @Override
    void sendNotification() {
        System.out.println("Sending Email -> " + message);
    }
}

// =====================================================================
// INHERITANCE + RUNTIME POLYMORPHISM:
// - Compile-time polymorphism  = method overloading (see User above).
// - Runtime polymorphism       = method overriding, resolved via the
//   actual object type at run time even when accessed through a parent
//   reference (see `speak()` below).
// =====================================================================

class Animal {
    void speak() {
        System.out.println("The animal makes a sound");
    }
}

class Dog extends Animal {
    @Override
    void speak() {
        System.out.println("The dog barks");
    }
}

// =====================================================================
// STRATEGY PATTERN: SortingService is configured with a SortingAlgorithm
// and delegates to it. New algorithms can be added without changing
// SortingService at all (Open/Closed Principle).
// =====================================================================

interface SortingAlgorithm {
    void sort(int[] array);
}

class MergeSort implements SortingAlgorithm {
    @Override
    public void sort(int[] array) {
        if (array.length < 2) return;
        mergeSort(array, 0, array.length - 1);
    }

    private void mergeSort(int[] array, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(array, left, mid);
        mergeSort(array, mid + 1, right);
        merge(array, left, mid, right);
    }

    private void merge(int[] array, int left, int mid, int right) {
        int[] merged = new int[right - left + 1];
        int i = left, j = mid + 1, k = 0;
        while (i <= mid && j <= right) {
            merged[k++] = array[i] <= array[j] ? array[i++] : array[j++];
        }
        while (i <= mid) merged[k++] = array[i++];
        while (j <= right) merged[k++] = array[j++];
        System.arraycopy(merged, 0, array, left, merged.length);
    }
}

class QuickSort implements SortingAlgorithm {
    @Override
    public void sort(int[] array) {
        quickSort(array, 0, array.length - 1);
    }

    private void quickSort(int[] array, int low, int high) {
        if (low >= high) return;
        int pivot = array[high];
        int partitionIndex = low;
        for (int i = low; i < high; i++) {
            if (array[i] < pivot) {
                swap(array, i, partitionIndex);
                partitionIndex++;
            }
        }
        swap(array, partitionIndex, high);
        quickSort(array, low, partitionIndex - 1);
        quickSort(array, partitionIndex + 1, high);
    }

    private void swap(int[] array, int i, int j) {
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

class SelectionSort implements SortingAlgorithm {
    @Override
    public void sort(int[] array) {
        for (int i = 0; i < array.length - 1; i++) {
            int minIndex = i;
            for (int j = i + 1; j < array.length; j++) {
                if (array[j] < array[minIndex]) {
                    minIndex = j;
                }
            }
            int tmp = array[minIndex];
            array[minIndex] = array[i];
            array[i] = tmp;
        }
    }
}

class SortingService {
    private SortingAlgorithm sortingAlgorithm;

    void setSortingAlgorithm(SortingAlgorithm sortingAlgorithm) {
        this.sortingAlgorithm = sortingAlgorithm;
    }

    void sortAPI(int[] array) {
        if (sortingAlgorithm == null) {
            throw new IllegalStateException("no sorting algorithm configured");
        }
        sortingAlgorithm.sort(array);
    }
}

public class Solution {
    public static void main(String[] args) {

        System.out.println("== Method overloading ==");
        User pradeep = new User();
        pradeep.setName("Pradeep");
        pradeep.addMoney(123);
        pradeep.addMoney(123, 2);
        pradeep.addMoney(123, 2, 3);
        System.out.println("Total money: " + pradeep.getMoney());

        System.out.println("\n== Abstraction via Stack interface ==");
        Stack arrayStack = new ArrayStack(5);
        arrayStack.push(1);
        arrayStack.push(2);
        System.out.println("ArrayStack size: " + arrayStack.getSize() + ", pop -> " + arrayStack.pop());

        Stack linkedListStack = new LinkedListStack();
        linkedListStack.push(10);
        linkedListStack.push(20);
        System.out.println("LinkedListStack size: " + linkedListStack.getSize() + ", pop -> " + linkedListStack.pop());

        System.out.println("\n== Template method (Notification) ==");
        new SMSNotification("u1", "Your OTP is 4242").send();
        new EmailNotification("u1", "Your invoice is ready").send();

        System.out.println("\n== Inheritance + runtime polymorphism ==");
        Animal animal = new Dog(); // declared type Animal, actual type Dog
        animal.speak();            // prints Dog's version -> resolved at runtime

        System.out.println("\n== Strategy pattern (sorting) ==");
        SortingService service = new SortingService();
        int[] array = {3, 4, 1, 2};

        service.setSortingAlgorithm(new MergeSort());
        service.sortAPI(array);
        System.out.println("After merge sort: " + java.util.Arrays.toString(array));

        int[] array2 = {5, 1, 4, 2, 8};
        service.setSortingAlgorithm(new QuickSort());
        service.sortAPI(array2);
        System.out.println("After quick sort: " + java.util.Arrays.toString(array2));

        int[] array3 = {9, 3, 7, 1};
        service.setSortingAlgorithm(new SelectionSort());
        service.sortAPI(array3);
        System.out.println("After selection sort: " + java.util.Arrays.toString(array3));
    }
}
