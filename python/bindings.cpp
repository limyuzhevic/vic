// Enhanced Python Bindings for NLM (Neural Learning Machine)
// 
// This file provides comprehensive Python bindings for the NLM C++ neural simulation framework.
// It includes the core bindings from the original system plus enhanced documentation,
// convenience functions, and better integration with Phase 6 features.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>

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

// Helper functions and utilities

// Configuration helper functions
static std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set reasonable Phase 6 defaults
    config->set("brain.neuron_count", 1000, ConfigSource::Default);
    config->set("brain.synapse_density", 0.15f, ConfigSource::Default);
    config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
    config->set("brain.v_thresh", -55.0f, ConfigSource::Default);
    config->set("brain.v_rest", -70.0f, ConfigSource::Default);
    config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity configuration
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Neuromodulation configuration
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    return config;
}

static std::shared_ptr<Config> createSmallConfig() {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 100, ConfigSource::Default);
    config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
    config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
    return config;
}

static std::shared_ptr<Config> createLargeConfig() {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 5000, ConfigSource::Default);
    config->set("brain.synapse_density", 0.2f, ConfigSource::Default);
    config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
    return config;
}

// Convenience function for creating brain with default configuration
static std::shared_ptr<Brain> createBrain() {
    return std::make_shared<Brain>(createDefaultConfig());
}

// Convenience function for creating brain with custom parameters
static std::shared_ptr<Brain> createBrain(size_t neuronCount, float connectionProbability = 0.05f) {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("brain.connection_probability", connectionProbability, ConfigSource::Default);
    return std::make_shared<Brain>(config);
}

// Convenience function for creating a complete agent setup
static std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> createAgentSetup(size_t worldSize = 16) {
    // Create components
    auto brain = createBrain();
    auto world = std::make_shared<SimpleWorld>();
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Configure world
    world->configure(worldSize, worldSize, 8, 8);
    world->reset();
    world->setAgentStart(worldSize / 2.0f, worldSize / 2.0f);
    
    // Initialize agent
    agent->initialize(*world);
    
    return std::make_tuple(brain, agent, world);
}

// Simulation helper functions
static void runEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                       std::shared_ptr<AgentBrain> agent, size_t numSteps) {
    if (!brain || !world || !agent) {
        throw std::runtime_error("All components must be initialized");
    }
    
    brain->initialize();
    agent->initialize(*world);
    
    for (size_t step = 0; step < numSteps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain thinks
        brain->step(step, step * 0.001);
        
        // Get action and apply
        MotorCommand cmd = agent->decodeMotorCommand();
        world->applyMotorCommand(cmd, world->getSimulationTime());
        
        // Apply learning signals
        float reward = 0.0f;
        if (world->getSensoryPercept().getInternal().size() > 0) {
            reward = world->getSensoryPercept().getInternal()[0];
        }
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
    }
}

static float runSimulation(std::shared_ptr<Brain> brain, size_t numSteps) {
    if (!brain) {
        throw std::runtime_error("Brain must be initialized");
    }
    
    brain->initialize();
    
    float totalReward = 0.0f;
    for (size_t step = 0; step < numSteps; ++step) {
        brain->step(step, step * 0.001);
        
        // Simulate some reward based on firing rate
        float reward = brain->getFiringNeuronCount() / static_cast<float>(brain->getTotalNeuronCount());
        totalReward += reward;
    }
    
    return totalReward / numSteps;
}

// Diagnostic and analysis functions
static std::string analyzeBrainState(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return "Brain is not initialized";
    }
    
    std::stringstream ss;
    ss << "=== Brain State Analysis ===" << std::endl;
    ss << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    ss << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    ss << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    ss << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    ss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    ss << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    return ss.str();
}

