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

#include <iostream>
#include <memory>
#include <stdexcept>

class Payment {
public:
    virtual ~Payment() = default;
    virtual void pay() = 0;
    virtual void refund() = 0;
    virtual void cashBack() = 0;
};

class Application {
public:
    void setPayment(std::shared_ptr<Payment> payment) {
        payment_ = std::move(payment);
    }

    void applicationUI() {
        payment_->pay();
        payment_->refund();   // callers can't trust this won't throw
        payment_->cashBack();
    }

private:
    std::shared_ptr<Payment> payment_;
};

class PhonePay : public Payment {
public:
    void pay() override { std::cout << "PhonePay: payment successful\n"; }

    void refund() override {
        // PhonePay (in this example) does not support refunds at all --
        // forced to implement a method it cannot honor.
        throw std::logic_error("PhonePay does not support refunds");
    }

    void cashBack() override { std::cout << "PhonePay: cashback credited\n"; }
};

class PayTm : public Payment {
public:
    void pay() override { std::cout << "PayTm: payment successful\n"; }
    void refund() override { std::cout << "PayTm: refund processed\n"; }
    void cashBack() override { std::cout << "PayTm: cashback credited\n"; }
};

// --------------------------------------------------------------------
// THE FIX: split the fat interface into small, focused ones (ISP).
// Now Application can depend on exactly the capability it needs, and
// any implementation of RefundFeature is guaranteed to honor refund()
// -- restoring LSP, because there's no method left to throw from.
// --------------------------------------------------------------------

class CashBackFeature {
public:
    virtual ~CashBackFeature() = default;
    virtual void cashBack() = 0;
};

class RefundFeature {
public:
    virtual ~RefundFeature() = default;
    virtual void refund() = 0;
};

class SegregatedPayTm : public Payment, public RefundFeature, public CashBackFeature {
public:
    void pay() override { std::cout << "PayTm: payment successful\n"; }
    void refund() override { std::cout << "PayTm: refund processed\n"; }
    void cashBack() override { std::cout << "PayTm: cashback credited\n"; }
};

int main() {
    std::cout << "== LSP violation via fat interface ==\n";
    Application app;
    app.setPayment(std::make_shared<PayTm>());
    app.applicationUI();

    app.setPayment(std::make_shared<PhonePay>());
    try {
        app.applicationUI(); // blows up on refund() -- PhonePay is NOT
                              // truly substitutable for PayTm here.
    } catch (const std::logic_error& e) {
        std::cout << "LSP violated: " << e.what() << "\n";
    }

    std::cout << "\n== After segregating the interface (ISP) ==\n";
    std::shared_ptr<RefundFeature> refundable = std::make_shared<SegregatedPayTm>();
    refundable->refund(); // guaranteed to work: only refundable types implement this

    return 0;
}
