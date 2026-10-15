#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <variant>
#include <optional>

// Include all the core NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

// Include performance-optimized headers
#include "../src/performance/OptimizedBrain.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/SIMDVectorization.hpp"
#include "../src/performance/SparseConnectivity.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/performance/Performance.hpp"

namespace py = pybind11;
namespace nlm {

// Python exception classes
class PyRuntimeError : public std::runtime_error {
public:
    explicit PyRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyValueError : public std::runtime_error {
public:
    explicit PyValueError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyTypeError : public std::runtime_error {
public:
    explicit PyTypeError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyKeyError : public std::runtime_error {
public:
    explicit PyKeyError(const std::string& msg) : std::runtime_error(msg) {}
};

// Configuration preset structures
struct DefaultConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, std::variant<int, double, bool, std::string>> parameters;
};

struct DevelopmentConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, std::variant<int, double, bool, std::string>> parameters;
};

struct MinimalConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, std::variant<int, double, bool, std::string>> parameters;
};

// Helper functions
namespace ConfigHelpers {
    std::map<std::string, std::variant<int, double, bool, std::string>> 
    getDefaultConfigParameters() {
        std::map<std::string, std::variant<int, double, bool, std::string>> params;
        params["brain.neuronCount"] = 1000;
        params["brain.regionCount"] = 5;
        params["brain.learningRate"] = 0.1;
        params["brain.plasticityEnabled"] = true;
        params["world.width"] = 100.0;
        params["world.height"] = 100.0;
        params["world.energyDecayRate"] = 0.01;
        params["agent.curiosityLevel"] = 0.5;
        params["agent.rewardWeight"] = 1.0;
        return params;
    }
    
    std::map<std::string, std::variant<int, double, bool, std::string>> 
    getDevelopmentConfigParameters() {
        std::map<std::string, std::variant<int, double, bool, std::string>> params;
        params["brain.neuronCount"] = 2000;
        params["brain.regionCount"] = 8;
        params["brain.learningRate"] = 0.05;
        params["brain.plasticityEnabled"] = true;
        params["world.width"] = 200.0;
        params["world.height"] = 200.0;
        params["world.energyDecayRate"] = 0.005;
        params["agent.curiosityLevel"] = 1.0;
        params["agent.rewardWeight"] = 1.5;
        params["agent.enableDevelopment"] = true;
        params["brain.enableStructuralPlasticity"] = true;
        return params;
    }
    
    std::map<std::string, std::variant<int, double, bool, std::string>> 
    getMinimalConfigParameters() {
        std::map<std::string, std::variant<int, double, bool, std::string>> params;
        params["brain.neuronCount"] = 500;
        params["brain.regionCount"] = 2;
        params["brain.learningRate"] = 0.2;
        params["brain.plasticityEnabled"] = false;
        params["world.width"] = 50.0;
        params["world.height"] = 50.0;
        params["world.energyDecayRate"] = 0.02;
        return params;
    }
}

// Context manager support for Brain
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    bool autoReset;
public:
    BrainContextManager(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w, 
                       std::shared_ptr<AgentBrain> a, bool autoReset = true)
        : brain(b), world(w), agent(a), autoReset(autoReset) {}
    
    ~BrainContextManager() {
        if (autoReset) {
            brain->reset();
            if (world) world->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() { return brain; }
    std::shared_ptr<SimpleWorld> getWorld() { return world; }
    std::shared_ptr<AgentBrain> getAgent() { return agent; }
    
    void disableAutoReset() { autoReset = false; }
};

// Statistical aggregation methods
class BrainStats {
private:
    std::shared_ptr<Brain> brain;
    std::vector<std::pair<std::string, double>> metrics;
    SimulationStep currentStep;
    Timestamp currentTime;
public:
    BrainStats(std::shared_ptr<Brain> b, SimulationStep step = 0, Timestamp time = 0)
        : brain(b), currentStep(step), currentTime(time) {
        updateMetrics();
    }
    
    void updateMetrics() {
        metrics.clear();
        metrics.push_back({"neuronCount", static_cast<double>(brain->getTotalNeuronCount())});
        metrics.push_back({"synapseCount", static_cast<double>(brain->getTotalSynapseCount())});
        metrics.push_back({"activeNeuronCount", static_cast<double>(brain->getActiveNeuronCount())});
        metrics.push_back({"firingNeuronCount", static_cast<double>(brain->getFiringNeuronCount())});
        metrics.push_back({"averageFiringRate", brain->getAverageFiringRate()});
        metrics.push_back({"excitationInhibitionRatio", brain->getExcitationInhibitionRatio()});
        metrics.push_back({"totalSpikeCount", static_cast<double>(brain->getTotalSpikeCount())});
        metrics.push_back({"developmentStage", static_cast<double>(static_cast<int>(brain->getDevelopmentalStage()))});
    }
    
    double getMetric(const std::string& name) const {
        for (const auto& metric : metrics) {
            if (metric.first == name) {
                return metric.second;
            }
        }
        throw PyKeyError("Metric not found: " + name);
    }
    
    std::vector<std::string> getMetricNames() const {
        std::vector<std::string> names;
        for (const auto& metric : metrics) {
            names.push_back(metric.first);
        }
        return names;
    }
    
    std::map<std::string, double> getAllMetrics() const {
        std::map<std::string, double> result;
        for (const auto& metric : metrics) {
            result[metric.first] = metric.second;
        }
        return result;
    }
    
    std::string toString() const {
        std::string result = "Brain Statistics (Step " + std::to_string(currentStep) + "):\n";
        for (const auto& metric : metrics) {
            result += "  " + metric.first + ": " + std::to_string(metric.second) + "\n";
        }
        return result;
    }
    
    std::string summary() const {
        return "Neuron Count: " + std::to_string(brain->getTotalNeuronCount()) +
               ", Synapse Count: " + std::to_string(brain->getTotalSynapseCount()) +
               ", Firing Rate: " + std::to_string(brain->getAverageFiringRate()) +
               ", Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
    }
};

// Experiment setup structure
class ExperimentSetup {
private:
    std::shared_ptr<Experiment> experiment;
    std::shared_ptr<ExperimentRunner> runner;
    std::string experimentName;
public:
    ExperimentSetup(const std::string& name) : experimentName(name) {
        runner = std::make_shared<ExperimentRunner>();
        experiment = runner->createExperiment(name);
    }
    
    std::shared_ptr<Experiment> getExperiment() { return experiment; }
    std::shared_ptr<ExperimentRunner> getRunner() { return runner; }
    
    void setExperimentName(const std::string& name) {
        experimentName = name;
        experiment->setName(name);
    }
    
    std::string getExperimentName() const { return experimentName; }
    
    void setSeed(uint64_t seed) {
        experiment->setSeed(seed);
    }
    
    void addConfigValue(const std::string& key, const std::string& value) {
        experiment->setConfigValue(key, value);
    }
    
    void recordMetric(const std::string& name, double value) {
        experiment->recordMetric(name, value);
    }
    
    std::string getExperimentSummary() const {
        return "Experiment: " + experimentName + ", Seed: " + std::to_string(experiment->getSeed());
    }
};

// Visualization helpers
namespace VisualizationHelpers {
    std::string generateNeuralNetworkLayout(const std::shared_ptr<Brain>& brain) {
        std::string result = "Neural Network Layout:\n";
        result += "Total Regions: " + std::to_string(brain->getRegionCount()) + "\n";
        
        for (const auto& region : brain->getRegions()) {
            result += "  Region ID: " + std::to_string(region->getId().value) + "\n";
            result += "    Neuron Count: " + std::to_string(region->getNeuronCount()) + "\n";
            result += "    Synapse Count: " + std::to_string(region->getSynapseCount()) + "\n";
        }
        
        return result;
    }
    
    std::string generateBrainActivityHeatmap(const std::shared_ptr<Brain>& brain) {
        std::string result = "Brain Activity Heatmap:\n";
        result += "Active Neurons: " + std::to_string(brain->getActiveNeuronCount()) + "\n";
        result += "Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
        
        if (brain->getTotalNeuronCount() > 0) {
            double firingRate = static_cast<double>(brain->getFiringNeuronCount()) / 
                               brain->getTotalNeuronCount() * 100.0;
            result += "Firing Rate: " + std::to_string(firingRate) + "%\n";
        }
        
        return result;
    }
    
    std::string generateDevelopmentTimeline(const std::shared_ptr<Brain>& brain) {
        std::string result = "Development Timeline:\n";
        result += "Current Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) + "\n";
        result += "Brain Age: " + std::to_string(brain->getTotalSpikeCount()) + " spikes\n";
        
        // Add stage information
        switch (brain->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial:
                result += "Stage Description: Initial - Basic neural formation\n";
                break;
            case DevelopmentalStage::CriticalPeriod:
                result += "Stage Description: Critical Period - High plasticity, rapid learning\n";
                break;
            case DevelopmentalStage::Maturation:
                result += "Stage Description: Maturation - Refined connections, stabilization\n";
                break;
            case DevelopmentalStage::Adult:
                result += "Stage Description: Adult - Optimized performance, maintenance\n";
                break;
            case DevelopmentalStage::Aging:
                result += "Stage Description: Aging - Gradual decline, adaptation\n";
                break;
        }
        
        return result;
    }
}

// Performance profiling helpers
class PerformanceProfiler {
private:
    struct PerformanceMetrics {
        double initializationTime = 0.0;
        double stepTime = 0.0;
        double memoryUsage = 0.0;
        double cpuUsage = 0.0;
        std::vector<double> stepTimes;
        size_t totalSteps = 0;
    };
    
    PerformanceMetrics metrics;
    std::chrono::steady_clock::time_point startTime;
public:
    PerformanceProfiler() {
        startTime = std::chrono::steady_clock::now();
    }
    
    void startMeasurement() {
        startTime = std::chrono::steady_clock::now();
    }
    
    void recordInitializationTime() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        metrics.initializationTime = duration.count();
    }
    
    void recordStepTime() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        double stepTime = duration.count();
        metrics.stepTimes.push_back(stepTime);
        metrics.stepTime = stepTime;
        metrics.totalSteps++;
        startTime = now;
    }
    
