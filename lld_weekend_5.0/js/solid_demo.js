// SOLID principles, demonstrated with a bad design followed by a good one.

"use strict";

// ============================================================
// PART 1: BAD DESIGN - VIOLATES SOLID PRINCIPLES
// ============================================================

class BadOrderService {
  // SRP Violation: One class handles pricing, payment, notification, invoice.
  // OCP Violation: Adding a payment method requires modifying this method.
  // DIP Violation: Business logic depends on concrete implementations.
  placeOrder(customer, amount, paymentType) {
    // 1. Calculate discount
    let finalAmount = amount;
    if (amount > 1000) {
      finalAmount = amount * 0.9;
    }

    // 2. Process payment
    if (paymentType === "UPI") {
      console.log(`Paid via UPI: ${finalAmount}`);
    } else if (paymentType === "CARD") {
      console.log(`Paid via Card: ${finalAmount}`);
    } else {
      throw new Error("Invalid payment");
    }

    // 3. Send email
    console.log(`Email sent to ${customer}`);

    // 4. Generate invoice
    console.log(`Invoice generated: ${finalAmount}`);
  }
}

// LSP Violation: Subclass cannot safely fulfill its parent's contract.

class BadPaymentMethod {
  pay(_amount) {
    console.log("Payment completed");
  }

  refund(_amount) {
    console.log("Refund completed");
  }
}

class BadCashOnDelivery extends BadPaymentMethod {
  refund(_amount) {
    throw new Error("COD does not support online refunds");
  }
}

// ISP Violation: Every implementation must define methods it doesn't need.

class BadOrderOperations {
  pay(_amount) {
    throw new Error("not implemented");
  }
  refund(_amount) {
    throw new Error("not implemented");
  }
  sendEmail(_customer) {
    throw new Error("not implemented");
  }
  generateInvoice(_amount) {
    throw new Error("not implemented");
  }
}

class BadUPIProcessor extends BadOrderOperations {
  pay(_amount) {
    console.log("UPI payment");
  }

  refund(_amount) {
    console.log("UPI refund");
  }

  sendEmail(_customer) {
    throw new Error("not supported");
  }

  generateInvoice(_amount) {
    throw new Error("not supported");
  }
}

// ============================================================
// PART 2: GOOD DESIGN - FOLLOWING SOLID PRINCIPLES
// ============================================================

// --------------------
// S: SINGLE RESPONSIBILITY
// --------------------
class DiscountCalculator {
  calculate(amount) {
    return amount > 1000 ? amount * 0.9 : amount;
  }
}

class InvoiceGenerator {
  generate(amount) {
    console.log(`Invoice generated: ${amount}`);
  }
}

// --------------------
// O: OPEN/CLOSED
// --------------------
// New payment methods can be added without modifying
// the existing order processing logic.

class PaymentMethod {
  pay(_amount) {
    throw new Error("pay() must be implemented");
  }
}

class UPIPayment extends PaymentMethod {
  pay(amount) {
    console.log(`Paid via UPI: ${amount}`);
  }
}

class CardPayment extends PaymentMethod {
  pay(amount) {
    console.log(`Paid via Card: ${amount}`);
  }
}

// New feature: No changes to OrderService required.
class WalletPayment extends PaymentMethod {
  pay(amount) {
    console.log(`Paid via Wallet: ${amount}`);
  }
}

// --------------------
// L: LISKOV SUBSTITUTION
// --------------------
// Every PaymentMethod implementation supports pay().
// Refund is NOT forced into PaymentMethod because
// not every payment method supports online refunds.

class RefundablePayment extends PaymentMethod {
  refund(_amount) {
    throw new Error("refund() must be implemented");
  }
}

class RefundableUPIPayment extends RefundablePayment {
  pay(amount) {
    console.log(`UPI payment: ${amount}`);
  }

  refund(amount) {
    console.log(`UPI refund: ${amount}`);
  }
}

class CashOnDelivery extends PaymentMethod {
  pay(amount) {
    console.log(`COD order placed: ${amount}`);
  }
}

// --------------------
// I: INTERFACE SEGREGATION
// --------------------
// Small, focused interfaces.

class Notifier {
  notifyCustomer(_customer) {
    throw new Error("notifyCustomer() must be implemented");
  }
}

class InvoiceService {
  generate(_amount) {
    throw new Error("generate() must be implemented");
  }
}

class EmailNotifier extends Notifier {
  notifyCustomer(customer) {
    console.log(`Email sent to ${customer}`);
  }
}

class SimpleInvoiceService extends InvoiceService {
  #generator = new InvoiceGenerator();

  generate(amount) {
    this.#generator.generate(amount);
  }
}

// --------------------
// D: DEPENDENCY INVERSION
// --------------------
// High-level OrderService depends on abstractions,
// not concrete UPI/Card/Email implementations.

class OrderService {
  #payment;
  #notifier;
  #invoiceService;
  #discountCalculator;

  constructor(payment, notifier, invoiceService, discountCalculator) {
    this.#payment = payment;
    this.#notifier = notifier;
    this.#invoiceService = invoiceService;
    this.#discountCalculator = discountCalculator;
  }

  placeOrder(customer, amount) {
    const finalAmount = this.#discountCalculator.calculate(amount);
    this.#payment.pay(finalAmount);
    this.#notifier.notifyCustomer(customer);
    this.#invoiceService.generate(finalAmount);
  }
}

// ============================================================
// PART 3: MAIN - RUN BOTH IMPLEMENTATIONS
// ============================================================

function main() {
  console.log("====== BAD DESIGN ======");
  const badService = new BadOrderService();
  badService.placeOrder("Pradeep", 2000, "UPI");

  console.log("\n====== GOOD DESIGN ======");
  const goodService = new OrderService(
    new UPIPayment(),
    new EmailNotifier(),
    new SimpleInvoiceService(),
    new DiscountCalculator()
  );
  goodService.placeOrder("Pradeep", 2000);

  console.log("\n====== EXTENDING PAYMENT ======");
  // Switching to Wallet requires no OrderService changes.
  const walletOrder = new OrderService(
    new WalletPayment(),
    new EmailNotifier(),
    new SimpleInvoiceService(),
    new DiscountCalculator()
  );
  walletOrder.placeOrder("Rahul", 1500);

  console.log("\n====== LSP DEMO ======");
  const cod = new CashOnDelivery();
  cod.pay(500);

  const refundable = new RefundableUPIPayment();
  refundable.pay(500);
  refundable.refund(500);
}

main();
