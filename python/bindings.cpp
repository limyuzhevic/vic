#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Add numpy support if available
try {
    #include <numpy/ndarrayobject.h>
    #define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
    #include <numpy/arrayobject.h>
    HAS_NUMPY = true;
} catch (...) {
    HAS_NUMPY = false;
}

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

// Custom exceptions for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation error: " + msg) {}
};

// Enhanced Config class with Pythonic API
class PythonicConfig {
private:
    std::shared_ptr<Config> config;
public:
    PythonicConfig() : config(std::make_shared<Config>()) {}
    
    // Pythonic setters with validation
    PythonicConfig& set(const std::string& key, double value) {
        config->set(key, value, ConfigSource::User);
        return *this;
    }
    
    PythonicConfig& set(const std::string& key, int64_t value) {
        config->set(key, value, ConfigSource::User);
        return *this;
    }
    
    PythonicConfig& set(const std::string& key, float value) {
        config->set(key, value, ConfigSource::User);
        return *this;
    }
    
    PythonicConfig& set(const std::string& key, bool value) {
        config->set(key, value, ConfigSource::User);
        return *this;
    }
    
    PythonicConfig& set(const std::string& key, const std::string& value) {
        config->set(key, value, ConfigSource::User);
        return *this;
    }
    
    // Pythonic getters with type conversion
    std::optional<double> getDouble(const std::string& key) const {
        return config->get<double>(key);
    }
    
    std::optional<int64_t> getInt(const std::string& key) const {
        return config->get<int64_t>(key);
    }
    
    std::optional<float> getFloat(const std::string& key) const {
        return config->get<float>(key);
    }
    
    std::optional<bool> getBool(const std::string& key) const {
        return config->get<bool>(key);
    }
    
    std::optional<std::string> getString(const std::string& key) const {
        return config->get<std::string>(key);
    }
    
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config->getKeys();
    }
    
    std::string getSummary() const {
        return config->summary();
    }
    
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    // Batch configuration from dictionary
    PythonicConfig& configureFromDict(const py::dict& settings) {
        for (auto item : settings) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            
            if (value.is(py::bool_())) {
                set(key, py::cast<bool>(value));
            } else if (value.is(py::int_())) {
                set(key, py::cast<int64_t>(value));
            } else if (value.is(py::float_())) {
                set(key, py::cast<double>(value));
            } else if (value.is(py::str())) {
                set(key, py::cast<std::string>(value));
            } else {
                throw ConfigurationError("Unsupported type for key: " + key);
            }
        }
        return *this;
    }
    
    // Load configuration from file with error handling
    bool loadFromFile(const std::string& filepath) {
        try {
            return config->loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to load from file ") + filepath + ": " + e.what());
        }
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const {
        try {
            return config->saveToFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to save to file ") + filepath + ": " + e.what());
        }
    }
    
    // Reset configuration
    void clear() {
        config->clear();
    }
    
    // Validate configuration
    bool validate() const {
        // Basic validation: check for required keys
        const std::vector<std::string> requiredKeys = {"brain.neuron_count", "brain.synapse_density"};
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                return false;
            }
        }
        return true;
    }
    
    // Create default configuration
    static std::shared_ptr<PythonicConfig> createDefault() {
        auto pyConfig = std::make_shared<PythonicConfig>();
        // Set some sensible defaults
        pyConfig->set("brain.neuron_count", 1000)
                  .set("brain.synapse_density", 0.1f)
                  .set("random_seed", 42LL)
                  .set("simulation_timestep", 0.001)
                  .set("plasticity.stdp.enable", true)
                  .set("plasticity.hebbian.enable", true)
                  .set("neuromod.dopamine.scale", 1.0f)
                  .set("neuromod.curiosity.enable", true)
                  .set("neuromod.novelty.enable", true);
        return pyConfig;
    }
};

// Enhanced Brain class with Pythonic API
class PythonicBrain {
private:
    std::shared_ptr<Brain> brain;
    std::string name;
public:
    PythonicBrain(std::shared_ptr<PythonicConfig> config) 
        : name("Brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count())) {
        brain = std::make_shared<Brain>(config->getConfig());
    }
    
