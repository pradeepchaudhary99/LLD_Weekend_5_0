// =====================================================================
// TOPIC: Open/Closed Principle (the "O" in SOLID)
//
// "Software entities should be open for EXTENSION but closed for
// MODIFICATION."
//
// DiscountService never changes when a new discount type is added --
// we just write a new type that implements the Discount interface.
// =====================================================================

package main

import "fmt"

type Discount interface {
	CalculateDiscount(amount int) int
}

type DiwaliDiscount struct{}

func (DiwaliDiscount) CalculateDiscount(amount int) int {
	return amount - (amount * 20 / 100) // 20% off during Diwali
}

type HoliDiscount struct{}

func (HoliDiscount) CalculateDiscount(amount int) int {
	return amount - (amount * 10 / 100) // 10% off during Holi
}

type FestiveDiscount struct{}

func (FestiveDiscount) CalculateDiscount(amount int) int {
	return amount - (amount * 5 / 100) // flat 5% for any other festive season
}

// Adding NewYearDiscount tomorrow needs ZERO changes below this line --
// that is exactly what "open for extension, closed for modification" means.
type NewYearDiscount struct{}

func (NewYearDiscount) CalculateDiscount(amount int) int {
	return amount - (amount * 15 / 100)
}

type DiscountService struct{}

func (DiscountService) GetFinalAmount(discount Discount, amount int) int {
	return discount.CalculateDiscount(amount)
}

func main() {
	service := DiscountService{}
	amount := 1000

	fmt.Println("Diwali price:", service.GetFinalAmount(DiwaliDiscount{}, amount))
	fmt.Println("Holi price:", service.GetFinalAmount(HoliDiscount{}, amount))
	fmt.Println("Festive price:", service.GetFinalAmount(FestiveDiscount{}, amount))
	fmt.Println("New Year price:", service.GetFinalAmount(NewYearDiscount{}, amount))
}
