# NLM Advanced User Guide

This document provides advanced users with guidance on extending and modifying the NLM (Neural Learning Machine) system.

## Overview

NLM is a modular, extensible neural simulation framework with Phase 6 representing the final integrated artificial brain implementation. The system is designed to be easily extended with new neural components, memory systems, and behaviors.

## Directory Structure

```
src/
├── core/          # Core utilities and types
├── brain/         # Neural computation (Phase 2+)
├── sensory/       # Sensory processing
├── motor/         # Motor control
├── environment/   # Environment interaction
├── agent/         # Agent-brain interface
├── world/         # World representation
├── experiments/   # Experimental frameworks
├── visualization/ # Visualization system
└── performance/   # Performance optimizations

README.md                     # Project overview and building
HOW_TO_USE.md                 # Basic usage guide
ADVANCED_GUIDE.md             # This advanced user guide
```

## Quick Start for Advanced Users

### Setting Up Your Development Environment

```bash
# Clone the repository
mkdir nlm-workspace
cd nlm-workspace
# Add your custom code directory
cd nlm-workspace/src

# Build with CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4

# Run tests
./nlm_test

# Run main demo
./nlm --neurons 1000 --seed 12345
```

### Creating Custom Brain Regions

The NLM brain consists of regions containing neural populations. Here's how to create custom regions:

```cpp
// In your custom source file
#include "src/brain/NeuralRegion.hpp"
#include "src/brain/Brain.hpp"

class CustomRegion : public nlm::NeuralRegion {
public:
    CustomRegion(nlm::RegionId id, const std::string& name) 
        : NeuralRegion(id, name) {}
    
    // Add custom population type
    nlm::PopulationId addCustomPopulation(
        size_t neuronCount, 
        nlm::NeuronType type,
        float connectionProbability = 0.1f,
        float initialWeight = 0.5f
    ) {
        // Add standard population
        auto popId = addPopulation(neuronCount, type);
        
        // Configure custom connectivity for this population
        auto* pop = getPopulation(popId);
        if (pop) {
            // Configure connectivity with specific parameters
            pop->configureConnectivity(connectionProbability, initialWeight);
            
            // Add custom neuromodulation connections
            connectToNeuromodulation(popId);
        }
        
        return popId;
    }
    
    void connectToNeuromodulation(nlm::PopulationId popId) {
        // Implement custom neuromodulation connections
        auto* pop = getPopulation(popId);
        if (!pop) return;
        
        // For each neuron in this population, add neuromodulation inputs
        for (auto* neuron : pop->getNeurons()) {
            // Add custom connections to neuromodulation systems
            // This is implementation-specific based on your needs
            neuron->addCustomNeuromodulationPath(
                nlm::NeuromodulatorType::Dopamine, 
                0.1f,  // connection strength
                nlm::PlasticityFlags::STDP  // plasticity flags
            );
        }
    }
};
```

### Custom Brain Initialization

