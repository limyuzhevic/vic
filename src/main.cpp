// NLM (熙然) - Neural Learning Machine
// Refactored Phase 2: Real Neural Computation with improved architecture
// 
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Enhanced configuration management and modular design
// - Improved error handling and documentation

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
#include <memory>
#include <functional>
#include <chrono>
#include <csignal>
#include <algorithm>
#include <stdexcept>
#include <map>

using namespace nlm;

// Forward declarations for improved organization
class CommandLineParser;
class ExperimentController;
class DebugVisualizer;

// Global state management for signal handling and configuration
// This improves code organization and helps maintain better separation of concerns.
// The refactored approach ensures clearer responsibility distribution, improved error handling, and enhanced extensibility.
// Key improvements focus on modular design, making the code more maintainable and robust for complex neural simulation scenarios.

// Utility functions for configuration and initialization
namespace nlm::utils {
    // Parse configuration from command line arguments
    bool parseCommandLineArgs(int argc, char** argv, std::shared_ptr<Config> config);
    
    // Setup logging configuration
    void setupLogging(const std::shared_ptr<Config>& config);
    
    // Validate configuration parameters
    bool validateConfiguration(const std::shared_ptr<Config>& config);
    
    // Print banner and help information
    void printHelp(const std::string& programName);
    void printBanner();
    
    // Safe exit with cleanup
    void safeExit(int exitCode);
}

// Main application entry point with improved structure
class NLMApplication {
public:
    NLMApplication(int argc, char** argv);
    ~NLMApplication();
    
    // Disable copying, enable moving
    NLMApplication(const NLMApplication&) = delete;
    NLMApplication& operator=(const NLMApplication&) = delete;
    NLMApplication(NLMApplication&&) noexcept;
    NLMApplication& operator=(NLMApplication&&) noexcept;
    
    // Initialize application components
    bool initialize();
    
    // Run the main application logic
    int run();
    
    // Cleanup and shutdown
    void shutdown();
    
    // Get application status
    bool isRunning() const;
    const std::shared_ptr<Config>& getConfig() const;
    
private:
    // Application state
    bool running_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<RandomGenerator> rng_;
    std::shared_ptr<SimulationClock> clock_;
    
    // Application components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Environment> environment_;
    
    // Command line parser
    std::unique_ptr<CommandLineParser> parser_;
    
    // Signal handling
    static void signalHandler(int signal);
    static bool shouldExit;
    
    // Private initialization methods
    bool setupConfiguration(int argc, char** argv);
    bool setupLogging();
    bool setupRandomGenerator();
    bool setupSimulationClock();
    bool initializeBrain();
    bool initializeEnvironment();
    
    // Private execution methods
    int runPhase1Demo();
    int runPhase2Demo();
    int runPhase6Demo();
    int runCustomExperiment();
    
    // Utility methods
    void setupSignalHandlers();
    void restoreSignalHandlers();
    void logStartupInfo() const;
    void logConfigurationInfo() const;
};

// Experiment controller for managing different experimental scenarios
class ExperimentController {
public:
    enum class ExperimentType {
        BasicConnectivity,
        PlasticityLearning,
        STDPVerification,
        Neuromodulation,
        MemoryFormation,
        Development,
        Custom
    };
    
    ExperimentController(std::shared_ptr<Brain> brain);
    ~ExperimentController();
    
    // Run specific experiment
    bool runExperiment(ExperimentType type);
    bool runExperiment(ExperimentType type, const std::map<std::string, std::string>& parameters);
    
    // Get experiment status
    bool isRunning() const;
    std::string getCurrentExperimentName() const;
    
    // Get experiment statistics
    size_t getTotalSteps() const;
    size_t getTotalSpikes() const;
    float getAverageFiringRate() const;
    
    // Experiment callbacks and hooks
    std::function<void(const std::string&)> onExperimentStart;
    std::function<void(const std::string&)> onExperimentEnd;
    std::function<void(float)> onProgressUpdate;
    
private:
    bool running_;
    std::shared_ptr<Brain> brain_;
    std::string currentExperimentName_;
    size_t totalSteps_;
    size_t totalSpikes_;
    
    // Internal experiment implementations
    bool runBasicConnectivityExperiment();
    bool runPlasticityLearningExperiment();
    bool runSTDPVerificationExperiment();
    bool runNeuromodulationExperiment();
    bool runMemoryFormationExperiment();
    bool runDevelopmentExperiment();
    bool runCustomExperiment(const std::map<std::string, std::string>& parameters);
    
