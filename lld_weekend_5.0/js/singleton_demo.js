// =====================================================================
// TOPIC: Singleton pattern -- lazy initialization, contrasted with
// eager initialization.
//
// WHY THERE IS NO "DOUBLE-CHECKED LOCKING" HERE: the Java version this
// was translated from needs a lock because the JVM can run getInstance()
// on many OS threads at once, so two threads could both see
// `instance === null` before either finishes constructing it. JS's
// main thread runs one synchronous call stack at a time -- getInstance()
// always runs to completion before any other JS code gets a turn, so
// the race that double-checked locking defends against cannot occur
// here. (Multiple Worker threads each get their OWN module state, so
// even Workers don't reintroduce the race.) This is a real semantic
// difference between the languages, not a simplification.
// =====================================================================

"use strict";

class ConfigurationManager {
  static #instance = null;

  constructor() {
    this.configuration = {};
  }

  static getInstance() {
    if (ConfigurationManager.#instance === null) {
      console.log("Object created");
      ConfigurationManager.#instance = new ConfigurationManager();
    }
    return ConfigurationManager.#instance;
  }
}

// Eager initialization: the instance is created the moment this
// module is first imported/required -- Node's module cache guarantees
// a module's top-level code runs exactly once, so this is just as
// "thread-safe" as the lazy version, with no lazy/first-call delay.
class ConfigurationManagerEager {
  constructor() {
    this.configuration = {};
  }

  static getInstance() {
    return eagerInstance;
  }
}

const eagerInstance = new ConfigurationManagerEager();

function main() {
  console.log("== Lazy singleton: 100 calls to getInstance() ==");
  for (let i = 0; i < 100; i++) {
    ConfigurationManager.getInstance();
  }
  console.log('"Object created" should have printed exactly once above.');

  console.log("\n== Eager initialization ==");
  const eager1 = ConfigurationManagerEager.getInstance();
  const eager2 = ConfigurationManagerEager.getInstance();
  console.log(`Same instance? ${eager1 === eager2}`);
}

main();
