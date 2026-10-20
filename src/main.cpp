// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced Command Line Interface Implementation

// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// Enhanced with comprehensive CLI, configuration management, logging, and performance monitoring

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>
#include <regex>
#include <filesystem>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <cmath>

namespace fs = std::filesystem;

using namespace nlm;

// Global version and build information
const std::string NLM_VERSION = "2.0.0";
const std::string BUILD_DATE = __DATE__;
const std::string COMPILER = __VERSION__;

// Forward declarations for enhanced CLI components
class EnhancedCLI;
class ConfigurationManager;
class AdvancedLogger;
class PerformanceProfiler;
class OutputFormatter;
class ErrorHandler;

// Enhanced logging levels for NLM
enum class EnhancedLogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6
};

// Neural computation modes
enum class PlasticityMode {
    STDP,
    Hebbian,
    Combined
};

// Experiment types for advanced testing
enum class ExperimentType {
    Connectivity,
    Plasticity,
    STDP,
    Learning,
    Integration
};

// Experiment execution modes
enum class ExperimentMode {
    Quick,
    Thorough,
    Deep
};

// Agent behavior modes
enum class AgentMode {
    Passive,
    Active,
    Reactive
};

// Structured data for machine-readable output
struct NLMMetrics {
    std::string timestamp;
    int64_t neuronCount;
    int64_t regionCount;
    int64_t synapseCount;
    int64_t spikeCount;
    double totalTime;
    double firingRate;
    double connectionDensity;
    std::map<std::string, double> plasticityMetrics;
    std::map<std::string, std::string> experimentResults;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

// Enhanced Console Logger with advanced features
class AdvancedConsoleLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    AdvancedConsoleLogger();
    explicit AdvancedConsoleLogger(EnhancedLogLevel level);
    ~AdvancedConsoleLogger();
    
    void log(const std::string& message, EnhancedLogLevel level = EnhancedLogLevel::Info,
             const std::string& file = "", int line = 0, const std::string& function = "");
    void setLevel(EnhancedLogLevel level);
    EnhancedLogLevel getLevel() const;
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void enableStructuredLogging(bool enable);
    void setLogFormat(const std::string& format);
    void logStructured(const std::map<std::string, std::string>& data);
    std::string formatLogEntry(const std::string& message, EnhancedLogLevel level,
                               const std::string& file = "", int line = 0, const std::string& function = "") const;
    std::string levelToString(EnhancedLogLevel level) const;
    std::string levelToColor(EnhancedLogLevel level) const;
};

// Enhanced Configuration Manager with multi-source support
class ConfigurationManager {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    bool loadFromFile(const std::string& filepath);
    bool loadFromCommandLine(int argc, char** argv);
    bool loadFromDefaults();
    bool saveToFile(const std::string& filepath) const;
    std::string get(const std::string& key) const;
    int64_t getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    bool has(const std::string& key) const;
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime);
    std::vector<std::string> getKeys() const;
    bool validate() const;
    std::string summary() const;
    void mergeConfigurations(const ConfigurationManager& other);
    ConfigSource getSource(const std::string& key) const;
};

// Advanced Performance Profiler with detailed metrics
class PerformanceProfiler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startTimer(const std::string& operation);
    void stopTimer(const std::string& operation);
    double getElapsedTime(const std::string& operation) const;
    void enableProfiling(bool enable);
    bool isProfilingEnabled() const;
    void setProfilerFile(const std::string& filepath);
    void logPerformanceReport() const;
    void reset();
    void startCPUProfiling();
    void stopCPUProfiling();
    void startMemoryProfiling();
    void stopMemoryProfiling();
    double getCurrentMemoryUsage() const;
    int getCurrentThreadCount() const;
    double getCPUUsage() const;
};

// Advanced Output Formatter with multiple output modes
class OutputFormatter {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    OutputFormatter();
    ~OutputFormatter();
    
    void setOutputMode(const std::string& mode);
    void setOutputFile(const std::string& filepath);
    void setOutputFormat(const std::string& format);
    void printHelp(const std::map<std::string, std::string>& commandDescriptions) const;
    void printVersion() const;
    void printConfiguration(const ConfigurationManager& config) const;
    void printMetrics(const NLMMetrics& metrics) const;
    void printProgress(const std::string& message, double progress = -1.0);
    void printError(const std::string& error, const std::string& suggestion = "");
    void printSuccess(const std::string& message);
    void printWarning(const std::string& warning);
    void printMachineReadable(const std::string& data) const;
    void printHumanReadable(const std::string& data) const;
    void clearScreen();
    void setPager(bool enable);
};

