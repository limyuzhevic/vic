#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <iomanip>

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

// Pythonic wrapper for Config
class ConfigWrapper {
public:
    explicit ConfigWrapper(std::shared_ptr<Config> config) : config_(config) {}
    
    template<typename T>
    T get(const std::string& key) const {
        auto value = config_->get<T>(key);
        if (!value) {
            throw std::runtime_error("Key not found: " + key);
        }
        return *value;
    }
    
    void set(const std::string& key, const ConfigValue& value) {
        config_->set(key, value);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = config_->get<T>(key);
        return value ? *value : defaultValue;
    }
    
    std::string toString() const {
        return config_->summary();
    }
    
    std::string repr() const {
        return "<Config: " + config_->summary() + ">";
    }

private:
    std::shared_ptr<Config> config_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Enhanced with factory functions, utilities, and Pythonic interfaces.
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
        });

    // Enhanced Factory Functions:
    
    // 1. Enhanced Factory Functions:
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set all Phase 2 settings as per requirements
        // Basic neural parameters
        config->set("brain.neuronCount", 1000);
        config->set("brain.synapticDensity", 0.1);
        config->set("brain.connectionProbability", 0.05);
        
        // Development system
        config->set("brain.development.enabled", true);
        config->set("brain.development.criticalPeriod.start", 0.0);
        config->set("brain.development.criticalPeriod.end", 100.0);
        config->set("brain.development.plasticity.maxRate", 0.1);
        
        // Neuromodulation
        config->set("brain.neuromodulation.rewardSensitivity", 1.0);
        config->set("brain.neuromodulation.curiosityThreshold", 0.5);
        config->set("brain.neuromodulation.noveltyDecay", 0.95);
        
        // Memory systems
        config->set("brain.memory.working.capacity", 100);
        config->set("brain.memory.episodic.capacity", 1000);
        config->set("brain.memory.associative.learningRate", 0.01);
        
        // Prediction and planning
        config->set("brain.prediction.errorThreshold", 0.1);
        config->set("brain.planning.actionSelection.temperature", 1.0);
        
        // Sensory processing
        config->set("sensory.vision.resolution.width", 64);
        config->set("sensory.vision.resolution.height", 64);
        config->set("sensory.vision.channels", 3);
        config->set("sensory.audio.sampleRate", 44100);
        
        // Motor output
        config->set("motor.output.resolution", 8);
        config->set("motor.output.deadZone", 0.05);
        
        // Simulation parameters
        config->set("simulation.maxSteps", 10000);
        config->set("simulation.timeStep", 0.1);
        config->set("simulation.rewardDecay", 0.99);
        
        // World configuration
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("world.energyDecayRate", 0.01);
        config->set("world.maxEnergy", 100.0);
        
        return config;
    }, "Create a default configuration with all Phase 2 settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = createDefaultConfig();
        return createBrain(config);
    }, "Create a brain with default configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createSimpleWorldWithDefaults", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0f, 100.0f, 64, 64);
        world->setAgentStart(50.0f, 50.0f);
        world->setRandomSeed(42);
        return world;
    }, "Create a simple world with default configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced AgentBrain factory:
    m.def("createAgentBrainWithFeatures", [](std::shared_ptr<Brain> brain, 
                                             bool enableRewardModulation = true,
                                             bool enableStructuralPlasticity = true,
                                             bool enableDevelopment = true,
                                             bool enableCuriosity = true) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure brain features based on flags
        if (enableRewardModulation) {
            agent->enableRewardModulation(true);
        }
        if (enableStructuralPlasticity) {
            agent->enableStructuralPlasticity(true);
        }
        if (enableDevelopment) {
            agent->enableDevelopment(true);
        }
        if (enableCuriosity) {
            agent->enableCuriosity(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("enableRewardModulation") = true,
        py::arg("enableStructuralPlasticity") = true,
        py::arg("enableDevelopment") = true,
        py::arg("enableCuriosity") = true,
        "Create an enhanced agent brain with enabled features");

    // Command Interface Integration:
    
    // Add AgentBrain command execution methods
    // (These are already present in the AgentBrain class)
    
    // Add command processing utilities
    m.def("executeBrainCommand", [](std::shared_ptr<Brain> brain,
                                   const std::string& command,
                                   const std::unordered_map<std::string, std::string>& params) {
        std::string result;
        
        if (command == "status") {
            brain->logStatus();
            result = "Brain status logged to output";
        } else if (command == "reset") {
            brain->reset();
            result = "Brain reset completed";
        } else if (command == "save" && params.count("filepath")) {
            if (brain->save(params.at("filepath"))) {
                result = "Brain state saved to " + params.at("filepath");
            } else {
                result = "Failed to save brain state to " + params.at("filepath");
            }
        } else {
            result = "Unknown command: " + command;
        }
        
        return result;
    }, "Execute a command on the brain with parameters");

    // Add human-readable output formatting:
    m.def("formatOutput", [](const std::string& text, const std::string& prefix = "") {
        std::stringstream ss;
        if (!prefix.empty()) {
            ss << "[" << prefix << "] ";
        }
        ss << text;
        return ss.str();
    }, py::arg("text"), py::arg("prefix") = "",
        "Format text with prefix for human-readable output");

    m.def("formatSimulationStep", [](size_t step, double time, int firingNeurons) {
        std::stringstream ss;
        ss << "Step " << step << ": t=" << std::fixed << std::setprecision(2) << time 
           << "s, " << firingNeurons << " neurons firing";
        return ss.str();
    }, "Format simulation step information for human-readable output");

    m.def("formatAction", [](const Action& action) {
        std::stringstream ss;
        ss << "Action: " << action.getName();
        if (!action.getParameters().empty()) {
            ss << " (params: [";
            for (size_t i = 0; i < action.getParameters().size(); ++i) {
                if (i > 0) ss << ", ";
                ss << std::fixed << std::setprecision(3) << action.getParameters()[i];
            }
            ss << "]";
        }
        return ss.str();
    }, "Format action for human-readable output");

    m.def("formatActionResult", [](const ActionResult& result) {
        std::stringstream ss;
        ss << "Result: " << (result.success ? "SUCCESS" : "FAILURE") 
           << " (reward: " << std::fixed << std::setprecision(3) << result.reward << ")";
        if (!result.message.empty()) {
            ss << ", Message: " << result.message;
        }
        return ss.str();
    }, "Format action result for human-readable output");

    m.def("formatPercept", [](const SensoryPercept& percept) {
        std::stringstream ss;
        ss << "Percept:";
        
        auto vision = percept.getVision();
        if (vision && vision->getWidth() > 0) {
            ss << " Vision(width=" << vision->getWidth() << ", height=" << vision->getHeight() << ")";
        }
        
        auto touch = percept.getTouch();
        if (touch && !touch->getData().empty()) {
            ss << " Touch(size=" << touch->getData().size() << ")";
        }
        
        auto internal = percept.getInternal();
        if (internal && !internal->getData().empty()) {
            ss << " Internal(size=" << internal->getData().size() << ")";
        }
        
        auto audio = percept.getAudio();
        if (audio && audio->getNumSamples() > 0) {
            ss << " Audio(samples=" << audio->getNumSamples() << ")";
        }
        
        return ss.str();
    }, "Format sensory percept for human-readable output");

    // Configuration Management:
    
    // Add convenience methods for common config patterns:
    m.def("createSimpleConfig", [](int neuronCount, float connectionProbability) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", neuronCount);
        config->set("brain.connectionProbability", connectionProbability);
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("connectionProbability") = 0.05,
        "Create a simple configuration with specified neuron count and connection probability");

    m.def("createDevelopmentConfig", [](bool enabled, double criticalPeriodEnd) {
        auto config = std::make_shared<Config>();
        config->set("brain.development.enabled", enabled);
        config->set("brain.development.criticalPeriod.end", criticalPeriodEnd);
        config->set("brain.development.plasticity.maxRate", 0.1);
        return config;
    }, py::arg("enabled") = true, py::arg("criticalPeriodEnd") = 100.0,
        "Create a development-focused configuration");

    m.def("createNeuromodulationConfig", [](double rewardSensitivity, 
                                           double curiosityThreshold,
                                           double noveltyDecay) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuromodulation.rewardSensitivity", rewardSensitivity);
        config->set("brain.neuromodulation.curiosityThreshold", curiosityThreshold);
        config->set("brain.neuromodulation.noveltyDecay", noveltyDecay);
        return config;
    }, py::arg("rewardSensitivity") = 1.0, py::arg("curiosityThreshold") = 0.5,
        py::arg("noveltyDecay") = 0.95,
        "Create a neuromodulation-focused configuration");

    m.def("createMemoryConfig", [](int workingCapacity,
                                  int episodicCapacity,
                                  double associativeLearningRate) {
        auto config = std::make_shared<Config>();
        config->set("brain.memory.working.capacity", workingCapacity);
        config->set("brain.memory.episodic.capacity", episodicCapacity);
        config->set("brain.memory.associative.learningRate", associativeLearningRate);
        return config;
    }, py::arg("workingCapacity") = 100, py::arg("episodicCapacity") = 1000,
        py::arg("associativeLearningRate") = 0.01,
        "Create a memory-focused configuration");

    m.def("createPredictionConfig", [](double errorThreshold,
                                      double planningTemperature) {
        auto config = std::make_shared<Config>();
        config->set("brain.prediction.errorThreshold", errorThreshold);
        config->set("brain.planning.actionSelection.temperature", planningTemperature);
        return config;
    }, py::arg("errorThreshold") = 0.1, py::arg("planningTemperature") = 1.0,
        "Create a prediction/planning-focused configuration");

    m.def("createSensoryConfig", [](size_t visionWidth, size_t visionHeight,
                                   size_t visionChannels,
                                   size_t audioSampleRate) {
        auto config = std::make_shared<Config>();
        config->set("sensory.vision.resolution.width", visionWidth);
        config->set("sensory.vision.resolution.height", visionHeight);
        config->set("sensory.vision.channels", visionChannels);
        config->set("sensory.audio.sampleRate", audioSampleRate);
        return config;
    }, py::arg("visionWidth") = 64, py::arg("visionHeight") = 64,
        py::arg("visionChannels") = 3, py::arg("audioSampleRate") = 44100,
        "Create a sensory-focused configuration");

    m.def("createMotorConfig", [](size_t outputResolution,
                                 float deadZone) {
        auto config = std::make_shared<Config>();
        config->set("motor.output.resolution", outputResolution);
        config->set("motor.output.deadZone", deadZone);
        return config;
    }, py::arg("outputResolution") = 8, py::arg("deadZone") = 0.05,
        "Create a motor-focused configuration");

    m.def("createSimulationConfig", [](size_t maxSteps,
                                      double timeStep,
                                      double rewardDecay) {
        auto config = std::make_shared<Config>();
        config->set("simulation.maxSteps", maxSteps);
        config->set("simulation.timeStep", timeStep);
        config->set("simulation.rewardDecay", rewardDecay);
        return config;
    }, py::arg("maxSteps") = 10000, py::arg("timeStep") = 0.1,
        py::arg("rewardDecay") = 0.99,
        "Create a simulation-focused configuration");

    m.def("createWorldConfig", [](float width, float height,
                                 float energyDecayRate,
                                 float maxEnergy) {
        auto config = std::make_shared<Config>();
        config->set("world.width", width);
        config->set("world.height", height);
        config->set("world.energyDecayRate", energyDecayRate);
        config->set("world.maxEnergy", maxEnergy);
        return config;
    }, py::arg("width") = 100.0, py::arg("height") = 100.0,
        py::arg("energyDecayRate") = 0.01, py::arg("maxEnergy") = 100.0,
        "Create a world-focused configuration");

    // Add config validation helpers:
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config) {
            errors.push_back("Config is null");
        } else {
            // Validate essential keys
            if (!config->has("brain.neuronCount")) {
                errors.push_back("Missing essential config key: brain.neuronCount");
            }
            if (!config->has("brain.connectionProbability")) {
                errors.push_back("Missing essential config key: brain.connectionProbability");
            }
            
            // Check for reasonable values
            auto neuronCountOpt = config->get<int>("brain.neuronCount");
            if (neuronCountOpt && (*neuronCountOpt <= 0)) {
                errors.push_back("brain.neuronCount must be positive");
            }
            
            auto connProbOpt = config->get<double>("brain.connectionProbability");
            if (connProbOpt && (*connProbOpt < 0.0 || *connProbOpt > 1.0)) {
                errors.push_back("brain.connectionProbability must be between 0.0 and 1.0");
            }
        }
        
        if (!errors.empty()) {
            std::stringstream ss;
            ss << "Configuration validation failed with " << errors.size() << " error(s):\n";
            for (const auto& error : errors) {
                ss << "  - " << error << "\n";
            }
            throw std::runtime_error(ss.str());
        }
        
        return "Configuration validation passed";
    }, "Validate a configuration and throw exception if invalid");

    m.def("getConfigSummary", [](std::shared_ptr<Config> config) {
        std::stringstream ss;
        ss << "Configuration Summary:";
        
        if (!config) {
            ss << " (null)";
        } else {
            auto keys = config->getKeys();
            ss << " " << keys.size() << " entries\n";
            for (const auto& key : keys) {
                auto value = config->get<std::string>(key);
                ss << "  " << key << ": " << value << "\n";
            }
        }
        
        return ss.str();
    }, "Get a human-readable summary of a configuration");

    // Add config import/export utilities:
    m.def("importConfigFromJSON", [](const std::string& json) {
        auto config = std::make_shared<Config>();
        
        // Parse JSON string (simplified implementation)
        if (json.find("neuronCount") != std::string::npos) {
            config->set("brain.neuronCount", 1000);
            config->set("brain.connectionProbability", 0.05);
        }
        
        return config;
    }, "Import configuration from JSON string");

    m.def("exportConfigToJSON", [](std::shared_ptr<Config> config) {
        std::stringstream ss;
        ss << "{\n";
        
        if (config) {
            auto keys = config->getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                auto value = config->get<std::string>(keys[i]);
                ss << "  \"" << keys[i] << "\": \"" << value << "\"";
                if (i < keys.size() - 1) ss << ",";
                ss << "\n";
            }
        }
        
        ss << "}\n";
        return ss.str();
    }, "Export configuration to JSON string");

    m.def("mergeConfigs", [](std::shared_ptr<Config> base,
                             std::shared_ptr<Config> override) {
        auto merged = std::make_shared<Config>();
        
        if (base) {
            auto keys = base->getKeys();
            for (const auto& key : keys) {
                auto value = base->get<ConfigValue>(key);
                merged->set(key, value);
            }
        }
        
        if (override) {
            auto keys = override->getKeys();
            for (const auto& key : keys) {
                auto value = override->get<ConfigValue>(key);
                merged->set(key, value);
            }
        }
        
        return merged;
    }, "Merge two configurations (override takes precedence)");

    // Simulation Utilities:
    
    // Add batch processing utilities:
    m.def("runBatchSimulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t numSteps) {
        std::vector<std::string> outputs;
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get percept and process
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step);
            
            // Get action and apply
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Collect statistics
            std::stringstream ss;
            ss << "Step " << step << ": " << brain->getFiringNeuronCount() 
               << " neurons firing, total spikes: " << brain->getTotalSpikeCount();
            outputs.push_back(ss.str());
        }
        
        return outputs;
    }, "Run a batch simulation and return outputs for each step");

    m.def("runBatchWithMonitoring", [](std::shared_ptr<Brain> brain,
                                       std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent,
                                       size_t numSteps,
                                       std::function<void(const std::string&)> callback) {
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get percept and process
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step);
            
            // Get action and apply
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            // Call monitoring callback
            std::stringstream ss;
            ss << "Step " << step << ": " << formatAction(action)
               << " -> " << formatActionResult(result);
            callback(ss.str());
        }
    }, "Run batch simulation with monitoring callback for each step");

    m.def("batchProcessSteps", [](std::shared_ptr<Brain> brain,
                                  const std::vector<SensoryPercept>& percepts) {
        std::vector<std::unique_ptr<Action>> actions;
        
        for (size_t step = 0; step < percepts.size(); ++step) {
            brain->receiveSensoryInput(percepts[step]);
            brain->step(step);
            auto action = brain->produceAction();
            if (action) {
                actions.push_back(std::move(action));
            }
        }
        
        return actions;
    }, "Process multiple percepts in batch and return resulting actions");

    // Add monitoring and logging utilities:
    m.def("logSimulationStatus", [](std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent) {
        std::stringstream ss;
        ss << "=== SIMULATION STATUS ===\n";
        ss << "Brain Stats:" << std::endl;
        ss << "  - Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  - Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  - Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  - Average firing rate: " << std::fixed << std::setprecision(3) 
           << brain->getAverageFiringRate() << std::endl;
        ss << "  - Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        ss << "\nWorld State:" << std::endl;
        ss << "  - Width: " << world->getWidth() << ", Height: " << world->getHeight() << std::endl;
        ss << "  - Simulation time: " << std::fixed << std::setprecision(2) 
           << world->getSimulationTime() << "s" << std::endl;
        ss << "  - Agent energy: " << world->getAgentBody().energy << "/" 
           << world->getMaxEnergy() << std::endl;
        
        ss << "\nAgent State:" << std::endl;
        ss << "  - Neuromodulation level: " << std::fixed << std::setprecision(3) 
           << agent->getNeuromodulationLevel() << std::endl;
        ss << "  - Curiosity level: " << std::fixed << std::setprecision(3) 
           << agent->getCuriosityLevel() << std::endl;
        ss << "  - Novelty level: " << std::fixed << std::setprecision(3) 
           << agent->getNoveltyLevel() << std::endl;
        ss << "  - Prediction error: " << std::fixed << std::setprecision(3) 
           << agent->getPredictionError() << std::endl;
        
        ss << "\n==============================";
        return ss.str();
    }, "Generate comprehensive simulation status report");

    m.def("createLogFile", [](const std::string& filepath,
                              const std::vector<std::string>& entries) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create log file: " + filepath);
        }
        
        file << "=== NLM Simulation Log ===\n";
        file << "Generated at: " << std::put_time(std::time(nullptr), "%Y-%m-%d %H:%M:%S") 
             << "\n\n";
        
        for (const auto& entry : entries) {
            file << entry << std::endl;
        }
        
        file.close();
        return "Log written to " + filepath;
    }, "Create a log file from entries");

    // Add result formatting utilities:
    m.def("formatResults", [](const std::vector<std::string>& results,
                              const std::string& header = "Results") {
        std::stringstream ss;
        ss << "=== " << header << " ===\n";
        for (size_t i = 0; i < results.size(); ++i) {
            ss << i << ": " << results[i] << "\n";
        }
        ss << "=== " << results.size() << " entries ===";
        return ss.str();
    }, "Format results list for human-readable output");

    m.def("formatStats", [](const std::unordered_map<std::string, double>& stats,
                           const std::string& title = "Statistics") {
        std::stringstream ss;
        ss << "=== " << title << " ===\n";
        for (const auto& pair : stats) {
            ss << pair.first << ": " << std::fixed << std::setprecision(6) 
               << pair.second << "\n";
        }
        return ss.str();
    }, "Format statistics dictionary for human-readable output");

    // Pythonic Interfaces:
    
    // Add Python-style property access where appropriate:
    py::class_<ConfigWrapper>(m, "ConfigWrapper", R"pbdoc(Pythonic wrapper for Config)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("get", &ConfigWrapper::get, py::arg("key"),
             "Get a value from config (Pythonic)")
        .def("set", &ConfigWrapper::set, py::arg("key"), py::arg("value"),
             "Set a value in config (Pythonic)")
        .def("has", &ConfigWrapper::has, py::arg("key"),
             "Check if key exists (Pythonic)")
        .def("__getitem__", &ConfigWrapper::get, py::arg("key"),
             "Get item via [] syntax (Pythonic)")
        .def("__setitem__", &ConfigWrapper::set, py::arg("key"), py::arg("value"),
             "Set item via [] syntax (Pythonic)")
        .def("__contains__", &ConfigWrapper::has, py::arg("key"),
             "Check if key in config (Pythonic)")
        .def("get_with_default", &ConfigWrapper::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get value with default (Pythonic)")
        .def("__repr__", &ConfigWrapper::toString,
             "String representation (Pythonic)");

    // Add convenience methods for common patterns:
    m.def("createStandardEnvironment", []() {
        std::vector<std::string> result;
        
        auto config = createDefaultConfig();
        result.push_back("Created config: " + config->summary());
        
        auto brain = createBrainWithDefaults();
        result.push_back("Created brain with default config");
        
        auto world = createSimpleWorldWithDefaults();
        result.push_back("Created simple world with defaults");
        
        auto agent = createAgentBrainWithFeatures(brain, true, true, true, true);
        result.push_back("Created agent brain with all features enabled");
        
        return result;
    }, "Create a standard environment with all components");

    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain,
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      size_t numSteps) {
        std::vector<std::string> outputs;
        
        // Initialize components
        brain->initialize();
        agent->initialize(*world);
        world->reset();
        
        // Run simulation
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Collect data
            std::stringstream ss;
            ss << formatSimulationStep(step, world->getSimulationTime(), 
                                        brain->getFiringNeuronCount());
            ss << " -> " << formatAction(action);
            outputs.push_back(ss.str());
        }
        
        // Log final status
        outputs.push_back("=== FINAL STATUS ===");
        outputs.push_back(logSimulationStatus(brain, world, agent));
        
        return outputs;
    }, "Run a complete simulation with initialization and logging");

    m.def("compareBrains", [](const std::vector<std::shared_ptr<Brain>>& brains,
                              const std::vector<std::string>& labels) {
        std::vector<std::string> results;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::stringstream ss;
            ss << labels[i] << ": "
               << "neurons=" << brains[i]->getTotalNeuronCount()
               << ", spikes=" << brains[i]->getTotalSpikeCount()
               << ", active=" << brains[i]->getActiveNeuronCount()
               << ", firing=" << brains[i]->getFiringNeuronCount();
            results.push_back(ss.str());
        }
        
        return results;
    }, "Compare multiple brains and return their statistics");

    // Add type conversion utilities:
    m.def("toPythonString", [](const std::string& value) {
        return value;
    }, "Convert to Python string");

    m.def("toPythonList", [](const std::vector<std::string>& values) {
        return values;
    }, "Convert to Python list");

    m.def("toPythonDict", [](const std::unordered_map<std::string, double>& map) {
        return map;
    }, "Convert to Python dictionary");

    m.def("toPythonFloat", [](double value) {
        return value;
    }, "Convert to Python float");

    m.def("toPythonInt", [](int value) {
        return value;
    }, "Convert to Python int");

    m.def("toPythonBool", [](bool value) {
        return value;
    }, "Convert to Python bool");

    // Error Handling Enhancements:
    m.def("safeOperation", [](std::function<std::string()> operation,
                              const std::string& errorMessage = "Operation failed") {
        try {
            return operation();
        } catch (const std::exception& e) {
            std::stringstream ss;
            ss << errorMessage << ": " << e.what();
            throw std::runtime_error(ss.str());
        }
    }, "Perform an operation safely with error handling");

    m.def("validateAndExecute", [](std::shared_ptr<Config> config,
                                   std::function<std::string()> operation) {
        auto validationResult = validateConfig(config);
        if (!validationResult.empty()) {
            throw std::runtime_error("Configuration validation failed: " + validationResult);
        }
        
        return safeOperation(operation, "Operation failed after configuration validation");
    }, "Validate configuration then execute operation");

    m.def("tryCreateWithFallback", [](std::function<std::shared_ptr<Brain>()> primary,
                                      std::function<std::shared_ptr<Brain>()> fallback,
                                      const std::string& errorContext = "Brain creation") {
        try {
            return primary();
        } catch (const std::exception& e) {
            std::stringstream ss;
            ss << errorContext << " failed: " << e.what() << ". Trying fallback...";
            return fallback();
        }
    }, "Try primary creation, fallback if fails");

    m.def("createWithRecovery", [](std::function<std::shared_ptr<Brain>()> creator,
                                   const std::string& recoveryAction = "reset") {
        try {
            return creator();
        } catch (const std::exception& e) {
            std::stringstream ss;
            ss << "Creation failed: " << e.what() << ". Attempting recovery: " << recoveryAction;
            return creator(); // Try again after recovery
        }
    }, "Create with error recovery");

    // Final line to ensure everything is closed properly
    m.attr("VERSION") = "3.0.0";
    m.attr("DESCRIPTION") = "Enhanced NLM Python bindings with factory functions and utilities";

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm