// Advanced features for power users and researchers
#ifndef NLM_ADVANCED_UTILS_H
#define NLM_ADVANCED_UTILS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <thread>
#include <future>
#include <chrono>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// Performance measurement utilities
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double total_time_ms;
        double average_step_time_ms;
        double min_step_time_ms;
        double max_step_time_ms;
        int64_t total_neurons;
        int64_t total_synapses;
        int64_t total_spikes;
        double average_firing_rate;
        int step_count;
        
        PerformanceMetrics() : total_time_ms(0.0), average_step_time_ms(0.0),
                              min_step_time_ms(0.0), max_step_time_ms(0.0),
                              total_neurons(0), total_synapses(0), total_spikes(0),
                              average_firing_rate(0.0), step_count(0) {}
    };
    
    static PerformanceMetrics profile_brain(const Brain& brain, int steps, bool warmup = true) {
        PerformanceMetrics metrics;
        
        if (warmup) {
            for (int i = 0; i < steps / 10; ++i) {  // Warmup with 10% of steps
                brain.step(i);
            }
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            auto step_start = std::chrono::high_resolution_clock::now();
            brain.step(i);
            auto step_end = std::chrono::high_resolution_clock::now();
            
            auto step_duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start).count();
            double step_time_ms = step_duration / 1000.0;
            
            metrics.total_time_ms += step_time_ms;
            metrics.min_step_time_ms = (i == 0) ? step_time_ms : std::min(metrics.min_step_time_ms, step_time_ms);
            metrics.max_step_time_ms = std::max(metrics.max_step_time_ms, step_time_ms);
            
            metrics.total_neurons = std::max(metrics.total_neurons, brain.getTotalNeuronCount());
            metrics.total_synapses = std::max(metrics.total_synapses, brain.getTotalSynapseCount());
            metrics.total_spikes = std::max(metrics.total_spikes, brain.getTotalSpikeCount());
            metrics.average_firing_rate = std::max(metrics.average_firing_rate, brain.getAverageFiringRate());
            metrics.step_count = steps;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double total_time_ms = total_duration / 1000.0;
        
        if (metrics.step_count > 0) {
            metrics.average_step_time_ms = metrics.total_time_ms / metrics.step_count;
            metrics.total_time_ms = total_time_ms;
        }
        
        return metrics;
    }
    
    static std::string metrics_to_string(const PerformanceMetrics& metrics) {
        std::stringstream ss;
        ss << "Performance Metrics:\n";
        ss << "  Total time: " << metrics.total_time_ms << " ms\n";
        ss << "  Average step time: " << metrics.average_step_time_ms << " ms\n";
        ss << "  Min step time: " << metrics.min_step_time_ms << " ms\n";
        ss << "  Max step time: " << metrics.max_step_time_ms << " ms\n";
        ss << "  Total neurons: " << metrics.total_neurons << "\n";
        ss << "  Total synapses: " << metrics.total_synapses << "\n";
        ss << "  Total spikes: " << metrics.total_spikes << "\n";
        ss << "  Average firing rate: " << metrics.average_firing_rate << " Hz\n";
        ss << "  Steps per second: " << (metrics.step_count / (metrics.total_time_ms / 1000.0)) << "\n";
        return ss.str();
    }
};

// Batch processing utilities
class BatchProcessor {
public:
    struct BatchResult {
        std::vector<std::shared_ptr<Brain>> brains;
        std::vector<std::shared_ptr<AgentBrain>> agents;
        std::vector<std::shared_ptr<SimpleWorld>> worlds;
        std::vector<ValidationResult> validation_results;
        std::vector<std::string> errors;
        bool success;
        
        BatchResult() : success(true) {}
    };
    
