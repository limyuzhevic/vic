#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace py = pybind11;
namespace nlm {

// Custom exception types for better error handling
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message) : NLMError("Config Error: " + message) {}
};

class BrainError : public NLMError {
public:
    BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class WorldError : public NLMError {
public:
    WorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions with pybind11
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError.ptr());
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError.ptr());
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError.ptr());

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
        .def("set_from_dict", [](Config& self, const py::dict& dict) {
            // Validate dictionary keys and values
            for (auto item : dict) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                
                // Validate key format
                if (key.empty() || key.find('.') == std::string::npos) {
                    throw ConfigError("Invalid configuration key format: '" + key + "'. Keys should use dot notation (e.g., 'brain.neuron_count')");
                }
                
                // Try to convert based on type
                try {
                    if (value.is(py::int_())) {
                        int int_value = py::cast<int>(value);
                        // Validate range for specific keys
                        if (key == "brain.neuron_count" && (int_value < 1 || int_value > 1000000)) {
                            throw ConfigError("Neuron count must be between 1 and 1,000,000, got " + std::to_string(int_value));
                        }
                        self.set<int>(key, int_value);
                    } else if (value.is(py::float_())) {
                        float float_value = py::cast<float>(value);
                        if (key == "brain.synapse_density" && (float_value < 0.0f || float_value > 1.0f)) {
                            throw ConfigError("Synapse density must be between 0.0 and 1.0, got " + std::to_string(float_value));
                        }
                        self.set<float>(key, float_value);
                    } else if (value.is(py::bool_())) {
                        bool bool_value = py::cast<bool>(value);
                        self.set<bool>(key, bool_value);
                    } else if (value.is(py::str())) {
                        std::string str_value = py::cast<std::string>(value);
                        // Validate string values for specific keys
                        if (key == "brain.neuron_type" && 
                            str_value != "excitatory" && str_value != "inhibitory" && 
                            str_value != "sensory" && str_value != "motor" &&
                            str_value != "modulatory" && str_value != "internal") {
                            throw ConfigError("Invalid neuron type: '" + str_value + "'. Must be: excitatory, inhibitory, sensory, motor, modulatory, or internal");
                        }
                        self.set<std::string>(key, str_value);
                    } else if (value.is(py::list())) {
                        // Handle list values
                        py::list list_value = py::cast<py::list>(value);
                        std::vector<float> float_list;
                        for (auto item : list_value) {
                            float_list.push_back(py::cast<float>(item));
                        }
                        self.set<std::vector<float>>(key, float_list);
                    } else {
                        throw ConfigError("Unsupported value type for key '" + key + "'. Supported types: int, float, bool, string, list");
                    }
                } catch (const py::cast_error& e) {
                    throw ConfigError("Cannot convert value for key '" + key + "': " + std::string(e.what()));
                }
            }
        }, py::arg("dict"),
            "Set multiple configuration values from a Python dictionary. Args: dict (dict) - Dictionary mapping configuration keys to values. Key format: 'section.key' (e.g., 'brain.neuron_count'). Supported value types: int, float, bool, string, list. Returns: None")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<py::object>(key);
                if (value) {
                    // Try to preserve the original Python type if possible
                    result[key] = *value;
                }
            }
            return result;
        }, "Convert configuration to a Python dictionary. Returns: dict with all configuration key-value pairs")
        .def("from_preset", [](std::shared_ptr<Config> self, const std::string& name) {
            // Create a default config and apply preset
            auto preset_config = std::make_shared<Config>();
            
            if (name == "default") {
                // Set default values
                preset_config->set("brain.neuronCount", 1000);
                preset_config->set("brain.regionCount", 10);
                preset_config->set("brain.learningRate", 0.01);
                preset_config->set("brain.inhibitionStrength", 1.0);
                preset_config->set("brain.excitationStrength", 1.0);
                preset_config->set("world.width", 100.0);
                preset_config->set("world.height", 100.0);
                preset_config->set("world.visionWidth", 5);
                preset_config->set("world.visionHeight", 5);
                preset_config->set("agent.maxEnergy", 100.0);
                preset_config->set("agent.energyDecayRate", 0.01);
            }
            else if (name == "high_plasticity") {
                preset_config->set("brain.neuronCount", 2000);
                preset_config->set("brain.regionCount", 20);
                preset_config->set("brain.learningRate", 0.05);
                preset_config->set("brain.inhibitionStrength", 0.5);
                preset_config->set("brain.excitationStrength", 1.5);
                preset_config->set("brain.stdpEnabled", true);
                preset_config->set("brain.hebbianEnabled", true);
                preset_config->set("world.width", 200.0);
                preset_config->set("world.height", 200.0);
                preset_config->set("world.visionWidth", 10);
                preset_config->set("world.visionHeight", 10);
            }
            else if (name == "agent_training") {
                preset_config->set("brain.neuronCount", 500);
                preset_config->set("brain.regionCount", 5);
                preset_config->set("brain.learningRate", 0.02);
                preset_config->set("brain.inhibitionStrength", 2.0);
                preset_config->set("brain.excitationStrength", 0.8);
                preset_config->set("agent.maxEnergy", 50.0);
                preset_config->set("agent.energyDecayRate", 0.05);
                preset_config->set("agent.rewardModulation", true);
                preset_config->set("agent.curiosityEnabled", true);
            }
            else {
                throw std::runtime_error("Unknown preset name: " + name);
            }
            
            return preset_config;
        }, py::arg("name"),
             "Create configuration from a named preset. Args: name (str) - Preset name ('default', 'high_plasticity', 'agent_training'). Returns: Config object")
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
.def("configure", [](SimpleWorld& self, size_t width, size_t height, 
                    size_t visionWidth, size_t visionHeight) {
            // Validate dimensions
            if (width == 0 || height == 0) {
                throw WorldError("World dimensions must be positive. Got: width=" + std::to_string(width) + ", height=" + std::to_string(height));
            }
            
            if (visionWidth > width || visionHeight > height) {
                throw WorldError("Vision dimensions cannot exceed world dimensions. Vision: " + std::to_string(visionWidth) + "x" + std::to_string(visionHeight) + ", World: " + std::to_string(width) + "x" + std::to_string(height));
            }
            
            if (visionWidth == 0 || visionHeight == 0) {
                throw WorldError("Vision dimensions must be positive. Got: visionWidth=" + std::to_string(visionWidth) + ", visionHeight=" + std::to_string(visionHeight));
            }
            
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions. Args: width (int) - World width, height (int) - World height, visionWidth (int) - Vision field width, visionHeight (int) - Vision field height. Throws: WorldError if dimensions are invalid. Returns: None")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("place_agent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
             "Place agent at starting position. Pythonic alias for setAgentStart. Args: x (float) - X coordinate, y (float) - Y coordinate")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("add_resource", [](SimpleWorld& self, WorldObjectType type, float value) {
            WorldObject resource(0.0f, 0.0f, type, value);
            self.addObject(resource);
        }, py::arg("type"), py::arg("value"),
             "Add a resource object to the world. Args: type (WorldObjectType) - Type of object, value (float) - Value/reward of object. Note: Position not specified, uses default (0,0)")
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
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
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        })
        .def("__exit__", [](SimpleWorld& self, py::object, py::object, py::object) {
            self.reset();
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
        // Pythonic convenience methods
        .def("run_for_steps", [](Brain& self, size_t num_steps, bool validate_state) {
            if (num_steps == 0) {
                throw BrainError("Cannot run simulation with 0 steps. Use at least 1 step.");
            }
            
            if (num_steps > 1000000) {
                py::warning("Running a large number of steps (" + std::to_string(num_steps) + ") may take a long time and consume significant memory.");
            }
            
            if (validate_state) {
                // Validate brain state using individual methods
                if (self.getTotalNeuronCount() == 0) {
                    throw BrainError("Cannot run simulation: Brain has no neurons. Ensure brain is properly initialized.");
                }
            }
            
            for (size_t i = 0; i < num_steps; ++i) {
                self.step(i);
            }
        }, py::arg("num_steps"), py::arg("validate_state") = true,
             "Run simulation for N steps. Args: num_steps (int) - Number of simulation steps to run. validate_state (bool) - If true, validate brain state before running. Throws: BrainError if brain has invalid state. Returns: None")
        .def("reset_and_run", [](Brain& self, size_t num_steps) {
            self.reset();
            self.run_for_steps(num_steps);
        }, py::arg("num_steps"),
             "Reset brain state and run for N steps. Args: num_steps (int) - Number of simulation steps to run after reset")
        .def("get_brain_state", [](Brain& self) {
            py::dict state;
            state["total_neurons"] = self.getTotalNeuronCount();
            state["total_synapses"] = self.getTotalSynapseCount();
            state["active_neurons"] = self.getActiveNeuronCount();
            state["firing_neurons"] = self.getFiringNeuronCount();
            state["average_firing_rate"] = self.getAverageFiringRate();
            state["excitatory_inhibitory_ratio"] = self.getExcitationInhibitionRatio();
            state["total_spikes"] = self.getTotalSpikeCount();
            state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return state;
        }, "Get comprehensive brain state as a Python dictionary. Returns: dict containing brain statistics")
        .def("set_neuron_current", [](Brain& self, const NeuronId& neuron, float current) {
            self.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Args: neuron (NeuronId) - Target neuron, current (float) - Current value to inject")
         .def("get_connected_neurons", [](Brain& self, const NeuronId& neuron) {
             // Note: This assumes Brain has a method to get connected neurons
             // If not available, we'll need to add it to the Brain class
             std::vector<NeuronId> connected;
             // Placeholder implementation - replace with actual brain logic
             return connected;
         }, py::arg("neuron"),
              "Get connected neuron IDs for a given neuron. Args: neuron (NeuronId) - Source neuron ID. Returns: list of NeuronId objects")
        // Context manager support with error recovery
        .def("__enter__", [](Brain& self) {
            // Brain is always initialized when constructed
            // Check if it has been initialized by checking neuron count
            if (self.getTotalNeuronCount() == 0) {
                throw BrainError("Cannot enter context: Brain object is not properly initialized");
            }
            
            // Save initial state
            self.save("/tmp/brain_backup_" + std::to_string(std::time(nullptr)) + ".bin");
            return &self;
        })
        .def("__exit__", [](Brain& self, py::object exc_type, py::object exc_value, py::object exc_traceback) {
            if (exc_type) {
                // Error occurred, try to restore from backup
                try {
                    std::string backup_file = "/tmp/brain_backup_" + std::to_string(std::time(nullptr) - 1) + ".bin";
                    if (std::filesystem::exists(backup_file)) {
                        self.load(backup_file);
                        py::info("Restored brain state from backup due to error");
                    }
                } catch (...) {
                    // If restore fails, reset instead
                    self.reset();
                    py::warning("Failed to restore from backup, brain reset to initial state");
                }
            } else {
                // Normal exit, save final state
                self.save("/tmp/brain_final_" + std::to_string(std::time(nullptr)) + ".bin");
            }
        });

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
.def("enableRewardModulation", [](AgentBrain& self, bool enable) {
            if (enable && !self.getBrain()) {
                throw BrainError("Cannot enable reward modulation: Brain object is not properly initialized. Call agent.initialize(world) first.");
            }
            
            self.enableRewardModulation(enable);
        }, py::arg("enable"),
             "Enable or disable reward modulation. Args: enable (bool) - Whether to enable reward modulation. Throws: BrainError if brain is not initialized. Returns: None")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Pythonic convenience methods
        .def("run_agent_episode", [](AgentBrain& self, SimpleWorld& world, size_t max_steps) {
            self.initialize(world);
            for (size_t i = 0; i < max_steps; ++i) {
                // Get sensory percept from world
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Get action from brain
                auto action = self.decodeMotorCommand();
                
                // Apply action to world
                world.applyMotorCommand(action.getType(), world.getSimulationTime());
                
                // Update development
                self.updateDevelopment(1.0);
            }
            self.reset();
        }, py::arg("world"), py::arg("max_steps"),
             "Run a complete agent episode. Args: world (SimpleWorld) - The world to simulate in, max_steps (int) - Maximum number of steps per episode")
        .def("train_agent", [](AgentBrain& self, SimpleWorld& world, 
                      size_t episodes, size_t steps_per_episode, 
                      std::function<void(size_t, size_t)> progress_callback) {
            if (episodes == 0) {
                throw BrainError("Number of episodes must be positive");
            }
            
            if (steps_per_episode == 0) {
                throw BrainError("Steps per episode must be positive");
            }
            
            if (episodes > 1000) {
                py::warning("Training for many episodes (" + std::to_string(episodes) + ") may take a long time.");
            }
            
            self.initialize(world);
            
            for (size_t episode = 0; episode < episodes; ++episode) {
                // Run episode
                for (size_t step = 0; step < steps_per_episode; ++step) {
                    auto percept = world.getSensoryPercept();
                    self.processSensoryInput(percept);
                    auto action = self.decodeMotorCommand();
                    world.applyMotorCommand(action.getType(), world.getSimulationTime());
                    self.updateDevelopment(1.0);
                }
                
                // Call progress callback
                if (progress_callback) {
                    progress_callback(episode, episodes);
                }
                
                // Reset for next episode
                self.reset();
            }
        }, py::arg("world"), py::arg("episodes"), py::arg("steps_per_episode") = 100,
             py::arg("progress_callback") = nullptr,
             "Train agent for multiple episodes. Args: world (SimpleWorld) - World to train in, episodes (int) - Number of training episodes, steps_per_episode (int) - Steps per episode, progress_callback (callable) - Optional progress callback function. Throws: BrainError or WorldError if validation fails. Returns: None")
        .def("get_agent_stats", [](AgentBrain& self) {
            py::dict stats;
            auto brain = self.getBrain();
            
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            
            if (brain) {
                stats["total_neurons"] = brain->getTotalNeuronCount();
                stats["total_synapses"] = brain->getTotalSynapseCount();
                stats["active_neurons"] = brain->getActiveNeuronCount();
                stats["firing_neurons"] = brain->getFiringNeuronCount();
                stats["average_firing_rate"] = brain->getAverageFiringRate();
                stats["total_spikes"] = brain->getTotalSpikeCount();
                stats["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
            }
            
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            
            return stats;
        }, "Get comprehensive agent statistics as a Python dictionary. Returns: dict containing agent and brain statistics")
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            return &self;
        })
        .def("__exit__", [](AgentBrain& self, py::object, py::object, py::object) {
            self.reset();
        });

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    m.def("createConfigFromPreset", [](const std::string& name) -> std::shared_ptr<Config> {
        auto preset_config = std::make_shared<Config>();
        
        if (name == "default") {
            // Set default values
            preset_config->set("brain.neuronCount", 1000);
            preset_config->set("brain.regionCount", 10);
            preset_config->set("brain.learningRate", 0.01);
            preset_config->set("brain.inhibitionStrength", 1.0);
            preset_config->set("brain.excitationStrength", 1.0);
            preset_config->set("world.width", 100.0);
            preset_config->set("world.height", 100.0);
            preset_config->set("world.visionWidth", 5);
            preset_config->set("world.visionHeight", 5);
            preset_config->set("agent.maxEnergy", 100.0);
            preset_config->set("agent.energyDecayRate", 0.01);
        }
        else if (name == "high_plasticity") {
            preset_config->set("brain.neuronCount", 2000);
            preset_config->set("brain.regionCount", 20);
            preset_config->set("brain.learningRate", 0.05);
            preset_config->set("brain.inhibitionStrength", 0.5);
            preset_config->set("brain.excitationStrength", 1.5);
            preset_config->set("brain.stdpEnabled", true);
            preset_config->set("brain.hebbianEnabled", true);
            preset_config->set("world.width", 200.0);
            preset_config->set("world.height", 200.0);
            preset_config->set("world.visionWidth", 10);
            preset_config->set("world.visionHeight", 10);
        }
        else if (name == "agent_training") {
            preset_config->set("brain.neuronCount", 500);
            preset_config->set("brain.regionCount", 5);
            preset_config->set("brain.learningRate", 0.02);
            preset_config->set("brain.inhibitionStrength", 2.0);
            preset_config->set("brain.excitationStrength", 0.8);
            preset_config->set("agent.maxEnergy", 50.0);
            preset_config->set("agent.energyDecayRate", 0.05);
            preset_config->set("agent.rewardModulation", true);
            preset_config->set("agent.curiosityEnabled", true);
        }
        else {
            throw std::runtime_error("Unknown preset name: " + name);
        }
        
        return preset_config;
    }, py::arg("name"),
         "Create configuration from a named preset. Args: name (str) - Preset name ('default', 'high_plasticity', 'agent_training'). Returns: Config object");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
