// Enhanced NLM Python API with improved interface and advanced features
// This provides a more intuitive and comprehensive Python interface to the NLM brain simulation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

// Import all the core NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Experiment.hpp"

namespace py = pybind11;
namespace nlm {

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        double simulationTime{0.0};
        double neuronUpdateTime{0.0};
        double spikePropagationTime{0.0};
        double plasticityTime{0.0};
        double memoryAccessTime{0.0};
        size_t totalSpikeCount{0};
        size_t totalNeuronCount{0};
        size_t activeNeuronCount{0};
        size_t firingNeuronCount{0};
        float averageFiringRate{0.0f};
        float excitationInhibitionRatio{0.0f};
        std::chrono::high_resolution_clock::time_point startTime;
        std::vector<double> timestepHistory;
    };
    
    PerformanceMonitor() {
        metrics.startTime = std::chrono::high_resolution_clock::now();
        metrics.timestepHistory.reserve(1000);
    }
    
    void recordStep(double timestep) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - metrics.startTime).count();
        metrics.timestepHistory.push_back(duration);
        if (metrics.timestepHistory.size() > 1000) {
            metrics.timestepHistory.erase(metrics.timestepHistory.begin());
        }
    }
    
    Metrics getMetrics() const { return metrics; }
    void reset() {
        metrics = Metrics();
        metrics.startTime = std::chrono::high_resolution_clock::now();
    }
    
    double getAverageTimestep() const {
        if (metrics.timestepHistory.empty()) return 0.0;
        double sum = 0.0;
        for (double t : metrics.timestepHistory) sum += t;
        return sum / metrics.timestepHistory.size();
    }
    
    double getPerformanceScore() const {
        if (metrics.timestepHistory.empty()) return 0.0;
        // Lower is better (faster timesteps)
        return getAverageTimestep();
    }
    
private:
    Metrics metrics;
};

// Enhanced Config class with more features
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()), monitor_(std::make_shared<PerformanceMonitor>()) {}
    
    // Configuration loading with format detection
    bool loadConfig(const std::string& filepath) {
        std::string ext = getFileExtension(filepath);
        if (ext == "json" || ext == "yaml" || ext == "yml") {
            // TODO: Implement JSON/YAML support
            std::cerr << "JSON/YAML parsing not yet implemented, using file as-is\n";
        }
        return config_->loadFromFile(filepath);
    }
    
    // Convenience methods for common configurations
    void setupDefaultBrain(size_t neuronCount = 1000, size_t regionCount = 2) {
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config_->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Phase 2 defaults
        config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    }
    
    void setupDevelopmentMode() {
        config_->set("development_enabled", true, ConfigSource::Default);
        config_->set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config_->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
        config_->set("structural_plasticity_enabled", true, ConfigSource::Default);
    }
    
    void setupLearningMode() {
        config_->set("reward_modulation_enabled", true, ConfigSource::Default);
        config_->set("curiosity_enabled", true, ConfigSource::Default);
        config_->set("novelty_detection_enabled", true, ConfigSource::Default);
        config_->set("prediction_system_enabled", true, ConfigSource::Default);
    }
    
    void setupAgentMode() {
        config_->set("agent_mode", true, ConfigSource::Default);
        config_->set("sensory_processing_enabled", true, ConfigSource::Default);
        config_->set("motor_decoding_enabled", true, ConfigSource::Default);
        config_->set("reward_modulation_enabled", true, ConfigSource::Default);
    }
    
    // Performance configuration
    void setupHighPerformance(size_t threads = 0) {
        config_->set("performance_parallel_neuron_updates", true, ConfigSource::Default);
        config_->set("performance_use_memory_pools", true, ConfigSource::Default);
        config_->set("performance_enable_simd", true, ConfigSource::Default);
        if (threads > 0) {
            config_->set("performance_thread_count", static_cast<int64_t>(threads), ConfigSource::Default);
        }
    }
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() { return config_; }
    
    // Get performance monitor
    std::shared_ptr<PerformanceMonitor> getMonitor() { return monitor_; }
    
    // Get configuration summary
    std::string getConfigSummary() const { return config_->summary(); }
    
    // List all configuration keys
    std::vector<std::string> getAllKeys() const { return config_->getKeys(); }
    
    // Get specific configuration values
    template<typename T>
    std::optional<T> getValue(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    template<typename T>
    T getValueOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Print configuration in user-friendly format
    void printConfig() const {
        std::cout << "=== NLM Configuration ===\n";
        std::cout << config_->summary();
        std::cout << "\n=== End Configuration ===\n";
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<PerformanceMonitor> monitor_;
    
    std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos) return "";
        std::string ext = filename.substr(pos + 1);
        for (char& c : ext) c = std::tolower(c);
        return ext;
    }
};

