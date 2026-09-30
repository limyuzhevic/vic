// Brain Initialization Fix Plan
//
// PROBLEM: Memory systems (workingMemory, episodicMemory, associativeMemory) are constructed
//          in Brain::Impl constructor but NOT initialized with brain reference.
//          Brain::initialize() is required to initialize them, but may never be called.
//          This leaves memory systems with brain_ = nullptr, causing integration issues.
//
// ROOT CAUSE:
// 1. Brain::Impl constructor (lines 112-114 in Brain.cpp):
//    - Creates memory systems: workingMemory, episodicMemory, associativeMemory
//    - Does NOT call .initialize(this) on them
//    - Memory system constructors set brain_ = nullptr
//
// 2. Brain::initialize() method (lines 177-300):
//    - Only called if explicitly invoked
//    - Calls .initialize(this) on memory systems (lines 235, 239, 243, 259, 260, etc.)
//    - This initializes the systems with brain reference
//
// 3. When Brain::initialize() is NOT called:
//    - Memory systems exist but brain_ = nullptr
//    - Methods like storeToNeuron() check if (brain_) and return early
//    - Integration fails because memory systems can't communicate with brain
//
// SOLUTION:
// Move memory system initialization from Brain::initialize() to Brain::Impl constructor
// so all memory systems are properly initialized when Brain object is created.
// Keep Brain::initialize() for neuron/region creation and configuration.
//
// FIX IMPLEMENTATION:
// 1. Add memory system initialization to Brain::Impl constructor (after line 154)
// 2. Remove memory system initialization from Brain::initialize() (lines 235, 239, 243, 259, 260)
// 3. Update all memory system initialize() methods to properly set up with brain reference
//
// STEP-BY-STEP:
// A. Modify Brain::Impl constructor to initialize all memory systems
// B. Update other systems (planner, conceptFormation, attention, novelty, curiosity, dopamine)
//    that are also initialized in Brain::initialize() to match initialization pattern
// C. Remove redundant initialization from Brain::initialize()
// D. Verify tests still pass (some may need adjustment if they expect initialize() to work)
//
// IMPACT:
// - Memory systems will always be properly initialized when Brain is created
// - No more need for optional Brain::initialize() call for memory system setup
// - More robust initialization sequence
// - Fixes integration issues where memory systems were unusable