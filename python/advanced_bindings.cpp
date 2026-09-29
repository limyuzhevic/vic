#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/plasticity/RewardModulatedSTDP.hpp"
#include "../src/plasticity/TripletSTDP.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Python configuration presets
py::dict getAdvancedConfigPresets() {
    py::dict presets;
    
    // Development-focused preset
    {
        py::dict dev_preset;
        dev_preset["brain.neuron_count"] = 2000;
        dev_preset["brain.synapse_density"] = 0.15f;
        dev_preset["plasticity.structural.enable"] = true;
        dev_preset["plasticity.structural.synaptogenesis_rate"] = 0.001f;
        dev_preset["plasticity.structural.pruning_rate"] = 0.0001f;
        dev_preset["neuromod.dopamine.scale"] = 1.2f;
        dev_preset["neuromod.curiosity.enable"] = true;
        dev_preset["neuromod.novelty.enable"] = true;
        dev_preset["development.initial_stage"] = "CriticalPeriod";
        presets["development"] = dev_preset;
    }
    
    // Learning-focused preset
    {
        py::dict learning_preset;
        learning_preset["brain.neuron_count"] = 1500;
        learning_preset["brain.synapse_density"] = 0.12f;
        learning_preset["plasticity.stdp.enable"] = true;
        learning_preset["plasticity.stdp.learning_rate"] = 0.005f;
        learning_preset["plasticity.hebbian.enable"] = true;
        learning_preset["plasticity.hebbian.learning_rate"] = 0.01f;
        learning_preset["neuromod.reward_modulation.enable"] = true;
        learning_preset["neuromod.reward_modulation.strength"] = 1.5f;
        presets["learning"] = learning_preset;
    }
    
    // Exploration-focused preset
    {
        py::dict exploration_preset;
        exploration_preset["brain.neuron_count"] = 1000;
        exploration_preset["brain.synapse_density"] = 0.08f;
        exploration_preset["plasticity.structural.enable"] = true;
        exploration_preset["plasticity.structural.synaptogenesis_rate"] = 0.002f;
        exploration_preset["neuromod.curiosity.enable"] = true;
        exploration_preset["neuromod.curiosity.exploration_rate"] = 0.8f;
        exploration_preset["neuromod.novelty.enable"] = true;
        exploration_preset["neuromod.novelty.sensitivity"] = 1.2f;
        presets["exploration"] = exploration_preset;
    }
    
    return presets;
}

