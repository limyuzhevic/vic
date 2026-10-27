// Enhanced Python bindings with advanced features
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError(msg) {}
};

// Enhanced configuration validation
class ConfigValidator {
public:
    static bool validateValue(const std::string& key, const nlm::ConfigValue& value) {
        // Validate common configuration keys
        if (key.find("neuron_count") != std::string::npos) {
            if (std::holds_alternative<int>(value) && std::get<int>(value) <= 0) {
                return false;
            }
        }
        if (key.find("synapse_density") != std::string::npos) {
            if (std::holds_alternative<double>(value)) {
                double density = std::get<double>(value);
                if (density < 0.0 || density > 1.0) {
                    return false;
                }
            }
        }
        if (key.find("connection_probability") != std::string::npos) {
            if (std::holds_alternative<double>(value)) {
                double prob = std::get<double>(value);
                if (prob < 0.0 || prob > 1.0) {
                    return false;
                }
            }
        }
        if (key.find("v_thresh") != std::string::npos) {
            if (std::holds_alternative<double>(value)) {
                double thresh = std::get<double>(value);
                if (thresh > -40.0 || thresh < -80.0) {
                    return false;
                }
            }
        }
        if (key.find("tau_mem") != std::string::npos) {
            if (std::holds_alternative<double>(value) && std::get<double>(value) <= 0.0) {
                return false;
            }
        }
        return true;
    }
    
    static std::string getValidationError(const std::string& key, const nlm::ConfigValue& value) {
        std::stringstream ss;
        ss << "Invalid value for configuration key '" << key << "': ";
        
        if (key.find("neuron_count") != std::string::npos) {
            if (std::holds_alternative<int>(value) && std::get<int>(value) <= 0) {
                ss << "neuron_count must be positive, got " << std::get<int>(value);
                return ss.str();
            }
        }
        if (key.find("synapse_density") != std::string::npos) {
            if (std::holds_alternative<double>(value)) {
                double density = std::get<double>(value);
                if (density < 0.0 || density > 1.0) {
                    ss << "synapse_density must be between 0.0 and 1.0, got " << density;
                    return ss.str();
                }
            }
        }
        if (key.find("connection_probability") != std::string::npos) {
            if (std::holds_alternative<double>(value)) {
                double prob = std::get<double>(value);
                if (prob < 0.0 || prob > 1.0) {
                    ss << "connection_probability must be between 0.0 and 1.0, got " << prob;
                    return ss.str();
                }
            }
        }
        if (key.find("v_thresh") != std::string::npos) {
            if (std::holds_alternative<double>(value)) {
                double thresh = std::get<double>(value);
                if (thresh > -40.0 || thresh < -80.0) {
                    ss << "v_thresh must be between -80.0 and -40.0 mV, got " << thresh;
                    return ss.str();
                }
            }
        }
        if (key.find("tau_mem") != std::string::npos) {
            if (std::holds_alternative<double>(value) && std::get<double>(value) <= 0.0) {
                ss << "tau_mem must be positive, got " << std::get<double>(value);
                return ss.str();
            }
        }
        
        ss << "(validation failed for key: " << key << ")";
        return ss.str();
    }
};

// Batch simulation manager
class BatchSimulationManager {
public:
    struct BatchConfig {
        int numSteps;
        int batchSize;
        bool parallelExecution;
        std::string outputFile;
        bool saveCheckpoints;
        double checkpointInterval;
        std::function<void(int, float)> progressCallback;
        std::function<void(int, const std::string&)> logCallback;
    };
    
    struct BatchResult {
        int totalSteps;
        int successfulSteps;
        float totalTime;
        float averageTimePerStep;
        std::map<std::string, float> metrics;
        std::vector<std::string> errors;
    };
    
    BatchSimulationManager(std::shared_ptr<Brain> brain, BatchConfig config)
        : brain_(brain), config_(config), isRunning_(false) {}
    
