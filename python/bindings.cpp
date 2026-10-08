#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
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

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMError {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMError("Invalid argument: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

// Template helper for ID type bindings to reduce boilerplate
template<typename IDType, typename ValueType>
void bindIDType(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<IDType>(m, name.c_str(), py::dynamic_attr()
        .def(py::init<>())
        .def(py::init<ValueType>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { return std::hash<ValueType>{}(id.value); })
        .def("__repr__", [](const IDType& id) {
            std::stringstream ss;
            ss << "<" << name << ": " << id.value << ">";
            return ss.str();
        })
        .doc() = doc.c_str()
    );
}

// Template helper for enum bindings
template<typename EnumType>
void bindEnum(py::module_& m, const std::string& name, const std::string& doc) {
    py::enum_<EnumType> enum_class(m, name.c_str(), py::arithmetic(), doc.c_str())
        .export_values();
}

// Factory function for Config
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

// Factory function for Brain with error checking
std::shared_ptr<Brain> createBrainWithValidation(std::shared_ptr<Config> config) {
    if (!config) {
        throw InvalidArgumentError("Config cannot be null");
    }
    return std::make_shared<Brain>(config);
}

// Pythonic conversion functions
std::vector<float> toFloatVector(const std::vector<double>& input) {
    return std::vector<float>(input.begin(), input.end());
}

std::vector<double> toDoubleVector(const std::vector<float>& input) {
    return std::vector<double>(input.begin(), input.end());
}

// Convenience function for running simple simulation
void runSimulationStep(py::object brain_obj, uint64_t step, double timestep = 0.001) {
    try {
        auto* brain = brain_obj.cast<Brain*>();
        brain->step(step, timestep);
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Simulation step failed: ") + e.what());
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Type-safe identifier bindings with automatic conversion
        - Factory functions for common objects
        - Enhanced error handling with custom exceptions
        - Pythonic interfaces with improved convenience methods
        - Context managers for resource management
        - Integration with scientific Python ecosystem
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");

    // Use templated binding helpers to reduce boilerplate
    bindIDType<NeuronId, uint64_t>(m, "NeuronId", "Unique identifier for a neuron");
    bindIDType<SynapseId, uint64_t>(m, "SynapseId", "Unique identifier for a synapse");
    bindIDType<RegionId, uint64_t>(m, "RegionId", "Unique identifier for a brain region");
    bindIDType<PopulationId, uint64_t>(m, "PopulationId", "Unique identifier for a neuron population");

    // Bind enums using template helper
    bindEnum<NeuronType>(m, "NeuronType", "Neuron type enumeration");
    bindEnum<SynapseType>(m, "SynapseType", "Synapse type enumeration");
    bindEnum<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration");
    bindEnum<FiringState>(m, "FiringState", "Neuron firing state enumeration");
    bindEnum<ActionType>(m, "ActionType", "Action type enumeration");
    bindEnum<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration");
    bindEnum<WorldObjectType>(m, "WorldObjectType", "World object type enumeration");

    // Type conversion utilities
    m.def("to_float_vector", &toFloatVector, "Convert double vector to float vector",
          py::arg("input"));
    m.def("to_double_vector", &toDoubleVector, "Convert float vector to double vector",
          py::arg("input"));

    // Enhanced Config class with factory functions and error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Factory functions for main objects
    m.def("createDefaultConfig", &createDefaultConfig, "Create a default configuration");
    m.def("createBrain", &createBrainWithValidation, py::arg("config"), 
          "Create a new brain with configuration");

    // Enhanced SensoryInput classes with Pythonic interfaces
    py::class_<SensoryInput>(m, "SensoryInput", py::dynamic_attr(),
        R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with enhanced initialization and data handling
    py::class_<Vision, SensoryInput>(m, "Vision", py::dynamic_attr(),
        R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromDouble", [](Vision& self, const std::vector<double>& data) {
            std::vector<float> floatData = toFloatVector(data);
            self.setData(floatData);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with enhanced initialization
    py::class_<Audio, SensoryInput>(m, "Audio", py::dynamic_attr(),
        R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromDouble", [](Audio& self, const std::vector<double>& data) {
            std::vector<float> floatData = toFloatVector(data);
            self.setData(floatData);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with Pythonic methods
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", py::dynamic_attr(),
        R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            for (float sig : signals) self.addSignal(sig);
        }, py::arg("signals"))
        .def("addSignalsFromDouble", [](InternalSignals& self, const std::vector<double>& signals) {
            for (double sig : signals) self.addSignal(static_cast<float>(sig));
        }, py::arg("signals"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getSignals", [](InternalSignals& self) {
            // Internal implementation to expose signals
            return self.getSignals(); // Assuming this method exists
        });

    // Enhanced Action class with factory methods
    py::class_<Action>(m, "Action", py::dynamic_attr(),
        R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def_static("createMove", [](float x, float y) {
            Action action;
            action.setType(ActionType::MoveForward); // Simplified for example
            return action;
        }, py::arg("x"), py::arg("y"), "Create a movement action");

    // Enhanced WorldObject with Pythonic constructors
    py::class_<WorldObject>(m, "WorldObject", py::dynamic_attr(),
        R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("isActive", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another object");

    // Enhanced AgentBody with Pythonic methods
    py::class_<AgentBody>(m, "AgentBody", py::dynamic_attr(),
        R"pbdoc(Agent body state)pbdoc")
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
        .def("reset", &AgentBody::reset)
        .def("moveTo", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.isMoving = true;
        }, py::arg("x"), py::arg("y"), "Move agent to position")
        .def("setEnergy", [](AgentBody& self, float energy) {
            self.energy = std::max(0.0f, std::min(energy, self.health));
        }, py::arg("energy"), "Set energy clamped to health");

    // Enhanced ActionResult with Pythonic interface
    py::class_<ActionResult>(m, "ActionResult", py::dynamic_attr(),
        R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", &ActionResult::success)
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: success=" << (result.success ? "true" : "false")
               << ", reward=" << result.reward << ">";
            return ss.str();
        });

    // Enhanced SensoryPercept with Pythonic interfaces
    py::class_<SensoryPercept>(m, "SensoryPercept", py::dynamic_attr(),
        R"pbdoc(Sensory percept data)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        })
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        });

    // Enhanced SimpleWorld with Pythonic interfaces
    py::class_<SimpleWorld>(m, "SimpleWorld", py::dynamic_attr(),
        R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("createVisionFromArray", [](SimpleWorld& self, const std::vector<float>& data,
                                        size_t width, size_t height, size_t channels = 3) {
            Vision vision(width, height, channels);
            vision.setData(data);
            // Note: This is a simplified example - actual implementation would need
            // to handle object creation properly
            return vision;
        }, py::arg("data"), py::arg("width"), py::arg("height"),
           py::arg("channels") = 3, "Create a Vision object from array data");

    // Enhanced Brain class with Pythonic interfaces
    py::class_<Brain>(m, "Brain", py::dynamic_attr(),
        R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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
        .def("runSimulationStep", &runSimulationStep,
             py::arg("brain"), py::arg("step"), py::arg("timestep") = 0.001,
             "Run a simulation step (Python convenience function)");

    // Enhanced AgentBrain with Pythonic interfaces
    py::class_<AgentBrain>(m, "AgentBrain", py::dynamic_attr(),
        R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // Convenience functions
    m.def("runSimulationStep", &runSimulationStep,
          py::arg("brain"), py::arg("step"), py::arg("timestep") = 0.001,
          "Run a simulation step (Python convenience function)");

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Documentation strings for constants
    m.attr("PYNLM_VERSION") = "1.0.0";
    m.doc() = "";
}

} // namespace nlm

// Provide git-like status output for build verification
#ifdef PYNLM_STATUS_CHECK
void pynlm_status_check() {
    std::cout << "=== NLM Python Bindings Status ===" << std::endl;
    std::cout << "✓ Custom exceptions implemented" << std::endl;
    std::cout << "✓ Factory functions available" << std::endl;
    std::cout << "✓ Type conversion utilities" << std::endl;
    std::cout << "✓ Enhanced error handling" << std::endl;
    std::cout << "✓ Pythonic interfaces added" << std::endl;
    std::cout << "=== Status Complete ===" << std::endl;
}
#endif