    double getAverageStepTime() const {
        if (metrics.stepTimes.empty()) return 0.0;
        double sum = 0.0;
        for (double time : metrics.stepTimes) {
            sum += time;
        }
        return sum / metrics.stepTimes.size();
    }
    
    double getMaxStepTime() const {
        if (metrics.stepTimes.empty()) return 0.0;
        double max = 0.0;
        for (double time : metrics.stepTimes) {
            if (time > max) max = time;
        }
        return max;
    }
    
    double getMinStepTime() const {
        if (metrics.stepTimes.empty()) return 0.0;
        double min = metrics.stepTimes[0];
        for (double time : metrics.stepTimes) {
            if (time < min) min = time;
        }
        return min;
    }
    
    size_t getTotalSteps() const { return metrics.totalSteps; }
    
    std::string getPerformanceReport() const {
        std::string result = "Performance Report:\n";
        result += "Initialization Time: " + std::to_string(metrics.initializationTime) + " ms\n";
        result += "Average Step Time: " + std::to_string(getAverageStepTime()) + " ms\n";
        result += "Max Step Time: " + std::to_string(getMaxStepTime()) + " ms\n";
        result += "Min Step Time: " + std::to_string(getMinStepTime()) + " ms\n";
        result += "Total Steps: " + std::to_string(metrics.totalSteps) + "\n";
        
        if (metrics.totalSteps > 0) {
            double avg = getAverageStepTime();
            if (avg > 0) {
                result += "Steps per Second: " + std::to_string(1000.0 / avg) + "\n";
            }
        }
        
        return result;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides enhanced convenience functions, error handling, and Pythonic API patterns.
    )pbdoc";

    // Register Python exceptions
    py::register_exception<PyRuntimeError>(m, "PyRuntimeError");
    py::register_exception<PyValueError>(m, "PyValueError");
    py::register_exception<PyTypeError>(m, "PyTypeError");
    py::register_exception<PyKeyError>(m, "PyKeyError");

    // Basic types and enums (unchanged from original)
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
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

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

