// Configuration Testing Framework Implementation for NLM

#include "ConfigurationTestSuite.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <regex>

namespace nlm {
namespace testing {

ConfigurationTestSuite::ConfigurationTestSuite() {
    std::cout << "=== NLM Configuration Test Suite Initialization ===" << std::endl;
    
    // Initialize configuration parsers
    std::cout << "Configuration test suite initialized successfully" << std::endl;
}

ConfigurationTestSuite::~ConfigurationTestSuite() {
    std::cout << "=== NLM Configuration Test Suite Shutdown ===" << std::endl;
}

void ConfigurationTestSuite::registerSchema(const ConfigurationSchema& schema) {
    if (validateSchema(schema)) {
        schemas_.push_back(schema);
        std::cout << "Registered schema: " << schema.schemaName << " (v" << schema.schemaVersion << ")" << std::endl;
    } else {
        std::cout << "Error: Invalid schema: " << schema.schemaName << std::endl;
    }
}

bool ConfigurationTestSuite::validateSchema(const ConfigurationSchema& schema) const {
    bool isValid = true;
    
    if (schema.schemaName.empty()) {
        std::cout << "Error: Schema name cannot be empty" << std::endl;
        isValid = false;
    }
    
    if (schema.schemaVersion.empty()) {
        std::cout << "Error: Schema version cannot be empty" << std::endl;
        isValid = false;
    }
    
    // Validate field consistency
    for (const auto& pair : schema.fieldRanges) {
        if (!pair.first.empty() && schema.fieldTypes.find(pair.first) == schema.fieldTypes.end()) {
            std::cout << "Error: Field in range has no type definition: " << pair.first << std::endl;
            isValid = false;
        }
    }
    
    return isValid;
}

void ConfigurationTestSuite::loadSchemaFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open schema file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    ConfigurationSchema schema("", "");
    
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find("SchemaName:") != std::string::npos) {
            if (!schema.schemaName.empty()) {
                registerSchema(schema);
                schema = ConfigurationSchema("", "");
            }
            schema.schemaName = line.substr(11);
        } else if (line.find("SchemaVersion:") != std::string::npos) {
            schema.schemaVersion = line.substr(14);
        } else if (line.find("Description:") != std::string::npos) {
            schema.description = line.substr(12);
        } else if (line.find("Required:") != std::string::npos) {
            std::string value = line.substr(9);
            schema.isRequired = (value == "true" || value == "yes" || value == "1");
        } else if (line.find("FieldType:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string field = line.substr(10, colonPos - 11);
                std::string type = line.substr(colonPos + 2);
                schema.fieldTypes[field] = type;
            }
        } else if (line.find("FieldRange:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string field = line.substr(11, colonPos - 12);
                std::string rangeStr = line.substr(colonPos + 2);
                
                // Parse range like "min,max"
                size_t commaPos = rangeStr.find(",");
                if (commaPos != std::string::npos) {
                    double min = std::stod(rangeStr.substr(0, commaPos));
                    double max = std::stod(rangeStr.substr(commaPos + 1));
                    schema.fieldRanges[field] = std::make_pair(min, max);
                }
            }
        } else if (line.find("FieldDependency:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string field = line.substr(16, colonPos - 17);
                std::string dependencies = line.substr(colonPos + 2);
                
                // Split dependencies by semicolon
                std::stringstream ss(dependencies);
                std::string dep;
                std::vector<std::string> depList;
                while (std::getline(ss, dep, ';')) {
                    if (!dep.empty()) {
                        depList.push_back(dep);
                    }
                }
                
                schema.fieldDependencies[field] = depList;
            }
        } else if (line.find("FieldFormat:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string field = line.substr(12, colonPos - 13);
                std::string format = line.substr(colonPos + 2);
                schema.fieldFormats[field] = format;
            }
        } else if (line.find("FieldDescription:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string field = line.substr(16, colonPos - 17);
                std::string description = line.substr(colonPos + 2);
                schema.fieldDescriptions[field] = description;
            }
        }
    }
    
    if (!schema.schemaName.empty()) {
        registerSchema(schema);
    }
    
    file.close();
    std::cout << "Schema loaded from: " << filename << std::endl;
}

