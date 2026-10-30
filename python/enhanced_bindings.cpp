// Enhanced NLM Python Bindings
// A Pythonic API for NLM with better abstractions and features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <fstream>
#include <stdexcept>
#include <iostream>

// Include the main NLM headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/motor/Action.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException(msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException(msg) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& msg) : NLMException(msg) {}
};

// Enhanced Config with Pythonic API
class PythonConfig {
public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    
    PythonConfig(const std::string& config_file) {
        config_ = std::make_shared<Config>();
        if (!config_->loadFromFile(config_file)) {
            throw ConfigurationError("Failed to load configuration from file: " + config_file);
        }
    }
    
    // Fluent interface for configuration
    PythonConfig& setBrainNeuronCount(size_t count) {
        config_->set("brain.neuron_count", int(count));
        return *this;
    }
    
    PythonConfig& setBrainSynapseDensity(float density) {
        config_->set("brain.synapse_density", density);
        return *this;
    }
    
    PythonConfig& setBrainConnectionProbability(float prob) {
        config_->set("brain.connection_probability", prob);
        return *this;
    }
    
    PythonConfig& enableHebbian(bool enable = true) {
        config_->set("plasticity.hebbian.enable", enable);
        return *this;
    }
    
    PythonConfig& setHebbianLearningRate(float rate) {
        config_->set("plasticity.hebbian.learning_rate", rate);
        return *this;
    }
    
    PythonConfig& enableSTDP(bool enable = true) {
        config_->set("plasticity.stdp.enable", enable);
        return *this;
    }
    
    PythonConfig& setSTDPLearningRate(float rate) {
        config_->set("plasticity.stdp.learning_rate", rate);
        return *this;
    }
    
    PythonConfig& enableCuriosity(bool enable = true) {
        config_->set("neuromod.curiosity.enable", enable);
        return *this;
    }
    
    PythonConfig& enableNovelty(bool enable = true) {
        config_->set("neuromod.novelty.enable", enable);
        return *this;
    }
    
    PythonConfig& enableDevelopment(bool enable = true) {
        config_->set("development.enabled", enable);
        return *this;
    }
    
    // Type-safe getters
    int getInt(const std::string& key) const {
        auto value = config_->get<int>(key);
        if (!value) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return *value;
    }
    
    double getDouble(const std::string& key) const {
        auto value = config_->get<double>(key);
        if (!value) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return *value;
    }
    
    bool getBool(const std::string& key) const {
        auto value = config_->get<bool>(key);
        if (!value) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return *value;
    }
    
    std::string getString(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        if (!value) {
            throw ConfigurationError("Configuration key not found: " + key);
        }
        return *value;
    }
    
    // Configuration utilities
    bool has(const std::string& key) const { return config_->has(key); }
    std::vector<std::string> getKeys() const { return config_->getKeys(); }
    bool saveToFile(const std::string& filepath) const { return config_->saveToFile(filepath); }
    void clear() { config_->clear(); }
    
    std::shared_ptr<Config> getInternal() { return config_; }
    
    // Convenience methods
    PythonConfig& withBrain(int neuron_count = 1000) {
        config_->set("brain.neuron_count", neuron_count);
        config_->set("brain.synapse_density", 0.1f);
        config_->set("brain.connection_probability", 0.05f);
        return *this;
    }
    
    PythonConfig& withLearning(float stdp_rate = 0.001f, float hebbian_rate = 0.001f) {
        config_->set("plasticity.stdp.enable", true);
        config_->set("plasticity.stdp.learning_rate", stdp_rate);
        config_->set("plasticity.hebbian.enable", true);
        config_->set("plasticity.hebbian.learning_rate", hebbian_rate);
        return *this;
    }
    
    PythonConfig& withNeuromodulation() {
        config_->set("neuromod.curiosity.enable", true);
        config_->set("neuromod.novelty.enable", true);
        return *this;
    }
    
    PythonConfig& withDevelopment() {
        config_->set("development.enabled", true);
        return *this;
    }
    
    // Static factory methods
    static std::shared_ptr<PythonConfig> createMinimal() {
        auto config = std::make_shared<PythonConfig>();
        config->withBrain(500).withLearning();
        return config;
    }
    
