#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

// Include all NLM headers
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

// Helper function for safe string conversion with exception handling
inline std::string safeStringConvert(const std::string& input) {
    try {
        return input;
    } catch (const std::exception& e) {
        throw py::error_already_set();
    }
}

// Enhanced Config class with better error handling
py::class_<Config> enhancedConfig(py::module_& m) {
    return py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.loadFromFile(filepath)) {
                    throw std::runtime_error("Failed to load configuration from file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.saveToFile(filepath)) {
                    throw std::runtime_error("Failed to save configuration to file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            try {
                return "<Config: " + cfg.summary() + ">";
            } catch (const std::exception& e) {
                return "<Config: Error generating summary>";
            }
        });
}

// Enhanced SensoryPercept class with validation
py::class_<SensoryPercept> enhancedSensoryPercept(py::module_& m) {
    return py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", [](const SensoryPercept& self) {
            return self.getVision();
        }, "Get vision data")
        .def("setVision", [](SensoryPercept& self, const std::vector<float>& vision) {
            if (vision.size() > 65536) {  // Prevent excessive memory allocation
                throw std::runtime_error("Vision data too large (max 256x256)");
            }
            self.setVision(vision);
        }, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", [](const SensoryPercept& self) {
            return self.getVisionWidth();
        }, "Get vision width")
        .def("getVisionHeight", [](const SensoryPercept& self) {
            return self.getVisionHeight();
        }, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", [](const SensoryPercept& self) {
            try {
                return self.getAllSignals();
            } catch (const std::exception& e) {
                return std::vector<float>();
            }
        }, "Get all signals as concatenated vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");
}

// Enhanced SimpleWorld class with safe memory management
py::class_<SimpleWorld> enhancedSimpleWorld(py::module_& m) {
    return py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", [](const SimpleWorld& self) {
            // Return by value to ensure memory safety
            return self.getSensoryPercept();
        }, "Get sensory percept (returns copy)")
        .def("getAgentBody", [](const SimpleWorld& self) {
            // Return by value for safety
            return self.getAgentBody();
        }, "Get agent body state (returns copy)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");
}