// Advanced simulation utilities
class AdvancedBrainSimulator {
public:
    AdvancedBrainSimulator(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Run simulation with progress tracking and callbacks
    void runWithProgress(size_t steps, std::function<void(size_t, float)> callback = nullptr) {
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step);
            if (callback) {
                float progress = static_cast<float>(step) / static_cast<float>(steps - 1);
                callback(step, progress);
            }
        }
    }
    
    // Run learning experiment with detailed metrics
    py::dict runLearningExperiment(size_t steps) {
        py::dict metrics;
        py::list weight_history;
        py::list spike_history;
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step);
            
            // Collect metrics
            weight_history.append(brain_->getTotalSynapseCount());
            spike_history.append(brain_->getTotalSpikeCount());
        }
        
        metrics["total_synapses"] = brain_->getTotalSynapseCount();
        metrics["total_spikes"] = brain_->getTotalSpikeCount();
        metrics["weight_history"] = weight_history;
        metrics["spike_history"] = spike_history;
        
        return metrics;
    }
    
    // Optimize brain configuration using simple evolutionary algorithm
    std::shared_ptr<Config> optimizeConfig(std::function<float(std::shared_ptr<Config>)> objective,
                                           size_t generations = 100) {
        auto best_config = std::make_shared<Config>();
        float best_score = -std::numeric_limits<float>::max();
        
        for (size_t gen = 0; gen < generations; ++gen) {
            // Create random perturbation of best config
            auto candidate_config = std::make_shared<Config>(*best_config);
            
            // Apply random mutations
            float mutation_rate = 0.1f;
            for (const auto& key : candidate_config->getKeys()) {
                if (rand() / (float)RAND_MAX < mutation_rate) {
                    if (key.find("neuron_count") != std::string::npos) {
                        candidate_config->set(key, static_cast<int64_t>(best_config->getOr<int64_t>(key, 1000) * (1.0f + (rand() / (float)RAND_MAX - 0.5f) * 0.2f)), nlm::ConfigSource::Default);
                    } else if (key.find("learning_rate") != std::string::npos || key.find("plasticity") != std::string::npos) {
                        candidate_config->set(key, best_config->getOr<float>(key, 1.0f) * (1.0f + (rand() / (float)RAND_MAX - 0.5f) * 0.3f), nlm::ConfigSource::Default);
                    }
                }
            }
            
            // Evaluate candidate
            float score = objective(candidate_config);
            if (score > best_score) {
                best_score = score;
                best_config = candidate_config;
            }
        }
        
        return best_config;
    }
    
    // Get comprehensive brain statistics
    py::dict getComprehensiveStats() {
        py::dict stats;
        
        stats["neuron_count"] = brain_->getTotalNeuronCount();
        stats["synapse_count"] = brain_->getTotalSynapseCount();
        stats["firing_neuron_count"] = brain_->getFiringNeuronCount();
        stats["active_neuron_count"] = brain_->getActiveNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["pending_spike_events"] = brain_->getPendingSpikeEventCount();
        
        // Development status
        stats["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        
        // Configuration summary
        auto config = brain_->getConfig();
        stats["config_keys"] = config->getKeys();
        
        return stats;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Advanced agent utilities
class AdvancedAgent {
public:
    AdvancedAgent(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) 
        : agent_(std::make_shared<AgentBrain>(brain)), world_(world) {}
    
    // Run complete reinforcement learning episode
    py::dict runEpisode(size_t max_steps, bool enable_learning = true) {
        py::dict episode_stats;
        py::list rewards;
        py::list actions;
        py::list states;
        
        world_->reset();
        
        for (size_t step = 0; step < max_steps; ++step) {
            // Get current state
            auto percept = world_->getSensoryPercept();
            states.append(percept);
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Run brain step
            brain_->step(step);
            
            // Get action
            auto action = agent_->decodeMotorCommand();
            actions.append(action);
            
            // Apply action to world
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Get reward if learning enabled
            float reward = 0.0f;
            if (enable_learning) {
                reward = percept.getInternal()[0] if (percept.getInternal().size() > 0 else 0.0f;
                agent_->applyRewardModulation(reward, 0.0f); // Simplified
            }
            
            rewards.append(reward);
        }
        
        episode_stats["rewards"] = rewards;
        episode_stats["actions"] = actions;
        episode_stats["states"] = states;
        episode_stats["total_reward"] = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
        episode_stats["steps"] = max_steps;
        
        return episode_stats;
    }
    
    // Enable all advanced learning mechanisms
    void enableAdvancedLearning(bool enable = true) {
        agent_->enableRewardModulation(enable);
        agent_->enableStructuralPlasticity(enable);
        agent_->enableDevelopment(enable);
        agent_->enableCuriosity(enable);
    }
    
    // Set learning hyperparameters
    void setLearningParameters(float reward_strength = 1.0f, float curiosity_gain = 0.5f,
                               float novelty_threshold = 0.3f) {
        // Note: These would need to be exposed in the AgentBrain API
        // For now, this is a placeholder for future implementation
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
};

// Configuration template system
class ConfigTemplate {
public:
    ConfigTemplate(const std::string& name, const py::dict& template_data) 
        : name_(name), template_data_(template_data) {}
    
    // Apply template to create configuration
    std::shared_ptr<Config> createConfig() {
        auto config = std::make_shared<Config>();
        
        for (auto it = template_data_.begin(); it != template_data_.end(); ++it) {
            std::string key = it->first.cast<std::string>();
            
            // Handle different parameter types
            if (py::isinstance<py::int_>(it->second)) {
                config->set(key, it->second.cast<int64_t>(), nlm::ConfigSource::Default);
            } else if (py::isinstance<py::float_>(it->second)) {
                config->set(key, it->second.cast<float>(), nlm::ConfigSource::Default);
            } else if (py::isinstance<py::bool_>(it->second)) {
                config->set(key, it->second.cast<bool>(), nlm::ConfigSource::Default);
            }
        }
        
        return config;
    }
    
    const std::string& getName() const { return name_; }
    const py::dict& getTemplateData() const { return template_data_; }
    
private:
    std::string name_;
    py::dict template_data_;
};

// Python module initialization with advanced features
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM Python Module
        ---------------------------
        Enhanced Python bindings for NLM with advanced simulation features,
        learning utilities, and configuration templates.
    )pbdoc";
    
    // Add basic NLM functionality
    py::module_ nlm_basic = py::module::import("pynlm");
    
    // Register advanced configuration presets
    m.def("getAdvancedConfigPresets", &getAdvancedConfigPresets,
          "Get predefined configuration presets for different use cases");
    
    // Register advanced brain simulator
    py::class_<AdvancedBrainSimulator>(m, "AdvancedBrainSimulator")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("runWithProgress", &AdvancedBrainSimulator::runWithProgress,
             py::arg("steps"), py::arg("callback") = nullptr,
             "Run simulation with progress tracking and callbacks")
        .def("runLearningExperiment", &AdvancedBrainSimulator::runLearningExperiment,
             py::arg("steps"), "Run learning experiment with detailed metrics")
        .def("optimizeConfig", &AdvancedBrainSimulator::optimizeConfig,
             py::arg("objective"), py::arg("generations") = 100,
             "Optimize brain configuration using evolutionary algorithm")
        .def("getComprehensiveStats", &AdvancedBrainSimulator::getComprehensiveStats,
             "Get comprehensive brain statistics");
    
    // Register advanced agent
    py::class_<AdvancedAgent>(m, "AdvancedAgent")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"))
        .def("runEpisode", &AdvancedAgent::runEpisode,
             py::arg("max_steps"), py::arg("enable_learning") = true,
             "Run complete reinforcement learning episode")
        .def("enableAdvancedLearning", &AdvancedAgent::enableAdvancedLearning,
             py::arg("enable") = true, "Enable all advanced learning mechanisms")
        .def("setLearningParameters", &AdvancedAgent::setLearningParameters,
             py::arg("reward_strength") = 1.0f, py::arg("curiosity_gain") = 0.5f,
             py::arg("novelty_threshold") = 0.3f, "Set learning hyperparameters");
    
    // Register configuration template system
    py::class_<ConfigTemplate>(m, "ConfigTemplate")
        .def(py::init<std::string, py::dict>(), py::arg("name"), py::arg("template_data"))
        .def("createConfig", &ConfigTemplate::createConfig,
             "Apply template to create configuration")
        .def("getName", &ConfigTemplate::getName)
        .def("getTemplateData", &ConfigTemplate::getTemplateData);
    
    // Register new plasticity rule classes
    py::class_<RewardModulatedSTDP, std::shared_ptr<RewardModulatedSTDP>>(m, "RewardModulatedSTDP")
        .def(py::init<>())
        .def("getName", &RewardModulatedSTDP::getName)
        .def("setRewardStrength", &RewardModulatedSTDP::setRewardStrength)
        .def("getRewardStrength", &RewardModulatedSTDP::getRewardStrength)
        .def("setTDDiscount", &RewardModulatedSTDP::setTDDiscount)
        .def("getTDDiscount", &RewardModulatedSTDP::getTDDiscount);
    
    py::class_<TripletSTDP, std::shared_ptr<TripletSTDP>>(m, "TripletSTDP")
        .def(py::init<>())
        .def("getName", &TripletSTDP::getName)
        .def("setA1Plus", &TripletSTDP::setA1Plus)
        .def("getA1Plus", &TripletSTDP::getA1Plus)
        .def("setA1Minus", &TripletSTDP::setA1Minus)
        .def("getA1Minus", &TripletSTDP::getA1Minus)
        .def("setA2Plus", &TripletSTDP::setA2Plus)
        .def("getA2Plus", &TripletSTDP::getA2Plus)
        .def("setA2Minus", &TripletSTDP::setA2Minus)
        .def("getA2Minus", &TripletSTDP::getA2Minus);
    
    // Advanced simulation mode enum
    py::enum_<SimulationMode>(m, "SimulationMode")
        .value("STANDARD", SimulationMode::Standard)
        .value("FAST", SimulationMode::Fast)
        .value("ACCURATE", SimulationMode::Accurate)
        .value("BALANCED", SimulationMode::Balanced)
        .export_values();
    
    // Advanced metrics structure
    py::class_<AdvancedMetrics>(m, "AdvancedMetrics")
        .def_readonly("total_spikes", &AdvancedMetrics::totalSpikes)
        .def_readonly("average_weight_change", &AdvancedMetrics::averageWeightChange)
        .def_readonly("learning_efficiency", &AdvancedMetrics::learningEfficiency)
        .def_readonly("stability_index", &AdvancedMetrics::stabilityIndex);
    
    // Add integration utilities
    m.def("createAdvancedSimulator", [](std::shared_ptr<Brain> brain) {
        return AdvancedBrainSimulator(brain);
    }, py::arg("brain"), "Create advanced brain simulator");
    
    m.def("createAdvancedAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        return AdvancedAgent(brain, world);
    }, py::arg("brain"), py::arg("world"), "Create advanced agent");
    
    m.def("createConfigTemplate", [](const std::string& name, py::dict data) {
        return ConfigTemplate(name, data);
    }, py::arg("name"), py::arg("data"), "Create configuration template");
}

} // namespace nlm
