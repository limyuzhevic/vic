// User-friendly error reporting utilities
// Provides clear, actionable error messages for users

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>

/**
 * Error message formatter for user-friendly output
 */
class ErrorFormatter {
public:
    // Format an error message with helpful context
    static std::string formatError(const std::string& message, 
                                   const std::string& context = "") {
        std::ostringstream oss;
        oss << "Error: " << message;
        if (!context.empty()) {
            oss << "\n  Context: " << context;
        }
        return oss.str();
    }
    
    // Format a suggestion-based error message
    static std::string formatWithSuggestion(const std::string& message,
                                           const std::string& suggestion,
                                           const std::string& context = "") {
        std::ostringstream oss;
        oss << "Error: " << message;
        if (!context.empty()) {
            oss << "\n  Context: " << context;
        }
        oss << "\n  Suggestion: " << suggestion;
        return oss.str();
    }
    
    // Format validation errors with specific field information
    static std::string formatValidationError(const std::string& fieldName,
                                            const std::string& error,
                                            const std::map<std::string, std::string>& fieldValues = {}) {
        std::ostringstream oss;
        oss << "Invalid value for field '" << fieldName << "': " << error;
        
        if (!fieldValues.empty() && fieldValues.find(fieldName) != fieldValues.end()) {
            oss << "\n  Current value: '" << fieldValues.at(fieldName) << "'";
        }
        
        return oss.str();
    }
    
    // Format usage error with examples
    static std::string formatUsageError(const std::string& command,
                                       const std::string& expectedFormat,
                                       const std::vector<std::string>& examples = {}) {
        std::ostringstream oss;
        oss << "Usage error for command: " << command << "\n";
        oss << "  Expected format: " << expectedFormat << "\n";
        
        if (!examples.empty()) {
            oss << "  Examples:";
            for (const auto& example : examples) {
                oss << "\n    " << example;
            }
        }
        
        return oss.str();
    }
    
    // Format file I/O errors with path suggestions
    static std::string formatFileError(const std::string& operation,
                                       const std::string& path,
                                       const std::string& reason = "") {
        std::ostringstream oss;
        oss << "File operation failed: " << operation << "\n";
        oss << "  File: " << path;
        
        if (!reason.empty()) {
            oss << "\n  Reason: " << reason;
        }
        
        // Provide common suggestions based on error type
        if (reason.find("not found") != std::string::npos || 
            reason.find("No such file") != std::string::npos) {
            oss << "\n  Suggestion: Check that the file exists and is readable";
        } else if (reason.find("permission") != std::string::npos || 
                   reason.find("access denied") != std::string::npos) {
            oss << "\n  Suggestion: Check file permissions or run with appropriate privileges";
        } else if (reason.find("disk space") != std::string::npos || 
                   reason.find("no space") != std::string::npos) {
            oss << "\n  Suggestion: Free up disk space or specify a different location";
        }
        
        return oss.str();
    }
    
    // Format configuration validation errors
    static std::string formatConfigValidation(const std::vector<std::string>& errors) {
        if (errors.empty()) return "";
        
        std::ostringstream oss;
        oss << "Configuration validation failed:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
        
        oss << "\n\nSuggestion: Check configuration file or command-line arguments";
        oss << "\n  See --help for valid options and examples.";
        
        return oss.str();
    }
    
    // Format numerical value errors
    static std::string formatNumberError(const std::string& fieldName,
                                        double value,
                                        double min = -std::numeric_limits<double>::max(),
                                        double max = std::numeric_limits<double>::max()) {
        std::ostringstream oss;
        oss << "Invalid value for '" << fieldName << "': " << value << "\n";
        
        if (value < min) {
            oss << "  Minimum allowed value: " << min;
        } else if (value > max) {
            oss << "  Maximum allowed value: " << max;
        }
        
        oss << "\n  Suggestion: Provide a value between " << min << " and " << max;
        
        return oss.str();
    }
    
    // Format string validation errors
    static std::string formatStringError(const std::string& fieldName,
                                        const std::string& value,
                                        const std::string& pattern = "") {
        std::ostringstream oss;
        oss << "Invalid value for '" << fieldName << "': '" << value << "'";
        
        if (!pattern.empty()) {
            oss << "\n  Expected format: " << pattern;
        }
        
        oss << "\n  Suggestion: Check the format and try again";
        
        return oss.str();
    }
    
    // Format option parsing errors
    static std::string formatOptionError(const std::string& option,
                                        const std::string& value,
                                        const std::string& expectedType = "") {
        std::ostringstream oss;
        oss << "Invalid value for option '--" << option << "': '" << value << "'";
        
        if (!expectedType.empty()) {
            oss << "\n  Expected type: " << expectedType;
        }
        
        oss << "\n  Suggestion: Check option syntax in help (--help)";
        
        return oss.str();
    }
};