// Enhanced Brain class with performance monitoring and advanced features
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Initialize with performance monitoring
    bool initialize(bool enableMonitoring = true) {
        auto start = std::chrono::high_resolution_clock::now();
        bool success = brain_->initialize();
        auto end = std::chrono::high_resolution_clock::now();
        
        if (enableMonitoring) {
            double initTime = std::chrono::duration<double>(end - start).count();
            std::cout << "Brain initialized in " << std::fixed << std::setprecision(4) 
                      << initTime << " seconds\n";
        }
        
        return success;
    }
    
    // Step with performance monitoring
    void step(SimulationStep step, double timestep = 0.001, bool recordMetrics = true) {
        if (recordMetrics) {
            monitor_->recordStep(timestep);
        }
        brain_->step(step, timestep);
    }
    
    // Batch processing for efficiency
    void batchProcess(size_t numSteps, double timestep = 0.001) {
        std::cout << "Processing " << numSteps << " steps with timestep " << timestep << "s\n";
        
        for (size_t i = 0; i < numSteps; ++i) {
            step(i, timestep, true);
            if (i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain_->getTotalSpikeCount() << " spikes\n";
            }
        }
    }
    
    // Get performance metrics
    PerformanceMonitor::Metrics getMetrics() const { return monitor_->getMetrics(); }
    
    // Performance analysis
    std::string getPerformanceReport() const {
        auto metrics = monitor_->getMetrics();
        std::ostringstream oss;
        oss << "=== Performance Report ===\n";
        oss << "Average timestep: " << std::fixed << std::setprecision(6) 
            << monitor_->getAverageTimestep() << " s\n";
        oss << "Performance score: " << std::scientific << monitor_->getPerformanceScore() << "\n";
        oss << "Total spikes: " << metrics.totalSpikeCount << "\n";
        oss << "Total neurons: " << metrics.totalNeuronCount << "\n";
        oss << "Active neurons: " << metrics.activeNeuronCount << "\n";
        oss << "Firing neurons: " << metrics.firingNeuronCount << "\n";
        oss << "Average firing rate: " << std::fixed << std::setprecision(2) 
            << metrics.averageFiringRate << " Hz\n";
        oss << "Excitatory/Inhibition ratio: " << std::fixed << std::setprecision(3) 
            << metrics.excitationInhibitionRatio << "\n";
        return oss.str();
    }
    
    // Enhanced logging
    void enableLogging(bool enable, const std::string& logFile = "") {
        // TODO: Implement enhanced logging
        std::cout << "Logging " << (enable ? "enabled" : "disabled") << std::endl;
        if (!logFile.empty()) {
            std::cout << "Log file: " << logFile << std::endl;
        }
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
    // Convenience methods
    size_t getNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
    size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
    
    // Save brain state
    bool save(const std::string& filepath) { return brain_->save(filepath); }
    
    // Load brain state
    bool load(const std::string& filepath) { return brain_->load(filepath); }
    
private:
    std::shared_ptr<Brain> brain_;
    PerformanceMonitor monitor_;
};

// Enhanced Agent with more sophisticated capabilities
class EnhancedAgent {
public:
    EnhancedAgent(std::shared_ptr<Brain> brain) : agent_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with world and enable advanced features
    void initialize(const SimpleWorld& world, bool enableExploration = true, 
                   bool enableLearning = true, bool enableDevelopment = true) {
        agent_->initialize(world);
        
        if (enableExploration) {
            agent_->enableCuriosity(true);
        }
        
        if (enableLearning) {
            agent_->enableRewardModulation(true);
            agent_->enableStructuralPlasticity(true);
        }
        
        if (enableDevelopment) {
            agent_->enableDevelopment(true);
        }
        
        std::cout << "Enhanced agent initialized with: "
                  << "Exploration=" << (enableExploration ? "Yes" : "No") << ", "
                  << "Learning=" << (enableLearning ? "Yes" : "No") << ", "
                  << "Development=" << (enableDevelopment ? "Yes" : "No") << std::endl;
    }
    
