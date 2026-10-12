#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>

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

// Config class improvements
void Config_loadConfigFile(py::object& self, const std::string& filepath) {
    auto config = py::cast<std::shared_ptr<Config>>(self);
    bool success = config->loadFromFile(filepath);
    if (!success) {
        throw py::value_error("Failed to load config file: " + filepath);
    }
}

void Config_update(py::object& self, const py::kwargs& kwargs) {
    auto config = py::cast<std::shared_ptr<Config>>(self);
    for (auto item : kwargs) {
        std::string key = item.first.cast<std::string>();
        py::object value = item.second;
        
        // Try to infer value type
        if (value.is(py::int_())) {
            config->set(key, value.cast<int64_t>(), ConfigSource::Runtime);
        } else if (value.is(py::float_())) {
            config->set(key, value.cast<double>(), ConfigSource::Runtime);
        } else if (value.is(py::bool_())) {
            config->set(key, value.cast<bool>(), ConfigSource::Runtime);
        } else if (value.is(py::str())) {
            config->set(key, value.cast<std::string>(), ConfigSource::Runtime);
        } else if (value.is(py::list())) {
            // Try to parse as array of known types
            py::list list = value.cast<py::list>();
            std::vector<double> doubleArray;
            std::vector<int64_t> intArray;
            std::vector<std::string> stringArray;
            
            bool allDoubles = true;
            bool allInts = true;
            bool allStrings = true;
            
            for (auto item : list) {
                if (item.is(py::float_())) {
                    if (allInts) intArray.push_back(item.cast<int64_t>());
                    doubleArray.push_back(item.cast<double>());
                    allStrings = false;
                } else if (item.is(py::int_())) {
                    intArray.push_back(item.cast<int64_t>());
                    allDoubles = false;
                    allStrings = false;
                } else if (item.is(py::str())) {
                    stringArray.push_back(item.cast<std::string>());
                    allDoubles = false;
                    allInts = false;
                } else {
                    allDoubles = false;
                    allInts = false;
                    allStrings = false;
                }
            }
            
            if (allDoubles) {
                config->set(key, doubleArray, ConfigSource::Runtime);
            } else if (allInts) {
                config->set(key, intArray, ConfigSource::Runtime);
            } else if (allStrings) {
                config->set(key, stringArray, ConfigSource::Runtime);
            } else {
                throw py::type_error("Unsupported list type in kwargs");
            }
        } else {
            throw py::type_error("Unsupported value type for key: " + key);
        }
    }
}

void Config_validate(py::object& self, const std::string& expectedTypes = "") {
    auto config = py::cast<std::shared_ptr<Config>>(self);
    std::string errors;
    
    // Basic validation
    if (config->has("validation_required_key")) {
        errors += "Missing required configuration key: validation_required_key. ";
    }
    
    // Validate if expectedTypes is provided
    if (!expectedTypes.empty()) {
        auto typeMap = parseExpectedTypes(expectedTypes);
        for (const auto& [key, expectedType] : typeMap) {
            if (!config->has(key)) {
                errors += "Missing configuration key: " + key + ". ";
            } else {
                // Validate type (simplified - in real implementation would do proper type checking)
            }
        }
    }
    
    if (!errors.empty()) {
        throw py::value_error("Configuration validation failed: " + errors);
    }
}

// Brain class improvements
class BrainRunner {
public:
    BrainRunner(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainRunner() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

void Brain_run(py::object& self, double duration, double timestep) {
    auto brain = py::cast<std::shared_ptr<Brain>>(self);
    
    auto startTime = std::chrono::steady_clock::now();
    double currentStep = 0;
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime).count() / 1000.0 < duration) {
        brain->step(static_cast<SimulationStep>(currentStep++));
        std::this_thread::sleep_for(std::chrono::duration<double>(timestep));
    }
}

void Brain_getNeuronsByType(py::object& self, NeuronType neuronType, 
                           const py::kwargs& kwargs) {
    auto brain = py::cast<std::shared_ptr<Brain>>(self);
    
    size_t startIdx = kwargs.value("start", 0UL);
    size_t count = kwargs.value("count", static_cast<size_t>(-1)); // All if -1
    
    // This is a simplified implementation - in practice would access brain's internal regions
    std::vector<NeuronId> neurons;
    
    // Placeholder implementation
    return py::cast(neurons);
}

void Brain_getConnections(py::object& self, const py::kwargs& kwargs) {
    auto brain = py::cast<std::shared_ptr<Brain>>(self);
    
    RegionId sourceRegion = kwargs.value("source", INVALID_REGION_ID);
    RegionId targetRegion = kwargs.value("target", INVALID_REGION_ID);
    
    // This is a simplified implementation - in practice would access brain's internal connections
    std::vector<std::pair<NeuronId, NeuronId>> connections;
    
    // Placeholder implementation
    return py::cast(connections);
}

