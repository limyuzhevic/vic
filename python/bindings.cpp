#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::enum_<nlm::ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enum)pbdoc")
        .value("Default", nlm::ConfigSource::Default)
        .value("File", nlm::ConfigSource::File)
        .value("CommandLine", nlm::ConfigSource::CommandLine)
        .value("Runtime", nlm::ConfigSource::Runtime);
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (simple key=value format)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("setInt", [](Config& self, const std::string& key, int value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
             "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, double value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
             "Set float configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
             "Set string configuration value")
        .def("getInt", &Config::template getOr<int>, py::arg("key"), py::arg("default_value") = 0,
             "Get integer configuration value with default")
        .def("getFloat", &Config::template getOr<double>, py::arg("key"), py::arg("default_value") = 0.0,
             "Get float configuration value with default")
        .def("getString", &Config::template getOr<std::string>, py::arg("key"), py::arg("default_value") = "",
             "Get string configuration value with default")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("toDict", [](const Config& self) {
            std::unordered_map<std::string, std::string> dict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get different types in order
                auto intVal = self.get<int>(key);
                if (intVal.has_value()) {
                    dict[key] = std::to_string(intVal.value());
                    continue;
                }
                auto doubleVal = self.get<double>(key);
                if (doubleVal.has_value()) {
                    dict[key] = std::to_string(doubleVal.value());
                    continue;
                }
                auto boolVal = self.get<bool>(key);
                if (boolVal.has_value()) {
                    dict[key] = boolVal.value() ? "true" : "false";
                    continue;
                }
                auto stringVal = self.get<std::string>(key);
                if (stringVal.has_value()) {
                    dict[key] = stringVal.value();
                    continue;
                }
            }
            return dict;
        }, "Convert configuration to Python dictionary")
        .def("fromDict", [](Config& self, const std::unordered_map<std::string, std::string>& configDict) {
            for (const auto& pair : configDict) {
                // Try to infer type from string value
                const auto& valueStr = pair.second;
                // Simple type inference
                try {
                    // Try to parse as integer
                    size_t pos;
                    int intVal = std::stoi(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        self.set(pair.first, intVal, nlm::ConfigSource::Runtime);
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try to parse as float
                    size_t pos;
                    double floatVal = std::stod(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        self.set(pair.first, floatVal, nlm::ConfigSource::Runtime);
                        continue;
                    }
                } catch (...) {}
                
                if (valueStr == "true" || valueStr == "false") {
                    bool boolVal = (valueStr == "true");
                    self.set(pair.first, boolVal, nlm::ConfigSource::Runtime);
                    continue;
                }
                
                // Default to string
                self.set(pair.first, valueStr, nlm::ConfigSource::Runtime);
            }
        }, py::arg("config_dict"),
             "Load configuration from Python dictionary with automatic type inference")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def(py::init<>(), R"pbdoc(Create brain with default configuration)pbdoc")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initializeWithConfig", [](Brain& self, const std::unordered_map<std::string, nlm::ConfigValue>& configMap) {
            auto config = std::make_shared<Config>();
            for (const auto& pair : configMap) {
                config->set(pair.first, pair.second, nlm::ConfigSource::User);
            }
            self.getConfig()->copyFrom(*config);
            return self.initialize();
        }, py::arg("config_map"),
             "Initialize brain with configuration provided as a Python dictionary")
        .def("quickStart", [](Brain& self, int neuronCount, int regionCount, float connectionProbability) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::User);
            config->set("connection_probability", connectionProbability, nlm::ConfigSource::User);
            self.getConfig()->copyFrom(*config);
            return self.initialize();
        }, py::arg("neuron_count"), py::arg("region_count"), py::arg("connection_probability"),
             "Quick initialization with common parameters (convenience method)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("runSimulation", [](Brain& self, int numSteps, Timestamp startTime = 0) {
            for (nlm::SimulationStep step = 0; step < numSteps; ++step) {
                self.step(step, startTime + step);
            }
        }, py::arg("num_steps"), py::arg("start_time") = 0,
             "Run multiple simulation steps (convenience method)")
        .def("runEpisode", [](Brain& self, const class SensoryInput& initialInput, 
                            int numSteps, Timestamp startTime = 0) {
            if (initialInput) {
                self.receiveSensoryInput(initialInput);
            }
            for (nlm::SimulationStep step = 0; step < numSteps; ++step) {
                self.step(step, startTime + step);
            }
        }, py::arg("initial_input"), py::arg("num_steps"), py::arg("start_time") = 0,
             "Run a complete simulation episode with initial input (convenience method)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("getDetailedStats", [](const Brain& self) {
            std::unordered_map<std::string, std::variant<int, double, std::string>> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spike_count"] = self.getTotalSpikeCount();
            stats["pending_spike_events"] = self.getPendingSpikeEventCount();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get detailed statistics as Python dictionary")
        .def("injectPattern", [](Brain& self, const std::vector<float>& pattern, NeuronType type = NeuronType::Sensory) {
            auto sensoryInput = std::make_shared<nlm::SensoryInput>();
            for (float value : pattern) {
                sensoryInput->addSignal(value);
            }
            self.receiveSensoryInput(*sensoryInput);
            self.injectCurrentToNeurons(type, 1.0f);
        }, py::arg("pattern"), py::arg("type") = NeuronType::Sensory,
             "Inject a current pattern into neurons of specified type")
        .def("batchStep", [](Brain& self, int numSteps, Timestamp startTime = 0, 
                            const std::vector<float>& initialInput = std::vector<float>()) {
            if (!initialInput.empty()) {
                auto sensoryInput = std::make_shared<nlm::SensoryInput>();
                for (float value : initialInput) {
                    sensoryInput->addSignal(value);
                }
                self.receiveSensoryInput(*sensoryInput);
            }
            
            nlm::SimulationStep currentStep = startTime;
            for (int i = 0; i < numSteps; ++i) {
                self.step(currentStep, startTime + i);
                currentStep = startTime + i + 1;
            }
            
            return numSteps;
        }, py::arg("num_steps"), py::arg("start_time") = 0, py::arg("initial_input") = std::vector<float>(),
             "Perform multiple simulation steps in batch")
        .def("createAdvancedConfig", [](int neuronCount, int regionCount, float connectionProbability,
                               float learningRate, bool enablePlasticity, bool enableDevelopment) {
            auto config = std::make_shared<Config>();
            config->setInt("neuron_count", neuronCount);
            config->setInt("region_count", regionCount);
            config->setFloat("connection_probability", connectionProbability);
            config->setFloat("learning_rate", learningRate);
            config->setString("plasticity_enabled", enablePlasticity ? "true" : "false");
            config->setString("development_enabled", enableDevelopment ? "true" : "false");
            return config;
        }, py::arg("neuron_count"), py::arg("region_count"), py::arg("connection_probability"),
             py::arg("learning_rate"), py::arg("enable_plasticity") = true, py::arg("enable_development") = true,
             "Create an advanced configuration with typical parameters")
        .def("getRegionInfo", [](const Brain& self, const std::string& regionName = "") {
            auto regions = self.getRegions();
            std::vector<std::unordered_map<std::string, std::string>> regionInfoList;
            
            for (const auto& regionPtr : regions) {
                if (regionName.empty() || regionPtr->getName() == regionName) {
                    std::unordered_map<std::string, std::string> info;
                    info["name"] = regionPtr->getName();
                    info["id"] = "RegionId:" + std::to_string(regionPtr->getId().value);
                    info["neuron_count"] = std::to_string(regionPtr->getNeuronCount());
                    info["synapse_count"] = std::to_string(regionPtr->getSynapseCount());
                    regionInfoList.push_back(info);
                }
            }
            
            return regionInfoList;
        }, py::arg("region_name") = "",
             "Get detailed information about regions, optionally filtered by name")
        .def("getConnectivityInfo", [](const Brain& self) {
            std::unordered_map<std::string, int> connectivityStats;
            connectivityStats["total_regions"] = self.getRegionCount();
            connectivityStats["total_neurons"] = self.getTotalNeuronCount();
            connectivityStats["total_synapses"] = self.getTotalSynapseCount();
            connectivityStats["avg_neurons_per_region"] = self.getRegionCount() > 0 ? 
                self.getTotalNeuronCount() / self.getRegionCount() : 0;
            connectivityStats["avg_synapses_per_region"] = self.getRegionCount() > 0 ? 
                self.getTotalSynapseCount() / self.getRegionCount() : 0;
            return connectivityStats;
        }, "Get connectivity statistics")
        .def("getPlasticityState", [](const Brain& self) {
            std::unordered_map<std::string, bool> plasticityState;
            auto stdp = self.getSTDP();
            auto hebbian = self.getHebbian();
            auto structural = self.getStructuralPlasticity();
            
            plasticityState["stdp_enabled"] = stdp != nullptr;
            plasticityState["hebbian_enabled"] = hebbian != nullptr;
            plasticityState["structural_plasticity_enabled"] = structural != nullptr;
            
            return plasticityState;
        }, "Get current plasticity system state")
        .def("resetWithConfig", [](Brain& self, std::shared_ptr<Config> config) {
            self.getConfig()->copyFrom(*config);
            return self.initialize();
        }, py::arg("config"), "Reset brain with new configuration")
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

    py::class_<ContextManager>(m, "ContextManager", R"pbdoc(Context manager for NLM simulation resources)pbdoc")
        .def(py::init<>())
        .def("__enter__", [](ContextManager& self) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                self.brain = brain;
                return brain;
            }
            throw std::runtime_error("Failed to initialize brain");
        }, R"pbdoc(Enter the context manager and initialize a brain)pbdoc")
        .def("__exit__", [](ContextManager& self, py::args) {
            if (self.brain) {
                self.brain->reset();
                self.brain.reset();
            }
        }, R"pbdoc(Exit the context manager and clean up resources)pbdoc")
        .def("withConfig", [](const std::shared_ptr<Config>& config) {
            return ContextManager(config);
        }, py::arg("config"), R"pbdoc(Create context manager with custom configuration)pbdoc")
        .def("withQuickConfig", [](int neuronCount, int regionCount) {
            auto config = std::make_shared<Config>();
            config->setInt("neuron_count", neuronCount);
            config->setInt("region_count", regionCount);
            return ContextManager(config);
        }, py::arg("neuron_count"), py::arg("region_count"), R"pbdoc(Create context manager with quick configuration)pbdoc")
        .def("getBrain", [](ContextManager& self) {
            if (!self.brain) {
                throw std::runtime_error("Brain not initialized. Use brain() to get or initialize.");
            }
            return self.brain;
        }, R"pbdoc(Get the brain instance from the context manager)pbdoc")
        .def("getConfig", [](ContextManager& self) {
            auto config = std::make_shared<Config>();
            if (self.brain && self.brain->getConfig()) {
                config->copyFrom(*self.brain->getConfig());
            }
            return config;
        }, R"pbdoc(Get configuration from context manager)pbdoc")
        .def("runEpisode", [](ContextManager& self, const class SensoryInput& initialInput, int numSteps, Timestamp startTime = 0) {
            if (!self.brain) {
                throw std::runtime_error("Brain not initialized. Use brain() to get or initialize.");
            }
            
            if (initialInput) {
                self.brain->receiveSensoryInput(initialInput);
            }
            
            nlm::SimulationStep currentStep = startTime;
            for (int i = 0; i < numSteps; ++i) {
                self.brain->step(currentStep, startTime + i);
                currentStep = startTime + i + 1;
            }
            
            return numSteps;
        }, py::arg("initial_input"), py::arg("num_steps"), py::arg("start_time") = 0, R"pbdoc(Run a simulation episode within the context)pbdoc")
        .def("batchProcess", [](ContextManager& self, const std::vector<std::pair<int, std::vector<float>>>& episodes) {
            if (!self.brain) {
                throw std::runtime_error("Brain not initialized. Use brain() to get or initialize.");
            }
            
            std::vector<int> results;
            for (const auto& episode : episodes) {
                self.brain->reset();
                
                nlm::SimulationStep currentStep = 0;
                for (int step = 0; step < episode.first; ++step) {
                    self.brain->step(currentStep, step);
                    currentStep = step + 1;
                }
                
                results.push_back(step);
            }
            
            return results;
        }, py::arg("episodes"), R"pbdoc(Run multiple episodes in batch, resetting between each)pbdoc");

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

    m.def("createContextManager", []() -> std::shared_ptr<ContextManager> {
        return std::make_shared<ContextManager>();
    }, "Create a context manager for resource management");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Context manager implementation
