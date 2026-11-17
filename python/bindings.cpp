// Main NLM Python bindings wrapper
// This file conditionally includes either the original or enhanced bindings
// based on the configuration in nlm_python_bindings_config.h

#include "nlm_python_bindings_config.h"

#if ENHANCED_BINDINGS
    // Use enhanced bindings
    #include "bindings_enhanced.cpp"
#else
    // Use original bindings
    #include "bindings.cpp"
#endif

// This file simply includes one of the binding implementations
// The CMakeLists.txt will compile the appropriate version based on the build configuration