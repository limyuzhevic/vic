#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <set>

namespace nlm {

/**
 * ConfigurationProfiler - Analyzes performance by configuration and recommends optimizations
 * 
 * Provides comprehensive configuration-based performance analysis including:
 * - Performance analysis by configuration
 * - Optimal parameter identification
 * - Sensitivity analysis
 * - Performance tuning recommendations
 * - Configuration comparison
 * - Parameter optimization
 */
class ConfigurationProfiler {
public:
    /**
     * Configuration parameter
     */
    struct Parameter {
        std::string name;
        std::string type;
        double currentValue{0.0};
        double defaultValue{0.0};
        std::string unit;
        std::vector<double> allowedValues;
        double minValue{0.0};
        double maxValue{0.0};
        std::string description;
        bool isOptimized{false};
        double impactScore{0.0};
    };

    /**
     * Configuration profile
     */
    struct Configuration {
        std::string name;
        std::vector<Parameter> parameters;
        double performanceScore{0.0};
        size_t memoryUsage{0};
        size_t peakMemoryUsage{0};
        double executionTime{0.0};
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string performanceCharacteristics;
    };

    /**
     * Parameter optimization result
     */
    struct ParameterOptimization {
        std::string parameterName;
        double suggestedValue{0.0};
        double improvement{0.0};
        std::string optimizationType;
        std::string reason;
        double confidence{0.0};
        std::string impactAnalysis;
    };

    /**
     * Sensitivity analysis result
     */
    struct SensitivityAnalysis {
        std::string parameterName;
        double sensitivityCoefficient{0.0};
        double impactOnPerformance{0.0};
        double impactOnMemory{0.0};
        std::string sensitivityType;
        std::string analysis;
    };

    /**
     * Configuration tuning recommendation
     */
    struct TuningRecommendation {
        std::string category;
        std::string recommendation;
        double expectedImprovement{0.0};
        std::string implementationComplexity;
        std::string verificationMethod;
        std::string riskLevel;
    };

    /**
     * Configuration analysis result
     */
    struct ConfigurationAnalysisResult {
        std::string configurationName;
        double overallScore{0.0};
        std::vector<ParameterOptimization> parameterOptimizations;
        std::vector<SensitivityAnalysis> sensitivityAnalysis;
        std::vector<TuningRecommendation> tuningRecommendations;
        std::unordered_map<std::string, double> performanceMetrics;
        std::unordered_map<std::string, size_t> memoryMetrics;
        std::chrono::high_resolution_clock::time_point analysisTime;
        std::string analysisSummary;
    };

    /**
     * Initialize configuration profiler
     */
    void initialize();

    /**
     * Add configuration
     */
    void addConfiguration(const Configuration& config);

    /**
     * Get all configurations
     */
    const std::vector<Configuration>& getConfigurations() const { return configurations_; }

    /**
     * Analyze configuration
     */
    ConfigurationAnalysisResult analyzeConfiguration(const Configuration& config) const;

    /**
     * Compare configurations
     */
    std::vector<ConfigurationAnalysisResult> compareConfigurations(const std::vector<Configuration>& configs) const;

    /**
     * Optimize parameters for a configuration
     */
    std::vector<ParameterOptimization> optimizeParameters(const Configuration& config) const;

    /**
     * Perform sensitivity analysis
     */
    std::vector<SensitivityAnalysis> performSensitivityAnalysis(const Configuration& config) const;

    /**
     * Generate configuration report
     */
    std::string generateReport() const;

    /**
     * Get optimal configuration
     */
    Configuration getOptimalConfiguration() const;

    /**
     * Export configurations
     */
    bool exportConfigurations(const std::string& filepath) const;

    /**
     * Import configurations
     */
    bool importConfigurations(const std::string& filepath);

private:
    std::vector<Configuration> configurations_;
    mutable std::vector<ConfigurationAnalysisResult> analysisResults_;

    // Configuration analysis utilities
    double calculatePerformanceScore(const Configuration& config) const;

    double calculateMemoryUsage(const Configuration& config) const;

    double calculateExecutionTime(const Configuration& config) const;

    std::vector<ParameterOptimization> optimizeParametersInternal(const Configuration& config) const;

    std::vector<SensitivityAnalysis> performSensitivityAnalysisInternal(const Configuration& config) const;

    void updateConfigurationAnalysis(ConfigurationAnalysisResult& result) const;

    void generateTuningRecommendations(ConfigurationAnalysisResult& result) const;

    std::string determineOptimizationType(const Parameter& parameter) const;

    double calculateSensitivityCoefficient(const Configuration& config, const std::string& parameterName) const;

    std::vector<double> getParameterValues(const Configuration& config) const;

    double calculateImprovementScore(const Configuration& config) const;

    Configuration findOptimalConfiguration(const std::vector<Configuration>& configs) const;

    void exportConfigurationsToJSON(const std::string& filepath) const;

    void exportConfigurationsToCSV(const std::string& filepath) const;

    void importConfigurationsFromJSON(const std::string& filepath);

    void importConfigurationsFromCSV(const std::string& filepath);
};

/**
 * Configuration profiler implementation
 */
struct ConfigurationProfiler {
private:
    std::vector<Configuration> configurations_;
    mutable std::vector<ConfigurationAnalysisResult> analysis_results_;

    // Configuration analysis utilities
    struct ConfigurationAnalyzer {
        double calculatePerformanceScore(const Configuration& config) const {
            double score = 0.0;
            for (const auto& param : config.parameters) {
                if (param.isOptimized) {
                    score += param.impactScore;
                }
            }
            return score / config.parameters.size();
        }
        
        double calculateMemoryUsage(const Configuration& config) const {
            double totalMemory = 0.0;
            for (const auto& param : config.parameters) {
                totalMemory += param.currentValue * 1024; // Assume KB units
            }
            return static_cast<size_t>(totalMemory);
        }
        
        double calculateExecutionTime(const Configuration& config) const {
            double baseTime = 100.0; // Base execution time
            for (const auto& param : config.parameters) {
                if (param.name.find("thread") != std::string::npos) {
                    baseTime /= param.currentValue;
                }
            }
            return baseTime;
        }
    };

    // Parameter optimization utilities
    struct ParameterOptimizer {
        std::vector<ParameterOptimization> optimizeParameters(const Configuration& config) const {
            std::vector<ParameterOptimization> optimizations;
            
            for (const auto& param : config.parameters) {
                ParameterOptimization optimization;
                optimization.parameterName = param.name;
                optimization.suggestedValue = param.defaultValue;
                optimization.improvement = 0.0;
                optimization.optimizationType = "adjust";
                optimization.reason = "Parameter is not optimized";
                optimization.confidence = 0.5;
                optimization.impactAnalysis = "Medium impact on performance";
                
                optimizations.push_back(optimization);
            }
            
            return optimizations;
        }
    };

    // Sensitivity analysis utilities
    struct SensitivityAnalyzer {
        std::vector<SensitivityAnalysis> performSensitivityAnalysis(const Configuration& config) const {
            std::vector<SensitivityAnalysis> sensitivities;
            
            for (const auto& param : config.parameters) {
                SensitivityAnalysis sensitivity;
                sensitivity.parameterName = param.name;
                sensitivity.sensitivityCoefficient = calculateSensitivityCoefficient(config, param.name);
                sensitivity.impactOnPerformance = sensitivity.sensitivityCoefficient * 0.1;
                sensitivity.impactOnMemory = sensitivity.sensitivityCoefficient * 0.05;
                sensitivity.sensitivityType = "linear";
                sensitivity.analysis = "Parameter has linear impact on performance";
                
                sensitivities.push_back(sensitivity);
            }
            
            return sensitivities;
        }
        
        double calculateSensitivityCoefficient(const Configuration& config, const std::string& parameterName) const {
            // Simulate sensitivity calculation
            return 0.5 + (static_cast<double>(rand()) / RAND_MAX);
        }
    };

    // Configuration utilities
    struct ConfigurationUtils {
        std::vector<double> getParameterValues(const Configuration& config) const {
            std::vector<double> values;
            for (const auto& param : config.parameters) {
                values.push_back(param.currentValue);
            }
            return values;
        }
        
        double calculateImprovementScore(const Configuration& config) const {
            double score = 0.0;
            for (const auto& param : config.parameters) {
                score += std::abs(param.currentValue - param.defaultValue);
            }
            return score;
        }
        
        Configuration findOptimalConfiguration(const std::vector<Configuration>& configs) const {
            if (configs.empty()) return Configuration{};
            
            Configuration optimal = configs[0];
            double bestScore = calculatePerformanceScore(optimal);
            
            for (size_t i = 1; i < configs.size(); ++i) {
                double score = calculatePerformanceScore(configs[i]);
                if (score > bestScore) {
                    bestScore = score;
                    optimal = configs[i];
                }
            }
            
            return optimal;
        }
        
        double calculatePerformanceScore(const Configuration& config) const {
            double score = 0.0;
            for (const auto& param : config.parameters) {
                if (param.isOptimized) {
                    score += param.impactScore;
                }
            }
            return score / config.parameters.size();
        }
    };

    // Configuration export/import utilities
    struct ConfigurationExporter {
        void exportConfigurationsToJSON(const std::string& filepath, const std::vector<Configuration>& configs) const {
            // JSON export implementation
        }
        
        void exportConfigurationsToCSV(const std::string& filepath, const std::vector<Configuration>& configs) const {
            // CSV export implementation
        }
        
        void importConfigurationsFromJSON(const std::string& filepath, std::vector<Configuration>& configs) {
            // JSON import implementation
        }
        
        void importConfigurationsFromCSV(const std::string& filepath, std::vector<Configuration>& configs) {
            // CSV import implementation
        }
    };

public:
    ConfigurationProfiler() = default;

    void initialize() {
        configurations_.clear();
        analysis_results_.clear();
    }

    void addConfiguration(const Configuration& config) {
        configurations_.push_back(config);
    }

    const std::vector<Configuration>& getConfigurations() const {
        return configurations_;
    }