    // Utility functions
    void recordMetrics() const;
    void saveCheckpoint(const std::string& filename) const;
    void loadCheckpoint(const std::string& filename);
};

// Debug visualizer for monitoring and analysis
class DebugVisualizer {
public:
    DebugVisualizer(std::shared_ptr<Brain> brain);
    ~DebugVisualizer();
    
    // Enable/disable visualization
    void enable(bool enabled);
    bool isEnabled() const;
    
    // Set visualization mode
    enum class VisualizationMode {
        NeuralActivity,
        SynapticWeights,
        MemoryTraces,
        NetworkTopology,
        PerformanceMetrics,
        Custom
    };
    
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Update visualization with current brain state
    void update();
    
    // Export visualization data
    bool exportToFile(const std::string& filename) const;
    bool exportToJSON(const std::string& filename) const;
    bool exportToCSV(const std::string& filename) const;
    
    // Get visualization data
    std::string getNeuralActivityData() const;
    std::string getSynapticWeightsData() const;
    std::string getMemoryTracesData() const;
    
    // Configure visualization parameters
    void setUpdateInterval(double interval);
    void setMaxDataPoints(size_t maxPoints);
    void setOutputDirectory(const std::string& directory);
    
private:
    bool enabled_;
    std::shared_ptr<Brain> brain_;
    VisualizationMode mode_;
    double updateInterval_;
    size_t maxDataPoints_;
    std::string outputDirectory_;
    
    // Visualization data buffers
    std::vector<float> neuralActivityData_;
    std::vector<float> synapticWeightsData_;
    std::vector<float> memoryTracesData_;
    
    // Internal visualization methods
    void updateNeuralActivityVisualization();
    void updateSynapticWeightsVisualization();
    void updateMemoryTracesVisualization();
    void updateNetworkTopologyVisualization();
    void updatePerformanceMetricsVisualization();
};

// Command line parser for enhanced argument handling
class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    ~CommandLineParser();
    
    // Parse command line arguments
    bool parse();
    
    // Get parsed values
    std::string getProgramName() const;
    std::string getConfigFile() const;
    std::string getOutputFile() const;
    std::string getLogLevel() const;
    bool getEnableDebug() const;
    bool getEnableVisualization() const;
    std::string getExperimentType() const;
    std::string getExperimentParams() const;
    bool getHelp() const;
    bool getVersion() const;
    
    // Get all key-value pairs
    std::map<std::string, std::string> getAllParameters() const;
    
    // Check if argument exists
    bool hasArgument(const std::string& name) const;
    
    // Get help text
    static std::string getUsageText();
    
private:
    int argc_;
    char** argv_;
    std::map<std::string, std::string> parameters_;
    
    // Internal parsing methods
    void parseStandardArgs();
    void parseExperimentArgs();
    void parseDebugArgs();
    void parseOutputArgs();
    
    // Argument validation
    bool validateArguments() const;
    
    // Help generation
    static std::string generateHelp() const;
};

// Global instances for easy access
namespace nlm::globals {
    extern std::shared_ptr<Config> config;
    extern std::shared_ptr<Logger> logger;
    extern std::shared_ptr<RandomGenerator> rng;
    extern std::shared_ptr<SimulationClock> clock;
    
    void initializeGlobals();
    void cleanupGlobals();
}

// Utility functions implementation
namespace nlm::utils {
    bool parseCommandLineArgs(int argc, char** argv, std::shared_ptr<Config> config) {
        CommandLineParser parser(argc, argv);
        if (!parser.parse()) {
            return false;
        }
        
        // Apply parsed parameters to config
        auto params = parser.getAllParameters();
        for (const auto& param : params) {
            config->set(param.first, param.second);
        }
        
        return true;
    }
    
    void setupLogging(const std::shared_ptr<Config>& config) {
        auto logger = std::make_shared<Logger>();
        
        // Determine log level from config
        std::string logLevel = config->getOr<std::string>("log_level", "Info");
        LogLevel level;
        
        if (logLevel == "Debug") level = LogLevel::Debug;
        else if (logLevel == "Info") level = LogLevel::Info;
        else if (logLevel == "Warning") level = LogLevel::Warning;
        else if (logLevel == "Error") level = LogLevel::Error;
        else level = LogLevel::Info; // Default
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
        
        Logger::setGlobal(logger);
    }
    