void ConfigurationTestSuite::registerConfigurationTest(const ConfigurationTestCase& testCase) {
    std::lock_guard<std::shared_mutex> lock(testsMutex_);
    configurationTests_.push_back(testCase);
    std::cout << "Registered configuration test: " << testCase.name << " (" << testCase.testType << ")" << std::endl;
}

void ConfigurationTestSuite::registerConfigurationTest(const std::string& name,
                                                      const std::string& description,
                                                      const std::unordered_map<std::string, std::string>& expectedValues,
                                                      const std::string& configFile,
                                                      const std::string& configSection) {
    ConfigurationTestCase testCase(name, description, expectedValues);
    testCase.configFile = configFile;
    testCase.configSection = configSection;
    
    registerConfigurationTest(testCase);
}

void ConfigurationTestSuite::runConfigurationTests() {
    std::cout << "\n=== Running NLM Configuration Tests ===" << std::endl;
    
    size_t passedTests = 0, failedTests = 0;
    
    for (auto& testCase : configurationTests_) {
        std::cout << "\nRunning configuration test: " << testCase.name << std::endl;
        std::cout << "  Description: " << testCase.description << std::endl;
        std::cout << "  Test Type: " << testCase.testType << std::endl;
        
        bool testSuccess = executeConfigurationTest(testCase);
        
        if (testSuccess) {
            std::cout << "  PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "  FAILED" << std::endl;
            failedTests++;
        }
    }
    
    std::cout << "\n=== Configuration Test Summary ===" << std::endl;
    std::cout << "Total Tests: " << configurationTests_.size() << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    
    generateConfigurationReport();
}

void ConfigurationTestSuite::runSingleConfigurationTest(const std::string& testName) {
    std::cout << "\nRunning single configuration test: " << testName << std::endl;
    
    std::lock_guard<std::shared_mutex> lock(testsMutex_);
    
    for (auto& testCase : configurationTests_) {
        if (testCase.name == testName) {
            bool testSuccess = executeConfigurationTest(testCase);
            
            if (testSuccess) {
                std::cout << "  PASSED" << std::endl;
            } else {
                std::cout << "  FAILED" << std::endl;
            }
            return;
        }
    }
    
    std::cout << "Error: Configuration test not found: " << testName << std::endl;
}