```cpp
// In your main.cpp or custom application

class CustomBrainInitializer {
public:
    void setupCustomBrain(nlm::Brain* brain, const nlm::Config& config) {
        // Add custom regions
        setupCustomRegions(brain, config);
        
        // Configure custom plasticity rules
        setupCustomPlasticity(brain, config);
        
        // Set up custom neuromodulation
        setupCustomNeuromodulation(brain, config);
        
        // Initialize custom connectivity
        setupCustomConnectivity(brain);
    }
    
private:
    void setupCustomRegions(nlm::Brain* brain, const nlm::Config& config) {
        // Add custom region types
        auto visualRegionId = brain->addRegion("Visual_Cortex");
        auto* visualRegion = brain->getRegion(visualRegionId);
        
        if (visualRegion) {
            // Add visual processing populations
            auto sensoryPop = visualRegion->addPopulation(500, nlm::NeuronType::Sensory);
            auto internalPop = visualRegion->addPopulation(200, nlm::NeuronType::Internal);
            
            // Configure visual-specific parameters
            visualRegion->configureRegion(nlm::RegionType::Sensory, 
                                        nlm::ProcessingStyle::Feedforward);
        }
        
        // Add motor control region
        auto motorRegionId = brain->addRegion("Motor_Cortex");
        auto* motorRegion = brain->getRegion(motorRegionId);
        
        if (motorRegion) {
            // Add motor populations
            auto motorPop = motorRegion->addPopulation(300, nlm::NeuronType::Motor);
            auto proprioceptionPop = motorRegion->addPopulation(100, nlm::NeuronType::Sensory);
        }
    }
    
    void setupCustomPlasticity(nlm::Brain* brain, const nlm::Config& config) {
        // Custom STDP parameters
        if (auto* stdp = brain->getSTDP()) {
            float customLTP = config.getOr<float>("stdp_ltp_weight", 0.02f);
            float customLTD = config.getOr<float>("stdp_ltd_weight", 0.018f);
            float customTau = config.getOr<float>("stdp_tau", 20.0f);
            
            stdp->configure(customLTP, customLTD, customTau);
            
            // Add custom plasticity flags
            stdp->enableEmotionModulation(true);
            stdp->enableActivityHistory(true);
        }
        
        // Custom Hebbian parameters
        if (auto* hebbian = brain->getHebbian()) {
            hebbian->configure(0.1f, 0.9f);
        }
    }
    
    void setupCustomNeuromodulation(nlm::Brain* brain, const nlm::Config& config) {
        // Configure dopamine system with custom parameters
        if (auto* dopamine = brain->getDopamine()) {
            dopamine->setBaseline(config.getOr<float>("dopamine_baseline", 0.1f));
            dopamine->setLearningRate(config.getOr<float>("dopamine_learning_rate", 0.05f));
            dopamine->enablePhasicResponse(true);
        }
        
        // Configure curiosity with custom exploration
        if (auto* curiosity = brain->getCuriosity()) {
            curiosity->setExplorationRate(0.8f);
            curiosity->setNoveltyThreshold(0.3f);
        }
    }
    
    void setupCustomConnectivity(nlm::Brain* brain) {
        // Add custom inter-region connections
        if (auto* visualRegion = brain->getRegion(nlm::RegionId(1))) {
            if (auto* motorRegion = brain->getRegion(nlm::RegionId(2))) {
                // Add visual-to-motor connection
                brain->addInterRegionConnection(
                    nlm::RegionId(1),  // Visual region
                    nlm::RegionId(2),  // Motor region
                    0.3f,             // Connection strength
                    2,                // Delay in steps
                    nlm::PlasticityFlags::STDP | nlm::PlasticityFlags::Hebbian
                );
            }
        }
    }
};

// Usage in your main function
int main() {
    // Initialize configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 2000);
    config->set("region_count", 3);
    config->set("stdp_ltp_weight", 0.025f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Apply custom initialization
    CustomBrainInitializer customInit;
    customInit.setupCustomBrain(brain.get(), *config);
    
    // Your custom simulation loop
    for (size_t step = 0; step < 5000; ++step) {
        // Custom logic here
        if (step % 1000 == 0) {
            brain->logStatus();
        }
        
        brain->step(step, step * 0.001);
    }
    
    // Save your custom brain state
    brain->save("./custom_brain_state.bin");
    
    return 0;
}
```

### Advanced Configuration Management

```cpp
// Custom configuration manager
class CustomConfigManager {
public:
    CustomConfigManager() {
        // Register all custom configuration sections
        registerConfigSections();
    }
    
    void registerConfigSections() {
        // Core brain configuration
        registerBrainConfig();
        
        // Memory systems configuration
        registerMemoryConfig();
        
        // Neuromodulation configuration
        registerNeuromodulationConfig();
        
        // Development configuration
        registerDevelopmentConfig();
    }
    
    nlm::Config createCustomConfig(const std::string& configFile = "") {
        nlm::Config config;
        
        // Apply defaults
        applyDefaultConfiguration(config);
        
        // Load from file if provided
        if (!configFile.empty()) {
            config.loadFromFile(configFile);
        }
        
        // Apply command-line overrides
        applyCommandLineOverrides(config);
        
        return config;
    }
    
private:
    void registerBrainConfig() {
        // Brain-specific configuration
        nlm::ConfigSchema brainSchema;
        brainSchema.addParameter("neuron_count", nlm::ConfigType::Integer, 1000);
        brainSchema.addParameter("region_count", nlm::ConfigType::Integer, 1);
        brainSchema.addParameter("connection_probability", nlm::ConfigType::Float, 0.1f);
        brainSchema.addParameter("simulation_timestep", nlm::ConfigType::Float, 0.001f);
        
        // Store in registry
        configRegistry_["brain"] = brainSchema;
    }
    
    void registerMemoryConfig() {
        // Memory systems configuration
        nlm::ConfigSchema memorySchema;
        
        // Working memory
        nlm::ConfigSchema workingMemSchema;
        workingMemSchema.addParameter("capacity", nlm::ConfigType::Integer, 100);
        workingMemSchema.addParameter("decay_rate", nlm::ConfigType::Float, 0.01f);
        memorySchema.addSubschema("working_memory", workingMemSchema);
        
        // Episodic memory
        nlm::ConfigSchema episodicSchema;
        episodicSchema.addParameter("max_episodes", nlm::ConfigType::Integer, 1000);
        episodicSchema.addParameter("replay_interval", nlm::ConfigType::Integer, 100);
        memorySchema.addSubschema("episodic_memory", episodicSchema);
        
        configRegistry_["memory"] = memorySchema;
    }
    
    void applyDefaultConfiguration(nlm::Config& config) {
        // Apply default values from registry
        for (const auto& [section, schema] : configRegistry_) {
            for (const auto& param : schema.getParameters()) {
                if (!config.hasValue(section, param.name)) {
                    config.set(section, param.name, param.defaultValue);
                }
            }
        }
    }
    
    // Other methods...
    
    std::unordered_map<std::string, nlm::ConfigSchema> configRegistry_;
};
```