    bool validateConfiguration(const std::shared_ptr<Config>& config) {
        // Basic validation - check for required parameters
        if (config->has("neuron_count")) {
            auto neuronCount = config->get<size_t>("neuron_count");
            if (neuronCount == 0) {
                return false;
            }
        }
        
        if (config->has("simulation_timestep")) {
            auto timestep = config->get<double>("simulation_timestep");
            if (timestep <= 0.0) {
                return false;
            }
        }
        
        return true;
    }
    
    void printHelp(const std::string& programName) {
        std::cout << "NLM - Neural Learning Machine" << std::endl;
        std::cout << "=" << std::string(programName.length() + 20, '=') << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                  Show this help message" << std::endl;
        std::cout << "  --version               Show version information" << std::endl;
        std::cout << "  --config <file>         Load configuration from file" << std::endl;
        std::cout << "  --output <file>         Save output to file" << std::endl;
        std::cout << "  --log-level <level>     Set log level (Debug, Info, Warning, Error)" << std::endl;
        std::cout << "  --enable-debug          Enable debug mode" << std::endl;
        std::cout << "  --enable-visualization  Enable visualization" << std::endl;
        std::cout << "  --experiment <type>     Run specific experiment (BasicConnectivity, PlasticityLearning, STDPVerification, Neuromodulation, MemoryFormation, Development, Custom)" << std::endl;
        std::cout << "  --params <params>       Experiment parameters as JSON string" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << programName << " --config config.cfg --experiment PlasticityLearning" << std::endl;
        std::cout << "  " << programName << " --experiment STDPVerification --enable-debug" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    }
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void safeExit(int exitCode) {
        // Perform cleanup
        nlm::globals::cleanupGlobals();
        std::exit(exitCode);
    }
}

// NLMApplication implementation
NLMApplication::NLMApplication(int argc, char** argv) 
    : running_(false), config_(std::make_shared<Config>()),
      logger_(std::make_shared<Logger>()), rng_(nullptr), clock_(nullptr),
      brain_(nullptr), environment_(nullptr), parser_(nullptr) {
    
    setupConfiguration(argc, argv);
}

NLMApplication::~NLMApplication() {
    shutdown();
}

NLMApplication::NLMApplication(NLMApplication&& other) noexcept
    : running_(other.running_), config_(std::move(other.config_)),
      logger_(std::move(other.logger_)), rng_(std::move(other.rng_)),
      clock_(std::move(other.clock_)), brain_(std::move(other.brain_)),
      environment_(std::move(other.environment_)), parser_(std::move(other.parser_)) {
    other.running_ = false;
}

NLMApplication& NLMApplication::operator=(NLMApplication&& other) noexcept {
    if (this != &other) {
        shutdown();
        
        running_ = other.running_;
        config_ = std::move(other.config_);
        logger_ = std::move(other.logger_);
        rng_ = std::move(other.rng_);
        clock_ = std::move(other.clock_);
        brain_ = std::move(other.brain_);
        environment_ = std::move(other.environment_);
        parser_ = std::move(other.parser_);
        
        other.running_ = false;
    }
    return *this;
}

bool NLMApplication::initialize() {
    if (running_) {
        return false;
    }
    
    try {
        // Setup components in dependency order
        if (!setupLogging()) {
            return false;
        }
        
        if (!setupRandomGenerator()) {
            return false;
        }
        
        if (!setupSimulationClock()) {
            return false;
        }
        
        if (!initializeBrain()) {
            return false;
        }
        
        if (!initializeEnvironment()) {
            return false;
        }
        
        setupSignalHandlers();
        
        running_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during initialization: " << e.what() << std::endl;
        shutdown();
        return false;
    }
}

