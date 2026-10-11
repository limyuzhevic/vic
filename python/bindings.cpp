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
        })
        // Property-based accessors for Python
        .def("__getitem__", [](const Config& self, const std::string& key) {
            // Try to get as different types
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            }
            
            auto val_int = self.get<int>(key);
            if (val_int.has_value()) {
                return py::cast(val_int.value());
            }
            
            auto val_float = self.get<double>(key);
            if (val_float.has_value()) {
                return py::cast(val_float.value());
            }
            
            auto val_bool = self.get<bool>(key);
            if (val_bool.has_value()) {
                return py::cast(val_bool.value());
            }
            
            throw py::key_error("Key not found: " + key);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        })
        .def("__setitem__", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        })
        .def("__setitem__", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        })
        .def("__setitem__", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        })
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            // Try to get as different types
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            }
            
            auto val_int = self.get<int>(key);
            if (val_int.has_value()) {
                return py::cast(val_int.value());
            }
            
            auto val_float = self.get<double>(key);
            if (val_float.has_value()) {
                return py::cast(val_float.value());
            }
            
            auto val_bool = self.get<bool>(key);
            if (val_bool.has_value()) {
                return py::cast(val_bool.value());
            }
            
            return py::none();
        }, py::arg("key"), "Get a configuration value by key")
        .def("get", [](const Config& self, const std::string& key, py::object defaultValue) {
            // Try to get as different types
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return py::cast(val.value());
            }
            
            auto val_int = self.get<int>(key);
            if (val_int.has_value()) {
                return py::cast(val_int.value());
            }
            
            auto val_float = self.get<double>(key);
            if (val_float.has_value()) {
                return py::cast(val_float.value());
            }
            
            auto val_bool = self.get<bool>(key);
            if (val_bool.has_value()) {
                return py::cast(val_bool.value());
            }
            
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a configuration value with default")

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
        .def("clone", &Action::clone)
        .def_property("type", &Action::getType, &Action::setType,
                     "Action type property")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
                     "Action parameters property")
        .def("__repr__", [](const Action& action) {
            std::string name = action.getName();
            return "<Action type=" + name + " params=[" + 
                   (action.getParameters().empty() ? "empty" : "values") + "]>";
        });

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
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def_property_readonly("width", &SimpleWorld::getWidth, "Get world width")
        .def_property_readonly("height", &SimpleWorld::getHeight, "Get world height")
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) +
                   " height=" + std::to_string(world.getHeight()) +
                   " time=" + std::to_string(world.getSimulationTime()) + ">";
        });

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
             "Log brain status")
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount,
                               "Get total neuron count")
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount,
                               "Get total synapse count")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                               "Get count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                               "Get average firing rate across all neurons")
        .def_property_readonly("spike_count", &Brain::getTotalSpikeCount,
                               "Get total spike count")
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage,
                               "Get current developmental stage")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) +
                   " stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ">";
        })

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def_property_readonly("development_stage", &AgentBrain::getDevelopmentalStage,
                               "Get current developmental stage")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                               "Get current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                               "Get curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                               "Get novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                               "Get prediction error")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain>";
        })

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Advanced Python features - Iterators and comprehensions
    
    // Iterator for regions in Brain
    m.def("iter_regions", [](const std::shared_ptr<Brain>& brain) {
        auto regions = brain->getRegions();
        return py::make_iterator(regions.begin(), regions.end());
    }, py::arg("brain"), "Get iterator over brain regions");
    
    // Iterator for neurons in a region
    m.def("iter_neurons", [](const std::shared_ptr<Brain>& brain, RegionId region_id) {
        auto region = brain->getRegion(region_id);
        if (!region) {
            throw py::key_error("Region not found: " + std::to_string(region_id.value));
        }
        return py::make_iterator(region->getNeurons().begin(), region->getNeurons().end());
    }, py::arg("brain"), py::arg("region_id"), "Get iterator over neurons in a region");
    
    // Iterator for synapses in a region
    m.def("iter_synapses", [](const std::shared_ptr<Brain>& brain, RegionId region_id) {
        auto region = brain->getRegion(region_id);
        if (!region) {
            throw py::key_error("Region not found: " + std::to_string(region_id.value));
        }
        return py::make_iterator(region->getSynapses().begin(), region->getSynapses().end());
    }, py::arg("brain"), py::arg("region_id"), "Get iterator over synapses in a region");
    
    // List comprehensions support for common operations
    m.def("get_active_neuron_ids", [](const std::shared_ptr<Brain>& brain) {
        std::vector<NeuronId> active_ids;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                if (neuron->isActive()) {
                    active_ids.push_back(neuron->getId());
                }
            }
        }
        return active_ids;
    }, py::arg("brain"), "Get list of active neuron IDs");
    
    m.def("get_firing_neuron_ids", [](const std::shared_ptr<Brain>& brain) {
        std::vector<NeuronId> firing_ids;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                if (neuron->isFiring()) {
                    firing_ids.push_back(neuron->getId());
                }
            }
        }
        return firing_ids;
    }, py::arg("brain"), "Get list of firing neuron IDs");
    
    m.def("get_region_names", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::string> names;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            names.push_back(region->getName());
        }
        return names;
    }, py::arg("brain"), "Get list of region names");
    
    // Generator-style iteration for simulation steps
    m.def("generate_brain_steps", [](std::shared_ptr<Brain> brain, size_t num_steps) {
        return py::make_generator([brain, num_steps](py::function yield) {
            for (size_t step = 0; step < num_steps; ++step) {
                brain->step(step);
                yield(py::make_tuple(step, brain->getFiringNeuronCount(), 
                                    brain->getAverageFiringRate()));
            }
        });
    }, py::arg("brain"), py::arg("num_steps"), 
           "Generate brain simulation steps as an iterator");
    
    // Advanced type conversion utilities
    m.def("neuron_id_from_int", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Create a NeuronId from an integer");
    
    m.def("synapse_id_from_int", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Create a SynapseId from an integer");
    
    m.def("region_id_from_int", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Create a RegionId from an integer");
    
    m.def("population_id_from_int", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), "Create a PopulationId from an integer");
    
    // Enhanced dictionary-like access for Config
    m.def("config_to_dict", [](const std::shared_ptr<Config>& config) {
        py::dict dict;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto val = config->get<std::string>(key);
            if (val.has_value()) {
                dict[key] = py::cast(val.value());
            }
        }
        return dict;
    }, py::arg("config"), "Convert Config to Python dictionary");
    
    m.def("dict_to_config", [](std::shared_ptr<Config> config, const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            
            if (value.is_none()) {
                continue;
            }
            
            try {
                if (value.is(py::int_())) {
                    config->set(key, py::cast<int>(value));
                } else if (value.is(py::float_())) {
                    config->set(key, py::cast<float>(value));
                } else if (value.is(py::bool_())) {
                    config->set(key, py::cast<bool>(value));
                } else {
                    config->set(key, py::cast<std::string>(value));
                }
            } catch (const py::cast_error&) {
                // Fall back to string conversion
                config->set(key, py::cast<std::string>(value));
            }
        }
        return config;
    }, py::arg("config"), py::arg("dict"), "Convert Python dictionary to Config");
    
    // Batch operations for advanced users
    m.def("batch_brain_operations", [](std::shared_ptr<Brain> brain, 
                                      const py::dict& operations) {
        py::dict results;
        
        if (operations.contains("steps")) {
            size_t steps = py::cast<size_t>(operations["steps"]);
            results["spike_count"] = brain->getTotalSpikeCount();
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            results["spike_count_after"] = brain->getTotalSpikeCount();
        }
        
        if (operations.contains("reset")) {
            brain->reset();
            results["reset"] = true;
        }
        
        if (operations.contains("initialize")) {
            results["initialized"] = brain->initialize();
        }
        
        return results;
    }, py::arg("brain"), py::arg("operations"), "Perform batch brain operations");

    // Advanced simulation utilities
    m.def("simulate_agent", [](std::shared_ptr<AgentBrain> agent, 
                             std::shared_ptr<SimpleWorld> world,
                             size_t num_steps,
                             const py::dict& options) {
        py::dict simulation_result;
        float total_reward = 0.0f;
        size_t successful_steps = 0;
        
        bool enable_learning = options.get("enable_learning", py::bool_(true)).cast<bool>();
        bool enable_curiosity = options.get("enable_curiosity", py::bool_(true)).cast<bool>();
        float timestep = options.get("timestep", py::float_(0.1f)).cast<float>();
        
        agent->enableRewardModulation(enable_learning);
        agent->enableCuriosity(enable_curiosity);
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Get brain step
            auto brain = agent->getBrain();
            if (brain) {
                brain->step(step, step * timestep);
            }
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Calculate reward based on action outcome
            float reward = 0.0f;
            if (action.getType() == ActionType::Eat) {
                reward = 10.0f; // Food reward
            } else if (action.getType() == ActionType::MoveForward) {
                reward = 1.0f; // Small movement reward
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            total_reward += reward;
            successful_steps++;
        }
        
        simulation_result["total_reward"] = total_reward;
        simulation_result["steps_completed"] = static_cast<int>(num_steps);
        simulation_result["successful_steps"] = static_cast<int>(successful_steps);
        simulation_result["avg_reward"] = total_reward / num_steps;
        simulation_result["final_time"] = world->getSimulationTime();
        
        return simulation_result;
    }, py::arg("agent"), py::arg("world"), py::arg("num_steps"), py::arg("options") = py::dict(),
           "Advanced agent simulation with configurable options");

    // Configuration utilities
    m.def("get_config_summary", [](const std::shared_ptr<Config>& config) {
        return config->summary();
    }, py::arg("config"), "Get config summary as string");
    
    m.def("load_config_from_string", [](const std::string& config_str) {
        auto config = std::make_shared<Config>();
        // Simple key=value format parsing
        std::istringstream stream(config_str);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                config->set(key, value);
            }
        }
        return config;
    }, py::arg("config_str"), "Load config from string");

    // Serialization utilities
    m.def("serialize_brain", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Serialize brain to file");
    
    m.def("deserialize_brain", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            return brain;
        }
        return std::shared_ptr<Brain>();
    }, py::arg("filepath"), "Deserialize brain from file");

    // Memory management utilities
    m.def("brain_stats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["region_count"] = brain->getRegionCount();
        stats["firing_count"] = brain->getFiringNeuronCount();
        stats["active_count"] = brain->getActiveNeuronCount();
        return stats;
    }, py::arg("brain"), "Get detailed brain memory statistics");
