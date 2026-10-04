#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include "performance/StatusManager.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {

class ValidateCommand : public SimpleCLICommand {
public:
    ValidateCommand() : SimpleCLICommand(
        "validate",
        "Validate configuration and show errors",
        "nlm validate [--strict] [--fix] [--verbose]",
        "Validate the current configuration for errors and warnings.\n\n"
        "Options:\n"
        "  --strict           Strict validation (treat warnings as errors)\n"
        "  --fix             Attempt automatic fixes for errors\n"
        "  --verbose          Detailed output with error codes\n"
        "  --export <file>    Export validation report to file\n\n"
        "Examples:\n"
        "  nlm validate\n"
        "  nlm validate --strict\n"
        "  nlm validate --fix\n"
        "  nlm validate --verbose --export report.txt"
    ) {
        options = {
            {"strict", "", "Strict validation (treat warnings as errors)", OptionType::Flag},
            {"fix", "", "Attempt automatic fixes for errors", OptionType::Flag},
            {"verbose", "", "Detailed output with error codes", OptionType::Flag},
            {"export", "", "Export validation report to file", OptionType::OptionalValue}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get options
        bool strict = hasOption(args, "strict");
        bool fix = hasOption(args, "fix");
        bool verbose = hasOption(args, "verbose");
        std::string exportFile = getOptionValue(args, "export");
        
        // Validate configuration
        auto config = getConfig();
        std::vector<ValidationError> errors;
        bool isValid = config->isValid(errors);
        
        // Get warnings
        std::vector<ValidationWarning> warnings;
        config->getWarnings(warnings);
        
        // Display validation results
        if (verbose) {
            displayValidationReport(errors, warnings, strict);
        } else {
            displaySimpleValidationReport(errors, warnings, strict);
        }
        
        // Attempt fixes if requested
        if (fix && !errors.empty()) {
            CLIFormatters::printInfo("Attempting to fix errors...");
            fixErrors(config, errors);
            
            // Validate again after fixes
            errors.clear();
            warnings.clear();
            isValid = config->isValid(errors);
            config->getWarnings(warnings);
            
            CLIFormatters::printInfo("Validation after fixes:");
            if (verbose) {
                displayValidationReport(errors, warnings, strict);
            } else {
                displaySimpleValidationReport(errors, warnings, strict);
            }
        }
        
        // Export report if requested
        if (!exportFile.empty()) {
            exportValidationReport(errors, warnings, exportFile);
        }
        
        if (isValid) {
            if (errors.empty()) {
                CLIFormatters::printSuccess("Configuration is valid! ✓");
                if (!warnings.empty()) {
                    CLIFormatters::printWarning("Warnings detected");
                }
                return 0;
            } else if (!strict) {
                CLIFormatters::printWarning("Configuration has errors that can be ignored");
                return 1;
            } else {
                CLIFormatters::printError("Configuration validation failed");
                return 1;
            }
        } else {
            CLIFormatters::printError("Configuration validation failed");
            return 1;
        }
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        // Basic validation of arguments
        for (const auto& arg : args) {
            if (arg.find("--export") != std::string::npos && arg != "--export") {
                // Check if there's a value after --export
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    void displayValidationReport(const std::vector<ValidationError>& errors,
                                 const std::vector<ValidationWarning>& warnings,
                                 bool strict) {
        std::cout << "Configuration Validation Report" << std::endl;
        std::cout << "=============================" << std::endl << std::endl;
        
        if (errors.empty() && warnings.empty()) {
            CLIFormatters::printSuccess("Configuration is valid! ✓");
            return;
        }
        
        if (!errors.empty()) {
            std::cout << "Errors (" << errors.size() << "):" << std::endl;
            for (size_t i = 0; i < errors.size(); ++i) {
                const auto& error = errors[i];
                std::cout << "  Error " << (i + 1) << ": " << error.message << std::endl;
                if (!error.key.empty()) {
                    std::cout << "    Key: " << error.key << std::endl;
                }
                if (!error.suggestion.empty()) {
                    std::cout << "    Suggestion: " << error.suggestion << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        if (!warnings.empty()) {
            std::cout << "Warnings (" << warnings.size() << "):" << std::endl;
            for (size_t i = 0; i < warnings.size(); ++i) {
                const auto& warning = warnings[i];
                std::cout << "  Warning " << (i + 1) << ": " << warning.message << std::endl;
                if (!warning.key.empty()) {
                    std::cout << "    Key: " << warning.key << std::endl;
                }
                if (!warning.suggestion.empty()) {
                    std::cout << "    Suggestion: " << warning.suggestion << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        // Check if validation fails
        bool hasErrors = !errors.empty();
        bool hasWarnings = !warnings.empty();
        
        if (hasErrors) {
            if (strict || hasWarnings) {
                std::cout << "VALIDATION FAILED" << std::endl;
            }
        }
        
        if (hasWarnings && strict) {
            std::cout << "VALIDATION FAILED (warnings treated as errors)" << std::endl;
        }
    }
    
    void displaySimpleValidationReport(const std::vector<ValidationError>& errors,
                                       const std::vector<ValidationWarning>& warnings,
                                       bool strict) {
        if (errors.empty() && warnings.empty()) {
            CLIFormatters::printSuccess("Configuration is valid! ✓");
            return;
        }
        
        if (!errors.empty()) {
            std::cout << "Errors: " << errors.size() << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << error.message << std::endl;
            }
        }
        
        if (!warnings.empty()) {
            std::cout << "Warnings: " << warnings.size() << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "  - " << warning.message << std::endl;
            }
        }
        
        if (errors.empty()) {
            CLIFormatters::printWarning("Warnings detected");
        } else {
            CLIFormatters::printError("Validation failed");
        }
    }
    
    void fixErrors(std::shared_ptr<Config> config, std::vector<ValidationError>& errors) {
        // Attempt to fix common errors automatically
        for (auto& error : errors) {
            if (fixKeyError(config, error)) {
                error.fixed = true;
            }
        }
    }
    
    bool fixKeyError(std::shared_ptr<Config> config, ValidationError& error) {
        // Implement automatic fixes for specific error types
        // This is a simplified implementation
        if (error.type == ValidationErrorType::RequiredKeyMissing) {
            // Try to set default value for missing key
            // This would require knowing the default value
            return false; // For now, don't auto-fix required keys
        }
        return false;
    }
    
    void exportValidationReport(const std::vector<ValidationError>& errors,
                                const std::vector<ValidationWarning>& warnings,
                                const std::string& filename) {
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            CLIFormatters::printError("Cannot write to file: " + filename);
            return;
        }
        
        file << "Validation Report\n";
        file << "================\n\n";
        file << "Generated: " << CLIUtils::getCurrentTimestamp() << "\n\n";
        
        file << "Errors: " << errors.size() << "\n";
        for (const auto& error : errors) {
            file << "  - " << error.message << "\n";
        }
        
        file << "\nWarnings: " << warnings.size() << "\n";
        for (const auto& warning : warnings) {
            file << "  - " << warning.message << "\n";
        }
        
        file.close();
        CLIFormatters::printSuccess("Validation report exported to: " + filename);
    }
    
    // Helper methods
    std::shared_ptr<Config> getConfig() {
        // Get the global configuration
        // This should be implemented based on how configuration is managed
        static std::shared_ptr<Config> config = std::make_shared<Config>();
        return config;
    }
};

} // namespace nlm