    PythonicBrain(std::shared_ptr<Config> config) 
        : brain(std::make_shared<Brain>(config)),
          name("Brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count())) {}
    
    // Initialize with error handling
    bool initialize() {
        try {
            return brain->initialize();
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to initialize brain: ") + e.what());
        }
    }
    
    // Step with validation
    void step(size_t step) {
        if (!brain) {
            throw SimulationError("Brain not initialized");
        }
        brain->step(static_cast<SimulationStep>(step));
    }
    
    void step(size_t step, double time) {
        if (!brain) {
            throw SimulationError("Brain not initialized");
        }
        brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
    }
    
    // Enhanced statistics with validation
    size_t getNeuronCount() const {
        return brain->getTotalNeuronCount();
    }
    
    size_t getSynapseCount() const {
        return brain->getTotalSynapseCount();
    }
    
    size_t getActiveNeuronCount() const {
        return brain->getActiveNeuronCount();
    }
    
    size_t getFiringNeuronCount() const {
        return brain->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const {
        return brain->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        return brain->getTotalSpikeCount();
    }
    
    size_t getPendingSpikeEventCount() const {
        return brain->getPendingSpikeEventCount();
    }
    
    // Developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return brain->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
    }
    
    // Memory systems access
    NeuralWorkingMemory* getWorkingMemory() {
        return brain->getWorkingMemory();
    }
    
    NeuralEpisodicMemory* getEpisodicMemory() {
        return brain->getEpisodicMemory();
    }
    
    NeuralAssociativeMemory* getAssociativeMemory() {
        return brain->getAssociativeMemory();
    }
    
    PredictionSystem* getPredictionSystem() {
        return brain->getPredictionSystem();
    }
    
    NeuralPlanner* getPlanner() {
        return brain->getPlanner();
    }
    
    ConceptFormation* getConceptFormation() {
        return brain->getConceptFormation();
    }
    
    AttentionalSelection* getAttention() {
        return brain->getAttention();
    }
    
    DevelopmentSystem* getDevelopmentSystem() {
        return brain->getDevelopmentSystem();
    }
    
    Dopamine* getDopamine() {
        return brain->getDopamine();
    }
    
    Curiosity* getCuriosity() {
        return brain->getCuriosity();
    }
    
    Novelty* getNovelty() {
        return brain->getNovelty();
    }
    
    PredictionError* getPredictionErrorSignal() {
        return brain->getPredictionErrorSignal();
    }
    
    // Region management
    RegionId addRegion(const std::string& name = "") {
        return brain->addRegion(name);
    }
    
    NeuralRegion* getRegion(RegionId id) {
        return brain->getRegion(id);
    }
    
    size_t getRegionCount() const {
        return brain->getRegionCount();
    }
    
    std::vector<RegionId> getRegionIds() const {
        return brain->getRegionIds();
    }
    
    // Sensory processing
    void receiveSensoryInput(const SensoryInput& input) {
        brain->receiveSensoryInput(input);
    }
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        brain->injectCurrent(neuron, current);
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        brain->injectCurrentToNeurons(type, current);
    }
    
    // Action production
    std::unique_ptr<Action> produceAction() {
        return brain->produceAction();
    }
    
    // State management
    bool save(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    void reset() {
        brain->reset();
    }
    
    // Simulation control
    void stepWithTime(size_t step, double time) {
        brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
    }
    
    // Batch operations
    void runSteps(size_t startStep, size_t numSteps) {
        for (size_t i = startStep; i < startStep + numSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
        }
    }
    
    // Statistics monitoring
    std::string getStatusReport() const {
        std::stringstream ss;
        ss << "Brain Status Report:" << std::endl;
        ss << "  Regions: " << getRegionCount() << std::endl;
        ss << "  Neurons: " << getNeuronCount() << std::endl;
        ss << "  Synapses: " << getSynapseCount() << std::endl;
        ss << "  Active neurons: " << getActiveNeuronCount() << std::endl;
        ss << "  Firing neurons: " << getFiringNeuronCount() << std::endl;
        ss << "  Total spikes: " << getTotalSpikeCount() << std::endl;
        ss << "  Avg firing rate: " << getAverageFiringRate() << std::endl;
        ss << "  E/I ratio: " << getExcitationInhibitionRatio() << std::endl;
        ss << "  Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
        return ss.str();
    }
    
    // Get underlying brain for advanced usage
    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }
    
