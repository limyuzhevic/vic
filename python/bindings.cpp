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
        
        Example usage:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> action = agent.decodeMotorCommand()
    )pbdoc";

    // Register custom exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    
    // Define NLM-specific exceptions
    py::class_<std::runtime_error>(m, "NLMError", 
        R"pbdoc(Exception raised by NLM operations)pbdoc")
        .def(py::init<const std::string&>());
        
    py::class_<std::invalid_argument>(m, "InvalidArgumentError",
        R"pbdoc(Exception raised for invalid argument values)pbdoc")
        .def(py::init<const std::string&>());

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Configuration class for the NLM neural simulation framework. This class provides
        a key-value storage system for configuration parameters, with support for
        different data types and configuration sources.
        
        The configuration system is used to control brain parameters, simulation
        settings, plasticity rules, neuromodulation, and development processes.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 2000)
            >>> config.set("plasticity.stdp.enable", True)
            >>> config.set("neuromod.dopamine.scale", 1.0)
            >>> neurons = config.getOr("brain.neuron_count", 1000)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file)")
        // Enhanced configuration access with type validation
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw std::invalid_argument("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get configuration value by key, throws if not found)")
        .def("try_get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            return value; // Returns None if not found
        }, py::arg("key"),
             R"pbdoc(Try to get configuration value by key, returns None if not found)")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (!value) {
                throw std::invalid_argument("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get configuration value as int, throws if not found)")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) {
                throw std::invalid_argument("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get configuration value as float, throws if not found)")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value) {
                throw std::invalid_argument("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get configuration value as bool, throws if not found)")
        .def("get_list", [](const Config& self, const std::string& key) {
            auto value = self.get<std::vector<double>>(key);
            if (!value) {
                throw std::invalid_argument("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get configuration value as list, throws if not found)")
        .def("get_keys", &Config::getKeys,
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
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Visual sensory input class representing camera-like vision data.
        Stores grayscale or color image data with associated metadata.
        
        Properties:
            width: Image width in pixels (default: 16)
            height: Image height in pixels (default: 16) 
            channels: Number of color channels (1=grayscale, 3=RGB)
            
        Example:
            >>> import pynlm
            >>> vision = pynlm.Vision(32, 32)  # 32x32 grayscale
            >>> vision.setData(list(range(1024)))
            >>> width = vision.getWidth()
            >>> height = vision.getHeight()
            >>> data = vision.getData()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width") = 16, py::arg("height") = 16, py::arg("channels") = 3,
             R"pbdoc(Initialize vision input with specified dimensions)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data from a flat list of pixel values)")
        .def("getData", &Vision::getData,
             R"pbdoc(Get vision data as a flat list of pixel values)")
        .def("toArray", [](const Vision& self) {
            // Convert vision data to numpy array for easier processing
            py::array_t<float> arr({self.getHeight(), self.getWidth(), self.getChannels()});
            auto buf = arr.mutable_unchecked<3>();
            
            const auto& data = self.getData();
            size_t data_idx = 0;
            
            for (ssize_t h = 0; h < buf.shape(0); ++h) {
                for (ssize_t w = 0; w < buf.shape(1); ++w) {
                    for (ssize_t c = 0; c < buf.shape(2); ++c) {
                        if (data_idx < data.size()) {
                            buf(h, w, c) = data[data_idx++];
                        } else {
                            buf(h, w, c) = 0.0f;
                        }
                    }
                }
            }
            
            return arr;
        }, R"pbdoc(Convert vision data to numpy array for easier processing)")
        .def("getWidth", &Vision::getWidth,
             "Get image width in pixels")
        .def("getHeight", &Vision::getHeight,
             "Get image height in pixels")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels");

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        A SimpleWorld represents the simulated environment in which an agent operates.
        It provides the interface between the agent's brain and the external world,
        handling sensory input and motor output interactions.
        
        The world supports:
        - 2D grid-based spatial representation
        - Vision, touch, and internal sensory inputs
        - Agent movement and interaction capabilities
        - Resource and hazard management
        
        Example:
            >>> import pynlm
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(100, 100, 16, 16)  # 100x100 grid, 16x16 vision
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> percept = world.getSensoryPercept()
            >>> action = agent.decodeMotorCommand()
            >>> world.applyMotorCommand(action, world.getSimulationTime())
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions and vision capabilities)")
        .def("reset", &SimpleWorld::reset,
             "Reset world state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and get world feedback")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory perception")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        // Property-based access for easier world manipulation
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        // Read-only properties
        .def_readonly("width", &SimpleWorld::getWidth,
                     "World width in grid units")
        .def_readonly("height", &SimpleWorld::getHeight,
                     "World height in grid units")
        .def_readonly("simulation_time", &SimpleWorld::getSimulationTime,
                     "Current simulation time")
        // Properties with setters
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy,
                     "Maximum agent energy")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Energy decay rate per timestep")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        A Brain represents a complete neural network simulation with multiple regions,
        each containing populations of neurons connected by synapses. The brain processes
        sensory inputs and produces motor outputs through its neural circuitry.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            >>> neurons = brain.getTotalNeuronCount()
            >>> print(f"Brain has {neurons} neurons")
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object self, py::object, py::object, py::object) {
            self.attr("reset")();
            return py::none();
        })
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
        .def("getRegion", [](Brain& b, RegionId id) {
            auto region = b.getRegion(id);
            if (!region) {
                throw std::runtime_error("Region not found");
            }
            return region;
        }, py::arg("id"),
             R"pbdoc(Get region by ID, throws if not found)pbdoc")
        .def("try_getRegion", [](Brain& b, RegionId id) {
            return b.getRegion(id);
        }, py::arg("id"),
             R"pbdoc(Try to get region, returns None if not found)pbdoc")
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
        // Property-based access for easier Python usage
        .def_readonly("neuron_count", &Brain::getTotalNeuronCount,
                     "Total number of neurons in the brain")
        .def_readonly("synapse_count", &Brain::getTotalSynapseCount,
                     "Total number of synapses in the brain")
        .def_readonly("active_neuron_count", &Brain::getActiveNeuronCount,
                     "Number of currently active neurons")
        .def_readonly("firing_neuron_count", &Brain::getFiringNeuronCount,
                     "Number of currently firing neurons")
        .def_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                     "Average firing rate across all neurons")
        .def_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
                     "Excitation/inhibition balance ratio")
        .def_readonly("total_spike_count", &Brain::getTotalSpikeCount,
                     "Total number of spikes recorded")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        An AgentBrain acts as an interface between a neural brain and an external world,
        handling sensory transduction and motor decoding. It processes sensory inputs
        from the world and converts neural activity into motor commands.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> percept = world.getSensoryPercept()
            >>> agent.processSensoryInput(percept)
            >>> action = agent.decodeMotorCommand()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object self, py::object, py::object, py::object) {
            self.attr("reset")();
            return py::none();
        })
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
        // Property-based access for agent state
        .def_readonly("brain", &AgentBrain::getBrain,
                     "Get the underlying brain (shared_ptr)")
        .def_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
                     "Expected sensory input size")
        .def_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
                     "Expected motor output size")
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
        
        // Batch processing for improved performance
        .def("processBatch", &AgentBrain::processBatch,
             py::arg("percepts"),
             R"pbdoc(Process multiple sensory inputs in batch for efficiency)")
        .def("getState", &AgentBrain::getState,
             R"pbdoc(Get agent state for checkpointing/saving)")
        .def("setState", &AgentBrain::setState,
             py::arg("state"),
             R"pbdoc(Restore agent state from checkpoint)")
        .def("resetEpisode", &AgentBrain::resetEpisode,
             py::arg("cleanup_memory") = true,
             R"pbdoc(Reset agent for new episode with optional memory cleanup)");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)")
    
    // Enhanced factory functions with error handling and validation
    m.def("createConfigFromDict", [](const py::dict& config_dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            if (key.empty()) {
                throw std::invalid_argument("Configuration key cannot be empty");
            }
            // Try different types to match the Config value types
            try {
                std::string str_value = py::cast<std::string>(item.second);
                config->set(key, str_value);
            } catch (const std::exception&) {
                try {
                    int int_value = py::cast<int>(item.second);
                    config->set(key, int_value);
                } catch (const std::exception&) {
                    try {
                        double float_value = py::cast<double>(item.second);
                        config->set(key, float_value);
                    } catch (const std::exception&) {
                        try {
                            bool bool_value = py::cast<bool>(item.second);
                            config->set(key, bool_value);
                        } catch (const std::exception&) {
                            std::vector<double> list_value;
                            if (py::isinstance<py::list>(item.second)) {
                                auto py_list = item.second.cast<py::list>();
                                list_value.reserve(py_list.size());
                                for (size_t i = 0; i < py_list.size(); ++i) {
                                    list_value.push_back(py_list[i].cast<double>());
                                }
                                config->set(key, list_value);
                            } else {
                                throw std::invalid_argument("Unsupported type for configuration value at key: " + key);
                            }
                        }
                    }
                }
            }
        }
        return config;
    }, py::arg("config_dict"), R"pbdoc(Create config from Python dictionary)")
    
    // Create brain with standard configuration
    m.def("createStandardBrain", []() -> std::shared_ptr<Brain> {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("development.enabled", true);
        config->set("curiosity.enabled", true);
        
        auto brain = createBrain(config);
        brain->initialize();
        return brain;
    }, R"pbdoc(Create a brain with standard configuration)")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Constants for invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
