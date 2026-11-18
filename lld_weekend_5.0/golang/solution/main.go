// =====================================================================
// TOPIC: Method Overloading, Abstraction, Inheritance/Polymorphism,
//        Interfaces, and the Strategy Design Pattern.
//
// NOTE ON OVERLOADING: Go has no method/function overloading at all --
// two functions cannot share a name in the same package regardless of
// signature. We emulate Java's three `addMoney` overloads with a single
// variadic method, `AddMoney(multipliers ...int)`, which is the
// idiomatic Go way to accept a variable number of arguments.
//
// NOTE ON INHERITANCE: Go has no classes or `extends`. "IS-A" is
// modeled with struct embedding (see Dog embedding Animal below), and
// interfaces give you abstraction/polymorphism without a class
// hierarchy at all.
// =====================================================================

package main

import (
	"errors"
	"fmt"
)

// ---------------------------------------------------------------------

type User struct {
	name  string
	money int
}

func (u *User) SetName(suggestedName string) error {
	// Basic validation before accepting external input.
	if suggestedName == "" {
		return errors.New("name cannot be empty")
	}
	u.name = suggestedName
	return nil
}

// AddMoney stands in for Java's addMoney(a) / addMoney(a, b) / addMoney(a, b, c)
// by folding any extra multipliers into a running product.
func (u *User) AddMoney(num1 int, multipliers ...int) {
	total := num1
	for _, m := range multipliers {
		total *= m
	}
	u.money += total
}

// ---------------------------------------------------------------------
// ABSTRACTION via an interface: callers depend on Stack, never on the
// concrete data structure behind it.
// ---------------------------------------------------------------------

type Stack interface {
	Push(value int)
	Pop() (int, error)
	GetSize() int
}

// ArrayStack is a slice-backed stack.
type ArrayStack struct {
	data []int
}

func NewArrayStack() *ArrayStack {
	return &ArrayStack{data: make([]int, 0)}
}

func (s *ArrayStack) Push(value int) {
	s.data = append(s.data, value)
}

func (s *ArrayStack) Pop() (int, error) {
	if len(s.data) == 0 {
		return 0, errors.New("stack is empty")
	}
	last := len(s.data) - 1
	value := s.data[last]
	s.data = s.data[:last]
	return value, nil
}

func (s *ArrayStack) GetSize() int {
	return len(s.data)
}

// LinkedListStack is a linked-list-backed stack: same contract, different representation.
type node struct {
	value int
	next  *node
}

type LinkedListStack struct {
	head *node
	size int
}

func (s *LinkedListStack) Push(value int) {
	s.head = &node{value: value, next: s.head}
	s.size++
}

func (s *LinkedListStack) Pop() (int, error) {
	if s.head == nil {
		return 0, errors.New("stack is empty")
	}
	value := s.head.value
	s.head = s.head.next
	s.size--
	return value, nil
}

func (s *LinkedListStack) GetSize() int {
	return s.size
}

// ---------------------------------------------------------------------
// TEMPLATE-METHOD-LIKE pattern without inheritance: Go favors
// composition, so the fixed skeleton lives in a free function that
// takes the varying step as a function value.
// ---------------------------------------------------------------------

type Notification struct {
	UserID  string
	Message string
}

func (n *Notification) templateLoading() {
	n.Message = fmt.Sprintf("[Notification for %s] %s", n.UserID, n.Message)
}

// Send runs the fixed skeleton (load template, then dispatch) using
// whichever "send" behavior is passed in.
func (n *Notification) Send(sendFn func(message string)) {
	n.templateLoading()
	sendFn(n.Message)
}

func sendSMS(message string) {
	fmt.Println("Sending SMS ->", message)
}

func sendEmail(message string) {
	fmt.Println("Sending Email ->", message)
}

// ---------------------------------------------------------------------
// INHERITANCE (via embedding) + RUNTIME POLYMORPHISM (via interfaces):
// Dog embeds Animal to reuse its fields/methods ("IS-A" via
// composition), and the Speaker interface gives us dynamic dispatch
// without a class hierarchy.
// ---------------------------------------------------------------------

type Speaker interface {
	Speak()
}

type Animal struct{}

func (Animal) Speak() {
	fmt.Println("The animal makes a sound")
}

type Dog struct {
	Animal // embedding: Dog "has an" Animal and inherits its methods
}

func (Dog) Speak() {
	fmt.Println("The dog barks")
}

// ---------------------------------------------------------------------
// STRATEGY PATTERN
// ---------------------------------------------------------------------

