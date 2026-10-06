#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>

// Event system headers
#include "../src/core/Events/EventSystem.hpp"
#include "../src/core/Events/EventTypes.hpp"

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

// Custom exception hierarchy for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class SensoryError : public NLMError {
public:
    explicit SensoryError(const std::string& msg) : NLMError("Sensory error: " + msg) {}
};

class ActionError : public NLMError {
public:
    explicit ActionError(const std::string& msg) : NLMError("Action error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

class DevelopmentError : public NLMError {
public:
    explicit DevelopmentError(const std::string& msg) : NLMError("Development error: " + msg) {}
};

// Helper function to register custom exceptions with pybind11
inline void registerCustomExceptions(py::module_& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const SensoryError& e) {
            PyErr_SetString(PyExc_TypeError, e.what());
        } catch (const ActionError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const MemoryError& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const DevelopmentError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, std::string("NLM runtime error: ") + e.what());
        }
    });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Includes convenience methods for common use cases and custom exception hierarchy.
    )pbdoc";

    // Register custom exception hierarchy
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<SensoryError>(m, "SensoryError", PyExc_TypeError);
    py::register_exception<ActionError>(m, "ActionError", PyExc_ValueError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_MemoryError);
    py::register_exception<DevelopmentError>(m, "DevelopmentError", PyExc_RuntimeError);
    
    // Register translator for custom exceptions
    registerCustomExceptions(m);
    
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    // Enhanced convenience functions and classes
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Convenience function: Create brain from config dict
    m.def("createBrainFromDict", [](const py::dict& config_dict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::str(item.first);
            std::string value_str = py::str(item.second);
            
            try {
                // Try integer conversion first
                try {
                    int64_t int_val = std::stoll(value_str);
                    config->set(key, int_val, ConfigSource::Python);
                } catch (...) {
                    // Try float conversion
                    try {
                        float float_val = std::stof(value_str);
                        config->set(key, float_val, ConfigSource::Python);
                    } catch (...) {
                        // Try string
                        config->set(key, value_str, ConfigSource::Python);
                    }
                }
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to set config key '" + key + "': " + e.what());
            }
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"), 
         "Create brain from Python dictionary configuration");
    
    // Convenience function: Run complete episode simulation
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                         std::shared_ptr<SimpleWorld> world, int steps,
                         bool enable_logging = false) {
        if (enable_logging) {
            NLM_LOG_INFO("Starting episode with " + std::to_string(steps) + " steps");
        }
        
        brain->initialize();
        agent->initialize(*world);
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process in agent/brain
            agent->processSensoryInput(percept);
            brain->step(step);
            
            // Get action and apply
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = 0.0f;
            if (auto internal = percept->getInternal()) {
                reward = internal->size() > 0 ? (*internal)[0] : 0.0f;
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            if (enable_logging && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           ": Firing neurons=" + std::to_string(brain->getFiringNeuronCount()) +
                           ", Curiosity=" + std::to_string(agent->getCuriosityLevel()) +
                           ", Novelty=" + std::to_string(agent->getNoveltyLevel()));
            }
        }
        
        if (enable_logging) {
            NLM_LOG_INFO("Episode completed");
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
         py::arg("enable_logging") = false,
         "Run complete agent episode simulation");
    
    // Convenience function: Quick brain initialization with environment setup
    m.def("createAgentEnvironment", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        
        // Enable typical subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, agent, world);
    }, 
         "Create and initialize a complete agent environment with default settings");
    
    // Convenience function: Get brain statistics as Python dictionary
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return stats;
    }, py::arg("brain"), "Get brain statistics as Python dictionary");
    
    // Convenience function: Enhanced world object creation
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
         "Create a world object with specified parameters");
    
    // Convenience function: Apply action with validation
    m.def("applyActionSafely", [](std::shared_ptr<SimpleWorld> world, const Action& action, float time) {
        if (!world->isValidPosition(action.getParameters()[0], action.getParameters()[1])) {
            throw ActionError("Action position " + std::to_string(action.getParameters()[0]) + ", " +
                            std::to_string(action.getParameters()[1]) + " is outside world bounds");
        }
        world->applyMotorCommand(action, time);
    }, py::arg("world"), py::arg("action"), py::arg("time"),
         "Apply action with position validation");
    
    // Convenience function: Batch world object creation
    m.def("createWorldObjects", [](const py::list& object_specs) {
        std::vector<WorldObject> objects;
        for (auto item : object_specs) {
            py::dict spec = item.cast<py::dict>();
            float x = spec["x"].cast<float>();
            float y = spec["y"].cast<float>();
            WorldObjectType type = spec["type"].cast<WorldObjectType>();
            float value = spec.get("value", 0.0f).cast<float>();
            float radius = spec.get("radius", 0.5f).cast<float>();
            objects.emplace_back(x, y, type, value, radius);
        }
        return objects;
    }, py::arg("object_specs"), 
         "Create multiple world objects from Python list of dictionaries");
    
    // Convenience function: Enhanced percept access
    m.def("getCompletePercept", [](std::shared_ptr<SimpleWorld> world) {
        auto percept = world->getSensoryPercept();
        
        py::dict complete;
        complete["vision"] = percept->getVision();
        complete["vision_width"] = percept->getVisionWidth();
        complete["vision_height"] = percept->getVisionHeight();
        complete["vision_channels"] = percept->getVision().getChannels();
        complete["touch"] = percept->getTouch();
        complete["internal"] = percept->getInternal();
        complete["proprioception"] = percept->getProprioception();
        complete["audio"] = percept->getAudio();
        complete["timestamp"] = percept->getTimestamp();
        
        return complete;
    }, py::arg("world"), "Get complete sensory percept as Python dictionary");
    
    // Convenience function: Advanced configuration loading
    m.def("loadConfigWithDefaults", [](const std::string& filepath, const py::dict& defaults) {
        auto config = std::make_shared<Config>();
        
        // Apply defaults first
        for (auto item : defaults) {
            std::string key = py::str(item.first);
            std::string value_str = py::str(item.second);
            
            try {
                // Try integer conversion first
                try {
                    int64_t int_val = std::stoll(value_str);
                    config->set(key, int_val, ConfigSource::Python);
                } catch (...) {
                    try {
                        float float_val = std::stof(value_str);
                        config->set(key, float_val, ConfigSource::Python);
                    } catch (...) {
                        config->set(key, value_str, ConfigSource::Python);
                    }
                }
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to set default config key '" + key + "': " + e.what());
            }
        }
        
        // Load from file (overwrites defaults for existing keys)
        if (config->loadFromFile(filepath)) {
            return config;
        } else {
            NLM_LOG_WARN("Config file not found, using defaults: " + filepath);
            return config;
        }
    }, py::arg("filepath"), py::arg("defaults"),
         "Load configuration with Python dictionary defaults");
    
    // Convenience function: Run multiple episodes with statistics
    m.def("runEpisodes", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                           std::shared_ptr<SimpleWorld> world, int num_episodes,
                           int steps_per_episode, bool collect_stats = false) {
        std::vector<py::dict> episode_stats;
        
        for (int episode = 0; episode < num_episodes; ++episode) {
            // Create a fresh environment for each episode
            if (episode > 0) {
                brain->reset();
                brain->initialize();
                agent->reset();
                agent->initialize(*world);
                world->reset();
            }
            
            // Run episode
            for (int step = 0; step < steps_per_episode; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                float reward = 0.0f;
                if (auto internal = percept->getInternal()) {
                    reward = internal->size() > 0 ? (*internal)[0] : 0.0f;
                }
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1);
            }
            
            if (collect_stats) {
                py::dict stats;
                stats["episode"] = episode;
                stats["final_neurons_firing"] = brain->getFiringNeuronCount();
                stats["total_spikes"] = brain->getTotalSpikeCount();
                stats["curiosity_level"] = agent->getCuriosityLevel();
                stats["novelty_level"] = agent->getNoveltyLevel();
                stats["prediction_error"] = agent->getPredictionError();
                episode_stats.push_back(stats);
            }
            
            NLM_LOG_INFO("Episode " + std::to_string(episode) + " completed");
        }
        
        return episode_stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
         py::arg("num_episodes"), py::arg("steps_per_episode"), py::arg("collect_stats") = false,
         "Run multiple episodes and return statistics");
    
