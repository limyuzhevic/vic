#include "Brain.hpp"

// Include implementation files for modular organization
#include "BrainInitialization.cpp"
#include "BrainProcessingLoop.cpp"
#include "BrainIO.cpp"
#include "BrainUtilities.cpp"

namespace nlm {

// This file now serves as a central include that pulls in all brain implementation files
// The actual implementations are in the separate modular files:
// - BrainInitialization.cpp: Impl struct and initialization methods
// - BrainProcessingLoop.cpp: step() methods
// - BrainIO.cpp: save() and load() methods  
// - BrainUtilities.cpp: All other utility functions

} // namespace nlm

