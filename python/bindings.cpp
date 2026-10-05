#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Custom Python exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain initialization error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

// Pythonic wrapper for Brain with enhanced API
class BrainWrapper {
public:
    std::shared_ptr<Brain> brain;
    
    explicit BrainWrapper(std::shared_ptr<Brain> b) : brain(b) {}
    
    bool initialize() {
        return brain->initialize();
    }
    
    void step(uint64_t step) {
        brain->step(step);
    }
    
    void step_with_time(uint64_t step, double time) {
        brain->step(step, time);
    }
    
    // Enhanced convenience methods
    std::vector<float> getNeuronActivations() const {
        std::vector<float> activations;
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        activations.push_back(1.0f);
                    } else {
                        activations.push_back(0.0f);
                    }
                }
            }
        }
        return activations;
    }
    
    size_t getActiveNeuronCount() const {
        return brain->getActiveNeuronCount();
    }
    
    float getAverageFiringRate() const {
        return brain->getAverageFiringRate();
    }
    
    size_t getTotalSpikeCount() const {
        return brain->getTotalSpikeCount();
    }
    
    std::string getStatus() const {
        std::stringstream ss;
        ss << "=== NLM Brain Status (Phase 6) ===" << std::endl;
        ss << "Regions: " << brain->getRegionCount() << std::endl;
        ss << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "Firing neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
        ss << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        // Memory system status
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            ss << "Working memory traces: " << wm->getActiveTraces() << std::endl;
        }
        
        auto* em = brain->getEpisodicMemory();
        if (em) {
            ss << "Episodic memory episodes: " << em->getEpisodeCount() << std::endl;
        }
        
        return ss.str();
    }
    
    // Memory system access methods
    std::string getWorkingMemoryStatus() const {
        if (auto* wm = brain->getWorkingMemory()) {
            return "Working memory: " + std::to_string(wm->getActiveTraces()) + " active traces";
        }
        return "Working memory: Not available";
    }
    
    std::string getEpisodicMemoryStatus() const {
        if (auto* em = brain->getEpisodicMemory()) {
            return "Episodic memory: " + std::to_string(em->getEpisodeCount()) + " episodes stored";
        }
        return "Episodic memory: Not available";
    }
    
    std::string getPredictionSystemStatus() const {
        if (brain->getPredictionSystem()) {
            return "Prediction system: Available (placeholder implementation)";
        }
        return "Prediction system: Not available";
    }
    
    std::string getCognitionStatus() const {
        std::stringstream ss;
        ss << "Cognitive Systems:" << std::endl;
        
        if (brain->getPlanner()) ss << "  Neural Planner: Available" << std::endl;
        if (brain->getConceptFormation()) ss << "  Concept Formation: Available" << std::endl;
        if (brain->getAttention()) ss << "  Attentional Selection: Available" << std::endl;
        
        return ss.str();
    }
    
    std::string getNeuromodulationStatus() const {
        std::stringstream ss;
        ss << "Neuromodulation Systems:" << std::endl;
        
        if (auto* dopamine = brain->getDopamine()) {
            ss << "  Dopamine: Available (level: " << dopamine->getLevel() << ")" << std::endl;
        }
        if (auto* curiosity = brain->getCuriosity()) {
            ss << "  Curiosity: Available (level: " << curiosity->getLevel() << ")" << std::endl;
        }
        if (auto* novelty = brain->getNovelty()) {
            ss << "  Novelty: Available (level: " << novelty->getLevel() << ")" << std::endl;
        }
        if (auto* predictionError = brain->getPredictionErrorSignal()) {
            ss << "  Prediction Error: Available (level: " << predictionError->getLevel() << ")" << std::endl;
        }
        
        return ss.str();
    }
    
    std::string getDevelopmentStatus() const {
        auto stage = brain->getDevelopmentalStage();
        std::string stageStr;
        switch (stage) {
            case DevelopmentalStage::Initial: stageStr = "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: stageStr = "Critical Period"; break;
            case DevelopmentalStage::Maturation: stageStr = "Maturation"; break;
            case DevelopmentalStage::Adult: stageStr = "Adult"; break;
            case DevelopmentalStage::Aging: stageStr = "Aging"; break;
        }
        return "Development: Stage " + stageStr + ", updates every 1000 steps";
    }
    
    // Convenience methods for simulation control
    void run_steps(size_t numSteps) {
        for (size_t i = 0; i < numSteps; ++i) {
            step(i);
        }
    }
    
    void run_steps_with_time(size_t numSteps, double timeStep = 0.001) {
        for (size_t i = 0; i < numSteps; ++i) {
            step_with_time(i, i * timeStep);
        }
    }
    
    // Statistics tracking
    struct SimulationStats {
        size_t totalSteps;
        size_t totalSpikes;
        float avgFiringRate;
        float totalReward;
        size_t episodesStored;
        double elapsedTime;
        
        std::string to_string() const {
            std::stringstream ss;
            ss << "Simulation Statistics:" << std::endl;
            ss << "  Total steps: " << totalSteps << std::endl;
            ss << "  Total spikes: " << totalSpikes << std::endl;
            ss << "  Average firing rate: " << avgFiringRate << " Hz" << std::endl;
            ss << "  Total reward: " << totalReward << std::endl;
            ss << "  Episodes stored: " << episodesStored << std::endl;
            ss << "  Elapsed time: " << elapsedTime << " seconds" << std::endl;
            return ss.str();
        }
    };
    
    SimulationStats getSimulationStats() const {
        SimulationStats stats;
        stats.totalSteps = 0; // Would need to track this in the Brain class
        stats.totalSpikes = brain->getTotalSpikeCount();
        stats.avgFiringRate = brain->getAverageFiringRate();
        stats.totalReward = 0.0f; // Would need tracking
        stats.episodesStored = brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0;
        stats.elapsedTime = stats.totalSteps * 0.001; // Assuming 1ms timestep
        return stats;
    }
    
    // Reset methods
    void reset_all() {
        brain->reset();
    }
    
    // Configuration access
    std::shared_ptr<const Config> get_config() const {
        return brain->getConfig();
    }
};