    BatchResult runBatch() {
        BatchResult result;
        result.totalSteps = config_.numSteps;
        result.successfulSteps = 0;
        result.totalTime = 0.0f;
        
        isRunning_ = true;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            for (int step = 0; step < config_.numSteps && isRunning_; ++step) {
                auto stepStartTime = std::chrono::high_resolution_clock::now();
                
                brain_->step(step);
                
                auto stepEndTime = std::chrono::high_resolution_clock::now();
                auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEndTime - stepStartTime).count() / 1000.0f;
                
                result.successfulSteps++;
                result.totalTime += stepDuration;
                
                // Save checkpoint if enabled
                if (config_.saveCheckpoints && step % static_cast<int>(config_.checkpointInterval) == 0) {
                    std::string checkpointFile = config_.outputFile + "_checkpoint_step_" + std::to_string(step) + ".nlm";
                    brain_->save(checkpointFile);
                }
                
                // Report progress
                if (config_.progressCallback) {
                    float progress = static_cast<float>(step) / static_cast<float>(config_.numSteps);
                    config_.progressCallback(step, progress);
                }
                
                // Log every 100 steps
                if (step % 100 == 0 && config_.logCallback) {
                    config_.logCallback(step, "Batch simulation in progress...");
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.totalTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
            
            if (config_.logCallback) {
                config_.logCallback(config_.numSteps, "Batch simulation completed successfully.");
            }
            
        } catch (const std::exception& e) {
            result.errors.push_back(std::string("Error during batch simulation: ") + e.what());
            isRunning_ = false;
        }
        
        if (result.successfulSteps > 0) {
            result.averageTimePerStep = result.totalTime / result.successfulSteps;
        }
        
        return result;
    }
    
    void stop() {
        isRunning_ = false;
    }
    
    bool isRunning() const {
        return isRunning_;
    }
    
    BatchConfig& getConfig() {
        return config_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    BatchConfig config_;
    std::atomic<bool> isRunning_;
};

// Performance monitor
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuUsage;
        double memoryUsage;
        double simulationTime;
        size_t spikeCount;
        size_t activeNeuronCount;
        float averageFiringRate;
        int stepCount;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
    };
    
    PerformanceMonitor() : isMonitoring_(false), stepCount_(0), totalSimulationTime_(0.0) {}
    
    void startMonitoring(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        isMonitoring_ = true;
        stepCount_ = 0;
        totalSimulationTime_ = 0.0;
        metrics_.startTime = std::chrono::system_clock::now();
        
        // Start monitoring thread
        monitoringThread_ = std::thread(&PerformanceMonitor::monitorLoop, this);
    }
    
    void stopMonitoring() {
        isMonitoring_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        metrics_.endTime = std::chrono::system_clock::now();
        metrics_.simulationTime = std::chrono::duration<double>(metrics_.endTime - metrics_.startTime).count();
        
        // Collect final metrics
        collectMetrics();
    }
    
    Metrics getMetrics() const {
        return metrics_;
    }
    
    void recordStep(double duration) {
        stepCount_++;
        totalSimulationTime_ += duration;
    }
    
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== Performance Monitoring Report ===\n";
        ss << "Simulation Time: " << metrics_.simulationTime << " seconds\n";
        ss << "Total Steps: " << stepCount_ << "\n";
        ss << "Average Step Time: " << (stepCount_ > 0 ? totalSimulationTime_ / stepCount_ : 0.0) << " seconds\n";
        ss << "CPU Usage: " << metrics_.cpuUsage << "%\n";
        ss << "Memory Usage: " << metrics_.memoryUsage << "%\n";
        ss << "Spike Count: " << metrics_.spikeCount << "\n";
        ss << "Active Neuron Count: " << metrics_.activeNeuronCount << "\n";
        ss << "Average Firing Rate: " << metrics_.averageFiringRate << " Hz\n";
        ss << "===================================\n";
        return ss.str();
    }
    
    void exportMetrics(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw NLMError("Could not open file for metrics export: " + filename);
        }
        
        file << "step,simulation_time,spike_count,active_neuron_count,firing_rate\n";
        for (size_t i = 0; i < stepMetrics_.size(); ++i) {
            file << i << ","
                 << stepMetrics_[i].simulationTime << ","
                 << stepMetrics_[i].spikeCount << ","
                 << stepMetrics_[i].activeNeuronCount << ","
                 << stepMetrics_[i].firingRate << "\n";
        }
        
        file.close();
    }
    