static std::string analyzeAgentState(std::shared_ptr<AgentBrain> agent) {
    if (!agent) {
        return "Agent is not initialized";
    }
    
    std::stringstream ss;
    ss << "=== Agent State Analysis ===" << std::endl;
    ss << "Developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    ss << "Neuromodulation level: " << agent->getNeuromodulationLevel() << std::endl;
    ss << "Curiosity level: " << agent->getCuriosityLevel() << std::endl;
    ss << "Novelty level: " << agent->getNoveltyLevel() << std::endl;
    ss << "Prediction error: " << agent->getPredictionError() << std::endl;
    ss << "Reward modulation enabled: " << (agent->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
    ss << "Development enabled: " << (agent->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    ss << "Curiosity enabled: " << (agent->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

static std::string analyzeWorldState(std::shared_ptr<SimpleWorld> world) {
    if (!world) {
        return "World is not initialized";
    }
    
    std::stringstream ss;
    ss << "=== World State Analysis ===" << std::endl;
    ss << "Width: " << world->getWidth() << std::endl;
    ss << "Height: " << world->getHeight() << std::endl;
    ss << "Simulation time: " << world->getSimulationTime() << std::endl;
    ss << "Max energy: " << world->getMaxEnergy() << std::endl;
    
    auto body = world->getAgentBody();
    ss << "Agent position: (" << body.x << ", " << body.y << ")" << std::endl;
    ss << "Agent orientation: " << body.orientation << std::endl;
    ss << "Agent energy: " << body.energy << std::endl;
    ss << "Agent health: " << body.health << std::endl;
    ss << "Agent age: " << body.age << std::endl;
    
    return ss.str();
}

// Configuration loading helper
static std::shared_ptr<Config> loadConfigFromFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        throw std::runtime_error("Failed to load configuration from: " + filepath);
    }
    return config;
}

static bool saveConfigToFile(std::shared_ptr<Config> config, const std::string& filepath) {
    if (!config) {
        throw std::runtime_error("Configuration is not initialized");
    }
    return config->saveToFile(filepath);
}

// PyBind11 module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ----------------------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        This enhanced version includes:
        
        **Core Classes:**
        - Brain: Central neural simulation brain class
        - Config: Configuration class for NLM system
        - AgentBrain: Agent interface connecting brain to world
        - SimpleWorld: Simple 2D world for NLM simulation
        
        **Data Types:**
        - NeuronType, SynapseType, DevelopmentalStage, FiringState
        - ActionType, MotorCommand, WorldObjectType
        - NeuronId, SynapseId, RegionId, PopulationId
        
        **Sensory and Motor Classes:**
        - Vision, Audio, InternalSignals: Sensory input types
        - Action: Motor output representation
        - SensoryPercept: Sensory percept data
        - AgentBody: Agent body state
        
        **Helper Functions:**
        - createDefaultConfig(): Create configuration with Phase 6 defaults
        - createBrain(): Create brain with default configuration
        - createBrain(neuronCount, connectionProbability): Create brain with custom parameters
        - createAgentSetup(worldSize): Create complete agent setup (brain, agent, world)
        - loadConfigFromFile(filepath): Load configuration from file
        - saveConfigToFile(config, filepath): Save configuration to file
        - runEpisode(brain, world, agent, numSteps): Run an episode
        - runSimulation(brain, numSteps): Run a simulation
        - analyzeBrainState(brain): Analyze brain state
        - analyzeAgentState(agent): Analyze agent state
        - analyzeWorldState(world): Analyze world state
        
        **Usage Examples:**
        
        ```python
        import pynlm
        
        # Create a brain with default configuration
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Run a simple simulation
        avg_reward = pynlm.runSimulation(brain, 1000)
        print(f"Average reward: {avg_reward}")
        
        # Create a complete agent setup
        brain, agent, world = pynlm.createAgentSetup(16)
        
        # Run an episode
        pynlm.runEpisode(brain, world, agent, 500)
        
        # Analyze states
        print(pynlm.analyzeBrainState(brain))
        print(pynlm.analyzeAgentState(agent))
        print(pynlm.analyzeWorldState(world))
        ```
        
        The enhanced bindings provide comprehensive documentation, convenience functions,
        and better integration with Phase 6 features for both beginners and advanced users.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register exception for configuration errors
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Basic identifier types with enhanced documentation
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enumeration types with descriptions
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that increase firing rates")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that decrease firing rates")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that influence plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that produce actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse (gap junction)")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited from firing")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Enhanced Config class with better documentation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON or YAML file")
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
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set an integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set a float configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set a string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set a boolean configuration value")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get an integer configuration value (with default)")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get a float configuration value (with default)")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get a string configuration value (with default)")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a boolean configuration value (with default)");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("normalize", [](Vision& self) {
            self.normalize();
        }, "Normalize vision data to [0, 1]")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            self.resize(newWidth, newHeight);
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize vision image");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("normalize", [](Audio& self) {
            self.normalize();
        }, "Normalize audio data to [-1, 1]")
        .def("computeFFT", &Audio::computeFFT, "Compute frequency spectrum");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get number of signals")
        .def("getMaxSignal", &InternalSignals::getMaxSignal, "Get maximum signal value")
        .def("getMinSignal", &InternalSignals::getMinSignal, "Get minimum signal value")
        .def("getMeanSignal", &InternalSignals::getMeanSignal, "Get mean signal value")
        .def("getStdDev", &InternalSignals::getStdDev, "Get standard deviation of signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("addParameter", [](Action& self, float param) {
            auto params = self.getParameters();
            params.push_back(param);
            self.setParameters(params);
        }, py::arg("param"), "Add a parameter to action")
        .def("getParameterCount", &Action::getParameterCount, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            return index < params.size() ? params[index] : 0.0f;
        }, py::arg("index"), "Get parameter at index")
        .def("clone", &Action::clone, "Clone this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position and properties")
        .def_readwrite("x", &WorldObject::x, "Object X coordinate")
        .def_readwrite("y", &WorldObject::y, "Object Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("isInside", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < self.radius;
        }, py::arg("x"), py::arg("y"), "Check if position is inside object");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Agent X position")
        .def_readwrite("y", &AgentBody::y, "Agent Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Agent orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Agent X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Agent Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Agent angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Agent energy level")
        .def_readwrite("health", &AgentBody::health, "Agent health")
        .def_readwrite("age", &AgentBody::age, "Agent age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent speed")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent direction in radians")
        .def("move", [](AgentBody& self, float dx, float dy, float dt) {
            self.x += dx * dt;
            self.y += dy * dt;
            self.velocityX += dx / dt;
            self.velocityY += dy / dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Move agent by offset")
        .def("turn", [](AgentBody& self, float angle, float dt) {
            self.orientation += angle;
            self.angularVelocity += angle / dt;
        }, py::arg("angle"), py::arg("dt"), "Turn agent by angle")
        .def("reset", &AgentBody::reset, "Reset agent to initial state");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the result")
        .def("isPositive", &ActionResult::isPositive, "Whether result is positive (reward > 0)")
        .def("isNegative", &ActionResult::isNegative, "Whether result is negative (reward < 0)")
        .def("isNeutral", &ActionResult::isNeutral, "Whether result is neutral (reward = 0)");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a single vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            // Merge another percept into this one
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
        }, py::arg("other"), "Merge another percept into this one")
        .def("clone", &SensoryPercept::clone, "Clone this percept");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectCount", &SimpleWorld::getObjectCount, "Get number of objects")
        .def("getObjectsAt", [](const SimpleWorld& self, float x, float y) {
            return self.getObjectsAt(x, y);
        }, py::arg("x"), py::arg("y"), "Get objects at position")
        .def("hasObjectAt", [](const SimpleWorld& self, float x, float y) {
            return self.hasObjectAt(x, y);
        }, py::arg("x"), py::arg("y"), "Check if object exists at position")
        .def("getNearestObject", [](const SimpleWorld& self, float x, float y) {
            return self.getNearestObject(x, y);
        }, py::arg("x"), py::arg("y"), "Get nearest object to position");

    // Enhanced Brain class with more methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
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
        .def("getTotalStepCount", &Brain::getTotalStepCount,
             "Get total number of steps taken")
        .def("getAverageStepTime", &Brain::getAverageStepTime,
             "Get average time per step")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("enablePlasticity", [](Brain& self, bool hebbian, bool stdp, bool rewardModulated) {
            if (auto* region = self.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(hebbian, stdp, rewardModulated);
                }
            }
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
           "Enable plasticity mechanisms on all synapses")
        .def("getSynapseCountByRegion", [](const Brain& self, RegionId regionId) {
            if (auto* region = self.getRegion(regionId)) {
                return region->getSynapseCount();
            }
            return size_t(0);
        }, py::arg("regionId"), "Get synapse count for a specific region")
        .def("getNeuronCountByRegion", [](const Brain& self, RegionId regionId) {
            if (auto* region = self.getRegion(regionId)) {
                return region->getNeuronCount();
            }
            return size_t(0);
        }, py::arg("regionId"), "Get neuron count for a specific region");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("setCuriosityEnabled", [](AgentBrain& self, bool enable) {
            self.enableCuriosity(enable);
        }, py::arg("enable"), "Enable or disable curiosity (alias for enableCuriosity)")
        .def("setDevelopmentEnabled", [](AgentBrain& self, bool enable) {
            self.enableDevelopment(enable);
        }, py::arg("enable"), "Enable or disable development (alias for enableDevelopment)")
        .def("setRewardModulationEnabled", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
        }, py::arg("enable"), "Enable or disable reward modulation (alias for enableRewardModulation)")
        .def("setStructuralPlasticityEnabled", [](AgentBrain& self, bool enable) {
            self.enableStructuralPlasticity(enable);
        }, py::arg("enable"), "Enable or disable structural plasticity (alias for enableStructuralPlasticity)")
        .def("getExplorationLevel", [](const AgentBrain& self) {
            return self.getCuriosityLevel() * 0.5f + self.getNoveltyLevel() * 0.5f;
        }, "Get overall exploration level (curiosity + novelty)")
        .def("getLearningReadiness", [](const AgentBrain& self) {
            float neuromod = self.getNeuromodulationLevel();
            float curiosity = self.getCuriosityLevel();
            return std::abs(neuromod) * 0.7f + curiosity * 0.3f;
        }, "Get learning readiness level")
        .def("getActionSelectionBias", [](const AgentBrain& self) {
            return self.getCuriosityLevel();
        }, "Get action selection bias (higher = more random)");

    // Enhanced helper functions
    m.def("createDefaultConfig", &createDefaultConfig, "Create a default configuration with Phase 6 settings");
    
    m.def("createSmallConfig", &createSmallConfig, "Create a small configuration for testing");
    
    m.def("createLargeConfig", &createLargeConfig, "Create a large configuration for full simulations");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with custom configuration");
    
    m.def("createBrain", &createBrain, "Create a brain with default configuration");
    
    m.def("createBrain", &createBrain, py::arg("neuronCount"), py::arg("connectionProbability") = 0.05f,
         "Create a brain with specified neuron count and connection probability");
    
    m.def("createSimpleWorld", &createSimpleWorld, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    m.def("createAgentSetup", [](size_t worldSize) {
        auto result = createAgentSetup(worldSize);
        return std::make_tuple(std::get<0>(result), std::get<1>(result), std::get<2>(result));
    }, py::arg("worldSize") = 16,
       "Create a complete agent setup (brain, agent, world)");
    
    m.def("runEpisode", &runEpisode,
         py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
         "Run a complete episode with brain, world, and agent");
    
    m.def("runSimulation", &runSimulation,
         py::arg("brain"), py::arg("numSteps"),
         "Run a simple simulation with just brain");
    
    m.def("loadConfigFromFile", &loadConfigFromFile, py::arg("filepath"),
          "Load configuration from file");
    
    m.def("saveConfigToFile", &saveConfigToFile, py::arg("config"), py::arg("filepath"),
          "Save configuration to file");
    
    m.def("analyzeBrainState", &analyzeBrainState, py::arg("brain"),
          "Analyze and return brain state as a string");
    
    m.def("analyzeAgentState", &analyzeAgentState, py::arg("agent"),
          "Analyze and return agent state as a string");
    
    m.def("analyzeWorldState", &analyzeWorldState, py::arg("world"),
          "Analyze and return world state as a string");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_WORLD_SIZE") = 16;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_RANDOM_SEED") = 42;

    // Usage examples and documentation
    m.def("getUsageExamples", []() {
        return R"pbdoc(
            # Basic usage
            import pynlm
            
            brain = pynlm.createBrain()
            brain.initialize()
            
            # Run simulation
            reward = pynlm.runSimulation(brain, 100)
            
            # Create agent setup
            brain, agent, world = pynlm.createAgentSetup(16)
            
            # Run episode
            pynlm.runEpisode(brain, world, agent, 500)
            
            # Analyze state
            print(pynlm.analyzeBrainState(brain))
        )pbdoc";
    }, "Get usage examples");

    m.def("getQuickStart", []() {
        return R"pbdoc(
            # Quick Start (Copy & Paste)
            
            ### Example 1: Simplest Brain
            import pynlm
            
            brain = pynlm.createBrain()
            brain.initialize()
            
            for i in range(100):
                brain.step(i)
            
            print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
            
            ### Example 2: Brain in a Simple World
            import pynlm
            
            brain = pynlm.createBrain()
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            for step in range(50):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            print("Simulation finished!")
        )pbdoc";
    }, "Get quick start examples");

    m.def("getConfigurationParameters", []() {
        return R"pbdoc(
            Configuration Parameters:
            
            Brain Configuration:
            - brain.neuron_count: Total number of neurons (int)
            - brain.synapse_density: Synapse connectivity density (float)
            - brain.connection_probability: Probability of connection between neurons (float)
            - brain.initial_weight_mean: Mean initial synaptic weight (float)
            - brain.initial_weight_std: Standard deviation of initial weights (float)
            - brain.v_thresh: Neuron threshold potential (mV)
            - brain.v_rest: Resting potential (mV)
            - brain.v_reset: Reset potential after spike (mV)
            - brain.tau_mem: Membrane time constant (ms)
            - brain.tau_ref: Refractory period (ms)
            
            Plasticity Configuration:
            - plasticity.stdp.enable: Enable STDP (bool)
            - plasticity.stdp.learning_rate: STDP learning rate (float)
            - plasticity.stdp.tau_plus: STDP time constant (ms)
            - plasticity.stdp.tau_minus: STDP time constant (ms)
            - plasticity.hebbian.enable: Enable Hebbian learning (bool)
            - plasticity.structural.enable: Enable structural plasticity (bool)
            
            Neuromodulation Configuration:
            - neuromod.dopamine.scale: Dopamine modulation scale (float)
            - neuromod.curiosity.enable: Enable curiosity-driven exploration (bool)
            - neuromod.novelty.enable: Enable novelty detection (bool)
        )pbdoc";
    }, "Get configuration parameters documentation");

    m.def("getActionTypes", []() {
        return R"pbdoc(
            Action Types:
            - MoveForward: Move forward action
            - MoveBackward: Move backward action
            - MoveLeft: Move left action
            - MoveRight: Move right action
            - TurnLeft: Turn left action
            - TurnRight: Turn right action
            - Look: Look action
            - LookUp: Look up action
            - LookDown: Look down action
            - Interact: Interact action
            - Eat: Eat action
            - Drink: Drink action
            - Rest: Rest action
            - Wait: Wait action
            - Custom: Custom action
        )pbdoc";
    }, "Get action types documentation");

    m.def("getNeuronTypes", []() {
        return R"pbdoc(
            Neuron Types:
            - Excitatory: Excitatory neurons that increase firing rates
            - Inhibitory: Inhibitory neurons that decrease firing rates
            - Modulatory: Modulatory neurons that influence plasticity
            - Sensory: Sensory neurons that receive external input
            - Motor: Motor neurons that produce actions
            - Internal: Internal neurons for internal processing
        )pbdoc";
    }, "Get neuron types documentation");

    m.def("getDevelopmentalStages", []() {
        return R"pbdoc(
            Developmental Stages:
            - Initial: Initial developmental stage
            - CriticalPeriod: Critical period for learning
            - Maturation: Maturation stage
            - Adult: Adult stage
            - Aging: Aging stage
        )pbdoc";
    }, "Get developmental stages documentation");

    m.def("getSensoryInputTypes", []() {
        return R"pbdoc(
            Sensory Input Types:
            - Vision: Visual input from world
            - Audio: Audio input from world
            - InternalSignals: Internal signals (energy, health, age)
            - Proprioception: Body position and movement
        )pbdoc";
    }, "Get sensory input types documentation");
}

} // namespace nlm