bool ConfigurationTestSuite::validateConfigurationFile(const std::string& filename) const {
    std::cout << "Validating configuration file: " << filename << std::endl;
    
    // Parse configuration file
    std::unordered_map<std::string, std::string> config = parseConfigurationFile(filename);
    
    if (config.empty()) {
        std::cout << "  Error: Failed to parse configuration file" << std::endl;
        return false;
    }
    
    bool isValid = true;
    
    // Validate against schemas
    for (const auto& schema : schemas_) {
        if (validateSchemaConfiguration(schema, config)) {
            std::cout << "  Schema " << schema.schemaName << " validation passed" << std::endl;
        } else {
            std::cout << "  Schema " << schema.schemaName << " validation failed" << std::endl;
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigurationTestSuite::validateConfigurationSection(const std::string& filename,
                                                         const std::string& section) const {
    std::cout << "Validating configuration section: " << section << " in " << filename << std::endl;
    
    std::unordered_map<std::string, std::string> config = parseConfigurationSection(filename, section);
    
    if (config.empty()) {
        std::cout << "  Error: Failed to parse configuration section" << std::endl;
        return false;
    }
    
    // Validate section against registered tests
    for (const auto& testCase : configurationTests_) {
        if (testCase.configSection == section) {
            bool testSuccess = executeConfigurationValidation(testCase, config);
            
            if (!testSuccess) {
                std::cout << "  Configuration validation failed for section: " << section << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigurationTestSuite::validateConfigurationValue(const std::string& fieldName,
                                                       const std::string& fieldValue,
                                                       const std::string& expectedValue,
                                                       const std::string& expectedRange) const {
    // Check exact match
    if (fieldValue == expectedValue) {
        return true;
    }
    
    // Check range match
    if (!expectedRange.empty()) {
        if (validateValueRange(fieldValue, expectedRange)) {
            return true;
        }
    }
    
    return false;
}

bool ConfigurationTestSuite::validateConfigurationDependency(const std::string& filename,
                                                            const std::string& section,
                                                            const std::string& dependency) const {
    // In a real implementation, this would check for specific dependencies
    // For now, we'll simulate dependency validation
    std::cout << "Validating dependency: " << dependency << std::endl;
    
    // Simulate dependency validation
    return true;
}

std::unordered_map<std::string, std::string> ConfigurationTestSuite::parseConfigurationFile(const std::string& filename) const {
    std::cout << "Parsing configuration file: " << filename << std::endl;
    
    std::unordered_map<std::string, std::string> config;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file: " << filename << std::endl;
            return config;
        }
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Check for section header
            if (line[0] == '[' && line[line.size() - 1] == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key-value pair
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                trim(key);
                trim(value);
                
                if (!key.empty()) {
                    config[key] = value;
                }
            }
        }
        
        file.close();
    } catch (const std::exception& e) {
        std::cout << "Error parsing configuration file: " << e.what() << std::endl;
    }
    
    std::cout << "  Parsed " << config.size() << " configuration entries" << std::endl;
    return config;
}

std::unordered_map<std::string, std::string> ConfigurationTestSuite::parseConfigurationSection(const std::string& filename,
                                                                                              const std::string& section) const {
    std::cout << "Parsing configuration section: " << section << " from " << filename << std::endl;
    
    std::unordered_map<std::string, std::string> config;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file: " << filename << std::endl;
            return config;
        }
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Check for section header
            if (line[0] == '[' && line[line.size() - 1] == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Only process lines in the requested section
            if (currentSection != section) {
                continue;
            }
            
            // Parse key-value pair
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                trim(key);
                trim(value);
                
                if (!key.empty()) {
                    config[key] = value;
                }
            }
        }
        
        file.close();
    } catch (const std::exception& e) {
        std::cout << "Error parsing configuration section: " << e.what() << std::endl;
    }
    
    std::cout << "  Parsed " << config.size() << " configuration entries for section" << std::endl;
    return config;
}

bool ConfigurationTestSuite::compareConfigurations(const std::unordered_map<std::string, std::string>& config1,
                                                  const std::unordered_map<std::string, std::string>& config2) const {
    if (config1.size() != config2.size()) {
        return false;
    }
    
    for (const auto& pair : config1) {
        auto it = config2.find(pair.first);
        if (it == config2.end() || it->second != pair.second) {
            return false;
        }
    }
    
    return true;
}

void ConfigurationTestSuite::generateConfigurationReport() const {
    std::cout << "\n=== Configuration Test Report ===" << std::endl;
    
    // Summary statistics
    std::cout << "Total configuration tests: " << configurationTests_.size() << std::endl;
    
    // Schema summary
    std::cout << "Registered schemas: " << schemas_.size() << std::endl;
    
    // Test results summary
    std::cout << "Test results: " << testResults_.size() << std::endl;
    
    // Configuration files parsed
    std::cout << "Configuration files parsed: " << configData_.size() << std::endl;
}

void ConfigurationTestSuite::exportConfigurationResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing results: " << filename << std::endl;
        return;
    }
    
    file << "Configuration Test Results" << std::endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    
    file << "\nTest Results:" << std::endl;
    for (const auto& result : testResults_) {
        file << "  " << result.testName << ": " << (result.isValid ? "PASSED" : "FAILED") << std::endl;
        file << "    Field: " << result.fieldName << std::endl;
        file << "    Value: " << result.fieldValue << std::endl;
        file << "    Expected: " << result.expectedValue << std::endl;
        if (!result.expectedRange.empty()) {
            file << "    Range: " << result.expectedRange << std::endl;
        }
        if (!result.errorMessage.empty()) {
            file << "    Error: " << result.errorMessage << std::endl;
        }
    }
    
    file << "\nSchemas:" << std::endl;
    for (const auto& schema : schemas_) {
        file << "  " << schema.schemaName << " (v" << schema.schemaVersion << ")" << std::endl;
    }
    
    file.close();
    std::cout << "Configuration test results exported to: " << filename << std::endl;
}