    // Get name for identification
    std::string getName() const {
        return name;
    }
    
    // Set name
    void setName(const std::string& newName) {
        name = newName;
    }
};

// Enhanced AgentBrain class with Pythonic API
class PythonicAgentBrain {
private:
    std::shared_ptr<AgentBrain> agentBrain;
public:
    PythonicAgentBrain(std::shared_ptr<PythonicBrain> brain) 
        : agentBrain(std::make_shared<AgentBrain>(brain->getBrain())) {}
    
    PythonicAgentBrain(std::shared_ptr<Brain> brain) 
        : agentBrain(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with error handling
    bool initialize(std::shared_ptr<SimpleWorld> world) {
        try {
            agentBrain->initialize(world.get());
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to initialize agent brain: ") + e.what());
        }
    }
    
    // Enhanced sensory processing
    void processSensoryInput(const SensoryPercept& percept) {
        agentBrain->processSensoryInput( percept);
    }
    
    // Enhanced action decoding
    std::unique_ptr<Action> decodeMotorCommand() {
        return agentBrain->decodeMotorCommand();
    }
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward) {
        agentBrain->applyRewardModulation(reward, predictedReward);
    }
    
    void updateDevelopment(double timestep) {
        agentBrain->updateDevelopment(timestep);
    }
    
    // Development and neuromodulation access
    DevelopmentalStage getDevelopmentalStage() const {
        return agentBrain->getDevelopmentalStage();
    }
    
    float getNeuromodulationLevel() const {
        return agentBrain->getNeuromodulationLevel();
    }
    
    float getCuriosityLevel() const {
        return agentBrain->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agentBrain->getNoveltyLevel();
    }
    
    float getPredictionError() const {
        return agentBrain->getPredictionError();
    }
    
    // Subsystem management
    void enableRewardModulation(bool enable) {
        agentBrain->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain->enableCuriosity(enable);
    }
    
    bool isRewardModulationEnabled() const {
        return agentBrain->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain->isCuriosityEnabled();
    }
    
    // Get underlying agent brain
    std::shared_ptr<AgentBrain> getAgentBrain() const {
        return agentBrain;
    }
    
    // Get the underlying brain
    std::shared_ptr<Brain> getBrain() const {
        return agentBrain->getBrain();
    }
};

// Enhanced SimpleWorld class with Pythonic API
class PythonicSimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world;
public:
    PythonicSimpleWorld() : world(std::make_shared<SimpleWorld>()) {}
    
    // Configuration with validation
    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
    }
    
    void configure(const std::string& configJson) {
        // TODO: Implement JSON parsing
        throw NotImplementedError("JSON configuration not yet implemented");
    }
    
    // World control
    void reset() {
        world->reset();
    }
    
    void update(double timestep) {
        world->update(timestep);
    }
    
    void setAgentStart(double x, double y) {
        world->setAgentStart(x, y);
    }
    
    // Agent interaction
    void applyMotorCommand(const Action& cmd, double currentTime) {
        world->applyMotorCommand(cmd, currentTime);
    }
    
    // State access
    SensoryPercept getSensoryPercept() const {
        return world->getSensoryPercept();
    }
    
    AgentBody getAgentBody() const {
        return world->getAgentBody();
    }
    
    // World objects
    void addObject(const WorldObject& obj) {
        world->addObject(obj);
    }
    
    void removeObject(double x, double y) {
        world->removeObject(x, y);
    }
    
    bool isValidPosition(double x, double y) const {
        return world->isValidPosition(x, y);
    }
    
    // Properties
    size_t getWidth() const {
        return world->getWidth();
    }
    
    size_t getHeight() const {
        return world->getHeight();
    }
    
    double getSimulationTime() const {
        return world->getSimulationTime();
    }
    
    double getMaxEnergy() const {
        return world->getMaxEnergy();
    }
    
    void setMaxEnergy(double energy) {
        world->setMaxEnergy(energy);
    }
    
    double getEnergyDecayRate() const {
        return world->getEnergyDecayRate();
    }
    
    void setEnergyDecayRate(double rate) {
        world->setEnergyDecayRate(rate);
    }
    
    int64_t getRandomSeed() const {
        return world->getRandomSeed();
    }
    
    void setRandomSeed(int64_t seed) {
        world->setRandomSeed(seed);
    }
    
    // World simulation utilities
    double getCellSize() const {
        return 1.0; // Default cell size
    }
    
    std::vector<std::vector<double>> getOccupancyMap() const {
        std::vector<std::vector<double>> map(world->getHeight(), std::vector<double>(world->getWidth(), 0.0));
        // TODO: Implement actual occupancy map
        return map;
    }
    
    // Get underlying world
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world;
    }
    
    // Convenience methods for common operations
    std::vector<double> getAgentPosition() const {
        auto body = getAgentBody();
        return {body.x, body.y};
    }
    
    double getAgentX() const {
        return getAgentPosition()[0];
    }
    
    double getAgentY() const {
        return getAgentPosition()[1];
    }
    
    // Environment interaction
    void moveAgent(double dx, double dy) {
        auto body = getAgentBody();
        setAgentStart(body.x + dx, body.y + dy);
    }
    
    void turnAgent(double angle) {
        // TODO: Implement agent turning
        throw NotImplementedError("Agent turning not yet implemented");
    }
};