class ContextManager {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    ContextManager() = default;
    explicit ContextManager(std::shared_ptr<Config> cfg) : config(cfg) {}
    
    std::shared_ptr<Brain> brain() {
        if (!brain) {
            throw std::runtime_error("Brain not initialized. Use brain() to get or initialize.");
        }
        return brain;
    }
    
    std::shared_ptr<Config> config() {
        auto cfg = std::make_shared<Config>();
        if (brain && brain->getConfig()) {
            cfg->copyFrom(*brain->getConfig());
        }
        return cfg;
    }
    
    int runEpisode(const SensoryInput& initialInput, int numSteps, Timestamp startTime = 0) {
        if (!brain) {
            throw std::runtime_error("Brain not initialized. Use brain() to get or initialize.");
        }
        
        if (initialInput) {
            brain->receiveSensoryInput(initialInput);
        }
        
        nlm::SimulationStep currentStep = startTime;
        for (int i = 0; i < numSteps; ++i) {
            brain->step(currentStep, startTime + i);
            currentStep = startTime + i + 1;
        }
        
        return numSteps;
    }
    
    std::vector<int> batchProcess(const std::vector<std::pair<int, std::vector<float>>>& episodes) {
        if (!brain) {
            throw std::runtime_error("Brain not initialized. Use brain() to get or initialize.");
        }
        
        std::vector<int> results;
        for (const auto& episode : episodes) {
            brain->reset();
            
            nlm::SimulationStep currentStep = 0;
            for (int step = 0; step < episode.first; ++step) {
                brain->step(currentStep, step);
                currentStep = step + 1;
            }
            
            results.push_back(step);
        }
        
        return results;
    }
};

} // namespace nlm