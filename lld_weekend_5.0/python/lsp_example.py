"""
TOPIC: Liskov Substitution Principle (the "L" in SOLID) and how a
"fat"/"god" interface (ISP violation) sets you up to break it.

"Subtypes must be substitutable for their base type without altering
the correctness of the program."

Payment below forces every implementation to support pay(), refund()
AND cash_back() -- even payment methods that cannot support all three
in real life. The moment PhonePay raises from refund(), any code
written against the Payment protocol can no longer treat a PhonePay as
a drop-in replacement for a PayTm: that's an LSP violation, and it was
caused by the interface being too fat (an ISP violation).
"""

from abc import ABC, abstractmethod


class Application:
    def __init__(self) -> None:
        self.payment: "Payment" = None

    def set_payment(self, payment: "Payment") -> None:
        self.payment = payment

    def application_ui(self) -> None:
        self.payment.pay()
        self.payment.refund()    # callers can't trust this won't blow up
        self.payment.cash_back()


# God interface / fat interface:
# every implementer is forced to support all three operations, whether
# or not it actually can.
class Payment(ABC):
    @abstractmethod
    def pay(self) -> None: ...

    @abstractmethod
    def refund(self) -> None: ...

    @abstractmethod
    def cash_back(self) -> None: ...


class PhonePay(Payment):
    def pay(self) -> None:
        print("PhonePay: payment successful")

    def refund(self) -> None:
        # PhonePay (in this example) does not support refunds at all --
        # forced to implement a method it cannot honor.
        raise NotImplementedError("PhonePay does not support refunds")

    def cash_back(self) -> None:
        print("PhonePay: cashback credited")


class PayTm(Payment):
    def pay(self) -> None:
        print("PayTm: payment successful")

    def refund(self) -> None:
        print("PayTm: refund processed")

    def cash_back(self) -> None:
        print("PayTm: cashback credited")


# --------------------------------------------------------------------
# THE FIX: split the fat interface into small, focused ones (ISP).
# Now Application can depend on exactly the capability it needs, and
# any implementation of RefundFeature is guaranteed to honor refund()
# -- restoring LSP, because there's no method left to raise from.
# --------------------------------------------------------------------

class CashBackFeature(ABC):
    @abstractmethod
    def cash_back(self) -> None: ...


class RefundFeature(ABC):
    @abstractmethod
    def refund(self) -> None: ...


class SegregatedPayTm(Payment, RefundFeature, CashBackFeature):
    def pay(self) -> None:
        print("PayTm: payment successful")

    def refund(self) -> None:
        print("PayTm: refund processed")

    def cash_back(self) -> None:
        print("PayTm: cashback credited")


def main() -> None:
    print("== LSP violation via fat interface ==")
    app = Application()
    app.set_payment(PayTm())
    app.application_ui()

    app.set_payment(PhonePay())
    try:
        app.application_ui()  # blows up on refund() -- PhonePay is NOT
                               # truly substitutable for PayTm here.
    except NotImplementedError as e:
        print(f"LSP violated: {e}")

    print("\n== After segregating the interface (ISP) ==")
    refundable: RefundFeature = SegregatedPayTm()
    refundable.refund()  # guaranteed to work: only refundable types implement this


if __name__ == "__main__":
    main()
