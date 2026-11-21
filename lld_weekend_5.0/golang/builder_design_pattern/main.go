// =====================================================================
// TOPIC: Builder Design Pattern
//
// Solves the "telescoping constructor" problem (too many constructor
// variants for optional fields) by building the object step by step
// through a fluent, chainable API, and only assembling the real value
// at the very end via Build().
//
// Two properties worth noticing:
//  - CHAINING: every UserBuilder setter returns *UserBuilder, so calls
//    can be strung together.
//  - IMMUTABILITY: once a User is built, its name/rollNumber/marks are
//    unexported and never reassigned -- the object can't drift out of
//    a valid state after construction. (address is left mutable on
//    purpose, to show the contrast -- see SetAddress below.)
//
// NOTE: Go has no classes or private constructors, so "User can only
// be built via UserBuilder" is enforced by convention (an unexported
// struct literal) rather than by the compiler the way Java's private
// constructor does it.
// =====================================================================

package main

import "fmt"

type User struct {
	name       string
	rollNumber int
	address    string // mutable on purpose, unlike the other fields
	marks      []int
}

// SetAddress is the only exported way to change a User after
// construction.
func (u *User) SetAddress(address string) {
	u.address = address
}

func (u *User) String() string {
	return fmt.Sprintf("User{name=%s, rollNumber=%d, address=%s, marks=%v}",
		u.name, u.rollNumber, u.address, u.marks)
}

type UserBuilder struct {
	name       string // required: supplied via NewUserBuilder
	rollNumber int    // optional
	address    string // optional
	marks      []int  // optional
}

func NewUserBuilder(name string) *UserBuilder {
	return &UserBuilder{name: name}
}

func (b *UserBuilder) SetRollNumber(rollNumber int) *UserBuilder {
	b.rollNumber = rollNumber
	return b // enables chaining
}

func (b *UserBuilder) SetAddress(address string) *UserBuilder {
	b.address = address
	return b
}

func (b *UserBuilder) SetMarks(marks []int) *UserBuilder {
	b.marks = marks
	return b
}

func (b *UserBuilder) Build() *User {
	return &User{
		name:       b.name,
		rollNumber: b.rollNumber,
		address:    b.address,
		marks:      b.marks,
	}
}

func main() {
	user := NewUserBuilder("Pradeep").
		SetAddress("Bangalore").
		SetRollNumber(123).
		SetMarks([]int{90, 85}).
		Build()

	fmt.Println(user)

	// address is still mutable after construction:
	user.SetAddress("Mumbai")
	fmt.Println("After moving:", user)
}

// Builder:
// - Chaining -> fluent, readable object construction
// - Immutability -> the built value can't be left half-configured
