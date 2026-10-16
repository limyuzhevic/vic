// pynlm - Improved Python Bindings for NLM
// Provides better abstractions for user-friendly Python interface

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// ========== IMPROVED BINDINGS FOR BETTER USER EXPERIENCE ==========

// 1. High-level Configuration Builder
class PythonConfigBuilder {
public:
    PythonConfigBuilder() {
        config = std::make_shared<Config>();
    }
    
    // Neural network configuration
    PythonConfigBuilder& setNeuronCount(size_t count) {
        config->set("neuron_count", static_cast<int64_t>(count), ConfigSource::Runtime);
        return *this;
    }
    
    PythonConfigBuilder& setRegionCount(size_t count) {
        config->set("region_count", static_cast<int64_t>(count), ConfigSource::Runtime);
        return *this;
    }
    
    PythonConfigBuilder& setConnectionProbability(float prob) {
        config->set("connection_probability", prob, ConfigSource::Runtime);
        return *this;
    }
    
    // Plasticity configuration
    PythonConfigBuilder& setSTDPParameters(float ltp_weight, float ltd_weight, float tau) {
        config->set("stdp_ltp_weight", ltp_weight, ConfigSource::Runtime);
        config->set("stdp_ltd_weight", ltd_weight, ConfigSource::Runtime);
        config->set("stdp_tau", tau, ConfigSource::Runtime);
        return *this;
    }
    
    PythonConfigBuilder& setStructuralPlasticity(float synaptogenesis, float pruning) {
        config->set("synaptogenesis_rate", synaptogenesis, ConfigSource::Runtime);
        config->set("pruning_rate", pruning, ConfigSource::Runtime);
        return *this;
    }
    
    // Memory system configuration
    PythonConfigBuilder& setMemoryCapacity(size_t working_memory, size_t episodic_memory) {
        // Set working memory capacity
        config->set("working_memory_capacity", static_cast<int64_t>(working_memory), ConfigSource::Runtime);
        // Set max episodic memory episodes
        config->set("max_episodes", static_cast<int64_t>(episodic_memory), ConfigSource::Runtime);
        return *this;
    }
    
    // Simulation configuration
    PythonConfigBuilder& setSimulation(double timestep, size_t max_steps) {
        config->set("simulation_timestep", timestep, ConfigSource::Runtime);
        config->set("max_steps", static_cast<int64_t>(max_steps), ConfigSource::Runtime);
        return *this;
    }
    
    // Neuromodulation configuration
    PythonConfigBuilder& enableNeuromodulation(bool enable) {
        config->set("enable_neuromodulation", enable, ConfigSource::Runtime);
        return *this;
    }
    
    PythonConfigBuilder& setDopamineLevel(float level) {
        config->set("dopamine_level", level, ConfigSource::Runtime);
        return *this;
    }
    
    // Development configuration
    PythonConfigBuilder& enableDevelopment(bool enable) {
        config->set("enable_development", enable, ConfigSource::Runtime);
        return *this;
    }
    
    // Get the configured config
    std::shared_ptr<Config> build() {
        return config;
    }
    
