#pragma once

#include "core/Constants.hpp"
#include "core/ConfigFactory.hpp"
#include <string>
#include <vector>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace py = pybind11;

namespace nlm {

// Python-enhanced Configuration Factory
class EnhancedConfigFactory {
public:
    // Create default configuration with optional parameters
    static std::shared_ptr<Config> createConfig(
        size_t neuronCount = Constants::BRAIN_DEFAULT_NEURON_COUNT,
        float connectionProbability = Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
        float simulationTimestep = Constants::DEFAULT_TIMESTEP,
        int randomSeed = Constants::SIMULATION_DEFAULT_RANDOM_SEED,
        const std::string& preset = "default"
    ) {
        auto config = createConfig(neuronCount, connectionProbability, simulationTimestep, randomSeed);
        
        if (preset != "default") {
            applyPreset(config, preset);
        }
        
        return config;
    }
    
    // Create configuration from JSON string
    static std::shared_ptr<Config> createConfigFromJson(const std::string& json) {
        auto config = createDefaultConfig();
        
        // Simple JSON parsing (placeholder for real implementation)
        // In production, use a proper JSON library
        parseSimpleJson(json, config);
        
        return config;
    }
    
    // Create configuration builder for fluent API
    class ConfigBuilder {
    public:
        ConfigBuilder() : config(createDefaultConfig()) {}
        
        ConfigBuilder& setNeuronCount(size_t count) {
            config->set(ConfigDefaults::BRAIN_NEURON_COUNT, 
                       static_cast<int64_t>(count), ConfigSource::Runtime);
            return *this;
        }
        
        ConfigBuilder& setConnectionProbability(float prob) {
            config->set(ConfigDefaults::BRAIN_CONNECTION_PROBABILITY, 
                       prob, ConfigSource::Runtime);
            return *this;
        }
        
        ConfigBuilder& setSimulationTimestep(float timestep) {
            config->set(ConfigDefaults::SIMULATION_TIMESTEP, 
                       timestep, ConfigSource::Runtime);
            return *this;
        }
        
        ConfigBuilder& setRandomSeed(int seed) {
            config->set(ConfigDefaults::RANDOM_SEED, 
                       seed, ConfigSource::Runtime);
            return *this;
        }
        
        ConfigBuilder& enablePlasticity(bool enable) {
            config->set(ConfigDefaults::PLASTICITY_STDP_ENABLE, 
                       enable, ConfigSource::Runtime);
            return *this;
        }
        
        ConfigBuilder& setLearningRate(float rate) {
            config->set(ConfigDefaults::PLASTICITY_STDP_LEARNING_RATE, 
                       rate, ConfigSource::Runtime);
            return *this;
        }
        
        ConfigBuilder& preset(const std::string& presetName) {
            applyPreset(config, presetName);
            return *this;
        }
        
        std::shared_ptr<Config> build() {
            return config;
        }
        
    private:
        std::shared_ptr<Config> config;
    };
    
    static ConfigBuilder builder() {
        return ConfigBuilder();
    }
    
    // Get all available presets
    static std::vector<std::string> getAvailablePresets() {
        return {"default", "fast", "detailed", "learning", "stable"};
    }
    
    // Validate configuration with detailed error reporting
    static std::pair<bool, std::vector<std::string>> validateConfig(std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        bool valid = validateConfig(*config, errors);
        return {valid, errors};
    }
    
    // Apply multiple presets
    static void applyPresets(std::shared_ptr<Config> config, 
                            const std::vector<std::string>& presetNames) {
        for (const auto& preset : presetNames) {
            applyPreset(config, preset);
        }
    }
    
    // Configuration comparison
    static bool configsEqual(std::shared_ptr<Config> config1, 
                           std::shared_ptr<Config> config2) {
        return config1->summary() == config2->summary();
    }
    
    // Configuration serialization
    static std::string serializeConfig(std::shared_ptr<Config> config) {
        return config->summary();
    }
    
