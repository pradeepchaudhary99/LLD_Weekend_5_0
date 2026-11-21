// =====================================================================
// TOPIC: Singleton pattern -- the package-level variable approach,
// Go's closest equivalent to Java's enum-based singleton.
//
// Go has no enum type at all, so there's no direct equivalent of
// Java's enum-constant trick. The closest idiom that gives you the
// same "exactly one instance, created once, no locking needed"
// property is a package-level variable: the Go runtime guarantees
// package-level variables are initialized exactly once, before any
// other code in the package runs and before main() starts -- the same
// guarantee that makes Java's enum singleton the simplest option
// there.
// =====================================================================

package main

import "fmt"

type databaseConnection struct{}

func (databaseConnection) Connect() {
	fmt.Println("Connected to DB")
}

// Instance is the only databaseConnection we ever create. The Go
// runtime guarantees this package-level variable is initialized
// exactly once per process, before main() runs.
var Instance = databaseConnection{}

func main() {
	db1 := Instance
	db2 := Instance
	db1.Connect()
	fmt.Println("Same instance?", db1 == db2)
}