// User-friendly error printer
namespace UserFriendlyErrors {
    
    // Print error with formatting
    void printError(const std::string& message, bool showHelp = true) {
        std::cerr << "Error: " << message << std::endl;
        if (showHelp) {
            std::cerr << std::endl;
            std::cerr << "Use '--help' for usage information." << std::endl;
        }
    }
    
    // Print validation errors
    void printValidationErrors(const std::vector<std::string>& errors, bool showHelp = true) {
        if (errors.empty()) return;
        
        std::cerr << "Validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  - " << error << std::endl;
        }
        
        std::cerr << std::endl;
        if (showHelp) {
            std::cerr << "Use '--help' for valid options and examples." << std::endl;
        }
    }
    
    // Print configuration file error
    void printConfigFileError(const std::string& filepath, const std::string& error, bool showHelp = true) {
        std::cerr << "Failed to load configuration from: " << filepath << std::endl;
        std::cerr << "  Reason: " << error << std::endl;
        std::cerr << std::endl;
        std::cerr << "Using default configuration." << std::endl;
        if (showHelp) {
            std::cerr << stdendl;
            std::cerr << "To specify a different configuration file:" << std::endl;
            std::cerr << "  --config /path/to/config.cfg" << std::endl;
        }
    }
    
    // Print option parsing error with suggestions
    void printOptionError(const std::string& option, const std::string& value, 
                         const std::string& expectedType = "", bool showHelp = true) {
        std::cerr << "Invalid option value:" << std::endl;
        std::cerr << "  Option: --" << option << std::endl;
        std::cerr << "  Value:  '" << value << "'" << std::endl;
        
        if (!expectedType.empty()) {
            std::cerr << "  Expected: " << expectedType << std::endl;
        }
        
        std::cerr << std::endl;
        if (showHelp) {
            std::cerr << "Use '--help' to see all available options." << std::endl;
        }
    }
    
    // Print numeric value error
    void printNumberError(const std::string& fieldName, double value,
                         double min = -std::numeric_limits<double>::max(),
                         double max = std::numeric_limits<double>::max(),
                         bool showHelp = true) {
        std::cerr << "Invalid value for '" << fieldName << "': " << value << std::endl;
        
        if (value < min) {
            std::cerr << "  Minimum allowed: " << min << std::endl;
        } else if (value > max) {
            std::cerr << "  Maximum allowed: " << max << std::endl;
        }
        
        std::cerr << std::endl;
        std::cerr << "Suggestion: Provide a value between " << min << " and " << max;
        
        if (showHelp) {
            std::cerr << std::endl;
            std::cerr << "Check the configuration documentation for valid ranges.";
        }
    }
    
    // Print string format error
    void printStringError(const std::string& fieldName, const std::string& value,
                         const std::string& expectedFormat = "",
                         bool showHelp = true) {
        std::cerr << "Invalid format for '" << fieldName << "': '" << value << "'" << std::endl;
        
        if (!expectedFormat.empty()) {
            std::cerr << "  Expected format: " << expectedFormat << std::endl;
        }
        
        std::cerr << std::endl;
        std::cerr << "Suggestion: Check the format and try again";
        
        if (showHelp) {
            std::cerr << std::endl;
            std::cerr << "Use '--help' for examples of valid formats.";
        }
    }
    
    // Print memory error with actionable advice
    void printMemoryError(const std::string& operation, const std::string& details,
                         bool showHelp = true) {
        std::cerr << "Memory error during " << operation << ":" << std::endl;
        std::cerr << "  " << details << std::endl;
        std::cerr << std::endl;
        std::cerr << "Suggestions:" << std::endl;
        std::cerr << "  - Try reducing the number of neurons or regions" << std::endl;
        std::cerr << "  - Check available system memory" << std::endl;
        std::cerr << "  - Try running with smaller simulation parameters" << std::endl;
        
        if (showHelp) {
            std::cerr << std::endl;
            std::cerr << "Use '--help' to see configuration options.";
        }
    }
    
    // Print simulation error with context
    void printSimulationError(const std::string& phase, const std::string& error,
                             const std::string& context = "",
                             bool showHelp = true) {
        std::cerr << "Simulation error in " << phase << ":" << std::endl;
        std::cerr << "  " << error << std::endl;
        
        if (!context.empty()) {
            std::cerr << "  Context: " << context << std::endl;
        }
        
        std::cerr << std::endl;
        std::cerr << "This might be due to:" << std::endl;
        std::cerr << "  - Invalid configuration parameters" << std::endl;
        std::cerr << "  - Memory limitations" << std::endl;
        std::cerr << "  - Numerical instability" << std::endl;
        
        if (showHelp) {
            std::cerr << std::endl;
            std::cerr << "Use '--help' for configuration tips.";
        }
    }
    
} // namespace UserFriendlyErrors