    // Configuration deserialization
    static std::shared_ptr<Config> deserializeConfig(const std::string& serialized) {
        // Placeholder implementation
        return createDefaultConfig();
    }
    
private:
    static void parseSimpleJson(const std::string& json, std::shared_ptr<Config> config) {
        // Simple key=value parsing from JSON-like format
        size_t pos = 0;
        while (pos < json.size()) {
            size_t keyEnd = json.find('"', pos);
            if (keyEnd == std::string::npos) break;
            
            size_t keyStart = pos + 1;
            size_t valueStart = json.find('"', keyEnd + 1);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = json.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string key = json.substr(keyStart, keyEnd - keyStart);
            std::string value = json.substr(valueStart + 1, valueEnd - valueStart - 1);
            
            // Convert string value to appropriate type
            if (value == "true") {
                config->set(key, true, ConfigSource::Runtime);
            } else if (value == "false") {
                config->set(key, false, ConfigSource::Runtime);
            } else {
                try {
                    // Try integer
                    size_t intPos;
                    int64_t intVal = std::stoll(value, &intPos);
                    if (intPos == value.size()) {
                        config->set(key, intVal, ConfigSource::Runtime);
                        pos = valueEnd + 1;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try float
                    size_t floatPos;
                    double floatVal = std::stod(value, &floatPos);
                    if (floatPos == value.size()) {
                        config->set(key, floatVal, ConfigSource::Runtime);
                        pos = valueEnd + 1;
                        continue;
                    }
                } catch (...) {}
                
                // Default to string
                config->set(key, value, ConfigSource::Runtime);
            }
            
            pos = valueEnd + 3; // Skip ", "
        }
    }
};

// Python-enhanced Brain wrapper
class PythonBrainWrapper {
public:
    PythonBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Easy brain initialization
    bool initialize() {
        return brain_->initialize();
    }
    
    // Run simulation with error handling
    bool runSimulation(size_t steps, bool logProgress = false) {
        try {
            for (size_t i = 0; i < steps; ++i) {
                brain_->step(i);
                if (logProgress && (i % 100 == 0)) {
                    // Log progress every 100 steps
                    auto stats = getStatistics();
                }
            }
            return true;
        } catch (const std::exception& e) {
            // Log error
            return false;
        }
    }
    
    // Get brain statistics as dictionary
    py::dict getStatistics() {
        py::dict stats;
        
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        
        if (brain_->getConceptFormation()) {
            stats["concept_count"] = brain_->getConceptFormation()->getConceptCount();
        }
        
        if (brain_->getPlanner()) {
            stats["planner_available"] = true;
        } else {
            stats["planner_available"] = false;
        }
        
        return stats;
    }
    
