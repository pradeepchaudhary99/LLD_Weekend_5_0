// =====================================================================
// TOPIC: Singleton pattern -- idiomatic Go uses sync.Once instead of
// hand-rolled double-checked locking, contrasted with eager
// initialization via a package-level variable.
//
// THE BUG IN THE ORIGINAL JAVA DRAFT this was translated from: it
// checked `instance == nil` once, then locked a mutex and created the
// instance WITHOUT checking again inside the lock. Picture 5
// goroutines all passing the first nil-check before any of them
// acquires the lock -- they would then enter the lock one at a time
// and EACH create a new ConfigurationManager, printing "Object
// created" 5 times. That defeats the entire point of a singleton.
//
// Go's standard library gives you the correct version for free:
// sync.Once.Do runs its function exactly once no matter how many
// goroutines call it concurrently, and every caller blocks until that
// one run finishes -- it IS "double-checked locking", implemented
// correctly, inside the standard library.
// =====================================================================

package main

import (
	"fmt"
	"sync"
)

type ConfigurationManager struct {
	configuration map[string]string
}

var (
	instance     *ConfigurationManager
	instanceOnce sync.Once
)

func GetConfigurationManager() *ConfigurationManager {
	instanceOnce.Do(func() {
		fmt.Println("Object created")
		instance = &ConfigurationManager{configuration: make(map[string]string)}
	})
	return instance
}

// Eager initialization: the instance is created when the package is
// initialized, before main() runs and before any goroutine could
// possibly race to create one. Go's runtime guarantees package-level
// variable initialization happens once, before any other code in the
// package executes -- no explicit synchronization needed.
type configurationManagerEager struct {
	configuration map[string]string
}

var eagerInstance = &configurationManagerEager{configuration: make(map[string]string)}

func GetConfigurationManagerEager() *configurationManagerEager {
	return eagerInstance
}

func main() {
	fmt.Println("== sync.Once: 100 goroutines racing GetConfigurationManager() ==")
	var wg sync.WaitGroup
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			GetConfigurationManager()
		}()
	}
	wg.Wait()
	fmt.Println("\"Object created\" should have printed exactly once above.")

	fmt.Println("\n== Eager initialization ==")
	eager1 := GetConfigurationManagerEager()
	eager2 := GetConfigurationManagerEager()
	fmt.Println("Same instance?", eager1 == eager2)
}
