#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::value_error("Error loading from arguments: " + std::string(e.what()));
            }
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
        // Convenience getter methods
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto val = self.getFloat(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found or not a float: " + key);
        }, py::arg("key"), "Get float value from configuration")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto val = self.getInt(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found or not an integer: " + key);
        }, py::arg("key"), "Get integer value from configuration")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto val = self.getBool(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found or not a boolean: " + key);
        }, py::arg("key"), "Get boolean value from configuration")
        .def("getString", [](const Config& self, const std::string& key) {
            auto val = self.getString(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found or not a string: " + key);
        }, py::arg("key"), "Get string value from configuration")
        .def("getFloatArray", [](const Config& self, const std::string& key) {
            auto val = self.getFloatArray(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found or not a float array: " + key);
        }, py::arg("key"), "Get float array value from configuration")
        .def("getIntArray", [](const Config& self, const std::string& key) {
            auto val = self.getIntArray(key);
            if (val) return *val;
            throw py::key_error("Configuration key not found or not an integer array: " + key);
        }, py::arg("key"), "Get integer array value from configuration")
        // Type-specific getter methods for common parameters
        .def("getLearningRate", [](const Config& self) {
            auto val = self.getLearningRate();
            if (val) return *val;
            return 0.01; // Default learning rate
        }, "Get learning rate parameter")
        .def("getDecayRate", [](const Config& self) {
            auto val = self.getDecayRate();
            if (val) return *val;
            return 0.99; // Default decay rate
        }, "Get decay rate parameter")
        .def("getPopulationSize", [](const Config& self) {
            auto val = self.getPopulationSize();
            if (val) return *val;
            return 100; // Default population size
        }, "Get population size parameter")
        .def("getConnectionDensity", [](const Config& self) {
            auto val = self.getConnectionDensity();
            if (val) return *val;
            return 0.1; // Default connection density
        }, "Get connection density parameter")
        .def("getMaxSynapticAge", [](const Config& self) {
            auto val = self.getMaxSynapticAge();
            if (val) return *val;
            return 1000; // Default max synaptic age
        }, "Get max synaptic age parameter")
        .def("getEnablePlasticity", [](const Config& self) {
            auto val = self.getEnablePlasticity();
            if (val) return *val;
            return true; // Default plasticity enabled
        }, "Get plasticity enable flag")
        .def("getEnableNeuromodulation", [](const Config& self) {
            auto val = self.getEnableNeuromodulation();
            if (val) return *val;
            return true; // Default neuromodulation enabled
        }, "Get neuromodulation enable flag")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Error handling methods
        .def("hasError", &SensoryInput::hasError, "Check if the object has an error state")
        .def("getErrorMessage", &SensoryInput::getErrorMessage, "Get error message if any");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Vision processing methods
        .def("getChannelData", [](Vision& self, size_t channel) {
            try {
                return self.getChannelData(channel);
            } catch (const std::exception& e) {
                throw py::value_error("Error getting channel data: " + std::string(e.what()));
            }
        }, py::arg("channel"), "Get specific color channel data")
        .def("applyFilter", [](Vision& self, const std::vector<float>& kernel) {
            try {
                self.applyFilter(kernel);
            } catch (const std::exception& e) {
                throw py::value_error("Error applying filter: " + std::string(e.what()));
            }
        }, py::arg("kernel"), "Apply a processing filter to the image")
        .def("normalize", [](Vision& self, float targetMean, float targetStdDev) {
            try {
                self.normalize(targetMean, targetStdDev);
            } catch (const std::exception& e) {
                throw py::value_error("Error normalizing image: " + std::string(e.what()));
            }
        }, py::arg("targetMean") = 0.0f, py::arg("targetStdDev") = 1.0f, "Normalize image data")
        .def("scale", [](Vision& self, float factor) {
            try {
                self.scale(factor);
            } catch (const std::exception& e) {
                throw py::value_error("Error scaling image: " + std::string(e.what()));
            }
        }, py::arg("factor"), "Scale image dimensions by a factor")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            try {
                self.resize(newWidth, newHeight);
            } catch (const std::exception& e) {
                throw py::value_error("Error resizing image: " + std::string(e.what()));
            }
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize image to specified dimensions");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Audio processing methods
        .def("applyGain", [](Audio& self, float gainDb) {
            try {
                self.applyGain(gainDb);
            } catch (const std::exception& e) {
                throw py::value_error("Error applying gain: " + std::string(e.what()));
            }
        }, py::arg("gainDb"), "Apply gain in dB to audio signal")
        .def("getDuration", &Audio::getDuration, "Get audio signal duration in seconds")
        .def("getFrequency", [](Audio& self) {
            try {
                return self.getFrequency();
            } catch (const std::exception& e) {
                throw py::value_error("Error getting frequency content: " + std::string(e.what()));
            }
        }, "Get frequency content of audio signal");

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
        // String representation methods
        .def("toString", &Action::toString, "Get string representation of action")
        .def("fromString", [](const std::string& str) {
            auto action = Action::fromString(str);
            if (!action) {
                throw py::value_error("Failed to parse action from string: " + str);
            }
            return action;
        }, py::arg("str"), "Parse action from string representation"),
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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set default configuration values
        config->set("learning_rate", 0.01);
        config->set("decay_rate", 0.99);
        config->set("population_size", 100);
        config->set("connection_density", 0.1);
        config->set("max_synaptic_age", 1000);
        config->set("enable_plasticity", true);
        config->set("enable_neuromodulation", true);
        return config;
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Specialized factory functions for common use cases
    m.def("createSimpleRobotBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("learning_rate", 0.05);
        config->set("decay_rate", 0.98);
        config->set("population_size", 200);
        config->set("connection_density", 0.15);
        config->set("enable_plasticity", true);
        return std::make_shared<Brain>(config);
    }, "Create a simple robot brain with basic settings");

    m.def("createLearningAgentWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 84, 84); // 84x84 vision for RL
        world->setAgentStart(400, 300);
        return world;
    }, "Create a simple world for learning agents");

    m.def("createVisionSensor", [](size_t width, size_t height, size_t channels) -> std::shared_ptr<Vision> {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a vision sensor with specified dimensions");

    m.def("createAudioSensor", [](size_t sampleRate, size_t numSamples) -> std::shared_ptr<Audio> {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create an audio sensor with specified parameters");

    // Utility functions for common operations
    m.def("normalizeData", [](const std::vector<float>& data, float targetMean, float targetStdDev) {
        if (data.empty()) return std::vector<float>{};
        
        float mean = 0.0f, stddev = 0.0f;
        for (float val : data) mean += val;
        mean /= data.size();
        
        for (float val : data) stddev += (val - mean) * (val - mean);
        stddev = std::sqrt(stddev / data.size());
        
        if (stddev == 0.0f) stddev = 1.0f; // Avoid division by zero
        
        std::vector<float> result;
        result.reserve(data.size());
        for (float val : data) {
            result.push_back(targetStdDev * (val - mean) / stddev + targetMean);
        }
        
        return result;
    }, py::arg("data"), py::arg("targetMean") = 0.0f, py::arg("targetStdDev") = 1.0f, 
       "Normalize data to specified mean and standard deviation");

    m.def("scaleData", [](const std::vector<float>& data, float factor) {
        std::vector<float> result;
        result.reserve(data.size());
        for (float val : data) {
            result.push_back(val * factor);
        }
        return result;
    }, py::arg("data"), py::arg("factor"), "Scale all data values by a factor");

    m.def("clipData", [](const std::vector<float>& data, float min, float max) {
        std::vector<float> result;
        result.reserve(data.size());
        for (float val : data) {
            result.push_back(std::max(min, std::min(max, val)));
        }
        return result;
    }, py::arg("data"), py::arg("min"), py::arg("max"), "Clip data values to specified range");

    // Action creation helper functions
    m.def("createAction", [](ActionType type, const std::vector<float>& parameters) -> std::unique_ptr<Action> {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(), "Create an action with specified type and parameters");

    m.def("createMoveAction", [](float dx, float dy) -> std::unique_ptr<Action> {
        auto action = std::make_unique<Action>(ActionType::Custom);
        std::vector<float> params{dx, dy};
        action->setParameters(params);
        return action;
    }, py::arg("dx"), py::arg("dy"), "Create a movement action with displacement");

    m.def("createLookAction", [](float angleX, float angleY) -> std::unique_ptr<Action> {
        auto action = std::make_unique<Action>(ActionType::Look);
        std::vector<float> params{angleX, angleY};
        action->setParameters(params);
        return action;
    }, py::arg("angleX"), py::arg("angleY"), "Create a look action with angles");

    // Neural ID utility functions
    m.def("getRandomNeuronId", []() -> NeuronId {
        uint64_t random = std::rand();
        return NeuronId(random);
    }, "Get a random neuron ID");

    m.def("getRandomSynapseId", []() -> SynapseId {
        uint64_t random = std::rand();
        return SynapseId(random);
    }, "Get a random synapse ID");

    m.def("getRandomRegionId", []() -> RegionId {
        uint64_t random = std::rand();
        return RegionId(random);
    }, "Get a random region ID");

    // Type conversion utility functions
    m.def("neuronIdToInt", [](const NeuronId& id) -> uint64_t { return id.index(); }, 
          "Convert NeuronId to unsigned integer");

    m.def("intToNeuronId", [](uint64_t value) -> NeuronId { return NeuronId(value); },
          "Create NeuronId from unsigned integer");

    m.def("synapseIdToInt", [](const SynapseId& id) -> uint64_t { return id.index(); },
          "Convert SynapseId to unsigned integer");

    m.def("intToSynapseId", [](uint64_t value) -> SynapseId { return SynapseId(value); },
          "Create SynapseId from unsigned integer");

    // Get invalid IDs (sentinel values)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