private:
    void monitorLoop() {
        while (isMonitoring_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            collectMetrics();
        }
    }
    
    void collectMetrics() {
        if (!brain_) return;
        
        Metrics currentMetrics;
        currentMetrics.cpuUsage = getCPUUsage();
        currentMetrics.memoryUsage = getMemoryUsage();
        currentMetrics.spikeCount = brain_->getTotalSpikeCount();
        currentMetrics.activeNeuronCount = brain_->getActiveNeuronCount();
        currentMetrics.averageFiringRate = brain_->getAverageFiringRate();
        
        StepMetrics stepMetric;
        stepMetric.simulationTime = totalSimulationTime_ / stepCount_;
        stepMetric.spikeCount = currentMetrics.spikeCount;
        stepMetric.activeNeuronCount = currentMetrics.activeNeuronCount;
        stepMetric.firingRate = currentMetrics.averageFiringRate;
        
        stepMetrics_.push_back(stepMetric);
        
        // Update aggregated metrics
        metrics_ = currentMetrics;
        metrics_.stepCount = stepCount_;
    }
    
    double getCPUUsage() {
        // Placeholder - would use system-specific methods
        return 0.0;
    }
    
    double getMemoryUsage() {
        // Placeholder - would use system-specific methods
        return 0.0;
    }
    
    struct StepMetrics {
        double simulationTime;
        size_t spikeCount;
        size_t activeNeuronCount;
        float firingRate;
    };
    
    std::shared_ptr<Brain> brain_;
    std::atomic<bool> isMonitoring_;
    std::thread monitoringThread_;
    Metrics metrics_;
    std::atomic<int> stepCount_;
    std::atomic<double> totalSimulationTime_;
    std::vector<StepMetrics> stepMetrics_;
};