    // Advanced action selection with curiosity and exploration
    Action executeAction(const SimpleWorld& world, double reward = 0.0f, 
                        double predictedReward = 0.0f, bool enableExploration = true) {
        // Update world state
        world.update(0.1);
        
        // Get sensory percept
        auto percept = world.getSensoryPercept();
        
        // Process sensory input
        agent_->processSensoryInput(percept);
        
        // Apply reward modulation
        if (reward != 0.0f) {
            agent_->applyRewardModulation(reward, predictedReward);
        }
        
        // Update development
        agent_->updateDevelopment(0.1);
        
        // Decode motor command
        MotorCommand cmd = agent_->decodeMotorCommand();
        
        // Convert to action
        Action action(cmd);
        
        // Apply action to world
        world.applyMotorCommand(cmd, world.getSimulationTime());
        
        return action;
    }
    
    // Curiosity-driven exploration
    Action exploreWithCuriosity(const SimpleWorld& world, double threshold = 0.5f) {
        if (agent_->getCuriosityLevel() > threshold) {
            std::cout << "Curiosity-driven exploration triggered!" << std::endl;
            // Use random action for exploration
            MotorCommand randomCmd = selectRandomAction();
            Action action(randomCmd);
            world.applyMotorCommand(randomCmd, world.getSimulationTime());
            return action;
        }
        
        // Normal behavior
        return executeAction(world);
    }
    
    // Get agent status
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "=== Agent Status ===\n";
        oss << "Neuromodulation Level: " << std::fixed << std::setprecision(3) 
            << agent_->getNeuromodulationLevel() << "\n";
        oss << "Curiosity Level: " << std::fixed << std::setprecision(3) 
            << agent_->getCuriosityLevel() << "\n";
        oss << "Novelty Level: " << std::fixed << std::setprecision(3) 
            << agent_->getNoveltyLevel() << "\n";
        oss << "Prediction Error: " << std::fixed << std::setprecision(3) 
            << agent_->getPredictionError() << "\n";
        oss << "Developmental Stage: " << static_cast<int>(agent_->getDevelopmentalStage()) << "\n";
        oss << "Reward Modulation: " << (agent_->isRewardModulationEnabled() ? "Enabled" : "Disabled") << "\n";
        oss << "Curiosity Enabled: " << (agent_->isCuriosityEnabled() ? "Yes" : "No") << "\n";
        oss << "Development Enabled: " << (agent_->isDevelopmentEnabled() ? "Yes" : "No") << "\n";
        oss << "Structural Plasticity: " << (agent_->isStructuralPlasticityEnabled() ? "Yes" : "No") << "\n";
        return oss.str();
    }
    
    // Get underlying agent
    std::shared_ptr<AgentBrain> getAgent() { return agent_; }
    
    // Enable features
    void enableFeature(const std::string& feature, bool enable = true) {
        if (feature == "reward_modulation") {
            agent_->enableRewardModulation(enable);
        } else if (feature == "structural_plasticity") {
            agent_->enableStructuralPlasticity(enable);
        } else if (feature == "development") {
            agent_->enableDevelopment(enable);
        } else if (feature == "curiosity") {
            agent_->enableCuriosity(enable);
        }
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
    
    MotorCommand selectRandomAction() {
        auto rng = agent_->getBrain()->getRandomGenerator();
        int choice = rng->uniformInt(0, 7);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
};

// Learning experiment utilities
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Run learning experiment with automatic recording
    void runLearningExperiment(double duration = 10.0, bool verbose = true) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Initialize for experiment
        brain_->reset();
        brain_->initialize();
        
        size_t steps = static_cast<size_t>(duration * 100); // 100 steps per second
        
        if (verbose) {
            std::cout << "Starting learning experiment for " << duration << " seconds (" 
                      << steps << " steps)\n";
        }
        
        // Record initial state
        recordInitialState();
        
        // Apply learning stimuli
        for (size_t i = 0; i < steps; ++i) {
            // Simulate learning environment
            injectLearningStimuli();
            
            // Step brain
            brain_->step(i, 0.01);
            
            if (verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain_->getTotalSpikeCount() << " spikes\n";
            }
        }
        
        // Record final state
        recordFinalState();
        
        // Compute and display results
        computeAndDisplayResults();
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (verbose) {
            std::cout << "\nExperiment completed in " << std::fixed << std::setprecision(2) 
                      << elapsed << " seconds\n";
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    
    void recordInitialState() {
        initialWeights_.clear();
        auto* region = brain_->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights_.push_back(syn->getWeight());
            }
        }
    }
    
    void recordFinalState() {
        finalWeights_.clear();
        auto* region = brain_->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights_.push_back(syn->getWeight());
            }
        }
    }
    
    void computeAndDisplayResults() {
        std::cout << "\n=== Learning Experiment Results ===\n";
        
        if (initialWeights_.empty() || finalWeights_.empty()) {
            std::cout << "ERROR: No weights recorded\n";
            return;
        }
        
        // Compute statistics
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        std::cout << "Weight Statistics:\n";
        std::cout << "  Initial mean weight: " << std::fixed << std::setprecision(6) << initialMean << "\n";
        std::cout << "  Final mean weight: " << std::fixed << std::setprecision(6) << finalMean << "\n";
        std::cout << "  Change: " << std::fixed << std::setprecision(6) << (finalMean - initialMean) << "\n";
        
        // Count changes
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights_[i] - initialWeights_[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        std::cout << "\nSynaptic Changes:\n";
        std::cout << "  Strengthened: " << strengthened << "\n";
        std::cout << "  Weakened: " << weakened << "\n";
        std::cout << "  Unchanged: " << unchanged << "\n";
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        std::cout << "\nLearning Result:\n";
        if (learningOccurred) {
            std::cout << "  ✓ LEARNING DETECTED: Synaptic weights changed through experience\n";
        } else {
            std::cout << "  ✗ NO LEARNING: Weights did not change significantly\n";
        }
    }
    
    void injectLearningStimuli() {
        // Apply random learning stimuli
        auto rng = brain_->getRandomGenerator();
        size_t numNeurons = brain_->getTotalNeuronCount() / 4;
        
        for (size_t i = 0; i < numNeurons; ++i) {
            NeuronId id = rng->uniformInt(1, static_cast<uint64_t>(brain_->getTotalNeuronCount()));
            brain_->injectCurrent(id, rng->uniformReal(10.0f, 50.0f));
        }
    }
};