### Custom Learning Algorithms

```cpp
// Custom learning algorithm implementation
class CustomLearningAlgorithm {
public:
    CustomLearningAlgorithm(nlm::Brain* brain) : brain_(brain) {}
    
    // Apply custom learning based on environment interaction
    void applyCustomLearning(size_t step, const nlm::Action& action) {
        // Monitor agent performance
        float performance = measurePerformance();
        
        // Apply reinforcement based on custom criteria
        if (performance < 0.3f && step > 100) {
            // Poor performance - increase exploration
            boostCuriosity(0.5f);
            enhanceNovelty(0.3f);
        } else if (performance > 0.8f) {
            // Good performance - strengthen successful pathways
            reinforceWinningConnections();
            boostRewardPrediction();
        }
        
        // Apply custom developmental plasticity
        applyCustomDevelopmentPlasticity(step);
    }
    
    // Custom plasticity based on experience
    void applyCustomDevelopmentPlasticity(size_t step) {
        // Developmental stage-specific plasticity
        auto stage = brain_->getDevelopmentalStage();
        
        switch (stage) {
            case nlm::DevelopmentalStage::Initial:
                // High plasticity for learning basic skills
                enhanceSynaptogenesis(0.001f);
                boostStructuralPlasticity(0.0001f);
                break;
                
            case nlm::DevelopmentalStage::CriticalPeriod:
                // Sensitive period for specific learning
                enableSensorySpecificPlasticity();
                enhanceRepetitionBasedLearning();
                break;
                
            case nlm::DevelopmentalStage::Maturation:
                // Refine and optimize
                enhanceFineMotorControl();
                boostCognitiveFlexibility();
                break;
                
            case nlm::DevelopmentalStage::Adult:
                // Maintain and adapt
                supportLongTermMemory();
                enableSkillRefinement();
                break;
        }
    }
    
private:
    // Helper methods for custom learning
    float measurePerformance() {
        // Custom performance metric based on task success
        // This could include reward history, action success rate, etc.
        return 0.5f;  // Placeholder
    }
    
    void boostCuriosity(float amount) {
        if (auto* curiosity = brain_->getCuriosity()) {
            float current = curiosity->getLevel();
            curiosity->setLevel(std::min(1.0f, current + amount));
        }
    }
    
    void enhanceNovelty(float amount) {
        if (auto* novelty = brain_->getNovelty()) {
            float current = novelty->getLevel();
            novelty->setLevel(std::min(1.0f, current + amount));
        }
    }
    
    void reinforceWinningConnections() {
        // Strengthen synapses that led to successful actions
        // This is a placeholder for your custom reinforcement logic
    }
    
    void boostRewardPrediction() {
        // Enhance reward prediction accuracy
        // This is a placeholder for your custom reward enhancement logic
    }
    
    void enhanceSynaptogenesis(float rate) {
        if (auto* structural = brain_->getStructuralPlasticity()) {
            structural->setSynaptogenesisRate(rate);
        }
    }
    
    void boostStructuralPlasticity(float rate) {
        if (auto* structural = brain_->getStructuralPlasticity()) {
            structural->setPruningRate(rate);
        }
    }
    
    // Other helper methods...
    
    nlm::Brain* brain_;
};
```

### Custom Performance Monitoring

