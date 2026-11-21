// =====================================================================
// TOPIC: Builder Design Pattern
//
// Solves the "telescoping constructor" problem (too many constructor
// overloads for optional fields) by building the object step by step
// through a fluent, chainable API, and only assembling the real object
// at the very end via Build().
//
// Two properties worth noticing:
//  - CHAINING: every UserBuilder setter returns `this`, so calls can
//    be strung together.
//  - IMMUTABILITY: once a User is built, its Name/RollNumber/Marks are
//    get-only -- the object can't drift out of a valid state after
//    construction. (Address is left mutable on purpose, to show the
//    contrast -- see SetAddress below.)
// =====================================================================

using System;
using System.Collections.Generic;

namespace LldWeekend.BuilderDesignPattern
{
    public class User
    {
        public string Name { get; }
        public int RollNumber { get; }
        public string Address { get; private set; } // mutable on purpose
        public List<int> Marks { get; }

        private User(UserBuilder builder)
        {
            Name = builder.Name;
            RollNumber = builder.RollNumberValue;
            Address = builder.AddressValue;
            Marks = builder.MarksValue;
        }

        // Only Address is allowed to change after construction.
        public void SetAddress(string address) => Address = address;

        public override string ToString() =>
            $"User{{Name={Name}, RollNumber={RollNumber}, Address={Address}, Marks=[{string.Join(", ", Marks)}]}}";

        public class UserBuilder
        {
            public string Name { get; }                 // required
            public int RollNumberValue { get; private set; }     // optional
            public string AddressValue { get; private set; }     // optional
            public List<int> MarksValue { get; private set; } = new();  // optional

            public UserBuilder(string name) => Name = name;

            public UserBuilder SetRollNumber(int rollNumber)
            {
                RollNumberValue = rollNumber;
                return this; // enables chaining
            }

            public UserBuilder SetAddress(string address)
            {
                AddressValue = address;
                return this;
            }

            public UserBuilder SetMarks(List<int> marks)
            {
                MarksValue = marks;
                return this;
            }

            public User Build() => new User(this);
        }
    }

    public static class BuilderDesignPatternProgram
    {
        public static void Main()
        {
            var user = new User.UserBuilder("Pradeep")
                .SetAddress("Bangalore")
                .SetRollNumber(123)
                .SetMarks(new List<int> { 90, 85 })
                .Build();

            Console.WriteLine(user);

            // Address is still mutable after construction:
            user.SetAddress("Mumbai");
            Console.WriteLine($"After moving: {user}");
        }
    }
}

// Builder:
// - Chaining -> fluent, readable object construction
// - Immutability -> the built object can't be left half-configured