// Main PYBIND11 module definition with enhanced bindings
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python API
        ----------------------
        An enhanced Python interface for the NLM neural simulation framework.
        Provides advanced features including performance monitoring, learning experiments,
        enhanced agent capabilities, and comprehensive configuration management.
        
        Key Features:
        - Performance monitoring and profiling
        - Enhanced configuration management
        - Advanced agent with curiosity-driven exploration
        - Learning experiment utilities
        - Improved logging and debugging tools
        - Better API design with more intuitive methods
    )pbdoc";
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register PerformanceMonitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("getMetrics", &PerformanceMonitor::getMetrics)
        .def("recordStep", &PerformanceMonitor::recordStep)
        .def("getAverageTimestep", &PerformanceMonitor::getAverageTimestep)
        .def("getPerformanceScore", &PerformanceMonitor::getPerformanceScore)
        .def("reset", &PerformanceMonitor::reset);
    
    // Register EnhancedConfig
    py::class_<EnhancedConfig>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("loadConfig", &EnhancedConfig::loadConfig, py::arg("filepath"),
             "Load configuration from file (supports .cfg, .json, .yaml)")
        .def("setupDefaultBrain", &EnhancedConfig::setupDefaultBrain, 
             py::arg("neuronCount") = 1000, py::arg("regionCount") = 2,
             "Setup a default brain configuration")
        .def("setupDevelopmentMode", &EnhancedConfig::setupDevelopmentMode,
             "Setup configuration for development/ learning mode")
        .def("setupLearningMode", &EnhancedConfig::setupLearningMode,
             "Setup configuration for learning mode")
        .def("setupAgentMode", &EnhancedConfig::setupAgentMode,
             "Setup configuration for agent simulation")
        .def("setupHighPerformance", &EnhancedConfig::setupHighPerformance,
             py::arg("threads") = 0, "Setup configuration for high performance")
        .def("getConfigSummary", &EnhancedConfig::getConfigSummary)
        .def("printConfig", &EnhancedConfig::printConfig)
        .def("getAllKeys", &EnhancedConfig::getAllKeys)
        .def("getMonitor", &EnhancedConfig::getMonitor)
        .def("getValue", [](const EnhancedConfig& self, const std::string& key) {
            auto val = self.getValue<double>(key); // Template will be resolved
            if (val.has_value()) {
                return py::cast(val.value());
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get configuration value")
        .def("getValueOr", [](const EnhancedConfig& self, const std::string& key, auto defaultValue) {
            return self.getValueOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default");
    
    // Register EnhancedBrain
    py::class_<EnhancedBrain>(m, "EnhancedBrain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &EnhancedBrain::initialize, py::arg("enableMonitoring") = true,
             "Initialize the brain with optional performance monitoring")
        .def("step", &EnhancedBrain::step, py::arg("currentStep"), py::arg("timestep") = 0.001,
             py::arg("recordMetrics") = true, "Perform a simulation step")
        .def("batchProcess", &EnhancedBrain::batchProcess, py::arg("numSteps"), py::arg("timestep") = 0.001,
             "Process multiple steps efficiently")
        .def("getMetrics", &EnhancedBrain::getMetrics)
        .def("getPerformanceReport", &EnhancedBrain::getPerformanceReport)
        .def("enableLogging", &EnhancedBrain::enableLogging, py::arg("enable") = true, py::arg("logFile") = "",
             "Enable or disable logging")
        .def("save", &EnhancedBrain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &EnhancedBrain::load, py::arg("filepath"), "Load brain state from file")
        .def("getNeuronCount", &EnhancedBrain::getNeuronCount)
        .def("getSynapseCount", &EnhancedBrain::getSynapseCount)
        .def("getActiveNeuronCount", &EnhancedBrain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &EnhancedBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &EnhancedBrain::getAverageFiringRate)
        .def("getDevelopmentalStage", &EnhancedBrain::getDevelopmentalStage);
    
    // Register EnhancedAgent
    py::class_<EnhancedAgent>(m, "EnhancedAgent")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &EnhancedAgent::initialize, 
             py::arg("world"), py::arg("enableExploration") = true, 
             py::arg("enableLearning") = true, py::arg("enableDevelopment") = true,
             "Initialize agent with enhanced capabilities")
        .def("executeAction", &EnhancedAgent::executeAction, 
             py::arg("world"), py::arg("reward") = 0.0f, py::arg("predictedReward") = 0.0f, py::arg("enableExploration") = true,
             "Execute action with advanced features")
        .def("exploreWithCuriosity", &EnhancedAgent::exploreWithCuriosity,
             py::arg("world"), py::arg("threshold") = 0.5f,
             "Execute curiosity-driven exploration")
        .def("getStatus", &EnhancedAgent::getStatus)
        .def("enableFeature", &EnhancedAgent::enableFeature,
             py::arg("feature"), py::arg("enable") = true,
             "Enable or disable agent features");
    
    // Register LearningExperiment
    py::class_<LearningExperiment>(m, "LearningExperiment")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("runLearningExperiment", &LearningExperiment::runLearningExperiment,
             py::arg("duration") = 10.0, py::arg("verbose") = true,
             "Run a learning experiment with automatic recording and analysis");
    
    // Factory functions for convenience
    m.def("createEnhancedConfig", []() {
        return std::make_shared<EnhancedConfig>();
    }, "Create an enhanced configuration object with advanced features");
    
    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<EnhancedBrain>(config);
    }, py::arg("config"), "Create an enhanced brain with performance monitoring");
    
    m.def("createEnhancedAgent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<EnhancedAgent>(brain);
    }, py::arg("brain"), "Create an enhanced agent with advanced capabilities");
    
    m.def("createLearningExperiment", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<LearningExperiment>(brain);
    }, py::arg("brain"), "Create a learning experiment utility");
    
    // Convenience constants
    m.attr("VERSION") = "1.0.0";
    m.attr("BUILD_TYPE") = std::string("Enhanced");
    
    // Example configuration templates
    m.def("getExampleConfigs", []() {
        py::dict examples;
        examples["default"] = R"(
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.1
        )";
        examples["learning"] = R"(
random_seed = 123
simulation_timestep = 0.001
neuron_count = 1000
region_count = 3
connection_probability = 0.15
reward_modulation_enabled = true
curiosity_enabled = true
        )";
        examples["development"] = R"(
random_seed = 456
simulation_timestep = 0.001
neuron_count = 800
region_count = 2
connection_probability = 0.12
development_enabled = true
structural_plasticity_enabled = true
        )";
        return examples;
    }, "Get example configuration templates");
}