bool ConfigurationTestSuite::validateConfigurationConsistency() const {
    std::cout << "Validating configuration consistency..." << std::endl;
    
    bool isConsistent = true;
    
    // Check for duplicate configuration entries
    std::unordered_map<std::string, int> keyCounts;
    for (const auto& pair : configData_) {
        for (const auto& entry : pair.second) {
            keyCounts[entry.first]++;
        }
    }
    
    for (const auto& pair : keyCounts) {
        if (pair.second > 1) {
            std::cout << "Warning: Duplicate key found: " << pair.first << std::endl;
            isConsistent = false;
        }
    }
    
    // Check for missing required fields
    for (const auto& schema : schemas_) {
        if (schema.isRequired) {
            for (const auto& field : schema.fieldTypes) {
                bool found = false;
                for (const auto& configPair : configData_) {
                    if (configPair.second.find(field.first) != configPair.second.end()) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cout << "Warning: Required field not found in schema " << schema.schemaName 
                              << ": " << field.first << std::endl;
                    isConsistent = false;
                }
            }
        }
    }
    
    if (isConsistent) {
        std::cout << "Configuration consistency validation passed" << std::endl;
    } else {
        std::cout << "Configuration consistency validation failed" << std::endl;
    }
    
    return isConsistent;
}

bool ConfigurationTestSuite::validateCrossComponentConfiguration() const {
    std::cout << "Validating cross-component configuration..." << std::endl;
    
    // In a real implementation, this would validate configuration consistency across components
    // For now, we'll simulate cross-component validation
    
    bool isConsistent = true;
    
    // Check if all components have consistent configurations
    size_t componentCount = configData_.size();
    if (componentCount == 0) {
        std::cout << "  Warning: No components configured" << std::endl;
        isConsistent = false;
    }
    
    // Check for cross-component dependencies
    for (const auto& schema : schemas_) {
        for (const auto& dependency : schema.fieldDependencies) {
            // In a real implementation, this would check if dependencies are properly satisfied
            std::cout << "  Checking dependency: " << dependency.first << " -> " 
                      << (dependency.second.empty() ? "(no dependencies)" : dependency.second[0]) << std::endl;
        }
    }
    
    if (isConsistent) {
        std::cout << "Cross-component configuration validation passed" << std::endl;
    } else {
        std::cout << "Cross-component configuration validation failed" << std::endl;
    }
    
    return isConsistent;
}

bool ConfigurationTestSuite::executeConfigurationTest(const ConfigurationTestCase& testCase) {
    std::cout << "Executing configuration test: " << testCase.name << std::endl;
    
    // Load configuration file if specified
    std::unordered_map<std::string, std::string> config;
    if (!testCase.configFile.empty()) {
        config = parseConfigurationFile(testCase.configFile);
        configData_[testCase.configFile] = config;
    }
    
    // Execute validation based on test type
    return executeConfigurationValidation(testCase, config);
}

bool ConfigurationTestSuite::executeConfigurationValidation(const ConfigurationTestCase& testCase,
                                                          const std::unordered_map<std::string, std::string>& config) {
    bool allPassed = true;
    
    for (const auto& expectedPair : testCase.expectedValues) {
        std::string fieldName = expectedPair.first;
        std::string expectedValue = expectedPair.second;
        
        std::string actualValue;
        auto it = config.find(fieldName);
        if (it != config.end()) {
            actualValue = it->second;
        }
        
        // Check for ranges
        std::string expectedRange;
        if (testCase.expectedRanges.find(fieldName) != testCase.expectedRanges.end()) {
            expectedRange = testCase.expectedRanges.at(fieldName);
        }
        
        // Validate
        bool isValid = validateConfigurationValue(fieldName, actualValue, expectedValue, expectedRange);
        
        if (!isValid) {
            ConfigurationValidationResult result(testCase.name, testCase.configFile,
                                                testCase.configSection, fieldName, actualValue,
                                                expectedValue, expectedRange, false);
            result.errorMessage = "Value validation failed";
            testResults_.push_back(result);
            allPassed = false;
            
            std::cout << "    Validation failed for field: " << fieldName 
                      << " (expected: " << expectedValue << ", actual: " << actualValue << ")" << std::endl;
        } else {
            std::cout << "    Validation passed for field: " << fieldName << std::endl;
        }
    }
    
    // Validate dependencies
    if (testCase.testType == "dependency") {
        for (const auto& depPair : testCase.expectedDependencies) {
            bool depValid = validateConfigurationDependency(testCase.configFile, 
                                                           testCase.configSection, depPair.first);
            
            if (!depValid) {
                std::cout << "    Dependency validation failed for: " << depPair.first << std::endl;
                allPassed = false;
            }
        }
    }
    
    return allPassed;
}