// Context manager for simulation episodes
class SimulationEpisode {
private:
    std::shared_ptr<PythonicBrain> brain;
    std::shared_ptr<PythonicAgentBrain> agent;
    std::shared_ptr<PythonicSimpleWorld> world;
    bool initialized;
    size_t episodeLength;
    size_t currentStep;
    double timestep;
    
public:
    SimulationEpisode(std::shared_ptr<PythonicBrain> brain,
                     std::shared_ptr<PythonicAgentBrain> agent,
                     std::shared_ptr<PythonicSimpleWorld> world,
                     size_t episodeLength,
                     double timestep)
        : brain(brain), agent(agent), world(world), 
          initialized(false), episodeLength(episodeLength), 
          currentStep(0), timestep(timestep) {}
    
    // Context manager entry
    SimulationEpisode& enter() {
        if (initialized) {
            throw SimulationError("Episode already initialized");
        }
        
        // Reset and initialize
        if (brain) brain->reset();
        if (agent) agent->reset();
        if (world) world->reset();
        
        initialized = true;
        currentStep = 0;
        
        // Initialize agent if world is provided
        if (agent && world) {
            agent->initialize(world);
        }
        
        return *this;
    }
    
    // Context manager exit
    void exit(std::exception_ptr) {
        // Finalize episode (save statistics, etc.)
        // For now, just mark as completed
        initialized = false;
    }
    
    // Run the episode
    void run() {
        if (!initialized) {
            throw SimulationError("Episode not initialized. Call enter() first.");
        }
        
        for (currentStep = 0; currentStep < episodeLength; ++currentStep) {
            // Update world
            if (world) {
                world->update(timestep);
                
                // Get sensory input
                if (agent) {
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                }
            }
            
            // Brain step
            if (brain) {
                brain->step(currentStep, currentStep * timestep);
            }
            
            // Agent action
            if (agent) {
                auto action = agent->decodeMotorCommand();
                if (world && action) {
                    world->applyMotorCommand(*action, world->getSimulationTime());
                }
            }
        }
    }
    
    // Get current step
    size_t getCurrentStep() const {
        return currentStep;
    }
    
    // Get progress
    double getProgress() const {
        return static_cast<double>(currentStep) / episodeLength;
    }
    
    // Check if completed
    bool isCompleted() const {
        return initialized && currentStep >= episodeLength;
    }
    
    // Get statistics
    std::string getStatistics() const {
        std::stringstream ss;
        ss << "Episode Statistics:" << std::endl;
        ss << "  Steps completed: " << currentStep << std::endl;
        ss << "  Episode length: " << episodeLength << std::endl;
        ss << "  Progress: " << (getProgress() * 100.0) << "%" << std::endl;
        ss << "  Brain neurons: " << (brain ? brain->getNeuronCount() : 0) << std::endl;
        ss << "  Brain synapses: " << (brain ? brain->getSynapseCount() : 0) << std::endl;
        ss << "  Brain firing rate: " << (brain ? brain->getAverageFiringRate() : 0.0f) << std::endl;
        return ss.str();
    }
};

