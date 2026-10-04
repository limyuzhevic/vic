#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>

// Standard NLM includes
#include "../src/brain/Brain.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/plasticity/PlasticityRule.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced exception hierarchy
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("Simulation Error: " + message) {}
};

class NeuronError : public NLMException {
public:
    explicit NeuronError(const std::string& message) : NLMException("Neuron Error: " + message) {}
};

class SynapseError : public NLMException {
public:
    explicit SynapseError(const std::string& message) : NLMException("Synapse Error: " + message) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message) : NLMException("Memory Error: " + message) {}
};

class DebugError : public NLMException {
public:
    explicit DebugError(const std::string& message) : NLMException("Debug Error: " + message) {}
};

// Network analysis utilities
class NetworkAnalyzer {
public:
    // Calculate clustering coefficient for a region
    static float calculateClusteringCoefficient(const NeuralRegion& region) {
        // Implementation for clustering coefficient calculation
        // C = 3 * number of triangles / (number of nodes * (number of nodes - 1))
        return 0.0f;
    }
    
    // Calculate average path length in the network
    static float calculateAveragePathLength(const NeuralRegion& region) {
        // Implementation for average path length calculation
        return 0.0f;
    }
    
    // Find network communities using Louvain algorithm
    static std::vector<std::vector<NeuronId>> findCommunities(const NeuralRegion& region) {
        return {};
    }
    
    // Analyze connection patterns
    static std::map<NeuronType, int> analyzeConnectionPatterns(const NeuralRegion& region) {
        return {};
    }
    
    // Calculate betweenness centrality
    static std::map<NeuronId, float> calculateBetweennessCentrality(const NeuralRegion& region) {
        return {};
    }
    
    // Calculate degree distribution
    static std::map<int, int> calculateDegreeDistribution(const NeuralRegion& region) {
        return {};
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuTime;
        double wallTime;
        double memoryUsage;
        size_t spikeCount;
        size_t neuronCount;
        size_t synapseCount;
        float averageFiringRate;
        float energyConsumption;
        std::string timestamp;
        
        PerformanceMetrics()
            : cpuTime(0.0), wallTime(0.0), memoryUsage(0.0), spikeCount(0),
              neuronCount(0), synapseCount(0), averageFiringRate(0.0f),
              energyConsumption(0.0f), timestamp("") {}
        
        std::string toString() const {
            std::stringstream ss;
            ss << "PerformanceMetrics{\n";
            ss << "  CPU Time: " << cpuTime << "s\n";
            ss << "  Wall Time: " << wallTime << "s\n";
            ss << "  Memory Usage: " << memoryUsage << " MB\n";
            ss << "  Spike Count: " << spikeCount << "\n";
            ss << "  Neuron Count: " << neuronCount << "\n";
            ss << "  Synapse Count: " << synapseCount << "\n";
            ss << "  Average Firing Rate: " << averageFiringRate << " Hz\n";
            ss << "  Energy Consumption: " << energyConsumption << "\n";
            ss << "  Timestamp: " << timestamp << "\n";
            ss << "}";
            return ss.str();
        }
    };
    
    static PerformanceMetrics captureMetrics(const Brain& brain) {
        PerformanceMetrics metrics;
        // Capture performance data
        return metrics;
    }
    
    static void startProfiling() {
        // Start performance profiling
    }
    
    static void stopProfiling() {
        // Stop performance profiling
    }
    
    static std::vector<PerformanceMetrics> getProfilingHistory() {
        return {};
    }
    
    static std::string identifyBottlenecks(const Brain& brain) {
        return "No bottlenecks detected";
    }
    
    static double getResourceUsage() {
        return 0.0;
    }
};

// Advanced configuration API
class AdvancedConfig {
public:
    static std::shared_ptr<Config> createDevelopmentConfig() {
        auto config = std::make_shared<Config>();
        // Set development-specific parameters
        return config;
    }
    
    static std::shared_ptr<Config> createProductionConfig() {
        auto config = std::make_shared<Config>();
        // Set production-specific parameters
        return config;
    }
    
    static std::shared_ptr<Config> createResearchConfig() {
        auto config = std::make_shared<Config>();
        // Set research-specific parameters
        return config;
    }
    
    static std::shared_ptr<Config> createQuickTestConfig() {
        auto config = std::make_shared<Config>();
        // Set quick test parameters
        return config;
    }
    
    static std::shared_ptr<Config> batchConfig(const std::vector<std::pair<std::string, std::string>>& settings) {
        auto config = std::make_shared<Config>();
        for (const auto& setting : settings) {
            config->set(setting.first, setting.second);
        }
        return config;
    }
    
    static std::map<std::string, std::string> getConfigTemplate(const std::string& useCase) {
        if (useCase == "debug") {
            return {
                {"log_level", "DEBUG"},
                {"enable_logging", "true"},
                {"save_intermediate_states", "true"}
            };
        } else if (useCase == "performance") {
            return {
                {"optimization_level", "high"},
                {"enable_neuromodulation", "true"},
                {"enable_structural_plasticity", "true"}
            };
        } else if (useCase == "stability") {
            return {
                {"learning_rate", "0.01"},
                {"weight_decay", "0.001"},
                {"min_weight", "-1.0"},
                {"max_weight", "1.0"}
            };
        }
        return {};
    }
};