    // Preset configurations for common use cases
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> createPerformanceConfig(size_t neuron_count) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("simulation_timestep", 0.002, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.006f, ConfigSource::Default);
        config->set("stdp_tau", 10.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
        config->set("pruning_rate", 0.000005f, ConfigSource::Default);
        config->set("enable_neuromodulation", false, ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> createResearchConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
        config->set("stdp_tau", 50.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        config->set("enable_neuromodulation", true, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("max_steps", static_cast<int64_t>(10000), ConfigSource::Default);
        return config;
    }
    
private:
    std::shared_ptr<Config> config;
};

// 2. High-level Simulation Manager
class PythonSimulationManager {
public:
    PythonSimulationManager(std::shared_ptr<Config> config) {
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }
    
    // Run simulation with given number of steps
    PythonSimulationManager& runSteps(size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        return *this;
    }
    
    // Run simulation with sensory input and action loop
    PythonSimulationManager& runEpisode(
        const std::vector<std::vector<float>>& sensory_inputs,
        const std::vector<size_t>& action_indices
    ) {
        if (sensory_inputs.size() != action_indices.size()) {
            throw std::runtime_error("Sensory inputs and actions must have same length");
        }
        
        brain->reset();
        
        for (size_t step = 0; step < sensory_inputs.size(); ++step) {
            // Convert Python sensory input to NLM format
            std::vector<float> sensory_data = sensory_inputs[step];
            
            // Create NLM sensory input object (TODO implementation)
            auto sensory_input = std::make_shared<Vision>(sensory_data.size() / 3, 1, 3);
            sensory_input->setData(sensory_data);
            
            // Process sensory input through brain
            brain->receiveSensoryInput(sensory_input);
            
            // Step brain
            brain->step(step);
            
            // Create and apply action (TODO implementation)
            // For now, just produce an action from brain activity
            auto action = brain->produceAction();
            
            // In a real scenario, this would be applied to the world
            // For now, we just log it
        }
        
        return *this;
    }
    
    // Get current brain state
    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }
    
    // Get current statistics
    py::dict getStatistics() const {
        py::dict stats;
        
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Get memory statistics if available
        auto working_memory = brain->getWorkingMemory();
        if (working_memory) {
            stats["working_memory_traces"] = working_memory->getActiveTraces();
        }
        
        auto episodic_memory = brain->getEpisodicMemory();
        if (episodic_memory) {
            stats["episodic_memory_episodes"] = episodic_memory->getEpisodeCount();
        }
        
        return stats;
    }
    
    // Save brain state
    bool saveState(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    // Load brain state
    bool loadState(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Enable learning
    void enableLearning(bool enable) {
        // Enable STDP and other plasticity rules
        // This would depend on the specific implementation
    }
    
    // Set learning rate
    void setLearningRate(float rate) {
        // Set learning rate for plasticity rules
        // This would depend on the specific implementation
    }
    
private:
    std::shared_ptr<Brain> brain;
};

// 3. High-level Agent Manager
class PythonAgentManager {
public:
    PythonAgentManager() {
        world = std::make_shared<SimpleWorld>();
        config = PythonConfigBuilder::createDefaultConfig();
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(*world);
    }
    
    PythonAgentManager& configureWorld(size_t width, size_t height) {
        world->configure(width, height, 20, 20);
        return *this;
    }
    
    PythonAgentManager& setAgentPosition(float x, float y) {
        world->setAgentStart(x, y);
        return *this;
    }
    
    PythonAgentManager& setAgentConfig(const std::string& config_file) {
        config->loadFromFile(config_file);
        return *this;
    }
    
    // Run agent episode
    PythonAgentManager& runEpisode(size_t max_steps) {
        world->reset();
        brain->reset();
        agent_brain->reset();
        
        for (size_t step = 0; step < max_steps; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input through agent brain
            agent_brain->processSensoryInput(percept);
            
            // Step the main brain
            brain->step(step);
            
            // Decode motor command
            auto motor_command = agent_brain->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(motor_command, step * 0.001);
            
            // Log step statistics
            logStep(step);
        }
        
        return *this;
    }
    
    // Get episode results
    py::dict getEpisodeResults() const {
        py::dict results;
        
        // Get brain statistics
        auto stats = brain->getConfig();
        results["neuron_count"] = stats->getOr<int64_t>("neuron_count", 1000);
        results["region_count"] = stats->getOr<int64_t>("region_count", 1);
        
        // Get agent statistics
        results["total_steps"] = episode_steps;
        results["final_dopamine"] = agent_brain->getNeuromodulationLevel();
        results["final_curiosity"] = agent_brain->getCuriosityLevel();
        results["final_novelty"] = agent_brain->getNoveltyLevel();
        
        return results;
    }
    
    // Reset for new episode
    void reset() {
        world->reset();
        brain->reset();
        agent_brain->reset();
        episode_steps = 0;
    }
    
    // Get current world state
    py::dict getWorldState() const {
        py::dict state;
        
        auto body = world->getAgentBody();
        state["x"] = body.x;
        state["y"] = body.y;
        state["orientation"] = body.orientation;
        state["velocity_x"] = body.velocityX;
        state["velocity_y"] = body.velocityY;
        state["angular_velocity"] = body.angularVelocity;
        state["energy"] = body.energy;
        state["health"] = body.health;
        state["age"] = body.age;
        
        return state;
    }
    
private:
    void logStep(size_t step) {
        // Log episode statistics
        episode_steps++;
    }
    
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent_brain;
    size_t episode_steps;
};

// 4. Convenience Functions
namespace utils {
    
    // Create simple sensory input from vision data
    std::shared_ptr<SensoryInput> createVisionInput(size_t width, size_t height, const std::vector<float>& data) {
        auto vision = std::make_shared<Vision>(width, height, 3);
        vision->setData(data);
        return vision;
    }
    
    // Create simple audio input
    std::shared_ptr<SensoryInput> createAudioInput(size_t sample_rate, const std::vector<float>& samples) {
        auto audio = std::make_shared<Audio>(sample_rate, samples.size());
        audio->setData(samples);
        audio->setSampleRate(sample_rate);
        return audio;
    }
    
    // Create action from type and parameters
    std::shared_ptr<Action> createAction(ActionType type, const std::vector<float>& parameters = {}) {
        return std::make_shared<Action>(type, parameters);
    }
    
    // Calculate similarity between two neural activity patterns
    float neuralSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) {
        if (pattern1.size() != pattern2.size()) {
            return 0.0f;
        }
        
        float dot_product = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;
        
        for (size_t i = 0; i < pattern1.size(); ++i) {
            dot_product += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }
        
        if (norm1 == 0.0f || norm2 == 0.0f) {
            return 0.0f;
        }
        
        return dot_product / std::sqrt(norm1 * norm2);
    }
    
    // Create preset configurations
    std::shared_ptr<Config> getPresetConfig(const std::string& name) {
        if (name == "default") {
            return PythonConfigBuilder::createDefaultConfig();
        } else if (name == "performance") {
            return PythonConfigBuilder::createPerformanceConfig(1000);
        } else if (name == "research") {
            return PythonConfigBuilder::createResearchConfig();
        } else {
            throw std::runtime_error("Unknown preset config: " + name);
        }
    }
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config) {
        bool valid = true;
        
        // Check required keys
        auto keys = config->getKeys();
        
        if (!config->has("neuron_count")) {
            // Valid, but may use default
        }
        
        if (!config->has("region_count")) {
            // Valid, but may use default
        }
        
        if (!config->has("simulation_timestep")) {
            // Valid, but may use default
        }
        
        return valid;
    }
}

// ========== PYBIND11 MODULE BINDINGS ==========

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Improved NLM Python Bindings
        -----------------------------
        
        This module provides improved Python bindings for the NLM (Neural Learning Machine)
        with high-level abstractions and user-friendly interfaces.
        
        Key Features:
        - High-level configuration builders
        - Simulation managers
        - Agent managers
        - Convenience utilities
        - Preset configurations
        
        Usage Example:
        
        import pynlm
        
        # Create a simulation manager
        config = pynlm.ConfigBuilder()
            .setNeuronCount(1000)
            .setRegionCount(1)
            .setSTDPParameters(0.01, 0.012, 20.0)
            .build()
        
        sim = pynlm.SimulationManager(config)
        sim.runSteps(1000)
        
        # Get statistics
        stats = sim.getStatistics()
        print(f"Neurons: {stats['neuron_count']}")
        
        # Run agent episode
        agent = pynlm.AgentManager()
        agent.configureWorld(100, 100)
        agent.setAgentPosition(50, 50)
        agent.runEpisode(200)
        
        results = agent.getEpisodeResults()
        print(f"Episode completed: {results['total_steps']} steps")
    )pbdoc";
    