type SortingAlgorithm interface {
	Sort(array []int)
}

type MergeSort struct{}

func (MergeSort) Sort(array []int) {
	if len(array) < 2 {
		return
	}
	mergeSort(array, 0, len(array)-1)
}

func mergeSort(array []int, left, right int) {
	if left >= right {
		return
	}
	mid := left + (right-left)/2
	mergeSort(array, left, mid)
	mergeSort(array, mid+1, right)
	merge(array, left, mid, right)
}

func merge(array []int, left, mid, right int) {
	merged := make([]int, 0, right-left+1)
	i, j := left, mid+1
	for i <= mid && j <= right {
		if array[i] <= array[j] {
			merged = append(merged, array[i])
			i++
		} else {
			merged = append(merged, array[j])
			j++
		}
	}
	merged = append(merged, array[i:mid+1]...)
	merged = append(merged, array[j:right+1]...)
	copy(array[left:right+1], merged)
}

type QuickSort struct{}

func (QuickSort) Sort(array []int) {
	quickSort(array, 0, len(array)-1)
}

func quickSort(array []int, low, high int) {
	if low >= high {
		return
	}
	pivot := array[high]
	partitionIndex := low
	for i := low; i < high; i++ {
		if array[i] < pivot {
			array[i], array[partitionIndex] = array[partitionIndex], array[i]
			partitionIndex++
		}
	}
	array[partitionIndex], array[high] = array[high], array[partitionIndex]
	quickSort(array, low, partitionIndex-1)
	quickSort(array, partitionIndex+1, high)
}

type SelectionSort struct{}

func (SelectionSort) Sort(array []int) {
	for i := 0; i < len(array)-1; i++ {
		minIndex := i
		for j := i + 1; j < len(array); j++ {
			if array[j] < array[minIndex] {
				minIndex = j
			}
		}
		array[i], array[minIndex] = array[minIndex], array[i]
	}
}

type SortingService struct {
	sortingAlgorithm SortingAlgorithm
}

func (s *SortingService) SetSortingAlgorithm(algorithm SortingAlgorithm) {
	s.sortingAlgorithm = algorithm
}

func (s *SortingService) SortAPI(array []int) error {
	if s.sortingAlgorithm == nil {
		return errors.New("no sorting algorithm configured")
	}
	s.sortingAlgorithm.Sort(array)
	return nil
}

func main() {
	fmt.Println("== Method overloading (emulated with variadic args) ==")
	pradeep := &User{}
	_ = pradeep.SetName("Pradeep")
	pradeep.AddMoney(123)
	pradeep.AddMoney(123, 2)
	pradeep.AddMoney(123, 2, 3)
	fmt.Println("Total money:", pradeep.money)

	fmt.Println("\n== Abstraction via Stack interface ==")
	var arrayStack Stack = NewArrayStack()
	arrayStack.Push(1)
	arrayStack.Push(2)
	size := arrayStack.GetSize()
	popped, _ := arrayStack.Pop()
	fmt.Printf("ArrayStack size: %d, pop -> %d\n", size, popped)

	var linkedListStack Stack = &LinkedListStack{}
	linkedListStack.Push(10)
	linkedListStack.Push(20)
	size = linkedListStack.GetSize()
	popped, _ = linkedListStack.Pop()
	fmt.Printf("LinkedListStack size: %d, pop -> %d\n", size, popped)

	fmt.Println("\n== Template-style notification ==")
	sms := &Notification{UserID: "u1", Message: "Your OTP is 4242"}
	sms.Send(sendSMS)
	email := &Notification{UserID: "u1", Message: "Your invoice is ready"}
	email.Send(sendEmail)

	fmt.Println("\n== Embedding + interface-based polymorphism ==")
	var speaker Speaker = Dog{} // declared type Speaker, actual type Dog
	speaker.Speak()             // prints Dog's version -> resolved at runtime

	fmt.Println("\n== Strategy pattern (sorting) ==")
	service := &SortingService{}
	array := []int{3, 4, 1, 2}
	service.SetSortingAlgorithm(MergeSort{})
	_ = service.SortAPI(array)
	fmt.Println("After merge sort:", array)

	array2 := []int{5, 1, 4, 2, 8}
	service.SetSortingAlgorithm(QuickSort{})
	_ = service.SortAPI(array2)
	fmt.Println("After quick sort:", array2)

	array3 := []int{9, 3, 7, 1}
	service.SetSortingAlgorithm(SelectionSort{})
	_ = service.SortAPI(array3)
	fmt.Println("After selection sort:", array3)
}
