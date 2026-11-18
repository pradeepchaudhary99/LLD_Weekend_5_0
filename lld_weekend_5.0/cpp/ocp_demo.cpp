// =====================================================================
// TOPIC: Open/Closed Principle (the "O" in SOLID)
//
// "Software entities should be open for EXTENSION but closed for
// MODIFICATION."
//
// DiscountService never changes when a new discount type is added --
// we just write a new class that implements Discount.
// =====================================================================

#include <iostream>

class Discount {
public:
    virtual ~Discount() = default;
    virtual int calculateDiscount(int amount) const = 0;
};

class DiwaliDiscount : public Discount {
public:
    int calculateDiscount(int amount) const override {
        // 20% off during Diwali.
        return amount - (amount * 20 / 100);
    }
};

class HoliDiscount : public Discount {
public:
    int calculateDiscount(int amount) const override {
        // 10% off during Holi.
        return amount - (amount * 10 / 100);
    }
};

class FestiveDiscount : public Discount {
public:
    int calculateDiscount(int amount) const override {
        // Flat 5% for any other festive season.
        return amount - (amount * 5 / 100);
    }
};

// Adding NewYearDiscount tomorrow needs ZERO changes below this line --
// that is exactly what "open for extension, closed for modification" means.
class NewYearDiscount : public Discount {
public:
    int calculateDiscount(int amount) const override {
        return amount - (amount * 15 / 100);
    }
};

class DiscountService {
public:
    double getFinalAmount(const Discount& discount, int amount) const {
        return discount.calculateDiscount(amount);
    }
};

int main() {
    DiscountService service;
    int amount = 1000;

    std::cout << "Diwali price: " << service.getFinalAmount(DiwaliDiscount(), amount) << "\n";
    std::cout << "Holi price: " << service.getFinalAmount(HoliDiscount(), amount) << "\n";
    std::cout << "Festive price: " << service.getFinalAmount(FestiveDiscount(), amount) << "\n";
    std::cout << "New Year price: " << service.getFinalAmount(NewYearDiscount(), amount) << "\n";

    return 0;
}
