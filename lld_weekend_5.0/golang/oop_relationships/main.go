// =====================================================================
// TOPIC: The five class relationships every LLD interview expects you
// to name correctly: Association, Aggregation, Composition,
// Inheritance, and Dependency.
//
// Go has no classes, so "inheritance" below is modeled with struct
// embedding -- the closest idiomatic equivalent to IS-A in Go.
// =====================================================================

package main

import "fmt"

// =========================================
// 1. ASSOCIATION (Knows-A)
// Student is associated with Teacher.
// Neither owns the other; both can exist independently.
// =========================================

type Teacher struct {
	Name string
}

func (t *Teacher) Teach() {
	fmt.Println(t.Name + " is teaching")
}

type Student struct {
	Name    string
	Teacher *Teacher
}

func (s *Student) AttendClass() {
	s.Teacher.Teach()
}

// =========================================
// 2. AGGREGATION (Weak HAS-A)
// Department contains Teachers, but Teachers can exist
// without the Department (e.g. shared across departments).
// Storing pointers (not owning values) is what makes this aggregation.
// =========================================

type Department struct {
	Name     string
	Teachers []*Teacher
}

func NewDepartment(name string, teachers []*Teacher) *Department {
	copied := make([]*Teacher, len(teachers))
	copy(copied, teachers)
	return &Department{Name: name, Teachers: copied}
}

func (d *Department) AddTeacher(t *Teacher) {
	d.Teachers = append(d.Teachers, t)
}

func (d *Department) ShowTeachers() {
	for _, t := range d.Teachers {
		fmt.Println(t.Name)
	}
}

// =========================================
// 3. COMPOSITION (Strong HAS-A)
// House owns its Rooms by value. Rooms have no meaning or
// lifecycle outside the House that created them -- there is no
// way to obtain a *room* without going through a *House.
// =========================================

type room struct { // lowercase: unexported, private to this package
	name string
}

type House struct {
	rooms []room
}

func NewHouse() *House {
	return &House{rooms: []room{{name: "Bedroom"}, {name: "Kitchen"}}}
}

func (h *House) ShowRooms() {
	for _, r := range h.rooms {
		fmt.Println(r.name)
	}
}

// =========================================
// 4. INHERITANCE (IS-A, via struct embedding)
// Dog embeds Animal: it reuses Animal's behavior and can be
// passed anywhere an Animal is expected (Go calls this embedding,
// not inheritance, but it serves the same IS-A purpose).
// =========================================

type Animal struct{}

func (Animal) Eat() {
	fmt.Println("Animal is eating")
}

type Dog struct {
	Animal
}

func (Dog) Bark() {
	fmt.Println("Dog is barking")
}

// =========================================
// 5. DEPENDENCY (USES-A)
// OrderService temporarily uses a PaymentService that is
// passed in -- it doesn't hold a field for it anywhere.
// =========================================

type PaymentService struct{}

func (PaymentService) Pay() {
	fmt.Println("Payment successful")
}

type OrderService struct{}

// No field on OrderService: it does NOT hold a PaymentService --
// it only "uses" one for the duration of a single Checkout() call.
// That's what makes this a dependency rather than
// association/aggregation/composition.
func (OrderService) Checkout(paymentService *PaymentService) {
	paymentService.Pay()
}

func main() {
	fmt.Println("1. ASSOCIATION")
	teacher := &Teacher{Name: "Rahul"}
	student := &Student{Name: "Amit", Teacher: teacher}
	student.AttendClass()

	fmt.Println("\n2. AGGREGATION")
	t1 := &Teacher{Name: "Rahul"}
	t2 := &Teacher{Name: "Priya"}

	dept := NewDepartment("Computer Science", []*Teacher{t1, t2})
	dept.ShowTeachers()

	// Teachers still exist independently of the department.
	fmt.Println(t1.Name + " exists independently")

	fmt.Println("\n3. COMPOSITION")
	house := NewHouse()
	house.ShowRooms()

	fmt.Println("\n4. INHERITANCE (embedding)")
	dog := Dog{}
	dog.Eat() // promoted from embedded Animal
	dog.Bark()

	fmt.Println("\n5. DEPENDENCY")
	orderService := OrderService{}
	paymentService := &PaymentService{}

	orderService.Checkout(paymentService)
}
