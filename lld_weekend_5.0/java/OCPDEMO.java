// =====================================================================
// TOPIC: Open/Closed Principle (the "O" in SOLID)
//
// "Software entities should be open for EXTENSION but closed for
// MODIFICATION."
//
// Here, DiscountService never changes when a new discount type is
// added -- we just write a new class that implements Discount.
// =====================================================================

interface Discount {
    int calculateDiscount(int amount);
}

class DiwaliDiscount implements Discount {
    @Override
    public int calculateDiscount(int amount) {
        // 20% off during Diwali.
        return amount - (amount * 20 / 100);
    }
}

class HoliDiscount implements Discount {
    @Override
    public int calculateDiscount(int amount) {
        // 10% off during Holi.
        return amount - (amount * 10 / 100);
    }
}

class FestiveDiscount implements Discount {
    @Override
    public int calculateDiscount(int amount) {
        // Flat 5% for any other festive season.
        return amount - (amount * 5 / 100);
    }
}

// Adding NewYearDiscount tomorrow needs ZERO changes below this line --
// that is exactly what "open for extension, closed for modification" means.
class NewYearDiscount implements Discount {
    @Override
    public int calculateDiscount(int amount) {
        return amount - (amount * 15 / 100);
    }
}

class DiscountService {
    double getFinalAmount(Discount discount, int amount) {
        return discount.calculateDiscount(amount);
    }
}

public class OCPDEMO {
    public static void main(String[] args) {
        DiscountService service = new DiscountService();
        int amount = 1000;

        System.out.println("Diwali price: " + service.getFinalAmount(new DiwaliDiscount(), amount));
        System.out.println("Holi price: " + service.getFinalAmount(new HoliDiscount(), amount));
        System.out.println("Festive price: " + service.getFinalAmount(new FestiveDiscount(), amount));
        System.out.println("New Year price: " + service.getFinalAmount(new NewYearDiscount(), amount));
    }
}