    static BatchResult batch_simulate(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::vector<int>& step_counts,
        int num_threads = 1
    ) {
        BatchResult result;
        
        if (configs.size() != step_counts.size()) {
            result.success = false;
            result.errors.push_back("Number of configs must match number of step counts");
            return result;
        }
        
        if (num_threads <= 0) {
            num_threads = 1;
        }
        
        // Determine batch sizes for each thread
        int batch_size = configs.size() / num_threads;
        int remainder = configs.size() % num_threads;
        
        std::vector<std::vector<int>> thread_batches(num_threads);
        size_t start = 0;
        
        for (int t = 0; t < num_threads; ++t) {
            int current_batch_size = batch_size + (t < remainder ? 1 : 0);
            thread_batches[t].resize(current_batch_size);
            
            for (int i = 0; i < current_batch_size; ++i) {
                thread_batches[t][i] = start++;
            }
        }
        
        // Process each thread
        std::vector<std::future<BatchResult>> futures;
        
        for (int t = 0; t < num_threads; ++t) {
            futures.push_back(std::async(std::launch::async, [configs, step_counts, &thread_batches, t]() {
                BatchResult local_result;
                
                for (int batch_index : thread_batches[t]) {
                    try {
                        auto config = configs[batch_index];
                        int steps = step_counts[batch_index];
                        
                        // Validate configuration
                        auto validation = ConfigValidator::validate(*config);
                        local_result.validation_results.push_back(validation);
                        
                        if (!validation.is_valid) {
                            local_result.errors.push_back("Config " + std::to_string(batch_index) + ": " + validation.get_summary());
                        }
                        
                        // Create and run brain
                        auto brain = std::make_shared<Brain>(config);
                        brain->initialize();
                        
                        for (int step = 0; step < steps; ++step) {
                            brain->step(step);
                        }
                        
                        local_result.brains.push_back(brain);
                        
                    } catch (const std::exception& e) {
                        local_result.success = false;
                        local_result.errors.push_back("Error in batch " + std::to_string(batch_index) + ": " + std::string(e.what()));
                    }
                }
                
                return local_result;
            }));
        }
        
        // Collect results
        for (auto& future : futures) {
            auto local_result = future.get();
            
            result.brains.insert(result.brains.end(), 
                                local_result.brains.begin(), local_result.brains.end());
            result.agents.insert(result.agents.end(), 
                                local_result.agents.begin(), local_result.agents.end());
            result.worlds.insert(result.worlds.end(), 
                               local_result.worlds.begin(), local_result.worlds.end());
            result.validation_results.insert(result.validation_results.end(), 
                                           local_result.validation_results.begin(), local_result.validation_results.end());
            result.errors.insert(result.errors.end(), 
                               local_result.errors.begin(), local_result.errors.end());
            
            if (!local_result.success) {
                result.success = false;
            }
        }
        
        return result;
    }
    
    static BatchResult batch_synchronize(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<std::shared_ptr<AgentBrain>>& agents,
        const std::vector<std::shared_ptr<SimpleWorld>>& worlds
    ) {
        BatchResult result;
        
        result.brains = brains;
        result.agents = agents;
        result.worlds = worlds;
        
        // Validate all components
        for (const auto& brain : brains) {
            auto validation = BrainValidator::validate(*brain);
            result.validation_results.push_back(validation);
            if (!validation.is_valid) {
                result.errors.push_back("Brain validation failed: " + validation.get_summary());
            }
        }
        
        for (const auto& agent : agents) {
            auto validation = AgentBrainValidator::validate(*agent);
            result.validation_results.push_back(validation);
            if (!validation.is_valid) {
                result.errors.push_back("Agent validation failed: " + validation.get_summary());
            }
        }
        
        for (const auto& world : worlds) {
            auto validation = WorldValidator::validate(*world);
            result.validation_results.push_back(validation);
            if (!validation.is_valid) {
                result.errors.push_back("World validation failed: " + validation.get_summary());
            }
        }
        
        result.success = result.errors.empty();
        return result;
    }
};

// Advanced configuration management
class AdvancedConfigManager {
public:
    struct ConfigurationProfile {
        std::string name;
        std::map<std::string, std::string> settings;
        std::string description;
        std::string author;
        std::string version;
        std::chrono::system_clock::time_point created_at;
        
        ConfigurationProfile(const std::string& name, const std::map<std::string, std::string>& settings,
                           const std::string& description = "", const std::string& author = "",
                           const std::string& version = "1.0.0")
            : name(name), settings(settings), description(description), author(author),
              version(version), created_at(std::chrono::system_clock::now()) {}
    };
    