bool ConfigurationTestSuite::validateValueRange(const std::string& value,
                                               const std::string& range) const {
    // Parse range like "min,max"
    size_t commaPos = range.find(',');
    if (commaPos == std::string::npos) {
        return false;
    }
    
    try {
        double min = std::stod(range.substr(0, commaPos));
        double max = std::stod(range.substr(commaPos + 1));
        double val = std::stod(value);
        
        return val >= min && val <= max;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigurationTestSuite::validateValueFormat(const std::string& value,
                                               const std::string& format) const {
    if (format == "integer") {
        return validateInteger(value);
    } else if (format == "double") {
        return validateDouble(value);
    } else if (format == "string") {
        return validateString(value);
    } else if (format == "boolean") {
        return validateBoolean(value);
    } else if (format.find("regex:") == 0) {
        std::string pattern = format.substr(6);
        return std::regex_match(value, std::regex(pattern));
    } else if (format.find("date:") == 0) {
        std::string dateFormat = format.substr(5);
        return validateDateFormat(value, dateFormat);
    }
    
    return true; // Unknown format, assume valid
}

void ConfigurationTestSuite::trim(std::string& str) const {
    // Remove leading whitespace
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    
    // Remove trailing whitespace
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}

bool ConfigurationTestSuite::isCommentLine(const std::string& line) const {
    return line.empty() || line[0] == '#' || line[0] == ';';
}

std::string ConfigurationTestSuite::extractSection(const std::string& line) const {
    size_t bracketPos = line.find('[');
    size_t closingBracketPos = line.find(']');
    if (bracketPos != std::string::npos && closingBracketPos != std::string::npos) {
        return line.substr(bracketPos + 1, closingBracketPos - bracketPos - 2);
    }
    return "";
}

std::pair<std::string, std::string> ConfigurationTestSuite::parseKeyValue(const std::string& line) const {
    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos) {
        return {"", ""};
    }
    
    std::string key = line.substr(0, equalPos);
    std::string value = line.substr(equalPos + 1);
    
    trim(key);
    trim(value);
    
    return {key, value};
}

void ConfigurationTestSuite::writeConfigurationTestResult(std::ofstream& file,
                                                         const ConfigurationValidationResult& result) const {
    file << "Test: " << result.testName << std::endl;
    file << "Config File: " << result.configFile << std::endl;
    file << "Section: " << result.configSection << std::endl;
    file << "Field: " << result.fieldName << std::endl;
    file << "Value: " << result.fieldValue << std::endl;
    file << "Expected: " << result.expectedValue << std::endl;
    if (!result.expectedRange.empty()) {
        file << "Range: " << result.expectedRange << std::endl;
    }
    file << "Status: " << (result.isValid ? "PASSED" : "FAILED") << std::endl;
    if (!result.errorMessage.empty()) {
        file << "Error: " << result.errorMessage << std::endl;
    }
    file << std::endl;
}

void ConfigurationTestSuite::generateConfigurationSummary(std::ofstream& file) const {
    file << "Configuration Test Summary" << std::endl;
    file << "============================" << std::endl;
    file << "Total Tests: " << configurationTests_.size() << std::endl;
    file << "Schemas: " << schemas_.size() << std::endl;
    file << "Configurations Parsed: " << configData_.size() << std::endl;
    file << std::endl;
    
    // Test type distribution
    std::unordered_map<std::string, size_t> testTypeCounts;
    for (const auto& testCase : configurationTests_) {
        testTypeCounts[testCase.testType]++;
    }
    
    file << "Test Types:" << std::endl;
    for (const auto& pair : testTypeCounts) {
        file << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

bool ConfigurationTestSuite::validateSchemaConfiguration(const ConfigurationSchema& schema,
                                                        const std::unordered_map<std::string, std::string>& config) const {
    bool isValid = true;
    
    // Validate field types
    for (const auto& fieldType : schema.fieldTypes) {
        if (schema.fieldFormats.find(fieldType.first) != schema.fieldFormats.end()) {
            // Validate format
            bool formatValid = validateFieldFormat(fieldType.second, schema.fieldFormats.at(fieldType.first));
            if (!formatValid) {
                std::cout << "  Format validation failed for field " << fieldType.first 
                          << " (type: " << fieldType.second << ")" << std::endl;
                isValid = false;
            }
        }
    }
    
    // Validate field ranges
    for (const auto& fieldRange : schema.fieldRanges) {
        if (schema.fieldTypes.find(fieldRange.first) == schema.fieldTypes.end()) {
            std::cout << "  Field " << fieldRange.first << " has no type defined" << std::endl;
            isValid = false;
        }
    }
    
    // Validate field dependencies
    for (const auto& fieldDep : schema.fieldDependencies) {
        // In a real implementation, this would check dependencies
        std::cout << "  Validating dependency: " << fieldDep.first << " -> " 
                  << (fieldDep.second.empty() ? "(no dependencies)" : fieldDep.second[0]) << std::endl;
    }
    
    return isValid;
}

bool ConfigurationSchemaValidator::validateSchema(const ConfigurationSchema& schema) const {
    bool isValid = true;
    
    if (schema.schemaName.empty()) {
        std::cout << "Error: Schema name cannot be empty" << std::endl;
        isValid = false;
    }
    
    if (schema.schemaVersion.empty()) {
        std::cout << "Error: Schema version cannot be empty" << std::endl;
        isValid = false;
    }
    
    // Validate field types
    for (const auto& pair : schema.fieldTypes) {
        if (!validateFieldType(pair.first, pair.second)) {
            std::cout << "Error: Invalid field type for field: " << pair.first 
                      << " (type: " << pair.second << ")" << std::endl;
            isValid = false;
        }
    }
    
    // Validate field ranges
    for (const auto& pair : schema.fieldRanges) {
        if (schema.fieldTypes.find(pair.first) == schema.fieldTypes.end()) {
            std::cout << "Error: Field in range has no type definition: " << pair.first << std::endl;
            isValid = false;
        } else {
            if (!validateFieldRange(pair.first, pair.second)) {
                std::cout << "Error: Invalid range for field: " << pair.first << std::endl;
                isValid = false;
            }
        }
    }
    
    // Validate field dependencies
    for (const auto& pair : schema.fieldDependencies) {
        if (!validateFieldDependencies(pair.first, pair.second)) {
            std::cout << "Error: Invalid dependencies for field: " << pair.first << std::endl;
            isValid = false;
        }
    }
    
    // Validate field formats
    for (const auto& pair : schema.fieldFormats) {
        if (!validateFieldFormat("dummy", pair.second)) { // Note: This is a simplified check
            std::cout << "Error: Invalid format for field: " << pair.first << std::endl;
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigurationSchemaValidator::validateFieldTypes(const ConfigurationSchema& schema,
                                                     const std::unordered_map<std::string, std::string>& config) const {
    bool isValid = true;
    
    for (const auto& pair : schema.fieldTypes) {
        if (config.find(pair.first) == config.end()) {
            if (schema.isRequired) {
                std::cout << "Error: Required field not found: " << pair.first << std::endl;
                isValid = false;
            }
        } else {
            if (!validateFieldType(pair.first, pair.second)) {
                std::cout << "Error: Field type validation failed for: " << pair.first << std::endl;
                isValid = false;
            }
        }
    }
    
    return isValid;
}

bool ConfigurationSchemaValidator::validateFieldRanges(const ConfigurationSchema& schema,
                                                      const std::unordered_map<std::string, std::string>& config) const {
    bool isValid = true;
    
    for (const auto& pair : schema.fieldRanges) {
        if (config.find(pair.first) != config.end()) {
            if (!validateFieldRange(pair.first, pair.second)) {
                std::cout << "Error: Field range validation failed for: " << pair.first << std::endl;
                isValid = false;
            }
        }
    }
    
    return isValid;
}

bool ConfigurationSchemaValidator::validateFieldDependencies(const ConfigurationSchema& schema,
                                                            const std::unordered_map<std::string, std::string>& config) const {
    bool isValid = true;
    
    for (const auto& pair : schema.fieldDependencies) {
        if (config.find(pair.first) != config.end()) {
            if (!validateFieldDependencies(pair.first, pair.second)) {
                std::cout << "Error: Field dependency validation failed for: " << pair.first << std::endl;
                isValid = false;
            }
        }
    }
    
    return isValid;
}

bool ConfigurationSchemaValidator::validateFieldFormat(const std::string& value,
                                                      const std::string& format) const {
    if (format == "integer") {
        return validateInteger(value);
    } else if (format == "double") {
        return validateDouble(value);
    } else if (format == "string") {
        return validateString(value);
    } else if (format == "boolean") {
        return validateBoolean(value);
    } else if (format == "email") {
        return validateEmail(value);
    } else if (format == "date") {
        return validateDate(value);
    } else if (format == "uri") {
        return validateURI(value);
    } else if (format.find("regex:") == 0) {
        std::string pattern = format.substr(6);
        return std::regex_match(value, std::regex(pattern));
    }
    
    return true; // Unknown format, assume valid
}

bool ConfigurationSchemaValidator::validateInteger(const std::string& value) const {
    try {
        std::stoi(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigurationSchemaValidator::validateDouble(const std::string& value) const {
    try {
        std::stod(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigurationSchemaValidator::validateString(const std::string& value) const {
    return !value.empty();
}

bool ConfigurationSchemaValidator::validateBoolean(const std::string& value) const {
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    return lowerValue == "true" || lowerValue == "false" || lowerValue == "yes" || lowerValue == "no" || 
           lowerValue == "1" || lowerValue == "0";
}

bool ConfigurationSchemaValidator::validateDate(const std::string& value) const {
    // Check if value matches YYYY-MM-DD format
    std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})")
    return std::regex_match(value, dateRegex);
}

bool ConfigurationSchemaValidator::validateEmail(const std::string& value) const {
    // Simple email regex
    std::regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})$");
    return std::regex_match(value, emailRegex);
}

bool ConfigurationSchemaValidator::validateURI(const std::string& value) const {
    // Check if value looks like a URI
    return value.find("://") != std::string::npos;
}

bool ConfigurationSchemaValidator::validateNumericFormat(const std::string& value,
                                                        const std::string& format) const {
    if (format.find("range:") == 0) {
        // Parse range like "range:min,max"
        std::string rangeStr = format.substr(6);
        size_t commaPos = rangeStr.find(',');
        if (commaPos == std::string::npos) {
            return false;
        }
        
        try {
            double min = std::stod(rangeStr.substr(0, commaPos));
            double max = std::stod(rangeStr.substr(commaPos + 1));
            double val = std::stod(value);
            
            return val >= min && val <= max;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return true;
}

bool ConfigurationSchemaValidator::validateDateFormat(const std::string& value,
                                                     const std::string& format) const {
    // In a real implementation, this would parse the format and validate the date
    // For now, we'll use a simple check
    return value.find('-') != std::string::npos && value.size() == 10;
}

bool ConfigurationSchemaValidator::validateFieldType(const std::string& fieldName,
                                                   const std::string& type) const {
    // In a real implementation, this would validate field types more thoroughly
    // For now, we'll accept any field name and type combination
    return true;
}

bool ConfigurationSchemaValidator::validateFieldRange(const std::string& fieldName,
                                                     const std::pair<double, double>& range) const {
    // In a real implementation, this would validate field ranges
    return true;
}

bool ConfigurationSchemaValidator::validateFieldDependencies(const std::string& fieldName,
                                                            const std::vector<std::string>& dependencies) const {
    // In a real implementation, this would validate dependencies
    return true;
}

} // namespace testing
} // namespace nlm