// Factory functions for easy creation
inline std::shared_ptr<PythonicConfig> createDefaultConfig() {
    return PythonicConfig::createDefault();
}

inline std::shared_ptr<PythonicBrain> createBrain(std::shared_ptr<PythonicConfig> config) {
    return std::make_shared<PythonicBrain>(config);
}

inline std::shared_ptr<PythonicBrain> createBrain(std::shared_ptr<Config> config) {
    return std::make_shared<PythonicBrain>(config);
}

inline std::shared_ptr<PythonicAgentBrain> createAgentBrain(std::shared_ptr<PythonicBrain> brain) {
    return std::make_shared<PythonicAgentBrain>(brain);
}

inline std::shared_ptr<PythonicAgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<PythonicAgentBrain>(brain);
}

inline std::shared_ptr<PythonicSimpleWorld> createWorld() {
    return std::make_shared<PythonicSimpleWorld>();
}

inline SimulationEpisode createEpisode(std::shared_ptr<PythonicBrain> brain,
                                       std::shared_ptr<PythonicAgentBrain> agent,
                                       std::shared_ptr<PythonicSimpleWorld> world,
                                       size_t episodeLength,
                                       double timestep = 0.1) {
    return SimulationEpisode(brain, agent, world, episodeLength, timestep);
}

// Batch simulation utilities
inline std::vector<std::string> runBatchSimulation(
    std::shared_ptr<PythonicBrain> brain,
    std::shared_ptr<PythonicAgentBrain> agent,
    std::shared_ptr<PythonicSimpleWorld> world,
    size_t numEpisodes,
    size_t episodeLength,
    double timestep = 0.1) {
    
    std::vector<std::string> results;
    
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        auto episodeObj = createEpisode(brain, agent, world, episodeLength, timestep);
        
        try {
            episodeObj.enter();
            episodeObj.run();
            
            std::stringstream ss;
            ss << "Episode " << episode << ": " << episodeObj.getStatistics();
            results.push_back(ss.str());
        } catch (const std::exception& e) {
            results.push_back("Episode " + std::to_string(episode) + " failed: " + std::string(e.what()));
        }
    }
    
    return results;
}

