// SOLID principles, demonstrated with a bad design followed by a good one.

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// ============================================================
// PART 1: BAD DESIGN - VIOLATES SOLID PRINCIPLES
// ============================================================

class BadOrderService {
public:
    // SRP Violation: One class handles pricing, payment, notification, invoice.
    // OCP Violation: Adding a payment method requires modifying this method.
    // DIP Violation: Business logic depends on concrete implementations.
    void placeOrder(const std::string& customer, double amount, const std::string& paymentType) {
        // 1. Calculate discount
        double finalAmount = amount;
        if (amount > 1000) {
            finalAmount = amount * 0.90;
        }

        // 2. Process payment
        if (paymentType == "UPI") {
            std::cout << "Paid via UPI: " << finalAmount << "\n";
        } else if (paymentType == "CARD") {
            std::cout << "Paid via Card: " << finalAmount << "\n";
        } else {
            throw std::invalid_argument("Invalid payment");
        }

        // 3. Send email
        std::cout << "Email sent to " << customer << "\n";

        // 4. Generate invoice
        std::cout << "Invoice generated: " << finalAmount << "\n";
    }
};

// LSP Violation: Subclass cannot safely fulfill its parent's contract.

class BadPaymentMethod {
public:
    virtual ~BadPaymentMethod() = default;
    virtual void pay(double) { std::cout << "Payment completed\n"; }
    virtual void refund(double) { std::cout << "Refund completed\n"; }
};

class BadCashOnDelivery : public BadPaymentMethod {
public:
    void refund(double) override {
        throw std::logic_error("COD does not support online refunds");
    }
};

// ISP Violation: Every implementation must define methods it doesn't need.

class BadOrderOperations {
public:
    virtual ~BadOrderOperations() = default;
    virtual void pay(double amount) = 0;
    virtual void refund(double amount) = 0;
    virtual void sendEmail(const std::string& customer) = 0;
    virtual void generateInvoice(double amount) = 0;
};

class BadUPIProcessor : public BadOrderOperations {
public:
    void pay(double) override { std::cout << "UPI payment\n"; }
    void refund(double) override { std::cout << "UPI refund\n"; }
    void sendEmail(const std::string&) override { throw std::logic_error("not supported"); }
    void generateInvoice(double) override { throw std::logic_error("not supported"); }
};

// ============================================================
// PART 2: GOOD DESIGN - FOLLOWING SOLID PRINCIPLES
// ============================================================

// --------------------
// S: SINGLE RESPONSIBILITY
// --------------------
class DiscountCalculator {
public:
    double calculate(double amount) const {
        return amount > 1000 ? amount * 0.90 : amount;
    }
};

class InvoiceGenerator {
public:
    void generate(double amount) const {
        std::cout << "Invoice generated: " << amount << "\n";
    }
};

// --------------------
// O: OPEN/CLOSED
// --------------------
// New payment methods can be added without modifying
// the existing order processing logic.

class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    virtual void pay(double amount) = 0;
};

class UPIPayment : public PaymentMethod {
public:
    void pay(double amount) override { std::cout << "Paid via UPI: " << amount << "\n"; }
};

class CardPayment : public PaymentMethod {
public:
    void pay(double amount) override { std::cout << "Paid via Card: " << amount << "\n"; }
};

// New feature: No changes to OrderService required.
class WalletPayment : public PaymentMethod {
public:
    void pay(double amount) override { std::cout << "Paid via Wallet: " << amount << "\n"; }
};

// --------------------
// L: LISKOV SUBSTITUTION
// --------------------
// Every PaymentMethod implementation supports pay().
// Refund is NOT forced into PaymentMethod because
// not every payment method supports online refunds.

class RefundablePayment : public PaymentMethod {
public:
    virtual void refund(double amount) = 0;
};

class RefundableUPIPayment : public RefundablePayment {
public:
    void pay(double amount) override { std::cout << "UPI payment: " << amount << "\n"; }
    void refund(double amount) override { std::cout << "UPI refund: " << amount << "\n"; }
};

class CashOnDelivery : public PaymentMethod {
public:
    void pay(double amount) override { std::cout << "COD order placed: " << amount << "\n"; }
};

// --------------------
// I: INTERFACE SEGREGATION
// --------------------
// Small, focused interfaces.

class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void notifyCustomer(const std::string& customer) = 0;
};

class InvoiceService {
public:
    virtual ~InvoiceService() = default;
    virtual void generate(double amount) = 0;
};

class EmailNotifier : public Notifier {
public:
    void notifyCustomer(const std::string& customer) override {
        std::cout << "Email sent to " << customer << "\n";
    }
};

class SimpleInvoiceService : public InvoiceService {
public:
    void generate(double amount) override {
        generator_.generate(amount);
    }

private:
    InvoiceGenerator generator_;
};

// --------------------
// D: DEPENDENCY INVERSION
// --------------------
// High-level OrderService depends on abstractions,
// not concrete UPI/Card/Email implementations.

class OrderService {
public:
    OrderService(std::shared_ptr<PaymentMethod> payment,
                  std::shared_ptr<Notifier> notifier,
                  std::shared_ptr<InvoiceService> invoiceService,
                  std::shared_ptr<DiscountCalculator> discountCalculator)
        : payment_(std::move(payment)),
          notifier_(std::move(notifier)),
          invoiceService_(std::move(invoiceService)),
          discountCalculator_(std::move(discountCalculator)) {}

    void placeOrder(const std::string& customer, double amount) {
        double finalAmount = discountCalculator_->calculate(amount);
        payment_->pay(finalAmount);
        notifier_->notifyCustomer(customer);
        invoiceService_->generate(finalAmount);
    }

private:
    std::shared_ptr<PaymentMethod> payment_;
    std::shared_ptr<Notifier> notifier_;
    std::shared_ptr<InvoiceService> invoiceService_;
    std::shared_ptr<DiscountCalculator> discountCalculator_;
};

// ============================================================
// PART 3: MAIN - RUN BOTH IMPLEMENTATIONS
// ============================================================

int main() {
    std::cout << "====== BAD DESIGN ======\n";
    BadOrderService badService;
    badService.placeOrder("Pradeep", 2000, "UPI");

    std::cout << "\n====== GOOD DESIGN ======\n";
    OrderService goodService(
        std::make_shared<UPIPayment>(),
        std::make_shared<EmailNotifier>(),
        std::make_shared<SimpleInvoiceService>(),
        std::make_shared<DiscountCalculator>());
    goodService.placeOrder("Pradeep", 2000);

    std::cout << "\n====== EXTENDING PAYMENT ======\n";
    // Switching to Wallet requires no OrderService changes.
    OrderService walletOrder(
        std::make_shared<WalletPayment>(),
        std::make_shared<EmailNotifier>(),
        std::make_shared<SimpleInvoiceService>(),
        std::make_shared<DiscountCalculator>());
    walletOrder.placeOrder("Rahul", 1500);

    std::cout << "\n====== LSP DEMO ======\n";
    CashOnDelivery cod;
    cod.pay(500);

    RefundableUPIPayment refundable;
    refundable.pay(500);
    refundable.refund(500);

    return 0;
}
