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
        
        This enhanced binding includes comprehensive error handling, parameter validation,
        performance monitoring interfaces, and advanced Python integration features.
    )pbdoc";

    // Enhanced exception hierarchy for Python
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::logic_error>(m, "LogicError");
    py::register_exception<std::runtime_error>(m, "ConfigurationError", "Configuration-related errors");
    py::register_exception<std::runtime_error>(m, "SimulationError", "Simulation operation errors");
    py::register_exception<std::runtime_error>(m, "MemoryError", "Memory allocation errors");

    // Helper function for type validation and error messages
    auto validatePointer = [](const void* ptr, const std::string& paramName) {
        if (!ptr) {
            throw py::value_error(std::string("Null pointer provided for parameter: ") + paramName);
        }
    };

    auto validateRange = [](size_t value, size_t min, size_t max, const std::string& paramName) {
        if (value < min || value > max) {
            throw py::value_error(std::string("Parameter ") + paramName + 
                                " has value " + std::to_string(value) +
                                ". Must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    };

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

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with comprehensive error handling and validation.
        
        The Brain class manages neural simulation with multiple regions, neurons, and synapses.
        This enhanced binding includes parameter validation, performance monitoring,
        and detailed error reporting.
        
        Args:
            config: Configuration object for brain initialization
            
        Attributes:
            regions: Dictionary mapping region names to RegionId objects
            neurons: Dictionary mapping neuron IDs to neuron data
            synapses: Dictionary mapping synapse IDs to synapse data
            
        Examples:
            >>> config = Config()
            >>> config.set("brain/neuron_count", 1000)
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> step = SimulationStep()
            >>> brain.step(step)
            >>> print(f"Total neurons: {brain.getTotalNeuronCount()}")
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration. "
             "Args: config - Configuration object for brain initialization. "
             "Raises RuntimeError if initialization fails.")
        .def("initialize", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                if (!self->initialize()) {
                    throw py::runtime_error("Brain initialization failed");
                }
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::initialize: ") + e.what());
            }
        }, "Initialize the brain with configuration. "
            "Returns True on success. "
            "Raises ConfigurationError if configuration is invalid, "
            "SimulationError if initialization fails, "
            "or RuntimeError for other errors.")
        .def("step", [](std::shared_ptr<Brain> self, const SimulationStep& currentStep, 
                     std::optional<Timestamp> currentTime = std::nullopt) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&currentStep, "currentStep");
                
                if (currentTime) {
                    self->step(currentStep, *currentTime);
                } else {
                    self->step(currentStep);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::step: ") + e.what());
            }
        }, py::arg("currentStep"), py::arg("currentTime") = std::nullopt,
         "Perform a simulation step with optional timestamp. "
         "Args: currentStep - The simulation step to execute. "
         "      currentTime - Optional timestamp for the step. "
         "Returns True on success. "
         "Raises SimulationError if step execution fails, "
         "InvalidArgumentError if step is invalid, "
         "or RuntimeError for other errors.")
        .def("receiveSensoryInput", [](std::shared_ptr<Brain> self, const SensoryPercept& input) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&input, "input");
                self->receiveSensoryInput(input);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::receiveSensoryInput: ") + e.what());
            }
        }, py::arg("input"), "Inject sensory input into the brain. "
            "Args: input - Sensory percept data to inject. "
            "Returns True on success. "
            "Raises InvalidArgumentError if input is invalid, "
            "SimulationError if input processing fails, "
            "or RuntimeError for other errors.")
        .def("injectCurrent", [](std::shared_ptr<Brain> self, const NeuronId& neuron, 
                               float current) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&neuron, "neuron");
                validateRange(current, -1000.0f, 1000.0f, "current");
                self->injectCurrent(neuron, current);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::injectCurrent: ") + e.what());
            }
        }, py::arg("neuron"), py::arg("current"),
         "Inject current into a specific neuron. "
         "Args: neuron - Neuron ID to inject current into. "
         "      current - Current value to inject (range: -1000 to 1000). "
         "Returns True on success. "
         "Raises InvalidArgumentError if neuron ID is invalid or current is out of range, "
         "SimulationError if injection fails, "
         "or RuntimeError for other errors.")
        .def("injectCurrentToNeurons", [](std::shared_ptr<Brain> self, NeuronType type, 
                                        float current) {
            try {
                validatePointer(self.get(), "brain");
                validateRange(current, -1000.0f, 1000.0f, "current");
                self->injectCurrentToNeurons(type, current);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::injectCurrentToNeurons: ") + e.what());
            }
        }, py::arg("type"), py::arg("current"),
         "Inject current into all neurons of a specific type. "
         "Args: type - Neuron type to inject into. "
         "      current - Current value to inject (range: -1000 to 1000). "
         "Returns True on success. "
         "Raises InvalidArgumentError if current is out of range, "
         "SimulationError if injection fails, "
         "or RuntimeError for other errors.")
        .def("produceAction", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->produceAction();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::produceAction: ") + e.what());
            }
        }, "Produce motor action based on neural activity. "
            "Returns Action object representing the produced action. "
            "Raises SimulationError if action production fails, "
            "or RuntimeError for other errors.")
        .def("reset", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                self->reset();
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::reset: ") + e.what());
            }
        }, "Reset brain state to initial conditions. "
            "Returns True on success. "
            "Raises SimulationError if reset fails, "
            "or RuntimeError for other errors.")
        .def("save", [](std::shared_ptr<Brain> self, const std::string& filepath) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&filepath, "filepath");
                if (filepath.empty()) {
                    throw py::value_error("File path cannot be empty");
                }
                self->save(filepath);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::save: ") + e.what());
            }
        }, py::arg("filepath"), "Save brain state to file. "
            "Args: filepath - Path to save brain state to. "
            "Returns True on success. "
            "Raises InvalidArgumentError if filepath is invalid, "
            "SimulationError if save fails, "
            "MemoryError if insufficient memory, "
            "or RuntimeError for other errors.")
        .def("load", [](std::shared_ptr<Brain> self, const std::string& filepath) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&filepath, "filepath");
                if (filepath.empty()) {
                    throw py::value_error("File path cannot be empty");
                }
                self->load(filepath);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::load: ") + e.what());
            }
        }, py::arg("filepath"), "Load brain state from file. "
            "Args: filepath - Path to load brain state from. "
            "Returns True on success. "
            "Raises InvalidArgumentError if filepath is invalid, "
            "SimulationError if load fails, "
            "MemoryError if insufficient memory, "
            "or RuntimeError for other errors.")
        .def("addRegion", [](std::shared_ptr<Brain> self, const std::string& name) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&name, "name");
                if (name.empty()) {
                    throw py::value_error("Region name cannot be empty");
                }
                return self->addRegion(name);
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::addRegion: ") + e.what());
            }
        }, py::arg("name") = "", "Add a new neural region. "
            "Args: name - Optional name for the region. "
            "Returns RegionId for the newly created region. "
            "Raises InvalidArgumentError if name is invalid, "
            "SimulationError if region creation fails, "
            "or RuntimeError for other errors.")
        .def("getRegion", [](std::shared_ptr<Brain> self, const RegionId& id) {
            try {
                validatePointer(self.get(), "brain");
                validatePointer(&id, "id");
                return self->getRegion(id);
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getRegion: ") + e.what());
            }
        }, py::arg("id"), py::return_value_policy::reference_internal,
         "Get a region by ID. "
         "Args: id - Region ID to retrieve. "
         "Returns reference to the Region object. "
         "Raises InvalidArgumentError if region ID is invalid, "
         "OutOfRangeError if region doesn't exist, "
         "or RuntimeError for other errors.")
        .def("getRegionCount", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getRegionCount();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getRegionCount: ") + e.what());
            }
        }, "Get the number of regions. "
            "Returns size_t count of regions. "
            "Raises RuntimeError for other errors.")
        .def("getRegionIds", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getRegionIds();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getRegionIds: ") + e.what());
            }
        }, "Get all region IDs. "
            "Returns vector<RegionId> of all region IDs. "
            "Raises RuntimeError for other errors.")
        .def("getRegions", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getRegions();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getRegions: ") + e.what());
            }
        }, py::return_value_policy::reference_internal,
         "Get all regions. "
         "Returns vector<RegionId> of all region IDs. "
         "Raises RuntimeError for other errors.")
        .def("getTotalNeuronCount", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getTotalNeuronCount();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getTotalNeuronCount: ") + e.what());
            }
        }, "Get total neuron count across all regions. "
            "Returns size_t total neuron count. "
            "Raises RuntimeError for other errors.")
        .def("getTotalSynapseCount", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getTotalSynapseCount();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getTotalSynapseCount: ") + e.what());
            }
        }, "Get total synapse count across all regions. "
            "Returns size_t total synapse count. "
            "Raises RuntimeError for other errors.")
        .def("getActiveNeuronCount", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getActiveNeuronCount();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getActiveNeuronCount: ") + e.what());
            }
        }, "Get count of active neurons. "
            "Returns size_t count of active neurons. "
            "Raises RuntimeError for other errors.")
        .def("getFiringNeuronCount", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getFiringNeuronCount();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getFiringNeuronCount: ") + e.what());
            }
        }, "Get count of currently firing neurons. "
            "Returns size_t count of firing neurons. "
            "Raises RuntimeError for other errors.")
        .def("getAverageFiringRate", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getAverageFiringRate();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getAverageFiringRate: ") + e.what());
            }
        }, "Get average firing rate across all neurons. "
            "Returns double average firing rate (Hz). "
            "Raises RuntimeError for other errors.")
        .def("getExcitationInhibitionRatio", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getExcitationInhibitionRatio();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getExcitationInhibitionRatio: ") + e.what());
            }
        }, "Get excitation/inhibition balance ratio. "
            "Returns double E/I ratio. "
            "Raises RuntimeError for other errors.")
        .def("getTotalSpikeCount", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getTotalSpikeCount();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getTotalSpikeCount: ") + e.what());
            }
        }, "Get total spike count. "
            "Returns size_t total spike count. "
            "Raises RuntimeError for other errors.")
        .def("getDevelopmentalStage", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getDevelopmentalStage();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getDevelopmentalStage: ") + e.what());
            }
        }, "Get current developmental stage. "
            "Returns DevelopmentalStage enum value. "
            "Raises RuntimeError for other errors.")
        .def("setDevelopmentalStage", [](std::shared_ptr<Brain> self, 
                                       DevelopmentalStage stage) {
            try {
                validatePointer(self.get(), "brain");
                self->setDevelopmentalStage(stage);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::setDevelopmentalStage: ") + e.what());
            }
        }, py::arg("stage"), "Set developmental stage. "
            "Args: stage - Developmental stage to set. "
            "Returns True on success. "
            "Raises InvalidArgumentError if stage is invalid, "
            "SimulationError if stage transition fails, "
            "or RuntimeError for other errors.")
        .def("getConfig", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                return self->getConfig();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::getConfig: ") + e.what());
            }
        }, py::return_value_policy::reference_internal,
         "Get the configuration. "
         "Returns Config object. "
         "Raises RuntimeError for other errors.")
        .def("logStatus", [](std::shared_ptr<Brain> self) {
            try {
                validatePointer(self.get(), "brain");
                self->logStatus();
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain::logStatus: ") + e.what());
            }
        }, "Log brain status to console or log file. "
            "Returns True on success. "
            "Raises RuntimeError for other errors.");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world with comprehensive error handling.
        
        AgentBrain provides high-level interface for agent-based neural simulation,
        handling sensory processing, motor control, and development with enhanced
        error reporting and validation.
        
        Args:
            brain: Underlying Brain object to control
            
        Attributes:
            is_initialized: Boolean indicating initialization state
            is_enabled: Dictionary of enabled features (reward_modulation, etc.)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain with underlying brain. "
             "Args: brain - Brain object to control. "
             "Raises InvalidArgumentError if brain is null.")
        .def("initialize", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world) {
            try {
                validatePointer(self.get(), "agent_brain");
                validatePointer(world.get(), "world");
                if (!self->initialize(*world)) {
                    throw py::runtime_error("AgentBrain initialization failed");
                }
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::initialize: ") + e.what());
            }
        }, py::arg("world"), "Initialize with world. "
            "Args: world - World object to interact with. "
            "Returns True on success. "
            "Raises InvalidArgumentError if world is null, "
            "SimulationError if initialization fails, "
            "or RuntimeError for other errors.")
        .def("getSensoryInputSize", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getSensoryInputSize();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getSensoryInputSize: ") + e.what());
            }
        }, "Get expected sensory input size. "
            "Returns size_t expected sensory input size. "
            "Raises RuntimeError for other errors.")
        .def("getMotorOutputSize", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getMotorOutputSize();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getMotorOutputSize: ") + e.what());
            }
        }, "Get expected motor output size. "
            "Returns size_t expected motor output size. "
            "Raises RuntimeError for other errors.")
        .def("processSensoryInput", [](std::shared_ptr<AgentBrain> self, const SensoryPercept& percept) {
            try {
                validatePointer(self.get(), "agent_brain");
                validatePointer(&percept, "percept");
                if (!self->processSensoryInput(percept)) {
                    throw py::runtime_error("AgentBrain sensory processing failed");
                }
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::processSensoryInput: ") + e.what());
            }
        }, py::arg("percept"), "Process sensory percept and inject into brain. "
            "Args: percept - Sensory percept data to process. "
            "Returns True on success. "
            "Raises InvalidArgumentError if percept is invalid, "
            "SimulationError if processing fails, "
            "or RuntimeError for other errors.")
        .def("decodeMotorCommand", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->decodeMotorCommand();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::decodeMotorCommand: ") + e.what());
            }
        }, "Decode brain motor activity into motor command. "
            "Returns Action object representing the motor command. "
            "Raises SimulationError if decoding fails, "
            "or RuntimeError for other errors.")
        .def("applyRewardModulation", [](std::shared_ptr<AgentBrain> self, 
                                       float reward, float predictedReward) {
            try {
                validatePointer(self.get(), "agent_brain");
                validateRange(reward, -100.0f, 100.0f, "reward");
                validateRange(predictedReward, -100.0f, 100.0f, "predictedReward");
                if (!self->applyRewardModulation(reward, predictedReward)) {
                    throw py::runtime_error("Reward modulation failed");
                }
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::applyRewardModulation: ") + e.what());
            }
        }, py::arg("reward"), py::arg("predictedReward"),
         "Apply reward-based neuromodulation. "
         "Args: reward - Actual reward received. "
         "      predictedReward - Predicted reward. "
         "Returns True on success. "
         "Raises InvalidArgumentError if reward values are out of range, "
         "SimulationError if modulation fails, "
         "or RuntimeError for other errors.")
        .def("updateDevelopment", [](std::shared_ptr<AgentBrain> self, Timestamp timestep) {
            try {
                validatePointer(self.get(), "agent_brain");
                validateRange(timestep, 0.0, 1e6, "timestep");
                if (!self->updateDevelopment(timestep)) {
                    throw py::runtime_error("Development update failed");
                }
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::updateDevelopment: ") + e.what());
            }
        }, py::arg("timestep"), "Update development system. "
            "Args: timestep - Time step for development update. "
            "Returns True on success. "
            "Raises InvalidArgumentError if timestep is invalid, "
            "SimulationError if development update fails, "
            "or RuntimeError for other errors.")
        .def("getDevelopmentalStage", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getDevelopmentalStage();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getDevelopmentalStage: ") + e.what());
            }
        }, "Get current developmental stage. "
            "Returns DevelopmentalStage enum value. "
            "Raises RuntimeError for other errors.")
        .def("getNeuromodulationLevel", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getNeuromodulationLevel();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getNeuromodulationLevel: ") + e.what());
            }
        }, "Get current neuromodulation level. "
            "Returns float neuromodulation level (0.0 to 1.0). "
            "Raises RuntimeError for other errors.")
        .def("getCuriosityLevel", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getCuriosityLevel();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getCuriosityLevel: ") + e.what());
            }
        }, "Get curiosity level. "
            "Returns float curiosity level (0.0 to 1.0). "
            "Raises RuntimeError for other errors.")
        .def("getNoveltyLevel", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getNoveltyLevel();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getNoveltyLevel: ") + e.what());
            }
        }, "Get novelty level. "
            "Returns float novelty level (0.0 to 1.0). "
            "Raises RuntimeError for other errors.")
        .def("getPredictionError", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getPredictionError();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getPredictionError: ") + e.what());
            }
        }, "Get prediction error. "
            "Returns float prediction error. "
            "Raises RuntimeError for other errors.")
        .def("reset", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                self->reset();
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::reset: ") + e.what());
            }
        }, "Reset agent for new episode. "
            "Returns True on success. "
            "Raises SimulationError if reset fails, "
            "or RuntimeError for other errors.")
        .def("getBrain", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->getBrain();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::getBrain: ") + e.what());
            }
        }, py::return_value_policy::reference_internal,
         "Get the underlying brain. "
         "Returns shared_ptr<Brain> to the underlying brain. "
         "Raises RuntimeError for other errors.")
        .def("enableRewardModulation", [](std::shared_ptr<AgentBrain> self, bool enable) {
            try {
                validatePointer(self.get(), "agent_brain");
                self->enableRewardModulation(enable);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::enableRewardModulation: ") + e.what());
            }
        }, py::arg("enable"), "Enable or disable reward modulation. "
            "Args: enable - True to enable, False to disable. "
            "Returns True on success. "
            "Raises RuntimeError for other errors.")
        .def("enableStructuralPlasticity", [](std::shared_ptr<AgentBrain> self, bool enable) {
            try {
                validatePointer(self.get(), "agent_brain");
                self->enableStructuralPlasticity(enable);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::enableStructuralPlasticity: ") + e.what());
            }
        }, py::arg("enable"), "Enable or disable structural plasticity. "
            "Args: enable - True to enable, False to disable. "
            "Returns True on success. "
            "Raises RuntimeError for other errors.")
        .def("enableDevelopment", [](std::shared_ptr<AgentBrain> self, bool enable) {
            try {
                validatePointer(self.get(), "agent_brain");
                self->enableDevelopment(enable);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::enableDevelopment: ") + e.what());
            }
        }, py::arg("enable"), "Enable or disable development. "
            "Args: enable - True to enable, False to disable. "
            "Returns True on success. "
            "Raises RuntimeError for other errors.")
        .def("enableCuriosity", [](std::shared_ptr<AgentBrain> self, bool enable) {
            try {
                validatePointer(self.get(), "agent_brain");
                self->enableCuriosity(enable);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::enableCuriosity: ") + e.what());
            }
        }, py::arg("enable"), "Enable or disable curiosity. "
            "Args: enable - True to enable, False to disable. "
            "Returns True on success. "
            "Raises RuntimeError for other errors.")
        .def("isRewardModulationEnabled", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->isRewardModulationEnabled();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::isRewardModulationEnabled: ") + e.what());
            }
        }, "Check if reward modulation is enabled. "
            "Returns bool true if enabled. "
            "Raises RuntimeError for other errors.")
        .def("isStructuralPlasticityEnabled", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->isStructuralPlasticityEnabled();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::isStructuralPlasticityEnabled: ") + e.what());
            }
        }, "Check if structural plasticity is enabled. "
            "Returns bool true if enabled. "
            "Raises RuntimeError for other errors.")
        .def("isDevelopmentEnabled", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->isDevelopmentEnabled();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::isDevelopmentEnabled: ") + e.what());
            }
        }, "Check if development is enabled. "
            "Returns bool true if enabled. "
            "Raises RuntimeError for other errors.")
        .def("isCuriosityEnabled", [](std::shared_ptr<AgentBrain> self) {
            try {
                validatePointer(self.get(), "agent_brain");
                return self->isCuriosityEnabled();
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("AgentBrain::isCuriosityEnabled: ") + e.what());
            }
        }, "Check if curiosity is enabled. "
            "Returns bool true if enabled. "
            "Raises RuntimeError for other errors.");

    // Helper functions with error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            return std::make_shared<Config>();
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("createDefaultConfig: ") + e.what());
        }
    }, "Create a default configuration. "
        "Returns Config object. "
        "Raises RuntimeError for other errors.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            validatePointer(config.get(), "config");
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("createBrain: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration. "
        "Args: config - Configuration object for brain initialization. "
        "Returns Brain object. "
        "Raises InvalidArgumentError if config is null, "
        "RuntimeError for other errors.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            return std::make_shared<SimpleWorld>();
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("createSimpleWorld: ") + e.what());
        }
    }, "Create a new simple world. "
        "Returns SimpleWorld object. "
        "Raises RuntimeError for other errors.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            validatePointer(brain.get(), "brain");
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("createAgentBrain: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface. "
        "Args: brain - Brain object to control. "
        "Returns AgentBrain object. "
        "Raises InvalidArgumentError if brain is null, "
        "RuntimeError for other errors.")

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