```cpp
// Advanced performance monitoring
class CustomPerformanceMonitor {
public:
    CustomPerformanceMonitor(nlm::Brain* brain) : brain_(brain) {
        initializeMetrics();
    }
    
    void logPerformance(size_t step) {
        // Record comprehensive performance metrics
        PerformanceMetrics metrics;
        
        // Neural performance metrics
        metrics.totalSpikes = brain_->getTotalSpikeCount();
        metrics.averageFiringRate = brain_->getAverageFiringRate();
        metrics.eiRatio = brain_->getExcitationInhibitionRatio();
        
        // Memory system metrics
        if (auto* wm = brain_->getWorkingMemory()) {
            metrics.workingMemoryTraces = wm->getActiveTraces();
        }
        if (auto* em = brain_->getEpisodicMemory()) {
            metrics.episodicMemoryEpisodes = em->getEpisodeCount();
        }
        
        // Neuromodulation metrics
        if (auto* dopamine = brain_->getDopamine()) {
            metrics.dopamineLevel = dopamine->getLevel();
        }
        if (auto* curiosity = brain_->getCuriosity()) {
            metrics.curiosityLevel = curiosity->getLevel();
        }
        
        // Store metrics for analysis
        storeMetrics(step, metrics);
        
        // Generate alerts if needed
        generateAlerts(step, metrics);
        
        // Log summary
        logPerformanceSummary(step, metrics);
    }
    
    std::vector<PerformanceMetrics> getMetricsForAnalysis(size_t stepsBack = 1000) {
        // Return metrics for analysis (e.g., machine learning)
        std::vector<PerformanceMetrics> recentMetrics;
        
        // Filter and return recent metrics
        // Implement time-based filtering
        
        return recentMetrics;
    }
    
    float computeLearningCurve(size_t stepsBack = 1000) {
        // Compute learning curve from stored metrics
        auto metrics = getMetricsForAnalysis(stepsBack);
        
        if (metrics.size() < 10) {
            return 0.0f;
        }
        
        // Calculate learning rate, saturation point, etc.
        float initialPerformance = metrics.front().averageFiringRate;
        float finalPerformance = metrics.back().averageFiringRate;
        
        return (finalPerformance - initialPerformance) / metrics.size();
    }
    
private:
    struct PerformanceMetrics {
        size_t step;
        size_t totalSpikes;
        float averageFiringRate;
        float eiRatio;
        size_t workingMemoryTraces;
        size_t episodicMemoryEpisodes;
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    void initializeMetrics() {
        // Initialize performance monitoring
        samplingInterval_ = 10;  // Log every 10 steps
        metricsHistory_.reserve(10000);
    }
    
    void storeMetrics(size_t step, const PerformanceMetrics& metrics) {
        // Store metrics for later analysis
        metricsHistory_.push_back(metrics);
    }
    
    void generateAlerts(size_t step, const PerformanceMetrics& metrics) {
        // Generate performance alerts
        if (metrics.averageFiringRate > 50.0f) {
            // High firing rate - potential issue
            onHighFiringRate(step, metrics);
        }
        
        if (metrics.dopamineLevel < 0.01f) {
            // Low dopamine - lack of motivation
            onLowMotivation(step, metrics);
        }
        
        if (metrics.workingMemoryTraces < 5 && step > 100) {
            // Low working memory activity - possible issue
            onLowWorkingMemory(step, metrics);
        }
    }
    
    void onHighFiringRate(size_t step, const PerformanceMetrics& metrics) {
        std::cout << "[ALERT] Step " << step << ": High firing rate ("
                  << metrics.averageFiringRate << ")" << std::endl;
    }
    
    void onLowMotivation(size_t step, const PerformanceMetrics& metrics) {
        std::cout << "[ALERT] Step " << step << ": Low motivation (dopamine="
                  << metrics.dopamineLevel << ")" << std::endl;
    }
    
    void onLowWorkingMemory(size_t step, const PerformanceMetrics& metrics) {
        std::cout << "[ALERT] Step " << step << ": Low working memory ("
                  << metrics.workingMemoryTraces << " traces)" << std::endl;
    }
    
    void logPerformanceSummary(size_t step, const PerformanceMetrics& metrics) {
        // Log performance summary at regular intervals
        if (step % 1000 == 0) {
            std::cout << "=== Performance Summary (Step " << step << ") ===" << std::endl;
            std::cout << "Spikes: " << metrics.totalSpikes << std::endl;
            std::cout << "Avg Firing: " << metrics.averageFiringRate << std::endl;
            std::cout << "E/I Ratio: " << metrics.eiRatio << std::endl;
            std::cout << "Working Memory: " << metrics.workingMemoryTraces << std::endl;
            std::cout << "Episodic Memory: " << metrics.episodicMemoryEpisodes << std::endl;
            std::cout << "Dopamine: " << metrics.dopamineLevel << std::endl;
            std::cout << "Curiosity: " << metrics.curiosityLevel << std::endl;
            std::cout << "===========================" << std::endl;
        }
    }
    
    std::vector<PerformanceMetrics> metricsHistory_;
    size_t samplingInterval_;
    nlm::Brain* brain_;
};
```

### Advanced Command Line Interface

