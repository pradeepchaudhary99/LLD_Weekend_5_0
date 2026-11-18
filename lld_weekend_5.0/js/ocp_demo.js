// =====================================================================
// TOPIC: Open/Closed Principle (the "O" in SOLID)
//
// "Software entities should be open for EXTENSION but closed for
// MODIFICATION."
//
// DiscountService never changes when a new discount type is added --
// we just write a new class that implements the Discount shape.
// =====================================================================

"use strict";

class Discount {
  calculateDiscount(_amount) {
    throw new Error("calculateDiscount() must be implemented");
  }
}

class DiwaliDiscount extends Discount {
  calculateDiscount(amount) {
    // 20% off during Diwali.
    return amount - Math.floor((amount * 20) / 100);
  }
}

class HoliDiscount extends Discount {
  calculateDiscount(amount) {
    // 10% off during Holi.
    return amount - Math.floor((amount * 10) / 100);
  }
}

class FestiveDiscount extends Discount {
  calculateDiscount(amount) {
    // Flat 5% for any other festive season.
    return amount - Math.floor((amount * 5) / 100);
  }
}

// Adding NewYearDiscount tomorrow needs ZERO changes below this line --
// that is exactly what "open for extension, closed for modification" means.
class NewYearDiscount extends Discount {
  calculateDiscount(amount) {
    return amount - Math.floor((amount * 15) / 100);
  }
}

class DiscountService {
  getFinalAmount(discount, amount) {
    return discount.calculateDiscount(amount);
  }
}

function main() {
  const service = new DiscountService();
  const amount = 1000;

  console.log(`Diwali price: ${service.getFinalAmount(new DiwaliDiscount(), amount)}`);
  console.log(`Holi price: ${service.getFinalAmount(new HoliDiscount(), amount)}`);
  console.log(`Festive price: ${service.getFinalAmount(new FestiveDiscount(), amount)}`);
  console.log(`New Year price: ${service.getFinalAmount(new NewYearDiscount(), amount)}`);
}

main();
