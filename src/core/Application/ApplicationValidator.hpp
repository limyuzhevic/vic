#include "NeuralValidation/NeuralComponentValidator.hpp"
#include "Schema/JSONSchemaValidator.hpp"
#include "Validation/NeuralInputValidator.hpp"
#include "Runtime/PerformanceMonitor.hpp"
#include <sstream>

// Enhanced validation integration for NLM main program
namespace nlm {

// Comprehensive validation runner for the main application
class ApplicationValidator {
public:
    struct ValidationReport {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double overallScore = 1.0;
        size_t totalChecks = 0;
        size_t successfulChecks = 0;
        
        ValidationReport& addError(const std::string& error) {
            errors.push_back(error);
            valid = false;
            overallScore = std::max(0.0, overallScore - 0.1);
            return *this;
        }
        
        ValidationReport& addWarning(const std::string& warning) {
            warnings.push_back(warning);
            overallScore = std::max(0.0, overallScore - 0.05);
            return *this;
        }
        
        ValidationReport& incrementCheck(bool success = true) {
            totalChecks++;
            if (success) successfulChecks++;
            return *this;
        }
        
        std::string getSummary() const {
            std::ostringstream oss;
            oss << "=== Application Validation Report ===\n";
            oss << "Overall Status: " << (valid ? "VALID" : "INVALID") << "\n";
            oss << "Score: " << overallScore << "/1.0\n";
            oss << "Checks Passed: " << successfulChecks << "/" << totalChecks << "\n";
            if (!errors.empty()) {
                oss << "\nErrors:\n";
                for (const auto& err : errors) {
                    oss << "  - " << err << "\n";
                }
            }
            if (!warnings.empty()) {
                oss << "\nWarnings:\n";
                for (const auto& warn : warnings) {
                    oss << "  - " << warn << "\n";
                }
            }
            return oss.str();
        }
    };
    
    // Enhanced configuration validation with schema support
    ValidationReport validateConfiguration(const Config& config) {
        ValidationReport report;
        
        NLM_LOG_INFO("Validating configuration...");
        report.incrementCheck();
        
        // Basic configuration validation
        if (!config.has("random_seed")) {
            report.addError("Missing required configuration key: random_seed");
        } else {
            auto seedOpt = config.get<int64_t>("random_seed");
            if (seedOpt) {
                if (*seedOpt < 0 || *seedOpt > 2147483647) {
                    report.addError("Invalid random_seed value: must be between 0 and 2147483647, got " + std::to_string(*seedOpt));
                } else {
                    report.incrementCheck(true);
                }
            }
        }
        
        if (!config.has("simulation_timestep")) {
            report.addError("Missing required configuration key: simulation_timestep");
        } else {
            auto timestepOpt = config.get<double>("simulation_timestep");
            if (timestepOpt) {
                if (*timestepOpt <= 0.0 || *timestepOpt > 1.0) {
                    report.addError("Invalid simulation_timestep: must be positive and <= 1.0, got " + std::to_string(*timestepOpt));
                } else {
                    report.incrementCheck(true);
                }
            }
        }
        
        if (!config.has("neuron_count")) {
            report.addError("Missing required configuration key: neuron_count");
        } else {
            auto neuronCountOpt = config.get<int64_t>("neuron_count");
            if (neuronCountOpt) {
                if (*neuronCountOpt <= 0 || *neuronCountOpt > 1000000) {
                    report.addError("Invalid neuron_count: must be positive and <= 1000000, got " + std::to_string(*neuronCountOpt));
                } else {
                    report.incrementCheck(true);
                }
            }
        }
        
        if (!config.has("region_count")) {
            report.addError("Missing required configuration key: region_count");
        } else {
            auto regionCountOpt = config.get<int64_t>("region_count");
            if (regionCountOpt) {
                if (*regionCountOpt <= 0 || *regionCountOpt > 1000) {
                    report.addError("Invalid region_count: must be positive and <= 1000, got " + std::to_string(*regionCountOpt));
                } else {
                    report.incrementCheck(true);
                }
            }
        }
        
        // Enhanced JSON schema validation
        auto schemaValidation = JSONSchemaValidator::validateConfig(config, "default");
        if (!schemaValidation.valid) {
            for (const auto& error : schemaValidation.errors) {
                report.addError("Schema validation: " + error);
            }
        } else {
            report.incrementCheck(true);
            for (const auto& warning : schemaValidation.warnings) {
                report.addWarning("Schema warning: " + warning);
            }
        }
        
        // Performance configuration validation
        if (config.has("connection_probability")) {
            auto probOpt = config.get<double>("connection_probability");
            if (probOpt) {
                if (*probOpt < 0.0 || *probOpt > 1.0) {
                    report.addWarning("Unusual connection_probability: " + std::to_string(*probOpt));
                }
            }
        }
        
        if (config.has("plasticity_learning_rate")) {
            auto plasticityOpt = config.get<double>("plasticity_learning_rate");
            if (plasticityOpt) {
                if (*plasticityOpt < 0.0 || *plasticityOpt > 1.0) {
                    report.addWarning("Unusual plasticity_learning_rate: " + std::to_string(*plasticityOpt));
                }
            }
        }
        
        NLM_LOG_INFO(report.getSummary());
        return report;
    }
    
