#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

// Validation result enumeration
enum class ValidationResult {
    Success,
    Warning,
    Error
};

// Configuration utility class
class ConfigUtils {
public:
    ConfigUtils();
    ~ConfigUtils();
    
    // Disable copying, enable moving
    ConfigUtils(const ConfigUtils&) = delete;
    ConfigUtils& operator=(const ConfigUtils&) = delete;
    ConfigUtils(ConfigUtils&&) noexcept;
    ConfigUtils& operator=(ConfigUtils&&) noexcept;
    
    // String manipulation utilities
    static std::string trim(const std::string& str, const std::string& chars = "");
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string normalizeKey(const std::string& key);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);
    
    // Validation utilities
    static bool containsOnlyDigits(const std::string& str);
    static bool containsOnlyDigitsAndDecimal(const std::string& str);
    static bool containsOnlyLetters(const std::string& str);
    static bool containsOnlyAlphanumeric(const std::string& str);
    
    // Advanced parsing utilities
    static std::vector<std::string> getAllPossibleKeys(const std::string& key);
    static std::string formatValue(const ConfigValue& value);
    static std::string validateString(const std::string& str, const std::string& key, ValidationResult& result);
    static std::string validateNumber(const std::string& str, const std::string& key, ValidationResult& result);
    static std::string validateBoolean(const std::string& str, const std::string& key, ValidationResult& result);
    static std::string validatePath(const std::string& str, const std::string& key, ValidationResult& result);
    static std::string validateFile(const std::string& str, const std::string& key, ValidationResult& result);
    static std::string validateEnum(const std::string& str, const std::string& key,
                                   const std::vector<std::string>& allowedValues, ValidationResult& result);
    static std::string validateRange(const std::string& str, const std::string& key,
                                    double min, double max, ValidationResult& result);
    
    // Command line argument parsing utilities
    static bool parseCommandLineArgs(const std::vector<std::string>& args,
                                    std::map<std::string, std::string>& parsedArgs);
    static bool validateCommandLineArgs(const std::map<std::string, std::string>& parsedArgs,
                                       const std::vector<std::string>& requiredKeys);
    static std::string generateHelpText(const std::map<std::string, std::string>& argDescriptions);
    static std::string generateErrorText(const std::vector<std::string>& errors);
    static std::string generateWarningText(const std::vector<std::string>& warnings);
    
    // Configuration convenience methods
    static bool loadFromAdvancedFile(const std::string& filepath, Config& config);
    static bool saveToAdvancedFile(const std::string& filepath, const Config& config);
    static bool validateConfigWithAdvancedRules(const Config& config, std::vector<ValidationError>& errors);
    static bool applyConfigOverrides(Config& config, const std::map<std::string, std::string>& overrides);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm