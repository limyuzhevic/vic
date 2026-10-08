/**
 * NLM Advanced Command Line Interface
 *
 * Enhanced command-line interface for NLM with support for:
 * - Multiple experiment modes
 * - Configuration file management
 * - Performance monitoring
 * - Output format options (text, JSON, CSV)
 * - Extended analysis modes
 * - Batch processing
 */

#ifndef NLM_ADVANCED_CLI_HPP
#define NLM_ADVANCED_CLI_HPP

#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace nlm {

/**
 * Advanced command line interface for NLM experiments
 */
class NLMCAdvancedCLI {
public:
    enum class OutputFormat { TEXT, JSON, CSV, TSV };
    enum class ExperimentMode { VERIFICATION_ONLY, QUICK_TEST, FULL_EXPERIMENT, PERFORMANCE_BENCHMARK };
    enum class AnalysisMode { BASIC, DETAILED, COMPREHENSIVE };
    
    struct AdvancedConfig {
        // Basic configuration
        std::string configFile = "configs/default.cfg";
        OutputFormat outputFormat = OutputFormat::TEXT;
        ExperimentMode experimentMode = ExperimentMode::FULL_EXPERIMENT;
        AnalysisMode analysisMode = AnalysisMode::DETAILED;
        
        // Performance settings
        bool enableProfiling = false;
        bool enableTiming = true;
        bool enableMemoryMonitoring = false;
        
        // Output settings
        std::string outputFile;
        bool verbose = false;
        bool quiet = false;
        
        // Experiment settings
        uint64_t neuronCount = 500;
        uint64_t maxSteps = 2000;
        uint64_t numTrials = 1;
        float timestep = 0.001f;
        
        // Integration test settings
        bool runIntegrationTests = true;
        bool runSystemTests = true;
        bool runCheckpointTests = true;
        bool runReplayTests = true;
        
        // Development settings
        bool enableDevelopment = true;
        bool enableStructuralPlasticity = true;
        bool enableCuriosity = true;
        bool enableNoveltyDetection = true;
        
        // Neuromodulation settings
        float dopamineBaseline = 0.1f;
        float curiosityLevel = 1.0f;
        float noveltyThreshold = 0.5f;
        
        // Memory settings
        uint64_t workingMemoryCapacity = 100;
        uint64_t episodicMemoryCapacity = 1000;
        
        // Prediction settings
        bool enablePrediction = true;
        float predictionConfidenceThreshold = 0.7f;
        
        // Development settings
        bool enableDevelopmentStages = true;
        float plasticityRateMultiplier = 1.0f;
        
        // Batch processing
        bool batchMode = false;
        std::vector<std::string> configFiles;
        std::string batchOutputDir;
    };

    NLMCAdvancedCLI() : startTime_(std::chrono::high_resolution_clock::now()) {
        setupDefaultConfiguration();
    }

    ~NLMCAdvancedCLI() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        if (config_.verbose) {
            std::cout << "CLI session completed in " << duration.count() << "ms" << std::endl;
        }
    }

    /**
     * Parse command line arguments
     */
    bool parseArguments(int argc, char* argv[]);

    /**
     * Run the experiment with the parsed configuration
     */
    int runExperiment();

    /**
     * Generate comprehensive report
     */
    std::string generateReport(const Phase6IntegrationResult& result) const;

    /**
     * Generate JSON report
     */
    std::string generateJSONReport(const Phase6IntegrationResult& result) const;

    /**
     * Generate CSV report
     */
    std::string generateCSVReport(const Phase6IntegrationResult& result) const;

    /**
     * Generate TSV report
     */
    std::string generateTSVReport(const Phase6IntegrationResult& result) const;

    /**
     * Write report to file
     */
    bool writeReport(const std::string& content) const;

    /**
     * Print help information
     */
    void printHelp() const;

    /**
     * Print version information
     */
    void printVersion() const;

    /**
     * Load configuration from file
     */
    bool loadConfiguration(const std::string& filepath);

    /**
     * Save configuration to file
     */
    bool saveConfiguration(const std::string& filepath) const;

    /**
     * Validate configuration
     */
    bool validateConfiguration() const;

    /**
     * Apply configuration to experiment
     */
    void applyConfiguration(Phase6IntegratedExperiment& experiment, Phase6Config& phase6Config) const;

    /**
     * Run batch processing
     */
    bool runBatchProcessing();

    /**
     * Run performance benchmark
     */
    bool runPerformanceBenchmark();

    /**
     * Generate configuration template
     */
    void generateConfigurationTemplate(const std::string& filepath) const;

    /**
     * Get configuration
     */
    const AdvancedConfig& getConfig() const { return config_; }

    /**
     * Set configuration
     */
    void setConfig(const AdvancedConfig& config) { config_ = config; }

private:
    AdvancedConfig config_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Command line argument definitions
    struct CommandLineOption {
        std::string flag;
        std::string longFlag;
        std::string description;
        std::string valueType;
        std::function<void(const std::string&)> action;
    };

    std::vector<CommandLineOption> commandLineOptions_;

    void setupDefaultConfiguration();
    void setupCommandLineOptions();
    void setupLogging();
    
    std::string formatOutput(const std::string& content) const;
    void printOutput(const std::string& content) const;
    
    std::string formatNumber(float value) const;
    std::string formatPercentage(float value) const;
    
    bool parseBoolean(const std::string& str) const;
    float parseFloat(const std::string& str) const;
    uint64_t parseUint64(const std::string& str) const;
    
    std::string currentTimestamp() const;
    
    void printSystemInfo() const;
    void printExperimentInfo() const;
    
    std::unordered_map<std::string, std::string> getSystemMetrics() const;
    
    // Extended experiment modes
    bool runExtendedVerification(Phase6IntegratedExperiment& experiment);
    bool runExtendedSystemTests(Phase6IntegratedExperiment& experiment);
    bool runExtendedPerformanceTests(Phase6IntegratedExperiment& experiment);
    
    // Analysis modes
    std::string generateDetailedAnalysis(const Phase6IntegrationResult& result) const;
    std::string generateComprehensiveAnalysis(const Phase6IntegrationResult& result) const;
}; 

// Global CLI instance for easy access
inline NLMCAdvancedCLI& getAdvancedCLI() {
    static NLMCAdvancedCLI cli;
    return cli;
}

// Convenience functions for backward compatibility
inline int runPhase6Demo(int argc, char* argv[]) {
    return getAdvancedCLI().runExperiment();
}

inline bool parsePhase6Args(int argc, char* argv[]) {
    return getAdvancedCLI().parseArguments(argc, argv);
}

} // namespace nlm

#endif // NLM_ADVANCED_CLI_HPP
