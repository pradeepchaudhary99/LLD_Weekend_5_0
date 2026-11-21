import java.util.ArrayList;

// =====================================================================
// TOPIC: Builder Design Pattern
//
// Solves the "telescoping constructor" problem (too many constructor
// overloads for optional fields) by building the object step by step
// through a fluent, chainable API, and only assembling the real object
// at the very end via build().
//
// Two properties worth noticing:
//  - CHAINING: every UserBuilder setter returns `this`, so calls can
//    be strung together: new UserBuilder(...).setX(..).setY(..).build()
//  - IMMUTABILITY: once User is built, it has no public setters for
//    name/rollNumber/marks -- the object can't drift out of a valid
//    state after construction. (address is left mutable on purpose,
//    to show the contrast -- see the comment on setAddress below.)
// =====================================================================

class User {
    private final String name;
    private final int rollNumber;
    private String address; // mutable on purpose, unlike the other fields
    private final ArrayList<Integer> marks;

    private User(UserBuilder builder) {
        // Bug in the original draft: this assigned from the OUTER
        // class's own (always-null/zero) fields instead of the
        // builder's fields, so address/rollNumber/marks were always
        // dropped. Reading from `builder.*` is the fix.
        this.name = builder.name;
        this.rollNumber = builder.rollNumber;
        this.address = builder.address;
        this.marks = builder.marks;
    }

    // Only address is allowed to change after construction --
    // name/rollNumber/marks stay immutable once built.
    void setAddress(String address) {
        this.address = address;
    }

    @Override
    public String toString() {
        return "User{name=" + name + ", rollNumber=" + rollNumber +
                ", address=" + address + ", marks=" + marks + "}";
    }

    static class UserBuilder {
        private final String name; // required: supplied via constructor, not a setter
        private int rollNumber;    // optional
        private String address;    // optional
        private ArrayList<Integer> marks; // optional

        UserBuilder(String name) {
            this.name = name;
        }

        UserBuilder setRollNumber(int rollNumber) {
            this.rollNumber = rollNumber;
            return this; // enables chaining
        }

        UserBuilder setAddress(String address) {
            this.address = address;
            return this;
        }

        UserBuilder setMarks(ArrayList<Integer> marks) {
            this.marks = marks;
            return this;
        }

        User build() {
            return new User(this);
        }
    }
}

public class BuilderDesignPattern {
    public static void main(String[] args) {
        ArrayList<Integer> marks = new ArrayList<>();
        marks.add(90);
        marks.add(85);

        User user = new User.UserBuilder("Pradeep")
                .setAddress("Bangalore")
                .setRollNumber(123)
                .setMarks(marks)
                .build();

        System.out.println(user);

        // address is still mutable after construction:
        user.setAddress("Mumbai");
        System.out.println("After moving: " + user);
    }
}

// Builder:
// - Chaining -> fluent, readable object construction
// - Immutability -> the built object can't be left half-configured
