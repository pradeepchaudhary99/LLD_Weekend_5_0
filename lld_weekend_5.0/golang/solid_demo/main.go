// SOLID principles, demonstrated with a bad design followed by a good one.
//
// Go has no classes or inheritance, so "violations" that rely on
// subclassing in the Java version (LSP, ISP) are reproduced here with
// struct embedding and interfaces -- the idiomatic Go equivalents.

package main

import (
	"errors"
	"fmt"
)

// ============================================================
// PART 1: BAD DESIGN - VIOLATES SOLID PRINCIPLES
// ============================================================

// BadOrderService:
// SRP Violation: one type handles pricing, payment, notification, invoice.
// OCP Violation: adding a payment method requires modifying this method.
// DIP Violation: business logic depends on concrete string checks.
type BadOrderService struct{}

func (BadOrderService) PlaceOrder(customer string, amount float64, paymentType string) error {
	// 1. Calculate discount
	finalAmount := amount
	if amount > 1000 {
		finalAmount = amount * 0.90
	}

	// 2. Process payment
	switch paymentType {
	case "UPI":
		fmt.Println("Paid via UPI:", finalAmount)
	case "CARD":
		fmt.Println("Paid via Card:", finalAmount)
	default:
		return errors.New("invalid payment")
	}

	// 3. Send email
	fmt.Println("Email sent to", customer)

	// 4. Generate invoice
	fmt.Println("Invoice generated:", finalAmount)
	return nil
}

// LSP-style violation: BadCashOnDelivery embeds BadPaymentMethod but
// cannot honor its Refund behavior -- callers that only know about
// BadPaymentMethod get a surprise panic.
type BadPaymentMethod struct{}

func (BadPaymentMethod) Pay(amount float64)    { fmt.Println("Payment completed") }
func (BadPaymentMethod) Refund(amount float64) { fmt.Println("Refund completed") }

type BadCashOnDelivery struct {
	BadPaymentMethod
}

func (BadCashOnDelivery) Refund(amount float64) {
	panic("COD does not support online refunds")
}

// ISP-style violation: one fat interface forces every implementation
// to define methods it doesn't actually need.
type BadOrderOperations interface {
	Pay(amount float64)
	Refund(amount float64)
	SendEmail(customer string)
	GenerateInvoice(amount float64)
}

type BadUPIProcessor struct{}

func (BadUPIProcessor) Pay(amount float64)       { fmt.Println("UPI payment") }
func (BadUPIProcessor) Refund(amount float64)    { fmt.Println("UPI refund") }
func (BadUPIProcessor) SendEmail(customer string) { panic("not supported") }
func (BadUPIProcessor) GenerateInvoice(amount float64) { panic("not supported") }

// ============================================================
// PART 2: GOOD DESIGN - FOLLOWING SOLID PRINCIPLES
// ============================================================

// --------------------
// S: SINGLE RESPONSIBILITY
// --------------------
type DiscountCalculator struct{}

func (DiscountCalculator) Calculate(amount float64) float64 {
	if amount > 1000 {
		return amount * 0.90
	}
	return amount
}

type InvoiceGenerator struct{}

func (InvoiceGenerator) Generate(amount float64) {
	fmt.Println("Invoice generated:", amount)
}

// --------------------
// O: OPEN/CLOSED
// --------------------
// New payment methods can be added without modifying
// the existing order processing logic.
type PaymentMethod interface {
	Pay(amount float64)
}

type UPIPayment struct{}

func (UPIPayment) Pay(amount float64) { fmt.Println("Paid via UPI:", amount) }

type CardPayment struct{}

func (CardPayment) Pay(amount float64) { fmt.Println("Paid via Card:", amount) }

// New feature: no changes to OrderService required.
type WalletPayment struct{}

func (WalletPayment) Pay(amount float64) { fmt.Println("Paid via Wallet:", amount) }

// --------------------
// L: LISKOV SUBSTITUTION
// --------------------
// Every PaymentMethod implementation supports Pay().
// Refund is NOT forced into PaymentMethod because not every
// payment method supports online refunds.
type RefundablePayment interface {
	PaymentMethod
	Refund(amount float64)
}

type RefundableUPIPayment struct{}

func (RefundableUPIPayment) Pay(amount float64)    { fmt.Println("UPI payment:", amount) }
func (RefundableUPIPayment) Refund(amount float64) { fmt.Println("UPI refund:", amount) }

type CashOnDelivery struct{}

func (CashOnDelivery) Pay(amount float64) { fmt.Println("COD order placed:", amount) }

// --------------------
// I: INTERFACE SEGREGATION
// --------------------
// Small, focused interfaces.
type Notifier interface {
	NotifyCustomer(customer string)
}

type InvoiceService interface {
	Generate(amount float64)
}

type EmailNotifier struct{}

func (EmailNotifier) NotifyCustomer(customer string) {
	fmt.Println("Email sent to", customer)
}

type SimpleInvoiceService struct {
	generator InvoiceGenerator
}

func (s SimpleInvoiceService) Generate(amount float64) {
	s.generator.Generate(amount)
}

// --------------------
// D: DEPENDENCY INVERSION
// --------------------
// High-level OrderService depends on interfaces (abstractions),
// not concrete UPI/Card/Email implementations.
type OrderService struct {
	payment            PaymentMethod
	notifier           Notifier
	invoiceService     InvoiceService
	discountCalculator DiscountCalculator
}

func NewOrderService(payment PaymentMethod, notifier Notifier,
	invoiceService InvoiceService, discountCalculator DiscountCalculator) *OrderService {
	return &OrderService{
		payment:            payment,
		notifier:           notifier,
		invoiceService:     invoiceService,
		discountCalculator: discountCalculator,
	}
}

func (s *OrderService) PlaceOrder(customer string, amount float64) {
	finalAmount := s.discountCalculator.Calculate(amount)
	s.payment.Pay(finalAmount)
	s.notifier.NotifyCustomer(customer)
	s.invoiceService.Generate(finalAmount)
}

// ============================================================
// PART 3: MAIN - RUN BOTH IMPLEMENTATIONS
// ============================================================

func main() {
	fmt.Println("====== BAD DESIGN ======")
	badService := BadOrderService{}
	_ = badService.PlaceOrder("Pradeep", 2000, "UPI")

	fmt.Println("\n====== GOOD DESIGN ======")
	goodService := NewOrderService(
		UPIPayment{},
		EmailNotifier{},
		SimpleInvoiceService{},
		DiscountCalculator{},
	)
	goodService.PlaceOrder("Pradeep", 2000)

	fmt.Println("\n====== EXTENDING PAYMENT ======")
	// Switching to Wallet requires no OrderService changes.
	walletOrder := NewOrderService(
		WalletPayment{},
		EmailNotifier{},
		SimpleInvoiceService{},
		DiscountCalculator{},
	)
	walletOrder.PlaceOrder("Rahul", 1500)

	fmt.Println("\n====== LSP DEMO ======")
	cod := CashOnDelivery{}
	cod.Pay(500)

	refundable := RefundableUPIPayment{}
	refundable.Pay(500)
	refundable.Refund(500)
}
