// NLM Configuration Validation Tool
// Validates configuration files for syntax errors and consistency
// Provides detailed error reporting and suggestions

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>

using namespace nlm;

class ConfigValidator {
public:
    ConfigValidator() {}
    ~ConfigValidator() {}

    // Validate configuration file and return validation results
    std::vector<std::string> validate(const std::string& filepath) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        auto config = std::make_shared<Config>();
        
        try {
            // Load configuration file
            if (!config->loadFromFile(filepath)) {
                errors.push_back("Failed to load configuration file: " + filepath);
                return {errors[0]};  // Just return the loading error
            }
            
            // Run validation checks
            errors = checkSyntax(config);
            if (errors.empty()) {
                warnings = checkConsistency(config);
            }
            
        } catch (const std::exception& e) {
            errors.push_back("Configuration validation error: " + std::string(e.what()));
        }
        
        // Report results
        if (!errors.empty()) {
            std::cout << "❌ Configuration validation FAILED:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  ERROR: " << error << std::endl;
            }
        }
        
        if (!warnings.empty()) {
            std::cout << "⚠️  Configuration validation PASSED with warnings:" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
        
        if (errors.empty() && warnings.empty()) {
            std::cout << "✅ Configuration validation PASSED" << std::endl;
        }
        
        return errors;
    }

