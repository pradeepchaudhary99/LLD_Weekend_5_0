// =====================================================================
// TOPIC: Method Overloading, Abstraction, Inheritance/Polymorphism,
//        Interfaces (pure abstract classes), and the Strategy Pattern.
//
// NOTE: in C++, runtime polymorphism (overriding) only works through
// `virtual` methods accessed via a pointer/reference to the base
// class. Overloading (same name, different params) is resolved at
// compile time, exactly like Java.
// =====================================================================

#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

class User {
public:
    void setName(const std::string& suggestedName) {
        // Basic validation before accepting external input.
        if (suggestedName.empty()) {
            throw std::invalid_argument("name cannot be empty");
        }
        name_ = suggestedName;
    }

    // Three overloads, resolved at compile time by the compiler.
    void addMoney(int num1) { money_ += num1; }
    void addMoney(int num1, int num2) { money_ += num1 * num2; }
    void addMoney(int num1, int num2, int num3) { money_ += num1 * num2 * num3; }

    int getMoney() const { return money_; }

private:
    std::string name_;
    int money_ = 0;
};

// =====================================================================
// ABSTRACTION via a pure abstract class ("interface"): callers depend
// on Stack, never on the concrete data structure behind it.
// =====================================================================

class Stack {
public:
    virtual ~Stack() = default;
    virtual void push(int value) = 0;
    virtual int pop() = 0;
    virtual int getSize() const = 0;
};

class ArrayStack : public Stack {
public:
    explicit ArrayStack(size_t capacity) : capacity_(capacity) {
        data_.reserve(capacity);
    }

    void push(int value) override {
        if (data_.size() == capacity_) {
            throw std::runtime_error("stack is full");
        }
        data_.push_back(value);
    }

    int pop() override {
        if (data_.empty()) {
            throw std::runtime_error("stack is empty");
        }
        int value = data_.back();
        data_.pop_back();
        return value;
    }

    int getSize() const override { return static_cast<int>(data_.size()); }

private:
    std::vector<int> data_;
    size_t capacity_;
};

class LinkedListStack : public Stack {
    struct Node {
        int value;
        Node* next;
    };

public:
    ~LinkedListStack() override {
        while (head_ != nullptr) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
    }

    void push(int value) override {
        head_ = new Node{value, head_};
        size_++;
    }

    int pop() override {
        if (head_ == nullptr) {
            throw std::runtime_error("stack is empty");
        }
        int value = head_->value;
        Node* old = head_;
        head_ = head_->next;
        delete old;
        size_--;
        return value;
    }

    int getSize() const override { return size_; }

private:
    Node* head_ = nullptr;
    int size_ = 0;
};

// =====================================================================
// TEMPLATE METHOD pattern: the abstract class fixes the *skeleton* of
// an algorithm (templateLoading -> sendNotification) while subclasses
// fill in only the varying step.
// =====================================================================

class Notification {
public:
    Notification(std::string userId, std::string message)
        : userId_(std::move(userId)), message_(std::move(message)) {}

    virtual ~Notification() = default;

    void send() {
        templateLoading();
        sendNotification();
    }

protected:
    void templateLoading() {
        message_ = "[Notification for " + userId_ + "] " + message_;
    }

    virtual void sendNotification() = 0;

    std::string userId_;
    std::string message_;
};

class SMSNotification : public Notification {
public:
    using Notification::Notification;

protected:
    void sendNotification() override {
        std::cout << "Sending SMS -> " << message_ << "\n";
    }
};

class EmailNotification : public Notification {
public:
    using Notification::Notification;

protected:
    void sendNotification() override {
        std::cout << "Sending Email -> " << message_ << "\n";
    }
};

// =====================================================================
// INHERITANCE + RUNTIME POLYMORPHISM: `speak` must be `virtual` for the
// override to be picked up through a base-class pointer/reference.
// =====================================================================

class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() const { std::cout << "The animal makes a sound\n"; }
};

class Dog : public Animal {
public:
    void speak() const override { std::cout << "The dog barks\n"; }
};

// =====================================================================
// STRATEGY PATTERN
// =====================================================================

class SortingAlgorithm {
public:
    virtual ~SortingAlgorithm() = default;
    virtual void sort(std::vector<int>& array) = 0;
};

class MergeSort : public SortingAlgorithm {
public:
    void sort(std::vector<int>& array) override {
        if (array.size() < 2) return;
        mergeSort(array, 0, static_cast<int>(array.size()) - 1);
    }

private:
    void mergeSort(std::vector<int>& array, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(array, left, mid);
        mergeSort(array, mid + 1, right);
        merge(array, left, mid, right);
    }