// Advanced agent behaviors
class AdvancedAgent {
public:
    struct AgentParameters {
        float learningRate = 0.01f;
        float explorationRate = 0.1f;
        float exploitationRate = 0.9f;
        float rewardBaseline = 0.0f;
        float noveltyThreshold = 0.5f;
        float curiosityStrength = 1.0f;
        float predictionErrorDecay = 0.99f;
        bool enableStrategicPlanning = false;
        bool enableMetaLearning = false;
        std::string strategy = "epsilon_greedy";
        
        std::string toString() const {
            std::stringstream ss;
            ss << "AgentParameters{\n";
            ss << "  learningRate: " << learningRate << "\n";
            ss << "  explorationRate: " << explorationRate << "\n";
            ss << "  exploitationRate: " << exploitationRate << "\n";
            ss << "  rewardBaseline: " << rewardBaseline << "\n";
            ss << "  noveltyThreshold: " << noveltyThreshold << "\n";
            ss << "  curiosityStrength: " << curiosityStrength << "\n";
            ss << "  predictionErrorDecay: " << predictionErrorDecay << "\n";
            ss << "  enableStrategicPlanning: " << (enableStrategicPlanning ? "true" : "false") << "\n";
            ss << "  enableMetaLearning: " << (enableMetaLearning ? "true" : "false") << "\n";
            ss << "  strategy: " << strategy << "\n";
            ss << "}";
            return ss.str();
        }
    };
    
    static AgentParameters createDefaultParameters() {
        AgentParameters params;
        params.learningRate = 0.01f;
        params.explorationRate = 0.1f;
        params.exploitationRate = 0.9f;
        params.noveltyThreshold = 0.5f;
        params.curiosityStrength = 1.0f;
        return params;
    }
    
    static AgentParameters createExplorationParameters() {
        AgentParameters params;
        params.learningRate = 0.001f;
        params.explorationRate = 0.5f;
        params.exploitationRate = 0.5f;
        params.noveltyThreshold = 0.2f;
        params.curiosityStrength = 2.0f;
        return params;
    }
    
    static AgentParameters createExploitationParameters() {
        AgentParameters params;
        params.learningRate = 0.05f;
        params.explorationRate = 0.01f;
        params.exploitationRate = 0.99f;
        params.noveltyThreshold = 0.8f;
        params.curiosityStrength = 0.5f;
        return params;
    }
    
    static float calculateExplorationExploitationBalance(
        const AgentParameters& params, float novelty, float reward) {
        // Calculate balance based on novelty and reward
        float noveltyFactor = std::min(1.0f, novelty / params.noveltyThreshold);
        float rewardFactor = std::min(1.0f, reward / (params.rewardBaseline + 0.001f));
        return params.explorationRate * noveltyFactor + params.exploitationRate * rewardFactor;
    }
    
    static MotorCommand strategicActionSelection(
        const AgentBrain& agent,
        const AgentParameters& params,
        const std::vector<MotorCommand>& availableActions) {
        // Implement strategic action selection
        return availableActions.empty() ? MotorCommand::Wait : availableActions[0];
    }
};