int NLMApplication::run() {
    if (!running_) {
        std::cerr << "Application not initialized. Call initialize() first." << std::endl;
        return 1;
    }
    
    try {
        // Determine which experiment to run based on configuration
        std::string experimentType = config_->getOr<std::string>("experiment_type", "Phase2Demo");
        
        if (experimentType == "Phase1Demo") {
            return runPhase1Demo();
        } else if (experimentType == "Phase2Demo") {
            return runPhase2Demo();
        } else if (experimentType == "Phase6Demo") {
            return runPhase6Demo();
        } else if (experimentType == "Custom") {
            return runCustomExperiment();
        } else {
            std::cerr << "Unknown experiment type: " << experimentType << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
        return 1;
    }
}

void NLMApplication::shutdown() {
    if (running_) {
        restoreSignalHandlers();
        
        // Shutdown components in reverse dependency order
        environment_ = nullptr;
        brain_ = nullptr;
        clock_ = nullptr;
        rng_ = nullptr;
        logger_ = nullptr;
        config_ = nullptr;
        
        running_ = false;
    }
}

bool NLMApplication::isRunning() const {
    return running_;
}

const std::shared_ptr<Config>& NLMApplication::getConfig() const {
    return config_;
}

// Utility function implementations
void NLMApplication::signalHandler(int signal) {
    shouldExit = true;
}

bool NLMApplication::shouldExit = false;

bool NLMApplication::setupConfiguration(int argc, char** argv) {
    // Parse command line arguments
    parser_ = std::make_unique<CommandLineParser>(argc, argv);
    if (!parser_->parse()) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return false;
    }
    
    // Load configuration file if specified
    std::string configFile = parser_->getConfigFile();
    if (!configFile.empty()) {
        if (!config_->loadFromFile(configFile)) {
            std::cerr << "Failed to load configuration from: " << configFile << std::endl;
            return false;
        }
    }
    
    // Override with command line arguments
    config_->overrideWithArgs(argc, argv);
    
    return true;
}

bool NLMApplication::setupLogging() {
    std::string logLevel = config_->getOr<std::string>("log_level", "Info");
    LogLevel level;
    
    if (logLevel == "Debug") level = LogLevel::Debug;
    else if (logLevel == "Info") level = LogLevel::Info;
    else if (logLevel == "Warning") level = LogLevel::Warning;
    else if (logLevel == "Error") level = LogLevel::Error;
    else level = LogLevel::Info;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    return true;
}

bool NLMApplication::setupRandomGenerator() {
    uint64_t seed = config_->getOr<uint64_t>("random_seed", 42);
    rng_ = std::make_shared<RandomGenerator>(seed);
    return true;
}

bool NLMApplication::setupSimulationClock() {
    double timestep = config_->getOr<double>("simulation_timestep", 0.001);
    clock_ = std::make_shared<SimulationClock>(timestep);
    return true;
}

bool NLMApplication::initializeBrain() {
    size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config_->getOr<size_t>("region_count", 1);
    float connectionProbability = config_->getOr<float>("connection_probability", 0.1f);
    
    // Create brain
    brain_ = std::make_shared<Brain>(config_);
    
    if (!brain_->initialize()) {
        std::cerr << "Failed to initialize brain." << std::endl;
        return false;
    }
    
    return true;
}

bool NLMApplication::initializeEnvironment() {
    // Create environment if specified
    std::string envType = config_->getOr<std::string>("environment_type", "");
    if (!envType.empty()) {
        environment_ = std::make_shared<Environment>(config_);
        
        if (!environment_->initialize()) {
            std::cerr << "Failed to initialize environment." << std::endl;
            return false;
        }
    }
    
    return true;
}

void NLMApplication::setupSignalHandlers() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

