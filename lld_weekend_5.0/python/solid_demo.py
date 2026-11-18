"""
SOLID principles, demonstrated with a bad design followed by a good one.
"""

from abc import ABC, abstractmethod


# ============================================================
# PART 1: BAD DESIGN - VIOLATES SOLID PRINCIPLES
# ============================================================

class BadOrderService:
    """
    SRP Violation: One class handles pricing, payment, notification, invoice.
    OCP Violation: Adding a payment method requires modifying this method.
    DIP Violation: Business logic depends on concrete implementations.
    """

    def place_order(self, customer: str, amount: float, payment_type: str) -> None:
        # 1. Calculate discount
        final_amount = amount
        if amount > 1000:
            final_amount = amount * 0.90

        # 2. Process payment
        if payment_type == "UPI":
            print(f"Paid via UPI: {final_amount}")
        elif payment_type == "CARD":
            print(f"Paid via Card: {final_amount}")
        else:
            raise ValueError("Invalid payment")

        # 3. Send email
        print(f"Email sent to {customer}")

        # 4. Generate invoice
        print(f"Invoice generated: {final_amount}")


# LSP Violation: Subclass cannot safely fulfill its parent's contract.

class BadPaymentMethod:
    def pay(self, amount: float) -> None:
        print("Payment completed")

    def refund(self, amount: float) -> None:
        print("Refund completed")


class BadCashOnDelivery(BadPaymentMethod):
    def refund(self, amount: float) -> None:
        raise NotImplementedError("COD does not support online refunds")


# ISP Violation: Every implementation must define methods it doesn't need.

class BadOrderOperations(ABC):
    @abstractmethod
    def pay(self, amount: float) -> None: ...

    @abstractmethod
    def refund(self, amount: float) -> None: ...

    @abstractmethod
    def send_email(self, customer: str) -> None: ...

    @abstractmethod
    def generate_invoice(self, amount: float) -> None: ...


class BadUPIProcessor(BadOrderOperations):
    def pay(self, amount: float) -> None:
        print("UPI payment")

    def refund(self, amount: float) -> None:
        print("UPI refund")

    def send_email(self, customer: str) -> None:
        raise NotImplementedError

    def generate_invoice(self, amount: float) -> None:
        raise NotImplementedError


# ============================================================
# PART 2: GOOD DESIGN - FOLLOWING SOLID PRINCIPLES
# ============================================================

# --------------------
# S: SINGLE RESPONSIBILITY
# --------------------
# Each class has one primary reason to change.

class DiscountCalculator:
    def calculate(self, amount: float) -> float:
        return amount * 0.90 if amount > 1000 else amount


class InvoiceGenerator:
    def generate(self, amount: float) -> None:
        print(f"Invoice generated: {amount}")


# --------------------
# O: OPEN/CLOSED
# --------------------
# New payment methods can be added without modifying
# the existing order processing logic.

class PaymentMethod(ABC):
    @abstractmethod
    def pay(self, amount: float) -> None: ...


class UPIPayment(PaymentMethod):
    def pay(self, amount: float) -> None:
        print(f"Paid via UPI: {amount}")


class CardPayment(PaymentMethod):
    def pay(self, amount: float) -> None:
        print(f"Paid via Card: {amount}")


# New feature: No changes to OrderService required.

class WalletPayment(PaymentMethod):
    def pay(self, amount: float) -> None:
        print(f"Paid via Wallet: {amount}")


# --------------------
# L: LISKOV SUBSTITUTION
# --------------------
# Every PaymentMethod implementation supports pay().
#
# Refund is NOT forced into PaymentMethod because
# not every payment method supports online refunds.

class RefundablePayment(PaymentMethod):
    @abstractmethod
    def refund(self, amount: float) -> None: ...


class RefundableUPIPayment(RefundablePayment):
    def pay(self, amount: float) -> None:
        print(f"UPI payment: {amount}")

    def refund(self, amount: float) -> None:
        print(f"UPI refund: {amount}")


class CashOnDelivery(PaymentMethod):
    def pay(self, amount: float) -> None:
        print(f"COD order placed: {amount}")


# --------------------
# I: INTERFACE SEGREGATION
# --------------------
# Small, focused interfaces.

class Notifier(ABC):
    @abstractmethod
    def notify_customer(self, customer: str) -> None: ...


class InvoiceService(ABC):
    @abstractmethod
    def generate(self, amount: float) -> None: ...


class EmailNotifier(Notifier):
    def notify_customer(self, customer: str) -> None:
        print(f"Email sent to {customer}")


class SimpleInvoiceService(InvoiceService):
    def __init__(self) -> None:
        self._generator = InvoiceGenerator()

    def generate(self, amount: float) -> None:
        self._generator.generate(amount)


# --------------------
# D: DEPENDENCY INVERSION
# --------------------
# High-level OrderService depends on abstractions,
# not concrete UPI/Card/Email implementations.

class OrderService:
    def __init__(
        self,
        payment: PaymentMethod,
        notifier: Notifier,
        invoice_service: InvoiceService,
        discount_calculator: DiscountCalculator,
    ) -> None:
        self._payment = payment
        self._notifier = notifier
        self._invoice_service = invoice_service
        self._discount_calculator = discount_calculator

    def place_order(self, customer: str, amount: float) -> None:
        final_amount = self._discount_calculator.calculate(amount)
        self._payment.pay(final_amount)
        self._notifier.notify_customer(customer)
        self._invoice_service.generate(final_amount)


# ============================================================
# PART 3: MAIN - RUN BOTH IMPLEMENTATIONS
# ============================================================

def main() -> None:
    print("====== BAD DESIGN ======")
    bad_service = BadOrderService()
    bad_service.place_order("Pradeep", 2000, "UPI")

    print("\n====== GOOD DESIGN ======")
    good_service = OrderService(
        UPIPayment(),
        EmailNotifier(),
        SimpleInvoiceService(),
        DiscountCalculator(),
    )
    good_service.place_order("Pradeep", 2000)

    print("\n====== EXTENDING PAYMENT ======")
    # Switching to Wallet requires no OrderService changes.
    wallet_order = OrderService(
        WalletPayment(),
        EmailNotifier(),
        SimpleInvoiceService(),
        DiscountCalculator(),
    )
    wallet_order.place_order("Rahul", 1500)

    print("\n====== LSP DEMO ======")
    cod = CashOnDelivery()
    cod.pay(500)

    refundable = RefundableUPIPayment()
    refundable.pay(500)
    refundable.refund(500)


if __name__ == "__main__":
    main()
