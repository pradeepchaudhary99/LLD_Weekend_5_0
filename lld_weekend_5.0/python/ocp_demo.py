"""
TOPIC: Open/Closed Principle (the "O" in SOLID)

"Software entities should be open for EXTENSION but closed for
MODIFICATION."

DiscountService never changes when a new discount type is added -- we
just write a new class that implements the Discount protocol.
"""

from abc import ABC, abstractmethod


class Discount(ABC):
    @abstractmethod
    def calculate_discount(self, amount: int) -> int: ...


class DiwaliDiscount(Discount):
    def calculate_discount(self, amount: int) -> int:
        # 20% off during Diwali.
        return amount - (amount * 20 // 100)


class HoliDiscount(Discount):
    def calculate_discount(self, amount: int) -> int:
        # 10% off during Holi.
        return amount - (amount * 10 // 100)


class FestiveDiscount(Discount):
    def calculate_discount(self, amount: int) -> int:
        # Flat 5% for any other festive season.
        return amount - (amount * 5 // 100)


# Adding NewYearDiscount tomorrow needs ZERO changes below this line --
# that is exactly what "open for extension, closed for modification" means.
class NewYearDiscount(Discount):
    def calculate_discount(self, amount: int) -> int:
        return amount - (amount * 15 // 100)


class DiscountService:
    def get_final_amount(self, discount: Discount, amount: int) -> float:
        return discount.calculate_discount(amount)


def main() -> None:
    service = DiscountService()
    amount = 1000

    print(f"Diwali price: {service.get_final_amount(DiwaliDiscount(), amount)}")
    print(f"Holi price: {service.get_final_amount(HoliDiscount(), amount)}")
    print(f"Festive price: {service.get_final_amount(FestiveDiscount(), amount)}")
    print(f"New Year price: {service.get_final_amount(NewYearDiscount(), amount)}")


if __name__ == "__main__":
    main()