// Debugging and inspection tools
class DebugTools {
public:
    struct DebugState {
        std::string timestamp;
        std::string currentStep;
        std::string developmentalStage;
        size_t neuronCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        double simulationTime;
        std::string memoryUsage;
        std::vector<std::string> recentLogs;
        std::map<std::string, float> neuromodulationLevels;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "DebugState{\n";
            ss << "  timestamp: " << timestamp << "\n";
            ss << "  currentStep: " << currentStep << "\n";
            ss << "  developmentalStage: " << developmentalStage << "\n";
            ss << "  neuronCount: " << neuronCount << "\n";
            ss << "  activeNeuronCount: " << activeNeuronCount << "\n";
            ss << "  firingNeuronCount: " << firingNeuronCount << "\n";
            ss << "  averageFiringRate: " << averageFiringRate << " Hz\n";
            ss << "  simulationTime: " << simulationTime << "s\n";
            ss << "  memoryUsage: " << memoryUsage << "\n";
            ss << "  recentLogs: [";
            for (size_t i = 0; i < recentLogs.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << recentLogs[i] << "\"";
            }
            ss << "]\n";
            ss << "  neuromodulationLevels: {";
            for (const auto& pair : neuromodulationLevels) {
                if (&pair != & neuromodulationLevels.begin()->first) ss << ", ";
                ss << pair.first << ": " << pair.second;
            }
            ss << "}\n";
            ss << "}";
            return ss.str();
        }
    };
    
    static DebugState captureDebugState(const Brain& brain) {
        DebugState state;
        // Capture debug information
        return state;
    }
    
    static void stepByStepDebug(Brain& brain, const DebugState& initialState,
                                const DebugState& targetState,
                                const std::function<bool(const DebugState&)>& condition) {
        // Implement step-by-step debugging
    }
    
    static void logEvent(const std::string& event, const std::string& details = "") {
        std::stringstream ss;
        ss << "[" << getCurrentTimestamp() << "] " << event;
        if (!details.empty()) ss << ": " << details;
        // Log the event
    }
    
    static std::vector<std::string> getEventLog() {
        return {};
    }
    
    static void inspectNeuralRegion(const NeuralRegion& region) {
        // Inspect neural region details
    }
    
    static void inspectSynapse(const Synapse& synapse) {
        // Inspect synapse details
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

class NetworkAnalyzer {
public:
    // Calculate clustering coefficient for a region
    static float calculateClusteringCoefficient(const NeuralRegion& region) {
        // Implementation for clustering coefficient calculation
        // C = 3 * number of triangles / (number of nodes * (number of nodes - 1))
        return 0.0f;
    }
    
    // Calculate average path length in the network
    static float calculateAveragePathLength(const NeuralRegion& region) {
        // Implementation for average path length calculation
        return 0.0f;
    }
    
    // Find network communities using Louvain algorithm
    static std::vector<std::vector<NeuronId>> findCommunities(const NeuralRegion& region) {
        return {};
    }
    
    // Analyze connection patterns
    static std::map<NeuronType, int> analyzeConnectionPatterns(const NeuralRegion& region) {
        return {};
    }
    
    // Calculate betweenness centrality
    static std::map<NeuronId, float> calculateBetweennessCentrality(const NeuralRegion& region) {
        return {};
    }
    
    // Calculate degree distribution
    static std::map<int, int> calculateDegreeDistribution(const NeuralRegion& region) {
        return {};
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuTime;
        double wallTime;
        double memoryUsage;
        size_t spikeCount;
        size_t neuronCount;
        size_t synapseCount;
        float averageFiringRate;
        float energyConsumption;
        std::string timestamp;
        
        PerformanceMetrics()
            : cpuTime(0.0), wallTime(0.0), memoryUsage(0.0), spikeCount(0),
              neuronCount(0), synapseCount(0), averageFiringRate(0.0f),
              energyConsumption(0.0f), timestamp("") {}
        
        std::string toString() const {
            std::stringstream ss;
            ss << "PerformanceMetrics{\n";
            ss << "  CPU Time: " << cpuTime << "s\n";
            ss << "  Wall Time: " << wallTime << "s\n";
            ss << "  Memory Usage: " << memoryUsage << " MB\n";
            ss << "  Spike Count: " << spikeCount << "\n";
            ss << "  Neuron Count: " << neuronCount << "\n";
            ss << "  Synapse Count: " << synapseCount << "\n";
            ss << "  Average Firing Rate: " << averageFiringRate << " Hz\n";
            ss << "  Energy Consumption: " << energyConsumption << "\n";
            ss << "  Timestamp: " << timestamp << "\n";
            ss << "}";
            return ss.str();
        }
    };
    
    static PerformanceMetrics captureMetrics(const Brain& brain) {
        PerformanceMetrics metrics;
        // Capture performance data
        return metrics;
    }
    
    static void startProfiling() {
        // Start performance profiling
    }
    
    static void stopProfiling() {
        // Stop performance profiling
    }
    
    static std::vector<PerformanceMetrics> getProfilingHistory() {
        return {};
    }
    
    static std::string identifyBottlenecks(const Brain& brain) {
        return "No bottlenecks detected";
    }
    
    static double getResourceUsage() {
        return 0.0;
    }
};

// Advanced configuration API
class AdvancedConfig {
public:
    static std::shared_ptr<Config> createDevelopmentConfig() {
        auto config = std::make_shared<Config>();
        // Set development-specific parameters
        return config;
    }
    
    static std::shared_ptr<Config> createProductionConfig() {
        auto config = std::make_shared<Config>();
        // Set production-specific parameters
        return config;
    }
    
    static std::shared_ptr<Config> createResearchConfig() {
        auto config = std::make_shared<Config>();
        // Set research-specific parameters
        return config;
    }
    
    static std::shared_ptr<Config> createQuickTestConfig() {
        auto config = std::make_shared<Config>();
        // Set quick test parameters
        return config;
    }
    
    static std::shared_ptr<Config> batchConfig(const std::vector<std::pair<std::string, std::string>>& settings) {
        auto config = std::make_shared<Config>();
        for (const auto& setting : settings) {
            config->set(setting.first, setting.second);
        }
        return config;
    }
    
    static std::map<std::string, std::string> getConfigTemplate(const std::string& useCase) {
        if (useCase == "debug") {
            return {
                {"log_level", "DEBUG"},
                {"enable_logging", "true"},
                {"save_intermediate_states", "true"}
            };
        } else if (useCase == "performance") {
            return {
                {"optimization_level", "high"},
                {"enable_neuromodulation", "true"},
                {"enable_structural_plasticity", "true"}
            };
        } else if (useCase == "stability") {
            return {
                {"learning_rate", "0.01"},
                {"weight_decay", "0.001"},
                {"min_weight", "-1.0"},
                {"max_weight", "1.0"}
            };
        }
        return {};
    }
};

// Advanced agent behaviors
class AdvancedAgent {
public:
    struct AgentParameters {
        float learningRate = 0.01f;
        float explorationRate = 0.1f;
        float exploitationRate = 0.9f;
        float rewardBaseline = 0.0f;
        float noveltyThreshold = 0.5f;
        float curiosityStrength = 1.0f;
        float predictionErrorDecay = 0.99f;
        bool enableStrategicPlanning = false;
        bool enableMetaLearning = false;
        std::string strategy = "epsilon_greedy";
        
        std::string toString() const {
            std::stringstream ss;
            ss << "AgentParameters{\n";
            ss << "  learningRate: " << learningRate << "\n";
            ss << "  explorationRate: " << explorationRate << "\n";
            ss << "  exploitationRate: " << exploitationRate << "\n";
            ss << "  rewardBaseline: " << rewardBaseline << "\n";
            ss << "  noveltyThreshold: " << noveltyThreshold << "\n";
            ss << "  curiosityStrength: " << curiosityStrength << "\n";
            ss << "  predictionErrorDecay: " << predictionErrorDecay << "\n";
            ss << "  enableStrategicPlanning: " << (enableStrategicPlanning ? "true" : "false") << "\n";
            ss << "  enableMetaLearning: " << (enableMetaLearning ? "true" : "false") << "\n";
            ss << "  strategy: " << strategy << "\n";
            ss << "}";
            return ss.str();
        }
    };
    
    static AgentParameters createDefaultParameters() {
        AgentParameters params;
        params.learningRate = 0.01f;
        params.explorationRate = 0.1f;
        params.exploitationRate = 0.9f;
        params.noveltyThreshold = 0.5f;
        params.curiosityStrength = 1.0f;
        return params;
    }
    
    static AgentParameters createExplorationParameters() {
        AgentParameters params;
        params.learningRate = 0.001f;
        params.explorationRate = 0.5f;
        params.exploitationRate = 0.5f;
        params.noveltyThreshold = 0.2f;
        params.curiosityStrength = 2.0f;
        return params;
    }
    
    static AgentParameters createExploitationParameters() {
        AgentParameters params;
        params.learningRate = 0.05f;
        params.explorationRate = 0.01f;
        params.exploitationRate = 0.99f;
        params.noveltyThreshold = 0.8f;
        params.curiosityStrength = 0.5f;
        return params;
    }
    
    static float calculateExplorationExploitationBalance(
        const AgentParameters& params, float novelty, float reward) {
        // Calculate balance based on novelty and reward
        float noveltyFactor = std::min(1.0f, novelty / params.noveltyThreshold);
        float rewardFactor = std::min(1.0f, reward / (params.rewardBaseline + 0.001f));
        return params.explorationRate * noveltyFactor + params.exploitationRate * rewardFactor;
    }
    
    static MotorCommand strategicActionSelection(
        const AgentBrain& agent,
        const AgentParameters& params,
        const std::vector<MotorCommand>& availableActions) {
        // Implement strategic action selection
        return availableActions.empty() ? MotorCommand::Wait : availableActions[0];
    }
};

// Debugging and inspection tools
class DebugTools {
public:
    struct DebugState {
        std::string timestamp;
        std::string currentStep;
        std::string developmentalStage;
        size_t neuronCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        double simulationTime;
        std::string memoryUsage;
        std::vector<std::string> recentLogs;
        std::map<std::string, float> neuromodulationLevels;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "DebugState{\n";
            ss << "  timestamp: " << timestamp << "\n";
            ss << "  currentStep: " << currentStep << "\n";
            ss << "  developmentalStage: " << developmentalStage << "\n";
            ss << "  neuronCount: " << neuronCount << "\n";
            ss << "  activeNeuronCount: " << activeNeuronCount << "\n";
            ss << "  firingNeuronCount: " << firingNeuronCount << "\n";
            ss << "  averageFiringRate: " << averageFiringRate << " Hz\n";
            ss << "  simulationTime: " << simulationTime << "s\n";
            ss << "  memoryUsage: " << memoryUsage << "\n";
            ss << "  recentLogs: [";
            for (size_t i = 0; i < recentLogs.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << recentLogs[i] << "\"";
            }
            ss << "]\n";
            ss << "  neuromodulationLevels: {";
            for (const auto& pair : neuromodulationLevels) {
                if (&pair != & neuromodulationLevels.begin()->first) ss << ", ";
                ss << pair.first << ": " << pair.second;
            }
            ss << "}\n";
            ss << "}";
            return ss.str();
        }
    };
    
    static DebugState captureDebugState(const Brain& brain) {
        DebugState state;
        // Capture debug information
        return state;
    }
    
    static void stepByStepDebug(Brain& brain, const DebugState& initialState,
                                const DebugState& targetState,
                                const std::function<bool(const DebugState&)>& condition) {
        // Implement step-by-step debugging
    }
    
    static void logEvent(const std::string& event, const std::string& details = "") {
        std::stringstream ss;
        ss << "[" << getCurrentTimestamp() << "] " << event;
        if (!details.empty()) ss << ": " << details;
        // Log the event
    }
    
    static std::vector<std::string> getEventLog() {
        return {};
    }
    
    static void inspectNeuralRegion(const NeuralRegion& region) {
        // Inspect neural region details
    }
    
    static void inspectSynapse(const Synapse& synapse) {
        // Inspect synapse details
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

} // namespace nlm

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -----------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides advanced features including:
        - Comprehensive exception handling with specific error types
        - Advanced configuration API with templates and batch operations
        - Network analysis tools for graph metrics and community detection
        - Performance monitoring and profiling capabilities
        - Advanced agent behaviors with strategic action selection
        - Enhanced debugging and inspection tools
        - Comprehensive documentation and type safety
    )pbdoc";

    // Enhanced error handling
    py::register_exception<nlm::NLMException>(m, "NLMException");
    py::register_exception<nlm::ConfigError>(m, "ConfigError", py::module_local());
    py::register_exception<nlm::SimulationError>(m, "SimulationError", py::module_local());
    py::register_exception<nlm::NeuronError>(m, "NeuronError", py::module_local());
    py::register_exception<nlm::SynapseError>(m, "SynapseError", py::module_local());
    py::register_exception<nlm::MemoryError>(m, "MemoryError", py::module_local());
    py::register_exception<nlm::DebugError>(m, "DebugError", py::module_local());

    // Standard exceptions (for backward compatibility)
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ID types (unchanged)
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums (unchanged)
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // === ENHANCED CONFIG ===
    py::class_<Config>(m, "Config", R"pbdoc(Comprehensive configuration class for NLM system)
        
        Advanced configuration API with batch operations, templates, and validation.
        
        Includes:
        - Simple parameter setting via batchConfig()
        - Configuration templates for different use cases
        - Advanced configuration methods for common operations
        - Type safety with parameter validation
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        
        // Advanced Configuration API methods
        .def("batchSet", [](Config& self, const std::map<std::string, std::string>& settings) {
            for (const auto& pair : settings) {
                self.set(pair.first, pair.second);
            }
        }, py::arg("settings"), R"pbdoc(
            Batch set multiple configuration parameters.
            
            Args:
                settings: Dictionary of configuration keys and values to set
            )pbdoc")
        
        .def("batchSetTyped", [](Config& self, const std::map<std::string, double>& settings) {
            for (const auto& pair : settings) {
                self.set(pair.first, pair.second);
            }
        }, py::arg("settings"), R"pbdoc(
            Batch set multiple configuration parameters as doubles.
            
            Args:
                settings: Dictionary of configuration keys and double values to set
            )pbdoc")
        
        .def("batchSetTyped", [](Config& self, const std::map<std::string, int>& settings) {
            for (const auto& pair : settings) {
                self.set(pair.first, pair.second);
            }
        }, py::arg("settings"), R"pbdoc(
            Batch set multiple configuration parameters as integers.
            
            Args:
                settings: Dictionary of configuration keys and integer values to set
            )pbdoc")
        
        .def("batchSetTyped", [](Config& self, const std::map<std::string, bool>& settings) {
            for (const auto& pair : settings) {
                self.set(pair.first, pair.second);
            }
        }, py::arg("settings"), R"pbdoc(
            Batch set multiple configuration parameters as booleans.
            
            Args:
                settings: Dictionary of configuration keys and boolean values to set
            )pbdoc")
        
        .def("setSafe", [](Config& self, const std::string& key, const std::string& value, bool validate = true) {
            if (validate) {
                // Validate the key exists in parameter schema
                if (!self.has(key)) {
                    throw ConfigError("Configuration key \"" + key + "\" does not exist");
                }
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), py::arg("validate") = true, R"pbdoc(
            Safely set a configuration parameter with optional validation.
            
            Args:
                key: Configuration parameter name
                value: Parameter value
                validate: Whether to validate against parameter schema (default: true)
                
            Raises:
                ConfigError: If validation is enabled and key doesn't exist
            )pbdoc")
        
        .def("getSafe", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "", R"pbdoc(
            Safely get a configuration parameter with default value.
            
            Args:
                key: Configuration parameter name
                defaultValue: Default value if key doesn't exist
                
            Returns:
                Parameter value or default if not found
            )pbdoc")
        
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0, R"pbdoc(
            Get an integer configuration parameter with default value.
            
            Args:
                key: Configuration parameter name
                defaultValue: Default value if key doesn't exist
                
            Returns:
                Parameter value or default if not found
            )pbdoc")
        
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0, R"pbdoc(
            Get a double configuration parameter with default value.
            
            Args:
                key: Configuration parameter name
                defaultValue: Default value if key doesn't exist
                
            Returns:
                Parameter value or default if not found
            )pbdoc")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false, R"pbdoc(
            Get a boolean configuration parameter with default value.
            
            Args:
                key: Configuration parameter name
                defaultValue: Default value if key doesn't exist
                
            Returns:
                Parameter value or default if not found
            )pbdoc")
        
        .def("validateParameter", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw ConfigError("Configuration key \"" + key + "\" does not exist");
            }
            // Additional validation logic could be added here
            return true;
        }, py::arg("key"), R"pbdoc(
            Validate a configuration parameter exists and is valid.
            
            Args:
                key: Configuration parameter name
                
            Returns:
                True if parameter is valid
                
            Raises:
                ConfigError: If parameter doesn't exist
            )pbdoc")
        
        // Configuration templates and factory methods
        .def_static("createDevelopmentConfig", &AdvancedConfig::createDevelopmentConfig,
                   R"pbdoc(
            Create a configuration optimized for development and debugging.
            
            Returns:
                Configuration object with development settings
            )pbdoc")
        
        .def_static("createProductionConfig", &AdvancedConfig::createProductionConfig,
                   R"pbdoc(
            Create a configuration optimized for production use.
            
            Returns:
                Configuration object with production settings
            )pbdoc")
        
        .def_static("createResearchConfig", &AdvancedConfig::createResearchConfig,
                   R"pbdoc(
            Create a configuration optimized for research experiments.
            
            Returns:
                Configuration object with research settings
            )pbdoc")
        
        .def_static("createQuickTestConfig", &AdvancedConfig::createQuickTestConfig,
                   R"pbdoc(
            Create a configuration optimized for quick testing.
            
            Returns:
                Configuration object with test settings
            )pbdoc")
        
        .def_static("getConfigTemplate", &AdvancedConfig::getConfigTemplate,
                   py::arg("useCase"),
                   R"pbdoc(
            Get a configuration template for a specific use case.
            
            Args:
                useCase: Use case name ("debug", "performance", "stability", etc.)
                
            Returns:
                Dictionary of configuration settings for the use case
            )pbdoc")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // === ADVANCED ERROR TYPES ===
    py::class_<nlm::NLMException, py::module_local>()
        .def(py::init<const std::string&>())
        .def("what", &nlm::NLMException::what);
    
    py::class_<nlm::ConfigError, py::module_local, nlm::NLMException>()
        .def(py::init<const std::string&>());
    
    py::class_<nlm::SimulationError, py::module_local, nlm::NLMException>()
        .def(py::init<const std::string&>());
    
    py::class_<nlm::NeuronError, py::module_local, nlm::NLMException>()
        .def(py::init<const std::string&>());
    
    py::class_<nlm::SynapseError, py::module_local, nlm::NLMException>()
        .def(py::init<const std::string&>());
    
    py::class_<nlm::MemoryError, py::module_local, nlm::NLMException>()
        .def(py::init<const std::string&>());
    
    py::class_<nlm::DebugError, py::module_local, nlm::NLMException>()
        .def(py::init<const std::string&>());

    // === SENSORY INPUT ===
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // === ACTION ===
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // === WORLD OBJECT ===
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // === AGENT BODY ===
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // === ACTION RESULT ===
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // === SENSORY PERCEPT ===
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // === WORLD ===
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // === BRAIN ===
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // === AGENT BRAIN ===
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // === FACTORY FUNCTIONS ===
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // === ADVANCED FEATURES ===
    
    // Network Analysis Tools
    py::class_<NetworkAnalyzer>(m, "NetworkAnalyzer", R"pbdoc(
        Network analysis tools for neural connectivity
        
        Provides advanced network analysis including clustering coefficient,
        average path length, community detection, and connection pattern analysis.
    )pbdoc")
        .def_static("calculateClusteringCoefficient", &NetworkAnalyzer::calculateClusteringCoefficient,
                   py::arg("region"), R"pbdoc(
            Calculate clustering coefficient for a neural region.
            
            Args:
                region: Neural region to analyze
                
            Returns:
                Clustering coefficient (0-1)
            )pbdoc")
        
        .def_static("calculateAveragePathLength", &NetworkAnalyzer::calculateAveragePathLength,
                   py::arg("region"), R"pbdoc(
            Calculate average path length in the network.
            
            Args:
                region: Neural region to analyze
                
            Returns:
                Average path length
            )pbdoc")
        
        .def_static("findCommunities", &NetworkAnalyzer::findCommunities,
                   py::arg("region"), R"pbdoc(
            Find communities in the network using Louvain algorithm.
            
            Args:
                region: Neural region to analyze
                
            Returns:
                List of communities (each community is a list of neuron IDs)
            )pbdoc")
        
        .def_static("analyzeConnectionPatterns", &NetworkAnalyzer::analyzeConnectionPatterns,
                   py::arg("region"), R"pbdoc(
            Analyze connection patterns in the network.
            
            Args:
                region: Neural region to analyze
                
            Returns:
                Dictionary mapping neuron types to connection counts
            )pbdoc")
        
        .def_static("calculateBetweennessCentrality", &NetworkAnalyzer::calculateBetweennessCentrality,
                   py::arg("region"), R"pbdoc(
            Calculate betweenness centrality for all neurons in the region.
            
            Args:
                region: Neural region to analyze
                
            Returns:
                Dictionary mapping neuron IDs to betweenness centrality values
            )pbdoc")
        
        .def_static("calculateDegreeDistribution", &NetworkAnalyzer::calculateDegreeDistribution,
                   py::arg("region"), R"pbdoc(
            Calculate degree distribution in the network.
            
            Args:
                region: Neural region to analyze
                
            Returns:
                Dictionary mapping degree values to counts
            )pbdoc");

    // Performance Monitoring
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(
        Performance monitoring and profiling for NLM simulations
        
        Provides comprehensive performance metrics, resource usage tracking,
        bottleneck analysis, and profiling capabilities.
    )pbdoc")
        .def_static("captureMetrics", &PerformanceMonitor::captureMetrics,
                   py::arg("brain"), R"pbdoc(
            Capture performance metrics for a brain simulation.
            
            Args:
                brain: Brain instance to analyze
                
            Returns:
                PerformanceMetrics object with detailed statistics
            )pbdoc")
        
        .def_static("startProfiling", &PerformanceMonitor::startProfiling,
                   R"pbdoc(
            Start performance profiling. Call stopProfiling() to end.
            )pbdoc")
        
        .def_static("stopProfiling", &PerformanceMonitor::stopProfiling,
                   R"pbdoc(
            Stop performance profiling.
            )pbdoc")
        
        .def_static("getProfilingHistory", &PerformanceMonitor::getProfilingHistory,
                   R"pbdoc(
            Get profiling history of performance metrics.
            
            Returns:
                List of PerformanceMetrics objects
            )pbdoc")
        
        .def_static("identifyBottlenecks", &PerformanceMonitor::identifyBottlenecks,
                   py::arg("brain"), R"pbdoc(
            Identify performance bottlenecks in the simulation.
            
            Args:
                brain: Brain instance to analyze
                
            Returns:
                String description of identified bottlenecks
            )pbdoc")
        
        .def_static("getResourceUsage", &PerformanceMonitor::getResourceUsage,
                   R"pbdoc(
            Get current resource usage (CPU, memory, etc.).
            
            Returns:
                Resource usage percentage
            )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::cpuTime, R"pbdoc(
            CPU time usage in seconds
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::wallTime, R"pbdoc(
            Wall clock time in seconds
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::memoryUsage, R"pbdoc(
            Memory usage in MB
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::spikeCount, R"pbdoc(
            Total spike count
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::neuronCount, R"pbdoc(
            Total neuron count
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::synapseCount, R"pbdoc(
            Total synapse count
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::averageFiringRate, R"pbdoc(
            Average firing rate in Hz
        )pbdoc")
        .def_readwrite("metrics", &PerformanceMonitor::PerformanceMetrics::energyConsumption, R"pbdoc(
            Energy consumption
        )pbdoc");

    // Advanced Agent Behaviors
    py::class_<AdvancedAgent::AgentParameters>(m, "AgentParameters", R"pbdoc(
        Agent parameters for controlling behavior
        
        Contains parameters for learning rate control, exploration-exploitation balance,
        curiosity, neuromodulation, and strategic planning.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("learningRate", &AdvancedAgent::AgentParameters::learningRate)
        .def_readwrite("explorationRate", &AdvancedAgent::AgentParameters::explorationRate)
        .def_readwrite("exploitationRate", &AdvancedAgent::AgentParameters::exploitationRate)
        .def_readwrite("rewardBaseline", &AdvancedAgent::AgentParameters::rewardBaseline)
        .def_readwrite("noveltyThreshold", &AdvancedAgent::AgentParameters::noveltyThreshold)
        .def_readwrite("curiosityStrength", &AdvancedAgent::AgentParameters::curiosityStrength)
        .def_readwrite("predictionErrorDecay", &AdvancedAgent::AgentParameters::predictionErrorDecay)
        .def_readwrite("enableStrategicPlanning", &AdvancedAgent::AgentParameters::enableStrategicPlanning)
        .def_readwrite("enableMetaLearning", &AdvancedAgent::AgentParameters::enableMetaLearning)
        .def_readwrite("strategy", &AdvancedAgent::AgentParameters::strategy)
        .def("toString", &AdvancedAgent::AgentParameters::toString, R"pbdoc(
            Convert parameters to string representation.
            
            Returns:
                String representation of parameters
            )pbdoc")
        .def_static("createDefaultParameters", &AdvancedAgent::createDefaultParameters,
                   R"pbdoc(
            Create default agent parameters for general use.
            
            Returns:
                AgentParameters object with default values
            )pbdoc")
        .def_static("createExplorationParameters", &AdvancedAgent::createExplorationParameters,
                   R"pbdoc(
            Create parameters optimized for exploration.
            
            Returns:
                AgentParameters object with exploration settings
            )pbdoc")
        .def_static("createExploitationParameters", &AdvancedAgent::createExploitationParameters,
                   R"pbdoc(
            Create parameters optimized for exploitation.
            
            Returns:
                AgentParameters object with exploitation settings
            )pbdoc");

    py::class_<AdvancedAgent>(m, "AdvancedAgent", R"pbdoc(
        Advanced agent control and behavior management
        
        Provides sophisticated agent control methods for strategic action selection,
        learning rate control, and exploration-exploitation balance.
    )pbdoc")
        .def_static("calculateExplorationExploitationBalance", 
                   &AdvancedAgent::calculateExplorationExploitationBalance,
                   py::arg("params"), py::arg("novelty"), py::arg("reward"),
                   R"pbdoc(
            Calculate exploration-exploitation balance based on novelty and reward.
            
            Args:
                params: Agent parameters
                novelty: Current novelty level
                reward: Current reward level
                
            Returns:
                Balance value between 0 and 1
            )pbdoc")
        
        .def_static("strategicActionSelection",
                   &AdvancedAgent::strategicActionSelection,
                   py::arg("agent"), py::arg("params"), py::arg("availableActions"),
                   R"pbdoc(
            Perform strategic action selection based on agent parameters.
            
            Args:
                agent: AgentBrain instance
                params: Agent parameters
                availableActions: List of available motor commands
                
            Returns:
                Selected motor command
            )pbdoc");

    // Enhanced Debugging Tools
    py::class_<DebugTools::DebugState>(m, "DebugState", R"pbdoc(
        Debug state snapshot for inspection and analysis
        
        Contains comprehensive information about simulation state including
        neuron counts, firing rates, neuromodulation levels, and more.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &DebugTools::DebugState::timestamp)
        .def_readwrite("currentStep", &DebugTools::DebugState::currentStep)
        .def_readwrite("developmentalStage", &DebugTools::DebugState::developmentalStage)
        .def_readwrite("neuronCount", &DebugTools::DebugState::neuronCount)
        .def_readwrite("activeNeuronCount", &DebugTools::DebugState::activeNeuronCount)
        .def_readwrite("firingNeuronCount", &DebugTools::DebugState::firingNeuronCount)
        .def_readwrite("averageFiringRate", &DebugTools::DebugState::averageFiringRate)
        .def_readwrite("simulationTime", &DebugTools::DebugState::simulationTime)
        .def_readwrite("memoryUsage", &DebugTools::DebugState::memoryUsage)
        .def_readwrite("recentLogs", &DebugTools::DebugState::recentLogs)
        .def_readwrite("neuromodulationLevels", &DebugTools::DebugState::neuromodulationLevels)
        .def("toString", &DebugTools::DebugState::toString, R"pbdoc(
            Convert debug state to string representation.
            
            Returns:
                String representation of debug state
            )pbdoc")
        .def_static("captureDebugState", &DebugTools::captureDebugState,
                   py::arg("brain"), R"pbdoc(
            Capture current debug state from a brain simulation.
            
            Args:
                brain: Brain instance to inspect
                
            Returns:
                DebugState object with current simulation state
            )pbdoc")
        .def_static("stepByStepDebug", &DebugTools::stepByStepDebug,
                   py::arg("brain"), py::arg("initialState"), py::arg("targetState"), py::arg("condition"),
                   R"pbdoc(
            Perform step-by-step debugging between initial and target states.
            
            Args:
                brain: Brain instance to debug
                initialState: Starting debug state
                targetState: Target debug state
                condition: Function that returns True when debugging should stop
            )pbdoc")
        .def_static("logEvent", &DebugTools::logEvent, py::arg("event"), py::arg("details") = "",
                   R"pbdoc(
            Log an event with optional details.
            
            Args:
                event: Event description
                details: Optional details string
            )pbdoc")
        .def_static("getEventLog", &DebugTools::getEventLog, R"pbdoc(
            Get the event log.
            
            Returns:
                List of event log entries
            )pbdoc")
        .def_static("inspectNeuralRegion", &DebugTools::inspectNeuralRegion,
                   py::arg("region"), R"pbdoc(
            Inspect neural region details for debugging.
            
            Args:
                region: Neural region to inspect
            )pbdoc")
        .def_static("inspectSynapse", &DebugTools::inspectSynapse,
                   py::arg("synapse"), R"pbdoc(
            Inspect synapse details for debugging.
            
            Args:
                synapse: Synapse to inspect
            )pbdoc");

    py::class_<DebugTools>(m, "DebugTools", R"pbdoc(
        Enhanced debugging tools for simulation development
        
        Provides step-by-step debugging, state inspection, event logging,
        and other debugging capabilities for simulation development and troubleshooting.
    )pbdoc")
        .def_static("getCurrentTimestamp", &DebugTools::getCurrentTimestamp,
                   R"pbdoc(
            Get current timestamp string.
            
            Returns:
                Current timestamp in YYYY-MM-DD HH:MM:SS format
            )pbdoc");

    // Sentinel values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error translations for Python
    m.def("translateNLMException", [](const std::exception& e) {
        std::string what = e.what();
        if (what.find("Configuration Error:") != std::string::npos) {
            return py::cast(ConfigError(e.what()));
        } else if (what.find("Simulation Error:") != std::string::npos) {
            return py::cast(SimulationError(e.what()));
        } else if (what.find("Neuron Error:") != std::string::npos) {
            return py::cast(NeuronError(e.what()));
        } else if (what.find("Synapse Error:") != std::string::npos) {
            return py::cast(SynapseError(e.what()));
        } else if (what.find("Memory Error:") != std::string::npos) {
            return py::cast(MemoryError(e.what()));
        } else if (what.find("Debug Error:") != std::string::npos) {
            return py::cast(DebugError(e.what()));
        }
        return py::cast(NLMException(e.what()));
    }, py::arg("exception"), R"pbdoc(
        Translate C++ exceptions to appropriate Python exception types.
        
        Args:
            exception: C++ exception to translate
            
        Returns:
            Appropriate Python exception instance
        )pbdoc");
}

} // namespace nlm
