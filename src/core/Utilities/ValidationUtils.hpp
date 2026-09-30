#pragma once

#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <functional>
#include "Types.hpp"

namespace nlm {
namespace Core {
namespace Utilities {

/**
 * Validation utility functions for neural network simulations
 */
class ValidationUtils {
public:
    // String validation
    static bool isValidIdentifier(const std::string& str);
    static bool isAlphaNumeric(const std::string& str);
    static bool containsOnlyLetters(const std::string& str);
    static bool containsOnlyDigits(const std::string& str);
    static bool isValidName(const std::string& name);
    static bool isValidKey(const std::string& key);
    static bool isValidFilename(const std::string& filename);
    static bool isValidPath(const std::string& path);

    // Format validation
    static bool isValidJSON(const std::string& json);
    static bool isValidXML(const std::string& xml);
    static bool isValidNumberFormat(const std::string& str);
    static bool isValidBooleanFormat(const std::string& str);
    static bool isValidVectorFormat(const std::string& str);

    // Content validation
    static bool containsOnlyPrintable(const std::string& str);
    static bool isSafeString(const std::string& str);
    static size_t countOccurrences(const std::string& str, char ch);
    static std::string sanitizeForJSON(const std::string& input);
    static std::string escapeHTML(const std::string& input);

    // Range validation
    template<typename T>
    static bool inRange(T value, T min, T max) {
        return value >= min && value <= max;
    }

    template<typename T>
    static bool inRangeExclusive(T value, T min, T max) {
        return value > min && value < max;
    }

    static bool isPositive(int value);
    static bool isPositive(double value);
    static bool isNonNegative(int value);
    static bool isNonNegative(double value);
    static bool isInteger(double value);
    static bool isFinite(double value);
    static bool isNormal(double value);

    // Configuration validation
    static bool validateConfigKey(const std::string& key);
    static bool validateConfigValue(const ConfigValue& value);
    static bool isValidConfigEntry(const ConfigEntry& entry);
    static bool validateConfigSchema(const ConfigSchema& schema);

    // Vector validation
    template<typename T>
    static bool isSorted(const std::vector<T>& values);

    template<typename T>
    static bool isUnique(const std::vector<T>& values);

    template<typename T>
    static bool containsValue(const std::vector<T>& values, const T& target);

    template<typename T>
    static size_t countLessThan(const std::vector<T>& values, const T& threshold);

    template<typename T>
    static size_t countGreaterThan(const std::vector<T>& values, const T& threshold);

    template<typename T>
    static std::vector<T> filterValues(const std::vector<T>& values, 
                                     std::function<bool(const T&)> predicate);

    template<typename T>
    static std::vector<T> findDuplicates(const std::vector<T>& values);

    // Type conversion validation
    static bool canConvertToInt(const std::string& str);
    static bool canConvertToDouble(const std::string& str);
    static bool canConvertToBool(const std::string& str);
    static bool canConvertToString(const ConfigValue& value);

    // Complex validation
    static bool validateNeuralRegionName(const std::string& name);
    static bool validateConnectionWeight(double weight);
    static bool validateSynapticDelay(double delay);
    static bool validateThreshold(double threshold);
    static bool validateLearningRate(double rate);

    // Batch validation
    static std::vector<std::string> validateAllKeys(const std::vector<std::string>& keys);
    static std::vector<ValidationError> validateAllEntries(const std::vector<ConfigEntry>& entries);
    static std::vector<std::string> findInvalidKeys(const std::vector<std::string>& keys);

    // Performance validation
    template<typename Container>
    static bool isEfficientSize(const Container& container, 
                               size_t maxSize = 1000000) {
        return container.size() <= maxSize;
    }

    template<typename Iterator>
    static bool isReasonableRange(Iterator begin, Iterator end,
                                 size_t maxSize = 1000000) {
        return std::distance(begin, end) <= maxSize;
    }

    // Statistical validation
    template<typename T>
    static bool isStatisticallySignificant(const std::vector<T>& values, 
                                         double confidence = 0.95) {
        // Simple statistical significance check (placeholder)
        return values.size() >= 30;  // Minimum sample size
    }

    // Memory validation
    static bool isReasonableMemorySize(size_t size);
    static bool isValidAllocationSize(size_t size);

    // Error handling utilities
    static std::string getValidationError(const std::string& field, const std::string& rule);
    static std::string formatValidationError(const std::string& field, 
                                            const std::string& value, 
                                            const std::string& rule);

    // Default validation rules
    static const std::vector<std::string> getDefaultRequiredFields();
    static const std::vector<std::string> getDefaultOptionalFields();
    static const std::vector<std::string> getDefaultRestrictedFields();
};

} // namespace Utils
} // namespace Core
} // namespace nlm