void NLMApplication::restoreSignalHandlers() {
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

void NLMApplication::logStartupInfo() const {
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Starting NLM Neural Learning Machine");
    NLM_LOG_INFO("Version: 0.1.0");
    NLM_LOG_INFO("Build: " << __DATE__ << " " << __TIME__);
}

void NLMApplication::logConfigurationInfo() const {
    NLM_LOG_INFO("Configuration loaded from: " << config_->getSource("experiment_type"));
    NLM_LOG_INFO("Log level: " << config_->getOr<std::string>("log_level", "Info"));
    NLM_LOG_INFO("Random seed: " << config_->getOr<uint64_t>("random_seed", 42));
    NLM_LOG_INFO("Neuron count: " << config_->getOr<size_t>("neuron_count", 1000));
    NLM_LOG_INFO("Region count: " << config_->getOr<size_t>("region_count", 1));
    NLM_LOG_INFO("Connection probability: " << config_->getOr<float>("connection_probability", 0.1f));
}

// ExperimentController implementation
ExperimentController::ExperimentController(std::shared_ptr<Brain> brain)
    : running_(false), brain_(brain), currentExperimentName_(""),
      totalSteps_(0), totalSpikes_(0) {
}

ExperimentController::~ExperimentController() {
    shutdown();
}

bool ExperimentController::runExperiment(ExperimentType type) {
    return runExperiment(type, std::map<std::string, std::string>());
}

bool ExperimentController::runExperiment(ExperimentType type, const std::map<std::string, std::string>& parameters) {
    if (running_) {
        return false;
    }
    
    running_ = true;
    totalSteps_ = 0;
    totalSpikes_ = 0;
    
    try {
        switch (type) {
            case ExperimentType::BasicConnectivity:
                currentExperimentName_ = "Basic Connectivity Test";
                return runBasicConnectivityExperiment();
            case ExperimentType::PlasticityLearning:
                currentExperimentName_ = "Plasticity Learning Experiment";
                return runPlasticityLearningExperiment();
            case ExperimentType::STDPVerification:
                currentExperimentName_ = "STDP Verification Experiment";
                return runSTDPVerificationExperiment();
            case ExperimentType::Neuromodulation:
                currentExperimentName_ = "Neuromodulation Experiment";
                return runNeuromodulationExperiment();
            case ExperimentType::MemoryFormation:
                currentExperimentName_ = "Memory Formation Experiment";
                return runMemoryFormationExperiment();
            case ExperimentType::Development:
                currentExperimentName_ = "Development Experiment";
                return runDevelopmentExperiment();
            case ExperimentType::Custom:
                currentExperimentName_ = "Custom Experiment";
                return runCustomExperiment(parameters);
        }
    } catch (const std::exception& e) {
        std::cerr << "Experiment error: " << e.what() << std::endl;
    }
    
    running_ = false;
    return false;
}

bool ExperimentController::isRunning() const {
    return running_;
}

std::string ExperimentController::getCurrentExperimentName() const {
    return currentExperimentName_;
}

size_t ExperimentController::getTotalSteps() const {
    return totalSteps_;
}

size_t ExperimentController::getTotalSpikes() const {
    return totalSpikes_;
}

float ExperimentController::getAverageFiringRate() const {
    if (totalSteps_ == 0) return 0.0f;
    return static_cast<float>(totalSpikes_) / totalSteps_;
}

bool ExperimentController::runBasicConnectivityExperiment() {
    NLM_LOG_INFO("Running Basic Connectivity Experiment");
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No region found for Basic Connectivity experiment");
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found for Basic Connectivity experiment");
        return false;
    }
    
    size_t initialSpikes = brain_->getTotalSpikeCount();
    
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain_->step(step, step * 0.001);
        ++totalSteps_;
    }
    
    size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Basic Connectivity Experiment completed: " << spikes << " spikes generated");
    
    return true;
}

bool ExperimentController::runPlasticityLearningExperiment() {
    NLM_LOG_INFO("Running Plasticity Learning Experiment");
    
    // Record initial state
    std::vector<float> initialWeights;
    auto* region = brain_->getRegion(RegionId(1));
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Enable plasticity
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Apply input patterns
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain_->step(step, step * 0.001);
        ++totalSteps_;
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " << step << ": Spikes: " << brain_->getTotalSpikeCount() << ", Firing: " << brain_->getFiringNeuronCount());
        }
    }
    
    NLM_LOG_INFO("Plasticity Learning Experiment completed");
    return true;
}

bool ExperimentController::runSTDPVerificationExperiment() {
    NLM_LOG_INFO("Running STDP Verification Experiment");
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 5) {
        NLM_LOG_ERROR("Not enough synapses for STDP verification");
        return false;
    }
    
    auto& synapses = region->getSynapses();
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Create correlated activity
    for (int trial = 0; trial < 50; ++trial) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            neurons[0]->injectCurrent(60.0f);
            brain_->step(trial * 2, trial * 2 * 0.001);
            
            neurons[1]->injectCurrent(60.0f);
            brain_->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            
            ++totalSteps_;
        }
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("! No change: STDP may not be triggering");
    }
    
    return true;
}

bool ExperimentController::runNeuromodulationExperiment() {
    NLM_LOG_INFO("Running Neuromodulation Experiment");
    
    // This would test dopamine, curiosity, novelty integration
    // For now, just log that it's a placeholder
    NLM_LOG_INFO("Neuromodulation experiment - placeholder implementation");
    
    return true;
}

bool ExperimentController::runMemoryFormationExperiment() {
    NLM_LOG_INFO("Running Memory Formation Experiment");
    
    // This would test working memory, episodic memory, and associative memory
    // For now, just log that it's a placeholder
    NLM_LOG_INFO("Memory formation experiment - placeholder implementation");
    
    return true;
}

bool ExperimentController::runDevelopmentExperiment() {
    NLM_LOG_INFO("Running Development Experiment");
    
    // This would test developmental stages and plasticity changes over time
    // For now, just log that it's a placeholder
    NLM_LOG_INFO("Development experiment - placeholder implementation");
    
    return true;
}