private:
    std::vector<std::string> checkSyntax(const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        // Check for required configuration keys
        std::vector<std::string> requiredKeys = {
            "random_seed",
            "simulation_timestep", 
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                errors.push_back("Missing required configuration key: '" + key + "'");
            }
        }
        
        // Validate value ranges and types
        validateValueRanges(config, errors);
        
        // Check for duplicate keys (should be prevented by config system, but good to verify)
        checkDuplicateKeys(config, errors);
        
        return errors;
    }
    
    std::vector<std::string> checkConsistency(const std::shared_ptr<Config>& config) {
        std::vector<std::string> warnings;
        
        // Check neuron_count is reasonable
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (neuronCountOpt.has_value()) {
            if (neuronCountOpt.value() <= 0) {
                warnings.push_back("neuron_count should be positive");
            } else if (neuronCountOpt.value() > 100000) {
                warnings.push_back("neuron_count is very large, may impact performance");
            }
        }
        
        // Check simulation_timestep is reasonable
        auto timestepOpt = config->get<double>("simulation_timestep");
        if (timestepOpt.has_value()) {
            if (timestepOpt.value() <= 0.0) {
                warnings.push_back("simulation_timestep should be positive");
            } else if (timestepOpt.value() > 1.0) {
                warnings.push_back("simulation_timestep is large (>1s), may affect accuracy");
            }
        }
        
        // Check connection_probability is in valid range
        auto connProbOpt = config->get<double>("connection_probability");
        if (connProbOpt.has_value()) {
            if (connProbOpt.value() < 0.0 || connProbOpt.value() > 1.0) {
                warnings.push_back("connection_probability should be between 0.0 and 1.0");
            }
        }
        
        // Check for other consistency issues
        checkDevelopmentParameters(config, warnings);
        checkNeuromodulationParameters(config, warnings);
        
        return warnings;
    }
    
    void validateValueRanges(const std::shared_ptr<Config>& config, std::vector<std::string>& errors) {
        auto validateRange = [&errors](const std::string& key, auto value, 
                                       auto min, auto max, const std::string& type) {
            if (value < min) {
                errors.push_back(key + " should be >= " + std::to_string(min) + " (" + type + ")");
            } else if (value > max) {
                errors.push_back(key + " should be <= " + std::to_string(max) + " (" + type + ")");
            }
        };
        
        // Validate random_seed
        auto seedOpt = config->get<int64_t>("random_seed");
        if (seedOpt.has_value()) {
            validateRange("random_seed", seedOpt.value(), 0, INT64_MAX, "integer");
        }
        
        // Validate simulation_timestep
        auto timestepOpt = config->get<double>("simulation_timestep");
        if (timestepOpt.has_value()) {
            validateRange("simulation_timestep", timestepOpt.value(), 0.0, 10.0, "seconds");
        }
        
        // Validate neuron_count
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (neuronCountOpt.has_value()) {
            validateRange("neuron_count", neuronCountOpt.value(), 1, 1000000, "neurons");
        }
        
        // Validate region_count
        auto regionCountOpt = config->get<int64_t>("region_count");
        if (regionCountOpt.has_value()) {
            validateRange("region_count", regionCountOpt.value(), 1, 100, "regions");
        }
        
        // Validate connection_probability
        auto connProbOpt = config->get<double>("connection_probability");
        if (connProbOpt.has_value()) {
            validateRange("connection_probability", connProbOpt.value(), 0.0, 1.0, "probability");
        }
        
        // Validate plasticity parameters
        auto plasticityRateOpt = config->get<double>("plasticity_learning_rate");
        if (plasticityRateOpt.has_value()) {
            validateRange("plasticity_learning_rate", plasticityRateOpt.value(), 0.0, 1.0, "rate");
        }
        
        // Validate STDP parameters
        auto stdpLtpOpt = config->get<double>("stdp_ltp_weight");
        if (stdpLtpOpt.has_value()) {
            validateRange("stdp_ltp_weight", stdpLtpOpt.value(), 0.0, 1.0, "weight");
        }
        
        auto stdpLtdOpt = config->get<double>("stdp_ltd_weight");
        if (stdpLtdOpt.has_value()) {
            validateRange("stdp_ltd_weight", stdpLtdOpt.value(), 0.0, 1.0, "weight");
        }
        
        auto stdpTauOpt = config->get<double>("stdp_tau");
        if (stdpTauOpt.has_value()) {
            validateRange("stdp_tau", stdpTauOpt.value(), 1.0, 1000.0, "time constant (ms)");
        }
    }
    
    void checkDuplicateKeys(const std::shared_ptr<Config>& config, std::vector<std::string>& errors) {
        auto keys = config->getKeys();
        std::unordered_map<std::string, int> keyCount;
        
        for (const auto& key : keys) {
            keyCount[key]++;
        }
        
        for (const auto& [key, count] : keyCount) {
            if (count > 1) {
                errors.push_back("Duplicate configuration key: '" + key + "' (appears " + 
                                std::to_string(count) + " times)");
            }
        }
    }
    
    void checkDevelopmentParameters(const std::shared_ptr<Config>& config, std::vector<std::string>& warnings) {
        auto synaptogenesisOpt = config->get<double>("development_synaptogenesis_rate");
        if (synaptogenesisOpt.has_value()) {
            if (synaptogenesisOpt.value() < 0.0 || synaptogenesisOpt.value() > 1.0) {
                warnings.push_back("development_synaptogenesis_rate should be between 0.0 and 1.0");
            }
        }
        
        auto pruningOpt = config->get<double>("development_pruning_rate");
        if (pruningOpt.has_value()) {
            if (pruningOpt.value() < 0.0 || pruningOpt.value() > 1.0) {
                warnings.push_back("development_pruning_rate should be between 0.0 and 1.0");
            }
        }
    }
    
    void checkNeuromodulationParameters(const std::shared_ptr<Config>& config, std::vector<std::string>& warnings) {
        // Check dopamine_baseline (we fixed the duplicate, but still validate)
        auto dopamineOpt = config->get<double>("dopamine_baseline");
        if (dopamineOpt.has_value()) {
            if (dopamineOpt.value() < -10.0 || dopamineOpt.value() > 10.0) {
                warnings.push_back("dopamine_baseline should be between -10.0 and 10.0");
            }
        }
        
        auto rewardDiscountOpt = config->get<double>("reward_discount_factor");
        if (rewardDiscountOpt.has_value()) {
            if (rewardDiscountOpt.value() < 0.0 || rewardDiscountOpt.value() > 1.0) {
                warnings.push_back("reward_discount_factor should be between 0.0 and 1.0");
            }
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Configuration Validator                              ║
    ║                                                               ║
    ║     Validates configuration files for syntax and consistency   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config_file>" << std::endl;
        std::cout << "Example: " << argv[0] << " configs/default.cfg" << std::endl;
        return 1;
    }
    
    std::string configFile = argv[1];
    
    std::cout << "Validating configuration file: " << configFile << std::endl;
    std::cout << std::endl;
    
    ConfigValidator validator;
    std::vector<std::string> errors = validator.validate(configFile);
    
    if (errors.empty()) {
        std::cout << std::endl;
        std::cout << "✅ Configuration validation completed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "❌ Configuration validation failed!" << std::endl;
        return 1;
    }
}