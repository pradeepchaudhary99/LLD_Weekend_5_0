import java.util.*;

// ============================================================
// PART 1: BAD DESIGN - VIOLATES SOLID PRINCIPLES
// ============================================================

class BadOrderService {

    // SRP Violation:
    // One class handles pricing, payment, notification, invoice.

    // OCP Violation:
    // Adding a payment method requires modifying this method.

    // DIP Violation:
    // Business logic depends on concrete implementations.

    public void placeOrder(String customer,
                           double amount,
                           String paymentType) {

        // 1. Calculate discount
        double finalAmount = amount;

        if (amount > 1000) {
            finalAmount = amount * 0.90;
        }

        // 2. Process payment
        if (paymentType.equals("UPI")) {
            System.out.println("Paid via UPI: " + finalAmount);
        } else if (paymentType.equals("CARD")) {
            System.out.println("Paid via Card: " + finalAmount);
        } else {
            throw new IllegalArgumentException("Invalid payment");
        }

        // 3. Send email
        System.out.println("Email sent to " + customer);

        // 4. Generate invoice
        System.out.println("Invoice generated: " + finalAmount);
    }
}

// LSP Violation:
// Subclass cannot safely fulfill its parent's contract.

class BadPaymentMethod {
    public void pay(double amount) {
        System.out.println("Payment completed");
    }

    public void refund(double amount) {
        System.out.println("Refund completed");
    }
}

class BadCashOnDelivery extends BadPaymentMethod {
    @Override
    public void refund(double amount) {
        throw new UnsupportedOperationException(
            "COD does not support online refunds"
        );
    }
}

// ISP Violation:
// Every implementation must define methods it doesn't need.

interface BadOrderOperations {
    void pay(double amount);
    void refund(double amount);
    void sendEmail(String customer);
    void generateInvoice(double amount);
}

class BadUPIProcessor implements BadOrderOperations {
    public void pay(double amount) {
        System.out.println("UPI payment");
    }

    public void refund(double amount) {
        System.out.println("UPI refund");
    }

    public void sendEmail(String customer) {
        throw new UnsupportedOperationException();
    }

    public void generateInvoice(double amount) {
        throw new UnsupportedOperationException();
    }
}


// ============================================================
// PART 2: GOOD DESIGN - FOLLOWING SOLID PRINCIPLES
// ============================================================

// --------------------
// S: SINGLE RESPONSIBILITY
// --------------------
// Each class has one primary reason to change.

class DiscountCalculator {
    public double calculate(double amount) {
        return amount > 1000 ? amount * 0.90 : amount;
    }
}

class InvoiceGenerator {
    public void generate(double amount) {
        System.out.println("Invoice generated: " + amount);
    }
}


// --------------------
// O: OPEN/CLOSED
// --------------------
// New payment methods can be added without modifying
// the existing order processing logic.

interface PaymentMethod {
    void pay(double amount);
}

class UPIPayment implements PaymentMethod {
    public void pay(double amount) {
        System.out.println("Paid via UPI: " + amount);
    }
}

class CardPayment implements PaymentMethod {
    public void pay(double amount) {
        System.out.println("Paid via Card: " + amount);
    }
}

// New feature: No changes to OrderService required.

class WalletPayment implements PaymentMethod {
    public void pay(double amount) {
        System.out.println("Paid via Wallet: " + amount);
    }
}


// --------------------
// L: LISKOV SUBSTITUTION
// --------------------
// Every PaymentMethod implementation supports pay().
//
// Refund is NOT forced into PaymentMethod because
// not every payment method supports online refunds.

interface RefundablePayment extends PaymentMethod {
    void refund(double amount);
}

class RefundableUPIPayment implements RefundablePayment {
    public void pay(double amount) {
        System.out.println("UPI payment: " + amount);
    }

    public void refund(double amount) {
        System.out.println("UPI refund: " + amount);
    }
}

class CashOnDelivery implements PaymentMethod {
    public void pay(double amount) {
        System.out.println("COD order placed: " + amount);
    }
}


// --------------------
// I: INTERFACE SEGREGATION
// --------------------
// Small, focused interfaces.

interface Notifier {
    void notifyCustomer(String customer);
}

interface InvoiceService {
    void generate(double amount);
}

class EmailNotifier implements Notifier {
    public void notifyCustomer(String customer) {
        System.out.println("Email sent to " + customer);
    }
}

class SimpleInvoiceService implements InvoiceService {
    private final InvoiceGenerator generator =
        new InvoiceGenerator();

    public void generate(double amount) {
        generator.generate(amount);
    }
}


// --------------------
// D: DEPENDENCY INVERSION
// --------------------
// High-level OrderService depends on abstractions,
// not concrete UPI/Card/Email implementations.

class OrderService {

    private final PaymentMethod payment;
    private final Notifier notifier;
    private final InvoiceService invoiceService;
    private final DiscountCalculator discountCalculator;

    public OrderService(PaymentMethod payment,
                        Notifier notifier,
                        InvoiceService invoiceService,
                        DiscountCalculator discountCalculator) {
        this.payment = payment;
        this.notifier = notifier;
        this.invoiceService = invoiceService;
        this.discountCalculator = discountCalculator;
    }

    public void placeOrder(String customer, double amount) {

        double finalAmount = discountCalculator.calculate(amount);

        payment.pay(finalAmount);

        notifier.notifyCustomer(customer);

        invoiceService.generate(finalAmount);
    }
}


// ============================================================
// PART 3: MAIN - RUN BOTH IMPLEMENTATIONS
// ============================================================

public class SOLIDDemo {

    public static void main(String[] args) {

        System.out.println("====== BAD DESIGN ======");

        BadOrderService badService = new BadOrderService();
        badService.placeOrder("Pradeep", 2000, "UPI");

        System.out.println("\n====== GOOD DESIGN ======");

        OrderService goodService = new OrderService(
            new UPIPayment(),
            new EmailNotifier(),
            new SimpleInvoiceService(),
            new DiscountCalculator()
        );

        goodService.placeOrder("Pradeep", 2000);

        System.out.println("\n====== EXTENDING PAYMENT ======");

        // Switching to Wallet requires no OrderService changes.
        OrderService walletOrder = new OrderService(
            new WalletPayment(),
            new EmailNotifier(),
            new SimpleInvoiceService(),
            new DiscountCalculator()
        );

        walletOrder.placeOrder("Rahul", 1500);

        System.out.println("\n====== LSP DEMO ======");

        PaymentMethod cod = new CashOnDelivery();
        cod.pay(500);

        RefundablePayment refundable = new RefundableUPIPayment();
        refundable.pay(500);
        refundable.refund(500);
    }
}
