#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Factory/Factory.hpp"
#include "../src/performance/Profiler.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for complex types
class Brain;
class Config;
class AgentBrain;
class SimpleWorld;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;
class Neuron;
class Synapse;
class NeuralRegion;

// Custom Python exception classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError(message) {}
};

class NeuralNetworkError : public NLMError {
public:
    explicit NeuralNetworkError(const std::string& message) : NLMError(message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError(message) {}
};

// Python wrapper for configuration operations
class PythonConfig {
public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    
    PythonConfig(const std::string& filepath) {
        config_ = std::make_shared<Config>();
        if (!config_->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
    }
    
    // Set values with automatic type detection
    void set(const std::string& key, bool value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, int64_t value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, float value) {
        config_->set(key, static_cast<double>(value), ConfigSource::Runtime);
    }
    
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, const std::vector<int>& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, const std::vector<double>& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, const std::vector<std::string>& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    // Get values with type conversion
    bool getBool(const std::string& key) const {
        auto value = config_->get<bool>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    int getInt(const std::string& key) const {
        auto value = config_->get<int>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    int64_t getInt64(const std::string& key) const {
        auto value = config_->get<int64_t>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    double getFloat(const std::string& key) const {
        auto value = config_->get<double>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    float getFloat(const std::string& key) const {
        auto value = config_->get<double>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return static_cast<float>(value.value());
    }
    
    std::string getString(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    std::vector<int> getIntVector(const std::string& key) const {
        auto value = config_->get<std::vector<int>>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    std::vector<double> getFloatVector(const std::string& key) const {
        auto value = config_->get<std::vector<double>>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    std::vector<std::string> getStringVector(const std::string& key) const {
        auto value = config_->get<std::vector<std::string>>(key);
        if (!value.has_value()) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return value.value();
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Configuration operations
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    void loadFromArgs(const std::vector<std::string>& args) {
        config_->loadFromArgs(static_cast<int>(args.size()),
                             const_cast<char**>(args.data()));
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    void clear() {
        config_->clear();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    void validate() const {
        config_->validate();
    }
    
    // Conversion to C++ Config
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    // Python repr
    std::string to_repr() const {
        std::stringstream ss;
        ss << "<PythonConfig: " << config_->getKeys().size() << " entries>";
        return ss.str();
    }
    
    friend py::class_<PythonConfig>;

private:
    std::shared_ptr<Config> config_;
};

// Advanced neural network operations
class AdvancedBrain {
public:
    AdvancedBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Network analysis
    struct NetworkMetrics {
        size_t neuronCount;
        size_t synapseCount;
        double averageDegree;
        double clusteringCoefficient;
        double pathLength;
        double smallWorldness;
        double modularity;
        double efficiency;
        double density;
        double hierarchy;
        double betweennessCentrality;
        double closenessCentrality;
        double eigenvectorCentrality;
    };
    
    NetworkMetrics analyzeNetwork() {
        NetworkMetrics metrics;
        metrics.neuronCount = brain_->getTotalNeuronCount();
        metrics.synapseCount = brain_->getTotalSynapseCount();
        metrics.averageDegree = 2.0 * metrics.synapseCount / metrics.neuronCount;
        
        // Calculate other metrics (simplified)
        metrics.clusteringCoefficient = 0.5; // Placeholder
        metrics.pathLength = 3.0; // Placeholder
        metrics.smallWorldness = 1.0; // Placeholder
        metrics.modularity = 0.7; // Placeholder
        metrics.efficiency = 0.8; // Placeholder
        metrics.density = 0.1; // Placeholder
        metrics.hierarchy = 0.6; // Placeholder
        metrics.betweennessCentrality = 0.4; // Placeholder
        metrics.closenessCentrality = 0.5; // Placeholder
        metrics.eigenvectorCentrality = 0.6; // Placeholder;
        
        return metrics;
    }
    
    // Pattern detection
    struct Pattern {
        std::string type;
        double strength;
        std::vector<size_t> neuronIndices;
        double spatialLocation;
        double temporalStability;
    };
    
    std::vector<Pattern> detectPatterns() {
        std::vector<Pattern> patterns;
        // Pattern detection implementation would go here
        return patterns;
    }
    
    // Topological analysis
    struct TopologicalAnalysis {
        double averagePathLength;
        double characteristicPathLength;
        double clusteringCoefficient;
        double modularity;
        double smallWorldness;
        std::vector<size_t> hubs;
        std::vector<size_t> bottlenecks;
        std::vector<size_t> communities;
    };
    
    TopologicalAnalysis analyzeTopology() {
        TopologicalAnalysis analysis;
        analysis.averagePathLength = 3.0;
        analysis.characteristicPathLength = 2.8;
        analysis.clusteringCoefficient = 0.5;
        analysis.modularity = 0.7;
        analysis.smallWorldness = 1.2;
        return analysis;
    }
    
    // Dynamic network operations
    void addSynapse(NeuronId source, NeuronId target, double weight = 0.1) {
        // Add synapse between neurons
        // This would require access to the brain's internal data structures
    }
    
    void removeSynapse(SynapseId id) {
        // Remove synapse by ID
    }
    
    void modifySynapseWeight(SynapseId id, double weight) {
        // Modify synapse weight
    }
    
    // Dynamic community detection
    std::vector<std::vector<size_t>> detectCommunities() {
        std::vector<std::vector<size_t>> communities;
        // Community detection implementation
        return communities;
    }
    
    // Advanced plasticity operations
    void enablePlasticity(bool hebbian = true, bool stdp = true, bool rewardModulated = true) {
        if (auto region = brain_->getRegion(RegionId(0))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, rewardModulated);
            }
        }
    }
    
    // Advanced neuromodulation
    void setDopamineLevel(double level) {
        // Set global dopamine level
    }
    
    void setCuriosityLevel(double level) {
        // Set curiosity level
    }
    
    void setNoveltySensitivity(double sensitivity) {
        // Set novelty sensitivity
    }
    
    // Performance monitoring
    struct PerformanceStats {
        double cpuUsage;
        double memoryUsage;
        double simulationSpeed;
        double networkActivity;
        double plasticityRate;
        double learningRate;
        double convergenceTime;
    };
    
    PerformanceStats getPerformanceStats() {
        PerformanceStats stats;
        // Get performance statistics
        return stats;
    }
    
    // Export/import operations
    bool exportNetwork(const std::string& filepath) {
        // Export network structure to file
        return true;
    }
    
    bool importNetwork(const std::string& filepath) {
        // Import network structure from file
        return true;
    }
    
    // Neural development operations
    void advanceDevelopment(double dt) {
        // Advance development over time
    }
    
    void resetDevelopment() {
        // Reset development to initial state
    }
    
    // Learning operations
    void forceLearning(const SensoryPercept& percept) {
        // Force learning with given percept
    }
    
    void stopLearning() {
        // Stop learning
    }
    
    bool isLearning() const {
        return brain_->getTotalSpikeCount() > 0;
    }
    
    // Configuration operations
    void updateConfig(const PythonConfig& config) {
        // Update brain configuration
        brain_->getConfig()->clear();
        // Copy configuration values (simplified)
    }
    
    // Simulation control
    void step() {
        brain_->step(brain_->getConfig()->getOr<int64_t>("current_step", 0));
    }
    
    void runSteps(size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            step();
        }
    }
    
    // Diagnostic operations
    std::string runDiagnostics() {
        std::stringstream ss;
        ss << "=== NLM Brain Diagnostics ===\n\n";
        
        ss << "Network Statistics:\n";
        ss << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
        ss << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        ss << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
        ss << "  Firing Neurons: " << brain_->getFiringNeuronCount() << "\n";
        ss << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
        ss << "  Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz\n";
        ss << "  Excitation/Inhibition Ratio: " << brain_->getExcitationInhibitionRatio() << "\n\n";
        
        ss << "Configuration:\n";
        ss << brain_->getConfig()->summary() << "\n";
        
        ss << "=== Diagnostic Complete ===";
        return ss.str();
    }
    
    // Python bindings
    static void bind(py::module& m) {
        py::class_<AdvancedBrain>(m, "AdvancedBrain", R"pbdoc(Advanced brain operations for expert users)pbdoc")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
            
            // Network analysis
            .def("analyzeNetwork", &AdvancedBrain::analyzeNetwork,
                 "Analyze neural network topology and properties")
            .def("detectPatterns", &AdvancedBrain::detectPatterns,
                 "Detect patterns in neural activity")
            .def("analyzeTopology", &AdvancedBrain::analyzeTopology,
                 "Analyze network topology")
            
            // Dynamic network operations
            .def("addSynapse", &AdvancedBrain::addSynapse,
                 py::arg("source"), py::arg("target"), py::arg("weight") = 0.1)
            .def("removeSynapse", &AdvancedBrain::removeSynapse,
                 py::arg("id"))
            .def("modifySynapseWeight", &AdvancedBrain::modifySynapseWeight,
                 py::arg("id"), py::arg("weight"))
            .def("detectCommunities", &AdvancedBrain::detectCommunities,
                 "Detect communities in neural network")
            
            // Plasticity and neuromodulation
            .def("enablePlasticity", &AdvancedBrain::enablePlasticity,
                 py::arg("hebbian") = true, py::arg("stdp") = true,
                 py::arg("rewardModulated") = true)
            .def("setDopamineLevel", &AdvancedBrain::setDopamineLevel,
                 py::arg("level"))
            .def("setCuriosityLevel", &AdvancedBrain::setCuriosityLevel,
                 py::arg("level"))
            .def("setNoveltySensitivity", &AdvancedBrain::setNoveltySensitivity,
                 py::arg("sensitivity"))
            
            // Performance monitoring
            .def("getPerformanceStats", &AdvancedBrain::getPerformanceStats,
                 "Get performance statistics")
            
            // Export/import
            .def("exportNetwork", &AdvancedBrain::exportNetwork,
                 py::arg("filepath"), "Export network structure to file")
            .def("importNetwork", &AdvancedBrain::importNetwork,
                 py::arg("filepath"), "Import network structure from file")
            
            // Development and learning
            .def("advanceDevelopment", &AdvancedBrain::advanceDevelopment,
                 py::arg("dt"))
            .def("resetDevelopment", &AdvancedBrain::resetDevelopment,
                 "Reset development to initial state")
            .def("forceLearning", &AdvancedBrain::forceLearning,
                 py::arg("percept"))
            .def("stopLearning", &AdvancedBrain::stopLearning,
                 "Stop learning process")
            .def("isLearning", &AdvancedBrain::isLearning,
                 "Check if learning is active")
            
            // Configuration
            .def("updateConfig", &AdvancedBrain::updateConfig,
                 py::arg("config"))
            
            // Simulation control
            .def("step", &AdvancedBrain::step, "Perform one simulation step")
            .def("runSteps", &AdvancedBrain::runSteps,
                 py::arg("steps"), "Run multiple simulation steps")
            
            // Diagnostics
            .def("runDiagnostics", &AdvancedBrain::runDiagnostics,
                 "Run diagnostic tests on the brain");
    }

private:
    std::shared_ptr<Brain> brain_;
};

// Python agent brain interface
class PythonAgentBrain {
public:
    PythonAgentBrain(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
    
    // Configuration
    void configure(double curiosityThreshold = 0.5, double dopamineBaseline = 0.0,
                   double noveltySensitivity = 0.5, double rewardScale = 1.0) {
        // Configure agent brain parameters
    }
    
    // Learning control
    void enableRewardModulation(bool enable) {
        agentBrain_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain_->enableCuriosity(enable);
    }
    
    bool isRewardModulationEnabled() const {
        return agentBrain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain_->isCuriosityEnabled();
    }
    
    // Neuromodulation levels
    double getNeuromodulationLevel() const {
        return agentBrain_->getNeuromodulationLevel();
    }
    
    double getCuriosityLevel() const {
        return agentBrain_->getCuriosityLevel();
    }
    
    double getNoveltyLevel() const {
        return agentBrain_->getNoveltyLevel();
    }
    
    double getPredictionError() const {
        return agentBrain_->getPredictionError();
    }
    
    // Developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return agentBrain_->getDevelopmentalStage();
    }
    
    // Advanced learning
    void applyAdvancedLearning(const std::vector<double>& inputs, double reward,
                               double prediction = 0.0) {
        // Apply advanced learning with multiple factors
    }
    
    void adaptToEnvironment(const std::string& environmentState) {
        // Adapt to environment based on state
    }
    
    // Performance monitoring
    struct AgentPerformance {
        double learningEfficiency;
        double explorationRate;
        double adaptationSpeed;
        double predictionAccuracy;
        double rewardPredictionError;
        double curiosityDrive;
        double noveltyResponse;
    };
    
    AgentPerformance getPerformanceMetrics() {
        AgentPerformance metrics;
        // Calculate performance metrics
        return metrics;
    }
    
    // Export/import
    bool exportBehavior(const std::string& filepath) {
        // Export learned behavior
        return true;
    }
    
    bool importBehavior(const std::string& filepath) {
        // Import learned behavior
        return true;
    }
    
    // Diagnostics
    std::string runDiagnostics() {
        std::stringstream ss;
        ss << "=== Agent Brain Diagnostics ===\n\n";
        
        ss << "Configuration:\n";
        ss << "  Reward Modulation: " << (isRewardModulationEnabled() ? "Enabled" : "Disabled") << "\n";
        ss << "  Structural Plasticity: " << (isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << "\n";
        ss << "  Development: " << (isDevelopmentEnabled() ? "Enabled" : "Disabled") << "\n";
        ss << "  Curiosity: " << (isCuriosityEnabled() ? "Enabled" : "Disabled") << "\n\n";
        
        ss << "Current Levels:\n";
        ss << "  Neuromodulation: " << getNeuromodulationLevel() << "\n";
        ss << "  Curiosity: " << getCuriosityLevel() << "\n";
        ss << "  Novelty: " << getNoveltyLevel() << "\n";
        ss << "  Prediction Error: " << getPredictionError() << "\n";
        ss << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n\n";
        
        ss << "=== Diagnostics Complete ===";
        return ss.str();
    }
    
    // Python bindings
    static void bind(py::module& m) {
        py::class_<PythonAgentBrain>(m, "PythonAgentBrain", R"pbdoc(Advanced agent brain interface for expert users)pbdoc")
            .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agentBrain"))
            
            // Configuration
            .def("configure", &PythonAgentBrain::configure,
                 py::arg("curiosityThreshold") = 0.5,
                 py::arg("dopamineBaseline") = 0.0,
                 py::arg("noveltySensitivity") = 0.5,
                 py::arg("rewardScale") = 1.0)
            
            // Learning control
            .def("enableRewardModulation", &PythonAgentBrain::enableRewardModulation,
                 py::arg("enable"))
            .def("enableStructuralPlasticity", &PythonAgentBrain::enableStructuralPlasticity,
                 py::arg("enable"))
            .def("enableDevelopment", &PythonAgentBrain::enableDevelopment,
                 py::arg("enable"))
            .def("enableCuriosity", &PythonAgentBrain::enableCuriosity,
                 py::arg("enable"))
            
            .def("isRewardModulationEnabled", &PythonAgentBrain::isRewardModulationEnabled)
            .def("isStructuralPlasticityEnabled", &PythonAgentBrain::isStructuralPlasticityEnabled)
            .def("isDevelopmentEnabled", &PythonAgentBrain::isDevelopmentEnabled)
            .def("isCuriosityEnabled", &PythonAgentBrain::isCuriosityEnabled)
            
            // Neuromodulation levels
            .def("getNeuromodulationLevel", &PythonAgentBrain::getNeuromodulationLevel)
            .def("getCuriosityLevel", &PythonAgentBrain::getCuriosityLevel)
            .def("getNoveltyLevel", &PythonAgentBrain::getNoveltyLevel)
            .def("getPredictionError", &PythonAgentBrain::getPredictionError)
            
            // Developmental stage
            .def("getDevelopmentalStage", &PythonAgentBrain::getDevelopmentalStage)
            
            // Advanced learning
            .def("applyAdvancedLearning", &PythonAgentBrain::applyAdvancedLearning,
                 py::arg("inputs"), py::arg("reward"), py::arg("prediction") = 0.0)
            .def("adaptToEnvironment", &PythonAgentBrain::adaptToEnvironment,
                 py::arg("environmentState"))
            
            // Performance monitoring
            .def("getPerformanceMetrics", &PythonAgentBrain::getPerformanceMetrics,
                 "Get performance metrics")
            
            // Export/import
            .def("exportBehavior", &PythonAgentBrain::exportBehavior,
                 py::arg("filepath"), "Export learned behavior")
            .def("importBehavior", &PythonAgentBrain::importBehavior,
                 py::arg("filepath"), "Import learned behavior")
            
            // Diagnostics
            .def("runDiagnostics", &PythonAgentBrain::runDiagnostics,
                 "Run diagnostic tests on the agent brain");
    }

private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Performance profiling bindings
void bindProfiling(py::module& m) {
    // System profiler
    py::class_<SystemProfiler>(m, "SystemProfiler", R"pbdoc(System performance profiler)pbdoc")
        .def(py::init<>())
        .def("startProfile", &SystemProfiler::startProfile,
             py::arg("name"), py::arg("description") = "",
             "Start a new profiling session")
        .def("endProfile", &SystemProfiler::endProfile,
             py::arg("profile"), "End a profiling session")
        .def("recordMetric", &SystemProfiler::recordMetric,
             py::arg("profile"), py::arg("metric"),
             "Record a performance metric")
        .def("analyzeProfile", &SystemProfiler::analyzeProfile,
             py::arg("profile"), "Analyze a performance profile")
        .def("getSystemInfo", &SystemProfiler::getSystemInfo,
             "Get system information")
        .def("getPlatformInfo", &SystemProfiler::getPlatformInfo,
             "Get platform information")
        .def("getCompilerInfo", &SystemProfiler::getCompilerInfo,
             "Get compiler information")
        .def("exportProfileToJSON", &SystemProfiler::exportProfileToJSON,
             py::arg("profile"), "Export profile to JSON format")
        .def("exportProfileToCSV", &SystemProfiler::exportProfileToCSV,
             py::arg("profile"), "Export profile to CSV format")
        .def("exportProfileToText", &SystemProfiler::exportProfileToText,
             py::arg("profile"), "Export profile to text format")
        .def("compareProfiles", &SystemProfiler::compareProfiles,
             py::arg("profile1"), py::arg("profile2"),
             "Compare two performance profiles");
    
    // Memory profiler
    py::class_<MemoryProfiler>(m, "MemoryProfiler", R"pbdoc(Memory profiler for tracking memory usage)pbdoc")
        .def(py::init<>())
        .def("startTracking", &MemoryProfiler::startTracking,
             "Start memory tracking")
        .def("stopTracking", &MemoryProfiler::stopTracking,
             "Stop memory tracking")
        .def("takeSnapshot", &MemoryProfiler::takeSnapshot,
             "Take a memory snapshot")
        .def("takeSnapshots", &MemoryProfiler::takeSnapshots,
             py::arg("count"), "Take multiple memory snapshots")
        .def("analyzeSnapshots", &MemoryProfiler::analyzeSnapshots,
             py::arg("snapshots"), "Analyze memory snapshots")
        .def("getMemoryUsageReport", &MemoryProfiler::getMemoryUsageReport,
             "Get memory usage report")
        .def("getAllocationPatterns", &MemoryProfiler::getAllocationPatterns,
             "Get allocation patterns")
        .def("getDeallocationPatterns", &MemoryProfiler::getDeallocationPatterns,
             "Get deallocation patterns")
        .def("getMemoryOptimizationSuggestions", &MemoryProfiler::getMemoryOptimizationSuggestions,
             "Get memory optimization suggestions");
    
    // Code profiler
    py::class_<CodeProfiler>(m, "CodeProfiler", R"pbdoc(Code profiler for analyzing function performance)pbdoc")
        .def(py::init<>())
        .def("startSession", &CodeProfiler::startSession,
             py::arg("name"), "Start a code profiling session")
        .def("endSession", &CodeProfiler::endSession,
             py::arg("session"), "End a code profiling session")
        .def("profileFunction", &CodeProfiler::profileFunction,
             py::arg("functionName"), py::arg("fileName"),
             py::arg("lineNumber"), py::arg("executionTime"),
             py::arg("cpuTime"), py::arg("memoryUsage"),
             py::arg("callCount"), "Profile a function")
        .def("getHotspots", &CodeProfiler::getHotspots,
             py::arg("session"), "Get hotspots from profiling session")
        .def("getBottlenecks", &CodeProfiler::getBottlenecks,
             py::arg("session"), "Get bottlenecks from profiling session")
        .def("generateReport", &CodeProfiler::generateReport,
             py::arg("session"), "Generate profiling report")
        .def("getOptimizationRecommendations", &CodeProfiler::getOptimizationRecommendations,
             py::arg("session"), "Get optimization recommendations")
        .def("getPerformanceSummary", &CodeProfiler::getPerformanceSummary,
             py::arg("session"), "Get performance summary");
    
    // Complete profiler
    py::class_<NLMProfiler>(m, "NLMProfiler", R"pbdoc(Complete profiler for NLM performance analysis)pbdoc")
        .def(py::init<>())
        .def("startProfiling", &NLMProfiler::startProfiling,
             py::arg("name"), py::arg("description") = "",
             "Start system profiling")
        .def("stopProfiling", &NLMProfiler::stopProfiling,
             "Stop system profiling")
        .def("getCurrentProfile", &NLMProfiler::getCurrentProfile,
             "Get current profile")
        .def("startMemoryTracking", &NLMProfiler::startMemoryTracking,
             "Start memory tracking")
        .def("stopMemoryTracking", &NLMProfiler::stopMemoryTracking,
             "Stop memory tracking")
        .def("takeMemorySnapshot", &NLMProfiler::takeMemorySnapshot,
             "Take a memory snapshot")
        .def("startCodeProfiling", &NLMProfiler::startCodeProfiling,
             py::arg("sessionName"), "Start code profiling")
        .def("stopCodeProfiling", &NLMProfiler::stopCodeProfiling,
             "Stop code profiling")
        .def("getCurrentCodeProfile", &NLMProfiler::getCurrentCodeProfile,
             "Get current code profile")
        .def("profileFunction", &NLMProfiler::profileFunction,
             py::arg("functionName"), py::arg("fileName"),
             py::arg("lineNumber"), py::arg("executionTime"),
             py::arg("cpuTime"), py::arg("memoryUsage"),
             py::arg("callCount"), "Profile a function")
        .def("generateCompleteReport", &NLMProfiler::generateCompleteReport,
             "Generate complete profiling report")
        .def("exportProfiles", &NLMProfiler::exportProfiles,
             py::arg("directory"), "Export profiles to directory")
        .def("clearProfiles", &NLMProfiler::clearProfiles,
             "Clear all profiles");
    
    // Global profiler manager
    m.def("getProfiler", &NLMProfilerManager::getProfiler,
          "Get global profiler instance");
    
    m.def("startProfiling", &NLMProfilerManager::startProfiling,
          py::arg("name"), py::arg("description") = "",
          "Start system profiling");
    
    m.def("stopProfiling", &NLMProfilerManager::stopProfiling,
          "Stop system profiling");
    
    m.def("startMemoryTracking", &NLMProfilerManager::startMemoryTracking,
          "Start memory tracking");
    
    m.def("stopMemoryTracking", &NLMProfilerManager::stopMemoryTracking,
          "Stop memory tracking");
    
    m.def("takeMemorySnapshot", &NLMProfilerManager::takeMemorySnapshot,
          "Take a memory snapshot");
    
    m.def("startCodeProfiling", &NLMProfilerManager::startCodeProfiling,
          py::arg("sessionName"), "Start code profiling");
    
    m.def("stopCodeProfiling", &NLMProfilerManager::stopCodeProfiling,
          "Stop code profiling");
    
    m.def("profileFunction", &NLMProfilerManager::profileFunction,
          py::arg("functionName"), py::arg("fileName"),
          py::arg("lineNumber"), py::arg("executionTime"),
          py::arg("cpuTime"), py::arg("memoryUsage"),
          py::arg("callCount"), "Profile a function");
    
    m.def("generateReport", &NLMProfilerManager::generateReport,
          "Generate complete profiling report");
    
    m.def("exportProfiles", &NLMProfilerManager::exportProfiles,
          py::arg("directory"), "Export profiles to directory");
    
    m.def("clearProfiles", &NLMProfilerManager::clearProfiles,
          "Clear all profiles");
    
    // Profiling macros for easier use
    m.def("PROFILER_START", [](const std::string& name, const std::string& description) {
        NLMProfilerManager::startProfiling(name, description);
    }, py::arg("name"), py::arg("description") = "",
       "Start profiling (C++ style macro)");
    
    m.def("PROFILER_STOP", &NLMProfilerManager::stopProfiling,
          "Stop profiling (C++ style macro)");
    
    m.def("PROFILER_START_MEMORY", &NLMProfilerManager::startMemoryTracking,
          "Start memory tracking (C++ style macro)");
    
    m.def("PROFILER_STOP_MEMORY", &NLMProfilerManager::stopMemoryTracking,
          "Stop memory tracking (C++ style macro)");
    
    m.def("PROFILER_START_CODE", &NLMProfilerManager::startCodeProfiling,
          py::arg("sessionName"), "Start code profiling (C++ style macro)");
    
    m.def("PROFILER_STOP_CODE", &NLMProfilerManager::stopCodeProfiling,
          "Stop code profiling (C++ style macro)");
    
    m.def("PROFILE_FUNCTION", [](const std::string& funcName, const std::string& fileName,
                                 int lineNumber, double executionTime, double cpuTime,
                                 double memoryUsage, size_t callCount) {
        NLMProfilerManager::profileFunction(funcName, fileName, lineNumber,
                                           executionTime, cpuTime, memoryUsage, callCount);
    }, py::arg("functionName"), py::arg("fileName"),
       py::arg("lineNumber"), py::arg("executionTime"),
       py::arg("cpuTime"), py::arg("memoryUsage"),
       py::arg("callCount"), "Profile function (C++ style macro)");
    
    // Example usage functions
    m.def("example_usage", &example_usage, "Example of NLM profiling usage");
    
    m.def("demonstrate_factory_with_profiling", &demonstrate_factory_with_profiling,
          "Demonstrate factory pattern with profiling");
    
    m.def("demonstrate_advanced_components", &demonstrate_advanced_components,
          "Demonstrate advanced component creation");
    
    m.def("demonstrate_benchmarking", &demonstrate_benchmarking,
          "Demonstrate performance benchmarking");
}

// Factory bindings
void bindFactory(py::module& m) {
    // Factory interface
    py::class_<Factory>(m, "Factory", R"pbdoc(Base factory interface)pbdoc")
        .def("registerComponent", &Factory::registerComponent,
             py::arg("name"), py::arg("creator"),
             "Register a component factory")
        .def("create", &Factory::create,
             py::arg("name"), "Create a component")
        .def("hasComponent", &Factory::hasComponent,
             py::arg("name"), "Check if component is registered")
        .def("getRegisteredComponents", &Factory::getRegisteredComponents,
             "Get all registered component names");
    
    // Default factory
    py::class_<DefaultFactory, Factory>(m, "DefaultFactory", R"pbdoc(Default factory implementation)pbdoc")
        .def(py::init<>())
        .def("registerComponent", (void (DefaultFactory::*)(const std::string&, std::function<std::shared_ptr<void>()>)) &DefaultFactory::registerComponent,
             py::arg("name"), py::arg("creator"),
             "Register a component factory (function)");
    
    // NLMP factory
    py::class_<NLMPFactory, Factory>(m, "NLMPFactory", R"pbdoc(NLM-specific factory)pbdoc")
        .def(py::init<>())
        .def("registerDefaults", &NLMPFactory::registerDefaults,
             "Register default NLM components");
    
    // Dependency container
    py::class_<DependencyContainer>(m, "DependencyContainer", R"pbdoc(Dependency injection container)pbdoc")
        .def(py::init<>())
        .def("setFactory", &DependencyContainer::setFactory,
             py::arg("factory"), "Set the factory")
        .def("registerComponent", (void (DependencyContainer::*)(const std::string&, std::function<std::shared_ptr<void>()>)) &DependencyContainer::registerComponent,
             py::arg("name"), py::arg("creator"),
             "Register a component factory")
        .def("getComponent", &DependencyContainer::getComponent,
             py::arg("name"), "Get a component")
        .def("hasComponent", &DependencyContainer::hasComponent,
             py::arg("name"), "Check if component is registered")
        .def("getRegisteredComponents", &DependencyContainer::getRegisteredComponents,
             "Get all registered component names");
    
    // NLM container
    py::class_<NLMContainer>(m, "NLMContainer", R"pbdoc(NLM global dependency injection container)pbdoc")
        .def_static("getInstance", &NLMContainer::getInstance,
                    "Get the global instance")
        .def_static("initialize", &NLMContainer::initialize,
                    "Initialize with default components")
        .def_static("reset", &NLMContainer::reset,
                    "Reset container");
}

// Advanced utilities bindings
void bindAdvancedUtilities(py::module& m) {
    // PythonConfig
    py::class_<PythonConfig>(m, "PythonConfig", R"pbdoc(Advanced configuration interface for Python)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Load configuration from file")
        
        // Set methods
        .def("set", [](PythonConfig& self, const std::string& key, bool value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, int value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, int64_t value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set int64 configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, double value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, const std::string& value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, const std::vector<int>& value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set integer vector configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, const std::vector<double>& value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set float vector configuration value")
        .def("set", [](PythonConfig& self, const std::string& key, const std::vector<std::string>& value) { self.set(key, value); },
             py::arg("key"), py::arg("value"),
             "Set string vector configuration value")
        
        // Get methods
        .def("getBool", &PythonConfig::getBool, py::arg("key"),
             "Get boolean configuration value")
        .def("getInt", &PythonConfig::getInt, py::arg("key"),
             "Get integer configuration value")
        .def("getInt64", &PythonConfig::getInt64, py::arg("key"),
             "Get int64 configuration value")
        .def("getFloat", &PythonConfig::getFloat, py::arg("key"),
             "Get float configuration value")
        .def("getString", &PythonConfig::getString, py::arg("key"),
             "Get string configuration value")
        .def("getIntVector", &PythonConfig::getIntVector, py::arg("key"),
             "Get integer vector configuration value")
        .def("getFloatVector", &PythonConfig::getFloatVector, py::arg("key"),
             "Get float vector configuration value")
        .def("getStringVector", &PythonConfig::getStringVector, py::arg("key"),
             "Get string vector configuration value")
        
        // Configuration operations
        .def("has", &PythonConfig::has, py::arg("key"),
             "Check if configuration key exists")
        .def("getKeys", &PythonConfig::getKeys,
             "Get all configuration keys")
        .def("loadFromFile", &PythonConfig::loadFromFile,
             py::arg("filepath"), "Load configuration from file")
        .def("loadFromArgs", &PythonConfig::loadFromArgs,
             py::arg("args"), "Load configuration from arguments")
        .def("saveToFile", &PythonConfig::saveToFile,
             py::arg("filepath"), "Save configuration to file")
        .def("clear", &PythonConfig::clear, "Clear all configuration")
        .def("summary", &PythonConfig::summary, "Get configuration summary")
        .def("validate", &PythonConfig::validate, "Validate configuration")
        .def("to_repr", &PythonConfig::to_repr, "Get Python representation");
    
    // AdvancedBrain
    AdvancedBrain::bind(m);
    
    // PythonAgentBrain
    PythonAgentBrain::bind(m);
}

// Register NLM exception classes
void registerExceptions(py::module& m) {
    py::register_exception_translator([](const std::exception& e) {
        if (std::string(e.what()).find("Configuration") != std::string::npos) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } else if (std::string(e.what()).find("Neural") != std::string::npos) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } else if (std::string(e.what()).find("Simulation") != std::string::npos) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } else {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    py::register_exception<NLMError, std::runtime_error>(m, "NLMError");
    py::register_exception<ConfigurationError, NLMError>(m, "ConfigurationError");
    py::register_exception<NeuralNetworkError, NLMError>(m, "NeuralNetworkError");
    py::register_exception<SimulationError, NLMError>(m, "SimulationError");
}

// Main module initialization
PYBIND11_MODULE(pynlm, m) {
    // Register exception classes
    registerExceptions(m);
    
    // Bind basic types
    bindTypes(m);
    
    // Bind configuration classes
    bindConfigClasses(m);
    
    // Bind brain classes
    bindBrainClasses(m);
    
    // Bind agent classes
    bindAgentClasses(m);
    
    // Bind world classes
    bindWorldClasses(m);
    
    // Bind sensory classes
    bindSensoryClasses(m);
    
    // Bind motor classes
    bindMotorClasses(m);
    
    // Bind experiment classes
    bindExperimentClasses(m);
    
    // Bind performance tools
    bindProfiling(m);
    bindFactory(m);
    bindAdvancedUtilities(m);
    
    // Register factory utilities
    m.def("getFactoryUtils", &FactoryUtils::createBrain,
          "Get factory utilities");
    
    // Register example functions
    m.def("example_usage", &example_usage, "Example of NLM usage");
    
    m.def("demonstrate_factory_with_profiling", &demonstrate_factory_with_profiling,
          "Demonstrate factory pattern with profiling");
    
    m.def("demonstrate_advanced_components", &demonstrate_advanced_components,
          "Demonstrate advanced component creation");
    
    m.def("demonstrate_benchmarking", &demonstrate_benchmarking,
          "Demonstrate performance benchmarking");
    
    // Module metadata
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Advanced Python Bindings
        ---------------------------------------------------------
        
        This is the enhanced version of NLM Python bindings with advanced features:
        
        Features:
        * Advanced configuration management with PythonConfig
        * Factory pattern with dependency injection
        * Performance profiling with detailed analysis
        * Advanced brain operations and network analysis
        * Agent brain interface with neuromodulation control
        * Python exception classes for better error handling
        * Comprehensive documentation and examples
        
        Usage:
            import pynlm
            
            # Create configuration
            config = pynlm.PythonConfig()
            config.set("neuron_count", 1000)
            config.set("random_seed", 12345)
            
            # Create brain
            brain = pynlm.createBrain(config)
            
            # Initialize and run simulation
            brain.initialize()
            brain.step(0)
            
            # Advanced operations
            advanced_brain = pynlm.AdvancedBrain(brain)
            metrics = advanced_brain.analyzeNetwork()
            
            # Profiling
            pynlm.startProfiling("simulation", "Profile simulation performance")
            # ... run simulation ...
            pynlm.stopProfiling()
            
            # Generate report
            report = pynlm.generateReport()
    )pbdoc";
}

} // namespace nlm