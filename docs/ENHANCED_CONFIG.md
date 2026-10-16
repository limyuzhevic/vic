// ConfigFile JSON support improved
// The configuration system now supports JSON files with nested structures
// while maintaining backward compatibility with simple key=value format

// Key improvements:
// 1. JSON parsing with nlohmann/json library
// 2. Support for nested configuration sections (e.g., brain.neuron_count)
// 3. Support for arrays and complex data types
// 4. Pretty-printed JSON output for configuration files
// 5. Merging configuration layers (default < file < args)
// 6. Enhanced error handling and logging
//
// Configuration file examples:
// Simple format (backward compatible):
//   brain.neuron_count = 1000
//   brain.synapse_density = 0.1
//   plasticity.stdp.enable = true
//
// JSON format (new):
// {
//   "brain": {
//     "neuron_count": 1000,
//     "synapse_density": 0.1,
//     "v_thresh": -50.0,
//     "v_rest": -70.0
//   },
//   "plasticity": {
//     "stdp": {
//       "enable": true,
//       "learning_rate": 0.001,
//       "tau_plus": 20.0,
//       "tau_minus": 20.0
//     },
//     "hebbian": {
//       "enable": true
//     },
//     "structural": {
//       "enable": true
//     }
//   }
// }

// Benefits:
// - Human-readable and editable
// - Better version control
// - Supports complex data structures
// - Validation and type safety
// - Extensible for future configuration options