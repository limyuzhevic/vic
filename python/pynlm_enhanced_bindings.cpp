# Enhanced Python bindings for NLM (Neural Learning Machine)
# 
# This file provides advanced Python API extensions for NLM, including:
# - Advanced configuration management
# - Enhanced simulation controls
# - Performance monitoring tools
# - Extensibility hooks for custom components
# - Advanced analysis and visualization utilities

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

// Forward declarations for extended functionality
namespace nlm {
    // Enhanced configuration with validation
    class ValidatedConfig;
    
    // Advanced simulation manager with step control
    class AdvancedSimulationManager;
    
    // Performance monitoring and metrics collection
    class PerformanceMonitor;
    
    // Custom component factory for extensibility
    class ComponentFactory;
    
    // Advanced analysis utilities
    class AnalysisUtils;
    
    // Custom neural network builders
    class NeuralNetworkBuilder;
    
    // Simulation scheduler for complex protocols
    class SimulationScheduler;
    
    // Advanced visualization and plotting utilities
    class VisualizationUtils;
}

namespace py = pybind11;
namespace nlm {

// Enhanced Configuration with Validation
class ValidatedConfig {
private:
    std::shared_ptr<Config> baseConfig;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::map<std::string, std::string> descriptions;
    bool strictMode;
    
public:
    ValidatedConfig() : baseConfig(std::make_shared<Config>()), strictMode(false) {}
    
    // Validation setup
    void addValidator(const std::string& key, 
                     std::function<bool(const ConfigValue&)> validator,
                     const std::string& description = "") {
        validators[key] = validator;
        descriptions[key] = description;
    }
    
    void setStrictMode(bool strict) {
        strictMode = strict;
    }
    
    // Enhanced configuration methods with validation
    template<typename T>
    bool set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime) {
        // Validate if validator exists
        auto it = validators.find(key);
        if (it != validators.end()) {
            ConfigValue configValue;
            if constexpr (std::is_same_v<T, std::string>) {
                configValue = value;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                configValue = value;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                configValue = value;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                configValue = value;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                configValue = value;
            }
            
            if (!it->second(configValue)) {
                if (strictMode) {
                    throw std::runtime_error("Validation failed for key: " + key);
                }
                return false;
            }
        }
        
        baseConfig->set(key, value, source);
        if (!descriptions[key].empty()) {
            baseConfig->set(key, ConfigValue(descriptions[key]), ConfigSource::Runtime);
        }
        return true;
    }
    
    // Convenience methods for common parameter types
    bool setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    bool setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    bool setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    bool setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    // Batch configuration setting
    bool setBatch(const std::map<std::string, ConfigValue>& batch) {
        bool success = true;
        for (const auto& pair : batch) {
            if (!set(pair.first, pair.second)) {
                success = false;
                if (strictMode) break;
            }
        }
        return success;
    }
    
    // Get underlying config
    std::shared_ptr<Config> getBaseConfig() const {
        return baseConfig;
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        py::class_<ValidatedConfig>(m, "ValidatedConfig")
            .def(py::init<>())
            .def("add_validator", &ValidatedConfig::addValidator,
                 py::arg("key"), py::arg("validator"), py::arg("description"))
            .def("set_strict_mode", &ValidatedConfig::setStrictMode)
            .def("set_int", &ValidatedConfig::setInt, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
            .def("set_double", &ValidatedConfig::setDouble, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
            .def("set_string", &ValidatedConfig::setString, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
            .def("set_bool", &ValidatedConfig::setBool, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
            .def("set_batch", &ValidatedConfig::setBatch)
            .def("get_base_config", &ValidatedConfig::getBaseConfig);
    }
};

// Advanced Simulation Manager
class AdvancedSimulationManager {
private:
    std::shared_ptr<Brain> brain;
    size_t currentStep;
    Timestamp currentTime;
    std::chrono::high_resolution_clock::time_point startTime;
    
    // Protocol definitions
    struct SimulationProtocol {
        std::string name;
        size_t steps;
        std::map<size_t, std::function<void(size_t, Timestamp)>> callbacks;
        std::vector<std::string> phaseMarkers;
        std::map<std::string, std::function<bool()>> conditionalStops;
    };
    
    std::vector<SimulationProtocol> protocols;
    size_t currentProtocolIndex;
    
    // Event system
    struct SimulationEvent {
        size_t timestamp;
        std::string type;
        std::map<std::string, std::string> data;
        std::function<void()> handler;
    };
    
    std::vector<SimulationEvent> events;
    
public:
    AdvancedSimulationManager() : currentStep(0), currentProtocolIndex(0) {}
    
    void initialize(std::shared_ptr<Brain> b) {
        brain = b;
        if (brain && brain->initialize()) {
            startTime = std::chrono::high_resolution_clock::now();
        }
    }
    
    // Protocol management
    void addProtocol(const std::string& name, size_t steps,
                    const std::vector<std::string>& phaseMarkers = {}) {
        SimulationProtocol protocol;
        protocol.name = name;
        protocol.steps = steps;
        protocol.phaseMarkers = phaseMarkers;
        protocols.push_back(protocol);
    }
    
    // Event system
    void addEvent(size_t timestamp, const std::string& type,
                  const std::map<std::string, std::string>& data,
                  std::function<void()> handler) {
        SimulationEvent event;
        event.timestamp = timestamp;
        event.type = type;
        event.data = data;
        event.handler = handler;
        events.push_back(event);
    }
    
    // Enhanced simulation with protocols
    bool runProtocol(const std::string& protocolName) {
        auto it = std::find_if(protocols.begin(), protocols.end(),
                              [&protocolName](const SimulationProtocol& p) {
                                  return p.name == protocolName;
                              });
        if (it == protocols.end()) {
            return false;
        }
        
        // Run protocol
        for (size_t step = 0; step < it->steps; ++step) {
            currentStep = step;
            currentTime = step * 0.001;  // Assuming 1ms timestep
            
            // Execute phase markers
            for (const auto& marker : it->phaseMarkers) {
                if (marker == "before_step" && step % 100 == 0) {
                    // Before step callback
                }
                // Add more phase markers as needed
            }
            
            // Execute events at this timestamp
            for (const auto& event : events) {
                if (event.timestamp == step) {
                    event.handler();
                }
            }
            
            // Run brain step
            if (brain) {
                brain->step(currentStep, currentTime);
            }
            
            // Check conditional stops
            for (const auto& condition : it->conditionalStops) {
                if (!condition.second()) {
                    return true;  // Protocol stopped early
                }
            }
        }
        
        currentProtocolIndex++;
        return true;
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        py::class_<AdvancedSimulationManager>(m, "AdvancedSimulationManager")
            .def(py::init<>())
            .def("initialize", &AdvancedSimulationManager::initialize)
            .def("add_protocol", &AdvancedSimulationManager::addProtocol,
                 py::arg("name"), py::arg("steps"), py::arg("phase_markers") = std::vector<std::string>())
            .def("add_event", &AdvancedSimulationManager::addEvent,
                 py::arg("timestamp"), py::arg("type"), py::arg("data"), py::arg("handler"))
            .def("run_protocol", &AdvancedSimulationManager::runProtocol);
    }
};

// Performance Monitor
class PerformanceMonitor {
private:
    struct PerformanceMetrics {
        size_t totalSteps;
        float avgFiringRate;
        float avgReward;
        float cpuUsage;
        float memoryUsage;
        std::chrono::duration<double> wallClockTime;
        std::vector<float> spikeRates;
        std::vector<float> rewardHistory;
        std::vector<float> noveltyLevels;
        std::vector<float> curiosityLevels;
    };
    
    PerformanceMetrics currentMetrics;
    std::vector<PerformanceMetrics> history;
    bool recording;
    
public:
    PerformanceMonitor() : recording(false) {}
    
    void startRecording() {
        recording = true;
        currentMetrics.totalSteps = 0;
        currentMetrics.spikeRates.clear();
        currentMetrics.rewardHistory.clear();
        currentMetrics.noveltyLevels.clear();
        currentMetrics.curiosityLevels.clear();
    }
    
    void stopRecording() {
        recording = false;
        if (currentMetrics.totalSteps > 0) {
            history.push_back(currentMetrics);
        }
    }
    
    void recordStep(float firingRate, float reward, float novelty = 0.0f, float curiosity = 0.0f) {
        if (!recording) return;
        
        currentMetrics.totalSteps++;
        currentMetrics.spikeRates.push_back(firingRate);
        currentMetrics.rewardHistory.push_back(reward);
        currentMetrics.noveltyLevels.push_back(novelty);
        currentMetrics.curiosityLevels.push_back(curiosity);
    }
    
    // Performance analysis
    float calculateAverageFiringRate() const {
        if (currentMetrics.spikeRates.empty()) return 0.0f;
        float sum = 0.0f;
        for (float rate : currentMetrics.spikeRates) {
            sum += rate;
        }
        return sum / currentMetrics.spikeRates.size();
    }
    
    float calculateAverageReward() const {
        if (currentMetrics.rewardHistory.empty()) return 0.0f;
        float sum = 0.0f;
        for (float reward : currentMetrics.rewardHistory) {
            sum += reward;
        }
        return sum / currentMetrics.rewardHistory.size();
    }
    
    float calculateStandardDeviation(const std::vector<float>& values) const {
        if (values.size() <= 1) return 0.0f;
        float mean = 0.0f;
        for (float val : values) mean += val;
        mean /= values.size();
        
        float variance = 0.0f;
        for (float val : values) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= values.size();
        
        return std::sqrt(variance);
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Report ===\n";
        oss << "Total steps: " << currentMetrics.totalSteps << "\n";
        oss << "Average firing rate: " << calculateAverageFiringRate() << "\n";
        oss << "Average reward: " << calculateAverageReward() << "\n";
        oss << "Firing rate std: " << calculateStandardDeviation(currentMetrics.spikeRates) << "\n";
        oss << "Reward std: " << calculateStandardDeviation(currentMetrics.rewardHistory) << "\n";
        oss << "Novelty std: " << calculateStandardDeviation(currentMetrics.noveltyLevels) << "\n";
        oss << "Curiosity std: " << calculateStandardDeviation(currentMetrics.curiosityLevels) << "\n";
        
        if (!history.empty()) {
            oss << "\n=== Historical Average ===\n";
            size_t numHistorical = std::min<size_t>(10, history.size());
            for (size_t i = std::max<size_t>(0, history.size() - numHistorical); i < history.size(); ++i) {
                const auto& metrics = history[i];
                oss << "Episode " << i << ": "
                    << "Firing rate: " << calculateAverageFiringRate() << ", "
                    << "Reward: " << calculateAverageReward() << "\n";
            }
        }
        
        return oss.str();
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
            .def(py::init<>())
            .def("start_recording", &PerformanceMonitor::startRecording)
            .def("stop_recording", &PerformanceMonitor::stopRecording)
            .def("record_step", &PerformanceMonitor::recordStep,
                 py::arg("firing_rate"), py::arg("reward"), py::arg("novelty") = 0.0f, py::arg("curiosity") = 0.0f)
            .def("calculate_average_firing_rate", &PerformanceMonitor::calculateAverageFiringRate)
            .def("calculate_average_reward", &PerformanceMonitor::calculateAverageReward)
            .def("generate_report", &PerformanceMonitor::generateReport);
    }
};

// Component Factory for Extensibility
class ComponentFactory {
private:
    struct ComponentInfo {
        std::string name;
        std::string version;
        std::function<void(std::shared_ptr<Brain>&)> initializer;
        std::map<std::string, std::function<void(std::shared_ptr<Brain>&, const ConfigValue&)>> configurators;
        std::map<std::string, std::function<std::shared_ptr<void>(std::shared_ptr<Brain>&)>> creators;
    };
    
    std::vector<ComponentInfo> components;
    
public:
    // Component registration
    void registerComponent(const std::string& name, const std::string& version,
                          std::function<void(std::shared_ptr<Brain>&)> initializer = nullptr,
                          std::map<std::string, std::function<void(std::shared_ptr<Brain>&, const ConfigValue&)>> configurators = {},
                          std::map<std::string, std::function<std::shared_ptr<void>(std::shared_ptr<Brain>&)>> creators = {}) {
        ComponentInfo info;
        info.name = name;
        info.version = version;
        info.initializer = initializer;
        info.configurators = configurators;
        info.creators = creators;
        components.push_back(info);
    }
    
    // Component usage
    bool initializeComponent(std::shared_ptr<Brain>& brain, const std::string& componentName) {
        auto it = std::find_if(components.begin(), components.end(),
                              [&componentName](const ComponentInfo& c) {
                                  return c.name == componentName;
                              });
        if (it == components.end()) {
            return false;
        }
        
        if (it->initializer) {
            it->initializer(brain);
        }
        
        return true;
    }
    
    bool configureComponent(std::shared_ptr<Brain>& brain, const std::string& componentName,
                           const std::string& configKey, const ConfigValue& configValue) {
        auto it = std::find_if(components.begin(), components.end(),
                              [&componentName](const ComponentInfo& c) {
                                  return c.name == componentName;
                              });
        if (it == components.end()) {
            return false;
        }
        
        auto configIt = it->configurators.find(configKey);
        if (configIt != it->configurators.end()) {
            configIt->second(brain, configValue);
            return true;
        }
        
        return false;
    }
    
    std::shared_ptr<void> createComponent(std::shared_ptr<Brain>& brain, const std::string& componentName,
                                          const std::string& instanceName) {
        auto it = std::find_if(components.begin(), components.end(),
                              [&componentName](const ComponentInfo& c) {
                                  return c.name == componentName;
                              });
        if (it == components.end()) {
            return nullptr;
        }
        
        auto creatorIt = it->creators.find(instanceName);
        if (creatorIt != it->creators.end()) {
            return creatorIt->second(brain);
        }
        
        return nullptr;
    }
    
    // Get registered components
    std::vector<std::string> getRegisteredComponents() const {
        std::vector<std::string> names;
        for (const auto& component : components) {
            names.push_back(component.name);
        }
        return names;
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        py::class_<ComponentFactory>(m, "ComponentFactory")
            .def(py::init<>())
            .def("register_component", &ComponentFactory::registerComponent,
                 py::arg("name"), py::arg("version"), py::arg("initializer") = py::cpp_function(nullptr),
                 py::arg("configurators") = std::map<std::string, std::function<void(std::shared_ptr<Brain>&, const ConfigValue&)>>(),
                 py::arg("creators") = std::map<std::string, std::function<std::shared_ptr<void>(std::shared_ptr<Brain>&)>>())
            .def("initialize_component", &ComponentFactory::initializeComponent,
                 py::arg("brain"), py::arg("component_name"))
            .def("configure_component", &ComponentFactory::configureComponent,
                 py::arg("brain"), py::arg("component_name"), py::arg("config_key"), py::arg("config_value"))
            .def("create_component", &ComponentFactory::createComponent,
                 py::arg("brain"), py::arg("component_name"), py::arg("instance_name"))
            .def("get_registered_components", &ComponentFactory::getRegisteredComponents);
    }
};

// Analysis Utilities
class AnalysisUtils {
public:
    // Neural activity analysis
    static std::vector<float> analyzeFiringPattern(std::shared_ptr<Brain> brain, size_t steps) {
        std::vector<float> firingRates;
        for (size_t i = 0; i < steps; ++i) {
            if (brain) {
                brain->step(i);
                firingRates.push_back(brain->getAverageFiringRate());
            }
        }
        return firingRates;
    }
    
    static float calculateFiringRateTrend(const std::vector<float>& firingRates) {
        if (firingRates.size() < 2) return 0.0f;
        
        // Calculate linear regression slope
        float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
        size_t n = firingRates.size();
        
        for (size_t i = 0; i < n; ++i) {
            float x = static_cast<float>(i);
            float y = firingRates[i];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }
        
        float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        return slope;
    }
    
    // Learning analysis
    static std::map<std::string, float> analyzeLearningProgress(const std::vector<float>& rewards,
                                                                 const std::vector<float>& firingRates,
                                                                 const std::vector<float>& noveltyLevels) {
        std::map<std::string, float> analysis;
        
        if (rewards.size() < 2) {
            return analysis;
        }
        
        // Calculate reward trend
        float rewardTrend = calculateFiringRateTrend(rewards);
        
        // Calculate firing rate trend
        float firingTrend = calculateFiringRateTrend(firingRates);
        
        // Calculate coherence between reward and firing
        float coherence = 0.0f;
        if (rewards.size() == firingRates.size()) {
            float sumXY = 0.0f, sumX = 0.0f, sumY = 0.0f, sumX2 = 0.0f, sumY2 = 0.0f;
            size_t n = rewards.size();
            
            for (size_t i = 0; i < n; ++i) {
                float x = rewards[i];
                float y = firingRates[i];
                sumXY += x * y;
                sumX += x;
                sumY += y;
                sumX2 += x * x;
                sumY2 += y * y;
            }
            
            float numerator = n * sumXY - sumX * sumY;
            float denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
            if (denominator > 0.0f) {
                coherence = numerator / denominator;
            }
        }
        
        analysis["reward_trend"] = rewardTrend;
        analysis["firing_trend"] = firingTrend;
        analysis["reward_firing_coherence"] = coherence;
        
        return analysis;
    }
    
    // Statistical utilities
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float val : values) sum += val;
        return sum / values.size();
    }
    
    static float calculateStdDev(const std::vector<float>& values, float mean) {
        if (values.size() <= 1) return 0.0f;
        
        float variance = 0.0f;
        for (float val : values) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= values.size();
        
        return std::sqrt(variance);
    }
    
    static std::string generateAnalysisReport(const std::map<std::string, float>& analysis) {
        std::ostringstream oss;
        oss << "=== NLM Analysis Report ===\n";
        for (const auto& pair : analysis) {
            oss << pair.first << ": " << pair.second << "\n";
        }
        return oss.str();
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        m.def("analyze_firing_pattern", &AnalysisUtils::analyzeFiringPattern,
              py::arg("brain"), py::arg("steps"));
        m.def("calculate_firing_rate_trend", &AnalysisUtils::calculateFiringRateTrend,
              py::arg("firing_rates"));
        m.def("analyze_learning_progress", &AnalysisUtils::analyzeLearningProgress,
              py::arg("rewards"), py::arg("firing_rates"), py::arg("novelty_levels"));
        m.def("calculate_mean", &AnalysisUtils::calculateMean,
              py::arg("values"));
        m.def("calculate_std_dev", &AnalysisUtils::calculateStdDev,
              py::arg("values"), py::arg("mean"));
        m.def("generate_analysis_report", &AnalysisUtils::generateAnalysisReport,
              py::arg("analysis"));
    }
};

// Neural Network Builder
class NeuralNetworkBuilder {
public:
    // Basic neural network creation
    static std::shared_ptr<Brain> createBasicNetwork(size_t neuronCount, size_t regionCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        return nullptr;
    }
    
    // Specialized network types
    static std::shared_ptr<Brain> createSensoryMotorNetwork(size_t sensoryNeurons, 
                                                            size_t motorNeurons,
                                                            size_t internalNeurons) {
        auto config = std::make_shared<Config>();
        size_t totalNeurons = sensoryNeurons + motorNeurons + internalNeurons;
        config->set("neuron_count", static_cast<int64_t>(totalNeurons));
        config->set("region_count", 3);
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        return nullptr;
    }
    
    // Development-aware network
    static std::shared_ptr<Brain> createDevelopmentalNetwork(size_t baseNeuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(baseNeuronCount));
        config->set("region_count", 2);
        config->set("connection_probability", 0.12f);
        config->set("synaptogenesis_rate", 0.001f);
        config->set("pruning_rate", 0.0001f);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        return nullptr;
    }
    
    // Export to Python
    void exportToPython(py::module& m) {
        m.def("create_basic_network", &NeuralNetworkBuilder::createBasicNetwork,
              py::arg("neuron_count"), py::arg("region_count"));
        m.def("create_sensory_motor_network", &NeuralNetworkBuilder::createSensoryMotorNetwork,
              py::arg("sensory_neurons"), py::arg("motor_neurons"), py::arg("internal_neurons"));
        m.def("create_developmental_network", &NeuralNetworkBuilder::createDevelopmentalNetwork,
              py::arg("base_neuron_count"));
    }
};

// Main enhanced bindings
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        
        This is an enhanced version of the NLM Python bindings with advanced features:
        
