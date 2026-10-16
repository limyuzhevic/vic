// Enhanced configuration validation system with type safety and comprehensive validation
// 
// This configuration validation system provides:
// 1. Type-safe configuration parameter validation
// 2. Constraint checking (ranges, formats, allowed values)
// 3. Dependency validation between parameters
// 4. Default value validation and application
// 5. Configuration schema validation
// 6. Integration with the existing Config system
// 7. Detailed validation error reporting
// 8. Validation rule management and configuration
// 
// Key features:
// - Type-safe validation with compile-time type checking
// - Flexible constraint system (ranges, enums, regex patterns)
// - Dependency validation for parameter relationships
// - Comprehensive error messages with suggestions
// - Rule-based validation system
// - Configuration schema validation
// - Performance-optimized validation
// - Integration with existing Config.cpp/Config.hpp
// 
// Usage examples:
//   nlm::ConfigValidator validator;
//   nlm::Config config;
//   
//   // Define validation rules
//   auto neuronsRule = nlm::ConfigValidator::IntRange(100, 10000);
//   auto densityRule = nlm::ConfigValidator::DoubleRange(0.0, 1.0);
//   auto typeRule = nlm::ConfigValidator::Enum({"excitatory", "inhibitory"});
//   
//   // Apply validation rules
//   validator.addRule("brain.neuron_count", neuronsRule);
//   validator.addRule("brain.synapse_density", densityRule);
//   validator.addRule("brain.neuron_type", typeRule);
//   
//   // Validate configuration
//   if (!validator.validate(config)) {
//       std::cerr << "Configuration validation failed:" << std::endl;
//       for (const auto& error : validator.getErrors()) {
//           std::cerr << "  " << error << std::endl;
//       }
//   }
//   
//   // Or use convenience function
//   if (auto error = nlm::validateConfig(config)) {
//       std::cerr << "Validation error: " << *error << std::endl;
//   }
//   
//   // Validate with predefined schemas
//   if (auto error = nlm::validateBrainConfig(config)) {
//       std::cerr << "Brain config error: " << *error << std::endl;
//   }
