// Modular Python Bindings for NLM (Neural Learning Machine)
// Refactored for better maintainability and organization
// 
// This file now delegates class bindings to helper functions
// providing better organization, documentation, and maintainability

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all necessary headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include helper functions for modular binding
#include "bindings_helper.cpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This is the Phase 6 modular bindings system with:
        - Organized helper functions for different binding categories
        - Enhanced documentation and examples
        - Better maintainability and extensibility
        - Consistent error handling patterns
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== ID TYPE BINDINGS =====
    createIdTypeBindings(m);

    // ===== ENUM BINDINGS =====
    createEnumBindings(m);

    // ===== CONFIG CLASS BINDINGS =====
    createConfigBindings(m);

    // ===== SENSORY INPUT CLASS BINDINGS =====
    createSensoryInputBindings(m);

    // ===== ACTION CLASS BINDINGS =====
    createActionBindings(m);

    // ===== WORLD OBJECT BINDINGS =====
    createWorldObjectBindings(m);

    // ===== AGENT BODY BINDINGS =====
    createAgentBodyBindings(m);

    // ===== ACTION RESULT BINDINGS =====
    createActionResultBindings(m);

    // ===== SENSORY PERCEPT BINDINGS =====
    createSensoryPerceptBindings(m);

    // ===== WORLD CLASS BINDINGS =====
    createSimpleWorldBindings(m);

    // ===== BRAIN CLASS BINDINGS =====
    createBrainBindings(m);

    // ===== AGENT BRAIN CLASS BINDINGS =====
    createAgentBrainBindings(m);

    // ===== FACTORY FUNCTION BINDINGS =====
    createFactoryBindings(m);

    // Additional module attributes
    m.attr("PYNLM_VERSION") = "Phase 6 Modular Bindings";
    m.attr("PROJECT_NAME") = "NLM - Neural Learning Machine";
    m.attr("DEVELOPMENT_PHASE") = "6 - Final Integration";
}

} // namespace nlm

/*
  SUMMARY OF IMPROVEMENTS:
  
  1. MODULAR DESIGN:
     - Each binding category has its own helper function
     - Better separation of concerns
     - Easier to maintain and extend
  
  2. ENHANCED DOCUMENTATION:
     - Each helper function has comprehensive docstrings
     - Module-level documentation explains the design
     - Examples and usage patterns included
  
  3. BETTER MAINTAINABILITY:
     - Changes to specific binding types are isolated
     - Easier to debug and test individual binding categories
     - Clear organization of related bindings
  
  4. EXTENSIBILITY:
     - New binding categories can be added by creating helper functions
     - Easy to modify existing bindings without affecting others
     - Consistent patterns for adding future classes
  
  5. ERROR HANDLING:
     - Consistent exception registration across all bindings
     - Better error messages and type safety
  
  USAGE:
  
  The improved bindings maintain full backward compatibility while providing:
  
  ```python
  import pynlm
  
  # Basic usage (unchanged)
  brain = pynlm.createBrain(pynlm.createDefaultConfig())
  brain.initialize()
  brain.step(0)
  
  # Enhanced with modular bindings
  config = pynlm.createDefaultConfig()
  config.loadFromFile("configs/nlm_config.json")  # Better config support planned
  
  # New Phase 6 features (when implemented)
  brain.enableAdvancedFeatures(True)
  brain.setDevelopmentStage("Adult")
  brain.enableMemoryReplay(True)
  ```
  
  FUTURE ENHANCEMENTS:
  
  1. JSON/YAML config parser integration
  2. Advanced error handling and validation
  3. Performance optimizations for complex operations
  4. Additional user commands and features
  5. Better integration with external tools
*/