    // Enhanced Config class with JSON support and validation
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with JSON support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromJSON", [](Config& self, const std::string& jsonStr) {
            // Simple JSON parsing for demonstration
            // In production, this would use a proper JSON library
            if (jsonStr.empty()) {
                throw PyValueError("Empty JSON string");
            }
            return self.loadFromFile(jsonStr); // For demo, reuse file loading
        }, py::arg("json_str"), "Load configuration from JSON string")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToJSON", [](const Config& self) {
            // Convert config to JSON representation
            std::string json = "{\n";
            auto keys = self.getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                json += "  \"" + keys[i] + "\": \"" + std::to_string(i) + "\"";
                if (i < keys.size() - 1) json += ",\n";
            }
            json += "\n}";
            return json;
        }, "Convert configuration to JSON string")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw PyKeyError("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get configuration value as string")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get configuration value as int with default")
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0.0,
           "Get configuration value as double with default")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = false,
           "Get configuration value as bool with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as int")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as double")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as bool")
        .def("validate", [](const Config& self) {
            // Validate configuration parameters
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find('.') == std::string::npos) {
                    throw PyValueError("Invalid configuration key format: " + key);
                }
            }
            return true;
        }, "Validate configuration keys")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Convenience Functions
    
    // Configuration presets
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set default values
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 5);
        config->set("brain.learningRate", 0.1);
        config->set("brain.plasticityEnabled", true);
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("world.energyDecayRate", 0.01);
        config->set("agent.curiosityLevel", 0.5);
        config->set("agent.rewardWeight", 1.0);
        return config;
    }, "Create a default configuration with preset values");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Development preset with more resources
        config->set("brain.neuronCount", 2000);
        config->set("brain.regionCount", 8);
        config->set("brain.learningRate", 0.05);
        config->set("brain.plasticityEnabled", true);
        config->set("world.width", 200.0);
        config->set("world.height", 200.0);
        config->set("world.energyDecayRate", 0.005);
        config->set("agent.curiosityLevel", 1.0);
        config->set("agent.rewardWeight", 1.5);
        config->set("agent.enableDevelopment", true);
        config->set("brain.enableStructuralPlasticity", true);
        return config;
    }, "Create a development configuration with enhanced capabilities");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Minimal preset for lightweight simulation
        config->set("brain.neuronCount", 500);
        config->set("brain.regionCount", 2);
        config->set("brain.learningRate", 0.2);
        config->set("brain.plasticityEnabled", false);
        config->set("world.width", 50.0);
        config->set("world.height", 50.0);
        config->set("world.energyDecayRate", 0.02);
        return config;
    }, "Create a minimal configuration for lightweight simulation");

    m.def("loadConfigFromJSON", [](const std::string& jsonStr) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // For now, reuse the file loading with a temporary file
        // In production, use a proper JSON library
        if (jsonStr.empty()) {
            throw PyValueError("Empty JSON string");
        }
        // Create a temporary file for demonstration
        std::string tempFile = "/tmp/temp_config.json";
        std::ofstream file(tempFile);
        if (!file) {
            throw PyRuntimeError("Cannot create temporary file for JSON");
        }
        file << jsonStr;
        file.close();
        
        if (!config->loadFromFile(tempFile)) {
            throw PyRuntimeError("Failed to load configuration from JSON");
        }
        
        // Clean up
        std::remove(tempFile.c_str());
        return config;
    }, py::arg("json_str"), "Load configuration from JSON string");

    // Enhanced Brain with context manager support
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        }, "Enter context manager")
        .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
            self->reset();
        }, "Exit context manager, automatically reset brain")
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
             "Log brain status")
        // Serialization helpers
        .def("saveToFile", [](const std::shared_ptr<Brain>& self, const std::string& filepath) {
            return self->save(filepath);
        }, py::arg("filepath"), "Save brain state to file")
        .def("loadFromFile", [](std::shared_ptr<Brain> self, const std::string& filepath) {
            return self->load(filepath);
        }, py::arg("filepath"), "Load brain state from file")
        .def("toDict", [](const std::shared_ptr<Brain>& self) {
            py::dict result;
            result["neuronCount"] = self->getTotalNeuronCount();
            result["synapseCount"] = self->getTotalSynapseCount();
            result["activeNeuronCount"] = self->getActiveNeuronCount();
            result["firingNeuronCount"] = self->getFiringNeuronCount();
            result["averageFiringRate"] = self->getAverageFiringRate();
            result["excitationInhibitionRatio"] = self->getExcitationInhibitionRatio();
            result["totalSpikeCount"] = self->getTotalSpikeCount();
            result["developmentalStage"] = static_cast<int>(self->getDevelopmentalStage());
            return result;
        }, "Convert brain to dictionary representation")
        .def("fromDict", [](std::shared_ptr<Brain> self, const py::dict& dict) {
            // For now, just log the conversion
            std::cout << "Brain fromDict: Loading from dictionary (not yet implemented)" << std::endl;
            return self;
        }, py::arg("dict"), "Create brain from dictionary (not yet implemented)");

    // Convenience functions for creating default objects
    
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Set default values
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 5);
        config->set("brain.learningRate", 0.1);
        config->set("brain.plasticityEnabled", true);
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("world.energyDecayRate", 0.01);
        config->set("agent.curiosityLevel", 0.5);
        config->set("agent.rewardWeight", 1.0);
        
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, "Create a brain with default configuration");

    m.def("runCompleteEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                std::shared_ptr<AgentBrain> agent, SimulationStep maxSteps,
                                bool enableLearning = true) {
        if (!brain || !world || !agent) {
            throw PyValueError("Brain, world, and agent must be provided");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation if enabled
            if (enableLearning) {
                auto result = world->getLastActionResult();
                if (result) {
                    agent->applyRewardModulation(result->reward, 0.0);
                }
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("max_steps"),
           py::arg("enable_learning") = true,
           "Run a complete episode simulation");

    m.def("getBrainStats", [](const std::shared_ptr<Brain>& brain, SimulationStep step = 0, 
                           Timestamp time = 0) -> std::map<std::string, double> {
        BrainStats stats(brain, step, time);
        return stats.getAllMetrics();
    }, py::arg("brain"), py::arg("step") = 0, py::arg("time") = 0,
       "Get comprehensive brain statistics");

    m.def("createExperimentSetup", [](const std::string& name) {
        return std::make_shared<ExperimentSetup>(name);
    }, py::arg("name"), "Create an experiment setup");

    // Enhanced visualization utilities
    m.def("generateNeuralNetworkLayout", &VisualizationHelpers::generateNeuralNetworkLayout,
          py::arg("brain"), "Generate neural network layout visualization");
    
    m.def("generateBrainActivityHeatmap", &VisualizationHelpers::generateBrainActivityHeatmap,
          py::arg("brain"), "Generate brain activity heatmap visualization");
    
    m.def("generateDevelopmentTimeline", &VisualizationHelpers::generateDevelopmentTimeline,
          py::arg("brain"), "Generate development timeline visualization");

    // Performance profiling helpers
    m.def("createPerformanceProfiler", []() {
        return std::make_shared<PerformanceProfiler>();
    }, "Create a performance profiler");

    // Performance Statistics (unchanged from original)
    py::class_<BrainStats>(m, "BrainStats", R"pbdoc(Statistical aggregation for brain metrics)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, SimulationStep, Timestamp>(),
             py::arg("brain"), py::arg("step") = 0, py::arg("time") = 0)
        .def("getMetric", &BrainStats::getMetric, py::arg("name"),
             "Get a specific metric value")
        .def("getMetricNames", &BrainStats::getMetricNames,
             "Get all metric names")
        .def("getAllMetrics", &BrainStats::getAllMetrics,
             "Get all metrics as dictionary")
        .def("toString", &BrainStats::toString,
             "Get string representation")
        .def("summary", &BrainStats::summary,
             "Get concise summary");

    // OptimizedBrain - High-performance neural simulation brain with event-driven processing, SoA format, sparse connectivity, multithreading, and SIMD
    py::class_<nlm::OptimizedBrain, std::shared_ptr<nlm::OptimizedBrain>>(m, "OptimizedBrain",
        R"pbdoc(High-performance neural simulation brain with extensive optimizations for large-scale simulations)
        
        This brain class implements several performance optimizations:
        - Event-driven processing: only active neurons consume computation
        - Structure-of-Arrays (SoA) format: better cache utilization and SIMD
        - Sparse connectivity: adjacency list representation for large networks
        - Multithreading: parallel population updates with work-stealing
        - SIMD vectorization: automatic SIMD instruction generation
        
        OptimizedBrain inherits from Brain and provides the same interface but with enhanced performance.
        All optimizations are configurable via the enable/set methods.
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create OptimizedBrain with configuration")
        .def("initialize", &nlm::OptimizedBrain::initialize,
             "Initialize the optimized brain")
        .def("step", static_cast<void (nlm::OptimizedBrain::*)(SimulationStep)>(&nlm::OptimizedBrain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (nlm::OptimizedBrain::*)(SimulationStep, Timestamp)>(&nlm::OptimizedBrain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("getPerformanceStats", &nlm::OptimizedBrain::getPerformanceStats,
             "Get performance statistics for the simulation")
        // Control over optimizations
        .def("enableEventDriven", &nlm::OptimizedBrain::enableEventDriven, py::arg("enable"),
             "Enable/disable event-driven processing")
        .def("enableMultithreading", &nlm::OptimizedBrain::enableMultithreading, py::arg("enable"),
             "Enable/disable multithreading")
        .def("enableSIMD", &nlm::OptimizedBrain::enableSIMD, py::arg("enable"),
             "Enable/disable SIMD vectorization")
        .def("enableSparseConnectivity", &nlm::OptimizedBrain::enableSparseConnectivity, py::arg("enable"),
             "Enable/disable sparse connectivity")
        .def("isEventDrivenEnabled", &nlm::OptimizedBrain::isEventDrivenEnabled,
             "Check if event-driven processing is enabled")
        .def("isMultithreadingEnabled", &nlm::OptimizedBrain::isMultithreadingEnabled,
             "Check if multithreading is enabled")
        .def("isSIMDEnabled", &nlm::OptimizedBrain::isSIMDEnabled,
             "Check if SIMD vectorization is enabled")
        .def("isSparseEnabled", &nlm::OptimizedBrain::isSparseEnabled,
             "Check if sparse connectivity is enabled")
        // Thread count configuration
        .def("setThreadCount", &nlm::OptimizedBrain::setThreadCount, py::arg("count"),
             "Set number of threads for parallel processing")
        .def("getThreadCount", &nlm::OptimizedBrain::getThreadCount,
             "Get current thread count")
        // Memory usage reporting
        .def("memoryUsage", [](const nlm::OptimizedBrain& self) {
            return self.neuralState_.memoryUsage() + self.synapseState_.memoryUsage();
        }, "Get total memory usage in bytes")
        .def("getNeuralStateMemory", [](const nlm::OptimizedBrain& self) {
            return self.neuralState_.memoryUsage();
        }, "Get neural state memory usage")
        .def("getSynapseStateMemory", [](const nlm::OptimizedBrain& self) {
            return self.synapseState_.memoryUsage();
        }, "Get synapse state memory usage")
        // Performance monitoring (using existing BrainStats pattern)
        .def("getNeuronCount", &nlm::OptimizedBrain::getTotalNeuronCount,
             "Get total neuron count")
        .def("getSynapseCount", &nlm::OptimizedBrain::getTotalSynapseCount,
             "Get total synapse count")
        .def("getActiveNeuronCount", &nlm::OptimizedBrain::getActiveNeuronCount,
             "Get active neuron count")
        .def("getFiringNeuronCount", &nlm::OptimizedBrain::getFiringNeuronCount,
             "Get firing neuron count")
        .def("getAverageFiringRate", &nlm::OptimizedBrain::getAverageFiringRate,
             "Get average firing rate")
        .def("getExcitationInhibitionRatio", &nlm::OptimizedBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &nlm::OptimizedBrain::getTotalSpikeCount,
             "Get total spike count")
        // For backward compatibility, expose some Brain methods
        .def("addRegion", &nlm::OptimizedBrain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::OptimizedBrain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &nlm::OptimizedBrain::getRegionCount,
             "Get the number of regions")
        .def("getRegions", &nlm::OptimizedBrain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getRegionIds", &nlm::OptimizedBrain::getRegionIds,
             "Get all region IDs")
        .def("logStatus", &nlm::OptimizedBrain::logStatus,
             "Log brain status")
        // Serialization helpers
        .def("save", &nlm::OptimizedBrain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::OptimizedBrain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("getConfig", &nlm::OptimizedBrain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("reset", &nlm::OptimizedBrain::reset,
             "Reset brain state")
        // Pythonic properties for convenience
        .def_readwrite("event_driven_enabled", &nlm::OptimizedBrain::eventDrivenEnabled_,
                      "Event-driven processing enabled state")
        .def_readwrite("multithreading_enabled", &nlm::OptimizedBrain::multithreadingEnabled_,
                      "Multithreading enabled state")
        .def_readwrite("simd_enabled", &nlm::OptimizedBrain::simdEnabled_,
                      "SIMD vectorization enabled state")
        .def_readwrite("sparse_enabled", &nlm::OptimizedBrain::sparseEnabled_,
                      "Sparse connectivity enabled state")
        .def_readwrite("thread_count", &nlm::OptimizedBrain::threadCount_,
                      "Thread count for parallel processing")
        .def_property("performance_stats", &nlm::OptimizedBrain::getPerformanceStats,
                     "Performance statistics")
        .def("to_dict", [](const std::shared_ptr<nlm::OptimizedBrain>& self) {
            py::dict result;
            result["neuron_count"] = self->getTotalNeuronCount();
            result["synapse_count"] = self->getTotalSynapseCount();
            result["active_neuron_count"] = self->getActiveNeuronCount();
            result["firing_neuron_count"] = self->getFiringNeuronCount();
            result["average_firing_rate"] = self->getAverageFiringRate();
            result["excitation_inhibition_ratio"] = self->getExcitationInhibitionRatio();
            result["total_spike_count"] = self->getTotalSpikeCount();
            result["event_driven_enabled"] = self->isEventDrivenEnabled();
            result["multithreading_enabled"] = self->isMultithreadingEnabled();
            result["simd_enabled"] = self->isSIMDEnabled();
            result["sparse_enabled"] = self->isSparseEnabled();
            result["thread_count"] = self->getThreadCount();
            
            auto perfStats = self->getPerformanceStats();
            result["neurons_per_second"] = perfStats.neuronsPerSecond;
            result["spikes_per_second"] = perfStats.spikesPerSecond;
            result["synaptic_events_per_second"] = perfStats.synapticEventsPerSecond;
            result["real_time_factor"] = perfStats.realTimeFactor;
            result["pending_spikes"] = perfStats.pendingSpikes;
            result["pending_delayed_spikes"] = perfStats.pendingDelayedSpikes;
            result["avg_firing_rate"] = perfStats.avgFiringRate;
            result["memory_usage_mb"] = perfStats.memoryUsageMB;
            result["step_time_ms"] = perfStats.stepTimeMs;
            
            return result;
        }, "Convert optimized brain to dictionary representation")
        .def("from_dict", [](std::shared_ptr<nlm::OptimizedBrain> self, const py::dict& dict) {
            // For now, just log the conversion
            std::cout << "OptimizedBrain.from_dict: Loading from dictionary (not yet implemented)" << std::endl;
            return self;
        }, py::arg("dict"), "Create optimized brain from dictionary (not yet implemented)");

    // MemoryPool - Lock-free memory pool for efficient object allocation and deallocation
    py::class_<nlm::MemoryPool<NeuronId>, std::shared_ptr<nlm::MemoryPool<NeuronId>>>(m, "NeuronPool",
        R"pbdoc(Lock-free memory pool for neuron IDs and states)
        
        Specialized memory pool optimized for neuron allocation.
        Provides efficient neuron ID allocation with lock-free operations,
        reducing memory allocation overhead during simulation.
        
        Features:
        - Lock-free allocation and deallocation
        - Pre-allocated memory blocks
        - Thread-safe operations
        - Memory usage monitoring
        - Statistics reporting (available/allocated counts)
        )pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("block_size") = 1024, py::arg("initial_blocks") = 4,
             "Create neuron pool with block size and initial blocks")
        .def("allocate", &nlm::MemoryPool<NeuronId>::allocate,
             "Allocate a neuron ID from the pool")
        .def("deallocate", &nlm::MemoryPool<NeuronId>::deallocate, py::arg("obj"),
             "Return a neuron ID to the pool")
        .def("empty", &nlm::MemoryPool<NeuronId>::empty,
             "Check if pool is empty")
        .def("allocated_count", &nlm::MemoryPool<NeuronId>::allocatedCount,
             "Get number of allocated neuron IDs")
        .def("available_count", &nlm::MemoryPool<NeuronId>::availableCount,
             "Get number of available neuron IDs")
        .def("memory_usage", &nlm::MemoryPool<NeuronId>::memoryUsage,
             "Get memory usage in bytes")
        .def("reserve", &nlm::MemoryPool<NeuronId>::reserve, py::arg("count"),
             "Pre-allocate additional neuron IDs")
        .def("stats", [](const std::shared_ptr<nlm::MemoryPool<NeuronId>>& pool) {
            py::dict stats;
            stats["allocated_count"] = pool->allocatedCount();
            stats["available_count"] = pool->availableCount();
            stats["memory_usage_bytes"] = pool->memoryUsage();
            stats["utilization_rate"] = static_cast<double>(pool->allocatedCount()) / 
                                         (pool->allocatedCount() + pool->availableCount());
            return stats;
        }, "Get pool statistics");

    // Generic MemoryPool<T> for arbitrary types
    py::class_<nlm::MemoryPool<void>, std::shared_ptr<nlm::MemoryPool<void>>>(m, "MemoryPool",
        R"pbdoc(Generic lock-free memory pool for arbitrary types)
        
        Template-based memory pool for efficient allocation/deallocation of objects.
        Provides pre-allocated object pools to reduce memory allocation overhead during simulation.
        
        Template parameter T specifies the object type to allocate. All objects in the pool
        have the same fixed size determined at construction.
        
        Features:
        - Lock-free allocation and deallocation
        - Pre-allocated memory blocks
        - Memory-efficient slab allocation
        - Thread-safe operations
        - Memory usage monitoring
        - Statistics reporting
        )pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("block_size") = 1024, py::arg("initial_blocks") = 4,
             "Create memory pool with block size and initial blocks")
        .def("allocate", &nlm::MemoryPool<void>::allocate,
             "Allocate an object from the pool")
        .def("deallocate", &nlm::MemoryPool<void>::deallocate, py::arg("obj"),
             "Return an object to the pool")
        .def("empty", &nlm::MemoryPool<void>::empty,
             "Check if pool is empty")
        .def("allocated_count", &nlm::MemoryPool<void>::allocatedCount,
             "Get number of allocated objects")
        .def("available_count", &nlm::MemoryPool<void>::availableCount,
             "Get number of available objects")
        .def("memory_usage", &nlm::MemoryPool<void>::memoryUsage,
             "Get memory usage in bytes")
        .def("reserve", &nlm::MemoryPool<void>::reserve, py::arg("count"),
             "Pre-allocate additional objects");

    // SynapsePool - Specialized pool for synapses
    py::class_<nlm::SynapsePool, std::shared_ptr<nlm::SynapsePool>>(m, "SynapsePool",
        R"pbdoc(Lock-free memory pool for synapses)
        
        Specialized memory pool optimized for synapse allocation.
        Provides efficient synapse ID allocation and state management.
        
        Features:
        - Lock-free allocation/deallocation
        - Pre-allocated synapse records
        - Thread-safe operations
        - Memory usage monitoring
        - Statistics reporting
        )pbdoc")
        .def(py::init<>(), "Create synapse pool")
        .def("allocate", &nlm::SynapsePool::allocate, py::arg("source"), py::arg("destination"),
             "Allocate a synapse from source to destination")
        .def("deallocate", &nlm::SynapsePool::deallocate, py::arg("id"),
             "Deallocate a synapse")
        .def("size", &nlm::SynapsePool::size,
             "Get total synapse count")
        .def("memory_usage", &nlm::SynapsePool::memoryUsage,
             "Get memory usage in bytes")
        .def("stats", [](const std::shared_ptr<nlm::SynapsePool>& pool) {
            py::dict stats;
            stats["size"] = pool->size();
            stats["memory_usage_bytes"] = pool->memoryUsage();
            return stats;
        }, "Get synapse pool statistics");

    // ThreadPool - Simple thread pool for parallel tasks
    py::class_<nlm::ThreadPool, std::shared_ptr<nlm::ThreadPool>>(m, "ThreadPool",
        R"pbdoc(Thread pool for parallel task execution)
        
        Provides thread pool infrastructure for parallel processing.
        Supports concurrent execution of tasks with work-stealing load balancing.
        
        Features:
        - Submit tasks for parallel execution
        - Thread-safe operations
        - Performance monitoring and statistics
        - Thread identification and management
        )pbdoc")
        .def(py::init<size_t>(), py::arg("num_threads") = nlm::DEFAULT_NUM_THREADS,
             "Create thread pool with specified number of threads")
        .def("submit", &nlm::ThreadPool::submit, py::arg("func"), py::arg("args"),
             "Submit a task for parallel execution, returns future")
        .def("submitVoid", &nlm::ThreadPool::submitVoid, py::arg("func"),
             "Submit a void task for parallel execution")
        .def("wait", &nlm::ThreadPool::wait,
             "Wait for all submitted tasks to complete")
        .def("get_num_threads", &nlm::ThreadPool::getNumThreads,
             "Get number of threads in pool")
        .def("get_current_thread_id", &nlm::ThreadPool::getCurrentThreadId,
             "Get current thread ID (0 = main, 1..N = workers)")
        .def("stats", [](const std::shared_ptr<nlm::ThreadPool>& pool) {
            py::dict stats;
            stats["num_threads"] = pool->getNumThreads();
            // Note: active_tasks_ is private, so we can't access it directly
            return stats;
        }, "Get thread pool statistics");

    // ParallelFor - Parallel for-loop execution
    py::class_<nlm::ParallelFor>(m, "ParallelFor",
        R"pbdoc(Parallel for-loop execution algorithms)
        
        Provides parallel processing for loops using static partitioning.
        Suitable for uniform workloads like updating neuron populations.
        
        Features:
        - Parallel range execution
        - Parallel reductions
        - Parallel prefix (scan) operations
        - Work-stealing load balancing
        )pbdoc")
        .def_static("range", &nlm::ParallelFor::range,
                   py::arg("begin"), py::arg("end"), py::arg("func"),
                   py::arg("grain_size") = 256,
                   "Execute function in parallel over a range")
        .def_static("simple", &nlm::ParallelFor::simple,
                   py::arg("begin"), py::arg("end"), py::arg("func"),
                   py::arg("grain_size") = 256,
                   "Execute function in parallel over a range (simpler API)")
        .def_static("reduce", &nlm::ParallelFor::reduce,
                   py::arg("begin"), py::arg("end"), py::arg("func"),
                   py::arg("combine"), py::arg("identity"),
                   "Execute reduction in parallel")
        .def_static("inclusive_scan", &nlm::ParallelFor::inclusiveScan,
                   py::arg("data"), py::arg("op"),
                   "Execute inclusive scan in parallel")
        .def_static("exclusive_scan", &nlm::ParallelFor::exclusiveScan,
                   py::arg("data"), py::arg("identity"), py::arg("op"),
                   "Execute exclusive scan in parallel")
        .def_static("parallel_reducers", &nlm::ParallelReducers::sum,
                   "Access parallel reducer functions (sum, max, min)");

    // ParallelNeuralProcessor - Parallel processing of neural populations
    py::class_<nlm::ParallelNeuralProcessor, std::shared_ptr<nlm::ParallelNeuralProcessor>>(m, "ParallelNeuralProcessor",
        R"pbdoc(Parallel processing of neural populations)
        
        Provides thread-safe parallel processing of neural computations.
        Each thread processes a subset of neurons with local state.
        
        Features:
        - Parallel neuron processing
        - Thread-local state accumulation
        - Synchronized spike aggregation
        - Performance monitoring
        - Thread management
        )pbdoc")
        .def(py::init<size_t>(), py::arg("num_threads") = nlm::DEFAULT_NUM_THREADS,
             "Create parallel neural processor with specified number of threads")
        .def("process_neurons", &nlm::ParallelNeuralProcessor::processNeurons,
             py::arg("num_neurons"), py::arg("process_func"), py::arg("sync_func") = nullptr,
             "Process neurons in parallel")
        .def("get_total_spike_count", &nlm::ParallelNeuralProcessor::getTotalSpikeCount,
             "Get total spike count from last processing")
        .def("get_total_firing_count", &nlm::ParallelNeuralProcessor::getTotalFiringCount,
             "Get total firing count from last processing")
        .def("get_thread_pool", &nlm::ParallelNeuralProcessor::getThreadPool,
             py::return_value_policy::reference_internal,
             "Get underlying thread pool")
        .def("stats", [](const std::shared_ptr<nlm::ParallelNeuralProcessor>& processor) {
            py::dict stats;
            stats["total_spike_count"] = processor->getTotalSpikeCount();
            stats["total_firing_count"] = processor->getTotalFiringCount();
            stats["num_threads"] = processor->getThreadPool().getNumThreads();
            return stats;
        }, "Get processing statistics");

    // ParallelReducers - Reduction operations for parallel processing
    py::class_<nlm::ParallelReducers>(m, "ParallelReducers",
        R"pbdoc(Reduction operations for parallel processing)
        
        Utility class containing reduction operations for parallel processing.
        
        Provides functions for combining values in parallel reductions and scans.
        )pbdoc")
        .def_static("sum", &nlm::ParallelReducers::sum, py::arg("a"), py::arg("b"),
                    "Sum two values")
        .def_static("max", &nlm::ParallelReducers::max, py::arg("a"), py::arg("b"),
                    "Get maximum of two values")
        .def_static("min", &nlm::ParallelReducers::min, py::arg("a"), py::arg("b"),
                    "Get minimum of two values")
        .def_static("sum_float", &nlm::ParallelReducers::sumFloat, py::arg("a"), py::arg("b"),
                    "Sum two float values")
        .def_static("sum_double", &nlm::ParallelReducers::sumDouble, py::arg("a"), py::arg("b"),
                    "Sum two double values");

    // Event Queue types and classes
    py::class_<nlm::QueuedSpikeEvent>(m, "QueuedSpikeEvent",
        R"pbdoc(Spike event with source, destination, synapse, and weight information)
        
        Represents a spike event in the neural network.
        Contains all necessary information for spike propagation.
        )pbdoc")
        .def(py::init<>(), "Create empty spike event")
        .def(py::init<uint64_t, uint64_t, uint64_t, float, bool>(),
             py::arg("source"), py::arg("dest"), py::arg("synapse"), py::arg("weight"), py::arg("is_excitatory"),
             "Create spike event with all fields")
        .def_readwrite("source_neuron", &nlm::QueuedSpikeEvent::sourceNeuron,
                      "Source neuron ID")
        .def_readwrite("dest_neuron", &nlm::QueuedSpikeEvent::destNeuron,
                      "Destination neuron ID")
        .def_readwrite("synapse_id", &nlm::QueuedSpikeEvent::synapseId,
                      "Synapse ID")
        .def_readwrite("weight", &nlm::QueuedSpikeEvent::weight,
                      "Synaptic weight")
        .def_readwrite("is_excitatory", &nlm::QueuedSpikeEvent::isExcitatory,
                      "Is excitatory synapse?");

    // SpikeRingBuffer - Lock-free single-producer single-consumer spike queue
    py::class_<nlm::SpikeRingBuffer, std::shared_ptr<nlm::SpikeRingBuffer>>(m, "SpikeRingBuffer",
        R"pbdoc(Lock-free single-producer single-consumer spike ring buffer)
        
        High-performance event queue using lock-free ring buffers.
        Designed for event-driven neural simulation where most neurons are inactive.
        
        Features:
        - Lock-free operations
        - Bounded capacity with overflow handling
        - Cache-efficient circular buffer
        - Support for immediate and delayed events
        )pbdoc")
        .def(py::init<size_t>(), py::arg("capacity") = nlm::SPIKE_QUEUE_CAPACITY,
             "Create spike ring buffer with specified capacity")
        .def("push", &nlm::SpikeRingBuffer::push, py::arg("event"),
             "Push a spike event to the queue, returns true if successful")
        .def("pop", &nlm::SpikeRingBuffer::pop,
             "Pop a spike event from the queue, returns None if empty")
        .def("empty", &nlm::SpikeRingBuffer::empty,
             "Check if queue is empty")
        .def("full", &nlm::SpikeRingBuffer::full,
             "Check if queue is full")
        .def("size", &nlm::SpikeRingBuffer::size,
             "Get number of events in queue")
        .def("capacity", &nlm::SpikeRingBuffer::capacity,
             "Get queue capacity")
        .def("clear", &nlm::SpikeRingBuffer::clear,
             "Clear all events")
        .def("stats", [](const std::shared_ptr<nlm::SpikeRingBuffer>& queue) {
            py::dict stats;
            stats["size"] = queue->size();
            stats["capacity"] = queue->capacity();
            stats["utilization"] = static_cast<double>(queue->size()) / queue->capacity();
            return stats;
        }, "Get queue statistics");

    // DelayedSpikeQueue - Time-bucketed delayed spike queue
    py::class_<nlm::DelayedSpikeQueue, std::shared_ptr<nlm::DelayedSpikeQueue>>(m, "DelayedSpikeQueue",
        R"pbdoc(Time-bucketed delayed spike queue)
        
        Organizes delayed spikes into time buckets for efficient processing.
        Each bucket contains spikes scheduled for delivery at that simulation step.
        
        Biological motivation:
        - Synaptic delays vary (1-20ms typically)
        - Spikes are delivered after their delay period
        - Grouping by delivery time enables efficient batch processing
        )pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("num_buckets") = nlm::DELAYED_SPIKE_BUCKETS,
             py::arg("max_delay_steps") = nlm::MAX_DELAY_STEPS,
             "Create delayed spike queue")
        .def("schedule", &nlm::DelayedSpikeQueue::schedule,
             py::arg("source_neuron"), py::arg("dest_neuron"), py::arg("synapse_id"),
             py::arg("weight"), py::arg("is_excitatory"), py::arg("delivery_step"),
             "Schedule a spike for delivery at a future step")
        .def("get_due_spikes", &nlm::DelayedSpikeQueue::getDueSpikes, py::arg("current_step"),
             "Get all spikes scheduled for delivery at current step")
        .def("pending_count", &nlm::DelayedSpikeQueue::pendingCount,
             "Get count of pending delayed spikes")
        .def("bucket_size", &nlm::DelayedSpikeQueue::bucketSize, py::arg("step"),
             "Get count of spikes in a specific bucket")
        .def("clear", &nlm::DelayedSpikeQueue::clear,
             "Clear all pending spikes")
        .def("advance_time", &nlm::DelayedSpikeQueue::advanceTime, py::arg("current_step"),
             "Advance time bucket (call at end of simulation step)")
        .def("stats", [](const std::shared_ptr<nlm::DelayedSpikeQueue>& queue) {
            py::dict stats;
            stats["pending_count"] = queue->pendingCount();
            stats["num_buckets"] = queue->numBuckets_;
            stats["max_delay_steps"] = queue->maxDelaySteps_;
            stats["current_step"] = queue->currentStep_;
            return stats;
        }, "Get queue statistics");

    // PriorityEventQueue - Priority event queue for time-sensitive events
    py::class_<nlm::PriorityEventQueue, std::shared_ptr<nlm::PriorityEventQueue>>(m, "PriorityEventQueue",
        R"pbdoc(Priority event queue for time-sensitive events)
        
        Uses a priority queue for events that need strict ordering.
        Used for neuromodulatory events, plasticity updates, etc.
        
        Events are prioritized by:
        1. Priority level (0 = highest priority)
        2. Timestamp (earlier timestamps first)
        )pbdoc")
        .def(py::init<>(), "Create priority event queue")
        .def("push", &nlm::PriorityEventQueue::push,
             py::arg("timestamp"), py::arg("priority"), py::arg("type"), py::arg("data") = nullptr,
             "Push a priority event")
        .def("pop", &nlm::PriorityEventQueue::pop,
             "Pop the highest priority event")
        .def("get_due_events", &nlm::PriorityEventQueue::getDueEvents, py::arg("current_time"),
             "Get all events due before a timestamp")
        .def("empty", &nlm::PriorityEventQueue::empty,
             "Check if queue is empty")
        .def("size", &nlm::PriorityEventQueue::size,
             "Get count of events in queue")
        .def("clear", &nlm::PriorityEventQueue::clear,
             "Clear all events")
        .def("stats", [](const std::shared_ptr<nlm::PriorityEventQueue>& queue) {
            py::dict stats;
            stats["size"] = queue->size();
            return stats;
        }, "Get queue statistics");

    // SIMD Vectorization utilities
    py::class_<nlm::SIMDUtils>(m, "SIMDUtils",
        R"pbdoc(SIMD utilities for portable vectorized operations)
        
        Provides SIMD detection and utility functions.
        Allows users to check available SIMD instruction sets.
        )pbdoc")
        .def_static("is_simd_available", &nlm::SIMDUtils::isSIMDAvailable,
                    "Check if SIMD is available")
        .def_static("get_simd_type", &nlm::SIMDUtils::getSIMDType,
                    "Get SIMD instruction set name")
        .def_static("get_alignment", &nlm::SIMDUtils::alignment,
                    "Get optimal memory alignment for SIMD operations")
        .def_static("get_simd_width", &nlm::SIMDUtils::SIMD_WIDTH,
                    "Get SIMD vector width (number of floats per vector)");

    // VectorizedLIF - Vectorized Leaky Integrate-and-Fire neuron model
    py::class_<nlm::VectorizedLIF>(m, "VectorizedLIF",
        R"pbdoc(Vectorized Leaky Integrate-and-Fire (LIF) neuron model)
        
        Vectorized LIF update for efficient batch processing of multiple neurons.
        Supports automatic SIMD vectorization when available.
        
        Mathematical model (per neuron):
        dV/dt = (V_rest - V)/tau + I/C
        
        Discrete update:
        V_new = V + dt * ((V_rest - V)/tau + I/C)
        
        if V_new >= threshold: spike = true, V = V_reset
        )pbdoc")
        .def(py::init<>(), "Create default VectorizedLIF neuron model")
        .def(py::init<float, float, float, float, float, float, float, float, float>(
            &nlm::VectorizedLIF::VectorizedLIF),
            py::arg("v_rest") = -70.0f, py::arg("v_reset") = -70.0f,
            py::arg("v_threshold") = -55.0f, py::arg("tau") = 20.0f,
            py::arg("c") = 1.0f, py::arg("dt") = 1.0f,
            py::arg("refractory_decay") = 1.0f,
            py::arg("refractory_period") = 5.0f,
            "Create VectorizedLIF with custom parameters")
        .def("set_parameters", [](nlm::VectorizedLIF& self,
                                 float v_rest = -70.0f, float v_reset = -70.0f,
                                 float v_threshold = -55.0f, float tau = 20.0f,
                                 float c = 1.0f, float dt = 1.0f,
                                 float refractory_decay = 1.0f,
                                 float refractory_period = 5.0f) {
            self.V_rest = v_rest;
            self.V_reset = v_reset;
            self.V_threshold = v_threshold;
            self.tau = tau;
            self.C = c;
            self.dt = dt;
            self.refractoryDecay = refractory_decay;
            self.refractoryPeriod = refractory_period;
        }, py::arg("v_rest") = -70.0f, py::arg("v_reset") = -70.0f,
           py::arg("v_threshold") = -55.0f, py::arg("tau") = 20.0f,
           py::arg("c") = 1.0f, py::arg("dt") = 1.0f,
           py::arg("refractory_decay") = 1.0f,
           py::arg("refractory_period") = 5.0f,
           "Set neuron model parameters")
        .def("update", [](nlm::VectorizedLIF& self,
                         std::vector<float>& V_inout, const std::vector<float>& I,
                         std::vector<uint32_t>& refractoryRemaining,
                         std::vector<uint8_t>& fired, size_t count) {
            return self.update(V_inout, I, refractoryRemaining, fired, count);
        }, py::arg("V_inout"), py::arg("I"), py::arg("refractory_remaining"),
           py::arg("fired"), py::arg("count"),
           "Update membrane potentials for neurons (returns number of spikes)")
        .def("stats", [](const nlm::VectorizedLIF& self) {
            py::dict stats;
            stats["v_rest"] = self.V_rest;
            stats["v_reset"] = self.V_reset;
            stats["v_threshold"] = self.V_threshold;
            stats["tau"] = self.tau;
            stats["c"] = self.C;
            stats["dt"] = self.dt;
            stats["refractory_decay"] = self.refractoryDecay;
            stats["refractory_period"] = self.refractoryPeriod;
            return stats;
        }, "Get neuron model parameters");

    // VectorizedSynapse - Vectorized short-term plasticity
    py::class_<nlm::VectorizedSynapse>(m, "VectorizedSynapse",
        R"pbdoc(Vectorized synapse model for short-term plasticity)
        
        Vectorized update of short-term synaptic plasticity state.
        Supports depression and facilitation dynamics.
        )pbdoc")
        .def(py::init<>(), "Create default vectorized synapse model")
        .def(py::init<float, float, float>(
            &nlm::VectorizedSynapse::VectorizedSynapse),
            py::arg("tau_facilitation") = 100.0f,
            py::arg("tau_depression") = 200.0f,
            py::arg("u_max") = 1.0f,
            "Create vectorized synapse with custom parameters")
        .def("set_parameters", [](nlm::VectorizedSynapse& self,
                                 float tau_facilitation = 100.0f,
                                 float tau_depression = 200.0f,
                                 float u_max = 1.0f) {
            self.tau_facilitation = tau_facilitation;
            self.tau_depression = tau_depression;
            self.U_max = u_max;
        }, py::arg("tau_facilitation") = 100.0f,
           py::arg("tau_depression") = 200.0f,
           py::arg("u_max") = 1.0f,
           "Set synapse model parameters")
        .def("update_std", [](nlm::VectorizedSynapse& self,
                             std::vector<float>& depression,
                             std::vector<float>& facilitation,
                             const float* last_pre_time,
                             const float* last_post_time,
                             float current_time,
                             size_t count) {
            return self.updateSTD(depression, facilitation, last_pre_time, last_post_time, current_time, count);
        }, py::arg("depression"), py::arg("facilitation"), py::arg("last_pre_time"),
           py::arg("last_post_time"), py::arg("current_time"), py::arg("count"),
           "Update short-term plasticity state")
        .def("stats", [](const nlm::VectorizedSynapse& self) {
            py::dict stats;
            stats["tau_facilitation"] = self.tau_facilitation;
            stats["tau_depression"] = self.tau_depression;
            stats["u_max"] = self.U_max;
            return stats;
        }, "Get synapse model parameters");

    // VectorizedSTDP - Vectorized Spike-Timing-Dependent Plasticity
    py::class_<nlm::VectorizedSTDP>(m, "VectorizedSTDP",
        R"pbdoc(Vectorized STDP (Spike-Timing-Dependent Plasticity) model)
        
        Vectorized weight update for STDP.
        Calculates weight changes based on pre/post spike timing.
        
        Pre before post: LTP (Long-Term Potentiation)
        Post before pre: LTD (Long-Term Depression)
        )pbdoc")
        .def(py::init<>(), "Create default VectorizedSTDP model")
        .def(py::init<float, float, float, float, float, float, float, float>(
            &nlm::VectorizedSTDP::VectorizedSTDP),
            py::arg("tau_plus") = 20.0f, py::arg("tau_minus") = 20.0f,
            py::arg("a_plus") = 0.01f, py::arg("a_minus") = 0.012f,
            py::arg("w_max") = 1.0f, py::arg("w_min") = -1.0f,
            "Create VectorizedSTDP with custom parameters")
        .def("set_parameters", [](nlm::VectorizedSTDP& self,
                                 float tau_plus = 20.0f, float tau_minus = 20.0f,
                                 float a_plus = 0.01f, float a_minus = 0.012f,
                                 float w_max = 1.0f, float w_min = -1.0f) {
            self.tau_plus = tau_plus;
            self.tau_minus = tau_minus;
            self.A_plus = a_plus;
            self.A_minus = a_minus;
            self.w_max = w_max;
            self.w_min = w_min;
        }, py::arg("tau_plus") = 20.0f, py::arg("tau_minus") = 20.0f,
           py::arg("a_plus") = 0.01f, py::arg("a_minus") = 0.012f,
           py::arg("w_max") = 1.0f, py::arg("w_min") = -1.0f,
           "Set STDP model parameters")
        .def("calculate_delta_w", &nlm::VectorizedSTDP::calculateDeltaW,
             py::arg("dt"), "Calculate weight delta given time difference")
        .def("update_weights", [](nlm::VectorizedSTDP& self,
                                 std::vector<float>& weights,
                                 const std::vector<float>& pre_spikes,
                                 const std::vector<float>& post_spikes,
                                 size_t num_pre,
                                 size_t num_post) {
            return self.updateWeights(weights, pre_spikes, post_spikes, num_pre, num_post);
        }, py::arg("weights"), py::arg("pre_spikes"), py::arg("post_spikes"),
           py::arg("num_pre"), py::arg("num_post"),
           "Update weights based on spike traces")
        .def("stats", [](const nlm::VectorizedSTDP& self) {
            py::dict stats;
            stats["tau_plus"] = self.tau_plus;
            stats["tau_minus"] = self.tau_minus;
            stats["a_plus"] = self.A_plus;
            stats["a_minus"] = self.A_minus;
            stats["w_max"] = self.w_max;
            stats["w_min"] = self.w_min;
            return stats;
        }, "Get STDP model parameters");

    // VectorizedStats - Vectorized statistics computation
    py::class_<nlm::VectorizedStats>(m, "VectorizedStats",
        R"pbdoc(Vectorized statistics computation utilities)
        
        Provides vectorized implementations of common statistical operations.
        Optimized for SIMD execution when available.
        )pbdoc")
        .def_static("sum", &nlm::VectorizedStats::sum, py::arg("arr"), py::arg("count"),
                    "Compute sum of array elements")
        .def_static("mean", &nlm::VectorizedStats::mean, py::arg("arr"), py::arg("count"),
                    "Compute mean of array elements")
        .def_static("variance", &nlm::VectorizedStats::variance, py::arg("arr"), py::arg("count"),
                    "Compute variance of array elements")
        .def_static("max", &nlm::VectorizedStats::max, py::arg("arr"), py::arg("count"),
                    "Compute max of array elements")
        .def_static("clamp", &nlm::VectorizedStats::clamp,
                    py::arg("arr"), py::arg("count"), py::arg("min_val"), py::arg("max_val"),
                    "Clamp array values to range");

    // SparseConnectivity - Memory-efficient neural connectivity representation
    py::class_<nlm::SparseConnectivity, std::shared_ptr<nlm::SparseConnectivity>>(m, "SparseConnectivity",
        R"pbdoc(Memory-efficient sparse connectivity representation)
        
        Provides sparse adjacency list storage for neural connections.
        Avoids O(N^2) memory requirement of dense connectivity matrices.
        
        Features:
        - Adjacency list representation
        - Efficient synapse lookup by source or destination neuron
        - Compact storage for large networks
        - Cache-friendly iteration patterns
        )pbdoc")
        .def(py::init<>(), "Create sparse connectivity structure")
        .def("reserve_neurons", &nlm::SparseConnectivity::reserveNeurons, py::arg("count"),
             "Reserve space for neurons")
        .def("add_synapse", &nlm::SparseConnectivity::addSynapse,
             py::arg("source_neuron"), py::arg("dest_neuron"), py::arg("weight"),
             py::arg("delay"), py::arg("synapse_type"),
             "Add a synapse, returns synapse record index")
        .def("remove_synapse", &nlm::SparseConnectivity::removeSynapse, py::arg("synapse_index"),
             "Remove a synapse by record index")
        .def("get_synapse", &nlm::SparseConnectivity::getSynapse, py::arg("index"),
             py::return_value_policy::reference_internal, "Get synapse record")
        .def("get_synapses_from", &nlm::SparseConnectivity::getSynapsesFrom, py::arg("source_neuron"),
             "Get all synapses from a source neuron")
        .def("get_synapses_to", &nlm::SparseConnectivity::getSynapsesTo, py::arg("dest_neuron"),
             "Get all synapses to a destination neuron")
        .def("get_synapse_count_from", &nlm::SparseConnectivity::getSynapseCountFrom, py::arg("source_neuron"),
             "Get synapse count from a specific neuron")
        .def("get_synapse_count_to", &nlm::SparseConnectivity::getSynapseCountTo, py::arg("dest_neuron"),
             "Get synapse count to a specific neuron")
        .def("get_synapse_count", &nlm::SparseConnectivity::getSynapseCount,
             "Get total synapse count")
        .def("get_neuron_count", &nlm::SparseConnectivity::getNeuronCount,
             "Get neuron count")
        .def("get_synapses_with_delay", &nlm::SparseConnectivity::getSynapsesWithDelay, py::arg("max_delay"),
             "Get synapses with delay <= max_delay")
        .def("get_stats", &nlm::SparseConnectivity::getStats,
             "Get connectivity statistics")
        .def("clear", &nlm::SparseConnectivity::clear,
             "Clear all synapses")
        .def("memory_usage", &nlm::SparseConnectivity::memoryUsage,
             "Get memory usage in bytes")
        .def("for_each_synapse", [](nlm::SparseConnectivity& self, const py::function<void(size_t, const nlm::SynapseRecord&)>& func) {
            self.forEachSynapse([func](size_t i, const nlm::SynapseRecord& syn) {
                func(i, syn);
            });
        }, py::arg("func"), "Iterate over all synapses")
        .def("for_each_synapse", [](const nlm::SparseConnectivity& self, const py::function<void(size_t, const nlm::SynapseRecord&)>& func) {
            self.forEachSynapse([func](size_t i, const nlm::SynapseRecord& syn) {
                func(i, syn);
            });
        }, py::arg("func"), "Iterate over all synapses (const)")
        .def("get_synapses", &nlm::SparseConnectivity::getSynapses,
             py::return_value_policy::reference_internal, "Get raw synapse array")
        .def("stats", [](const std::shared_ptr<nlm::SparseConnectivity>& conn) {
            py::dict stats = py::cast(conn->getStats());
            // Convert std::string keys to Python strings
            return stats;
        }, "Get connectivity statistics");

    // Performance Configuration and monitoring
    m.def("createOptimizedBrain", [](const std::shared_ptr<Config>& config) -> std::shared_ptr<nlm::OptimizedBrain> {
        return std::make_shared<nlm::OptimizedBrain>(config);
    }, py::arg("config"), "Create an optimized brain with configuration");

    m.def("createDefaultOptimizedBrain", []() -> std::shared_ptr<nlm::OptimizedBrain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 5);
        config->set("brain.learningRate", 0.1);
        config->set("brain.plasticityEnabled", true);
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("world.energyDecayRate", 0.01);
        config->set("agent.curiosityLevel", 0.5);
        config->set("agent.rewardWeight", 1.0);
        
        auto brain = std::make_shared<nlm::OptimizedBrain>(config);
        return brain;
    }, "Create an optimized brain with default configuration");

    m.def("createNeuronPool", [](size_t blockSize = 1024, size_t initialBlocks = 4) -> std::shared_ptr<nlm::MemoryPool<NeuronId>> {
        return std::make_shared<nlm::MemoryPool<NeuronId>>(blockSize, initialBlocks);
    }, py::arg("block_size") = 1024, py::arg("initial_blocks") = 4,
       "Create neuron pool with specified block size and initial blocks");

    m.def("createSynapsePool", []() -> std::shared_ptr<nlm::SynapsePool> {
        return std::make_shared<nlm::SynapsePool>();
    }, "Create synapse pool");

    m.def("createThreadPool", [](size_t numThreads = nlm::DEFAULT_NUM_THREADS) -> std::shared_ptr<nlm::ThreadPool> {
        return std::make_shared<nlm::ThreadPool>(numThreads);
    }, py::arg("num_threads") = nlm::DEFAULT_NUM_THREADS,
       "Create thread pool with specified number of threads");

    m.def("createParallelNeuralProcessor", [](size_t numThreads = nlm::DEFAULT_NUM_THREADS) -> std::shared_ptr<nlm::ParallelNeuralProcessor> {
        return std::make_shared<nlm::ParallelNeuralProcessor>(numThreads);
    }, py::arg("num_threads") = nlm::DEFAULT_NUM_THREADS,
       "Create parallel neural processor with specified number of threads");

    m.def("createSparseConnectivity", []() -> std::shared_ptr<nlm::SparseConnectivity> {
        return std::make_shared<nlm::SparseConnectivity>();
    }, "Create sparse connectivity structure");

    m.def("createSpikeRingBuffer", [](size_t capacity) -> std::shared_ptr<nlm::SpikeRingBuffer> {
        return std::make_shared<nlm::SpikeRingBuffer>(capacity);
    }, py::arg("capacity") = nlm::SPIKE_QUEUE_CAPACITY,
       "Create spike ring buffer with specified capacity");

    m.def("createDelayedSpikeQueue", [](size_t numBuckets = nlm::DELAYED_SPIKE_BUCKETS, 
                                        size_t maxDelaySteps = nlm::MAX_DELAY_STEPS) -> std::shared_ptr<nlm::DelayedSpikeQueue> {
        return std::make_shared<nlm::DelayedSpikeQueue>(numBuckets, maxDelaySteps);
    }, py::arg("num_buckets") = nlm::DELAYED_SPIKE_BUCKETS,
       py::arg("max_delay_steps") = nlm::MAX_DELAY_STEPS,
       "Create delayed spike queue with specified parameters");

    m.def("createPriorityEventQueue", []() -> std::shared_ptr<nlm::PriorityEventQueue> {
        return std::make_shared<nlm::PriorityEventQueue>();
    }, "Create priority event queue");

    // Performance utilities
    m.def("get_simd_info", []() {
        py::dict info;
        info["available"] = nlm::SIMDUtils::isSIMDAvailable();
        info["type"] = nlm::SIMDUtils::getSIMDType();
        info["width"] = nlm::SIMDUtils::SIMD_WIDTH;
        info["alignment"] = nlm::SIMDUtils::alignment();
        return info;
    }, "Get SIMD information");

    m.def("get_performance_config", []() {
        py::dict config;
        config["simd_available"] = nlm::SIMDUtils::isSIMDAvailable();
        config["simd_type"] = nlm::SIMDUtils::getSIMDType();
        config["simd_width"] = nlm::SIMDUtils::SIMD_WIDTH;
        config["num_threads_default"] = nlm::DEFAULT_NUM_THREADS;
        config["memory_pool_page_size"] = nlm::NLM_MEMORY_POOL_PAGE_SIZE;
        config["max_pool_size"] = nlm::NLM_MAX_POOL_SIZE;
        return config;
    }, "Get current performance configuration");

    m.def("get_enums", []() {
        py::dict result;
        
        py::enum_<NeuronType> neuron_type_enum(m, "NeuronTypeEnum", "Neuron type enumeration");
        neuron_type_enum.value("Excitatory", NeuronType::Excitatory)
                       .value("Inhibitory", NeuronType::Inhibitory)
                       .value("Modulatory", NeuronType::Modulatory)
                       .value("Sensory", NeuronType::Sensory)
                       .value("Motor", NeuronType::Motor)
                       .value("Internal", NeuronType::Internal)
                       .export_values();
        
        py::enum_<SynapseType> synapse_type_enum(m, "SynapseTypeEnum", "Synapse type enumeration");
        synapse_type_enum.value("Excitatory", SynapseType::Excitatory)
                       .value("Inhibitory", SynapseType::Inhibitory)
                       .value("Modulatory", SynapseType::Modulatory)
                       .value("Electrical", SynapseType::Electrical)
                       .value("GapJunction", SynapseType::GapJunction)
                       .export_values();
        
        py::enum_<SynapseType> synapse_type_enum2(m, "SynapseTypeEnum2", "Synapse type enumeration");
        synapse_type_enum2.value("Excitatory", SynapseType::Excitatory)
                        .value("Inhibitory", SynapseType::Inhibitory)
                        .value("Modulatory", SynapseType::Modulatory)
                        .value("Electrical", SynapseType::Electrical)
                        .value("GapJunction", SynapseType::GapJunction)
                        .export_values();
        
        return result;
    }, "Get all enumeration types");

    // Convenience functions for common use cases
    m.def("runOptimizedEpisode", [](std::shared_ptr<nlm::OptimizedBrain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent,
                                  SimulationStep maxSteps,
                                  bool enableLearning = true) {
        if (!brain || !world || !agent) {
            throw PyValueError("Brain, world, and agent must be provided");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation if enabled
            if (enableLearning) {
                auto result = world->getLastActionResult();
                if (result) {
                    agent->applyRewardModulation(result->reward, 0.0);
                }
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("max_steps"),
       py::arg("enable_learning") = true,
       "Run an optimized episode with performance monitoring");

    m.def("benchmarkPerformance", [](std::shared_ptr<nlm::OptimizedBrain> brain,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent,
                                    SimulationStep steps) {
        PerformanceProfiler profiler;
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        profiler.recordInitializationTime();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            profiler.recordStepTime();
        }
        
        std::cout << profiler.getPerformanceReport() << std::endl;
        return profiler.getPerformanceReport();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Benchmark performance and return report");

    // Constants
    m.attr("SPIKE_QUEUE_CAPACITY") = nlm::SPIKE_QUEUE_CAPACITY;
    m.attr("DELAYED_SPIKE_BUCKETS") = nlm::DELAYED_SPIKE_BUCKETS;
    m.attr("MAX_DELAY_STEPS") = nlm::MAX_DELAY_STEPS;
    m.attr("SIMD_WIDTH") = nlm::SIMD_WIDTH;
    m.attr("NLM_NUM_THREADS") = nlm::DEFAULT_NUM_THREADS;
    m.attr("NLM_MEMORY_POOL_PAGE_SIZE") = nlm::NLM_MEMORY_POOL_PAGE_SIZE;
    m.attr("NLM_MAX_POOL_SIZE") = nlm::NLM_MAX_POOL_SIZE;

    // Constants for enums
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBitory") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_MODULATORY") = NeuronType::Modulatory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_TYPE_INTERNAL") = NeuronType::Internal;

    m.attr("SYNAPSE_TYPE_EXCITATORY") = SynapseType::Excitatory;
    m.attr("SYNAPSE_TYPE_INHIBITORY") = SynapseType::Inhibitory;
    m.attr("SYNAPSE_TYPE_MODULATORY") = SynapseType::Modulatory;
    m.attr("SYNAPSE_TYPE_ELECTRICAL") = SynapseType::Electrical;
    m.attr("SYNAPSE_TYPE_GAP_JUNCTION") = SynapseType::GapJunction;

    m.attr("DEFAULT_NUM_THREADS") = nlm::DEFAULT_NUM_THREADS;

    // Error classes
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::value_error& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::type_error& e) {
            PyErr_SetString(PyExc_TypeError, e.what());
        }
    });

    m.def("getErrorClasses", []() {
        py::dict error_classes;
        error_classes["PyRuntimeError"] = PyRuntimeError("test");
        error_classes["PyValueError"] = PyValueError("test");
        error_classes["PyTypeError"] = PyTypeError("test");
        error_classes["PyKeyError"] = PyKeyError("test");
        return error_classes;
    }, "Get error class instances");
}

} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -----------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides enhanced convenience functions, error handling, and Pythonic API patterns.
        
        This enhanced version includes:
        - Performance-optimized classes (OptimizedBrain, MemoryPool, ParallelProcessing)
        - Event-driven processing and SIMD vectorization support
        - Sparse connectivity for large-scale neural networks
        - Comprehensive performance monitoring and statistics
        - Memory usage tracking and reporting
        - Thread management and configuration
        - Vectorized neuron and synapse models
        - Lock-free data structures for high-performance
        - Checkpoint system for long-running simulations
        - Flexible configuration options
        
        Key Features:
        * Backward compatible with existing Brain class
        * Performance not significantly impacted by Python bindings
        * Pythonic API following Python idioms and conventions
        * Proper exception handling and validation
        * Comprehensive docstrings for all classes and methods
        * Easy configuration of performance parameters
        * Performance monitoring and statistics in Python
        * Flexibility to enable/disable individual optimizations
    )pbdoc";

    // Register all NLM modules
    nlm::bind_all_modules(m);
}

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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

    // World (unchanged from original, but with enhanced documentation)
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
             py::return_value_policy::copy)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy)
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

    // Create default world
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Create default agent brain
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Create default config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Create default brain
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Exposed constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Additional utility functions
    
    // Statistical aggregation
    m.def("aggregateBrainStats", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        std::map<std::string, double> aggregated;
        if (brains.empty()) return aggregated;
        
        // Initialize with first brain's stats
        auto firstStats = std::make_shared<BrainStats>(brains[0]);
        aggregated = firstStats->getAllMetrics();
        
        // Aggregate with remaining brains
        for (size_t i = 1; i < brains.size(); ++i) {
            auto stats = std::make_shared<BrainStats>(brains[i]);
            auto metrics = stats->getAllMetrics();
            
            for (const auto& metric : metrics) {
                if (aggregated.find(metric.first) != aggregated.end()) {
                    aggregated[metric.first] += metric.second;
                }
            }
        }
        
        // Average
        for (auto& metric : aggregated) {
            metric.second /= brains.size();
        }
        
        return aggregated;
    }, py::arg("brains"), "Aggregate statistics from multiple brains");

    // Configuration validation helper
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw PyValueError("Config cannot be null");
        }
        return config->validate();
    }, py::arg("config"), "Validate configuration");

    // Environment setup helper
    m.def("setupExperimentEnvironment", [](const std::shared_ptr<ExperimentSetup>& setup,
                                           const std::shared_ptr<Brain>& brain,
                                           const std::shared_ptr<SimpleWorld>& world,
                                           const std::shared_ptr<AgentBrain>& agent) {
        if (!setup || !brain || !world || !agent) {
            throw PyValueError("Experiment setup and all components must be provided");
        }
        
        // Record initial state
        setup->recordMetric("initial_neuron_count", brain->getTotalNeuronCount());
        setup->recordMetric("initial_region_count", brain->getRegionCount());
        setup->recordMetric("world_width", world->getWidth());
        setup->recordMetric("world_height", world->getHeight());
        
        // Configure experiment
        setup->setExperimentName("NLM_Experiment");
        
        return true;
    }, py::arg("setup"), py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Setup experiment environment");
}

} // namespace nlm