    static std::vector<ConfigurationProfile> load_config_library(const std::string& directory) {
        std::vector<ConfigurationProfile> profiles;
        
        // This is a placeholder implementation
        // In a real implementation, you would scan the directory for config files
        // and load them from JSON, YAML, or other formats
        
        return profiles;
    }
    
    static bool save_config_profile(const ConfigurationProfile& profile, const std::string& directory) {
        // This is a placeholder implementation
        // In a real implementation, you would save the profile to a file
        return true;
    }
    
    static std::shared_ptr<Config> merge_configs(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::map<std::string, std::string>& overrides
    ) {
        auto merged_config = std::make_shared<Config>();
        
        // Merge all base configurations
        for (const auto& config : configs) {
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                if (config->has(key)) {
                    merged_config->set(key, config->getString(key, ""), nlm::ConfigSource::Runtime);
                }
            }
        }
        
        // Apply overrides
        for (const auto& [key, value] : overrides) {
            if (key.find('.') != std::string::npos) {
                // Convert string value to appropriate type
                if (value == "true" || value == "false") {
                    merged_config->set(key, value == "true", nlm::ConfigSource::Runtime);
                } else if (value.find('.') != std::string::npos) {
                    try {
                        double double_value = std::stod(value);
                        merged_config->set(key, double_value, nlm::ConfigSource::Runtime);
                    } catch (...) {
                        merged_config->set(key, value, nlm::ConfigSource::Runtime);
                    }
                } else {
                    try {
                        int64_t int_value = std::stoll(value);
                        merged_config->set(key, int_value, nlm::ConfigSource::Runtime);
                    } catch (...) {
                        merged_config->set(key, value, nlm::ConfigSource::Runtime);
                    }
                }
            }
        }
        
        return merged_config;
    }
    
    static std::map<std::string, std::string> extract_config_summary(const Config& config) {
        std::map<std::string, std::string> summary;
        
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            if (config.has(key)) {
                auto str_value = config.getString(key, "");
                if (!str_value.empty()) {
                    summary[key] = str_value;
                }
            }
        }
        
        return summary;
    }
};

// Export/import utilities
class StateExporter {
public:
    static std::string export_brain_state(const Brain& brain) {
        // Export brain state as JSON string
        // In a real implementation, you would serialize the neural network state
        std::stringstream ss;
        ss << "{\"neurons\": " << brain.getTotalNeuronCount()
           << ", \"synapses\": " << brain.getTotalSynapseCount()
           << ", \"spikes\": " << brain.getTotalSpikeCount()
           << ", \"stage\": " << static_cast<int>(brain.getDevelopmentalStage())
           << "}";
        return ss.str();
    }
    
    static std::string export_agent_state(const AgentBrain& agent) {
        // Export agent state as JSON string
        std::stringstream ss;
        ss << "{\"curiosity\": " << agent.getCuriosityLevel()
           << ", \"novelty\": " << agent.getNoveltyLevel()
           << ", \"prediction_error\": " << agent.getPredictionError()
           << ", \"neuromodulation\": " << agent.getNeuromodulationLevel()
           << "}";
        return ss.str();
    }
    
    static std::string export_world_state(const SimpleWorld& world) {
        // Export world state as JSON string
        std::stringstream ss;
        ss << "{\"width\": " << world.getWidth()
           << ", \"height\": " << world.getHeight()
           << ", \"time\": " << world.getSimulationTime()
           << ", \"max_energy\": " << world.getMaxEnergy()
           << "}";
        return ss.str();
    }
    
    static std::string export_complete_system(
        const Brain& brain,
        const AgentBrain& agent,
        const SimpleWorld& world
    ) {
        std::stringstream ss;
        ss << "{\"brain\": " << export_brain_state(brain)
           << ", \"agent\": " << export_agent_state(agent)
           << ", \"world\": " << export_world_state(world)
           << "}";
        return ss.str();
    }
    
    static std::string export_performance_metrics(const PerformanceProfiler::PerformanceMetrics& metrics) {
        std::stringstream ss;
        ss << "{\"total_time_ms\": " << metrics.total_time_ms
           << ", \"average_step_time_ms\": " << metrics.average_step_time_ms
           << ", \"min_step_time_ms\": " << metrics.min_step_time_ms
           << ", \"max_step_time_ms\": " << metrics.max_step_time_ms
           << ", \"total_neurons\": " << metrics.total_neurons
           << ", \"total_synapses\": " << metrics.total_synapses
           << ", \"total_spikes\": " << metrics.total_spikes
           << ", \"average_firing_rate\": " << metrics.average_firing_rate
           << ", \"steps_per_second\": " << (metrics.step_count / (metrics.total_time_ms / 1000.0))
           << "}";
        return ss.str();
    }
};

// Custom event system
class EventSystem {
public:
    struct Event {
        std::string type;
        double timestamp;
        std::map<std::string, std::string> data;
        std::string source;
        
        Event(const std::string& type, double timestamp, const std::map<std::string, std::string>& data,
              const std::string& source = "system")
            : type(type), timestamp(timestamp), data(data), source(source) {}
    };
    
    struct EventListener {
        std::string name;
        std::function<bool(const Event&)> callback;
        bool enabled;
        
        EventListener(const std::string& name, std::function<bool(const Event&)> callback)
            : name(name), callback(callback), enabled(true) {}
    };
    
    class EventManager {
    private:
        std::vector<EventListener> listeners;
        std::vector<Event> event_history;
        double current_time;
        
    public:
        EventManager() : current_time(0.0) {}
        
        void add_listener(const EventListener& listener) {
            listeners.push_back(listener);
        }
        
        void remove_listener(const std::string& name) {
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                                         [&name](const EventListener& listener) {
                                             return listener.name == name;
                                         }),
                           listeners.end());
        }
        
        void emit_event(const std::string& type, const std::map<std::string, std::string>& data, const std::string& source = "system") {
            Event event(type, current_time, data, source);
            event_history.push_back(event);
            
            for (auto& listener : listeners) {
                if (listener.enabled) {
                    try {
                        listener.callback(event);
                    } catch (const std::exception& e) {
                        // Log error but continue processing other listeners
                    }
                }
            }
        }
        
        void advance_time(double dt) {
            current_time += dt;
        }
        
        const std::vector<Event>& get_event_history() const {
            return event_history;
        }
        
        void clear_event_history() {
            event_history.clear();
        }
        
        void enable_listener(const std::string& name, bool enabled) {
            for (auto& listener : listeners) {
                if (listener.name == name) {
                    listener.enabled = enabled;
                    break;
                }
            }
        }
        
        std::string get_listener_status() const {
            std::stringstream ss;
            ss << "Event Listeners: " << listeners.size() << "\n";
            for (const auto& listener : listeners) {
                ss << "  - " << listener.name << " (" << (listener.enabled ? "enabled" : "disabled") << ")\n";
            }
            return ss.str();
        }
    };
    
    static void setup_default_events(EventManager& event_manager) {
        // Setup default event listeners
        event_manager.add_listener(EventListener("brain_step", [event_manager](const Event& event) {
            if (event.type == "brain_step") {
                std::cout << "Event: Brain step at time " << event.timestamp << std::endl;
            }
            return true;
        }));
        
        event_manager.add_listener(EventListener("neuron_spike", [event_manager](const Event& event) {
            if (event.type == "neuron_spike") {
                std::cout << "Event: Neuron spike detected at time " << event.timestamp << std::endl;
            }
            return true;
        }));
        
        event_manager.add_listener(EventListener("learning_event", [event_manager](const Event& event) {
            if (event.type == "learning_event") {
                std::cout << "Event: Learning event at time " << event.timestamp << std::endl;
            }
            return true;
        }));
    }
};