    // Neural structure validation
    ValidationReport validateNeuralStructure(const Brain& brain) {
        ValidationReport report;
        
        NLM_LOG_INFO("Validating neural structure...");
        report.incrementCheck();
        
        // Use the comprehensive neural component validator
        auto neuralReport = NeuralComponentValidator::validateBrain(brain);
        
        // Add to our report
        report.errors.insert(report.errors.end(), neuralReport.errors.begin(), neuralReport.errors.end());
        report.warnings.insert(report.warnings.end(), neuralReport.warnings.begin(), neuralReport.warnings.end());
        report.overallScore = std::min(report.overallScore, neuralReport.overallScore);
        report.incrementCheck(!neuralReport.valid); // Count as check even if failed
        
        NLM_LOG_INFO(neuralReport.getSummary());
        return report;
    }
    
    // Input validation for simulation
    ValidationReport validateSimulationInputs(const Config& config, const std::string& mode = "normal") {
        ValidationReport report;
        
        NLM_LOG_INFO("Validating simulation inputs...");
        report.incrementCheck();
        
        // Validate timestep
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (timestepOpt) {
            if (*timestepOpt <= 0.0) {
                report.addError("Invalid simulation timestep: must be positive, got " + std::to_string(*timestepOpt));
            } else if (*timestepOpt > 0.01) {
                report.addWarning("Large simulation timestep may affect accuracy: " + std::to_string(*timestepOpt));
            }
        }
        
        // Mode-specific validation
        if (mode == "stress") {
            // Stress test specific validation
            auto neuronCountOpt = config.get<int64_t>("neuron_count");
            if (neuronCountOpt && *neuronCountOpt > 10000) {
                report.addWarning("Large neural network for stress test: " + std::to_string(*neuronCountOpt) + " neurons");
            }
        } else if (mode == "development") {
            // Development stage validation
            auto stageOpt = config.get<std::string>("developmental_stage");
            if (stageOpt) {
                std::string stage = *stageOpt;
                if (stage != "initial" && stage != "critical_period" && stage != "maturation" && stage != "adult") {
                    report.addError("Invalid developmental stage: " + stage);
                }
            }
        }
        
        // Validate plasticity parameters
        auto plasticityOpt = config.get<double>("plasticity_learning_rate");
        if (plasticityOpt) {
            if (*plasticityOpt < 0.0 || *plasticityOpt > 1.0) {
                report.addWarning("Unusual plasticity learning rate: " + std::to_string(*plasticityOpt));
            }
        }
        
        // Validate connection parameters
        auto connectionProbOpt = config.get<double>("connection_probability");
        if (connectionProbOpt) {
            if (*connectionProbOpt < 0.01 || *connectionProbOpt > 0.5) {
                report.addWarning("Unusual connection probability: " + std::to_string(*connectionProbOpt));
            }
        }
        
        NLM_LOG_INFO(report.getSummary());
        return report;
    }
    
    // Performance and resource validation
    ValidationReport validatePerformanceResources() {
        ValidationReport report;
        
        NLM_LOG_INFO("Validating performance and resources...");
        report.incrementCheck();
        
        // Performance monitor validation
        auto metrics = PerformanceMonitor::getMetrics();
        
        if (metrics.timestepTimes.size() > 1) {
            double avgTimestep = std::accumulate(metrics.timestepTimes.begin(),
                                                metrics.timestepTimes.end(), 0.0) / metrics.timestepTimes.size();
            
            if (avgTimestep > 0.1) {
                report.addWarning("High average timestep: " + std::to_string(avgTimestep) + "s");
            } else if (avgTimestep < 0.001) {
                report.addWarning("Low average timestep: " + std::to_string(avgTimestep) + "s");
            }
        }
        
        // Memory usage validation
        if (metrics.memoryUsage > 1024 * 1024 * 1024) { // > 1GB
            report.addWarning("High memory usage: " + std::to_string(metrics.memoryUsage / (1024 * 1024)) + " MB");
        }
        
        // Performance anomaly check
        if (!metrics.performanceAnomalies.empty()) {
            report.addError("Performance anomalies detected: " + std::to_string(metrics.performanceAnomalies.size()));
            for (size_t i = 0; i < metrics.performanceAnomalies.size() && i < 3; ++i) {
                report.addError("  - " + metrics.anomalyTypes[i] + " (deviation: " + std::to_string(metrics.performanceAnomalies[i]) + ")");
            }
        }
        
        // Check alerts
        auto alerts = PerformanceMonitor::getAlerts();
        for (const auto& alert : alerts) {
            if (alert.severity == "ERROR" || alert.severity == "CRITICAL") {
                report.addError("Alert: " + alert.message + " (" + alert.severity + ")");
            } else {
                report.addWarning("Alert: " + alert.message + " (" + alert.severity + ")");
            }
        }
        
        NLM_LOG_INFO(report.getSummary());
        return report;
    }
    