bool ExperimentController::runCustomExperiment(const std::map<std::string, std::string>& parameters) {
    NLM_LOG_INFO("Running Custom Experiment");
    
    // Custom experiment implementation would go here
    // This allows users to specify custom experiment parameters
    
    NLM_LOG_INFO("Custom experiment parameters:");
    for (const auto& param : parameters) {
        NLM_LOG_INFO("  " << param.first << ": " << param.second);
    }
    
    return true;
}

void ExperimentController::recordMetrics() const {
    // Record experiment metrics for analysis
    NLM_LOG_INFO("Experiment metrics recorded:");
    NLM_LOG_INFO("  Total steps: " << totalSteps_);
    NLM_LOG_INFO("  Total spikes: " << totalSpikes_);
    NLM_LOG_INFO("  Average firing rate: " << getAverageFiringRate() << " Hz");
}

void ExperimentController::saveCheckpoint(const std::string& filename) const {
    // Save experiment state to checkpoint file
    if (brain_) {
        brain_->save(filename);
    }
}

void ExperimentController::loadCheckpoint(const std::string& filename) {
    // Load experiment state from checkpoint file
    if (brain_) {
        brain_->load(filename);
    }
}

// DebugVisualizer implementation
DebugVisualizer::DebugVisualizer(std::shared_ptr<Brain> brain)
    : enabled_(false), brain_(brain), mode_(VisualizationMode::NeuralActivity),
      updateInterval_(0.1), maxDataPoints_(1000), outputDirectory_("./visualization") {
}

DebugVisualizer::~DebugVisualizer() {
    // Cleanup visualization resources
}

void DebugVisualizer::enable(bool enabled) {
    enabled_ = enabled;
}

bool DebugVisualizer::isEnabled() const {
    return enabled_;
}

void DebugVisualizer::setVisualizationMode(VisualizationMode mode) {
    mode_ = mode;
}

DebugVisualizer::VisualizationMode DebugVisualizer::getVisualizationMode() const {
    return mode_;
}

void DebugVisualizer::update() {
    if (!enabled_ || !brain_) return;
    
    switch (mode_) {
        case VisualizationMode::NeuralActivity:
            updateNeuralActivityVisualization();
            break;
        case VisualizationMode::SynapticWeights:
            updateSynapticWeightsVisualization();
            break;
        case VisualizationMode::MemoryTraces:
            updateMemoryTracesVisualization();
            break;
        case VisualizationMode::NetworkTopology:
            updateNetworkTopologyVisualization();
            break;
        case VisualizationMode::PerformanceMetrics:
            updatePerformanceMetricsVisualization();
            break;
        case VisualizationMode::Custom:
            // Custom visualization logic would go here
            break;
    }
}

bool DebugVisualizer::exportToFile(const std::string& filename) const {
    // Export visualization data to file
    return false; // TODO: Implement file export
}

bool DebugVisualizer::exportToJSON(const std::string& filename) const {
    // Export visualization data to JSON
    return false; // TODO: Implement JSON export
}

bool DebugVisualizer::exportToCSV(const std::string& filename) const {
    // Export visualization data to CSV
    return false; // TODO: Implement CSV export
}

std::string DebugVisualizer::getNeuralActivityData() const {
    std::ostringstream oss;
    oss << "Neural Activity Data:" << std::endl;
    oss << "  Active neurons: " << (brain_ ? brain_->getActiveNeuronCount() : 0) << std::endl;
    oss << "  Firing neurons: " << (brain_ ? brain_->getFiringNeuronCount() : 0) << std::endl;
    oss << "  Total spikes: " << (brain_ ? brain_->getTotalSpikeCount() : 0) << std::endl;
    return oss.str();
}

std::string DebugVisualizer::getSynapticWeightsData() const {
    std::ostringstream oss;
    oss << "Synaptic Weights Data:" << std::endl;
    oss << "  Total synapses: " << (brain_ ? brain_->getTotalSynapseCount() : 0) << std::endl;
    oss << "  Average weight: " << (brain_ ? brain_->getExcitationInhibitionRatio() : 0.0f) << std::endl;
    return oss.str();
}