    // Save brain state with automatic error handling
    bool saveState(const std::string& filename) {
        try {
            brain_->save(filename);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Load brain state with automatic error handling
    bool loadState(const std::string& filename) {
        try {
            brain_->load(filename);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Reset brain to initial state
    void reset() {
        brain_->reset();
        brain_->initialize();
    }
    
    // Get brain age
    double getBrainAge() const {
        return brain_->getTotalSpikeCount() * Constants::DEFAULT_TIMESTEP / 1000.0; // Rough estimate
    }
    
    // Check if brain is still learning (simple heuristic)
    bool isStillLearning() const {
        // Implementation depends on specific learning metrics
        return brain_->getTotalSpikeCount() > 100;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Python-enhanced Agent wrapper
class PythonAgentWrapper {
public:
    PythonAgentWrapper(std::shared_ptr<AgentBrain> agent) : agent_(agent) {}
    
    // Initialize agent with world
    bool initialize(std::shared_ptr<SimpleWorld> world) {
        agent_->initialize(*world);
        return true;
    }
    
    // Process sensory input with automatic normalization
    py::dict processSensoryInput(const py::dict& percept) {
        // Convert Python dict to SensoryPercept
        SensoryPercept sensoryInput;
        
        // Extract vision data
        if (percept.contains("vision")) {
            auto visionArray = percept["vision"].cast<std::vector<float>>();
            sensoryInput.setVision(visionArray);
        }
        
        // Extract touch data
        if (percept.contains("touch")) {
            auto touchArray = percept["touch"].cast<std::vector<float>>();
            sensoryInput.setTouch(touchArray);
        }
        
        // Extract internal data
        if (percept.contains("internal")) {
            auto internalArray = percept["internal"].cast<std::vector<float>>();
            sensoryInput.setInternal(internalArray);
        }
        
        // Extract proprioception data
        if (percept.contains("proprioception")) {
            auto proprioArray = percept["proprioception"].cast<std::vector<float>>();
            sensoryInput.setProprioception(proprioArray);
        }
        
        // Extract text data
        if (percept.contains("text")) {
            sensoryInput.setText(percept["text"].cast<std::string>());
        }
        
        // Process sensory input
        agent_->processSensoryInput(sensoryInput);
        
        // Return processed state
        py::dict result;
        result["vision_fired"] = agent_->getVisionFiring();
        result["touch_fired"] = agent_->getTouchFiring();
        result["internal_fired"] = agent_->getInternalFiring();
        result["proprioception_fired"] = agent_->getProprioceptionFiring();
        result["text_processed"] = agent_->getTextProcessed();
        
        return result;
    }
    
    // Get motor command with error handling
    std::string decodeMotorCommand() {
        MotorCommand cmd = agent_->decodeMotorCommand();
        
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Wait";
        }
    }
    
    // Get agent statistics
    py::dict getAgentStatistics() {
        py::dict stats;
        
        stats["curiosity_level"] = agent_->getCuriosityLevel();
        stats["novelty_level"] = agent_->getNoveltyLevel();
        stats["prediction_error"] = agent_->getPredictionError();
        stats["neuromodulation_level"] = agent_->getNeuromodulationLevel();
        stats["developmental_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        
        // Get motor group activities
        auto activities = agent_->getMotorGroupActivities();
        py::dict motorStats;
        for (size_t i = 0; i < activities.size(); ++i) {
            motorStats[activities[i].name] = activities[i].firingRate;
        }
        stats["motor_activities"] = motorStats;
        
        return stats;
    }
    
    // Enable/disable agent subsystems
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
    
    // Get curiosity level
    float getCuriosityLevel() const {
        return agent_->getCuriosityLevel();
    }
    
    // Get novelty level
    float getNoveltyLevel() const {
        return agent_->getNoveltyLevel();
    }
    
    // Apply reward modulation
    void applyRewardModulation(float reward, float predictedReward = 0.0f) {
        agent_->applyRewardModulation(reward, predictedReward);
    }
    
    // Update development
    void updateDevelopment(double timestep) {
        agent_->updateDevelopment(timestep);
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
};

// Python-enhanced World wrapper
class PythonWorldWrapper {
public:
    PythonWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    // Configure world with simplified API
    void configureWorld(int width, int height, 
                       int visionWidth = Constants::DEFAULT_VISION_WIDTH,
                       int visionHeight = Constants::DEFAULT_VISION_HEIGHT) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    // Set agent start position
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Reset world to initial state
    void resetWorld() {
        world_->reset();
    }
    
    // Update world state
    void updateWorld(double timestep) {
        world_->update(timestep);
    }
    
    // Get sensory percept as Python dict
    py::dict getSensoryPercept() {
        py::dict percept;
        
        auto sensory = world_->getSensoryPercept();
        
        // Vision data
        auto vision = sensory.getVision();
        py::list visionList;
        for (float v : vision) {
            visionList.append(v);
        }
        percept["vision"] = visionList;
        
        // Touch data
        auto touch = sensory.getTouch();
        py::list touchList;
        for (float t : touch) {
            touchList.append(t);
        }
        percept["touch"] = touchList;
        
        // Internal data
        auto internal = sensory.getInternal();
        py::list internalList;
        for (float i : internal) {
            internalList.append(i);
        }
        percept["internal"] = internalList;
        
        // Proprioception data
        auto proprioception = sensory.getProprioception();
        py::list proprioceptionList;
        for (float p : proprioception) {
            proprioceptionList.append(p);
        }
        percept["proprioception"] = proprioceptionList;
        
        // Text data
        std::string text = sensory.getText();
        if (!text.empty()) {
            percept["text"] = text;
        }
        
        // World time
        percept["simulation_time"] = world_->getSimulationTime();
        
        // Agent position
        auto body = world_->getAgentBody();
        py::dict position;
        position["x"] = body.position.x;
        position["y"] = body.position.y;
        position["angle"] = body.angle;
        percept["agent_position"] = position;
        
        return percept;
    }
    
    // Apply motor command
    bool applyMotorCommand(const std::string& command, double time) {
        MotorCommand motorCmd;
        
        if (command == "MoveForward") motorCmd = MotorCommand::MoveForward;
        else if (command == "MoveBackward") motorCmd = MotorCommand::MoveBackward;
        else if (command == "TurnLeft") motorCmd = MotorCommand::TurnLeft;
        else if (command == "TurnRight") motorCmd = MotorCommand::TurnRight;
        else if (command == "Interact") motorCmd = MotorCommand::Interact;
        else motorCmd = MotorCommand::Wait;
        
        world_->applyMotorCommand(motorCmd, time);
        return true;
    }
    
    // Get world statistics
    py::dict getWorldStatistics() {
        py::dict stats;
        
        stats["width"] = world_->getWidth();
        stats["height"] = world_->getHeight();
        stats["vision_width"] = world_->getVisionWidth();
        stats["vision_height"] = world_->getVisionHeight();
        stats["simulation_time"] = world_->getSimulationTime();
        stats["max_energy"] = world_->getMaxEnergy();
        stats["energy_decay_rate"] = world_->getEnergyDecayRate();
        
        return stats;
    }
    
    // Set random seed for reproducible results
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

} // namespace nlm

// Python bindings for enhanced NLM API
void bindEnhancedPythonAPI(py::module& m) {
    // Bind enhanced configuration factory
    py::class_<EnhancedConfigFactory>(m, "ConfigFactory")
        .def_static("create_config", &EnhancedConfigFactory::createConfig,
                   py::arg("neuron_count") = Constants::BRAIN_DEFAULT_NEURON_COUNT,
                   py::arg("connection_probability") = Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
                   py::arg("simulation_timestep") = Constants::DEFAULT_TIMESTEP,
                   py::arg("random_seed") = Constants::SIMULATION_DEFAULT_RANDOM_SEED,
                   py::arg("preset") = "default")
        .def_static("create_config_from_json", &EnhancedConfigFactory::createConfigFromJson)
        .def_static("builder", &EnhancedConfigFactory::builder)
        .def_static("get_available_presets", &EnhancedConfigFactory::getAvailablePresets)
        .def_static("validate_config", &EnhancedConfigFactory::validateConfig)
        .def_static("apply_presets", &EnhancedConfigFactory::applyPresets)
        .def_static("configs_equal", &EnhancedConfigFactory::configsEqual)
        .def_static("serialize_config", &EnhancedConfigFactory::serializeConfig)
        .def_static("deserialize_config", &EnhancedConfigFactory::deserializeConfig);
    
    // Bind ConfigBuilder
    py::class_<EnhancedConfigFactory::ConfigBuilder>(m, "ConfigBuilder")
        .def("set_neuron_count", &EnhancedConfigFactory::ConfigBuilder::setNeuronCount)
        .def("set_connection_probability", &EnhancedConfigFactory::ConfigBuilder::setConnectionProbability)
        .def("set_simulation_timestep", &EnhancedConfigFactory::ConfigBuilder::setSimulationTimestep)
        .def("set_random_seed", &EnhancedConfigFactory::ConfigBuilder::setRandomSeed)
        .def("enable_plasticity", &EnhancedConfigFactory::ConfigBuilder::enablePlasticity)
        .def("set_learning_rate", &EnhancedConfigFactory::ConfigBuilder::setLearningRate)
        .def("preset", &EnhancedConfigFactory::ConfigBuilder::preset)
        .def("build", &EnhancedConfigFactory::ConfigBuilder::build);
    
    // Bind PythonBrainWrapper
    py::class_<PythonBrainWrapper>(m, "Brain")
        .def("initialize", &PythonBrainWrapper::initialize)
        .def("run_simulation", &PythonBrainWrapper::runSimulation,
             py::arg("steps"), py::arg("log_progress") = false)
        .def("get_statistics", &PythonBrainWrapper::getStatistics)
        .def("save_state", &PythonBrainWrapper::saveState)
        .def("load_state", &PythonBrainWrapper::loadState)
        .def("reset", &PythonBrainWrapper::reset)
        .def("get_brain_age", &PythonBrainWrapper::getBrainAge)
        .def("is_still_learning", &PythonBrainWrapper::isStillLearning);
    
    // Bind PythonAgentWrapper
    py::class_<PythonAgentWrapper>(m, "Agent")
        .def("initialize", &PythonAgentWrapper::initialize)
        .def("process_sensory_input", &PythonAgentWrapper::processSensoryInput)
        .def("decode_motor_command", &PythonAgentWrapper::decodeMotorCommand)
        .def("get_agent_statistics", &PythonAgentWrapper::getAgentStatistics)
        .def("enable_reward_modulation", &PythonAgentWrapper::enableRewardModulation)
        .def("enable_structural_plasticity", &PythonAgentWrapper::enableStructuralPlasticity)
        .def("enable_development", &PythonAgentWrapper::enableDevelopment)
        .def("enable_curiosity", &PythonAgentWrapper::enableCuriosity)
        .def("get_curiosity_level", &PythonAgentWrapper::getCuriosityLevel)
        .def("get_novelty_level", &PythonAgentWrapper::getNoveltyLevel)
        .def("apply_reward_modulation", &PythonAgentWrapper::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0f)
        .def("update_development", &PythonAgentWrapper::updateDevelopment);
    
    // Bind PythonWorldWrapper
    py::class_<PythonWorldWrapper>(m, "World")
        .def("configure_world", &PythonWorldWrapper::configureWorld,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width") = Constants::DEFAULT_VISION_WIDTH,
             py::arg("vision_height") = Constants::DEFAULT_VISION_HEIGHT)
        .def("set_agent_start", &PythonWorldWrapper::setAgentStart)
        .def("reset_world", &PythonWorldWrapper::resetWorld)
        .def("update_world", &PythonWorldWrapper::updateWorld)
        .def("get_sensory_percept", &PythonWorldWrapper::getSensoryPercept)
        .def("apply_motor_command", &PythonWorldWrapper::applyMotorCommand)
        .def("get_world_statistics", &PythonWorldWrapper::getWorldStatistics)
        .def("set_random_seed", &PythonWorldWrapper::setRandomSeed);
    
    // Bind convenience functions
    m.def("create_enhanced_brain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        return PythonBrainWrapper(brain);
    });
    
    m.def("create_enhanced_agent", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        return PythonAgentWrapper(agent);
    });
    
    m.def("create_enhanced_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        return PythonWorldWrapper(world);
    });
    
    // Bind utility functions
    m.def("get_version", []() { return "NLM Enhanced Python API v1.0"; });
    
    m.def("get_available_presets", []() {
        return EnhancedConfigFactory::getAvailablePresets();
    });
    
    m.def("validate_config", EnhancedConfigFactory::validateConfig);
    
    // Bind constants
    m.attr("DEFAULT_TIMESTEP") = Constants::DEFAULT_TIMESTEP;
    m.attr("BRAIN_V_THRESH") = Constants::BRAIN_V_THRESH;
    m.attr("BRAIN_V_REST") = Constants::BRAIN_V_REST;
    m.attr("BRAIN_V_RESET") = Constants::BRAIN_V_RESET;
    m.attr("BRAIN_TAU_MEM") = Constants::BRAIN_TAU_MEM;
    m.attr("BRAIN_TAU_REF") = Constants::BRAIN_TAU_REF;
    
    // Bind configuration presets
    m.def("apply_preset", EnhancedConfigFactory::applyPreset);
    
    // Bind brain creation functions
    m.def("create_brain", createDefaultConfig,
          py::arg("neuron_count") = Constants::BRAIN_DEFAULT_NEURON_COUNT,
          py::arg("connection_probability") = Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
          py::arg("simulation_timestep") = Constants::DEFAULT_TIMESTEP,
          py::arg("random_seed") = Constants::SIMULATION_DEFAULT_RANDOM_SEED);
    
    m.def("create_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(Constants::DEFAULT_VISION_WIDTH, Constants::DEFAULT_VISION_HEIGHT);
        return world;
    });
    
    m.def("create_agent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    });
}

} // namespace nlm