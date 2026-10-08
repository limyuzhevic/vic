#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================

        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides advanced interface for brain simulation, learning, and experimentation.

        Key Features:
        - Real spiking neural network simulation
        - Advanced learning mechanisms (STDP, Hebbian, structural plasticity)
        - Integrated memory systems (working, episodic, associative)
        - Cognitive functions (planning, prediction, attention, concept formation)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental systems
        - Sensorimotor integration
        - World simulation and interaction
        - Experiment management and analysis
        - Advanced configuration system
        - Comprehensive logging and error handling
        - Performance monitoring and optimization
        - Python-friendly API with type safety
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // Type definitions
    py::class_<NeuronId>(m, "NeuronId", "Unique identifier for a neuron")
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

    py::class_<SynapseId>(m, "SynapseId", "Unique identifier for a synapse")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Enum definitions
    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Advanced Config class with enhanced functionality
    py::class_<Config>(m, "Config", "Enhanced configuration class for NLM system")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(
                 &Config::set), py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(
                 &Config::set), py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(
                 &Config::set), py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(
                 &Config::set), py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("getInt", py::overload_cast<const std::string&>(&Config::get<int>),
             py::arg("key"), "Get integer value")
        .def("getFloat", py::overload_cast<const std::string&>(&Config::get<double>),
             py::arg("key"), "Get float value")
        .def("getString", py::overload_cast<const std::string&>(&Config::get<std::string>),
             py::arg("key"), "Get string value")
        .def("getBool", py::overload_cast<const std::string&>(&Config::get<bool>),
             py::arg("key"), "Get boolean value")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get int with default")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float with default")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string with default")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get bool with default");

    // Enhanced Action system with parameters and serialization
    py::class_<Action>(m, "Action", "Advanced action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_readwrite("type", &Action::getType, &Action::setType)
        .def_readwrite("parameters", &Action::getParameters, &Action::setParameters)
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def_static("fromDict", [](py::dict data) {
            Action action;
            if (data.contains("type")) {
                action.setType(py::cast<ActionType>(data["type"]));
            }
            if (data.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(data["parameters"]));
            }
            return action;
        }, py::arg("data"), "Create action from Python dictionary");

    // Enhanced world and agent classes
    py::class_<WorldObject>(m, "WorldObject", "World object representation")
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

    py::class_<AgentBody>(m, "AgentBody", "Enhanced agent body state")
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
        .def("toDict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert body to dictionary")
        .def_static("fromDict", [](py::dict data) {
            AgentBody body;
            if (data.contains("x")) body.x = data["x"].cast<float>();
            if (data.contains("y")) body.y = data["y"].cast<float>();
            if (data.contains("orientation")) body.orientation = data["orientation"].cast<float>();
            if (data.contains("velocityX")) body.velocityX = data["velocityX"].cast<float>();
            if (data.contains("velocityY")) body.velocityY = data["velocityY"].cast<float>();
            if (data.contains("angularVelocity")) body.angularVelocity = data["angularVelocity"].cast<float>();
            if (data.contains("energy")) body.energy = data["energy"].cast<float>();
            if (data.contains("health")) body.health = data["health"].cast<float>();
            if (data.contains("age")) body.age = data["age"].cast<float>();
            if (data.contains("isMoving")) body.isMoving = data["isMoving"].cast<bool>();
            if (data.contains("isTurning")) body.isTurning = data["isTurning"].cast<bool>();
            if (data.contains("lastActionTime")) body.lastActionTime = data["lastActionTime"].cast<float>();
            return body;
        }, py::arg("data"), "Create body from dictionary");

    // Enhanced sensory system
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    // Agent brain with advanced interface
    py::class_<AgentBrain>(m, "AgentBrain", "Advanced agent brain interface")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all advanced systems")
        .def("getSystemStatus", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            status["developmental_stage"] = self.getDevelopmentalStage();
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            return status;
        }, "Get system status as dictionary");

    // Advanced brain interface with performance monitoring
    py::class_<Brain>(m, "Brain", "Central neural simulation brain with performance monitoring")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
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
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status and performance metrics")
        .def("getPerformanceStats", [](const Brain& self) {
            py::dict stats;
            stats["neuron_count"] = self.getTotalNeuronCount();
            stats["synapse_count"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get brain performance statistics");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with advanced settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Logging utilities
    m.def("logDebug", &Logger::getGlobal().debug,
          py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
          py::arg("function") = "", "Log debug message");

    m.def("logInfo", &Logger::getGlobal().info,
          py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
          py::arg("function") = "", "Log info message");

    m.def("logWarning", &Logger::getGlobal().warning,
          py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
          py::arg("function") = "", "Log warning message");

    m.def("logError", &Logger::getGlobal().error,
          py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
          py::arg("function") = "", "Log error message");

    m.def("logCritical", &Logger::getGlobal().critical,
          py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
          py::arg("function") = "", "Log critical message");

    // Performance utilities
    m.def("benchmark", [](std::function<void()> func, int iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        return duration / iterations; // average milliseconds per iteration
    }, py::arg("func"), py::arg("iterations"), "Benchmark function performance");

    // Math utilities
    m.def("normalizeVector", [](const std::vector<float>& input) {
        float magnitude = 0.0f;
        for (float val : input) magnitude += val * val;
        magnitude = std::sqrt(magnitude);
        
        std::vector<float> result;
        if (magnitude > 0.0f) {
            for (float val : input) {
                result.push_back(val / magnitude);
            }
        }
        return result;
    }, py::arg("input"), "Normalize vector to unit length");

    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Clamp value between min and max");

    m.def("smoothStep", [](float edge0, float edge1, float x) {
        if (x < edge0) return 0.0f;
        if (x >= edge1) return 1.0f;
        float t = (x - edge0) / (edge1 - edge0);
        return t * t * (3.0f - 2.0f * t);
    }, py::arg("edge0"), py::arg("edge1"), py::arg("x"), "Smooth Hermite interpolation");

    // Demonstration and example utilities
    m.def("createLearningEnvironment", []() {
        py::dict env;
        env["width"] = 100;
        env["height"] = 100;
        env["num_neurons"] = 500;
        env["learning_rate"] = 0.1f;
        env["plasticity_enabled"] = true;
        env["memory_capacity"] = 1000;
        return env;
    }, "Create example learning environment configuration");

    // Advanced error handling
    m.def("safeExecute", [](std::function<py::object()> func) {
        try {
            return func();
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Execution failed: ") + e.what());
        }
    }, py::arg("func"), "Safely execute function with error handling");

    // Serialization utilities
    m.def("serializeBrainState", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Serialize brain state to file");

    m.def("deserializeBrainState", [](const std::string& filepath, std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            return brain;
        }
        throw py::value_error("Failed to deserialize brain state");
    }, py::arg("filepath"), py::arg("config") = py::none(), "Deserialize brain state from file");

    // Documentation and help
    m.def("getVersion", []() {
        return "NLM Python Bindings - Enhanced Version 0.1.0";
    }, "Get version information");

    m.def("getDocumentation", []() {
        return R"doc(
            NLM Python API Documentation:
            
            Classes:
            - Config: Enhanced configuration system
            - Brain: Advanced neural brain simulation
            - AgentBrain: Agent interface with sensory-motor integration
            - Action: Motor actions with parameters and serialization
            - SensoryInput: Sensory input abstractions (Vision, Audio, Internal)
            - WorldObject: 2D world objects for agent interaction
            - AgentBody: Agent physical state representation
            
            Enums:
            - NeuronType: Excitatory, Inhibitory, Sensory, Motor, etc.
            - SynapseType: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
            - DevelopmentalStage: Initial, CriticalPeriod, Maturation, Adult, Aging
            - ActionType: MoveForward, TurnLeft, Look, Interact, Eat, Drink, Rest, Wait, Custom
            
            Key Methods:
            - Brain.step(): Core simulation step
            - AgentBrain.processSensoryInput(): Sensory processing
            - AgentBrain.decodeMotorCommand(): Action selection
            - Config.set()/get(): Configuration management
            - Action.clone()/toDict()/fromDict(): Action serialization
            
            Advanced Features:
            - Performance monitoring and benchmarking
            - Memory management and optimization
            - Error handling and recovery
            - Serialization/deserialization
            - Development system integration
            - Neuromodulation support
            - Experiment management
        )doc";
    }, "Get comprehensive API documentation");

    // Custom exception types for advanced error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

} // namespace nlm