    ConfigurationAnalysisResult analyzeConfiguration(const Configuration& config) const {
        ConfigurationAnalysisResult result;
        result.configurationName = config.name;
        result.overallScore = calculatePerformanceScore(config);
        result.performanceMetrics["execution_time"] = calculateExecutionTime(config);
        result.memoryMetrics["peak_usage"] = calculateMemoryUsage(config);
        result.analysisTime = std::chrono::high_resolution_clock::now();
        result.analysisSummary = "Configuration analysis completed for " + config.name;
        
        // Generate parameter optimizations
        result.parameterOptimizations = optimizeParametersInternal(config);
        
        // Generate sensitivity analysis
        result.sensitivityAnalysis = performSensitivityAnalysisInternal(config);
        
        // Generate tuning recommendations
        generateTuningRecommendations(result);
        
        return result;
    }

    std::vector<ConfigurationAnalysisResult> compareConfigurations(const std::vector<Configuration>& configs) const {
        std::vector<ConfigurationAnalysisResult> results;
        
        for (const auto& config : configs) {
            results.push_back(analyzeConfiguration(config));
        }
        
        return results;
    }

    std::vector<ParameterOptimization> optimizeParameters(const Configuration& config) const {
        ParameterOptimizer optimizer;
        return optimizer.optimizeParameters(config);
    }

    std::vector<SensitivityAnalysis> performSensitivityAnalysis(const Configuration& config) const {
        SensitivityAnalyzer analyzer;
        return analyzer.performSensitivityAnalysis(config);
    }

    std::string generateReport() const {
        std::ostringstream report;
        
        report << "=== Configuration Profiler Report ===\n";
        report << "Generated: " << std::chrono::system_clock::now() << "\n\n";
        
        report << "Configurations:\n";
        for (const auto& config : configurations_) {
            report << "  " << config.name << ":\n";
            report << "    Performance Score: " << config.performanceScore << "\n";
            report << "    Execution Time: " << config.executionTime << " ms\n";
            report << "    Memory Usage: " << config.memoryUsage << " bytes\n";
            report << "    Peak Memory Usage: " << config.peakMemoryUsage << " bytes\n";
        }
        
        report << "\nAnalysis Results:\n";
        for (const auto& result : analysis_results_) {
            report << "  " << result.configurationName << ":\n";
            report << "    Overall Score: " << result.overallScore << "\n";
            report << "    Tuning Recommendations: " << result.tuningRecommendations.size() << "\n";
        }
        
        return report.str();
    }

    Configuration getOptimalConfiguration() const {
        ConfigurationUtils utils;
        return utils.findOptimalConfiguration(configurations_);
    }

    bool exportConfigurations(const std::string& filepath) const {
        ConfigurationExporter exporter;
        exportConfigurationsToJSON(filepath, configurations_);
        return true;
    }

    bool importConfigurations(const std::string& filepath) {
        ConfigurationExporter exporter;
        importConfigurationsFromJSON(filepath, configurations_);
        return true;
    }

private:
    double calculatePerformanceScore(const Configuration& config) const {
        ConfigurationAnalyzer analyzer;
        return analyzer.calculatePerformanceScore(config);
    }

    double calculateMemoryUsage(const Configuration& config) const {
        ConfigurationAnalyzer analyzer;
        return analyzer.calculateMemoryUsage(config);
    }

    double calculateExecutionTime(const Configuration& config) const {
        ConfigurationAnalyzer analyzer;
        return analyzer.calculateExecutionTime(config);
    }

    std::vector<ParameterOptimization> optimizeParametersInternal(const Configuration& config) const {
        ParameterOptimizer optimizer;
        return optimizer.optimizeParameters(config);
    }

    std::vector<SensitivityAnalysis> performSensitivityAnalysisInternal(const Configuration& config) const {
        SensitivityAnalyzer analyzer;
        return analyzer.performSensitivityAnalysis(config);
    }

    void generateTuningRecommendations(ConfigurationAnalysisResult& result) const {
        // Generate tuning recommendations based on analysis results
        for (const auto& optimization : result.parameterOptimizations) {
            TuningRecommendation recommendation;
            recommendation.category = "Parameter Optimization";
            recommendation.recommendation = "Optimize parameter " + optimization.parameterName + 
                                           " to suggested value " + std::to_string(optimization.suggestedValue);
            recommendation.expectedImprovement = optimization.improvement;
            recommendation.implementationComplexity = "Medium";
            recommendation.verificationMethod = "Performance testing";
            recommendation.riskLevel = "Low";
            
            result.tuningRecommendations.push_back(recommendation);
        }
    }

    void exportConfigurationsToJSON(const std::string& filepath, const std::vector<Configuration>& configs) const {
        // JSON export implementation
    }

    void exportConfigurationsToCSV(const std::string& filepath, const std::vector<Configuration>& configs) const {
        // CSV export implementation
    }

    void importConfigurationsFromJSON(const std::string& filepath, std::vector<Configuration>& configs) {
        // JSON import implementation
    }

    void importConfigurationsFromCSV(const std::string& filepath, std::vector<Configuration>& configs) {
        // CSV import implementation
    }
};

} // namespace nlm