// Performance monitoring and profiling
    // Benchmark results structure
    py::class_<BenchmarkResult>(m, "BenchmarkResult", R"pbdoc(Benchmark results structure)pbdoc")
        .def_readonly("neurons", &BenchmarkResult::neurons, "Number of neurons")
        .def_readonly("synapses", &BenchmarkResult::synapses, "Number of synapses")
        .def_readonly("steps", &BenchmarkResult::steps, "Number of simulation steps")
        .def_readonly("total_time", &BenchmarkResult::totalTime, "Total simulation time in seconds")
        .def_readonly("steps_per_second", &BenchmarkResult::stepsPerSecond, "Steps per second")
        .def_readonly("spikes_per_step", &BenchmarkResult::spikesPerStep, "Average spikes per step")
        .def_readonly("synapses_per_neuron", &BenchmarkResult::synapsesPerNeuron, "Average synapses per neuron");
    
    // Convenience function: Run performance benchmark
    m.def("benchmarkPerformance", [](size_t neuronCount, size_t steps) {
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run simulation and measure time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject some random input
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Collect results
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        return result;
    }, py::arg("neuron_count"), py::arg("steps"),
         "Run a performance benchmark with specified neuron count and steps");
    
    // Convenience function: Run multiple benchmarks
    m.def("runBenchmarks", []() {
        std::vector<BenchmarkResult> results;
        
        // Benchmark configurations: (neuron_count, steps)
        std::vector<std::pair<size_t, size_t>> benchmarks = {
            {100, 1000},      // 100 neurons, 1000 steps
            {500, 1000},      // 500 neurons, 1000 steps
            {1000, 500},      // 1000 neurons, 500 steps
            {2000, 200},      // 2000 neurons, 200 steps
        };
        
        for (const auto& [neurons, steps] : benchmarks) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                for (size_t i = 0; i < std::min(size_t(10), neurons / 10); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                
                brain->step(step, step * 0.001);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            BenchmarkResult result;
            result.neurons = brain->getTotalNeuronCount();
            result.synapses = brain->getTotalSynapseCount();
            result.steps = steps;
            result.totalTime = totalTime;
            result.stepsPerSecond = steps / totalTime;
            result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
            result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
            
            results.push_back(result);
        }
        
        return results;
    }, "Run a series of performance benchmarks");
    
    // Convenience function: Quick performance test
    m.def("quickBenchmark", [](size_t neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run a quick simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 100; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        py::dict stats;
        stats["neurons"] = neuronCount;
        stats["steps_per_second"] = 100.0 / totalTime;
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = static_cast<double>(brain->getTotalSpikeCount()) / 100.0;
        stats["benchmark_time_seconds"] = totalTime;
        
        return stats;
    }, py::arg("neuron_count"), "Quick performance test with specified neuron count");
    
    // Performance monitoring class
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Monitor and record performance metrics during simulation)pbdoc")
        .def(py::init<>())
        .def("start", &PerformanceMonitor::start, "Start performance monitoring")
        .def("stop", &PerformanceMonitor::stop, "Stop performance monitoring")
        .def("is_running", &PerformanceMonitor::isRunning, "Check if monitoring is running")
        .def("get_stats", &PerformanceMonitor::getStats, py::return_value_policy::copy,
             "Get recorded performance statistics")
        .def("record_event", &PerformanceMonitor::recordEvent, py::arg("event_name"),
             "Record a timing event");
    
    // Memory usage monitoring
    m.def("getMemoryUsage", []() {
        // Note: This is a placeholder - actual implementation would depend on the platform
        // For now, return 0 to indicate unknown
        return 0.0;
    }, "Get current memory usage in MB");
    
    // CPU usage monitoring  
    m.def("getCpuUsage", []() {
        // Note: This is a placeholder - actual implementation would depend on the platform
        // For now, return 0.0 to indicate unknown
        return 0.0;
    }, "Get current CPU usage percentage");