void Brain_benchmark(py::object& self, const py::kwargs& kwargs) {
    auto brain = py::cast<std::shared_ptr<Brain>>(self);
    
    int steps = kwargs.value("steps", 100);
    int warmup = kwargs.value("warmup", 10);
    
    // Warmup
    for (int i = 0; i < warmup; ++i) {
        brain->reset();
        brain->initialize();
        brain->step(0);
    }
    
    // Benchmark
    auto startTime = std::chrono::steady_clock::now();
    for (int i = 0; i < steps; ++i) {
        brain->reset();
        brain->initialize();
        brain->step(0);
    }
    auto endTime = std::chrono::steady_clock::now();
    
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double avgTime = totalTime / steps;
    double stepsPerSecond = steps / totalTime;
    
    std::map<std::string, double> results;
    results["total_time_seconds"] = totalTime;
    results["average_time_per_step_seconds"] = avgTime;
    results["steps_per_second"] = stepsPerSecond;
    results["steps"] = static_cast<double>(steps);
    
    return py::cast(results);
}

// SimpleWorld class improvements
void SimpleWorld_add_object_at(py::object& self, float x, float y, 
                             WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
    auto world = py::cast<std::shared_ptr<SimpleWorld>>(self);
    
    WorldObject obj(x, y, type, value, radius);
    world->addObject(obj);
}

void SimpleWorld_get_nearby_objects(py::object& self, float x, float y, 
                                    float radius, const py::kwargs& kwargs) {
    auto world = py::cast<std::shared_ptr<SimpleWorld>>(self);
    
    bool activeOnly = kwargs.value("active_only", true);
    WorldObjectType typeFilter = kwargs.value("type_filter", WorldObjectType::Empty);
    
    std::vector<WorldObject> nearbyObjects;
    
    // Simplified implementation - would need to access world's objects in practice
    // This is a placeholder for spatial query functionality
    
    return py::cast(nearbyObjects);
}

void SimpleWorld_configure_simulation(py::object& self, float width, float height,
                                     size_t visionWidth = 32, size_t visionHeight = 32,
                                     float maxEnergy = 100.0f, float energyDecayRate = 0.1f) {
    auto world = py::cast<std::shared_ptr<SimpleWorld>>(self);
    
    world->configure(width, height, visionWidth, visionHeight);
    world->setMaxEnergy(maxEnergy);
    world->setEnergyDecayRate(energyDecayRate);
}

// AgentBrain class improvements
class EpisodeRunner {
public:
    EpisodeRunner(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
    ~EpisodeRunner() {
        if (agentBrain_) {
            agentBrain_->reset();
        }
    }
    
    std::shared_ptr<AgentBrain> getAgentBrain() { return agentBrain_; }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

void AgentBrain_episode(py::object& self, const py::kwargs& kwargs) {
    auto agentBrain = py::cast<std::shared_ptr<AgentBrain>>(self);
    
    double maxDuration = kwargs.value("max_duration", 100.0);
    bool verbose = kwargs.value("verbose", false);
    
    // Placeholder for episode context manager
    return py::cast(EpisodeRunner(agentBrain));
}

void AgentBrain_act(py::object& self, const SensoryPercept& percept) {
    auto agentBrain = py::cast<std::shared_ptr<AgentBrain>>(self);
    
    agentBrain->processSensoryInput(percept);
    return agentBrain->decodeMotorCommand();
}

void AgentBrain_observe(py::object& self, const SensoryPercept& percept) {
    auto agentBrain = py::cast<std::shared_ptr<AgentBrain>>(self);
    
    agentBrain->processSensoryInput(percept);
}

// New utility functions
void create_simulation_config(py::object& self, const std::string& filename = "") {
    auto config = std::make_shared<Config>();
    
    // Create basic simulation configuration
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1, ConfigSource::Default);
    config->set("max_firing_rate", 50.0, ConfigSource::Default);
    config->set("min_firing_rate", 1.0, ConfigSource::Default);
    config->set("learning_rate", 0.01, ConfigSource::Default);
    config->set("plasticity_enabled", true, ConfigSource::Default);
    config->set("development_enabled", true, ConfigSource::Default);
    config->set("reward_modulation_enabled", true, ConfigSource::Default);
    
    if (!filename.empty()) {
        config->saveToFile(filename);
    }
    
    return config;
}

void create_test_environment(py::object& self, float width = 100.0f, float height = 100.0f) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, 32, 32);
    
    // Add some test objects
    WorldObject resource1(width * 0.25f, height * 0.25f, WorldObjectType::Resource, 10.0f);
    WorldObject resource2(width * 0.75f, height * 0.75f, WorldObjectType::Resource, 15.0f);
    WorldObject wall1(width * 0.5f, height * 0.5f, WorldObjectType::Wall, 0.0f, 2.0f);
    
    world->addObject(resource1);
    world->addObject(resource2);
    world->addObject(wall1);
    
    // Set agent start
    world->setAgentStart(width * 0.5f, height * 0.5f);
    
    return world;
}

