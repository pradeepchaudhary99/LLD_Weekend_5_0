// =====================================================================
// TOPIC: Liskov Substitution Principle (the "L" in SOLID) and how a
// "fat"/"god" interface (ISP violation) sets you up to break it.
//
// "Subtypes must be substitutable for their base type without
// altering the correctness of the program."
//
// Payment below forces every implementation to support pay(), refund()
// AND cashBack() -- even payment methods that cannot support all three
// in real life. The moment PhonePay throws from refund(), any code
// written against the Payment interface can no longer treat a PhonePay
// as a drop-in replacement for a PayTm: that's an LSP violation, and it
// was caused by the interface being too fat (an ISP violation).
// =====================================================================

class Application {
    Payment payment;

    void setPayment(Payment payment) {
        this.payment = payment;
    }

    void applicationUI() {
        payment.pay();
        payment.refund();   // callers can't trust this won't blow up
        payment.cashBack();
    }
}

// God interface / fat interface:
// every implementer is forced to support all three operations, whether
// or not it actually can.
interface Payment {
    void pay();
    void refund();
    void cashBack();
}

class PhonePay implements Payment {

    @Override
    public void pay() {
        System.out.println("PhonePay: payment successful");
    }

    @Override
    public void refund() {
        // PhonePay (in this example) does not support refunds at all --
        // forced to implement a method it cannot honor.
        throw new UnsupportedOperationException("PhonePay does not support refunds");
    }

    @Override
    public void cashBack() {
        System.out.println("PhonePay: cashback credited");
    }
}

class PayTm implements Payment {

    @Override
    public void pay() {
        System.out.println("PayTm: payment successful");
    }

    @Override
    public void refund() {
        System.out.println("PayTm: refund processed");
    }

    @Override
    public void cashBack() {
        System.out.println("PayTm: cashback credited");
    }
}

// --------------------------------------------------------------------
// THE FIX: split the fat interface into small, focused ones (ISP).
// Now Application can depend on exactly the capability it needs, and
// any implementation of RefundFeature is guaranteed to honor refund()
// -- restoring LSP, because there's no method left to throw from.
// --------------------------------------------------------------------

interface CashBackFeature {
    void cashBack();
}

interface RefundFeature {
    void refund();
}

class SegregatedPayTm implements Payment, RefundFeature, CashBackFeature {
    @Override
    public void pay() {
        System.out.println("PayTm: payment successful");
    }

    @Override
    public void refund() {
        System.out.println("PayTm: refund processed");
    }

    @Override
    public void cashBack() {
        System.out.println("PayTm: cashback credited");
    }
}

// PhonePay now simply never implements RefundFeature -- there is no
// method left to throw UnsupportedOperationException from, and every
// RefundFeature reference is truly substitutable.
class SegregatedPhonePay implements Payment, CashBackFeature {
    @Override
    public void pay() {
        System.out.println("PhonePay: payment successful");
    }

    @Override
    public void refund() {
        throw new UnsupportedOperationException("PhonePay does not support refunds");
    }

    @Override
    public void cashBack() {
        System.out.println("PhonePay: cashback credited");
    }
}

public class LSPExample {
    public static void main(String[] args) {
        System.out.println("== LSP violation via fat interface ==");
        Application app = new Application();
        app.setPayment(new PayTm());
        app.applicationUI();

        app.setPayment(new PhonePay());
        try {
            app.applicationUI(); // blows up on refund() -- PhonePay is NOT
                                  // truly substitutable for PayTm here.
        } catch (UnsupportedOperationException e) {
            System.out.println("LSP violated: " + e.getMessage());
        }

        System.out.println("\n== After segregating the interface (ISP) ==");
        RefundFeature refundable = new SegregatedPayTm();
        refundable.refund(); // guaranteed to work: only refundable types implement this
    }
}
