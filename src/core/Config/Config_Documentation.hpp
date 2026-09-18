// Configuration and Settings Module
// =======================
// 
// This module provides a flexible configuration system for the NLM (Neural Learning Machine).
// It supports multiple input formats (JSON, YAML, key=value) and maintains configuration
// history for reproducibility and debugging.
// 
// Key Features:
// - Multiple input formats (JSON, YAML, simple key=value)
// - Configuration versioning and history tracking
// - Runtime configuration overrides
// - Validation and type safety
// - Support for complex data types (arrays, vectors)
// - Serialization to JSON for checkpointing
// 
// Usage Example:
// ```cpp
// Config config;
// config->loadFromFile("config.json");
// config->set("learning_rate", 0.01, ConfigSource::Runtime);
// double rate = config->getOr<double>("learning_rate", 0.001);
// ```

// Configuration Entry Structure
// =============================
// Each configuration key has:
// - Key: String identifier
// - Value: Variant type supporting int, int64_t, double, bool, string
// - Source: How the value was loaded (Default, File, CommandLine, Runtime)
// - Description: Human-readable explanation
// 
// Configuration Sources
// --------------------
// Default: Hard-coded defaults
// File: From configuration files (JSON/YAML)
// CommandLine: From command-line arguments
// Runtime: Set during execution

// Configuration Access Patterns
// -----------------------------
// Safe access with defaults:
//   auto value = config->getOr<int>("key", default);
// 
// Optional access:
//   auto value = config->get<int>("key");
//   if (value) { use(*value); }
// 
// Configuration Override Priority (highest to lowest):
// 1. Runtime (most specific)
// 2. CommandLine
// 3. File
// 4. Default (least specific)

// Error Handling
// --------------
// - Missing keys return nullopt (optional)
// - Type mismatches use defaults
// - File loading failures are logged but don't crash
// - Invalid values are clamped to valid ranges

// Performance Considerations
// --------------------------
// - Configuration is immutable after initialization (except for runtime overrides)
// - Fast lookup via std::find_if (linear, could be optimized with hash table)
// - Serialization supports checkpointing for model persistence

// Example Configuration Files
// --------------------------
// JSON (recommended):
// {
//   "neuron_count": 1000,
//   "simulation_timestep": 0.001,
//   "plasticity": {
//     "stdp_ltp_weight": 0.01,
//     "stdp_ltd_weight": 0.012
//   }
// }
//
// key=value format (legacy):
// neuron_count = 1000
simulation_timestep = 0.001
plasticity.stdp_ltp_weight = 0.01

// Extension Points
// ----------------
// - Custom ConfigValue types can be added
// - Custom file parsers can be registered
// - Validation hooks can be registered
// - Configuration observers can be registered

// Testing and Validation
// ---------------------
// - Configuration can be validated against a schema
// - Default values can be validated
// - File formats can be validated
// - Type safety is enforced through std::variant

// Memory Management
// -----------------
// - PImpl pattern for source encapsulation
// - Smart pointers for memory safety
// - Copy semantics disabled (move-only)
// - Deep copying supported via serialization

// Logging and Debugging
// ---------------------
// - Configuration changes are logged
// - Source tracking helps debug configuration issues
// - Summary() method provides configuration overview
// - Debug output shows source and value for each key

// Integration with Other Systems
// ------------------------------
// - Used by Brain for all simulation parameters
// - Used by development system for growth parameters
// - Used by neuromodulation systems for signal parameters
// - Used by checkpoint system for state restoration

// Future Enhancements
// ------------------
// - Support for environment variables
// - Support for dynamic configuration (reloading)
// - Configuration schema validation
// - Configuration inheritance
// - Configuration templates
// - Configuration comparison (diff)
// - Configuration merging