    void merge(std::vector<int>& array, int left, int mid, int right) {
        std::vector<int> merged;
        merged.reserve(right - left + 1);
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            merged.push_back(array[i] <= array[j] ? array[i++] : array[j++]);
        }
        while (i <= mid) merged.push_back(array[i++]);
        while (j <= right) merged.push_back(array[j++]);
        for (size_t k = 0; k < merged.size(); k++) {
            array[left + k] = merged[k];
        }
    }
};

class QuickSort : public SortingAlgorithm {
public:
    void sort(std::vector<int>& array) override {
        quickSort(array, 0, static_cast<int>(array.size()) - 1);
    }

private:
    void quickSort(std::vector<int>& array, int low, int high) {
        if (low >= high) return;
        int pivot = array[high];
        int partitionIndex = low;
        for (int i = low; i < high; i++) {
            if (array[i] < pivot) {
                std::swap(array[i], array[partitionIndex]);
                partitionIndex++;
            }
        }
        std::swap(array[partitionIndex], array[high]);
        quickSort(array, low, partitionIndex - 1);
        quickSort(array, partitionIndex + 1, high);
    }
};

class SelectionSort : public SortingAlgorithm {
public:
    void sort(std::vector<int>& array) override {
        for (size_t i = 0; i + 1 < array.size(); i++) {
            size_t minIndex = i;
            for (size_t j = i + 1; j < array.size(); j++) {
                if (array[j] < array[minIndex]) {
                    minIndex = j;
                }
            }
            std::swap(array[i], array[minIndex]);
        }
    }
};

class SortingService {
public:
    void setSortingAlgorithm(std::shared_ptr<SortingAlgorithm> algorithm) {
        sortingAlgorithm_ = std::move(algorithm);
    }

    void sortAPI(std::vector<int>& array) {
        if (!sortingAlgorithm_) {
            throw std::runtime_error("no sorting algorithm configured");
        }
        sortingAlgorithm_->sort(array);
    }

private:
    std::shared_ptr<SortingAlgorithm> sortingAlgorithm_;
};

static void printArray(const std::vector<int>& array) {
    std::cout << "[";
    for (size_t i = 0; i < array.size(); i++) {
        std::cout << array[i] << (i + 1 < array.size() ? ", " : "");
    }
    std::cout << "]";
}

int main() {
    std::cout << "== Method overloading ==\n";
    User pradeep;
    pradeep.setName("Pradeep");
    pradeep.addMoney(123);
    pradeep.addMoney(123, 2);
    pradeep.addMoney(123, 2, 3);
    std::cout << "Total money: " << pradeep.getMoney() << "\n";

    std::cout << "\n== Abstraction via Stack interface ==\n";
    std::unique_ptr<Stack> arrayStack = std::make_unique<ArrayStack>(5);
    arrayStack->push(1);
    arrayStack->push(2);
    std::cout << "ArrayStack size: " << arrayStack->getSize() << ", pop -> " << arrayStack->pop() << "\n";

    std::unique_ptr<Stack> linkedListStack = std::make_unique<LinkedListStack>();
    linkedListStack->push(10);
    linkedListStack->push(20);
    std::cout << "LinkedListStack size: " << linkedListStack->getSize() << ", pop -> " << linkedListStack->pop() << "\n";

    std::cout << "\n== Template method (Notification) ==\n";
    SMSNotification("u1", "Your OTP is 4242").send();
    EmailNotification("u1", "Your invoice is ready").send();

    std::cout << "\n== Inheritance + runtime polymorphism ==\n";
    std::unique_ptr<Animal> animal = std::make_unique<Dog>(); // declared Animal*, actual Dog
    animal->speak();                                          // prints Dog's version -> vtable dispatch

    std::cout << "\n== Strategy pattern (sorting) ==\n";
    SortingService service;
    std::vector<int> array = {3, 4, 1, 2};
    service.setSortingAlgorithm(std::make_shared<MergeSort>());
    service.sortAPI(array);
    std::cout << "After merge sort: "; printArray(array); std::cout << "\n";

    std::vector<int> array2 = {5, 1, 4, 2, 8};
    service.setSortingAlgorithm(std::make_shared<QuickSort>());
    service.sortAPI(array2);
    std::cout << "After quick sort: "; printArray(array2); std::cout << "\n";

    std::vector<int> array3 = {9, 3, 7, 1};
    service.setSortingAlgorithm(std::make_shared<SelectionSort>());
    service.sortAPI(array3);
    std::cout << "After selection sort: "; printArray(array3); std::cout << "\n";

    return 0;
}