```cpp
// Extended command line interface
class AdvancedCLI {
public:
    AdvancedCLI(int argc, char* argv[]) : argc_(argc), argv_(argv) {
        parseArguments();
        applyArguments();
    }
    
    void parseArguments() {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                exit(0);
            } else if (arg == "--version") {
                showVersion();
                exit(0);
            } else if (arg == "--profile") {
                enableProfiling_ = true;
            } else if (arg == "--debug") {
                debugMode_ = true;
            } else if (arg == "--log-level") {
                if (i + 1 < argc) {
                    logLevel_ = parseLogLevel(argv[++i]);
                    ++i;
                }
            } else if (arg.find("--config=") == 0) {
                configFile_ = arg.substr(9);  // 8 is length of "--config="
            } else if (arg.find("--") == 0) {
                // Handle other configuration options
                processConfigOption(arg);
            }
        }
    }
    
    void processConfigOption(const std::string& arg) {
        // Process various configuration options
        if (arg.find("--neurons=") == 0) {
            neurons_ = std::stoull(arg.substr(10));  // 9 is length of "--neurons="
        } else if (arg.find("--seed=") == 0) {
            seed_ = std::stoull(arg.substr(8));  // 7 is length of "--seed="
        } else if (arg.find("--timestep=") == 0) {
            timestep_ = std::stod(arg.substr(11));  // 10 is length of "--timestep="
        } else if (arg.find("--plasticity=") == 0) {
            plasticityRate_ = std::stof(arg.substr(13));  // 12 is length of "--plasticity="
        }
        // Add more options as needed
    }
    
    void applyArguments() {
        // Apply parsed arguments to configuration
        if (seed_) {
            config_->set("random_seed", static_cast<int64_t>(*seed_));
        }
        
        if (neurons_) {
            config_->set("neuron_count", static_cast<int64_t>(*neurons_));
        }
        
        if (timestep_) {
            config_->set("simulation_timestep", *timestep_);
        }
        
        if (plasticityRate_) {
            config_->set("synaptogenesis_rate", *plasticityRate_);
        }
        
        // Apply other arguments as needed
    }
    
    void showHelp() const {
        std::cout << "=== Advanced NLM Command Line Options ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version                     Show version information" << std::endl;
        std::cout << "  --config=<file>              Load configuration from file" << std::endl;
        std::cout << std::endl;
        std::cout << "Brain Configuration:" << std::endl;
        std::cout << "  --neurons=<count>            Number of neurons (default: 500)" << std::endl;
        std::cout << "  --seed=<number>             Random seed (default: 42)" << std::endl;
        std::cout << "  --timestep=<seconds>        Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --plasticity=<rate>         Synaptogenesis rate (default: 0.0001)" << std::endl;
        std::cout << std::endl;
        std::cout << "Debug and Performance:" << std::endl;
        std::cout << "  --debug                      Enable debug mode" << std::endl;
        std::cout << "  --profile                    Enable performance profiling" << std::endl;
        std::cout << "  --log-level=<level>         Set log level (debug, info, warning, error)" << std::endl;
        std::cout << std::endl;
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --enable-curiosity           Enable curiosity-driven exploration" << std::endl;
        std::cout << "  --enable-development         Enable developmental plasticity" << std::endl;
        std::cout << "  --enable-replay              Enable memory replay" << std::endl;
        std::cout << "  --enable-checkpointing       Enable checkpointing" << std::endl;
        std::cout << "  --checkpoint-interval=<steps> Checkpoint interval (default: 1000)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm --neurons 1000 --seed 12345" << std::endl;
        std::cout << "  ./nlm --config my_config.cfg --timestep 0.0001 --debug" << std::endl;
        std::cout << "  ./nlm --neurons 2000 --enable-curiosity --enable-development" << std::endl;
    }
    
    void showVersion() const {
        std::cout << "NLM Phase 6 Advanced Edition" << std::endl;
        std::cout << "Version: 6.0.0" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    }
    
    LogLevel parseLogLevel(const std::string& levelStr) {
        if (levelStr == "debug") return LogLevel::Debug;
        if (levelStr == "info") return LogLevel::Info;
        if (levelStr == "warning") return LogLevel::Warning;
        if (levelStr == "error") return LogLevel::Error;
        return LogLevel::Info;  // Default
    }
    
private:
    int argc_;
    char** argv_;
    std::shared_ptr<nlm::Config> config_;
    bool enableProfiling_ = false;
    bool debugMode_ = false;
    LogLevel logLevel_ = LogLevel::Info;
    std::string configFile_;
    
    std::optional<size_t> seed_;
    std::optional<size_t> neurons_;
    std::optional<double> timestep_;
    std::optional<float> plasticityRate_;
    
    std::unordered_map<std::string, std::string> customConfig_;
};
```

