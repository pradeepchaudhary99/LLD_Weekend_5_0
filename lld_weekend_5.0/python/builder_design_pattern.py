"""
TOPIC: Builder Design Pattern

Solves the "telescoping constructor" problem (too many constructor
overloads for optional fields) by building the object step by step
through a fluent, chainable API, and only assembling the real object
at the very end via build().

Two properties worth noticing:
 - CHAINING: every UserBuilder setter returns self, so calls can be
   strung together: UserBuilder(...).set_x(..).set_y(..).build()
 - IMMUTABILITY: once User is built, its name/roll_number/marks are
   never reassigned -- the object can't drift out of a valid state
   after construction. (address is left mutable on purpose, to show
   the contrast -- see set_address below.)
"""

from typing import List, Optional


class User:
    def __init__(self, builder: "User.UserBuilder") -> None:
        self._name = builder.name
        self._roll_number = builder.roll_number
        self._address = builder.address  # mutable on purpose
        self._marks = builder.marks

    def set_address(self, address: str) -> None:
        # Only address is allowed to change after construction.
        self._address = address

    def __repr__(self) -> str:
        return (f"User(name={self._name}, roll_number={self._roll_number}, "
                f"address={self._address}, marks={self._marks})")

    class UserBuilder:
        def __init__(self, name: str) -> None:
            self.name = name                        # required
            self.roll_number: int = 0                # optional
            self.address: Optional[str] = None       # optional
            self.marks: Optional[List[int]] = None   # optional

        def set_roll_number(self, roll_number: int) -> "User.UserBuilder":
            self.roll_number = roll_number
            return self  # enables chaining

        def set_address(self, address: str) -> "User.UserBuilder":
            self.address = address
            return self

        def set_marks(self, marks: List[int]) -> "User.UserBuilder":
            self.marks = marks
            return self

        def build(self) -> "User":
            return User(self)


def main() -> None:
    marks = [90, 85]

    user = (
        User.UserBuilder("Pradeep")
        .set_address("Bangalore")
        .set_roll_number(123)
        .set_marks(marks)
        .build()
    )

    print(user)

    # address is still mutable after construction:
    user.set_address("Mumbai")
    print(f"After moving: {user}")


if __name__ == "__main__":
    main()

# Builder:
# - Chaining -> fluent, readable object construction
# - Immutability -> the built object can't be left half-configured
