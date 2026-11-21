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
// written against the Payment "interface" can no longer treat a
// PhonePay as a drop-in replacement for a PayTm: that's an LSP
// violation, and it was caused by the interface being too fat (an ISP
// violation). JS has no compiler-enforced interfaces, so we enforce
// the contract by throwing from the base class's stub methods.
// =====================================================================

"use strict";

class Application {
  setPayment(payment) {
    this.payment = payment;
  }

  applicationUI() {
    this.payment.pay();
    this.payment.refund();    // callers can't trust this won't throw
    this.payment.cashBack();
  }
}

// God interface / fat interface:
// every implementer is forced to support all three operations, whether
// or not it actually can.
class Payment {
  pay() {
    throw new Error("pay() must be implemented");
  }
  refund() {
    throw new Error("refund() must be implemented");
  }
  cashBack() {
    throw new Error("cashBack() must be implemented");
  }
}

class PhonePay extends Payment {
  pay() {
    console.log("PhonePay: payment successful");
  }

  refund() {
    // PhonePay (in this example) does not support refunds at all --
    // forced to implement a method it cannot honor.
    throw new Error("PhonePay does not support refunds");
  }

  cashBack() {
    console.log("PhonePay: cashback credited");
  }
}

class PayTm extends Payment {
  pay() {
    console.log("PayTm: payment successful");
  }

  refund() {
    console.log("PayTm: refund processed");
  }

  cashBack() {
    console.log("PayTm: cashback credited");
  }
}

// --------------------------------------------------------------------
// THE FIX: split the fat interface into small, focused "roles" (ISP).
// Now Application can depend on exactly the capability it needs, and
// any class fulfilling the RefundFeature role is guaranteed to honor
// refund() -- restoring LSP, because there's no method left to throw
// from.
// --------------------------------------------------------------------

class SegregatedPayTm {
  pay() {
    console.log("PayTm: payment successful");
  }

  refund() {
    console.log("PayTm: refund processed");
  }

  cashBack() {
    console.log("PayTm: cashback credited");
  }
}

function main() {
  console.log("== LSP violation via fat interface ==");
  const app = new Application();
  app.setPayment(new PayTm());
  app.applicationUI();

  app.setPayment(new PhonePay());
  try {
    app.applicationUI(); // blows up on refund() -- PhonePay is NOT
                          // truly substitutable for PayTm here.
  } catch (e) {
    console.log(`LSP violated: ${e.message}`);
  }

  console.log("\n== After segregating the interface (ISP) ==");
  const refundable = new SegregatedPayTm();
  refundable.refund(); // guaranteed to work: only refundable types implement this
}

main();
