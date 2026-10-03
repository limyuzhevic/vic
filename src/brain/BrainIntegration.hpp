// Main NLM Brain implementation
// Phase 2: Real Neural Computation
//
// This is the main Brain class implementation file that includes all the
// specialized implementation files and provides the complete Brain class
// interface. This file has been refactored from the original monolithic
// Brain.cpp (1117+ lines) into smaller, focused implementation files:
//
// - BrainInitializer.cpp: Brain initialization
// - BrainStep.cpp: Main simulation step execution
// - BrainMemory.cpp: Memory systems
// - BrainPrediction.cpp: Prediction systems
// - BrainCognition.cpp: Cognition systems
// - BrainNeuromodulation.cpp: Neuromodulation systems
// - BrainDevelopment.cpp: Development systems
// - BrainCheckpoint.cpp: Checkpoint systems
// - BrainStatistics.cpp: Statistics and logging
//
// This refactoring improves:
// - Code maintainability and readability
// - Separation of concerns
// - Testability
// - Documentation
// - Modularity

#include "Brain.hpp"
#include "BrainInitializer.hpp"
#include "BrainStep.hpp"
#include "BrainMemory.hpp"
#include "BrainPrediction.hpp"
#include "BrainCognition.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainDevelopment.hpp"
#include "BrainCheckpoint.hpp"
#include "BrainStatistics.hpp"

// Include convenience method implementations
#include "BrainIntegration.hpp"

namespace nlm {

// All Brain class method implementations are now in separate files
// and included here. The Brain class maintains a clean header file
// with comprehensive documentation and provides a user-friendly interface
// through convenience methods.

// For backward compatibility, we include all method implementations here.
// The actual implementations are in the separate implementation files.

// The convenience method implementations are in BrainIntegration.hpp/cpp

} // namespace nlm