std::string DebugVisualizer::getMemoryTracesData() const {
    std::ostringstream oss;
    oss << "Memory Traces Data:" << std::endl;
    oss << "  Working memory traces: " << (brain_ && brain_->getWorkingMemory() ? brain_->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
    oss << "  Episodic memory episodes: " << (brain_ && brain_->getEpisodicMemory() ? brain_->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
    return oss.str();
}

void DebugVisualizer::setUpdateInterval(double interval) {
    updateInterval_ = interval;
}

void DebugVisualizer::setMaxDataPoints(size_t maxPoints) {
    maxDataPoints_ = maxPoints;
}

void DebugVisualizer::setOutputDirectory(const std::string& directory) {
    outputDirectory_ = directory;
}

void DebugVisualizer::updateNeuralActivityVisualization() {
    // Update neural activity visualization data
    if (brain_) {
        // Add neural activity data to buffer
        float activity = static_cast<float>(brain_->getActiveNeuronCount()) / 
                        std::max(1.0f, static_cast<float>(brain_->getTotalNeuronCount()));
        neuralActivityData_.push_back(activity);
        
        // Limit buffer size
        if (neuralActivityData_.size() > maxDataPoints_) {
            neuralActivityData_.erase(neuralActivityData_.begin());
        }
    }
}

void DebugVisualizer::updateSynapticWeightsVisualization() {
    // Update synaptic weights visualization data
    if (brain_) {
        float eir = brain_->getExcitationInhibitionRatio();
        synapticWeightsData_.push_back(eir);
        
        if (synapticWeightsData_.size() > maxDataPoints_) {
            synapticWeightsData_.erase(synapticWeightsData_.begin());
        }
    }
}

void DebugVisualizer::updateMemoryTracesVisualization() {
    // Update memory traces visualization data
    if (brain_) {
        float workingMemoryTraces = 0.0f;
        float episodicMemoryEpisodes = 0.0f;
        
        if (brain_->getWorkingMemory()) {
            workingMemoryTraces = static_cast<float>(brain_->getWorkingMemory()->getActiveTraces());
        }
        
        if (brain_->getEpisodicMemory()) {
            episodicMemoryEpisodes = static_cast<float>(brain_->getEpisodicMemory()->getEpisodeCount());
        }
        
        memoryTracesData_.push_back(workingMemoryTraces + episodicMemoryEpisodes);
        
        if (memoryTracesData_.size() > maxDataPoints_) {
            memoryTracesData_.erase(memoryTracesData_.begin());
        }
    }
}

void DebugVisualizer::updateNetworkTopologyVisualization() {
    // Update network topology visualization (placeholder)
}

void DebugVisualizer::updatePerformanceMetricsVisualization() {
    // Update performance metrics visualization (placeholder)
}

// CommandLineParser implementation
CommandLineParser::CommandLineParser(int argc, char** argv) 
    : argc_(argc), argv_(argv) {
}

CommandLineParser::~CommandLineParser() {
}

bool CommandLineParser::parse() {
    // Parse standard arguments
    parseStandardArgs();
    
    // Parse experiment arguments
    parseExperimentArgs();
    
    // Parse debug arguments
    parseDebugArgs();
    
    // Parse output arguments
    parseOutputArgs();
    
    // Validate arguments
    return validateArguments();
}

std::string CommandLineParser::getProgramName() const {
    return programName_;
}

std::string CommandLineParser::getConfigFile() const {
    auto it = parameters_.find("config");
    return it != parameters_.end() ? it->second : "";
}

std::string CommandLineParser::getOutputFile() const {
    auto it = parameters_.find("output");
    return it != parameters_.end() ? it->second : "";
}

std::string CommandLineParser::getLogLevel() const {
    auto it = parameters_.find("log_level");
    return it != parameters_.end() ? it->second : "Info";
}

bool CommandLineParser::getEnableDebug() const {
    return parameters_.find("enable_debug") != parameters_.end();
}

bool CommandLineParser::getEnableVisualization() const {
    return parameters_.find("enable_visualization") != parameters_.end();
}

std::string CommandLineParser::getExperimentType() const {
    auto it = parameters_.find("experiment");
    return it != parameters_.end() ? it->second : "";
}

std::string CommandLineParser::getExperimentParams() const {
    auto it = parameters_.find("params");
    return it != parameters_.end() ? it->second : "";
}

bool CommandLineParser::getHelp() const {
    return parameters_.find("help") != parameters_.end();
}

bool CommandLineParser::getVersion() const {
    return parameters_.find("version") != parameters_.end();
}

std::map<std::string, std::string> CommandLineParser::getAllParameters() const {
    return parameters_;
}

bool CommandLineParser::hasArgument(const std::string& name) const {
    return parameters_.find(name) != parameters_.end();
}

std::string CommandLineParser::getUsageText() {
    return generateHelp();
}

void CommandLineParser::parseStandardArgs() {
    // Parse --help and --version
    for (int i = 1; i < argc_; ++i) {
        std::string arg(argv_[i]);
        
        if (arg == "--help" || arg == "-h") {
            parameters_["help"] = "true";
        } else if (arg == "--version" || arg == "-v") {
            parameters_["version"] = "true";
        }
    }
}

void CommandLineParser::parseExperimentArgs() {
    // Parse experiment-related arguments
    for (int i = 1; i < argc_; ++i) {
        std::string arg(argv_[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parameters_[key] = value;
            }
        }
    }
}

void CommandLineParser::parseDebugArgs() {
    // Parse debug-related arguments
    for (int i = 1; i < argc_; ++i) {
        std::string arg(argv_[i]);
        
        if (arg == "--enable-debug") {
            parameters_["enable_debug"] = "true";
        } else if (arg == "--enable-visualization") {
            parameters_["enable_visualization"] = "true";
        }
    }
}

void CommandLineParser::parseOutputArgs() {
    // Parse output-related arguments
    for (int i = 1; i < argc_; ++i) {
        std::string arg(argv_[i]);
        
        if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc_) {
                parameters_["output"] = argv_[++i];
            }
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc_) {
                parameters_["config"] = argv_[++i];
            }
        }
    }
}