### Custom Plugin Architecture

```cpp
// Plugin system for extending NLM functionality
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    // Called when plugin is initialized
    virtual void initialize(nlm::Brain* brain, const nlm::Config& config) = 0;
    
    // Called each simulation step
    virtual void update(nlm::Brain* brain, size_t step, double dt) = 0;
    
    // Called when simulation ends
    virtual void shutdown() = 0;
    
    // Get plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Plugin-specific configuration
    virtual void configure(const std::unordered_map<std::string, std::string>& settings) = 0;
};

// Plugin registry for managing plugins
class PluginRegistry {
public:
    PluginRegistry() = default;
    
    // Load a plugin from a shared library
    std::unique_ptr<NLMPlugin> loadPlugin(const std::string& libraryPath);
    
    // Register a built-in plugin
    void registerPlugin(std::unique_ptr<NLMPlugin> plugin);
    
    // Initialize all registered plugins
    void initializePlugins(nlm::Brain* brain, const nlm::Config& config);
    
    // Update all plugins
    void updatePlugins(nlm::Brain* brain, size_t step, double dt);
    
    // Shutdown all plugins
    void shutdownPlugins();
    
    // Get list of available plugins
    std::vector<std::string> getAvailablePlugins() const;
    
    // Configure a specific plugin
    void configurePlugin(const std::string& name, 
                        const std::unordered_map<std::string, std::string>& settings);
    
private:
    std::vector<std::unique_ptr<NLMPlugin>> plugins_;
    std::unordered_map<std::string, std::unique_ptr<NLMPlugin>> pluginMap_;
};

// Example plugin: Custom Learning Plugin
class CustomLearningPlugin : public NLMPlugin {
public:
    CustomLearningPlugin() = default;
    
    void initialize(nlm::Brain* brain, const nlm::Config& config) override {
        brain_ = brain;
        
        // Initialize custom learning parameters
        learningRate_ = config.getOr<float>("custom_learning_rate", 0.01f);
        explorationRate_ = config.getOr<float>("custom_exploration_rate", 0.3f);
        adaptationSpeed_ = config.getOr<float>("custom_adaptation_speed", 0.05f);
        
        // Create learning state
        learningState_.reset(new LearningState);
        
        std::cout << "[Plugin] CustomLearningPlugin initialized" << std::endl;
    }
    
    void update(nlm::Brain* brain, size_t step, double dt) override {
        if (!brain_) return;
        
        // Apply custom learning rules
        applyCustomLearning(step, dt);
        
        // Update learning state
        updateLearningState(step, dt);
        
        // Generate learning recommendations
        generateLearningRecommendations(step);
    }
    
    void shutdown() override {
        brain_ = nullptr;
        learningState_.reset();
        std::cout << "[Plugin] CustomLearningPlugin shutdown" << std::endl;
    }
    
    std::string getName() const override {
        return "CustomLearningPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Implements custom learning algorithms based on neural activity patterns";
    }
    
    void configure(const std::unordered_map<std::string, std::string>& settings) override {
        for (const auto& [key, value] : settings) {
            if (key == "learning_rate") {
                learningRate_ = std::stof(value);
            } else if (key == "exploration_rate") {
                explorationRate_ = std::stof(value);
            } else if (key == "adaptation_speed") {
                adaptationSpeed_ = std::stof(value);
            }
        }
    }
    
private:
    struct LearningState {
        float accumulatedExperience = 0.0f;
        float skillLevel = 0.0f;
        float adaptationProgress = 0.0f;
        size_t successfulActions = 0;
        size_t totalActions = 0;
        std::unordered_map<size_t, float> actionValues;
    };
    
    void applyCustomLearning(size_t step, double dt) {
        // Custom learning based on neural activity
        if (auto* dopamine = brain_->getDopamine()) {
            float rewardPrediction = dopamine->getLevel();
            
            // Custom reward-modulated learning
            if (rewardPrediction > 0.7f) {
                // High reward - strengthen successful pathways
                strengthenSuccessfulPathways();
            } else if (rewardPrediction < 0.3f) {
                // Low reward - explore new pathways
                increaseExploration();
            }
            
            // Update learning state based on reward
            learningState_->accumulatedExperience += rewardPrediction;
            learningState_->skillLevel = std::min(1.0f, 
                learningState_->skillLevel + adaptationSpeed_ * rewardPrediction);
        }
    }
    
    void updateLearningState(size_t step, double dt) {
        // Update learning state based on neural activity
        if (learningState_) {
            // Calculate success rate
            if (learningState_->totalActions > 0) {
                float successRate = static_cast<float>(learningState_->successfulActions) /
                                   learningState_->totalActions;
                learningState_->adaptationProgress = successRate;
            }
            
            // Decay unused knowledge
            for (auto& [action, value] : learningState_->actionValues) {
                value *= (1.0f - 0.01f * dt);  // Decay factor
            }
        }
    }
    
    void generateLearningRecommendations(size_t step) {
        // Generate learning recommendations based on current state
        if (learningState_ && learningState_->adaptationProgress < 0.5f) {
            // Low adaptation - recommend exploration
            std::cout << "[Plugin Recommendation] Step " << step 
                     << ": Increase exploration to improve learning" << std::endl;
        }
        
        if (learningState_->skillLevel > 0.8f) {
            // High skill - recommend refinement
            std::cout << "[Plugin Recommendation] Step " << step 
                     << ": Focus on refining existing skills" << std::endl;
        }
    }
    
    void strengthenSuccessfulPathways() {
        // Strengthen synapses that led to success
        // This is implementation-specific
    }
    
    void increaseExploration() {
        // Increase exploration through curiosity and novelty
        if (auto* curiosity = brain_->getCuriosity()) {
            float current = curiosity->getLevel();
            curiosity->setLevel(std::min(1.0f, current + 0.1f));
        }
    }
    
    nlm::Brain* brain_ = nullptr;
    float learningRate_ = 0.01f;
    float explorationRate_ = 0.3f;
    float adaptationSpeed_ = 0.05f;
    std::unique_ptr<LearningState> learningState_;
};
```

