// Enhanced configuration system with improved error handling and validation
// 
// This enhanced configuration system provides:
// 1. Comprehensive input validation for all operations
// 2. Detailed error reporting and recovery mechanisms
// 3. Type-safe value conversion with validation
// 4. Improved file I/O error handling
// 5. Memory-safe operations
// 6. Detailed error context and debugging information
// 
// Key improvements over original:
// - Input validation for file paths, keys, values
// - Detailed error messages with context
// - Graceful error recovery where possible
// - Type conversion safety with validation
// - File I/O error handling with retry logic
// - Memory safety checks
// - Thread-safe operations
// - Configuration validation
// 
// Usage examples:
//   nlm::Config config;
//   if (config.loadFromFile("config.json")) {
//       int neurons = config.getOr<int>("brain.neuron_count", 1000);
//   } else {
//       std::cerr << "Failed to load config: " << config.getLastError();
//   }
//   
//   config.set("brain.neuron_count", 2000);
//   if (!config.has("brain.synapse_density")) {
//       std::cerr << "Warning: synapse density not set!";
//   }
