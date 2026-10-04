#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace py = pybind11;

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class SimpleWorld;
    class AgentBrain;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    
    // Enums
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
    
    // ID types
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    
    // Types
    using ConfigValue = std::variant<int, int64_t, double, bool, std::string,
                                   std::vector<int>, std::vector<double>, std::vector<std::string>>;
    
    enum class ConfigSource { Default, File, CommandLine, Runtime };
    
    using SimulationStep = uint64_t;
    using Timestamp = uint64_t;
    
    // Invalid IDs (global constants)
    extern const NeuronId INVALID_NEURON_ID;
    extern const SynapseId INVALID_SYNAPSE_ID;
    extern const RegionId INVALID_REGION_ID;
    extern const PopulationId INVALID_POPULATION_ID;
}

// Exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("Config error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain error: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("World error: " + msg) {}
};

// Template function to create repetitive ID classes with less boilerplate
template<typename IdType, typename ValueType>
void bindIdClass(py::module_& m, const std::string& name,
                 const std::string& docString = "") {
    py::class_<IdType>(m, name.c_str(), docString.c_str())
        .def(py::init<>())
        .def(py::init<ValueType>(), py::arg("value"))
        .def_readwrite("value", &IdType::value)
        .def("index", &IdType::index)
        .def("__eq__", &IdType::operator==)
        .def("__ne__", &IdType::operator!=)
        .def("__hash__", [](const IdType& id) { return std::hash<ValueType>{}(id.value); })
        .def("__repr__", [](const IdType& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

// Helper function to create config from common parameters
nlm::Config createDefaultConfig(int neuronCount = 100, int regionCount = 1,
                                float connectionProbability = 0.1f,
                                float visionWidth = 32.0f, float visionHeight = 32.0f);

// Helper function to create a simple world with agent
std::shared_ptr<nlm::SimpleWorld> createWorldWithAgent(
    float width = 100.0f, float height = 100.0f,
    float visionWidth = 32.0f, float visionHeight = 32.0f);

// Helper function to run a simple episode
void runSimpleEpisode(
    std::shared_ptr<nlm::Brain> brain,
    std::shared_ptr<nlm::SimpleWorld> world,
    int maxSteps = 1000,
    std::function<bool()> shouldStop = nullptr,
    std::function<void(int)> onStep = nullptr);

// Helper function to visualize brain state
void visualizeBrain(const nlm::Brain& brain, const std::string& outputFile = "");

// Helper function to collect brain metrics
py::dict collectBrainMetrics(const nlm::Brain& brain);

// Context manager for automatic brain cleanup
class BrainContextManager {
private:
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::SimpleWorld> world;
public:
    BrainContextManager(std::shared_ptr<nlm::Brain> b, std::shared_ptr<nlm::SimpleWorld> w = nullptr)
        : brain(b), world(w) {}
    
    ~BrainContextManager() {
        // Cleanup will be handled by shared_ptr
    }
    
    std::shared_ptr<nlm::Brain> getBrain() const { return brain; }
    std::shared_ptr<nlm::SimpleWorld> getWorld() const { return world; }
    
    // Python context manager protocol
    void __enter__() { }
    void __exit__(py::args) { }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This enhanced binding provides:
        - High-level convenience functions for common tasks
        - Better error handling with Python exceptions
        - Context managers for resource management
        - Visualization and metrics collection helpers
        - Integration with Python logging and debugging
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Bind ID classes using template function (reduced boilerplate)
    bindIdClass<nlm::NeuronId, uint64_t>(m, "NeuronId", "Unique identifier for a neuron");
    bindIdClass<nlm::SynapseId, uint64_t>(m, "SynapseId", "Unique identifier for a synapse");
    bindIdClass<nlm::RegionId, uint64_t>(m, "RegionId", "Unique identifier for a brain region");
    bindIdClass<nlm::PopulationId, uint64_t>(m, "PopulationId", "Unique identifier for a neuron population");

    // Bind enums
    py::enum_<nlm::NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    py::enum_<nlm::SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    py::enum_<nlm::FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    py::enum_<nlm::ActionType>(m, "ActionType", "Action type enumeration")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    py::enum_<nlm::MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // ============= CONVENIENCE FUNCTIONS =============
    
    // Create default configuration with common parameters
    m.def("create_default_config", &createDefaultConfig,
          py::arg("neuron_count") = 100,
          py::arg("region_count") = 1,
          py::arg("connection_probability") = 0.1f,
          py::arg("vision_width") = 32.0f,
          py::arg("vision_height") = 32.0f,
          "Create a default configuration with common parameters for simulation",
          py::return_value_policy::take_ownership);

    // Create a simple world with agent pre-configured
    m.def("create_world_with_agent", &createWorldWithAgent,
          py::arg("width") = 100.0f,
          py::arg("height") = 100.0f,
          py::arg("vision_width") = 32.0f,
          py::arg("vision_height") = 32.0f,
          "Create a simple world with agent pre-configured for simulation",
          py::return_value_policy::take_ownership);

    // Run a simple episode with automatic error handling
    m.def("run_simple_episode", &runSimpleEpisode,
          py::arg("brain"),
          py::arg("world"),
          py::arg("max_steps") = 1000,
          py::arg("should_stop") = nullptr,
          py::arg("on_step") = nullptr,
          "Run a simple episode with the given brain and world",
          py::call_guard<py::call_guard<py::gil_scoped_release>>());

    // ============= CONTEXT MANAGER =============
    
    // Create a context manager for brain simulation
    py::class_<BrainContextManager>(m, "BrainSimulation", "Context manager for brain simulation")
        .def(py::init<std::shared_ptr<nlm::Brain>, std::shared_ptr<nlm::SimpleWorld>>(),
             py::arg("brain"), py::arg("world") = nullptr)
        .def("__enter__", [](BrainContextManager& self) {
            return self;
        })
        .def("__exit__", [](BrainContextManager& self, py::args) {
            // Cleanup handled by shared_ptr
        })
        .def("get_brain", &BrainContextManager::getBrain,
             "Get the brain from the context manager")
        .def("get_world", &BrainContextManager::getWorld,
             "Get the world from the context manager");

    // ============= METRICS AND VISUALIZATION HELPERS =============
    
    // Collect brain metrics as a Python dictionary
    m.def("collect_brain_metrics", &collectBrainMetrics,
          py::arg("brain"),
          "Collect brain metrics and return as a Python dictionary",
          py::return_value_policy::take_ownership);

    // ============= LOGGING HELPERS =============
    
    // Set up Python logging integration
    m.def("setup_python_logging", []() {
        py::module_ logging = py::module_::import("logging");
        logging.attr("basicConfig")(logging.attr("INFO"),
                                   py::arg("format") = "%(asctime)s - %(name)s - %(levelname)s - %(message)s");
        std::cout << "Python logging configured" << std::endl;
    }, "Setup Python logging integration");

    // ============= CONFIGURATION HELPERS =============
    
    // Validate configuration
    m.def("validate_config", [](const nlm::Config& config) {
        std::vector<std::string> errors;
        
        if (!config.has("neuron_count")) {
            errors.push_back("Missing 'neuron_count' parameter");
        }
        
        if (!config.has("region_count")) {
            errors.push_back("Missing 'region_count' parameter");
        }
        
        if (errors.empty()) {
            return py::dict();
        }
        
        throw ConfigError("Configuration validation failed: " + 
                         (errors.size() > 1 ? "Multiple issues" : errors[0]));
    }, "Validate configuration and return errors as a list", py::return_value_policy::take_ownership);

    // Get configuration as Python dictionary
    m.def("config_to_dict", [](const nlm::Config& config) {
        py::dict result;
        auto keys = config.getKeys();
        
        for (const auto& key : keys) {
            try {
                // Try to get as different types
                if (config.has(key)) {
                    if (auto value = config.get<int>(key)) {
                        result[key.c_str()] = *value;
                    } else if (auto value = config.get<double>(key)) {
                        result[key.c_str()] = *value;
                    } else if (auto value = config.get<bool>(key)) {
                        result[key.c_str()] = *value;
                    } else if (auto value = config.get<std::string>(key)) {
                        result[key.c_str()] = *value;
                    } else if (auto value = config.get<std::vector<int>>(key)) {
                        result[key.c_str()] = *value;
                    } else if (auto value = config.get<std::vector<double>>(key)) {
                        result[key.c_str()] = *value;
                    } else if (auto value = config.get<std::vector<std::string>>(key)) {
                        result[key.c_str()] = *value;
                    }
                }
            } catch (const std::exception& e) {
                // Skip invalid entries
                continue;
            }
        }
        
        return result;
    }, "Get configuration as a Python dictionary", py::return_value_policy::take_ownership);

    // ============= ADVANCED BRAIN OPERATIONS =============
    
    // Get brain connectivity matrix as Python 2D list
    m.def("get_connectivity_matrix", [](const nlm::Brain& brain) {
        auto regions = brain.getRegions();
        std::vector<std::vector<float>> matrix;
        
        for (const auto& region : regions) {
            std::vector<float> row;
            auto synapses = region->getSynapses();
            
            for (const auto& synapse : synapses) {
                row.push_back(synapse->getWeight());
            }
            
            matrix.push_back(row);
        }
        
        return matrix;
    }, "Get brain connectivity matrix as a Python 2D list", py::return_value_policy::take_ownership);

    // Get brain statistics summary
    m.def("get_brain_summary", [](const nlm::Brain& brain) {
        py::dict summary;
        
        summary["total_neurons"] = brain.getTotalNeuronCount();
        summary["total_synapses"] = brain.getTotalSynapseCount();
        summary["active_neurons"] = brain.getActiveNeuronCount();
        summary["firing_neurons"] = brain.getFiringNeuronCount();
        summary["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        summary["average_firing_rate"] = brain.getAverageFiringRate();
        summary["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        return summary;
    }, "Get brain statistics summary as a Python dictionary", py::return_value_policy::take_ownership);

    // ============= HIGH-LEVEL BRAIN OPERATIONS =============
    
    // Feed sensory input with Pythonic interface
    m.def("feed_sensory_input", [](nlm::Brain& brain, const py::dict& inputData) {
        // This would need a more sophisticated implementation to convert Python dict to SensoryInput
        // For now, this is a placeholder for the concept
        throw std::runtime_error("feed_sensory_input with dict not yet implemented");
    }, "Feed sensory input to brain from Python dictionary", py::call_guard<py::gil_scoped_release>());

    // ============= GLOBAL CONSTANTS =============
    
    // Expose global constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // ============= ORIGINAL BINDINGS (FOR BACKWARD COMPATIBILITY) =============
    
    // Include original bindings for backward compatibility
    // (In practice, these would be the original bindings code)

    // For demonstration, include a few key original bindings:

    py::class_<nlm::Config>(m, "Config", "Configuration class for NLM system")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<nlm::SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &nlm::SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &nlm::SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &nlm::SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &nlm::SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<nlm::Vision, nlm::SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](nlm::Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getChannels", &nlm::Vision::getChannels);

    py::class_<nlm::Audio, nlm::SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](nlm::Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &nlm::Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &nlm::Audio::getSampleRate)
        .def("getNumSamples", &nlm::Audio::getNumSamples);

    py::class_<nlm::InternalSignals, nlm::SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &nlm::InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &nlm::InternalSignals::clearSignals);

    py::class_<nlm::Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &nlm::Action::getType)
        .def("setType", &nlm::Action::setType, py::arg("type"))
        .def("getParameters", &nlm::Action::getParameters)
        .def("setParameters", &nlm::Action::setParameters, py::arg("params"))
        .def("getName", &nlm::Action::getName)
        .def("clone", &nlm::Action::clone);

    py::class_<nlm::WorldObject>(m, "WorldObject", "World object representation")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active);

    py::class_<nlm::AgentBody>(m, "AgentBody", "Agent body state")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX)
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY)
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("isMoving", &nlm::AgentBody::isMoving)
        .def_readwrite("isTurning", &nlm::AgentBody::isTurning)
        .def_readwrite("lastActionTime", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset);

    py::class_<nlm::ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message);

    py::class_<nlm::SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &nlm::SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &nlm::SensoryPercept::getVisionHeight)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &nlm::SensoryPercept::getAllSignals)
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp)
        .def("setTimestamp", &nlm::SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<nlm::SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed);

    py::class_<nlm::Brain>(m, "Brain", "Central neural simulation brain class")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &nlm::Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status");

    py::class_<nlm::AgentBrain>(m, "AgentBrain", "Agent brain interface connecting NLM brain to world")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled);

    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
}

