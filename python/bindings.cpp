#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>

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

// Custom exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Enhanced Python class for Config with better error handling and convenience methods
class PyConfig {
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    
    PyConfig(const PyConfig&) = default;
    PyConfig& operator=(const PyConfig&) = default;
    PyConfig(PyConfig&&) = default;
    PyConfig& operator=(PyConfig&&) = default;
    
    // Configuration loading/saving
    bool load_from_file(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    bool save_to_file(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Configuration access with better error handling
    template<typename T>
    T get(const std::string& key) const {
        auto result = config_->get<T>(key);
        if (!result.has_value()) {
            throw NLMError("Configuration key \"" + key + "\" not found");
        }
        return *result;
    }
    
    // Get with default value (Pythonic interface)
    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        return config_->getOr<T>(key, default_value);
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Set configuration values
    void set(const std::string& key, int value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, float value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    // Get all configuration keys
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    // Get configuration summary
    std::string get_summary() const {
        return config_->summary();
    }
    
    // Convenience methods for common brain configuration
    void configure_brain(int neuron_count = 1000, float synapse_density = 0.1f,
                        float connection_probability = 0.05f) {
        set("brain.neuron_count", neuron_count);
        set("brain.synapse_density", synapse_density);
        set("brain.connection_probability", connection_probability);
    }
    
    void configure_plasticity(bool enable_stdp = true, bool enable_hebbian = true,
                              float learning_rate = 0.001f) {
        set("plasticity.stdp.enable", enable_stdp);
        set("plasticity.hebbian.enable", enable_hebbian);
        set("plasticity.stdp.learning_rate", learning_rate);
    }
    
    void configure_neuromodulation(bool enable_curiosity = true,
                                   bool enable_novelty = true,
                                   float dopamine_scale = 1.0f) {
        set("neuromod.dopamine.scale", dopamine_scale);
        set("neuromod.curiosity.enable", enable_curiosity);
        set("neuromod.novelty.enable", enable_novelty);
    }
    
    // Get the underlying config
    std::shared_ptr<Config> get_config() const {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Python class for Brain with high-level APIs
class PyBrain {
public:
    PyBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    PyBrain(const PyBrain&) = default;
    PyBrain& operator=(const PyBrain&) = default;
    PyBrain(PyBrain&&) = default;
    PyBrain& operator=(PyBrain&&) = default;
    
    // Initialization
    bool initialize() {
        return brain_->initialize();
    }
    
    // High-level simulation with automatic time stepping
    struct SimulationResult {
        size_t step;
        double time;
        size_t active_neurons;
        size_t firing_neurons;
        float average_firing_rate;
        float e_i_ratio;
        size_t total_spikes;
        std::map<std::string, float> additional_stats;
    };
    
    SimulationResult step(size_t step, double time = 0.0) {
        SimulationResult result;
        result.step = step;
        result.time = time;
        
        // Record state before step
        result.active_neurons = brain_->getActiveNeuronCount();
        result.firing_neurons = brain_->getFiringNeuronCount();
        result.average_firing_rate = brain_->getAverageFiringRate();
        result.e_i_ratio = brain_->getExcitationInhibitionRatio();
        result.total_spikes = brain_->getTotalSpikeCount();
        
        // Perform step
        brain_->step(static_cast<SimulationStep>(step), time);
        
        // Record state after step
        result.active_neurons = brain_->getActiveNeuronCount();
        result.firing_neurons = brain_->getFiringNeuronCount();
        result.average_firing_rate = brain_->getAverageFiringRate();
        result.e_i_ratio = brain_->getExcitationInhibitionRatio();
        result.total_spikes = brain_->getTotalSpikeCount();
        
        return result;
    }
    
    // Convenience simulation methods
    std::vector<SimulationResult> run_simulation(size_t num_steps, double timestep = 0.001,
                                                bool auto_time = true) {
        std::vector<SimulationResult> results;
        for (size_t i = 0; i < num_steps; ++i) {
            double time = auto_time ? i * timestep : 0.0;
            results.push_back(step(i, time));
        }
        return results;
    }
    
    // Reset and initialize
    bool reset_and_initialize() {
        brain_->reset();
        return initialize();
    }
    
    // Save/load
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Statistics and monitoring
    struct BrainStats {
        size_t total_neurons;
        size_t total_synapses;
        size_t active_neurons;
        size_t firing_neurons;
        float average_firing_rate;
        float e_i_ratio;
        size_t total_spikes;
        size_t pending_spikes;
        std::string developmental_stage_str;
        float learned_preference;
        
        // Convert to Python-friendly format
        py::dict to_dict() const {
            py::dict d;
            d["total_neurons"] = total_neurons;
            d["total_synapses"] = total_synapses;
            d["active_neurons"] = active_neurons;
            d["firing_neurons"] = firing_neurons;
            d["average_firing_rate"] = average_firing_rate;
            d["e_i_ratio"] = e_i_ratio;
            d["total_spikes"] = total_spikes;
            d["pending_spikes"] = pending_spikes;
            d["developmental_stage"] = developmental_stage_str;
            d["learned_preference"] = learned_preference;
            return d;
        }
    };
    
    BrainStats get_stats() const {
        BrainStats stats;
        stats.total_neurons = brain_->getTotalNeuronCount();
        stats.total_synapses = brain_->getTotalSynapseCount();
        stats.active_neurons = brain_->getActiveNeuronCount();
        stats.firing_neurons = brain_->getFiringNeuronCount();
        stats.average_firing_rate = brain_->getAverageFiringRate();
        stats.e_i_ratio = brain_->getExcitationInhibitionRatio();
        stats.total_spikes = brain_->getTotalSpikeCount();
        stats.pending_spikes = brain_->getPendingSpikeEventCount();
        stats.developmental_stage_str = "Unknown";  // TODO: Add proper enum conversion
        stats.learned_preference = brain_->getLearnedPreference();
        return stats;
    }
    
    // Access to underlying components
    std::shared_ptr<Brain> get_brain() const {
        return brain_;
    }
    
    // Convenience methods for common operations
    void inject_current_to_neuron(size_t neuron_id, float current) {
        // Need neuron ID conversion - this would require additional bindings
        // For now, we'll use a placeholder
        throw NLMError("Direct neuron injection requires additional bindings");
    }
    
    void inject_current_to_sensory(float current) {
        brain_->injectCurrentToNeurons(NeuronType::Sensory, current);
    }
    
    void inject_current_to_motor(float current) {
        brain_->injectCurrentToNeurons(NeuronType::Motor, current);
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced AgentBrain with high-level APIs
class PyAgentBrain {
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    PyAgentBrain(const PyAgentBrain&) = default;
    PyAgentBrain& operator=(const PyAgentBrain&) = default;
    PyAgentBrain(PyAgentBrain&&) = default;
    PyAgentBrain& operator=(PyAgentBrain&&) = default;
    
    // Initialization with world
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agent_brain_->initialize(*world);
    }
    
    // High-level interaction loop
    struct InteractionResult {
        size_t step;
        float reward;
        bool success;
        std::string action_name;
        float curiosity_level;
        float novelty_level;
        float prediction_error;
        std::string thought_trace;
        
        py::dict to_dict() const {
            py::dict d;
            d["step"] = step;
            d["reward"] = reward;
            d["success"] = success;
            d["action"] = action_name;
            d["curiosity"] = curiosity_level;
            d["novelty"] = novelty_level;
            d["prediction_error"] = prediction_error;
            d["thought_trace"] = thought_trace;
            return d;
        }
    };
    
    InteractionResult interact_step(std::shared_ptr<SimpleWorld> world, size_t step,
                                   float reward = 0.0f, float predicted_reward = 0.0f) {
        InteractionResult result;
        result.step = step;
        
        // Update world
        world->update(0.1);  // Default timestep
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process input
        agent_brain_->processSensoryInput(percept);
        
        // Run brain step
        agent_brain_->getBrain()->step(step, step * 0.001);
        
        // Decode action
        auto action = agent_brain_->decodeMotorCommand();
        result.action_name = action_to_string(action);
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        agent_brain_->applyRewardModulation(reward, predicted_reward);
        
        // Update development
        agent_brain_->updateDevelopment(0.1);
        
        // Get statistics
        result.reward = reward;
        result.success = (reward > -0.1f);  // Simple success metric
        result.curiosity_level = agent_brain_->getCuriosityLevel();
        result.novelty_level = agent_brain_->getNoveltyLevel();
        result.prediction_error = agent_brain_->getPredictionError();
        result.thought_trace = agent_brain_->getThoughtTrace();
        
        return result;
    }
    
    // Run complete episode
    std::vector<InteractionResult> run_episode(std::shared_ptr<SimpleWorld> world, 
                                              size_t num_steps, 
                                              std::function<float(size_t)> reward_function = nullptr) {
        std::vector<InteractionResult> results;
        
        for (size_t i = 0; i < num_steps; ++i) {
            float reward = 0.0f;
            if (reward_function) {
                reward = reward_function(i);
            }
            
            float predicted = 0.0f;  // In a real implementation, this would come from prediction system
            
            InteractionResult result = interact_step(world, i, reward, predicted);
            results.push_back(result);
        }
        
        return results;
    }
    
    // Enable/disable subsystems
    void enable_subsystems(bool reward_modulation = true, bool structural_plasticity = true,
                          bool development = true, bool curiosity = true) {
        agent_brain_->enableRewardModulation(reward_modulation);
        agent_brain_->enableStructuralPlasticity(structural_plasticity);
        agent_brain_->enableDevelopment(development);
        agent_brain_->enableCuriosity(curiosity);
    }
    
    // Get statistics
    py::dict get_status() const {
        py::dict d;
        d["curiosity"] = agent_brain_->getCuriosityLevel();
        d["novelty"] = agent_brain_->getNoveltyLevel();
        d["prediction_error"] = agent_brain_->getPredictionError();
        d["neuromodulation"] = agent_brain_->getNeuromodulationLevel();
        d["developmental_stage"] = agent_brain_->getDevelopmentalStage();
        d["learned_preference"] = agent_brain_->getLearnedPreference();
        return d;
    }
    
    std::shared_ptr<AgentBrain> get_agent_brain() const {
        return agent_brain_;
    }
    
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
    std::string action_to_string(MotorCommand action) {
        switch (action) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
};

// Visualization utilities
class PyVisualization {
public:
    static void plot_neural_activity(const PyBrain& brain, size_t num_steps = 100) {
        std::cout << "Neural Activity Visualization (Phase " << num_steps << " steps)" << std::endl;
        std::cout << "Step, Active Neurons, Firing Neurons, Avg Firing Rate, E/I Ratio" << std::endl;
        
        for (size_t i = 0; i < num_steps; ++i) {
            auto result = brain.step(i, i * 0.001);
            std::cout << i << ", " 
                      << result.active_neurons << ", "
                      << result.firing_neurons << ", "
                      << result.average_firing_rate << ", "
                      << result.e_i_ratio << std::endl;
        }
    }
    
    static void plot_spike_patterns(const PyBrain& brain, size_t num_steps = 1000) {
        std::cout << "Spike Pattern Analysis" << std::endl;
        std::cout << "Step, Total Spikes, Spikes per Step" << std::endl;
        
        size_t cumulative_spikes = 0;
        for (size_t i = 0; i < num_steps; ++i) {
            auto result = brain.step(i, i * 0.001);
            cumulative_spikes += (result.total_spikes - (i > 0 ? 0 : 0));  // This is approximate
            std::cout << i << ", " << result.total_spikes << ", "
                      << (cumulative_spikes / (i + 1)) << std::endl;
        }
    }
    
    static void generate_activity_summary(const PyBrain& brain, size_t num_steps = 100) {
        std::cout << "=== Neural Activity Summary ===" << std::endl;
        
        auto stats = brain.get_stats();
        std::cout << "Final Statistics:" << std::endl;
        std::cout << "  Total Neurons: " << stats.total_neurons << std::endl;
        std::cout << "  Total Synapses: " << stats.total_synapses << std::endl;
        std::cout << "  Active Neurons: " << stats.active_neurons << std::endl;
        std::cout << "  Firing Neurons: " << stats.firing_neurons << std::endl;
        std::cout << "  Average Firing Rate: " << stats.average_firing_rate << " Hz" << std::endl;
        std::cout << "  E/I Ratio: " << stats.e_i_ratio << std::endl;
        std::cout << "  Total Spikes: " << stats.total_spikes << std::endl;
        std::cout << "  Learned Preference: " << stats.learned_preference << std::endl;
    }
    
    static void plot_world_interaction(const PyAgentBrain& agent, 
                                       std::shared_ptr<SimpleWorld> world,
                                       size_t num_steps = 50) {
        std::cout << "=== World Interaction Visualization ===" << std::endl;
        std::cout << "Step, Action, Reward, Curiosity, Novelty" << std::endl;
        
        agent.enable_subsystems(true, false, true, true);  // Enable learning subsystems
        
        for (size_t i = 0; i < num_steps; ++i) {
            float reward = 0.0f;  // Simple reward function
            auto result = agent.interact_step(world, i, reward, 0.0f);
            
            std::cout << i << ", " 
                      << result.action_name << ", "
                      << result.reward << ", "
                      << result.curiosity_level << ", "
                      << result.novelty_level << std::endl;
        }
    }
};

// Training utilities
class PyTraining {
public:
    struct TrainingResult {
        size_t episode;
        float total_reward;
        float average_reward_per_step;
        float final_curiosity;
        float final_novelty;
        float learning_rate;
        std::vector<float> reward_history;
        
        py::dict to_dict() const {
            py::dict d;
            d["episode"] = episode;
            d["total_reward"] = total_reward;
            d["average_reward"] = average_reward_per_step;
            d["final_curiosity"] = final_curiosity;
            d["final_novelty"] = final_novelty;
            d["learning_rate"] = learning_rate;
            d["reward_history"] = reward_history;
            return d;
        }
    };
    
    // Simple training loop
    static std::vector<TrainingResult> train_agent(PyAgentBrain& agent, 
                                                   std::shared_ptr<SimpleWorld> world,
                                                   size_t num_episodes,
                                                   size_t steps_per_episode,
                                                   std::function<float(size_t, const py::dict&)> reward_function = nullptr) {
        std::vector<TrainingResult> results;
        
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            std::vector<float> episode_rewards;
            float episode_total = 0.0f;
            
            // Reset agent for new episode
            agent.get_agent_brain()->reset();
            
            for (size_t step = 0; step < steps_per_episode; ++step) {
                // Get state information for reward function
                py::dict state;
                state["step"] = step;
                state["episode"] = episode;
                
                float reward = 0.0f;
                if (reward_function) {
                    reward = reward_function(step, state);
                }
                
                float predicted = 0.0f;  // Would use prediction system in real implementation
                
                auto interaction = agent.interact_step(world, step, reward, predicted);
                episode_rewards.push_back(reward);
                episode_total += reward;
            }
            
            TrainingResult result;
            result.episode = episode;
            result.total_reward = episode_total;
            result.average_reward_per_step = episode_total / steps_per_episode;
            result.reward_history = episode_rewards;
            result.final_curiosity = agent.get_status()["curiosity"].cast<float>();
            result.final_novelty = agent.get_status()["novelty"].cast<float>();
            result.learning_rate = 0.01f;  // Would come from agent's config in real implementation
            
            results.push_back(result);
            
            // Progress update
            if ((episode + 1) % 10 == 0) {
                std::cout << "Episode " << episode + 1 << "/" << num_episodes 
                          << " - Total Reward: " << episode_total 
                          << " - Avg: " << result.average_reward_per_step << std::endl;
            }
        }
        
        return results;
    }
    
    // Simple heuristic reward function
    static float heuristic_reward_function(size_t step, const py::dict& state) {
        // Example: Simple exploration reward based on curiosity
        size_t episode = state["episode"].cast<size_t>();
        
        // Simple exploration bonus
        if (step % 10 == 0) {
            return 0.1f;  // Small exploration bonus
        }
        
        // Large reward for reaching certain milestones
        if (step > 50 && step % 20 == 0) {
            return 1.0f;
        }
        
        return 0.0f;
    }
};

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
