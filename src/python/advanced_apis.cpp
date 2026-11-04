// Advanced APIs Module - advanced_apis.cpp
#ifndef ADVANCED_APIS_H
#define ADVANCED_APIS_H

#include <pybind11/pybind11.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace py = pybind11;
namespace nlm {

// Advanced convenience functions and high-level APIs

// Factory functions for creating specialized configurations
std::shared_ptr<Config> create_advanced_config(const std::string& config_type = "reinforcement_learning", 
                                              const py::dict& custom_params = py::dict()) {
    auto config = std::make_shared<Config>();
    
    if (config_type == "reinforcement_learning") {
        config->set("brain.neuron_count", 2000, ConfigSource::Default);
        config->set("brain.synapse_density", 0.15f, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    } else if (config_type == "pattern_recognition") {
        config->set("brain.neuron_count", 1500, ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("memory.episodic.capacity", 5000, ConfigSource::Default);
        config->set("memory.associative.capacity", 10000, ConfigSource::Default);
    } else if (config_type == "memory_formation") {
        config->set("brain.neuron_count", 3000, ConfigSource::Default);
        config->set("brain.synapse_density", 0.2f, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("development.initial_plasticity_rate", 0.1f, ConfigSource::Default);
    } else if (config_type == "agent_learning") {
        config->set("brain.neuron_count", 2500, ConfigSource::Default);
        config->set("agent.memory_capacity", 8000, ConfigSource::Default);
        config->set("agent.action_diversity", true, ConfigSource::Default);
    }
    
    // Apply custom parameters
    for (auto& item : custom_params) {
        std::string key = item.first.cast<std::string>();
        if (item.second.get_type() == py::detail::any_detail::PYBIND11_VERSION_NAMESPACE) {
            // Handle different types
            try {
                int64_t int_val = item.second.cast<int64_t>();
                config->set(key, int_val, ConfigSource::Default);
            } catch (...) {
                try {
                    float float_val = item.second.cast<float>();
                    config->set(key, float_val, ConfigSource::Default);
                } catch (...) {
                    try {
                        bool bool_val = item.second.cast<bool>();
                        config->set(key, bool_val, ConfigSource::Default);
                    } catch (...) {
                        // Skip if type cannot be converted
                    }
                }
            }
        }
    }
    
    return config;
}

// Experiment template class
class NLMExperiment {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    std::string name;
    std::vector<std::map<std::string, double>> metrics;
    
    NLMExperiment(const std::shared_ptr<Config>& config, const std::string& experiment_name = "experiment")
        : name(experiment_name) {
        brain = std::make_shared<Brain>(config);
        world = std::make_shared<SimpleWorld>();
        agent = std::make_shared<AgentBrain>(brain);
    }
    
    void setup() {
        brain->initialize();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
    }
    
    virtual std::map<std::string, double> runEpisode(size_t steps = 1000) {
        std::map<std::string, double> results;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        results["spikes"] = brain->getTotalSpikeCount();
        results["firing_rate"] = brain->getAverageFiringRate();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        
        return results;
    }
    
    std::map<std::string, double> run(size_t num_episodes = 10) {
        metrics.clear();
        
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            // Reset for new episode
            brain->reset();
            world->reset();
            agent->reset();
            setup();
            
            auto result = runEpisode();
            metrics.push_back(result);
        }
        
        return analyzeResults();
    }
    
    std::map<std::string, double> analyzeResults() {
        std::map<std::string, double> analysis;
        size_t num_episodes = metrics.size();
        
        if (num_episodes == 0) return analysis;
        
        double total_spikes = 0, total_firing_rate = 0, total_firing_neurons = 0;
        for (const auto& metric : metrics) {
            total_spikes += metric.at("spikes");
            total_firing_rate += metric.at("firing_rate");
            total_firing_neurons += metric.at("firing_neurons");
        }
        
        analysis["episodes"] = static_cast<double>(num_episodes);
        analysis["average_spike_count"] = total_spikes / num_episodes;
        analysis["average_firing_rate"] = total_firing_rate / num_episodes;
        analysis["average_firing_neurons"] = total_firing_neurons / num_episodes;
        analysis["total_reward"] = 0.0; // Would accumulate from world results
        
        return analysis;
    }
};

// Neural Network Visualizer
class NeuralNetworkVisualizer {
public:
    static py::dict create_connection_matrix(const std::shared_ptr<Brain>& brain, bool normalize = true) {
        py::dict matrix;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            return matrix;
        }
        
        auto neurons = region->getAllNeurons();
        size_t n = neurons.size();
        
        std::vector<std::vector<float>> weight_matrix(n, std::vector<float>(n, 0.0f));
        
        for (size_t i = 0; i < n; ++i) {
            auto src_neuron = neurons[i];
            for (const auto& syn : region->getSynapses()) {
                if (syn->getSourceNeuron() == src_neuron->getId() && 
                    syn->getWeight() > 0) {
                    
                    auto dest_neuron = region->getNeuronById(syn->getDestinationNeuron());
                    if (dest_neuron) {
                        for (size_t j = 0; j < n; ++j) {
                            if (neurons[j]->getId() == dest_neuron->getId()) {
                                weight_matrix[i][j] = syn->getWeight();
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        if (normalize) {
            float max_val = 0.0f;
            for (const auto& row : weight_matrix) {
                for (float val : row) {
                    if (val > max_val) max_val = val;
                }
            }
            
            if (max_val > 0) {
                for (auto& row : weight_matrix) {
                    for (float& val : row) {
                        val /= max_val;
                    }
                }
            }
        }
        
        // Convert to Python dict
        for (size_t i = 0; i < n; ++i) {
            py::list row;
            for (size_t j = 0; j < n; ++j) {
                row.append(weight_matrix[i][j]);
            }
            matrix[static_cast<int>(i)] = row;
        }
        
        return matrix;
    }
    
    static py::dict analyze_neural_activity(const std::shared_ptr<Brain>& brain, 
                                            size_t window_size = 100) {
        py::dict patterns;
        
        patterns["total_spikes"] = brain->getTotalSpikeCount();
        patterns["active_neurons"] = brain->getFiringNeuronCount();
        patterns["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        patterns["developmental_stage"] = brain->getDevelopmentalStage().value;
        
        return patterns;
    }
    
    static py::dict create_brain_snapshot(const std::shared_ptr<Brain>& brain) {
        py::dict snapshot;
        
        snapshot["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        snapshot["neuron_count"] = brain->getTotalNeuronCount();
        snapshot["synapse_count"] = brain->getTotalSynapseCount();
        snapshot["firing_neuron_count"] = brain->getFiringNeuronCount();
        snapshot["total_spike_count"] = brain->getTotalSpikeCount();
        snapshot["average_firing_rate"] = brain->getAverageFiringRate();
        snapshot["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        auto stage = brain->getDevelopmentalStage();
        snapshot["developmental_stage"] = static_cast<int>(stage);
        
        auto config_summary = brain->getConfig().summary();
        snapshot["config_summary"] = config_summary;
        
        return snapshot;
    }
};

void bind_advanced_apis(pybind11::module_ &m) {
    // Factory function for advanced configurations
    m.def("create_advanced_config", &create_advanced_config,
          py::arg("config_type") = "reinforcement_learning",
          py::arg("custom_params") = py::dict(),
          "Create a configuration optimized for specific learning paradigms. "
          "Available config_types: 'reinforcement_learning', 'pattern_recognition', "
          "'memory_formation', 'agent_learning'. "
          "Can override defaults with custom_params dictionary.");
    
    // Experiment template class
    py::class_<NLMExperiment>(m, "NLMExperiment", 
        "Template class for running structured NLM experiments with logging and analysis.")
        .def(py::init<std::shared_ptr<Config>, std::string>(),
             py::arg("config"), py::arg("name") = "experiment")
        .def("setup", &NLMExperiment::setup,
             "Setup experiment components")
        .def("runEpisode", &NLMExperiment::runEpisode,
             py::arg("steps") = 1000,
             "Run a single episode (override in subclass)")
        .def("run", &NLMExperiment::run,
             py::arg("num_episodes") = 10,
             "Run multiple episodes and analyze results")
        .def("analyzeResults", &NLMExperiment::analyzeResults,
             "Analyze and aggregate results across episodes");
    
    // Neural network visualizer class
    py::class_<NeuralNetworkVisualizer>(m, "NeuralNetworkVisualizer", 
        "Advanced visualization tools for NLM brain networks.")
        .def_static("create_connection_matrix", &NeuralNetworkVisualizer::create_connection_matrix,
                    py::arg("brain"), py::arg("normalize") = true,
                    "Create a connection matrix visualization of neural connections")
        .def_static("analyze_neural_activity", &NeuralNetworkVisualizer::analyze_neural_activity,
                    py::arg("brain"), py::arg("window_size") = 100,
                    "Analyze neural activity patterns over time")
        .def_static("create_brain_snapshot", &NeuralNetworkVisualizer::create_brain_snapshot,
                    py::arg("brain"),
                    "Create a comprehensive brain snapshot for analysis");
}

} // namespace nlm

#endif // ADVANCED_APIS_H