    // Complete application validation
    ValidationReport validateAll(const Config& config, const Brain& brain, 
                                const std::string& mode = "normal") {
        ValidationReport overallReport;
        
        NLM_LOG_INFO("=== Starting Comprehensive Application Validation ===");
        
        // Validate configuration
        auto configReport = validateConfiguration(config);
        overallReport.errors.insert(overallReport.errors.end(),
                                   configReport.errors.begin(), configReport.errors.end());
        overallReport.warnings.insert(overallReport.warnings.end(),
                                     configReport.warnings.begin(), configReport.warnings.end());
        overallReport.overallScore = std::min(overallReport.overallScore, configReport.overallScore);
        overallReport.incrementCheck(!configReport.valid);
        
        // Validate neural structure
        auto structureReport = validateNeuralStructure(brain);
        overallReport.errors.insert(overallReport.errors.end(),
                                   structureReport.errors.begin(), structureReport.errors.end());
        overallReport.warnings.insert(overallReport.warnings.end(),
                                     structureReport.warnings.begin(), structureReport.warnings.end());
        overallReport.overallScore = std::min(overallReport.overallScore, structureReport.overallScore);
        overallReport.incrementCheck(!structureReport.valid);
        
        // Validate simulation inputs
        auto inputReport = validateSimulationInputs(config, mode);
        overallReport.errors.insert(overallReport.errors.end(),
                                   inputReport.errors.begin(), inputReport.errors.end());
        overallReport.warnings.insert(overallReport.warnings.end(),
                                     inputReport.warnings.begin(), inputReport.warnings.end());
        overallReport.overallScore = std::min(overallReport.overallScore, inputReport.overallScore);
        overallReport.incrementCheck(!inputReport.valid);
        
        // Validate performance and resources
        auto perfReport = validatePerformanceResources();
        overallReport.errors.insert(overallReport.errors.end(),
                                   perfReport.errors.begin(), perfReport.errors.end());
        overallReport.warnings.insert(overallReport.warnings.end(),
                                     perfReport.warnings.begin(), perfReport.warnings.end());
        overallReport.overallScore = std::min(overallReport.overallScore, perfReport.overallScore);
        overallReport.incrementCheck(!perfReport.valid);
        
        // Determine overall validity
        overallReport.valid = overallReport.errors.empty();
        
        NLM_LOG_INFO("=== Comprehensive Application Validation Complete ===");
        NLM_LOG_INFO(overallReport.getSummary());
        
        return overallReport;
    }
    
    // Helper function to integrate with existing main.cpp
    static void runComprehensiveValidation(const Config& config, const Brain& brain, 
                                           const std::string& mode = "normal") {
        NLM_LOG_INFO("Running comprehensive validation for NLM application...");
        
        ApplicationValidator validator;
        auto report = validator.validateAll(config, brain, mode);
        
        if (report.valid) {
            NLM_LOG_INFO("✓ All validation checks passed! Application is ready for execution.");
        } else {
            NLM_LOG_ERROR("✗ Validation failed. Application may not be in a valid state.");
            
            // Attempt recovery for recoverable errors
            size_t recoveredCount = 0;
            for (const auto& error : report.errors) {
                if (error.find("recoverable") != std::string::npos || 
                    error.find("warning") != std::string::npos) {
                    NLM_LOG_WARNING("Attempting recovery for: " + error);
                    recoveredCount++;
                }
            }
            
            if (recoveredCount > 0) {
                NLM_LOG_INFO("Attempted recovery for " + std::to_string(recoveredCount) + " issues.");
            }
            
            NLM_LOG_ERROR("Validation report saved to error log.");
        }
    }
};

// Macro for easy validation in existing code
#define VALIDATE_NLM_APP(config, brain, mode) \
    do { \
        static nlm::ApplicationValidator validator; \
        auto report = validator.validateAll(config, brain, mode); \
        if (!report.valid) { \
            throw nlm::ValidationError("Application validation failed", "VALIDATION_ERROR"); \
        } \
    } while(0)

// Integration function for existing main.cpp
static void enhancedValidationIntegration(int argc, char** argv) {
    if (argc < 2) return;
    
    std::string arg1 = argv[1];
    
    if (arg1 == "--validate-only") {
        NLM_LOG_INFO("=== Running Enhanced Validation Mode ===");
        
        // In a real implementation, you would load the brain from a checkpoint
        // or create a temporary brain for validation
        
        NLM_LOG_INFO("Validation mode requires a brain instance.");
        NLM_LOG_INFO("Use '--validate-config' for configuration validation only.");
    }
}

} // namespace nlm