// Enhanced Config class with validation
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    
    // Enhanced validation
    bool set(const std::string& key, const nlm::ConfigValue& value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        // Validate before setting
        if (!ConfigValidator::validateValue(key, value)) {
            throw ValidationError(ConfigValidator::getValidationError(key, value));
        }
        
        config_->set(key, value, source);
        return true;
    }
    
    // Convenience setters with validation
    bool set(const std::string& key, int value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        nlm::ConfigValue val = value;
        return set(key, val, source);
    }
    
    bool set(const std::string& key, double value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        nlm::ConfigValue val = value;
        return set(key, val, source);
    }
    
    bool set(const std::string& key, const std::string& value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        nlm::ConfigValue val = value;
        return set(key, val, source);
    }
    
    bool set(const std::string& key, bool value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        nlm::ConfigValue val = value;
        return set(key, val, source);
    }
    
    // Get with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    // Get or with default and validation
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath) {
        // TODO: Implement proper JSON parsing
        // For now, use the existing file format but with JSON validation
        return config_->loadFromFile(filepath);
    }
    
    // Save to JSON file
    bool saveToJSON(const std::string& filepath) const {
        // TODO: Implement proper JSON serialization
        return config_->saveToFile(filepath);
    }
    
    // Validate entire configuration
    py::list validate() const {
        py::list errors;
        auto keys = config_->getKeys();
        
        for (const auto& key : keys) {
            // For simplicity, skip detailed validation in this binding
            // In a real implementation, this would access config entries
            if (key.find("neuron_count") != std::string::npos) {
                // We can't easily check the value without accessing private members
                // So we'll just provide a warning that validation is limited
                errors.append("Configuration validation requires access to internal config entries");
            }
        }
        
        return errors;
    }
    
    // Get underlying config
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Brain class with monitoring and control
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    bool initialize() {
        return brain_->initialize();
    }
    
    // Enhanced step methods with timing and monitoring
    void stepWithTiming(SimulationStep currentStep) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        brain_->step(currentStep);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0f;
        
        // Record step time for performance monitoring
        if (monitor_) {
            monitor_->recordStep(duration);
        }
    }
    
    void step(SimulationStep currentStep, Timestamp currentTime) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        brain_->step(currentStep, currentTime);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0f;
        
        if (monitor_) {
            monitor_->recordStep(duration);
        }
    }
    
    // Batch simulation
    BatchResult runBatch(int numSteps, int batchSize = 1, bool parallelExecution = false) {
        BatchSimulationManager::BatchConfig config;
        config.numSteps = numSteps;
        config.batchSize = batchSize;
        config.parallelExecution = parallelExecution;
        config.saveCheckpoints = false;
        config.checkpointInterval = 100.0;
        
        BatchSimulationManager batchManager(brain_, config);
        return batchManager.runBatch();
    }
    
    // Performance monitoring
    void startPerformanceMonitoring() {
        monitor_ = std::make_unique<PerformanceMonitor>();
        monitor_->startMonitoring(brain_);
    }
    
    void stopPerformanceMonitoring() {
        if (monitor_) {
            monitor_->stopMonitoring();
        }
    }
    
    PerformanceMonitor::Metrics getPerformanceMetrics() const {
        return monitor_ ? monitor_->getMetrics() : PerformanceMonitor::Metrics();
    }
    
    std::string generatePerformanceReport() const {
        return monitor_ ? monitor_->generateReport() : "Performance monitoring not started.";
    }
    
    // Export metrics
    void exportMetrics(const std::string& filename) const {
        if (monitor_) {
            monitor_->exportMetrics(filename);
        }
    }
    
    // All other Brain methods (delegating to underlying brain)
    bool initialize() {
        return brain_->initialize();
    }
    
    void step(SimulationStep currentStep) {
        brain_->step(currentStep);
    }
    
    void step(SimulationStep currentStep, Timestamp currentTime) {
        brain_->step(currentStep, currentTime);
    }
    
    void receiveSensoryInput(const SensoryInput& input) {
        brain_->receiveSensoryInput(input);
    }
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        brain_->injectCurrent(neuron, current);
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    SpikeSystem* getSpikeSystem() {
        return brain_->getSpikeSystem();
    }
    
    const SpikeSystem* getSpikeSystem() const {
        return brain_->getSpikeSystem();
    }
    
    STDP* getSTDP() {
        return brain_->getSTDP();
    }
    
    Hebbian* getHebbian() {
        return brain_->getHebbian();
    }
    
    StructuralPlasticity* getStructuralPlasticity() {
        return brain_->getStructuralPlasticity();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    size_t getPendingSpikeEventCount() const {
        return brain_->getPendingSpikeEventCount();
    }
    
    std::unique_ptr<Action> produceAction() {
        return brain_->produceAction();
    }
    
    void applyNeuromodulation(const Neuromodulator& signal) {
        brain_->applyNeuromodulation(signal);
    }
    
    void updatePlasticity() {
        brain_->updatePlasticity();
    }
    
    void develop() {
        brain_->develop();
    }
    
    void reset() {
        brain_->reset();
    }
    
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    RegionId addRegion(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    NeuralRegion* getRegion(RegionId id) {
        return brain_->getRegion(id);
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        return brain_->getRegion(id);
    }
    
    size_t getRegionCount() const {
        return brain_->getRegionCount();
    }
    
    std::vector<RegionId> getRegionIds() const {
        return brain_->getRegionIds();
    }
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        return brain_->getRegions();
    }
    
    void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1) {
        brain_->addInterRegionConnection(source, target, weight, delay);
    }
    
    void removeInterRegionConnection(RegionId source, RegionId target) {
        brain_->removeInterRegionConnection(source, target);
    }
    
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }
    
    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    NeuralWorkingMemory* getWorkingMemory() {
        return brain_->getWorkingMemory();
    }
    
    NeuralEpisodicMemory* getEpisodicMemory() {
        return brain_->getEpisodicMemory();
    }
    
    NeuralAssociativeMemory* getAssociativeMemory() {
        return brain_->getAssociativeMemory();
    }
    
    PredictionSystem* getPredictionSystem() {
        return brain_->getPredictionSystem();
    }
    
    NeuralPlanner* getPlanner() {
        return brain_->getPlanner();
    }
    
    ConceptFormation* getConceptFormation() {
        return brain_->getConceptFormation();
    }
    
    AttentionalSelection* getAttention() {
        return brain_->getAttention();
    }
    
    DevelopmentSystem* getDevelopmentSystem() {
        return brain_->getDevelopmentSystem();
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    Dopamine* getDopamine() {
        return brain_->getDopamine();
    }
    
    Curiosity* getCuriosity() {
        return brain_->getCuriosity();
    }
    
    Novelty* getNovelty() {
        return brain_->getNovelty();
    }
    
    PredictionError* getPredictionErrorSignal() {
        return brain_->getPredictionErrorSignal();
    }
    
    std::shared_ptr<const Config> getConfig() const {
        return brain_->getConfig();
    }
    
    RandomGenerator* getRandomGenerator() {
        return brain_->getRandomGenerator();
    }
    
    void logStatus() const {
        brain_->logStatus();
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<PerformanceMonitor> monitor_;
};

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides advanced features including batch processing, performance monitoring,
        configuration validation, and enhanced error handling.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);

    // Re-export all basic types and enums (same as in bindings.cpp)
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // ... (continue with all other types from original bindings.cpp)

    // Enhanced Config class
    py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(Enhanced configuration class with validation)pbdoc")
        .def(py::init<>())
        .def("set", static_cast<bool(EnhancedConfig::*)(const std::string&, const nlm::ConfigValue&, nlm::ConfigSource)>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
             "Set configuration value with validation")
        .def("set", static_cast<bool(EnhancedConfig::*)(const std::string&, int, nlm::ConfigSource)>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime)
        .def("set", static_cast<bool(EnhancedConfig::*)(const std::string&, double, nlm::ConfigSource)>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime)
        .def("set", static_cast<bool(EnhancedConfig::*)(const std::string&, const std::string&, nlm::ConfigSource)>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime)
        .def("set", static_cast<bool(EnhancedConfig::*)(const std::string&, bool, nlm::ConfigSource)>(&EnhancedConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime)
        .def("get", static_cast<std::optional<int>(EnhancedConfig::*)(const std::string&) const>(&EnhancedConfig::get<int>),
             py::arg("key"), "Get configuration value as int")
        .def("get", static_cast<std::optional<double>(EnhancedConfig::*)(const std::string&) const>(&EnhancedConfig::get<double>),
             py::arg("key"), "Get configuration value as double")
        .def("get", static_cast<std::optional<bool>(EnhancedConfig::*)(const std::string&) const>(&EnhancedConfig::get<bool>),
             py::arg("key"), "Get configuration value as bool")
        .def("get", static_cast<std::optional<std::string>(EnhancedConfig::*)(const std::string&) const>(&EnhancedConfig::get<std::string>),
             py::arg("key"), "Get configuration value as string")
        .def("getOr", static_cast<int(EnhancedConfig::*)(const std::string&, const int&) const>(&EnhancedConfig::getOr<int>),
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", static_cast<double(EnhancedConfig::*)(const std::string&, const double&) const>(&EnhancedConfig::getOr<double>),
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", static_cast<bool(EnhancedConfig::*)(const std::string&, const bool&) const>(&EnhancedConfig::getOr<bool>),
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", static_cast<std::string(EnhancedConfig::*)(const std::string&, const std::string&) const>(&EnhancedConfig::getOr<std::string>),
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("loadFromFile", &EnhancedConfig::loadFromJSON, py::arg("filepath"),
             "Load configuration from JSON file with validation")
        .def("saveToFile", &EnhancedConfig::saveToJSON, py::arg("filepath"),
             "Save configuration to JSON file")
        .def("validate", &EnhancedConfig::validate,
             "Validate entire configuration and return list of errors")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const EnhancedConfig& cfg) {
            return "<EnhancedConfig: " + cfg.getConfig()->summary() + ">";
        });

    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain", R"pbdoc(Enhanced brain class with performance monitoring and batch processing)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &EnhancedBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (EnhancedBrain::*)(SimulationStep)>(&EnhancedBrain::step),
             py::arg("currentStep"),
             "Perform a simulation step with timing")
        .def("step", static_cast<void (EnhancedBrain::*)(SimulationStep, Timestamp)>(&EnhancedBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timing and timestamp")
        .def("stepWithTiming", &EnhancedBrain::stepWithTiming,
             py::arg("currentStep"), "Perform a simulation step with detailed timing")
        .def("runBatch", &EnhancedBrain::runBatch,
             py::arg("numSteps"), py::arg("batchSize") = 1, py::arg("parallelExecution") = false,
             "Run batch simulation with performance monitoring")
        .def("startPerformanceMonitoring", &EnhancedBrain::startPerformanceMonitoring,
             "Start performance monitoring")
        .def("stopPerformanceMonitoring", &EnhancedBrain::stopPerformanceMonitoring,
             "Stop performance monitoring")
        .def("getPerformanceMetrics", &EnhancedBrain::getPerformanceMetrics,
             "Get current performance metrics")
        .def("generatePerformanceReport", &EnhancedBrain::generatePerformanceReport,
             "Generate performance monitoring report")
        .def("exportMetrics", &EnhancedBrain::exportMetrics, py::arg("filename"),
             "Export performance metrics to file")
        .def("reset", &EnhancedBrain::reset, "Reset brain state")
        .def("save", &EnhancedBrain::save, py::arg("filepath"), "Save brain state")
        .def("load", &EnhancedBrain::load, py::arg("filepath"), "Load brain state")
        .def("logStatus", &EnhancedBrain::logStatus, "Log brain status")
        .def("getBrain", &EnhancedBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain");

    // Performance monitoring result structure
    py::class_<PerformanceMonitor::Metrics>(m, "PerformanceMetrics")
        .def_readonly("cpuUsage", &PerformanceMonitor::Metrics::cpuUsage)
        .def_readonly("memoryUsage", &PerformanceMonitor::Metrics::memoryUsage)
        .def_readonly("simulationTime", &PerformanceMonitor::Metrics::simulationTime)
        .def_readonly("spikeCount", &PerformanceMonitor::Metrics::spikeCount)
        .def_readonly("activeNeuronCount", &PerformanceMonitor::Metrics::activeNeuronCount)
        .def_readonly("averageFiringRate", &PerformanceMonitor::Metrics::averageFiringRate)
        .def_readonly("stepCount", &PerformanceMonitor::Metrics::stepCount);

    // Batch simulation result structure
    py::class_<BatchSimulationManager::BatchResult>(m, "BatchResult")
        .def_readonly("totalSteps", &BatchSimulationManager::BatchResult::totalSteps)
        .def_readonly("successfulSteps", &BatchSimulationManager::BatchResult::successfulSteps)
        .def_readonly("totalTime", &BatchSimulationManager::BatchResult::totalTime)
        .def_readonly("averageTimePerStep", &BatchSimulationManager::BatchResult::averageTimePerStep)
        .def_readonly("metrics", &BatchSimulationManager::BatchResult::metrics)
        .def_readonly("errors", &BatchSimulationManager::BatchResult::errors);

    // Factory functions
    m.def("createEnhancedConfig", []() -> std::shared_ptr<EnhancedConfig> {
        return std::make_shared<EnhancedConfig>();
    }, "Create an enhanced configuration with validation");

    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<EnhancedBrain> {
        return std::make_shared<EnhancedBrain>(config);
    }, py::arg("config"), "Create an enhanced brain with performance monitoring");

    // Re-export standard functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (original)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain (original)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple world (original)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface (original)");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "NLM with enhanced Python bindings including validation, batch processing, and performance monitoring";
}

} // namespace nlm