### Advanced Testing Framework

```cpp
// Advanced testing framework for NLM extensions
class AdvancedTestFramework {
public:
    AdvancedTestFramework() {
        initializeTestFramework();
    }
    
    // Run all tests
    bool runAllTests() {
        bool allPassed = true;
        
        // Run core tests
        allPassed &= runCoreTests();
        
        // Run extension tests
        allPassed &= runExtensionTests();
        
        // Run performance tests
        allPassed &= runPerformanceTests();
        
        // Run integration tests
        allPassed &= runIntegrationTests();
        
        return allPassed;
    }
    
    // Test custom brain extensions
    bool testCustomBrainExtensions() {
        bool allPassed = true;
        
        // Test custom region types
        allPassed &= testCustomRegionTypes();
        
        // Test custom population types
        allPassed &= testCustomPopulationTypes();
        
        // Test custom connectivity patterns
        allPassed &= testCustomConnectivity();
        
        return allPassed;
    }
    
    // Test custom learning algorithms
    bool testCustomLearningAlgorithms() {
        bool allPassed = true;
        
        // Test custom STDP variants
        allPassed &= testCustomSTDP();
        
        // Test custom Hebbian learning
        allPassed &= testCustomHebbian();
        
        // Test custom neuromodulation
        allPassed &= testCustomNeuromodulation();
        
        return allPassed;
    }
    
    // Performance regression tests
    bool runPerformanceTests() {
        bool allPassed = true;
        
        // Test performance with increasing complexity
        allPassed &= testPerformanceWithComplexity(100, 1000);
        allPassed &= testPerformanceWithComplexity(1000, 10000);
        
        // Test memory usage
        allPassed &= testMemoryUsage();
        
        // Test CPU efficiency
        allPassed &= testCPUEfficiency();
        
        return allPassed;
    }
    
private:
    struct PerformanceBenchmark {
        size_t neuronCount;
        size_t steps;
        double wallClockTime;
        double cpuTime;
        size_t memoryUsage;
        float avgFiringRate;
    };
    
    void initializeTestFramework() {
        // Initialize test framework
        testResults_.clear();
        performanceBenchmarks_.clear();
    }
    
    bool testCustomRegionTypes() {
        // Test custom region implementations
        try {
            // Create custom regions
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", 500);
            config->set("region_count", 3);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Test custom region behavior
            auto regions = brain.getRegions();
            bool passed = true;
            
            for (const auto& region : regions) {
                passed &= testRegionBehavior(region.get());
            }
            
            return passed;
        } catch (const std::exception& e) {
            std::cerr << "Custom region test failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool testRegionBehavior(const nlm::NeuralRegion* region) {
        // Test specific region behavior
        if (!region) return false;
        
        // Test region-specific properties
        bool passed = true;
        
        // Test region connectivity
        passed &= testRegionConnectivity(region);
        
        // Test region dynamics
        passed &= testRegionDynamics(region);
        
        return passed;
    }
    
    bool testRegionConnectivity(const nlm::NeuralRegion* region) {
        // Test region-specific connectivity patterns
        // Implementation depends on your custom connectivity
        return true;  // Placeholder
    }
    
    bool testRegionDynamics(const nlm::NeuralRegion* region) {
        // Test region-specific dynamics
        // Implementation depends on your custom dynamics
        return true;  // Placeholder
    }
    
    bool testPerformanceWithComplexity(size_t neurons, size_t steps) {
        // Benchmark performance with different complexities
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", neurons);
        config->set("region_count", neurons / 100);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        // Store benchmark results
        PerformanceBenchmark benchmark;
        benchmark.neuronCount = neurons;
        benchmark.steps = steps;
        benchmark.wallClockTime = duration.count();
        benchmark.cpuTime = duration.count();  // Simplified
        benchmark.memoryUsage = 0;  // Simplified
        benchmark.avgFiringRate = brain.getAverageFiringRate();
        
        performanceBenchmarks_.push_back(benchmark);
        
        // Performance criteria (example)
        double targetTimePerStep = 0.001;  // 1ms per step
        double actualTimePerStep = duration.count() / steps;
        
        if (actualTimePerStep > targetTimePerStep * 10) {  // Allow 10x tolerance
            std::cout << "Performance warning: " << neurons << " neurons, "
                      << steps << " steps: " << actualTimePerStep << "s per step" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testMemoryUsage() {
        // Test memory usage efficiency
        // Implementation would involve memory profiling
        return true;  // Placeholder
    }
    
    bool testCPUEfficiency() {
        // Test CPU efficiency
        // Implementation would involve CPU profiling
        return true;  // Placeholder
    }
    
    std::vector<PerformanceBenchmark> performanceBenchmarks_;
    std::vector<TestResult> testResults_;
};
```