// Pythonic wrapper for AgentBrain
class AgentBrainWrapper {
public:
    std::shared_ptr<AgentBrain> agent;
    
    explicit AgentBrainWrapper(std::shared_ptr<AgentBrain> a) : agent(a) {}
    
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agent->initialize(world);
    }
    
    void processSensoryInput(const SensoryPercept& percept) {
        agent->processSensoryInput(percept);
    }
    
    Action decodeMotorCommand() {
        return agent->decodeMotorCommand();
    }
    
    void applyRewardModulation(float reward, float predictedReward) {
        agent->applyRewardModulation(reward, predictedReward);
    }
    
    void updateDevelopment(double timestep) {
        agent->updateDevelopment(timestep);
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return agent->getDevelopmentalStage();
    }
    
    float getNeuromodulationLevel() const {
        return agent->getNeuromodulationLevel();
    }
    
    float getCuriosityLevel() const {
        return agent->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agent->getNoveltyLevel();
    }
    
    float getPredictionError() const {
        return agent->getPredictionError();
    }
    
    void reset() {
        agent->reset();
    }
    
    std::string getMotivationalState() const {
        std::stringstream ss;
        ss << "=== Agent Motivational State ===" << std::endl;
        ss << "Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
        ss << "Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
        ss << "Novelty Level: " << agent->getNoveltyLevel() << std::endl;
        ss << "Prediction Error: " << agent->getPredictionError() << std::endl;
        ss << "Development Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        ss << "Reward Modulation: " << (agent->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "Structural Plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "Development: " << (agent->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "Curiosity: " << (agent->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        return ss.str();
    }
    
    void enable_all_systems() {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
    }
};

// Helper functions for creating common configurations
Config create_default_config() {
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory parameters
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    return config;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ===================================================
        
        An enhanced Python interface for the NLM C++ neural simulation framework.
        
        This module provides a Pythonic API for working with artificial developmental brains
        featuring integrated memory systems, prediction, cognition, and neuromodulation.
        
        Key Features:
        - Enhanced error handling with specific exception types
        - Pythonic convenience methods for common operations
        - Comprehensive status and monitoring utilities
        - Simplified configuration and setup
        - Motivational state tracking for agents
        - Easy-to-use simulation control
        
        Examples:
        ---------
        import pynlm
        
        # Create a default brain with sensible parameters
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        # Initialize the brain
        brain.initialize()
        
        # Run a simple simulation
        for i in range(1000):
            brain.step(i)
            
        # Get comprehensive status information
        print(brain.get_status())
        print(brain.get_working_memory_status())
        print(brain.get_episodic_memory_status())
        
        # Create and configure agent
        agent = pynlm.create_agent_brain(brain)
        agent.enable_all_systems()
        
        # Get agent motivational state
        print(agent.get_motivational_state())
    
    )pbdoc";
    
    // Register basic type definitions
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
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
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
    
    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        .def("set", py::overload_cast<const std::string&, int64_t>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("set", py::overload_cast<const std::string&, float>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("get", py::overload_cast<const std::string&, int64_t>(&Config::get),
             py::arg("key"), py::arg("default"),
             "Get an integer configuration value")
        .def("get", py::overload_cast<const std::string&, float>(&Config::get),
             py::arg("key"), py::arg("default"),
             "Get a float configuration value");
    
    // Sensory input classes
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
    
    // Action classes
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
    
    // World system
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
    
    // Sensory percept
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
    
    // Enhanced Brain class with Python wrapper
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(Primary interface to NLM neural simulation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &BrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<uint64_t>(&BrainWrapper::step),
             "Perform a simulation step")
        .def("step_with_time", &BrainWrapper::step_with_time,
             "Perform a simulation step with timestamp")
        .def("getNeuronActivations", &BrainWrapper::getNeuronActivations,
             "Get neuron activation vector (firing: 1.0, resting: 0.0)")
        .def("getActiveNeuronCount", &BrainWrapper::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getAverageFiringRate", &BrainWrapper::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getTotalSpikeCount", &BrainWrapper::getTotalSpikeCount,
             "Get total spike count")
        .def("getStatus", &BrainWrapper::getStatus,
             "Get comprehensive brain status information")
        .def("getWorkingMemoryStatus", &BrainWrapper::getWorkingMemoryStatus,
             "Get working memory status")
        .def("getEpisodicMemoryStatus", &BrainWrapper::getEpisodicMemoryStatus,
             "Get episodic memory status")
        .def("getPredictionSystemStatus", &BrainWrapper::getPredictionSystemStatus,
             "Get prediction system status")
        .def("getCognitionStatus", &BrainWrapper::getCognitionStatus,
             "Get cognitive systems status")
        .def("getNeuromodulationStatus", &BrainWrapper::getNeuromodulationStatus,
             "Get neuromodulation systems status")
        .def("getDevelopmentStatus", &BrainWrapper::getDevelopmentStatus,
             "Get development system status")
        .def("reset", &BrainWrapper::reset,
             "Reset brain state")
        .def("save", &BrainWrapper::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainWrapper::load, py::arg("filepath"),
             "Load brain state from file")
        .def("produceAction", &BrainWrapper::produceAction,
             "Produce motor action based on neural activity")
        .def("runSteps", &BrainWrapper::run_steps, py::arg("numSteps"),
             "Run multiple simulation steps")
        .def("runStepsWithTime", &BrainWrapper::run_steps_with_time, py::arg("numSteps"),
             "Run multiple simulation steps with time control");
    
    // Enhanced AgentBrain class with Python wrapper
    py::class_<AgentBrainWrapper>(m, "AgentBrain", R"pbdoc(Interface connecting NLM brain to world simulation)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>())
        .def("initialize", &AgentBrainWrapper::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrainWrapper::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrainWrapper::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrainWrapper::processSensoryInput,
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainWrapper::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrainWrapper::applyRewardModulation,
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrainWrapper::updateDevelopment,
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrainWrapper::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrainWrapper::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainWrapper::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrainWrapper::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrainWrapper::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrainWrapper::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrainWrapper::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrainWrapper::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrainWrapper::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrainWrapper::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrainWrapper::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrainWrapper::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrainWrapper::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrainWrapper::isCuriosityEnabled)
        .def("getMotivationalState", &AgentBrainWrapper::getMotivationalState,
             "Get agent motivational state string")
        .def("enableAllSystems", &AgentBrainWrapper::enableAllSystems,
             "Enable all agent brain systems");
    
    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
    // Agent body
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
    
    // Helper functions
    m.def("create_default_config", &create_default_config,
          "Create a default NLM configuration with sensible values");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<BrainWrapper> {
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<BrainWrapper>(brain);
    }, py::arg("config"), "Create a new brain with configuration and return Python wrapper");
    
    m.def("create_agent_brain", [](std::shared_ptr<BrainWrapper> brain) -> std::shared_ptr<AgentBrainWrapper> {
        auto rawBrain = brain->brain;
        auto agent = std::make_shared<AgentBrain>(rawBrain);
        return std::make_shared<AgentBrainWrapper>(agent);
    }, py::arg("brain"), "Create a new agent brain interface and return Python wrapper");
    
    // Legacy API for backward compatibility
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a raw C++ brain (legacy API)");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a raw C++ agent brain (legacy API)");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    // Error classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    
    // Backward compatibility constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
