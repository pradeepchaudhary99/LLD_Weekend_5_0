// SOLID principles, demonstrated with a bad design followed by a good one.

using System;

namespace LldWeekend.SolidDemo
{
    // ============================================================
    // PART 1: BAD DESIGN - VIOLATES SOLID PRINCIPLES
    // ============================================================

    public class BadOrderService
    {
        // SRP Violation: One class handles pricing, payment, notification, invoice.
        // OCP Violation: Adding a payment method requires modifying this method.
        // DIP Violation: Business logic depends on concrete implementations.
        public void PlaceOrder(string customer, double amount, string paymentType)
        {
            // 1. Calculate discount
            double finalAmount = amount;
            if (amount > 1000) finalAmount = amount * 0.90;

            // 2. Process payment
            if (paymentType == "UPI")
            {
                Console.WriteLine($"Paid via UPI: {finalAmount}");
            }
            else if (paymentType == "CARD")
            {
                Console.WriteLine($"Paid via Card: {finalAmount}");
            }
            else
            {
                throw new ArgumentException("Invalid payment");
            }

            // 3. Send email
            Console.WriteLine($"Email sent to {customer}");

            // 4. Generate invoice
            Console.WriteLine($"Invoice generated: {finalAmount}");
        }
    }

    // LSP Violation: Subclass cannot safely fulfill its parent's contract.

    public class BadPaymentMethod
    {
        public virtual void Pay(double amount) => Console.WriteLine("Payment completed");
        public virtual void Refund(double amount) => Console.WriteLine("Refund completed");
    }

    public class BadCashOnDelivery : BadPaymentMethod
    {
        public override void Refund(double amount) =>
            throw new NotSupportedException("COD does not support online refunds");
    }

    // ISP Violation: Every implementation must define methods it doesn't need.

    public interface IBadOrderOperations
    {
        void Pay(double amount);
        void Refund(double amount);
        void SendEmail(string customer);
        void GenerateInvoice(double amount);
    }

    public class BadUpiProcessor : IBadOrderOperations
    {
        public void Pay(double amount) => Console.WriteLine("UPI payment");
        public void Refund(double amount) => Console.WriteLine("UPI refund");
        public void SendEmail(string customer) => throw new NotImplementedException();
        public void GenerateInvoice(double amount) => throw new NotImplementedException();
    }

    // ============================================================
    // PART 2: GOOD DESIGN - FOLLOWING SOLID PRINCIPLES
    // ============================================================

    // --------------------
    // S: SINGLE RESPONSIBILITY
    // --------------------
    public class DiscountCalculator
    {
        public double Calculate(double amount) => amount > 1000 ? amount * 0.90 : amount;
    }

    public class InvoiceGenerator
    {
        public void Generate(double amount) => Console.WriteLine($"Invoice generated: {amount}");
    }

    // --------------------
    // O: OPEN/CLOSED
    // --------------------
    // New payment methods can be added without modifying
    // the existing order processing logic.

    public interface IPaymentMethod
    {
        void Pay(double amount);
    }

    public class UpiPayment : IPaymentMethod
    {
        public void Pay(double amount) => Console.WriteLine($"Paid via UPI: {amount}");
    }

    public class CardPayment : IPaymentMethod
    {
        public void Pay(double amount) => Console.WriteLine($"Paid via Card: {amount}");
    }

    // New feature: No changes to OrderService required.
    public class WalletPayment : IPaymentMethod
    {
        public void Pay(double amount) => Console.WriteLine($"Paid via Wallet: {amount}");
    }

    // --------------------
    // L: LISKOV SUBSTITUTION
    // --------------------
    // Every IPaymentMethod implementation supports Pay().
    // Refund is NOT forced into IPaymentMethod because
    // not every payment method supports online refunds.

    public interface IRefundablePayment : IPaymentMethod
    {
        void Refund(double amount);
    }

    public class RefundableUpiPayment : IRefundablePayment
    {
        public void Pay(double amount) => Console.WriteLine($"UPI payment: {amount}");
        public void Refund(double amount) => Console.WriteLine($"UPI refund: {amount}");
    }

    public class CashOnDelivery : IPaymentMethod
    {
        public void Pay(double amount) => Console.WriteLine($"COD order placed: {amount}");
    }

    // --------------------
    // I: INTERFACE SEGREGATION
    // --------------------
    // Small, focused interfaces.

    public interface INotifier
    {
        void NotifyCustomer(string customer);
    }

    public interface IInvoiceService
    {
        void Generate(double amount);
    }

    public class EmailNotifier : INotifier
    {
        public void NotifyCustomer(string customer) => Console.WriteLine($"Email sent to {customer}");
    }

    public class SimpleInvoiceService : IInvoiceService
    {
        private readonly InvoiceGenerator _generator = new();

        public void Generate(double amount) => _generator.Generate(amount);
    }

    // --------------------
    // D: DEPENDENCY INVERSION
    // --------------------
    // High-level OrderService depends on abstractions,
    // not concrete UPI/Card/Email implementations.

    public class OrderService
    {
        private readonly IPaymentMethod _payment;
        private readonly INotifier _notifier;
        private readonly IInvoiceService _invoiceService;
        private readonly DiscountCalculator _discountCalculator;

        public OrderService(IPaymentMethod payment, INotifier notifier,
            IInvoiceService invoiceService, DiscountCalculator discountCalculator)
        {
            _payment = payment;
            _notifier = notifier;
            _invoiceService = invoiceService;
            _discountCalculator = discountCalculator;
        }

        public void PlaceOrder(string customer, double amount)
        {
            double finalAmount = _discountCalculator.Calculate(amount);
            _payment.Pay(finalAmount);
            _notifier.NotifyCustomer(customer);
            _invoiceService.Generate(finalAmount);
        }
    }

    // ============================================================
    // PART 3: MAIN - RUN BOTH IMPLEMENTATIONS
    // ============================================================

    public static class SolidDemoProgram
    {
        public static void Main()
        {
            Console.WriteLine("====== BAD DESIGN ======");
            var badService = new BadOrderService();
            badService.PlaceOrder("Pradeep", 2000, "UPI");

            Console.WriteLine("\n====== GOOD DESIGN ======");
            var goodService = new OrderService(
                new UpiPayment(),
                new EmailNotifier(),
                new SimpleInvoiceService(),
                new DiscountCalculator());
            goodService.PlaceOrder("Pradeep", 2000);

            Console.WriteLine("\n====== EXTENDING PAYMENT ======");
            // Switching to Wallet requires no OrderService changes.
            var walletOrder = new OrderService(
                new WalletPayment(),
                new EmailNotifier(),
                new SimpleInvoiceService(),
                new DiscountCalculator());
            walletOrder.PlaceOrder("Rahul", 1500);

            Console.WriteLine("\n====== LSP DEMO ======");
            IPaymentMethod cod = new CashOnDelivery();
            cod.Pay(500);

            var refundable = new RefundableUpiPayment();
            refundable.Pay(500);
            refundable.Refund(500);
        }
    }
}