### Custom Documentation Generation

```cpp
// Custom documentation generator for NLM extensions
class CustomDocumentationGenerator {
public:
    CustomDocumentationGenerator() {
        setupDocumentationStructure();
    }
    
    void generateAPI Documentation(const std::string& outputDir) {
        // Generate API documentation
        generateAPIDocs(outputDir + "/api");
        
        // Generate user guide
        generateUserGuide(outputDir + "/user_guide");
        
        // Generate developer guide
        generateDeveloperGuide(outputDir + "/developer_guide");
        
        // Generate examples
        generateExamples(outputDir + "/examples");
    }
    
    void generateAPIDocs(const std::string& outputDir) {
        // Parse source headers and generate API documentation
        std::vector<std::string> sourceFiles = collectSourceFiles();
        
        for (const auto& file : sourceFiles) {
            parseSourceFile(file);
        }
        
        // Generate documentation
        writeAPIDocumentation(outputDir);
    }
    
    void generateUserGuide(const std::string& outputDir) {
        // Generate comprehensive user guide
        createUserGuideStructure(outputDir);
        
        // Add tutorials
        generateTutorials(outputDir);
        
        // Add API reference
        generateAPIReference(outputDir);
    }
    
    void generateDeveloperGuide(const std::string& outputDir) {
        // Generate developer extension guide
        createDeveloperGuideStructure(outputDir);
        
        // Add plugin development guide
        generatePluginGuide(outputDir);
        
        // Add performance optimization guide
        generatePerformanceGuide(outputDir);
    }
    
private:
    std::vector<std::string> collectSourceFiles() {
        // Collect all source files for documentation
        std::vector<std::string> files;
        
        // Example: Collect from src directory
        // This would use filesystem operations
        
        return files;
    }
    
    void parseSourceFile(const std::string& filePath) {
        // Parse source file and extract documentation
        // This would involve reading the file and extracting comments
        // and using a documentation generator like Doxygen or custom parser
    }
    
    void writeAPIDocumentation(const std::string& outputDir) {
        // Write API documentation to files
        // Generate HTML, PDF, or other formats
    }
    
    void createUserGuideStructure(const std::string& outputDir) {
        // Create directory structure for user guide
        std::filesystem::create_directories(outputDir);
        
        // Create index file
        // Create chapter files
    }
    
    void generateTutorials(const std::string& outputDir) {
        // Generate step-by-step tutorials
        // Create tutorial files for common tasks
    }
    
    void generateAPIReference(const std::string& outputDir) {
        // Generate API reference documentation
        // Create reference for all classes, functions, etc.
    }
    
    // Other methods for generating different documentation types...
};
```

This advanced user guide provides comprehensive guidance for extending and customizing the NLM system. It covers everything from basic extensions to advanced plugin architecture and performance optimization. The guide is designed to help you create powerful, customized neural simulation environments that leverage the full capabilities of NLM Phase 6.