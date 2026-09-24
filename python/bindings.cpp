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

py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.

        The Config class manages neural network configuration parameters including
        neuron properties, synaptic connections, plasticity rules, and neuromodulation.
        Configuration can be loaded from JSON files, command line arguments, or
        programmatically set.

        Key configuration categories:
        - brain: Neuron and network topology settings
        - plasticity: Learning mechanisms (STDP, Hebbian, structural)
        - neuromodulation: Dopamine, curiosity, novelty detection
        - development: Plasticity modulation during different stages
        - memory: Working memory, episodic memory, associative memory

        Examples:
            # Create default configuration
            config = pynlm.createDefaultConfig()

            # Load from file
            config.loadFromFile("my_config.json")

            # Set custom parameters
            config.set("brain.neuron_count", 2000)
            config.set("plasticity.stdp.enable", True)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False if file not found or invalid format.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Supports --key=value and -key value formats.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on write failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("get", [](Config& self, const std::string& key, const std::string& default_value = "") {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return default_value;
        }, py::arg("key"), py::arg("default_value") = "",
            "Get a configuration value as string. Returns default_value if key doesn't exist or value is not a string.")
        .def("getInt", [](Config& self, const std::string& key, int64_t default_value = 0) {
            auto value = self.get<int64_t>(key);
            if (value) return *value;
            return default_value;
        }, py::arg("key"), py::arg("default_value") = 0,
            "Get a configuration value as integer. Returns default_value if key doesn't exist or value is not an integer.")
        .def("getFloat", [](Config& self, const std::string& key, double default_value = 0.0) {
            auto value = self.get<double>(key);
            if (value) return *value;
            return default_value;
        }, py::arg("key"), py::arg("default_value") = 0.0,
            "Get a configuration value as float. Returns default_value if key doesn't exist or value is not a float.")
        .def("getBool", [](Config& self, const std::string& key, bool default_value = false) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            return default_value;
        }, py::arg("key"), py::arg("default_value") = false,
            "Get a configuration value as boolean. Returns default_value if key doesn't exist or value is not a boolean.")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set a configuration value. Source indicates how the configuration was obtained (Default, File, CommandLine).")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a human-readable summary string of the configuration including key-value pairs and sources.")
        .def("exportToDict", [](const Config& self) {
            py::dict result;
            for (const auto& entry : self.getAllEntries()) {
                result[entry.key] = entry.value;
            }
            return result;
        }, "Export configuration to Python dictionary.")
        .def("importFromDict", [](Config& self, const py::dict& dict) {
            for (auto it : dict) {
                std::string key = py::cast<std::string>(it.first);
                auto value = py::cast<std::string>(it.second);
                self.set(key, value, nlm::ConfigSource::Default);
            }
            return true;
        }, py::arg("dict"), "Import configuration from Python dictionary. Returns True on success.")
        .def("merge", [](Config& self, const Config& other) {
            // Import all entries from other config
            for (const auto& entry : other.getAllEntries()) {
                self.set(entry.key, entry.value, entry.source);
            }
            return true;
        }, py::arg("other"), "Merge another configuration into this one. Returns True on success.")
        .def("copy", [](const Config& self) {
            Config newConfig;
            for (const auto& entry : self.getAllEntries()) {
                newConfig.set(entry.key, entry.value, entry.source);
            }
            return newConfig;
        }, "Create a copy of this configuration. Returns a new Config instance.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
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

py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.

        The Brain class implements a full spiking neural network simulation using
        Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
        and multiple plasticity mechanisms. It serves as the main computational
        engine for the NLM (Neural Learning Machine) framework.

        The brain maintains a collection of neural regions, each containing
        populations of neurons with specific types and properties. Synaptic
        connections between neurons enable information processing and learning
        through various plasticity rules.

        Key Features:
        - Real-time neural simulation with configurable timestep
        - Multiple neural regions with independent dynamics
        - STDP and Hebbian plasticity mechanisms
        - Structural plasticity for connection growth and pruning
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages affecting plasticity rates
        - Working memory and episodic memory integration
        - Action production from neural activity

        Basic Usage:
            # Create brain with custom configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()

            # Run simulation
            for step in range(1000):
                brain.step(step)
                # Access statistics
                print(f"Firing neurons: {brain.getFiringNeuronCount()}")

            # Save brain state
            brain.save("brain_checkpoint.bin")
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration. Creates and sets up neural network with neurons and synapses.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Creates neurons and synapses according to config parameters. Returns True on success.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Advances neural dynamics, processes spikes, and applies plasticity.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Includes current time for time-dependent processes.")
        .def("reset", &Brain::reset,
             "Reset brain state. Stops all neurons, clears spikes, and resets synaptic weights to initial values.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Sensory data affects neural membrane potentials.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Temporarily increases membrane potential.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (Excitatory, Inhibitory, Sensory, Motor).")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns an Action object with motor commands.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Serializes neural weights, neuron states, and connectivity.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Restores neural weights, neuron states, and connectivity.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region to the brain. Returns region ID. name is optional identifier.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if region doesn't exist.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions in the brain.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs in the brain.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a list.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of neurons currently firing spikes.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (spikes per millisecond).")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio. >1.0 indicates excitation bias, <1.0 indicates inhibition bias.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since initialization.")
        .def("getMostActiveNeurons", [](Brain& self, size_t count) {
            return self.getSpikeSystem()->getMostActiveNeurons(count);
        }, py::arg("count"),
             "Get list of most active neuron IDs. Returns top N neurons by firing rate.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging).")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage. Affects plasticity rates and learning capabilities.")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration object used to initialize this brain.")
        .def("getFiringNeurons", [](Brain& self) {
            std::vector<NeuronId> firingNeurons;
            for (size_t i = 0; i < self.getTotalNeuronCount(); ++i) {
                auto* neuron = self.getNeuron(NeuronId(i));
                if (neuron && neuron->isFiring()) {
                    firingNeurons.push_back(neuron->getId());
                }
            }
            return firingNeurons;
        }, "Get list of currently firing neuron IDs.")
        .def("getNeuron", [](Brain& self, NeuronId id) {
            return self.getNeuron(id);
        }, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a specific neuron by ID. Returns None if neuron doesn't exist.")
        .def("getSynapse", [](Brain& self, SynapseId id) {
            return self.getSynapse(id);
        }, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a specific synapse by ID. Returns None if synapse doesn't exist.")
        .def("getStatistics", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as a Python dictionary.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status including statistics and configuration.")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses, " +
                   std::to_string(brain.getTotalSpikeCount()) + " spikes>";
        })
        .def("__str__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "NLM Brain (Phase " << static_cast<int>(brain.getDevelopmentalStage()) << ")\n";
            ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
            ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
            ss << "  Firing: " << brain.getFiringNeuronCount() << "\n";
            ss << "  Total Spikes: " << brain.getTotalSpikeCount() << "\n";
            ss << "  Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
            return ss.str();
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.

        The AgentBrain class serves as the interface between a neural brain and
        an external environment (world). It processes sensory input from the world,
        updates the brain with this information, and decodes neural activity into
        motor commands for world interaction.

        Key Features:
        - Sensory processing and injection into brain
        - Motor command decoding from neural activity
        - Reward-based neuromodulation (dopamine)
        - Curiosity-driven exploration
        - Developmental system updates
        - Novelty detection and prediction error
        - Enable/disable individual subsystems
        - Comprehensive state monitoring

        Basic Usage:
            # Setup brain and agent
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)

            # Initialize with world
            world = pynlm.createSimpleWorld()
            agent.initialize(world)

            # Enable learning subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)

            # Run simulation loop
            for step in range(1000):
                # Update world
                world.update(0.1)

                # Get sensory input
                percept = world.getSensoryPercept()

                # Process sensory input
                agent.processSensoryInput(percept)

                # Brain thinks
                brain.step(step)

                # Decode motor command
                motor_cmd = agent.decodeMotorCommand()

                # Apply action to world
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())

                # Apply reward modulation
                reward = ...  # Get reward from world
                agent.applyRewardModulation(reward, 0.0)

                # Update development
                agent.updateDevelopment(0.1)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain interface with underlying brain. The agent will use this brain for neural computation.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world. Sets up sensory input and output connections between agent and world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size (number of sensory channels).")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size (number of motor commands).")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept from world and inject into brain. Converts world percept to brain-compatible format.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Interprets neural activity as action commands.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation. Updates brain based on received reward and prediction.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update developmental system. Advances brain development and modulates plasticity rates.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage affecting brain maturation and plasticity.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (sum of all neuromodulator effects).")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level driving exploration and novelty seeking behavior.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level indicating detection of new or unexpected sensory input.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal used for learning and adjustment.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode. Clears internal state and prepares for fresh simulation.")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain object for direct access to neural computation.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward-based neuromodulation system.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity (growth and pruning of connections).")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable developmental system (brain maturation over time).")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration behavior.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is currently enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is currently enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is currently enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is currently enabled.")
        .def("getAgentStats", [](AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics as a Python dictionary.")
        .def("getSensoryConfig", [](AgentBrain& self) {
            return self.getSensoryInputSize();
        }, "Get sensory input configuration (expected input size).")
        .def("getMotorConfig", [](AgentBrain& self) {
            return self.getMotorOutputSize();
        }, "Get motor output configuration (expected output size).")
        .def("getEnablements", [](AgentBrain& self) {
            py::dict enablements;
            enablements["reward_modulation"] = self.isRewardModulationEnabled();
            enablements["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            enablements["development"] = self.isDevelopmentEnabled();
            enablements["curiosity"] = self.isCuriosityEnabled();
            return enablements;
        }, "Get current enablement status of all agent subsystems.")
        .def("setEnablements", [](AgentBrain& self, const py::dict& enablements) {
            if (enablements.contains("reward_modulation")) {
                self.enableRewardModulation(py::cast<bool>(enablements["reward_modulation"]));
            }
            if (enablements.contains("structural_plasticity")) {
                self.enableStructuralPlasticity(py::cast<bool>(enablements["structural_plasticity"]));
            }
            if (enablements.contains("development")) {
                self.enableDevelopment(py::cast<bool>(enablements["development"]));
            }
            if (enablements.contains("curiosity")) {
                self.enableCuriosity(py::cast<bool>(enablements["curiosity"]));
            }
            return true;
        }, py::arg("enablements"), "Enable/disable agent subsystems based on configuration dictionary.")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: Curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   ", Novelty=" + std::to_string(agent.getNoveltyLevel()) +
                   ", RewardModulation=" + std::to_string(agent.getNeuromodulationLevel()) + ">";
        })
        .def("__str__", [](const AgentBrain& agent) {
            std::stringstream ss;
            ss << "NLM Agent (Curiosity: " << agent.getCuriosityLevel() << ", Novelty: " << agent.getNoveltyLevel() << ")\n";
            ss << "  Neuromodulation Level: " << agent.getNeuromodulationLevel() << "\n";
            ss << "  Prediction Error: " << agent.getPredictionError() << "\n";
            ss << "  Developmental Stage: " << static_cast<int>(agent.getDevelopmentalStage()) << "\n";
            ss << "  Enabled Subsystems:\n";
            ss << "    Reward Modulation: " << (agent.isRewardModulationEnabled() ? "Yes" : "No") << "\n";
            ss << "    Structural Plasticity: " << (agent.isStructuralPlasticityEnabled() ? "Yes" : "No") << "\n";
            ss << "    Development: " << (agent.isDevelopmentEnabled() ? "Yes" : "No") << "\n";
            ss << "    Curiosity: " << (agent.isCuriosityEnabled() ? "Yes" : "No") << "\n";
            return ss.str();
        });

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
}

} // namespace nlm
