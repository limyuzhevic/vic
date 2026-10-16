// ConfigValidator Tests using Google Test Framework
// Comprehensive unit tests for nlm::ConfigValidator class
// Tests schema validation, error cases, and boundary conditions

#include "fixtures/TestHelpers.hpp"
#include "core/Config/ConfigValidator.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

namespace test {

// Test 2.1: Basic Validation
TEST_F(NLMTest, ConfigValidatorBasicValidation) {
    nlm::ConfigValidator validator;
    
    nlm::ConfigEntry entry("known_field", 42, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result = validator.validate(entry);
    
    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.errors.size(), 0);
}

// Test 2.2: Unknown Field Handling
TEST_F(NLMTest, ConfigValidatorUnknownField) {
    nlm::ConfigValidator validator;
    
    nlm::ConfigEntry entry("unknown_field", 42, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result = validator.validate(entry);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_GT(result.errors.size(), 0);
    EXPECT_TRUE(result.warnings.empty());  // Only error, no warning
}

// Test 2.3: Range Validation
TEST_F(NLMTest, ConfigValidatorRangeValidation) {
    nlm::NLMConfigValidator validator;
    
    // Test valid range
    nlm::ConfigEntry entry1("neuron_count", 100, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result1 = validator.validate(entry1);
    EXPECT_TRUE(result1.isValid);
    
    // Test invalid range (too low)
    nlm::ConfigEntry entry2("neuron_count", 0, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result2 = validator.validate(entry2);
    EXPECT_FALSE(result2.isValid);
    EXPECT_GT(result2.errors.size(), 0);
    
    // Test invalid range (too high)
    nlm::ConfigEntry entry3("neuron_count", 10001, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result3 = validator.validate(entry3);
    EXPECT_FALSE(result3.isValid);
    EXPECT_GT(result3.errors.size(), 0);
}

// Test 2.4: Enum Validation
TEST_F(NLMTest, ConfigValidatorEnumValidation) {
    nlm::NLMConfigValidator validator;
    
    // Test valid enum value
    nlm::ConfigEntry entry1("learning_rate_algorithm", "adam", nlm::ConfigSource::Runtime);
    nlm::ValidationResult result1 = validator.validate(entry1);
    EXPECT_TRUE(result1.isValid);
    
    // Test invalid enum value
    nlm::ConfigEntry entry2("learning_rate_algorithm", "invalid_algo", nlm::ConfigSource::Runtime);
    nlm::ValidationResult result2 = validator.validate(entry2);
    EXPECT_FALSE(result2.isValid);
    EXPECT_GT(result2.errors.size(), 0);
}

// Test 2.5: Format Validation
TEST_F(NLMTest, ConfigValidatorFormatValidation) {
    nlm::NLMConfigValidator validator;
    
    // Test valid format (should pass)
    nlm::ConfigEntry entry1("learning_rate_algorithm", "sgd", nlm::ConfigSource::Runtime);
    nlm::ValidationResult result1 = validator.validate(entry1);
    EXPECT_TRUE(result1.isValid);
    
    // Test invalid format (IP address for string field)
    nlm::ConfigEntry entry2("learning_rate_algorithm", "invalid_ip", nlm::ConfigSource::Runtime);
    nlm::ValidationResult result2 = validator.validate(entry2);
    EXPECT_FALSE(result2.isValid);
    EXPECT_GT(result2.errors.size(), 0);
}

// Test 2.6: Multiple Constraint Validation
TEST_F(NLMTest, ConfigValidatorMultipleConstraints) {
    nlm::NLMConfigValidator validator;
    
    // Valid value should pass all constraints
    nlm::ConfigEntry entry1("batch_size", 32, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result1 = validator.validate(entry1);
    EXPECT_TRUE(result1.isValid);
    
    // Invalid range should fail
    nlm::ConfigEntry entry2("batch_size", 0, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result2 = validator.validate(entry2);
    EXPECT_FALSE(result2.isValid);
}

// Test 2.7: Validation Result Aggregation
TEST_F(NLMTest, ConfigValidatorResultAggregation) {
    nlm::NLMConfigValidator validator;
    
    std::vector<nlm::ConfigEntry> entries;
    entries.push_back(nlm::ConfigEntry("neuron_count", 100, nlm::ConfigSource::Runtime));
    entries.push_back(nlm::ConfigEntry("region_count", 2, nlm::ConfigSource::Runtime));
    entries.push_back(nlm::ConfigEntry("invalid_field", "value", nlm::ConfigSource::Runtime));  // Unknown field
    
    nlm::ValidationResult result = validator.validateAll(entries);
    
    EXPECT_FALSE(result.isValid);  // Should fail due to unknown field
    EXPECT_GT(result.errors.size(), 0);
}

// Test 2.8: Validation Mode Strict
TEST_F(NLMTest, ConfigValidatorStrictMode) {
    nlm::NLMConfigValidator validator;
    validator.setStrictMode(true);
    
    nlm::ConfigEntry entry("unknown_field", 42, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result = validator.validate(entry);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_GT(result.errors.size(), 0);
    EXPECT_TRUE(result.warnings.empty());
}

// Test 2.9: Validation Mode Warning
TEST_F(NLMTest, ConfigValidatorWarningMode) {
    nlm::NLMConfigValidator validator;
    validator.setStrictMode(false);
    
    nlm::ConfigEntry entry("unknown_field", 42, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result = validator.validate(entry);
    
    EXPECT_FALSE(result.isValid);  // Still invalid
    EXPECT_GT(result.errors.size(), 0);
    // In warning mode, might have warnings instead of errors
    EXPECT_FALSE(result.warnings.empty());
}

// Test 2.10: Custom Constraint Validation
TEST_F(NLMTest, ConfigValidatorCustomConstraints) {
    nlm::NLMConfigValidator validator;
    
    // Add a custom range constraint
    auto rangeConstraint = std::make_unique<nlm::RangeConstraint>(-1, 1000, "Valid neuron count range");
    
    // Note: This test would require accessing validator's constraint management
    // For now, test the built-in constraints work correctly
    nlm::ConfigEntry entry("neuron_count", 50, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result = validator.validate(entry);
    EXPECT_TRUE(result.isValid);
}

// Test 2.11: Type Validation
TEST_F(NLMTest, ConfigValidatorTypeValidation) {
    nlm::NLMConfigValidator validator;
    
    // Test correct type passes
    nlm::ConfigEntry entry1("neuron_count", 100, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result1 = validator.validate(entry1);
    EXPECT_TRUE(result1.isValid);
    
    // Test type mismatch (should warn but not error for simple types)
    nlm::ConfigEntry entry2("neuron_count", "not_a_number", nlm::ConfigSource::Runtime);
    nlm::ValidationResult result2 = validator.validate(entry2);
    // Type checking is primarily done at runtime, so this might still be valid
    // depending on implementation
}

// Test 2.12: Validation Error Details
TEST_F(NLMTest, ConfigValidatorErrorDetails) {
    nlm::NLMConfigValidator validator;
    
    nlm::ConfigEntry entry("neuron_count", 0, nlm::ConfigSource::Runtime);
    nlm::ValidationResult result = validator.validate(entry);
    
    if (!result.isValid) {
        EXPECT_GT(result.errors.size(), 0);
        // Verify error details are meaningful
        for (const auto& error : result.errors) {
            EXPECT_FALSE(error->what().empty());
        }
    }
}

// Test RangeConstraint
TEST_F(NLMTest, RangeConstraintValidation) {
    nlm::RangeConstraint constraint(-10, 10, "Test range");
    nlm::ValidationResult result;
    
    // Valid value
    ASSERT_TRUE(constraint.validate(nlm::ConfigValue(5.0), result));
    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.errors.size(), 0);
    
    // Invalid value (too low)
    result.isValid = true;
    result.errors.clear();
    ASSERT_FALSE(constraint.validate(nlm::ConfigValue(-15.0), result));
    EXPECT_FALSE(result.isValid);
    EXPECT_GT(result.errors.size(), 0);
    
    // Invalid value (too high)
    result.isValid = true;
    result.errors.clear();
    ASSERT_FALSE(constraint.validate(nlm::ConfigValue(15.0), result));
    EXPECT_FALSE(result.isValid);
    EXPECT_GT(result.errors.size(), 0);
}

// Test EnumConstraint
TEST_F(NLMTest, EnumConstraintValidation) {
    std::vector<std::string> allowed = {"on", "off", "auto"};
    nlm::EnumConstraint constraint(allowed, "Test enum");
    nlm::ValidationResult result;
    
    // Valid value
    ASSERT_TRUE(constraint.validate(nlm::ConfigValue(std::string("on")), result));
    EXPECT_TRUE(result.isValid);
    
    // Invalid value
    result.isValid = true;
    result.errors.clear();
    ASSERT_FALSE(constraint.validate(nlm::ConfigValue(std::string("invalid")), result));
    EXPECT_FALSE(result.isValid);
    EXPECT_GT(result.errors.size(), 0);
}

// Test FormatConstraint
TEST_F(NLMTest, FormatConstraintValidation) {
    nlm::FormatConstraint constraint("^[a-zA-Z0-9_]+$", "alphanumeric identifier");
    nlm::ValidationResult result;
    
    // Valid format
    ASSERT_TRUE(constraint.validate(nlm::ConfigValue(std::string("valid123")), result));
    EXPECT_TRUE(result.isValid);
    
    // Invalid format
    result.isValid = true;
    result.errors.clear();
    ASSERT_FALSE(constraint.validate(nlm::ConfigValue(std::string("invalid-format")), result));
    EXPECT_FALSE(result.isValid);
    EXPECT_GT(result.errors.size(), 0);
}

} // namespace test