        1. Validated Configuration System
           - Type-safe parameter validation
           - Strict mode for production safety
           - Batch configuration setting
        
        2. Advanced Simulation Manager
           - Protocol-based simulation control
           - Event-driven execution
           - Phase marker support
        
        3. Performance Monitoring
           - Real-time metrics collection
           - Statistical analysis
           - Historical tracking
        
        4. Component Factory for Extensibility
           - Plugin architecture
           - Component registration
           - Dynamic component creation
        
        5. Analysis Utilities
           - Neural activity analysis
           - Learning progress tracking
           - Statistical computations
        
        6. Neural Network Builder
           - Pre-built network templates
           - Specialized network types
           - Developmental considerations
        
        This enhanced API is designed for advanced users and research applications.
    )pbdoc";
    
    // Register enhanced classes
    ValidatedConfig().exportToPython(m);
    AdvancedSimulationManager().exportToPython(m);
    PerformanceMonitor().exportToPython(m);
    ComponentFactory().exportToPython(m);
    AnalysisUtils().exportToPython(m);
    NeuralNetworkBuilder().exportToPython(m);
    
    // Include original bindings for compatibility
    py::module original_nlm = py::module::import("pynlm");
    
    // Copy essential classes from original module
    m.attr("Config") = original_nlm.attr("Config");
    m.attr("Brain") = original_nlm.attr("Brain");
    m.attr("AgentBrain") = original_nlm.attr("AgentBrain");
    m.attr("SimpleWorld") = original_nlm.attr("SimpleWorld");
    m.attr("SensoryInput") = original_nlm.attr("SensoryInput");
    m.attr("Vision") = original_nlm.attr("Vision");
    m.attr("Audio") = original_nlm.attr("Audio");
    m.attr("InternalSignals") = original_nlm.attr("InternalSignals");
    m.attr("Action") = original_nlm.attr("Action");
    m.attr("WorldObject") = original_nlm.attr("WorldObject");
    m.attr("AgentBody") = original_nlm.attr("AgentBody");
    m.attr("ActionResult") = original_nlm.attr("ActionResult");
    m.attr("SensoryPercept") = original_nlm.attr("SensoryPercept");
    
    // Copy enumerations
    m.attr("NeuronType") = original_nlm.attr("NeuronType");
    m.attr("SynapseType") = original_nlm.attr("SynapseType");
    m.attr("DevelopmentalStage") = original_nlm.attr("DevelopmentalStage");
    m.attr("FiringState") = original_nlm.attr("FiringState");
    m.attr("ActionType") = original_nlm.attr("ActionType");
    m.attr("MotorCommand") = original_nlm.attr("MotorCommand");
    m.attr("WorldObjectType") = original_nlm.attr("WorldObjectType");
    
    // Copy factory functions
    m.def("createDefaultConfig", original_nlm.attr("createDefaultConfig"));
    m.def("createBrain", original_nlm.attr("createBrain"));
    m.def("createSimpleWorld", original_nlm.attr("createSimpleWorld"));
    m.def("createAgentBrain", original_nlm.attr("createAgentBrain"));
    
    // Copy constants
    m.attr("INVALID_NEURON_ID") = original_nlm.attr("INVALID_NEURON_ID");
    m.attr("INVALID_SYNAPSE_ID") = original_nlm.attr("INVALID_SYNAPSE_ID");
    m.attr("INVALID_REGION_ID") = original_nlm.attr("INVALID_REGION_ID");
    m.attr("INVALID_POPULATION_ID") = original_nlm.attr("INVALID_POPULATION_ID");
}

} // namespace nlm