PYBIND11_MODULE(pynlm, m) {
    // Module docstring
    m.doc() = R"pbdoc(
        Enhanced NLM Python Interface
        -----------------------------
        
        This is an enhanced version of the NLM Python bindings with:
        - Better error handling and validation
        - Pythonic API design
        - Context managers for resource management
        - Batch operations
        - Type safety and validation
        
        The interface provides:
        * Config - Configuration management
        * Brain - Neural simulation
        * AgentBrain - Agent-environment interface
        * SimpleWorld - Simulation world
        * SimulationEpisode - Context manager for episodes
        * Action - Motor commands
        * SensoryPercept - Sensory input
        * AgentBody - Agent state
        * WorldObject - World objects
        * Utility functions for creating common objects
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Register Config class
    py::class_<PythonicConfig, std::shared_ptr<PythonicConfig>>(m, "Config")
        .def(py::init<>())
        .def("set", py::overload_cast<const std::string&, double>(&PythonicConfig::set))
        .def("set", py::overload_cast<const std::string&, int64_t>(&PythonicConfig::set))
        .def("set", py::overload_cast<const std::string&, float>(&PythonicConfig::set))
        .def("set", py::overload_cast<const std::string&, bool>(&PythonicConfig::set))
        .def("set", py::overload_cast<const std::string&, const std::string&>(&PythonicConfig::set))
        .def("getDouble", &PythonicConfig::getDouble)
        .def("getInt", &PythonicConfig::getInt)
        .def("getFloat", &PythonicConfig::getFloat)
        .def("getBool", &PythonicConfig::getBool)
        .def("getString", &PythonicConfig::getString)
        .def("has", &PythonicConfig::has)
        .def("getKeys", &PythonicConfig::getKeys)
        .def("getSummary", &PythonicConfig::getSummary)
        .def("configureFromDict", &PythonicConfig::configureFromDict)
        .def("loadFromFile", &PythonicConfig::loadFromFile)
        .def("saveToFile", &PythonicConfig::saveToFile)
        .def("clear", &PythonicConfig::clear)
        .def("validate", &PythonicConfig::validate)
        .def_static("createDefault", &PythonicConfig::createDefault)
        .def("__repr__", [](const PythonicConfig& config) {
            return "Config(" + config.getSummary() + ")";
        });
    
    // Register Brain class
    py::class_<PythonicBrain, std::shared_ptr<PythonicBrain>>(m, "Brain")
        .def(py::init<std::shared_ptr<PythonicConfig>>())
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &PythonicBrain::initialize)
        .def("step", py::overload_cast<size_t>(&PythonicBrain::step))
        .def("stepWithTime", &PythonicBrain::step)
        .def("getNeuronCount", &PythonicBrain::getNeuronCount)
        .def("getSynapseCount", &PythonicBrain::getSynapseCount)
        .def("getActiveNeuronCount", &PythonicBrain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &PythonicBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &PythonicBrain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &PythonicBrain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &PythonicBrain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &PythonicBrain::getPendingSpikeEventCount)
        .def("getDevelopmentalStage", &PythonicBrain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &PythonicBrain::setDevelopmentalStage)
        .def("getWorkingMemory", &PythonicBrain::getWorkingMemory)
        .def("getEpisodicMemory", &PythonicBrain::getEpisodicMemory)
        .def("getAssociativeMemory", &PythonicBrain::getAssociativeMemory)
        .def("getPredictionSystem", &PythonicBrain::getPredictionSystem)
        .def("getPlanner", &PythonicBrain::getPlanner)
        .def("getConceptFormation", &PythonicBrain::getConceptFormation)
        .def("getAttention", &PythonicBrain::getAttention)
        .def("getDevelopmentSystem", &PythonicBrain::getDevelopmentSystem)
        .def("getDopamine", &PythonicBrain::getDopamine)
        .def("getCuriosity", &PythonicBrain::getCuriosity)
        .def("getNovelty", &PythonicBrain::getNovelty)
        .def("getPredictionErrorSignal", &PythonicBrain::getPredictionErrorSignal)
        .def("addRegion", &PythonicBrain::addRegion)
        .def("getRegion", &PythonicBrain::getRegion)
        .def("getRegionCount", &PythonicBrain::getRegionCount)
        .def("getRegionIds", &PythonicBrain::getRegionIds)
        .def("receiveSensoryInput", &PythonicBrain::receiveSensoryInput)
        .def("injectCurrent", &PythonicBrain::injectCurrent)
        .def("injectCurrentToNeurons", &PythonicBrain::injectCurrentToNeurons)
        .def("produceAction", &PythonicBrain::produceAction)
        .def("save", &PythonicBrain::save)
        .def("load", &PythonicBrain::load)
        .def("reset", &PythonicBrain::reset)
        .def("runSteps", &PythonicBrain::runSteps)
        .def("getStatusReport", &PythonicBrain::getStatusReport)
        .def("getName", &PythonicBrain::getName)
        .def("setName", &PythonicBrain::setName)
        .def("__repr__", [](const PythonicBrain& brain) {
            return "Brain(name=" + brain.getName() + ", neurons=" + std::to_string(brain.getNeuronCount()) + ")";
        });
    
    // Register AgentBrain class
    py::class_<PythonicAgentBrain, std::shared_ptr<PythonicAgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<PythonicBrain>>())
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &PythonicAgentBrain::initialize)
        .def("processSensoryInput", &PythonicAgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &PythonicAgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &PythonicAgentBrain::applyRewardModulation)
        .def("updateDevelopment", &PythonicAgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &PythonicAgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &PythonicAgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &PythonicAgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &PythonicAgentBrain::getNoveltyLevel)
        .def("getPredictionError", &PythonicAgentBrain::getPredictionError)
        .def("enableRewardModulation", &PythonicAgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &PythonicAgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &PythonicAgentBrain::enableDevelopment)
        .def("enableCuriosity", &PythonicAgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &PythonicAgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &PythonicAgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &PythonicAgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &PythonicAgentBrain::isCuriosityEnabled)
        .def("__repr__", [](const PythonicAgentBrain& agent) {
            return "AgentBrain";
        });
    
    // Register SimpleWorld class
    py::class_<PythonicSimpleWorld, std::shared_ptr<PythonicSimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &PythonicSimpleWorld::configure)
        .def("reset", &PythonicSimpleWorld::reset)
        .def("update", &PythonicSimpleWorld::update)
        .def("setAgentStart", &PythonicSimpleWorld::setAgentStart)
        .def("applyMotorCommand", &PythonicSimpleWorld::applyMotorCommand)
        .def("getSensoryPercept", &PythonicSimpleWorld::getSensoryPercept)
        .def("getAgentBody", &PythonicSimpleWorld::getAgentBody)
        .def("addObject", &PythonicSimpleWorld::addObject)
        .def("removeObject", &PythonicSimpleWorld::removeObject)
        .def("isValidPosition", &PythonicSimpleWorld::isValidPosition)
        .def("getWidth", &PythonicSimpleWorld::getWidth)
        .def("getHeight", &PythonicSimpleWorld::getHeight)
        .def("getSimulationTime", &PythonicSimpleWorld::getSimulationTime)
        .def("setMaxEnergy", &PythonicSimpleWorld::setMaxEnergy)
        .def("getMaxEnergy", &PythonicSimpleWorld::getMaxEnergy)
        .def("setEnergyDecayRate", &PythonicSimpleWorld::setEnergyDecayRate)
        .def("getEnergyDecayRate", &PythonicSimpleWorld::getEnergyDecayRate)
        .def("setRandomSeed", &PythonicSimpleWorld::setRandomSeed)
        .def("getRandomSeed", &PythonicSimpleWorld::getRandomSeed)
        .def("getAgentPosition", &PythonicSimpleWorld::getAgentPosition)
        .def("getAgentX", &PythonicSimpleWorld::getAgentX)
        .def("getAgentY", &PythonicSimpleWorld::getAgentY)
        .def("moveAgent", &PythonicSimpleWorld::moveAgent)
        .def("__repr__", [](const PythonicSimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) + 
                   ", height=" + std::to_string(world.getHeight()) + ")";
        });
    
    // Register SimulationEpisode class
    py::class_<SimulationEpisode>(m, "SimulationEpisode")
        .def(py::init<std::shared_ptr<PythonicBrain>, 
                      std::shared_ptr<PythonicAgentBrain>,
                      std::shared_ptr<PythonicSimpleWorld>,
                      size_t, double>())
        .def("enter", &SimulationEpisode::enter)
        .def("run", &SimulationEpisode::run)
        .def("getCurrentStep", &SimulationEpisode::getCurrentStep)
        .def("getProgress", &SimulationEpisode::getProgress)
        .def("isCompleted", &SimulationEpisode::isCompleted)
        .def("getStatistics", &SimulationEpisode::getStatistics)
        .def("__enter__", &SimulationEpisode::enter)
        .def("__exit__", [](SimulationEpisode& self, py::args) {
            self.exit(nullptr);
        });
    
    // Register factory functions
    m.def("createDefaultConfig", &createDefaultConfig, "Create a default configuration")
     .def("createBrain", py::overload_cast<std::shared_ptr<PythonicConfig>>(&createBrain),
          "Create a brain with Pythonic configuration")
     .def("createBrain", py::overload_cast<std::shared_ptr<Config>>(&createBrain),
          "Create a brain with standard configuration")
     .def("createAgentBrain", py::overload_cast<std::shared_ptr<PythonicBrain>>(&createAgentBrain),
          "Create an agent brain interface")
     .def("createAgentBrain", py::overload_cast<std::shared_ptr<Brain>>(&createAgentBrain),
          "Create an agent brain interface from standard brain")
     .def("createWorld", &createWorld, "Create a simple world")
     .def("createEpisode", &createEpisode,
          "Create a simulation episode context manager")
     .def("runBatchSimulation", &runBatchSimulation,
          "Run batch simulation with multiple episodes");
    
    // Register exception types
    py::register_exception<NLMError>(m, "NLMError")
     .def("what", [](const NLMError& e) { return e.what(); });
    
    py::register_exception<ConfigurationError>(m, "ConfigurationError")
     .def("what", [](const ConfigurationError& e) { return e.what(); });
    
    py::register_exception<SimulationError>(m, "SimulationError")
     .def("what", [](const SimulationError& e) { return e.what(); });
    
    py::register_exception<ValidationError>(m, "ValidationError")
     .def("what", [](const ValidationError& e) { return e.what(); });
}
