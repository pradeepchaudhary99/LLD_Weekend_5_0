// =====================================================================
// TOPIC: Builder Design Pattern
//
// Solves the "telescoping constructor" problem (too many constructor
// overloads for optional fields) by building the object step by step
// through a fluent, chainable API, and only assembling the real object
// at the very end via build().
//
// Two properties worth noticing:
//  - CHAINING: every UserBuilder setter returns *this (by reference),
//    so calls can be strung together.
//  - IMMUTABILITY: once User is built, name/rollNumber/marks are never
//    reassigned -- the object can't drift out of a valid state after
//    construction. (address is left mutable on purpose, to show the
//    contrast -- see setAddress below.)
// =====================================================================

#include <iostream>
#include <string>
#include <vector>

class User {
public:
    class UserBuilder {
    public:
        explicit UserBuilder(std::string name) : name_(std::move(name)) {}

        UserBuilder& setRollNumber(int rollNumber) {
            rollNumber_ = rollNumber;
            return *this; // enables chaining
        }

        UserBuilder& setAddress(const std::string& address) {
            address_ = address;
            return *this;
        }

        UserBuilder& setMarks(std::vector<int> marks) {
            marks_ = std::move(marks);
            return *this;
        }

        User build() const {
            return User(*this);
        }

    private:
        friend class User;
        std::string name_;              // required
        int rollNumber_ = 0;             // optional
        std::string address_;            // optional
        std::vector<int> marks_;         // optional
    };

    // Only address is allowed to change after construction.
    void setAddress(const std::string& address) { address_ = address; }

    void print() const {
        std::cout << "User{name=" << name_ << ", rollNumber=" << rollNumber_
                   << ", address=" << address_ << ", marks=[";
        for (size_t i = 0; i < marks_.size(); i++) {
            std::cout << marks_[i] << (i + 1 < marks_.size() ? ", " : "");
        }
        std::cout << "]}\n";
    }

private:
    explicit User(const UserBuilder& builder)
        : name_(builder.name_),
          rollNumber_(builder.rollNumber_),
          address_(builder.address_),
          marks_(builder.marks_) {}

    std::string name_;
    int rollNumber_;
    std::string address_; // mutable on purpose, unlike the other fields
    std::vector<int> marks_;
};

int main() {
    User user = User::UserBuilder("Pradeep")
                     .setAddress("Bangalore")
                     .setRollNumber(123)
                     .setMarks({90, 85})
                     .build();

    user.print();

    // address is still mutable after construction:
    user.setAddress("Mumbai");
    std::cout << "After moving: ";
    user.print();

    return 0;
}

// Builder:
// - Chaining -> fluent, readable object construction
// - Immutability -> the built object can't be left half-configured
