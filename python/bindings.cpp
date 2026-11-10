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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides access to brain parameters, plasticity settings, neuromodulation,
        and system-wide configuration options. Keys follow a dot notation
        hierarchy (e.g., "brain.neuron_count", "plasticity.stdp.learning_rate").
        
        Common Categories:
        - brain: Basic brain parameters (neuron_count, synapse_density, etc.)
        - plasticity: Learning mechanisms (STDP, Hebbian, structural)
        - neuromod: Neuromodulation systems (dopamine, curiosity, novelty)
        - development: Developmental stages and rates
        - memory: Memory system parameters (working, episodic, associative)
        - prediction: Prediction system settings
        
        Usage:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 500)
            config.set("plasticity.stdp.learning_rate", 0.001)
            
        Value Types:
        - int: For neuron counts, step counts, region IDs, etc.
        - float: For weights, rates, probabilities, potentials
        - str: For names, file paths, string values
        - bool: For enable/disable flags
        )pbdoc")
        .def(py::init<>())
        
        // Enhanced configuration loading with error handling
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool success = self.loadFromFile(filepath);
            if (!success) {
                throw py::value_error("Failed to load configuration from file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), R"pbdoc(
            Load configuration from a JSON file.
            
            Args:
                filepath: Path to JSON configuration file
                
            Returns:
                True if configuration loaded successfully
                
            Raises:
                ValueError: If file cannot be read or parsed
        )pbdoc")
        
        // Enhanced configuration loading from arguments
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size() + 1);  // +1 for program name
            argv.push_back(const_cast<char*>("program"));  // dummy program name
            for (const auto& a : args) {
                argv.push_back(const_cast<char*>(a.c_str()));
            }
            
            bool success = self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
            if (!success) {
                throw py::value_error("Failed to load configuration from arguments");
            }
            return success;
        }, py::arg("args"), R"pbdoc(
            Load configuration from command line arguments.
            
            Args:
                args: List of command line arguments (e.g., ["--neuron_count", "500"])
                
            Returns:
                True if configuration loaded successfully
                
            Raises:
                ValueError: If arguments are invalid or cannot be parsed
        )pbdoc")
        
        // Enhanced configuration saving with error handling
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            bool success = self.saveToFile(filepath);
            if (!success) {
                throw py::value_error("Failed to save configuration to file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), R"pbdoc(
            Save configuration to a JSON file.
            
            Args:
                filepath: Path where configuration will be saved
                
            Returns:
                True if configuration saved successfully
                
            Raises:
                ValueError: If file cannot be written
        )pbdoc")
        
        // Enhanced type-safe getters
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.getGeneric(key);
            if (!value.has_value()) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return value;
        }, py::arg("key"), R"pbdoc(
            Get configuration value by key.
            
            Args:
                key: Configuration key in dot notation (e.g., "brain.neuron_count")
                
            Returns:
                Value of the configuration entry (type depends on key)
                
            Raises:
                KeyError: If key does not exist
        )pbdoc")
        
        // Type-safe getter with default value
        .def("getOr", [](Config& self, const std::string& key, py::object defaultValue) {
            auto value = self.getGeneric(key);
            if (value.has_value()) {
                return value.value();
            }
            // Convert Python default to appropriate C++ type
            if (defaultValue.is(py::int_())) {
                return defaultValue.cast<int64_t>();
            } else if (defaultValue.is(py::float_())) {
                return defaultValue.cast<float>();
            } else if (defaultValue.is(py::bool_())) {
                return defaultValue.cast<bool>();
            } else if (defaultValue.is(py::str())) {
                return defaultValue.cast<std::string>();
            } else {
                throw py::type_error("Unsupported default value type");
            }
        }, py::arg("key"), py::arg("defaultValue"), R"pbdoc(
            Get configuration value by key with default fallback.
            
            Args:
                key: Configuration key in dot notation
                defaultValue: Default value if key does not exist
                
            Returns:
                Value of the configuration entry, or default if not found
        )pbdoc")
        
        // Set with validation and error handling
        .def("set", [](Config& self, const std::string& key, py::object value) {
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>(), nlm::ConfigSource::Runtime);
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<float>(), nlm::ConfigSource::Runtime);
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>(), nlm::ConfigSource::Runtime);
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>(), nlm::ConfigSource::Runtime);
            } else {
                throw py::type_error("Unsupported value type. Use int, float, bool, or string.");
            }
        }, py::arg("key"), py::arg("value"), R"pbdoc(
            Set configuration value.
            
            Args:
                key: Configuration key in dot notation
                value: Value to set (int, float, bool, or string)
                
            Raises:
                TypeError: If value type is not supported
        )pbdoc")
        
        // Enhanced configuration checks
        .def("has", &Config::has, py::arg("key"), R"pbdoc(
            Check if a configuration key exists.
            
            Args:
                key: Configuration key in dot notation
                
            Returns:
                True if key exists in configuration
        )pbdoc")
        
        // Get all configuration keys
        .def("getKeys", &Config::getKeys, R"pbdoc(
            Get all configuration keys.
            
            Returns:
                List of all configuration keys
        )pbdoc")
        
        // Clear all configuration entries
        .def("clear", &Config::clear, R"pbdoc(
            Clear all configuration entries.
            Set configuration back to defaults.
        )pbdoc")
        
        // Enhanced configuration summary
        .def("summary", &Config::summary, R"pbdoc(
            Get a summary string of the configuration.
            
            Returns:
                Formatted string showing all configuration values
        )pbdoc")
        
        // String representation
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set default values for all common configuration keys
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, nlm::ConfigSource::Default);
        config->set("brain.v_thresh", -50.0f, nlm::ConfigSource::Default);
        config->set("brain.v_rest", -70.0f, nlm::ConfigSource::Default);
        config->set("brain.v_reset", -75.0f, nlm::ConfigSource::Default);
        config->set("brain.tau_mem", 20.0f, nlm::ConfigSource::Default);
        config->set("brain.tau_ref", 2.0f, nlm::ConfigSource::Default);
        
        // Default plasticity settings
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0f, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0f, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        
        // Default neuromodulation settings
        config->set("neuromod.dopamine.scale", 1.0f, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.threshold", 0.1f, nlm::ConfigSource::Default);
        
        // Default development settings
        config->set("development.initial_stage", "Initial", nlm::ConfigSource::Default);
        config->set("development.critical_period_duration", 100, nlm::ConfigSource::Default);
        
        // Default memory settings
        config->set("memory.working.capacity", 100, nlm::ConfigSource::Default);
        config->set("memory.episodic.max_episodes", 1000, nlm::ConfigSource::Default);
        
        // Default prediction settings
        config->set("prediction.system.enable", true, nlm::ConfigSource::Default);
        config->set("prediction.error.threshold", 0.1f, nlm::ConfigSource::Default);
        
        return config;
    }, "Create a default configuration with all common settings initialized");
    
    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Minimal configuration - just enough for basic operation
        config->set("brain.neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, nlm::ConfigSource::Default);
        return config;
    }, "Create a minimal configuration for basic testing");
    
    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Research-oriented configuration with emphasis on learning
        config->set("brain.neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.2f, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.002f, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("development.initial_stage", "CriticalPeriod", nlm::ConfigSource::Default);
        config->set("memory.working.capacity", 200, nlm::ConfigSource::Default);
        config->set("memory.episodic.max_episodes", 5000, nlm::ConfigSource::Default);
        return config;
    }, "Create a research configuration optimized for learning and exploration");
    
    m.def("createProductionConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Production configuration with stability and performance
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.05f, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.0005f, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", false, nlm::ConfigSource::Default);  // Less exploration
        config->set("neuromod.novelty.enable", false, nlm::ConfigSource::Default);   // Less novelty
        config->set("development.initial_stage", "Adult", nlm::ConfigSource::Default);
        config->set("memory.working.capacity", 500, nlm::ConfigSource::Default);
        config->set("memory.episodic.max_episodes", 10000, nlm::ConfigSource::Default);
        config->set("prediction.system.enable", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a production configuration optimized for stable operation");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw py::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        return world;
    }, "Create a simple world with default dimensions");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface for brain-world interaction");
    
    m.def("runDemoEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int numSteps) {
        auto agent = pynlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto motorCmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            float reward = 0.0f;
            if (percept.getInternal().size() > 0) {
                reward = percept.getInternal()[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), brain->getFiringNeuronCount(),
                               agent->getCuriosityLevel(), agent->getNoveltyLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("numSteps"), 
       "Run a complete demo episode with brain, agent, and world integration");
    
    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, int numSteps, 
                                     const std::vector<float>& pattern, float learningRate) {
        brain->initialize();
        
        // Inject pattern to stimulate learning
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            size_t neuronsToStimulate = std::min(static_cast<size_t>(20), neurons.size());
            
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                neurons[i]->injectCurrent(30.0f * learningRate);
            }
            
            // Run simulation with pattern
            for (int step = 0; step < numSteps; ++step) {
                for (size_t i = 0; i < neuronsToStimulate; ++i) {
                    neurons[i]->injectCurrent(30.0f * learningRate);
                }
                brain->step(step);
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("numSteps"), py::arg("pattern"), py::arg("learningRate"),
       "Run a learning experiment with pattern stimulation");
    
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int numSteps) {
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = numSteps / (duration.count() / 1000.0);
        
        return std::make_tuple(brain->getTotalSpikeCount(), brain->getFiringNeuronCount(),
                               brain->getAverageFiringRate(), stepsPerSecond);
    }, py::arg("brain"), py::arg("numSteps"), "Benchmark brain performance");
    
    m.def("printWelcomeMessage", []() {
        std::cout << R"(
==================================================
 NLM (Neural Learning Machine) - Python Bindings
==================================================
 Welcome to NLM's Python interface!
 
 This provides access to:
 - Complete brain simulation with spiking neurons
 - Integrated memory, prediction, and cognition systems
 - Neuromodulation for motivation and learning
 - Agent-based interaction with simulated worlds
 - Development and plasticity mechanisms
 
 Quick Start Examples:
 =====================
 1. Create a brain: config = pynlm.createDefaultConfig()
 2. Initialize: brain = pynlm.createBrain(config)
 3. Run: brain.step(step_number)
 4. Save: brain.save("checkpoint.bin")
 
 For more examples, see:
 - HOW_TO_USE.md (in project root)
 - easy_usage.md (in project root)
 - Python documentation in bindings.cpp

Phase 6 (Final Integration) Features:
==================================
 - Working memory with persistent activity
 - Episodic memory with experience replay
 - Neuromodulation (dopamine, curiosity, novelty)
 - Prediction systems and error signals
 - Neural planning and concept formation
 - Developmental stages affecting plasticity
 - Checkpoint save/load functionality
 - Memory consolidation and replay
==================================================

For assistance, visit:
 - https://github.com/nlm-project/nlm
 - https://nlm.readthedocs.io
==================================================
" << std::endl;
    }, "Display welcome message with quick start guide");
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add version attribute
    m.attr("__version__") = "6.0.0-FinalIntegration";
    m.attr("__description__") = "NLM (Neural Learning Machine) - Complete Artificial Brain System";
    m.attr("__phase__") = "Phase 6: Final Integration";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";
}
