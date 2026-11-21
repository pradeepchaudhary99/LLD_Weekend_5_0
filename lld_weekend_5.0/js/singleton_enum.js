// =====================================================================
// TOPIC: Singleton pattern -- the "frozen module export" approach.
//
// JS has no enum type at all, so there's no direct equivalent of
// Java's enum-based singleton trick. The closest idiom that gives you
// the same "exactly one instance, guaranteed by the runtime" property
// is exporting an already-constructed, frozen object from a module:
// Node/ESM module caching guarantees the module body runs once, so
// every importer receives the exact same object, and Object.freeze
// stops anyone from mutating it into something else.
// =====================================================================

"use strict";

class DatabaseConnection {
  connect() {
    console.log("Connected to DB");
  }
}

// INSTANCE is the only object of this class we ever create.
// Module caching guarantees this file's top-level code -- and so this
// object -- is created exactly once per process.
const INSTANCE = Object.freeze(new DatabaseConnection());

function main() {
  const db1 = INSTANCE;
  const db2 = INSTANCE;
  db1.connect();
  console.log(`Same instance? ${db1 === db2}`);
}

main();

module.exports = { INSTANCE };