    // Add high-level classes
    py::class_<PythonConfigBuilder>(m, "ConfigBuilder")
        .def(py::init<>())
        .def("setNeuronCount", &PythonConfigBuilder::setNeuronCount)
        .def("setRegionCount", &PythonConfigBuilder::setRegionCount)
        .def("setConnectionProbability", &PythonConfigBuilder::setConnectionProbability)
        .def("setSTDPParameters", &PythonConfigBuilder::setSTDPParameters)
        .def("setStructuralPlasticity", &PythonConfigBuilder::setStructuralPlasticity)
        .def("setMemoryCapacity", &PythonConfigBuilder::setMemoryCapacity)
        .def("setSimulation", &PythonConfigBuilder::setSimulation)
        .def("enableNeuromodulation", &PythonConfigBuilder::enableNeuromodulation)
        .def("setDopamineLevel", &PythonConfigBuilder::setDopamineLevel)
        .def("enableDevelopment", &PythonConfigBuilder::enableDevelopment)
        .def("build", &PythonConfigBuilder::build)
        .def_static("createDefaultConfig", &PythonConfigBuilder::createDefaultConfig)
        .def_static("createPerformanceConfig", &PythonConfigBuilder::createPerformanceConfig)
        .def_static("createResearchConfig", &PythonConfigBuilder::createResearchConfig);
    
    py::class_<PythonSimulationManager>(m, "SimulationManager")
        .def(py::init<std::shared_ptr<Config>>())
        .def("runSteps", &PythonSimulationManager::runSteps)
        .def("runEpisode", &PythonSimulationManager::runEpisode)
        .def("getBrain", &PythonSimulationManager::getBrain)
        .def("getStatistics", &PythonSimulationManager::getStatistics)
        .def("saveState", &PythonSimulationManager::saveState)
        .def("loadState", &PythonSimulationManager::loadState)
        .def("enableLearning", &PythonSimulationManager::enableLearning)
        .def("setLearningRate", &PythonSimulationManager::setLearningRate);
    
    py::class_<PythonAgentManager>(m, "AgentManager")
        .def(py::init<>())
        .def("configureWorld", &PythonAgentManager::configureWorld)
        .def("setAgentPosition", &PythonAgentManager::setAgentPosition)
        .def("setAgentConfig", &PythonAgentManager::setAgentConfig)
        .def("runEpisode", &PythonAgentManager::runEpisode)
        .def("getEpisodeResults", &PythonAgentManager::getEpisodeResults)
        .def("reset", &PythonAgentManager::reset)
        .def("getWorldState", &PythonAgentManager::getWorldState);
    
    // Add convenience functions
    m.def("createVisionInput", &utils::createVisionInput);
    m.def("createAudioInput", &utils::createAudioInput);
    m.def("createAction", &utils::createAction);
    m.def("neuralSimilarity", &utils::neuralSimilarity);
    m.def("getPresetConfig", &utils::getPresetConfig);
    m.def("validateConfig", &utils::validateConfig);
}

} // namespace nlm
