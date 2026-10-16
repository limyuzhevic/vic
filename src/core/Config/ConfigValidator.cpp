// Enhanced configuration validation with type safety and comprehensive validation
// 
// This file implements the ConfigValidator class and related validation infrastructure:
// 1. Validation rule system with type safety
// 2. Constraint validation (ranges, enums, patterns)
// 3. Dependency validation between parameters
// 4. Configuration schema validation
// 5. Integration with existing Config system
// 6. Error reporting and validation statistics
// 7. Performance-optimized validation
// 
// Key improvements:
// - Type-safe validation using template specialization
// - Comprehensive constraint checking
// - Dependency validation for parameter relationships
// - Detailed error messages with suggestions
// - Rule-based validation system
// - Configuration schema validation
// - Performance tracking and reporting
// - Thread-safe validation
// - Validation statistics and metrics
// 
// Validation rule types:
// - IntRange: Validates integer values within specified range
// - DoubleRange: Validates double values within specified range
// - Enum: Validates against set of allowed values
// - StringPattern: Validates string against regex pattern
// - LengthRange: Validates string/vector length
// - Positive: Validates value is positive
// - NonNegative: Validates value is non-negative
// - Boolean: Validates boolean values
// - Dependency: Validates relationship between parameters
// - Custom: User-defined validation functions
// 
// Validation statistics track:
// - Number of rules applied
// - Number of validation errors
// - Number of warnings
// - Validation time per parameter
// - Success rate