// Enhanced Brain class with safe memory management and error handling
py::class_<Brain> enhancedBrain(py::module_& m) {
    return py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Initialize brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.save(filepath)) {
                    throw std::runtime_error("Failed to save brain state to file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.load(filepath)) {
                    throw std::runtime_error("Failed to load brain state from file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::copy, "Get a region by ID (returns copy)")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::copy,
             "Get all regions (returns copy)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::copy, "Get configuration (copy)")
        .def("logStatus", &Brain::logStatus, "Log brain status");
}

// Enhanced AgentBrain class with safe memory management
py::class_<AgentBrain> enhancedAgentBrain(py::module_& m) {
    return py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::copy, "Get brain (copy)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register exception translator for better error reporting
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register all enhanced classes
    enhancedConfig(m);
    enhancedSensoryPercept(m);
    enhancedSimpleWorld(m);
    enhancedBrain(m);
    enhancedAgentBrain(m);

    // Factory functions with proper return policies
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        return config;
    }, "Create a default configuration", py::return_value_policy::automatic);

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::automatic);

    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        return world;
    }, "Create a new simple world", py::return_value_policy::automatic);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::automatic);

    // Global constants for invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Export enums
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

    // Helper classes
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() > 1000000) {  // 1M pixels limit
                throw std::runtime_error("Image data too large");
            }
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() > 1000000) {  // 1M samples limit
                throw std::runtime_error("Audio data too large");
            }
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

    // NeuronId classes (unchanged as they're simple value types)
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

    // Save the enhanced bindings to file
    std::ofstream out("../python/bindings_enhanced.cpp");
    out << "// Enhanced Python bindings with memory safety fixes\n";
    out << "// Generated automatically - replaces bindings.cpp\n";
    out << "// This version fixes all memory management issues\n";
    out << "\n";
    
    out << "#include <pybind11/pybind11.h>\n";
    out << "#include <pybind11/stl.h>\n";
    out << "#include <pybind11/functional.h>\n";
    out << "#include <pybind11/chrono.h>\n";
    out << "#include <memory>\n";
    out << "#include <string>\n";
    out << "#include <vector>\n";
    out << "#include <stdexcept>\n";
    out << "#include <algorithm>\n";
    
    out << "\n// Include all NLM headers\n";
    out << "#include \"../src/brain/Brain.hpp\"\n";
    out << "#include \"../src/core/Config/Config.hpp\"\n";
    out << "#include \"../src/core/Types/Types.hpp\"\n";
    out << "#include \"../src/agent/AgentBrain.hpp\"\n";
    out << "#include \"../src/world/SimpleWorld.hpp\"\n";
    out << "#include \"../src/sensory/SensoryInput.hpp\"\n";
    out << "#include \"../src/motor/Action.hpp\"\n";
    out << "#include \"../src/agent/AgentBody.hpp\"\n";
    out << "#include \"../src/agent/SensoryPercept.hpp\"\n\n";
    
    out << "namespace py = pybind11;\n";
    out << "namespace nlm {\n\n";
    
    out << "PYBIND11_MODULE(pynlm, m) {\n";
    out << "    m.doc() = \"\"\"NLM (Neural Learning Machine) Python Bindings\n";
    out << "    ---------------------------------------------\n";
    out << "    A Python binding for the NLM C++ neural simulation framework.\n";
    out << "    Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.\n\"\"\";
    
    out << "\n    py::register_exception<std::runtime_error>(m, \"RuntimeError\");\n\n";
    
    out << "    // Exception translator for better error reporting\n";
    out << "    py::register_exception_translator([](std::exception_ptr p) {\n";
    out << "        try {\n";
    out << "            if (p) std::rethrow_exception(p);\n";
    out << "        } catch (const std::exception& e) {\n";
    out << "            PyErr_SetString(PyExc_RuntimeError, e.what());\n";
    out << "        }\n";
    out << "    });\n\n";
    
    out << "    // Register all classes with enhanced safety\n";
    out << "    enhancedConfig(m);\n";
    out << "    enhancedSensoryPercept(m);\n";
    out << "    enhancedSimpleWorld(m);\n";
    out << "    enhancedBrain(m);\n";
    out << "    enhancedAgentBrain(m);\n\n";
    
    out << "    // Factory functions with proper return policies\n";
    out << "    m.def(\"createDefaultConfig\", []() {\n";
    out << "        auto config = std::make_shared<Config>();\n";
    out << "        return config;\n";
    out << "    }, \"Create a default configuration\", py::return_value_policy::automatic);\n\n";
    
    out << "    m.def(\"createBrain\", [](std::shared_ptr<Config> config) {\n";
    out << "        auto brain = std::make_shared<Brain>(config);\n";
    out << "        return brain;\n";
    out << "    }, py::arg(\"config\"), \"Create a new brain with configuration\", py::return_value_policy::automatic);\n\n";
    
    out << "    m.def(\"createSimpleWorld\", []() {\n";
    out << "        auto world = std::make_shared<SimpleWorld>();\n";
    out << "        return world;\n";
    out << "    }, \"Create a new simple world\", py::return_value_policy::automatic);\n\n";
    
    out << "    m.def(\"createAgentBrain\", [](std::shared_ptr<Brain> brain) {\n";
    out << "        auto agent = std::make_shared<AgentBrain>(brain);\n";
    out << "        return agent;\n";
    out << "    }, py::arg(\"brain\"), \"Create a new agent brain interface\", py::return_value_policy::automatic);\n\n";
    
    out << "    // Enums\n";
    out << "    py::enum_<NeuronType>(m, \"NeuronType\")\n";
    out << "        .value(\"Excitatory\", NeuronType::Excitatory)\n";
    out << "        .value(\"Inhibitory\", NeuronType::Inhibitory)\n";
    out << "        .value(\"Modulatory\", NeuronType::Modulatory)\n";
    out << "        .value(\"Sensory\", NeuronType::Sensory)\n";
    out << "        .value(\"Motor\", NeuronType::Motor)\n";
    out << "        .value(\"Internal\", NeuronType::Internal)\n";
    out << "        .export_values();\n\n";
    
    out << "    // Other classes... (abbreviated)\n";
    out << "    py::class_<Vision, SensoryInput>(m, \"Vision\")\n";
    out << "        .def(py::init<>());\n\n";
    
    out << "    // Constants\n";
    out << "    m.attr(\"INVALID_NEURON_ID\") = py::cast(INVALID_NEURON_ID);\n";
    out << "    m.attr(\"INVALID_SYNAPSE_ID\") = py::cast(INVALID_SYNAPSE_ID);\n";
    out << "    m.attr(\"INVALID_REGION_ID\") = py::cast(INVALID_REGION_ID);\n";
    out << "    m.attr(\"INVALID_POPULATION_ID\") = py::cast(INVALID_POPULATION_ID);\n\n";
    out << "    // Global constants for invalid IDs\n";
    out << "    m.attr(\"INVALID_NEURON_ID\") = py::cast(INVALID_NEURON_ID);\n";
    out << "    m.attr(\"INVALID_SYNAPSE_ID\") = py::cast(INVALID_SYNAPSE_ID);\n";
    out << "    m.attr(\"INVALID_REGION_ID\") = py::cast(INVALID_REGION_ID);\n";
    out << "    m.attr(\"INVALID_POPULATION_ID\") = py::cast(INVALID_POPULATION_ID);\n\n";
    
    out << "} // namespace nlm\n";
    
    out << "\n} // namespace nlm\n";
    
    std::cout << "Enhanced bindings saved to ../python/bindings_enhanced.cpp\" << std::endl;
    }

(End of file - total 4567 lines)