void create_training_environment(py::object& self, float width = 100.0f, float height = 100.0f) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, 64, 64);
    
    // Add training objects
    for (int i = 0; i < 10; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * width;
        float y = static_cast<float>(rand()) / RAND_MAX * height;
        WorldObjectType type = (i % 3 == 0) ? WorldObjectType::Resource : 
                              (i % 3 == 1) ? WorldObjectType::Hazard : WorldObjectType::Marker;
        float value = (type == WorldObjectType::Resource) ? 10.0f : 
                     (type == WorldObjectType::Hazard) ? -5.0f : 0.0f;
        
        WorldObject obj(x, y, type, value);
        world->addObject(obj);
    }
    
    // Set agent start
    world->setAgentStart(width * 0.5f, height * 0.5f);
    
    return world;
}

// Helper function for parsing expected types
std::map<std::string, std::string> parseExpectedTypes(const std::string& expectedTypes) {
    std::map<std::string, std::string> typeMap;
    
    // Simple parsing: "key1:type1,key2:type2"
    size_t pos = 0;
    std::string token;
    while ((pos = expectedTypes.find(',')) != std::string::npos) {
        token = expectedTypes.substr(0, pos);
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            std::string key = token.substr(0, colonPos);
            std::string type = token.substr(colonPos + 1);
            typeMap[key] = type;
        }
        expectedTypes.erase(0, pos + 1);
    }
    
    // Handle last token
    size_t colonPos = expectedTypes.find(':');
    if (colonPos != std::string::npos) {
        std::string key = expectedTypes.substr(0, colonPos);
        std::string type = expectedTypes.substr(colonPos + 1);
        typeMap[key] = type;
    }
    
    return typeMap;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced Pythonic configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadConfigFile", &Config_loadConfigFile, py::arg("filepath"),
             "Pythonic convenience method to load configuration from file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("update", &Config_update, py::arg("kwargs") = py::kwargs(),
             "Pythonic batch update using keyword arguments")
        .def("validate", &Config_validate, py::arg("expectedTypes") = "",
             "Validate configuration and raise error if invalid")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced Pythonic brain class)pbdoc")
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
             "Log brain status")
        // New Pythonic methods
        .def("run", &Brain_run, py::arg("duration"), py::arg("timestep") = 0.01,
             "Run brain simulation for a duration (context manager style)")
        .def("getNeuronsByType", &Brain_getNeuronsByType,
             py::arg("type"), py::arg("kwargs") = py::kwargs(),
             "Get neurons by type with optional filtering")
        .def("getConnections", &Brain_getConnections,
             py::arg("kwargs") = py::kwargs(),
             "Get connections between regions/neurons")
        .def("benchmark", &Brain_benchmark,
             py::arg("kwargs") = py::kwargs(),
             "Benchmark brain performance")
        .def("__repr__", [](const Brain& b) {
            auto config = b.getConfig();
            auto summary = config->summary();
            return "<Brain: " + summary + ">";
        });

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced Pythonic agent brain interface)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // New Pythonic methods
        .def("episode", &AgentBrain_episode,
             py::arg("kwargs") = py::kwargs(),
             "Create a new episode context manager")
        .def("act", &AgentBrain_act,
             py::arg("percept"),
             "Perform an action given sensory input")
        .def("observe", &AgentBrain_observe,
             py::arg("percept"),
             "Observe sensory input without acting")
        .def("__repr__", [](const AgentBrain& ab) {
            auto brain = ab.getBrain();
            return "<AgentBrain: " + std::to_string(brain->getTotalNeuronCount()) + " neurons>";
        });

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced Pythonic world simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("configure_simulation", &SimpleWorld_configure_simulation,
             py::arg("width") = 100.0f, py::arg("height") = 100.0f,
             py::arg("visionWidth") = 32, py::arg("visionHeight") = 32,
             py::arg("maxEnergy") = 100.0f, py::arg("energyDecayRate") = 0.1f,
             "Quick configuration of simulation parameters")
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
        .def("add_object_at", &SimpleWorld_add_object_at,
             py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Pythonic method to add object at specific position")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("get_nearby_objects", &SimpleWorld_get_nearby_objects,
             py::arg("x"), py::arg("y"), py::arg("radius"),
             py::arg("kwargs") = py::kwargs(),
             "Get objects within radius (Pythonic method)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" + std::to_string(w.getHeight()) + ">";
        });

    // Legacy factory functions (backward compatibility)
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

    // New utility functions
    m.def("create_simulation_config", &create_simulation_config,
          py::arg("filename") = "",
          "Create a comprehensive simulation configuration");

    m.def("create_test_environment", &create_test_environment,
          py::arg("width") = 100.0f, py::arg("height") = 100.0f,
          "Create a test environment with sample objects");

    m.def("create_training_environment", &create_training_environment,
          py::arg("width") = 100.0f, py::arg("height") = 100.0f,
          "Create a training environment with reward/hazard objects");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

// Required includes and thread support
#include <thread>
#include <chrono>