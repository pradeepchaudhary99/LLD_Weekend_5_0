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
//  - IMMUTABILITY: once a User is built, its name/rollNumber/marks are
//    never reassigned -- the object can't drift out of a valid state
//    after construction. (address is left mutable on purpose, to show
//    the contrast -- see setAddress below.)
// =====================================================================

"use strict";

class User {
  #name;
  #rollNumber;
  #address; // mutable on purpose, unlike the other fields
  #marks;

  constructor(builder) {
    this.#name = builder.name;
    this.#rollNumber = builder.rollNumber;
    this.#address = builder.address;
    this.#marks = builder.marks;
  }

  setAddress(address) {
    // Only address is allowed to change after construction.
    this.#address = address;
  }

  toString() {
    return `User{name=${this.#name}, rollNumber=${this.#rollNumber}, ` +
           `address=${this.#address}, marks=[${this.#marks}]}`;
  }

  static get Builder() {
    return UserBuilder;
  }
}

class UserBuilder {
  constructor(name) {
    this.name = name;       // required
    this.rollNumber = 0;    // optional
    this.address = null;    // optional
    this.marks = [];        // optional
  }

  setRollNumber(rollNumber) {
    this.rollNumber = rollNumber;
    return this; // enables chaining
  }

  setAddress(address) {
    this.address = address;
    return this;
  }

  setMarks(marks) {
    this.marks = marks;
    return this;
  }

  build() {
    return new User(this);
  }
}

function main() {
  const user = new UserBuilder("Pradeep")
    .setAddress("Bangalore")
    .setRollNumber(123)
    .setMarks([90, 85])
    .build();

  console.log(user.toString());

  // address is still mutable after construction:
  user.setAddress("Mumbai");
  console.log(`After moving: ${user}`);
}

main();

// Builder:
// - Chaining -> fluent, readable object construction
// - Immutability -> the built object can't be left half-configured
