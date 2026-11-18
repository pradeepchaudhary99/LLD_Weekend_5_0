// =====================================================================
// TOPIC: Open/Closed Principle (the "O" in SOLID)
//
// "Software entities should be open for EXTENSION but closed for
// MODIFICATION."
//
// DiscountService never changes when a new discount type is added --
// we just write a new class that implements IDiscount.
// =====================================================================

using System;

namespace LldWeekend.OcpDemo
{
    public interface IDiscount
    {
        int CalculateDiscount(int amount);
    }

    public class DiwaliDiscount : IDiscount
    {
        public int CalculateDiscount(int amount) =>
            amount - (amount * 20 / 100); // 20% off during Diwali
    }

    public class HoliDiscount : IDiscount
    {
        public int CalculateDiscount(int amount) =>
            amount - (amount * 10 / 100); // 10% off during Holi
    }

    public class FestiveDiscount : IDiscount
    {
        public int CalculateDiscount(int amount) =>
            amount - (amount * 5 / 100); // Flat 5% for any other festive season
    }

    // Adding NewYearDiscount tomorrow needs ZERO changes below this line --
    // that is exactly what "open for extension, closed for modification" means.
    public class NewYearDiscount : IDiscount
    {
        public int CalculateDiscount(int amount) => amount - (amount * 15 / 100);
    }

    public class DiscountService
    {
        public double GetFinalAmount(IDiscount discount, int amount) => discount.CalculateDiscount(amount);
    }

    public static class OcpDemoProgram
    {
        public static void Main()
        {
            var service = new DiscountService();
            int amount = 1000;

            Console.WriteLine($"Diwali price: {service.GetFinalAmount(new DiwaliDiscount(), amount)}");
            Console.WriteLine($"Holi price: {service.GetFinalAmount(new HoliDiscount(), amount)}");
            Console.WriteLine($"Festive price: {service.GetFinalAmount(new FestiveDiscount(), amount)}");
            Console.WriteLine($"New Year price: {service.GetFinalAmount(new NewYearDiscount(), amount)}");
        }
    }
}
