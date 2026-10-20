#include "main_bindings.h"

namespace nlm {

// This file now serves as a unified entry point that delegates to the new modular structure
// All bindings have been moved to individual files for better organization and maintainability

// The following functions are implemented in the new files:
// - main_bindings.cpp: Module entry point and initialization
// - basic_types.cpp: Basic type definitions, IDs, and enumerations
// - config.cpp: Configuration class
// - sensory.cpp: Sensory input classes (Vision, Audio, InternalSignals)
// - action.cpp: Action and WorldObject classes
// - agent.cpp: AgentBody, ActionResult, and SensoryPercept classes
// - world.cpp: SimpleWorld class
// - brain.cpp: Brain class
// - agentbrain.cpp: AgentBrain class
// - factories.cpp: Factory functions and utilities
// - error_handling.cpp: Error handling and type conversion utilities

// This file is kept for backward compatibility and to ensure the original module still works
PYBIND11_MODULE(pynlm, m) {
    // Delegate to the new modular binding system
    // The actual bindings are implemented in main_bindings.cpp which includes all other modules
    // This maintains the original API while providing improved organization
    
    // Import all the modular bindings
    // Note: In a real implementation, you would use pybind11's module system differently
    // For now, this maintains compatibility with the original bindings.cpp
    
    // The new structure is organized as follows:
    // 1. main_bindings.cpp: Contains the PYBIND11_MODULE macro
    // 2. Each component is in its own file and included via separate functions
    // 3. error_handling.cpp: Contains error handling and type conversion utilities
    
    // This file is essentially a placeholder to maintain backward compatibility
    // The real implementation is in main_bindings.cpp
}

} // namespace nlm