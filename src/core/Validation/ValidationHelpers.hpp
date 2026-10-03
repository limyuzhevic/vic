// Helper functions for schema validation
#pragma once

#include <string>
#include <vector>
#include <variant>
#include "Config.hpp"

namespace nlm {

// Helper functions for common validation
bool validatePositive(const ConfigValue& value, std::string& errorMsg);
bool validateRange(const ConfigValue& value, double min, double max, std::string& errorMsg);
bool validateEnum(const ConfigValue& value, const std::vector<std::string>& allowed, std::string& errorMsg);
bool validatePath(const ConfigValue& value, std::string& errorMsg);
bool validateEmail(const ConfigValue& value, std::string& errorMsg);
bool validateUUID(const ConfigValue& value, std::string& errorMsg);

// Configuration type checking
std::string getValueTypeName(const ConfigValue& value);
bool isNumericType(const std::string& type);
bool isVectorType(const std::string& type);
bool isMapType(const std::string& type);

// Configuration conversion
double toDouble(const ConfigValue& value);
int toInt(const ConfigValue& value);
std::string toString(const ConfigValue& value);
bool toBool(const ConfigValue& value);

// String utility functions
std::string trim(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
bool isEmail(const std::string& str);
bool isUUID(const std::string& str);
bool isPath(const std::string& str);

// Validation helper functions
bool isCriticalError(const std::string& error);
bool isWarning(const std::string& error);

// Configuration validation macros
#define VALIDATE_TYPE(value, expectedType) validateType(value, expectedType)
#define VALIDATE_RANGE(value, min, max) validateRange(value, min, max)
#define VALIDATE_ENUM(value, allowed) validateEnum(value, allowed)
#define VALIDATE_FORMAT(value, format) validateFormat(value, format)

// Configuration validation functions
bool validateType(const ConfigValue& value, const std::string& expectedType);
bool validateFormat(const ConfigValue& value, const std::string& format);
bool validateDependencies(const std::unordered_map<std::string, ConfigValue>& config,
                         const std::vector<std::string>& dependencies);
bool validateMutuallyExclusive(const std::unordered_map<std::string, ConfigValue>& config,
                              const std::vector<std::vector<std::string>>& groups);
bool validateRequiredCombinations(const std::unordered_map<std::string, ConfigValue>& config,
                                 const std::vector<std::vector<std::string>>& combinations);

} // namespace nlm