bool CommandLineParser::validateArguments() const {
    // Validate that required arguments are present
    if (hasArgument("help")) {
        return true;  // Help is always valid
    }
    
    if (hasArgument("version")) {
        return true;  // Version is always valid
    }
    
    // Check for experiment type if not specified
    if (!hasArgument("experiment") && !hasArgument("enable-debug")) {
        // Default to Phase2Demo if no specific experiment or debug mode is specified
        parameters_["experiment"] = "Phase2Demo";
    }
    
    return true;
}

std::string CommandLineParser::generateHelp() const {
    std::ostringstream oss;
    oss << "NLM - Neural Learning Machine" << std::endl;
    oss << "=" << std::string(30, '=') << std::endl;
    oss << std::endl;
    oss << "Usage: nlm [OPTIONS]" << std::endl;
    oss << std::endl;
    oss << "Options:" << std::endl;
    oss << "  --help                  Show this help message" << std::endl;
    oss << "  --version               Show version information" << std::endl;
    oss << "  --config <file>         Load configuration from file" << std::endl;
    oss << "  --output <file>         Save output to file" << std::endl;
    oss << "  --log-level <level>     Set log level (Debug, Info, Warning, Error)" << std::endl;
    oss << "  --enable-debug          Enable debug mode" << std::endl;
    oss << "  --enable-visualization  Enable visualization" << std::endl;
    oss << "  --experiment <type>     Run specific experiment (BasicConnectivity, PlasticityLearning, STDPVerification, Neuromodulation, MemoryFormation, Development, Custom)" << std::endl;
    oss << "  --params <params>       Experiment parameters as JSON string" << std::endl;
    oss << std::endl;
    oss << "Examples:" << std::endl;
    oss << "  nlm --config config.cfg --experiment PlasticityLearning" << std::endl;
    oss << "  nlm --experiment STDPVerification --enable-debug" << std::endl;
    oss << std::endl;
    oss << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    
    return oss.str();
}

// Global instances
namespace nlm::globals {
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<RandomGenerator> rng;
    std::shared_ptr<SimulationClock> clock;
    
    void initializeGlobals() {
        config = std::make_shared<Config>();
        logger = std::make_shared<Logger>();
        rng = std::make_shared<RandomGenerator>(42);
        clock = std::make_shared<SimulationClock>(0.001);
    }
    
    void cleanupGlobals() {
        config = nullptr;
        logger = nullptr;
        rng = nullptr;
        clock = nullptr;
    }
}

// Main application entry point
int main(int argc, char** argv) {
    try {
        // Initialize global instances
        nlm::globals::initializeGlobals();
        
        // Create and configure application
        NLMApplication app(argc, argv);
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize application." << std::endl;
            return 1;
        }
        
        // Print banner and help if requested
        if (app.getConfig()->getOr<std::string>("experiment_type", "") == "help") {
            nlm::utils::printHelp(argv[0]);
            return 0;
        }
        
        // Run the application
        int result = app.run();
        
        // Cleanup
        app.shutdown();
        nlm::globals::cleanupGlobals();
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        nlm::globals::cleanupGlobals();
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error." << std::endl;
        nlm::globals::cleanupGlobals();
        return 1;
    }
}

} // namespace nlm