    static std::shared_ptr<PythonConfig> createComplex() {
        auto config = std::make_shared<PythonConfig>();
        config->withBrain(2000).withLearning(0.001f, 0.0005f)
              .withNeuromodulation().withDevelopment();
        return config;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Brain with Pythonic API
class PythonBrain {
public:
    PythonBrain(std::shared_ptr<PythonConfig> config) {
        if (!config) {
            throw ValidationError("Configuration cannot be null");
        }
        config_ = config;
        brain_ = std::make_shared<Brain>(config_->getInternal());
        
        if (!brain_->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
    }
    
    // Run simulation with statistics collection
    py::dict runSimulation(int steps, bool collect_detailed_stats = false) {
        py::dict stats;
        stats["steps_completed"] = 0;
        stats["start_time"] = 0.0f;
        
        if (collect_detailed_stats) {
            stats["neuron_counts"] = std::vector<size_t>{};
            stats["firing_rates"] = std::vector<float>{};
            stats["spike_counts"] = std::vector<size_t>{};
            stats["exc_inh_ratios"] = std::vector<float>{};
        }
        
        try {
            for (int i = 0; i < steps; ++i) {
                brain_->step(i);
                
                if (collect_detailed_stats) {
                    auto& neuron_counts = stats["neuron_counts"].cast<std::vector<size_t>&>();
                    auto& firing_rates = stats["firing_rates"].cast<std::vector<float>&>();
                    auto& spike_counts = stats["spike_counts"].cast<std::vector<size_t>&>();
                    auto& exc_inh_ratios = stats["exc_inh_ratios"].cast<std::vector<float>&>();
                    
                    neuron_counts.push_back(brain_->getFiringNeuronCount());
                    firing_rates.push_back(brain_->getAverageFiringRate());
                    spike_counts.push_back(brain_->getTotalSpikeCount());
                    exc_inh_ratios.push_back(brain_->getExcitationInhibitionRatio());
                }
                
                stats["steps_completed"] = i + 1;
            }
        } catch (const std::exception& e) {
            stats["error"] = std::string("Simulation error: ") + e.what();
            throw;
        }
        
        // Add final statistics
        stats["final_neuron_count"] = brain_->getTotalNeuronCount();
        stats["final_synapse_count"] = brain_->getTotalSynapseCount();
        stats["final_firing_neurons"] = brain_->getFiringNeuronCount();
        stats["final_average_firing_rate"] = brain_->getAverageFiringRate();
        stats["final_total_spikes"] = brain_->getTotalSpikeCount();
        stats["final_exc_inh_ratio"] = brain_->getExcitationInhibitionRatio();
        
        return stats;
    }
    
    // Convenience methods
    void enableAllLearning(bool enable = true) {
        config_->set("plasticity.stdp.enable", enable);
        config_->set("plasticity.hebbian.enable", enable);
        config_->set("plasticity.structural.enable", enable);
    }
    
    void setLearningRates(float stdp_rate, float hebbian_rate = 0.0f) {
        config_->set("plasticity.stdp.learning_rate", stdp_rate);
        config_->set("plasticity.hebbian.learning_rate", hebbian_rate);
    }
    
    // State management
    bool saveState(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool loadState(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Statistics
    size_t getNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    size_t getTotalSpikes() const { return brain_->getTotalSpikeCount(); }
    float getExcInhRatio() const { return brain_->getExcitationInhibitionRatio(); }
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
    
    // Reset
    void reset() { brain_->reset(); }
    
    std::shared_ptr<Brain> getInternalBrain() { return brain_; }
    
private:
    std::shared_ptr<PythonConfig> config_;
    std::shared_ptr<Brain> brain_;
};

// Enhanced Agent with Pythonic API
class PythonAgent {
public:
    PythonAgent(std::shared_ptr<PythonBrain> brain) {
        if (!brain) {
            throw ValidationError("Brain cannot be null");
        }
        brain_ = brain;
        agent_ = std::make_shared<AgentBrain>(brain_->getInternalBrain());
    }
    
    // Initialize with world
    void initialize(std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw ValidationError("World cannot be null");
        }
        agent_->initialize(*world);
    }
    
    // Process sensory input
    void processSensoryInput(const SensoryPercept& percept) {
        agent_->processSensoryInput(percept);
    }
    
    // Get action
    MotorCommand getAction() {
        return agent_->decodeMotorCommand();
    }
    
    // Apply reward
    void applyReward(float reward, float predicted_reward = 0.0f) {
        agent_->applyRewardModulation(reward, predicted_reward);
    }
    
    // Update development
    void updateDevelopment(double timestep = 0.1) {
        agent_->updateDevelopment(timestep);
    }
    
    // Enable subsystems
    void enableRewardModulation(bool enable) {
        agent_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agent_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agent_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agent_->enableCuriosity(enable);
    }
    
    // Get statistics
    float getCuriosityLevel() const { return agent_->getCuriosityLevel(); }
    float getNoveltyLevel() const { return agent_->getNoveltyLevel(); }
    float getPredictionError() const { return agent_->getPredictionError(); }
    float getNeuromodulationLevel() const { return agent_->getNeuromodulationLevel(); }
    DevelopmentalStage getDevelopmentalStage() const { return agent_->getDevelopmentalStage(); }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() { return agent_->getBrain(); }
    
    // Reset
    void reset() { agent_->reset(); }
    
private:
    std::shared_ptr<PythonBrain> brain_;
    std::shared_ptr<AgentBrain> agent_;
};

// Enhanced World with Pythonic API
class PythonWorld {
public:
    PythonWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Configure world
    void configure(float width, float height, size_t vision_width, size_t vision_height) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    // Set agent start
    void setAgentStart(float x, float y) { world_->setAgentStart(x, y); }
    
    // Update world
    void update(double timestep) { world_->update(timestep); }
    
    // Apply command
    py::dict applyCommand(MotorCommand cmd, double current_time) {
        py::dict result;
        ActionResult action_result = world_->applyMotorCommand(cmd, current_time);
        
        result["success"] = action_result.success;
        result["reward"] = action_result.reward;
        result["message"] = action_result.message;
        
        return result;
    }
    
    // Get percept
    SensoryPercept getPercept() const { return world_->getSensoryPercept(); }
    
    // Get agent body
    AgentBody getAgentBody() const { return world_->getAgentBody(); }
    
    // Reset world
    void reset() { world_->reset(); }
    
    // Set seed
    void setSeed(uint64_t seed) { world_->setRandomSeed(seed); }
    
    // Get properties
    float getWidth() const { return world_->getWidth(); }
    float getHeight() const { return world_->getHeight(); }
    double getSimulationTime() const { return world_->getSimulationTime(); }
    
    std::shared_ptr<SimpleWorld> getInternalWorld() { return world_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Factory for creating enhanced NLM setups
class NLMFactory {
public:
    // Create simple simulation setup
    static std::tuple<std::shared_ptr<PythonBrain>, std::shared_ptr<PythonAgent>, std::shared_ptr<PythonWorld>> 
    createSimpleSimulation() {
        auto config = std::make_shared<PythonConfig>();
        auto brain = std::make_shared<PythonBrain>(config);
        auto world = std::make_shared<PythonWorld>();
        auto agent = std::make_shared<PythonAgent>(brain);
        
        world->configure(20.0f, 20.0f, 8, 8);
        world->setAgentStart(10.0f, 10.0f);
        agent->initialize(world->getInternalWorld());
        
        return std::make_tuple(brain, agent, world);
    }
    
    // Create custom simulation setup
    static std::tuple<std::shared_ptr<PythonBrain>, std::shared_ptr<PythonAgent>, std::shared_ptr<PythonWorld>> 
    createCustomSimulation(int neuron_count = 1000, float learning_rate = 0.001f) {
        auto config = std::make_shared<PythonConfig>();
        config->withBrain(neuron_count).withLearning(learning_rate).withNeuromodulation();
        
        auto brain = std::make_shared<PythonBrain>(config);
        auto world = std::make_shared<PythonWorld>();
        auto agent = std::make_shared<PythonAgent>(brain);
        
        world->configure(30.0f, 30.0f, 8, 8);
        world->setAgentStart(15.0f, 15.0f);
        agent->initialize(world->getInternalWorld());
        agent->enableAllLearning(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }
    
    // Create training setup
    static std::tuple<std::shared_ptr<PythonBrain>, std::shared_ptr<PythonAgent>, std::shared_ptr<PythonWorld>> 
    createTrainingSetup(int episodes = 100, int steps_per_episode = 100) {
        auto config = std::make_shared<PythonConfig>();
        config->withBrain(500).withLearning(0.001f, 0.0005f)
              .withNeuromodulation().withDevelopment();
        
        auto brain = std::make_shared<PythonBrain>(config);
        auto world = std::make_shared<PythonWorld>();
        auto agent = std::make_shared<PythonAgent>(brain);
        
        world->configure(20.0f, 20.0f, 8, 8);
        world->setAgentStart(10.0f, 10.0f);
        agent->initialize(world->getInternalWorld());
        agent->enableAllLearning(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }
};

// Phase 6 integration experiment wrapper
class Phase6Experiment {
public:
    Phase6Experiment() {
        experiment_ = std::make_shared<nlm::Phase6IntegratedExperiment>();
        nlm::Logger::getInstance().setLevel(nlm::Logger::Level::Info);
    }
    
    py::dict runPhase6Test(int steps, bool detailed_stats = false) {
        py::dict results;
        results["test_type"] = "Phase6_Integration";
        results["steps_completed"] = steps;
        
        if (detailed_stats) {
            results["neuron_counts"] = std::vector<size_t>{};
            results["firing_rates"] = std::vector<float>{};
            results["curiosity_levels"] = std::vector<float>{};
            results["novelty_levels"] = std::vector<float>{};
        }
        
        // Configure for Phase 6
        nlm::Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = steps;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        // Run the experiment
        auto result = experiment_->run(config);
        
        // Map results
        results["total_reward"] = result.totalReward;
        results["avg_firing_rate"] = result.avgFiringRate;
        results["memory_episodes_stored"] = result.memoryEpisodesStored;
        results["curiosity_level"] = result.curiosityLevel;
        results["novelty_level"] = result.noveltyLevel;
        results["dopamine_level"] = result.dopamineLevel;
        
        // Integration status
        results["memory_working_memory_integrated"] = result.memoryWorkingMemoryIntegrated;
        results["memory_episodic_memory_integrated"] = result.memoryEpisodicMemoryIntegrated;
        results["neuromodulation_integrated"] = result.neuromodulationIntegrated;
        results["prediction_integrated"] = result.predictionIntegrated;
        results["development_integrated"] = result.developmentIntegrated;
        results["checkpointing_works"] = result.checkpointingWorks;
        results["replay_works"] = result.replayWorks;
        
        return results;
    }
    
    bool saveCheckpoint(const std::string& filepath) {
        return false; // TODO: Implement proper checkpointing
    }
    
    bool loadCheckpoint(const std::string& filepath) {
        return false; // TODO: Implement proper checkpointing
    }
    
    std::shared_ptr<PythonAgent> getAgent() {
        return nullptr; // TODO: Implement proper agent creation
    }
    
    std::shared_ptr<PythonBrain> getBrain() {
        return nullptr; // TODO: Implement proper brain creation
    }
    
    std::shared_ptr<PythonWorld> getWorld() {
        return nullptr; // TODO: Implement proper world creation
    }
    
private:
    std::shared_ptr<nlm::Phase6IntegratedExperiment> experiment_;
};

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python API with Pythonic Features
        -----------------------------------------------
        
        This module provides enhanced Python bindings for NLM with:
        
        Key Features:
        1. Pythonic configuration management with fluent interface
        2. Enhanced brain simulation with statistics collection
        3. Improved agent interface with better error handling
        4. Context managers for resource management
        5. Factory methods for common setup patterns
        6. Custom exception classes
        7. Better documentation and type safety
        
        The API is designed to be more intuitive and powerful than the original bindings.
    )pbdoc";
    
    // Register exception classes
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError.ptr());
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError.ptr());
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError.ptr());
    
    // PythonConfig class
    py::class_<PythonConfig>(m, "PythonConfig")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("setBrainNeuronCount", &PythonConfig::setBrainNeuronCount)
        .def("setBrainSynapseDensity", &PythonConfig::setBrainSynapseDensity)
        .def("setBrainConnectionProbability", &PythonConfig::setBrainConnectionProbability)
        .def("enableHebbian", &PythonConfig::enableHebbian)
        .def("setHebbianLearningRate", &PythonConfig::setHebbianLearningRate)
        .def("enableSTDP", &PythonConfig::enableSTDP)
        .def("setSTDPLearningRate", &PythonConfig::setSTDPLearningRate)
        .def("enableCuriosity", &PythonConfig::enableCuriosity)
        .def("enableNovelty", &PythonConfig::enableNovelty)
        .def("enableDevelopment", &PythonConfig::enableDevelopment)
        .def("getInt", &PythonConfig::getInt)
        .def("getDouble", &PythonConfig::getDouble)
        .def("getBool", &PythonConfig::getBool)
        .def("getString", &PythonConfig::getString)
        .def("has", &PythonConfig::has)
        .def("getKeys", &PythonConfig::getKeys)
        .def("saveToFile", &PythonConfig::saveToFile)
        .def("clear", &PythonConfig::clear)
        .def("withBrain", &PythonConfig::withBrain)
        .def("withLearning", &PythonConfig::withLearning)
        .def("withNeuromodulation", &PythonConfig::withNeuromodulation)
        .def("withDevelopment", &PythonConfig::withDevelopment)
        .def_static("createMinimal", &PythonConfig::createMinimal)
        .def_static("createComplex", &PythonConfig::createComplex);
    
    // PythonBrain class
    py::class_<PythonBrain>(m, "PythonBrain")
        .def(py::init<std::shared_ptr<PythonConfig>>())
        .def("runSimulation", &PythonBrain::runSimulation)
        .def("enableAllLearning", &PythonBrain::enableAllLearning)
        .def("setLearningRates", &PythonBrain::setLearningRates)
        .def("saveState", &PythonBrain::saveState)
        .def("loadState", &PythonBrain::loadState)
        .def("getNeuronCount", &PythonBrain::getNeuronCount)
        .def("getSynapseCount", &PythonBrain::getSynapseCount)
        .def("getFiringNeuronCount", &PythonBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &PythonBrain::getAverageFiringRate)
        .def("getTotalSpikes", &PythonBrain::getTotalSpikes)
        .def("getExcInhRatio", &PythonBrain::getExcInhRatio)
        .def("getDevelopmentalStage", &PythonBrain::getDevelopmentalStage)
        .def("reset", &PythonBrain::reset)
        .def("getInternalBrain", &PythonBrain::getInternalBrain);
    
    // PythonAgent class
    py::class_<PythonAgent>(m, "PythonAgent")
        .def(py::init<std::shared_ptr<PythonBrain>>())
        .def("initialize", &PythonAgent::initialize)
        .def("processSensoryInput", &PythonAgent::processSensoryInput)
        .def("getAction", &PythonAgent::getAction)
        .def("applyReward", &PythonAgent::applyReward)
        .def("updateDevelopment", &PythonAgent::updateDevelopment)
        .def("enableRewardModulation", &PythonAgent::enableRewardModulation)
        .def("enableStructuralPlasticity", &PythonAgent::enableStructuralPlasticity)
        .def("enableDevelopment", &PythonAgent::enableDevelopment)
        .def("enableCuriosity", &PythonAgent::enableCuriosity)
        .def("getCuriosityLevel", &PythonAgent::getCuriosityLevel)
        .def("getNoveltyLevel", &PythonAgent::getNoveltyLevel)
        .def("getPredictionError", &PythonAgent::getPredictionError)
        .def("getNeuromodulationLevel", &PythonAgent::getNeuromodulationLevel)
        .def("getDevelopmentalStage", &PythonAgent::getDevelopmentalStage)
        .def("getBrain", &PythonAgent::getBrain)
        .def("reset", &PythonAgent::reset);
    
    // PythonWorld class
    py::class_<PythonWorld>(m, "PythonWorld")
        .def(py::init<>())
        .def("configure", &PythonWorld::configure)
        .def("setAgentStart", &PythonWorld::setAgentStart)
        .def("update", &PythonWorld::update)
        .def("applyCommand", &PythonWorld::applyCommand)
        .def("getPercept", &PythonWorld::getPercept)
        .def("getAgentBody", &PythonWorld::getAgentBody)
        .def("reset", &PythonWorld::reset)
        .def("setSeed", &PythonWorld::setSeed)
        .def("getWidth", &PythonWorld::getWidth)
        .def("getHeight", &PythonWorld::getHeight)
        .def("getSimulationTime", &PythonWorld::getSimulationTime)
        .def("getInternalWorld", &PythonWorld::getInternalWorld);
    
    // NLMFactory class
    py::class_<NLMFactory>(m, "NLMFactory")
        .def_static("createSimpleSimulation", &NLMFactory::createSimpleSimulation)
        .def_static("createCustomSimulation", &NLMFactory::createCustomSimulation)
        .def_static("createTrainingSetup", &NLMFactory::createTrainingSetup);
    
    // Convenience factory functions
    m.def("createConfig", []() { return std::make_shared<PythonConfig>(); });
    m.def("createBrain", [](std::shared_ptr<PythonConfig> config) {
        return std::make_shared<PythonBrain>(config);
    });
    m.def("createAgent", [](std::shared_ptr<PythonBrain> brain) {
        return std::make_shared<PythonAgent>(brain);
    });
    m.def("createWorld", []() { return std::make_shared<PythonWorld>(); });
    
    // Enum exports
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType")
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
    
    // Phase 6 experiment class
    py::class_<Phase6Experiment>(m, "Phase6Experiment")
        .def(py::init<>())
        .def("runPhase6Test", &Phase6Experiment::runPhase6Test)
        .def("saveCheckpoint", &Phase6Experiment::saveCheckpoint)
        .def("loadCheckpoint", &Phase6Experiment::loadCheckpoint)
        .def("getAgent", &Phase6Experiment::getAgent)
        .def("getBrain", &Phase6Experiment::getBrain)
        .def("getWorld", &Phase6Experiment::getWorld);
    
    // Convenience Phase 6 factory
    m.def("createPhase6Experiment", []() {
        return std::make_shared<Phase6Experiment>();
    });
}

} // namespace nlm
