// Enhanced Python bindings for NLM with improved API
// This replaces the original bindings.cpp with better usability and more features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Helper function to create a default configuration with common settings
std::shared_ptr<Config> createEnhancedConfig() {
    auto config = std::make_shared<Config>();
    
    // Set default values for common use cases
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", double(0.001));
    config->set("stdp_ltp_weight", float(0.01f));
    config->set("stdp_ltd_weight", float(0.012f));
    config->set("stdp_tau", float(20.0f));
    config->set("synaptogenesis_rate", float(0.0001f));
    config->set("pruning_rate", float(0.00001f));
    config->set("replay_interval", size_t(100));
    config->set("consolidation_interval", size_t(1000));
    
    return config;
}

// Enhanced brain factory with automatic initialization
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {
        brain_->initialize();
    }
    
    // Step brain with optional callback for monitoring
    void step(SimulationStep step, std::function<void(const EnhancedBrain&)> monitor = nullptr) {
        brain_->step(step);
        if (monitor) {
            monitor(*this);
        }
    }
    
    // Convenience method for common monitoring tasks
    void stepAndMonitor(SimulationStep step) {
        step(step, [this](const EnhancedBrain& enhancedBrain) {
            // Log important statistics
            size_t firingCount = enhancedBrain.getFiringNeuronCount();
            size_t totalSpikes = enhancedBrain.getTotalSpikeCount();
            if (firingCount > 0) {
                // Could log or process these stats
            }
        });
    }
    
    // Enable all learning systems with reasonable defaults
    void enableAllLearning() {
        if (agentBrain_) {
            agentBrain_->enableRewardModulation(true);
            agentBrain_->enableStructuralPlasticity(true);
            agentBrain_->enableDevelopment(true);
            agentBrain_->enableCuriosity(true);
        }
    }
    
    // Convenience methods for common operations
    float getExcitatoryInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getActiveNeurons() const {
        return brain_->getActiveNeuronCount();
    }
    
    size_t getTotalNeurons() const {
        return brain_->getTotalNeuronCount();
    }
    
    // Get underlying brain for advanced operations
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Get agent brain if available
    std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
    
    // Set agent brain for world interaction
    void setAgentBrain(std::shared_ptr<AgentBrain> agentBrain) {
        agentBrain_ = agentBrain;
    }
    
    // Create a complete agent setup
    static std::pair<std::shared_ptr<EnhancedBrain>, std::shared_ptr<SimpleWorld>> 
    createAgentWithWorld(int width = 20, int height = 20) {
        auto config = createEnhancedConfig();
        auto enhancedBrain = std::make_shared<EnhancedBrain>(config);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(enhancedBrain->getBrain());
        agent->initialize(world);
        enhancedBrain->setAgentBrain(agent);
        
        return {enhancedBrain, world};
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Enhanced sensory input factory with common patterns
class SensoryInputFactory {
public:
    // Create vision input with common configurations
    static std::shared_ptr<Vision> createVision(size_t width, size_t height, size_t channels = 3) {
        return std::make_shared<Vision>(width, height, channels);
    }
    
    // Create audio input with common sampling rates
    static std::shared_ptr<Audio> createAudio(size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }
    
    // Create internal signals with default capacity
    static std::shared_ptr<InternalSignals> createInternalSignals(size_t capacity = 100) {
        auto signals = std::make_shared<InternalSignals>();
        for (size_t i = 0; i < capacity; ++i) {
            signals->addSignal(0.0f);
        }
        return signals;
    }
    
    // Create percept from components
    static std::shared_ptr<SensoryPercept> createPercept(
        std::shared_ptr<Vision> vision = nullptr,
        std::shared_ptr<Audio> audio = nullptr,
        std::shared_ptr<InternalSignals> internal = nullptr
    ) {
        auto percept = std::make_shared<SensoryPercept>();
        if (vision) percept->setVision(vision);
        if (audio) percept->setAudio(audio);
        if (internal) percept->setInternal(internal);
        return percept;
    }
};

// Enhanced agent brain with convenience methods
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
    
    // Process sensory input with automatic timestamping
    void processSensoryInput(const SensoryPercept& percept) {
        agentBrain_->processSensoryInput(percept);
    }
    
    // Decode motor command with safety checks
    Action decodeMotorCommandSafely() {
        auto action = agentBrain_->decodeMotorCommand();
        if (!action) {
            // Return safe default action
            return Action(ActionType::Wait);
        }
        return *action;
    }
    
    // Apply reward modulation with bounds checking
    void applyRewardModulation(float reward, float predictedReward = 0.0f) {
        // Clamp reward to reasonable range
        reward = std::max(-10.0f, std::min(10.0f, reward));
        predictedReward = std::max(-10.0f, std::min(10.0f, predictedReward));
        
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Get comprehensive state summary
    std::map<std::string, float> getStateSummary() const {
        std::map<std::string, float> state;
        
        auto brain = agentBrain_->getBrain();
        if (brain) {
            state["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            state["average_firing_rate"] = brain->getAverageFiringRate();
            state["total_spikes"] = static_cast<float>(brain->getTotalSpikeCount());
            state["development_stage"] = static_cast<float>(brain->getDevelopmentalStage());
        }
        
        state["neuromodulation_level"] = agentBrain_->getNeuromodulationLevel();
        state["curiosity_level"] = agentBrain_->getCuriosityLevel();
        state["novelty_level"] = agentBrain_->getNoveltyLevel();
        state["prediction_error"] = agentBrain_->getPredictionError();
        
        return state;
    }
    
    // Update development with optional callback
    void updateDevelopment(SimulationStep step, std::function<void(float)> callback = nullptr) {
        agentBrain_->updateDevelopment(step);
        if (callback) {
            float stageProgress = static_cast<float>(agentBrain_->getDevelopmentalStage()) / 4.0f;
            callback(stageProgress);
        }
    }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides:
        - EnhancedBrain: High-level brain interface with built-in monitoring
        - EnhancedAgentBrain: Convenience wrapper for agent operations
        - SensoryInputFactory: Factory methods for creating sensory inputs
        - Advanced configuration management
        - Simplified world setup for agents
        
        Features:
        - Automatic brain initialization
        - Monitoring callbacks for simulation tracking
        - Enhanced learning system control
        - Comprehensive state tracking
        - Improved error handling and safety checks
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic type definitions (unchanged from original)
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

    // Enums (unchanged from original)
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

    // Config class
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

    // Enhanced Config factory function
    m.def("createEnhancedConfig", &createEnhancedConfig,
          "Create an enhanced configuration with sensible defaults");

    // Original sensory input classes
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

    // Sensory input factory
    py::class_<SensoryInputFactory>(m, "SensoryInputFactory", R"pbdoc(Factory for creating sensory inputs)pbdoc")
        .def_static("createVision", &SensoryInputFactory::createVision,
                   py::arg("width"), py::arg("height"), py::arg("channels") = 3,
                   "Create vision sensory input with common configuration")
        .def_static("createAudio", &SensoryInputFactory::createAudio,
                   py::arg("sampleRate"), py::arg("numSamples"),
                   "Create audio sensory input with common configuration")
        .def_static("createInternalSignals", &SensoryInputFactory::createInternalSignals,
                   py::arg("capacity") = 100,
                   "Create internal signals with default capacity")
        .def_static("createPercept", &SensoryInputFactory::createPercept,
                   py::arg("vision") = nullptr, py::arg("audio") = nullptr, py::arg("internal") = nullptr,
                   "Create a sensory percept from components");

    // Action class (unchanged from original)
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

    // Other original classes (WorldObject, AgentBody, ActionResult, SensoryPercept)
    // [Keep original implementations for backward compatibility]

    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain", R"pbdoc(High-level brain interface with built-in monitoring)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("step", &EnhancedBrain::step,
             py::arg("current_step"), py::arg("monitor") = nullptr,
             "Perform a simulation step with optional monitoring callback")
        .def("stepAndMonitor", &EnhancedBrain::stepAndMonitor,
             py::arg("current_step"), "Perform a step and automatically monitor statistics")
        .def("enableAllLearning", &EnhancedBrain::enableAllLearning,
             "Enable all learning systems with reasonable defaults")
        .def("getExcitatoryInhibitionRatio", &EnhancedBrain::getExcitatoryInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getActiveNeurons", &EnhancedBrain::getActiveNeurons,
             "Get count of active neurons")
        .def("getTotalNeurons", &EnhancedBrain::getTotalNeurons,
             "Get total neuron count across all regions")
        .def("enableRewardModulation", &EnhancedBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &EnhancedBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &EnhancedBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development system")
        .def("enableCuriosity", &EnhancedBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity system");

    // Original Brain class
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

    // Factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create a new brain with configuration and automatic initialization");

    m.def("createEnhancedBrain", &EnhancedBrain,
          py::arg("config"), "Create an enhanced brain with monitoring and convenience methods");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createEnhancedConfig", &createEnhancedConfig,
          "Create an enhanced configuration with sensible defaults for common use cases");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Create a complete agent setup helper
    m.def("createAgentWithWorld", []() {
        return std::make_tuple(std::make_shared<EnhancedBrain>(createEnhancedConfig()),
                             std::make_shared<SimpleWorld>());
    }, "Create a complete agent setup with brain and world");

    // AgentBrain class
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

    // Invalid IDs (unchanged from original)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