// Advanced Error Handler with recovery mechanisms
class AdvancedErrorHandler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    AdvancedErrorHandler();
    ~AdvancedErrorHandler();
    
    bool handleError(const std::string& error, ErrorType type = ErrorType::General);
    void setExitOnError(bool exit);
    int getExitCode() const;
    void addErrorRecovery(const std::string& errorPattern,
                         const std::string& recoveryCommand,
                         const std::string& description);
    void setGracefulDegradation(bool enable);
    bool shouldGracefullyDegrade() const;
    void logErrorDetails(const std::string& error, const std::string& context = "");
};

// Core NLM Simulation with enhanced functionality
class EnhancedNLMSimulation {
public:
    virtual ~EnhancedNLMSimulation() = default;
    
    virtual bool initialize(const ConfigurationManager& config) = 0;
    virtual void run(int steps) = 0;
    virtual void reset() = 0;
    virtual void logStatus() const = 0;
    virtual int64_t getTotalNeuronCount() const = 0;
    virtual int64_t getTotalSynapseCount() const = 0;
    virtual int64_t getTotalSpikeCount() const = 0;
    virtual double getAverageFiringRate() const = 0;
    virtual NLMMetrics captureMetrics() const = 0;
    virtual void exportResults(const std::string& format,
                               const std::string& filepath) const = 0;
    virtual void runConnectivityTest() const = 0;
    virtual void runPlasticityExperiment() const = 0;
    virtual void runStdpVerification() const = 0;
    virtual void runLearningExperiment() const = 0;
    virtual void runAdvancedExperiments(const std::string& type,
                                        const std::string& mode) const = 0;
};

// Main Brain Simulation Implementation
class BrainSimulation : public EnhancedNLMSimulation {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    BrainSimulation(const ConfigurationManager& config);
    ~BrainSimulation() override;
    
    bool initialize(const ConfigurationManager& config) override;
    void run(int steps) override;
    void reset() override;
    void logStatus() const override;
    int64_t getTotalNeuronCount() const override;
    int64_t getTotalSynapseCount() const override;
    int64_t getTotalSpikeCount() const override;
    double getAverageFiringRate() const override;
    NLMMetrics captureMetrics() const override;
    void exportResults(const std::string& format,
                       const std::string& filepath) const override;
    void runConnectivityTest() const override;
    void runPlasticityExperiment() const override;
    void runStdpVerification() const override;
    void runLearningExperiment() const override;
    void runAdvancedExperiments(const std::string& type,
                                const std::string& mode) const override;
    
    // Learning experiment helper
    struct LearningExperiment {
        std::shared_ptr<Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<NeuronId> mostActiveNeurons;
        
        LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
            : brain(b), seed(s), initialSynapseCount(0) {
            if (!b) {
                NLM_LOG_ERROR("LearningExperiment created with null brain pointer");
                throw std::invalid_argument("Brain pointer cannot be null");
            }
        }
        
        void recordInitialState() {
            if (!brain) {
                NLM_LOG_ERROR("recordInitialState called on null brain");
                return;
            }
            
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            // Record initial weights from first region
            if (auto* region = brain->getRegion(RegionId(1))) {
                auto& synapses = region->getSynapses();
                initialWeights.reserve(synapses.size());
                
                for (size_t i = 0; i < synapses.size(); ++i) {
                    initialWeights.push_back(synapses[i]->getWeight());
                }
            }
            
            NLM_LOG_INFO("Initial state recorded:");
            NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
            if (!initialWeights.empty()) {
                float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float mean = sum / initialWeights.size();
                NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
            }
        }
        
        void recordFinalState() {
            if (!brain) {
                NLM_LOG_ERROR("recordFinalState called on null brain");
                return;
            }
            
            finalWeights.clear();
            
            // Record final weights from first region
            if (auto* region = brain->getRegion(RegionId(1))) {
                auto& synapses = region->getSynapses();
                finalWeights.reserve(synapses.size());
                
                for (size_t i = 0; i < synapses.size(); ++i) {
                    finalWeights.push_back(synapses[i]->getWeight());
                }
            }
            
            mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
            
            NLM_LOG_INFO("Final state recorded:");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            if (!finalWeights.empty()) {
                float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float mean = sum / finalWeights.size();
                NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
            }
        }
        
        void computeStatistics() {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Learning Experiment Results ===");
            NLM_LOG_INFO("");
            
            if (initialWeights.empty() || finalWeights.empty()) {
                NLM_LOG_INFO("ERROR: No weights recorded");
                return;
            }
            
            // Compute weight changes
            float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float initialMean = initialSum / initialWeights.size();
            float finalMean = finalSum / finalWeights.size();
            
            NLM_LOG_INFO("Weight Statistics:");
            NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
            NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
            NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
            
            // Count synapses that changed significantly
            size_t strengthened = 0;
            size_t weakened = 0;
            size_t unchanged = 0;
            
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) ++strengthened;
                else if (delta < -0.01f) ++weakened;
                else ++unchanged;
            }
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Synaptic Changes:");
            NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
            NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
            NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Spike Activity:");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
            
            // Determine if learning occurred
            bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                    (strengthened > 0 || weakened > 0);
            
            NLM_LOG_INFO("");
            if (learningOccurred) {
                NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            } else {
                NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
            }
        }
    };
};

// Main Enhanced NLM Application
class EnhancedNLMApplication {
private:
    ConfigurationManager configManager;
    AdvancedLogger loggerManager;
    PerformanceProfiler performanceProfiler;
    OutputFormatter outputFormatter;
    AdvancedErrorHandler errorHandler;
    
    std::unique_ptr<EnhancedNLMSimulation> simulation;
    
    // Command-line configuration
    bool verbose;
    bool debug;
    bool performanceMode;
    bool profile;
    bool stats;
    bool exportResults;
    std::string exportFormat;
    std::string replayPath;
    bool batchMode;
    bool developmentEnabled;
    std::string plasticityMode;
    int neurons;
    int regions;
    double timestep;
    int steps;
    std::string experimentType;
    std::string experimentMode;
    int experimentSeed;
    std::string experimentId;
    std::string agentMode;
    bool agentLearning;
    double agentExploreRate;
    double agentExploitRate;
    std::string configFile;

public:
    EnhancedNLMApplication(int argc, char** argv);
    ~EnhancedNLMApplication();
    
    bool run();
    void cleanup();
    
private:
    void parseCommandLine(int argc, char** argv);
    void loadConfiguration();
    void initializeSystem();
    void runCoreSimulation() const;
    void runAdvancedTests() const;
    void exportResults() const;
    void showStatistics() const;
    void showHelp() const;
    void showVersion() const;
    bool validateSystemConfiguration() const;
    void setupSystem(const ConfigurationManager& config) const;
    NLMMetrics captureSystemMetrics() const;
    void logSystemConfiguration() const;
    void setupBatchMode() const;
    void replayFromCheckpoint() const;
    void runSingleExperiment() const;
    void runExperimentSuite() const;
    void setupAgentBehavior() const;
    std::string generateExperimentId() const;
    void setupPerformanceMonitoring() const;
    void setupOutputFormatting() const;
};

// Helper Functions
std::string trim(const std::string& str);
std::string toLower(const std::string& str);
std::string getCurrentTimestamp();
EnhancedLogLevel stringToEnhancedLogLevel(const std::string& level);
PlasticityMode stringToPlasticityMode(const std::string& mode);
ExperimentType stringToExperimentType(const std::string& type);
ExperimentMode stringToExperimentMode(const std::string& mode);
AgentMode stringToAgentMode(const std::string& mode);
bool parseBoolean(const std::string& value);
int parseInt(const std::string& value);
double parseDouble(const std::string& value);
float parseFloat(const std::string& value);
std::string formatDuration(double seconds);
double calculateEstimatedTimeRemaining(int currentStep, int totalSteps, double elapsedTime);

// Global function for banner
void printBanner();

// Global learning experiment helper
LearningExperiment createLearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);

// Legacy compatibility function
void runLegacyBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runLegacyPlasticityExperiment(std::shared_ptr<Brain> brain);
void runLegacyStdpVerification(std::shared_ptr<Brain> brain);

// Main function
int main(int argc, char** argv);