// Debugging utilities
class DebugUtils {
public:
    static std::string generate_debug_report(const Brain& brain, const std::string& title = "Debug Report") {
        std::stringstream ss;
        ss << "=== " << title << " ===\n";
        ss << "Brain State:\n";
        ss << "  Total neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "  Total synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "  Firing neurons: " << brain.getFiringNeuronCount() << "\n";
        ss << "  Total spikes: " << brain.getTotalSpikeCount() << "\n";
        ss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
        ss << "  E/I ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "  Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        ss << "\nValidation:\n";
        auto validation = BrainValidator::validate(brain);
        ss << "  " << validation.get_summary() << "\n";
        
        if (!validation.errors.empty()) {
            ss << "  Errors:\n";
            for (const auto& error : validation.errors) {
                ss << "    - " << error << "\n";
            }
        }
        
        if (!validation.warnings.empty()) {
            ss << "  Warnings:\n";
            for (const auto& warning : validation.warnings) {
                ss << "    - " << warning << "\n";
            }
        }
        
        return ss.str();
    }
    
    static std::string format_memory_usage(size_t bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        } else if (bytes < 1024 * 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else if (bytes < 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        }
    }
    
    static std::string get_system_info() {
        std::stringstream ss;
        ss << "System Information:\n";
        ss << "  Platform: " << (sizeof(void*) == 8 ? "64-bit" : "32-bit") << "\n";
        ss << "  Threads: " << std::thread::hardware_concurrency() << "\n";
        ss << "  Compiler: " << __VERSION__ << "\n";
        return ss.str();
    }
};

// Advanced plasticity rules
class AdvancedPlasticity {
public:
    struct PlasticityRule {
        std::string name;
        std::string description;
        std::map<std::string, double> parameters;
        
        PlasticityRule(const std::string& name, const std::string& description)
            : name(name), description(description) {}
    };
    
    struct CustomPlasticityConfig {
        std::vector<PlasticityRule> rules;
        std::map<std::string, double> global_parameters;
        
        CustomPlasticityConfig() {}
        
        void add_rule(const PlasticityRule& rule) {
            rules.push_back(rule);
        }
        
        void set_global_parameter(const std::string& key, double value) {
            global_parameters[key] = value;
        }
        
        double get_global_parameter(const std::string& key, double default_value = 0.0) const {
            auto it = global_parameters.find(key);
            return (it != global_parameters.end()) ? it->second : default_value;
        }
    };
    
    static CustomPlasticityConfig create_advanced_config() {
        CustomPlasticityConfig config;
        
        // Add common plasticity rules
        config.add_rule(PlasticityRule("STDP", "Spike-Timing Dependent Plasticity"));
        config.add_rule(PlasticityRule("Hebbian", "Hebbian learning based on co-activation"));
        config.add_rule(PlasticityRule("Homeostatic", "Homeostatic regulation of firing rates"));
        config.add_rule(PlasticityRule("Neuromodulatory", "Neuromodulator-driven plasticity"));
        
        // Set global parameters
        config.set_global_parameter("learning_rate", 0.01);
        config.set_global_parameter("decay_rate", 0.001);
        config.set_global_parameter("max_weight", 1.0);
        config.set_global_parameter("min_weight", 0.0);
        
        return config;
    }
    
    static std::string analyze_plasticity_impact(const Brain& brain, const CustomPlasticityConfig& config) {
        std::stringstream ss;
        ss << "Plasticity Impact Analysis:\n";
        ss << "  Rules configured: " << config.rules.size() << "\n";
        ss << "  Global parameters: " << config.global_parameters.size() << "\n";
        
        // Analyze brain activity for plasticity effects
        double total_weight = 0.0;
        double max_weight = 0.0;
        double min_weight = 0.0;
        
        // In a real implementation, you would analyze the actual synaptic weights
        // For now, provide a placeholder analysis
        ss << "  Estimated weight range: " << config.get_global_parameter("min_weight", 0.0) 
           << " to " << config.get_global_parameter("max_weight", 1.0) << "\n";
        ss << "  Learning rate: " << config.get_global_parameter("learning_rate", 0.01) << "\n";
        ss << "  Decay rate: " << config.get_global_parameter("decay_rate", 0.001) << "\n";
        
        return ss.str();
    }
};

} // namespace nlm

#endif // NLM_ADVANCED_UTILS_H