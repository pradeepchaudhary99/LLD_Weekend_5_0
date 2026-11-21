// =====================================================================
// TOPIC: Liskov Substitution Principle (the "L" in SOLID) and how a
// "fat"/"god" interface (ISP violation) sets you up to break it.
//
// "Subtypes must be substitutable for their base type without
// altering the correctness of the program."
//
// IPayment below forces every implementation to support Pay(),
// Refund() AND CashBack() -- even payment methods that cannot support
// all three in real life. The moment PhonePay throws from Refund(),
// any code written against IPayment can no longer treat a PhonePay as
// a drop-in replacement for a PayTm: that's an LSP violation, caused
// by the interface being too fat (an ISP violation).
// =====================================================================

using System;

namespace LldWeekend.LspExample
{
    public class Application
    {
        private IPayment _payment;

        public void SetPayment(IPayment payment) => _payment = payment;

        public void ApplicationUi()
        {
            _payment.Pay();
            _payment.Refund();   // callers can't trust this won't throw
            _payment.CashBack();
        }
    }

    // God interface / fat interface:
    // every implementer is forced to support all three operations,
    // whether or not it actually can.
    public interface IPayment
    {
        void Pay();
        void Refund();
        void CashBack();
    }

    public class PhonePay : IPayment
    {
        public void Pay() => Console.WriteLine("PhonePay: payment successful");

        public void Refund() =>
            // PhonePay (in this example) does not support refunds at all --
            // forced to implement a method it cannot honor.
            throw new NotSupportedException("PhonePay does not support refunds");

        public void CashBack() => Console.WriteLine("PhonePay: cashback credited");
    }

    public class PayTm : IPayment
    {
        public void Pay() => Console.WriteLine("PayTm: payment successful");
        public void Refund() => Console.WriteLine("PayTm: refund processed");
        public void CashBack() => Console.WriteLine("PayTm: cashback credited");
    }

    // --------------------------------------------------------------------
    // THE FIX: split the fat interface into small, focused ones (ISP).
    // Now Application can depend on exactly the capability it needs, and
    // any implementation of IRefundFeature is guaranteed to honor
    // Refund() -- restoring LSP, because there's no method left to throw
    // from.
    // --------------------------------------------------------------------

    public interface ICashBackFeature
    {
        void CashBack();
    }

    public interface IRefundFeature
    {
        void Refund();
    }

    public class SegregatedPayTm : IPayment, IRefundFeature, ICashBackFeature
    {
        public void Pay() => Console.WriteLine("PayTm: payment successful");
        public void Refund() => Console.WriteLine("PayTm: refund processed");
        public void CashBack() => Console.WriteLine("PayTm: cashback credited");
    }

    public static class LspExampleProgram
    {
        public static void Main()
        {
            Console.WriteLine("== LSP violation via fat interface ==");
            var app = new Application();
            app.SetPayment(new PayTm());
            app.ApplicationUi();

            app.SetPayment(new PhonePay());
            try
            {
                app.ApplicationUi(); // blows up on Refund() -- PhonePay is NOT
                                      // truly substitutable for PayTm here.
            }
            catch (NotSupportedException e)
            {
                Console.WriteLine($"LSP violated: {e.Message}");
            }

            Console.WriteLine("\n== After segregating the interface (ISP) ==");
            IRefundFeature refundable = new SegregatedPayTm();
            refundable.Refund(); // guaranteed to work: only refundable types implement this
        }
    }
}
