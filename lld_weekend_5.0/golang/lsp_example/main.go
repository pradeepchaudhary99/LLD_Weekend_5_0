// =====================================================================
// TOPIC: Liskov Substitution Principle (the "L" in SOLID) and how a
// "fat"/"god" interface (ISP violation) sets you up to break it.
//
// "Subtypes must be substitutable for their base type without
// altering the correctness of the program."
//
// Payment below forces every implementation to support Pay(),
// Refund() AND CashBack() -- even payment methods that cannot support
// all three in real life. The moment PhonePay panics from Refund(),
// any code written against the Payment interface can no longer treat
// a PhonePay as a drop-in replacement for a PayTm: that's an LSP
// violation, caused by the interface being too fat (an ISP violation).
// =====================================================================

package main

import "fmt"

// God interface / fat interface:
// every implementer is forced to support all three operations,
// whether or not it actually can.
type Payment interface {
	Pay()
	Refund()
	CashBack()
}

type Application struct {
	payment Payment
}

func (a *Application) SetPayment(payment Payment) {
	a.payment = payment
}

func (a *Application) ApplicationUI() {
	a.payment.Pay()
	a.payment.Refund()   // callers can't trust this won't panic
	a.payment.CashBack()
}

type PhonePay struct{}

func (PhonePay) Pay() { fmt.Println("PhonePay: payment successful") }

func (PhonePay) Refund() {
	// PhonePay (in this example) does not support refunds at all --
	// forced to implement a method it cannot honor.
	panic("PhonePay does not support refunds")
}

func (PhonePay) CashBack() { fmt.Println("PhonePay: cashback credited") }

type PayTm struct{}

func (PayTm) Pay()      { fmt.Println("PayTm: payment successful") }
func (PayTm) Refund()   { fmt.Println("PayTm: refund processed") }
func (PayTm) CashBack() { fmt.Println("PayTm: cashback credited") }

// --------------------------------------------------------------------
// THE FIX: split the fat interface into small, focused ones (ISP).
// Now Application can depend on exactly the capability it needs, and
// anything satisfying RefundFeature is guaranteed to honor Refund()
// -- restoring LSP, because there's no method left to panic from.
// --------------------------------------------------------------------

type CashBackFeature interface {
	CashBack()
}

type RefundFeature interface {
	Refund()
}

type SegregatedPayTm struct{}

func (SegregatedPayTm) Pay()      { fmt.Println("PayTm: payment successful") }
func (SegregatedPayTm) Refund()   { fmt.Println("PayTm: refund processed") }
func (SegregatedPayTm) CashBack() { fmt.Println("PayTm: cashback credited") }

func main() {
	fmt.Println("== LSP violation via fat interface ==")
	app := &Application{}
	app.SetPayment(PayTm{})
	app.ApplicationUI()

	app.SetPayment(PhonePay{})
	func() {
		defer func() {
			if r := recover(); r != nil {
				fmt.Println("LSP violated:", r) // PhonePay is NOT truly
				// substitutable for PayTm here.
			}
		}()
		app.ApplicationUI()
	}()

	fmt.Println("\n== After segregating the interface (ISP) ==")
	var refundable RefundFeature = SegregatedPayTm{}
	refundable.Refund() // guaranteed to work: only refundable types implement this
}
