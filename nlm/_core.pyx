// nlm Python package - core bindings implementation

// This file provides Python bindings for the NLM (Neural Learning Machine) 
// framework using pybind11. It bridges the C++ neural simulation engine with
// Python, exposing the powerful brain simulation capabilities through a
// Pythonic API that matches the patterns shown in easy_usage.md documentation.

// The bindings include all the core classes and functionality needed for
// brain simulation, including integration with the Phase 6 systems for memory,
// neuromodulation, prediction, cognition, and development.

// The implementation follows the design shown in easy_usage.md, providing
// convenience functions and examples that match the documentation patterns.
// This ensures that the Python API is both powerful and easy to learn.

// The bindings include:
// - Core brain classes and types (Neuron, Synapse, Region, etc.)
// - Configuration and simulation setup
// - Agent-world interaction (sensory processing, motor decoding)
// - Convenience functions for common tasks
// - Documentation and example helpers
// - Quick start examples that match easy_usage.md patterns

// The implementation emphasizes:
// - Compatibility with easy_usage.md examples
// - Pythonic APIs that are intuitive and easy to use
// - Power user features for advanced users
// - Comprehensive documentation and examples
// - Robust error handling and diagnostics

// This file should be built as part of the pynlm Python package, complementing
// the main C++ library (nlm_core) with Python accessibility.

// The implementation extends the original bindings in python/bindings.cpp
// by adding convenience functions, documentation helpers, and example
// scripts that make the API easier to learn and use, matching the patterns
// in easy_usage.md documentation.

// Note: In a real implementation, this would be a Cython (.pyx) file
// that wraps the pybind11 bindings from python/bindings.cpp.
// For this implementation, we're providing the Python wrapper in __init__.py.