// Implementation of helper functions

nlm::Config createDefaultConfig(int neuronCount, int regionCount,
                                float connectionProbability,
                                float visionWidth, float visionHeight) {
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(neuronCount));
    config.set("region_count", static_cast<int64_t>(regionCount));
    config.set("connection_probability", connectionProbability);
    config.set("vision_width", visionWidth);
    config.set("vision_height", visionHeight);
    return config;
}

std::shared_ptr<nlm::SimpleWorld> createWorldWithAgent(
    float width, float height,
    float visionWidth, float visionHeight) {
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(width, height, visionWidth, visionHeight);
    world->setAgentStart(width * 0.5f, height * 0.5f);
    
    return world;
}

void runSimpleEpisode(
    std::shared_ptr<nlm::Brain> brain,
    std::shared_ptr<nlm::SimpleWorld> world,
    int maxSteps,
    std::function<bool()> shouldStop,
    std::function<void(int)> onStep) {
    
    if (!brain || !world) {
        throw BrainError("Brain or world is null");
    }
    
    bool success = brain->initialize();
    if (!success) {
        throw BrainError("Failed to initialize brain");
    }
    
    for (int step = 0; step < maxSteps; ++step) {
        try {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            brain->receiveSensoryInput(percept);
            
            // Update brain
            brain->step(step);
            
            // Process motor output
            auto action = brain->produceAction();
            if (action) {
                world->applyMotorCommand(*action, step);
            }
            
            // Call step callback if provided
            if (onStep) {
                onStep(step);
            }
            
            // Check if we should stop
            if (shouldStop && shouldStop()) {
                break;
            }
            
        } catch (const std::exception& e) {
            throw BrainError(std::string("Error at step ") + std::to_string(step) + ": " + e.what());
        }
    }
}

void visualizeBrain(const nlm::Brain& brain, const std::string& outputFile) {
    auto metrics = collectBrainMetrics(brain);
    
    std::cout << "Brain Visualization:" << std::endl;
    std::cout << "===================" << std::endl;
    for (const auto& item : metrics) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    
    if (!outputFile.empty()) {
        // TODO: Implement actual visualization
        std::cout << "Visualization would be saved to: " << outputFile << std::endl;
    }
}

py::dict collectBrainMetrics(const nlm::Brain& brain) {
    py::dict metrics;
    
    metrics["total_neurons"] = brain.getTotalNeuronCount();
    metrics["total_synapses"] = brain.getTotalSynapseCount();
    metrics["active_neurons"] = brain.getActiveNeuronCount();
    metrics["firing_neurons"] = brain.getFiringNeuronCount();
    metrics["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
    metrics["average_firing_rate"] = brain.getAverageFiringRate();
    metrics["total_spike_count"] = brain.getTotalSpikeCount();
    metrics["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
    
    return metrics;
}

// Implement missing function that was referenced
void NotImplementedError(const std::string& msg) {
    throw std::runtime_error("Not implemented: " + msg);
}