#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

#include "bindings.cpp"

namespace py = pybind11;
namespace nlm {

// Define the classes used in advanced bindings
struct SimulationResult {
    int episode = 0;
    size_t total_steps = 0;
    float total_reward = 0.0f;
    float avg_firing_rate = 0.0f;
    float final_energy = 0.0f;
    float final_curiosity = 0.0f;
    float final_novelty = 0.0f;
    float final_dopamine = 0.0f;
    size_t synapse_count = 0;
    std::map<std::string, float> action_counts;
    
    void add_action_count(const std::string& action, float count) {
        action_counts[action] = count;
    }
    
    float get_action_count(const std::string& action) const {
        auto it = action_counts.find(action);
        return it != action_counts.end() ? it->second : 0.0f;
    }
};

struct BatchSimulationParams {
    size_t num_episodes = 10;
    size_t steps_per_episode = 100;
    float timestep = 0.1f;
    bool enable_progress = true;
    size_t progress_interval = 10;
    size_t save_interval = 100;
    std::string checkpoint_path = "./checkpoint.bin";
    bool enable_early_stop = false;
    float reward_threshold = 100.0f;
    size_t max_steps = 10000;
};

struct PerformanceMetrics {
    double cpu_time = 0.0;
    double wall_time = 0.0;
    double memory_usage_mb = 0.0;
    double neurons_per_second = 0.0;
    double synapses_per_second = 0.0;
    double spikes_per_second = 0.0;
    double steps_per_second = 0.0;
};

class BatchSimulationRunner {
public:
    std::vector<SimulationResult> run_batch_simulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        BatchSimulationParams params) {
        std::vector<SimulationResult> results;
        
        for (size_t episode = 0; episode < params.num_episodes; ++episode) {
            SimulationResult result;
            result.episode = episode;
            
            brain->reset();
            agent->reset();
            world->reset();
            
            float total_reward = 0.0f;
            float total_firing_rate = 0.0f;
            size_t firing_count = 0;
            
            for (size_t step = 0; step < params.steps_per_episode; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto reward = world->getSensoryPercept();
                agent->applyRewardModulation(0.0f, 0.0f);
                agent->updateDevelopment(0.1f);
                world->update(params.timestep);
                
                total_reward += 0.0f; // Simplified reward calculation
                total_firing_rate += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) firing_count++;
                
                if (params.enable_progress && (step % params.progress_interval) == 0) {
                    std::cout << "Episode " << episode << ", Step " << step 
                             << "/" << params.steps_per_episode 
                             << ", Reward: " << total_reward / (step + 1)
                             << ", Firing: " << brain->getAverageFiringRate()
                             << std::endl;
                }
                
                if (params.save_interval > 0 && (step % params.save_interval) == 0) {
                    std::string path = params.checkpoint_path + "_ep" + 
                                     std::to_string(episode) + "_step" + 
                                     std::to_string(step) + ".bin";
                    brain->save(path);
                }
                
                if (params.enable_early_stop && total_reward > params.reward_threshold) {
                    result.total_steps = step + 1;
                    break;
                }
            }
            
            result.total_steps = params.steps_per_episode;
            result.total_reward = total_reward / params.steps_per_episode;
            result.avg_firing_rate = total_firing_rate / params.steps_per_episode;
            result.final_energy = world->getAgentBody().energy;
            result.final_curiosity = agent->getCuriosityLevel();
            result.final_novelty = agent->getNoveltyLevel();
            result.final_dopamine = agent->getNeuromodulationLevel();
            result.synapse_count = brain->getTotalSynapseCount();
            
            results.push_back(result);
            
            if (params.enable_progress) {
                std::cout << "Episode " << episode << " complete: "
                         << "Reward: " << result.total_reward
                         << ", Firing Rate: " << result.avg_firing_rate
                         << std::endl;
            }
            
            if (params.enable_early_stop && total_reward > params.reward_threshold) {
                std::cout << "Early stopping condition met at episode " << episode
                         << " (reward: " << total_reward << ")" << std::endl;
                break;
            }
        }
        
        return results;
    }
    
    std::vector<SimulationResult> run_multiple_episodes(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_episodes,
        size_t steps_per_episode) {
        BatchSimulationParams params;
        params.num_episodes = num_episodes;
        params.steps_per_episode = steps_per_episode;
        return run_batch_simulation(brain, world, agent, params);
    }
    
    std::vector<SimulationResult> run_experiments(
        std::shared_ptr<Config> config,
        const std::vector<std::string>& experiment_names) {
        std::vector<SimulationResult> all_results;
        
        for (const auto& name : experiment_names) {
            auto exp_config = std::make_shared<Config>(*config);
            
            // Configure based on experiment name
            if (name == "baseline") {
                exp_config->set("neuron_count", 500);
                exp_config->set("connection_probability", 0.1f);
            } else if (name == "high_learning") {
                exp_config->set("neuron_count", 1000);
                exp_config->set("connection_probability", 0.2f);
                exp_config->set("stdp_ltp_weight", 0.02f);
            } else if (name == "fast_learning") {
                exp_config->set("neuron_count", 300);
                exp_config->set("connection_probability", 0.05f);
                exp_config->set("synaptogenesis_rate", 0.001f);
            }
            
            auto brain = std::make_shared<Brain>(exp_config);
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);
            
            world->configure(20.0f, 20.0f, 16, 16);
            world->reset();
            
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            
            BatchSimulationParams params;
            params.num_episodes = 5;
            params.steps_per_episode = 200;
            
            auto results = run_batch_simulation(brain, world, agent, params);
            all_results.insert(all_results.end(), results.begin(), results.end());
        }
        
        return all_results;
    }
    
    std::vector<SimulationResult> compare_configurations(
        const std::vector<std::shared_ptr<Config>>& config_list) {
        std::vector<SimulationResult> all_results;
        
        for (const auto& config : config_list) {
            auto brain = std::make_shared<Brain>(config);
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);
            
            world->configure(20.0f, 20.0f, 16, 16);
            world->reset();
            
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            
            BatchSimulationParams params;
            params.num_episodes = 3;
            params.steps_per_episode = 100;
            
            auto results = run_batch_simulation(brain, world, agent, params);
            all_results.insert(all_results.end(), results.begin(), results.end());
        }
        
        return all_results;
    }
    
    bool save_results(const std::vector<SimulationResult>& results,
                     const std::string& filepath) {
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file) return false;
            
            // Simple binary serialization
            size_t size = results.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
            
            for (const auto& result : results) {
                file.write(reinterpret_cast<const char*>(&result), sizeof(SimulationResult));
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error saving results: " << e.what() << std::endl;
            return false;
        }
    }
    
    std::vector<SimulationResult> load_results(const std::string& filepath) {
        std::vector<SimulationResult> results;
        
        try {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) return results;
            
            size_t size;
            file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            
            results.resize(size);
            file.read(reinterpret_cast<char*>(results.data()), size * sizeof(SimulationResult));
            
            file.close();
            return results;
        } catch (const std::exception& e) {
            std::cerr << "Error loading results: " << e.what() << std::endl;
            return results;
        }
    }
};

class PerformanceAnalyzer {
public:
    PerformanceMetrics analyze_performance(const PerformanceMetrics& metrics) {
        PerformanceMetrics analysis = metrics;
        
        // Calculate derived metrics
        if (metrics.wall_time > 0) {
            analysis.neurons_per_second = 500.0 / metrics.wall_time; // Placeholder
            analysis.synapses_per_second = 2000.0 / metrics.wall_time; // Placeholder
            analysis.spikes_per_second = 5000.0 / metrics.wall_time; // Placeholder
            analysis.steps_per_second = 100.0 / metrics.wall_time; // Placeholder
        }
        
        return analysis;
    }
    
    double get_efficiency_rating(const PerformanceMetrics& metrics) {
        double base_efficiency = 100.0;
        
        if (metrics.wall_time > 10.0) {
            base_efficiency *= 0.5;
        }
        
        if (metrics.memory_usage_mb > 500.0) {
            base_efficiency *= 0.8;
        }
        
        return std::max(0.0, base_efficiency);
    }
    
    std::vector<std::string> get_optimization_suggestions(const PerformanceMetrics& metrics) {
        std::vector<std::string> suggestions;
        
        if (metrics.wall_time > 5.0) {
            suggestions.push_back("Consider reducing simulation steps or increasing timestep");
        }
        
        if (metrics.memory_usage_mb > 300.0) {
            suggestions.push_back("Consider using more memory-efficient data structures");
        }
        
        if (metrics.steps_per_second < 10.0) {
            suggestions.push_back("Investigate performance bottlenecks in neural dynamics");
        }
        
        if (suggestions.empty()) {
            suggestions.push_back("Performance is within acceptable range");
        }
        
        return suggestions;
    }
    
    std::string compare_performance(const std::vector<PerformanceMetrics>& metrics_list) {
        if (metrics_list.empty()) return "No metrics to compare";
        
        double avg_wall_time = 0.0;
        double avg_memory = 0.0;
        
        for (const auto& metrics : metrics_list) {
            avg_wall_time += metrics.wall_time;
            avg_memory += metrics.memory_usage_mb;
        }
        
        avg_wall_time /= metrics_list.size();
        avg_memory /= metrics_list.size();
        
        std::stringstream ss;
        ss << "Performance Comparison:" << std::endl;
        ss << "Average wall time: " << avg_wall_time << "s" << std::endl;
        ss << "Average memory usage: " << avg_memory << "MB" << std::endl;
        
        if (metrics_list.size() > 1) {
            ss << std::endl << "Performance variance:" << std::endl;
            for (size_t i = 0; i < metrics_list.size(); ++i) {
                double wall_time_diff = metrics_list[i].wall_time - avg_wall_time;
                ss << "Run " << i << ": wall time difference: " << wall_time_diff << "s" << std::endl;
            }
        }
        
        return ss.str();
    }
};

class ResultAggregator {
public:
    void add_result(const SimulationResult& result) {
        results.push_back(result);
    }
    
    SimulationResult aggregate_results() {
        SimulationResult aggregated;
        
        if (results.empty()) return aggregated;
        
        for (const auto& result : results) {
            aggregated.episode += result.episode;
            aggregated.total_steps += result.total_steps;
            aggregated.total_reward += result.total_reward;
            aggregated.avg_firing_rate += result.avg_firing_rate;
            aggregated.final_energy += result.final_energy;
            aggregated.final_curiosity += result.final_curiosity;
            aggregated.final_novelty += result.final_novelty;
            aggregated.final_dopamine += result.final_dopamine;
            aggregated.synapse_count += result.synapse_count;
            
            for (const auto& pair : result.action_counts) {
                aggregated.action_counts[pair.first] += pair.second;
            }
        }
        
        size_t count = results.size();
        aggregated.episode /= count;
        aggregated.total_reward /= count;
        aggregated.avg_firing_rate /= count;
        aggregated.final_energy /= count;
        aggregated.final_curiosity /= count;
        aggregated.final_novelty /= count;
        aggregated.final_dopamine /= count;
        aggregated.synapse_count /= count;
        
        return aggregated;
    }
    
    SimulationResult get_best_result(float (*metric_func)(const SimulationResult&)) {
        if (results.empty()) return SimulationResult();
        
        size_t best_index = 0;
        float best_value = metric_func(results[0]);
        
        for (size_t i = 1; i < results.size(); ++i) {
            float value = metric_func(results[i]);
            if (value > best_value) {
                best_value = value;
                best_index = i;
            }
        }
        
        return results[best_index];
    }
    
    std::string get_statistics() {
        if (results.empty()) return "No results to analyze";
        
        std::stringstream ss;
        ss << "Result Statistics:" << std::endl;
        ss << "Total simulations: " << results.size() << std::endl;
        ss << "Average reward: " << get_average_reward() << std::endl;
        ss << "Average firing rate: " << get_average_firing_rate() << std::endl;
        ss << "Best reward: " << get_best_reward() << std::endl;
        ss << "Worst reward: " << get_worst_reward() << std::endl;
        
        return ss.str();
    }
    
private:
    std::vector<SimulationResult> results;
    
    float get_average_reward() const {
        float sum = 0.0f;
        for (const auto& result : results) {
            sum += result.total_reward;
        }
        return results.empty() ? 0.0f : sum / results.size();
    }
    
    float get_average_firing_rate() const {
        float sum = 0.0f;
        for (const auto& result : results) {
            sum += result.avg_firing_rate;
        }
        return results.empty() ? 0.0f : sum / results.size();
    }
    
    float get_best_reward() const {
        float best = std::numeric_limits<float>::lowest();
        for (const auto& result : results) {
            if (result.total_reward > best) best = result.total_reward;
        }
        return best;
    }
    
    float get_worst_reward() const {
        float worst = std::numeric_limits<float>::max();
        for (const auto& result : results) {
            if (result.total_reward < worst) worst = result.total_reward;
        }
        return worst;
    }
};

class ProgressTracker {
public:
    void start(size_t total_steps) {
        this->total_steps = total_steps;
        current_step = 0;
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void update(size_t step) {
        current_step = step;
        auto end_time = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count() / 1000.0;
    }
    
    double get_progress() const {
        if (total_steps == 0) return 0.0;
        return static_cast<double>(current_step) / total_steps * 100.0;
    }
    
    bool is_complete() const {
        return current_step >= total_steps;
    }
    
    double get_eta() const {
        if (current_step == 0 || elapsed == 0.0) return 0.0;
        double rate = static_cast<double>(current_step) / elapsed;
        return (total_steps - current_step) / rate;
    }
    
    void set_callback(std::function<void(double)> callback) {
        progress_callback = callback;
    }
    
private:
    size_t total_steps = 0;
    size_t current_step = 0;
    double elapsed = 0.0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::function<void(double)> progress_callback;
};

class SimulationUtilities {
public:
    std::shared_ptr<Brain> create_brain_from_template(const std::string& template_name) {
        auto config = std::make_shared<Config>();
        
        if (template_name == "simple") {
            config->set("neuron_count", 500);
            config->set("region_count", 2);
        } else if (template_name == "large") {
            config->set("neuron_count", 2000);
            config->set("region_count", 4);
        } else if (template_name == "sparse") {
            config->set("neuron_count", 200);
            config->set("region_count", 1);
        }
        
        return std::make_shared<Brain>(config);
    }
    
    std::vector<std::string> validate_environment(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> errors;
        
        if (!brain || !brain->initialize()) {
            errors.push_back("Brain initialization failed");
        }
        
        if (!world) {
            errors.push_back("World object is null");
        }
        
        if (!agent) {
            errors.push_back("Agent brain interface is null");
        }
        
        return errors;
    }
    
    void optimize_parameters(
        std::shared_ptr<Brain> brain,
        const std::map<std::string, float>& target_metrics) {
        auto config = brain->getConfig();
        
        if (target_metrics.count("speed")) {
            float speed_target = target_metrics.at("speed");
            config->set("simulation_timestep", 0.001f * (10.0f / speed_target));
        }
        
        if (target_metrics.count("learning")) {
            float learning_target = target_metrics.at("learning");
            config->set("stdp_ltp_weight", 0.01f * learning_target);
        }
        
        brain->reset();
    }
    
    bool generate_report(
        const std::vector<SimulationResult>& results,
        const std::string& output_file) {
        try {
            std::ofstream file(output_file);
            if (!file) return false;
            
            file << "NLM Advanced Simulation Report" << std::endl;
            file << "=================================" << std::endl;
            file << "Total simulations: " << results.size() << std::endl;
            file << "Average reward: " << get_average_reward(results) << std::endl;
            file << "Average firing rate: " << get_average_firing_rate(results) << std::endl;
            file << "Average energy: " << get_average_energy(results) << std::endl;
            
            file << std::endl << "Detailed Results:" << std::endl;
            for (size_t i = 0; i < results.size(); ++i) {
                file << "Simulation " << i << ":" << std::endl;
                file << "  Episode: " << results[i].episode << std::endl;
                file << "  Total Reward: " << results[i].total_reward << std::endl;
                file << "  Firing Rate: " << results[i].avg_firing_rate << std::endl;
                file << "  Energy: " << results[i].final_energy << std::endl;
                file << std::endl;
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error generating report: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    float get_average_reward(const std::vector<SimulationResult>& results) const {
        float sum = 0.0f;
        for (const auto& result : results) {
            sum += result.total_reward;
        }
        return results.empty() ? 0.0f : sum / results.size();
    }
    
    float get_average_firing_rate(const std::vector<SimulationResult>& results) const {
        float sum = 0.0f;
        for (const auto& result : results) {
            sum += result.avg_firing_rate;
        }
        return results.empty() ? 0.0f : sum / results.size();
    }
    
    float get_average_energy(const std::vector<SimulationResult>& results) const {
        float sum = 0.0f;
        for (const auto& result : results) {
            sum += result.final_energy;
        }
        return results.empty() ? 0.0f : sum / results.size();
    }
};

std::vector<std::string> validate_simulation_setup(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent) {
    std::vector<std::string> errors;
    
    if (!brain || !brain->initialize()) {
        errors.push_back("Brain initialization failed");
    }
    
    if (!world) {
        errors.push_back("World object is null");
    }
    
    if (!agent) {
        errors.push_back("Agent brain interface is null");
    }
    
    return errors;
}

PerformanceMetrics benchmark_simulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t steps,
    size_t repeats) {
    PerformanceMetrics metrics;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (size_t repeat = 0; repeat < repeats; ++repeat) {
        if (brain && world && agent) {
            brain->reset();
            agent->reset();
            world->reset();
            
            for (size_t step = 0; step < steps; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(0.0f, 0.0f);
                agent->updateDevelopment(0.1f);
                world->update(0.1f);
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time).count() / 1000.0;
    
    metrics.wall_time = duration;
    metrics.steps_per_second = (steps * repeats) / metrics.wall_time;
    
    return metrics;
}

} // namespace nlm

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Python API
        ----------------------
        Enhanced Python bindings for NLM with advanced features:
        - Batch processing for multiple simulations
        - Performance monitoring and profiling
        - Advanced error handling and validation
        - Result aggregation and analysis
        - Progress tracking and callbacks
        - Enhanced configuration validation
        - Advanced simulation utilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Simulation Result
    py::class_<nlm::SimulationResult>(m, "SimulationResult")
        .def(py::init<>())
        .def_readwrite("episode", &nlm::SimulationResult::episode)
        .def_readwrite("total_steps", &nlm::SimulationResult::total_steps)
        .def_readwrite("total_reward", &nlm::SimulationResult::total_reward)
        .def_readwrite("avg_firing_rate", &nlm::SimulationResult::avg_firing_rate)
        .def_readwrite("final_energy", &nlm::SimulationResult::final_energy)
        .def_readwrite("final_curiosity", &nlm::SimulationResult::final_curiosity)
        .def_readwrite("final_novelty", &nlm::SimulationResult::final_novelty)
        .def_readwrite("final_dopamine", &nlm::SimulationResult::final_dopamine)
        .def_readwrite("synapse_count", &nlm::SimulationResult::synapse_count)
        .def("add_action_count", &nlm::SimulationResult::add_action_count)
        .def("get_action_count", &nlm::SimulationResult::get_action_count);

    // Batch Simulation Params
    py::class_<nlm::BatchSimulationParams>(m, "BatchSimulationParams")
        .def(py::init<>())
        .def_readwrite("num_episodes", &nlm::BatchSimulationParams::num_episodes)
        .def_readwrite("steps_per_episode", &nlm::BatchSimulationParams::steps_per_episode)
        .def_readwrite("timestep", &nlm::BatchSimulationParams::timestep)
        .def_readwrite("enable_progress", &nlm::BatchSimulationParams::enable_progress)
        .def_readwrite("progress_interval", &nlm::BatchSimulationParams::progress_interval)
        .def_readwrite("save_interval", &nlm::BatchSimulationParams::save_interval)
        .def_readwrite("checkpoint_path", &nlm::BatchSimulationParams::checkpoint_path)
        .def_readwrite("enable_early_stop", &nlm::BatchSimulationParams::enable_early_stop)
        .def_readwrite("reward_threshold", &nlm::BatchSimulationParams::reward_threshold)
        .def_readwrite("max_steps", &nlm::BatchSimulationParams::max_steps);

    // Performance Metrics
    py::class_<nlm::PerformanceMetrics>(m, "PerformanceMetrics")
        .def(py::init<>())
        .def_readwrite("cpu_time", &nlm::PerformanceMetrics::cpu_time)
        .def_readwrite("wall_time", &nlm::PerformanceMetrics::wall_time)
        .def_readwrite("memory_usage_mb", &nlm::PerformanceMetrics::memory_usage_mb)
        .def_readwrite("neurons_per_second", &nlm::PerformanceMetrics::neurons_per_second)
        .def_readwrite("synapses_per_second", &nlm::PerformanceMetrics::synapses_per_second)
        .def_readwrite("spikes_per_second", &nlm::PerformanceMetrics::spikes_per_second)
        .def_readwrite("steps_per_second", &nlm::PerformanceMetrics::steps_per_second);

    // Batch Simulation Runner
    py::class_<nlm::BatchSimulationRunner>(m, "BatchSimulationRunner")
        .def(py::init<>())
        .def("run_batch_simulation", &nlm::BatchSimulationRunner::run_batch_simulation,
             py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("params"))
        .def("run_multiple_episodes", &nlm::BatchSimulationRunner::run_multiple_episodes,
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             py::arg("num_episodes"), py::arg("steps_per_episode"))
        .def("run_experiments", &nlm::BatchSimulationRunner::run_experiments,
             py::arg("config"), py::arg("experiment_names"))
        .def("compare_configurations", &nlm::BatchSimulationRunner::compare_configurations,
             py::arg("config_list"))
        .def("save_results", &nlm::BatchSimulationRunner::save_results,
             py::arg("results"), py::arg("filepath"))
        .def("load_results", &nlm::BatchSimulationRunner::load_results,
             py::arg("filepath"));

    // Performance Analyzer
    py::class_<nlm::PerformanceAnalyzer>(m, "PerformanceAnalyzer")
        .def(py::init<>())
        .def("analyze_performance", &nlm::PerformanceAnalyzer::analyze_performance,
             py::arg("metrics"))
        .def("get_efficiency_rating", &nlm::PerformanceAnalyzer::get_efficiency_rating)
        .def("get_optimization_suggestions", &nlm::PerformanceAnalyzer::get_optimization_suggestions)
        .def("compare_performance", &nlm::PerformanceAnalyzer::compare_performance,
             py::arg("metrics_list"));

    // Result Aggregator
    py::class_<nlm::ResultAggregator>(m, "ResultAggregator")
        .def(py::init<>())
        .def("add_result", &nlm::ResultAggregator::add_result, py::arg("result"))
        .def("aggregate_results", &nlm::ResultAggregator::aggregate_results)
        .def("get_summary", &nlm::ResultAggregator::get_summary)
        .def("get_best_result", &nlm::ResultAggregator::get_best_result, py::arg("metric_func"))
        .def("get_statistics", &nlm::ResultAggregator::get_statistics);

    // Progress Tracker
    py::class_<nlm::ProgressTracker>(m, "ProgressTracker")
        .def(py::init<>())
        .def("start", &nlm::ProgressTracker::start, py::arg("total_steps"))
        .def("update", &nlm::ProgressTracker::update, py::arg("current_step"))
        .def("get_progress", &nlm::ProgressTracker::get_progress)
        .def("is_complete", &nlm::ProgressTracker::is_complete)
        .def("get_eta", &nlm::ProgressTracker::get_eta)
        .def("set_callback", &nlm::ProgressTracker::set_callback, py::arg("callback"));

    // Simulation Utilities
    py::class_<nlm::SimulationUtilities>(m, "SimulationUtilities")
        .def(py::init<>())
        .def("create_brain_from_template", &nlm::SimulationUtilities::create_brain_from_template,
             py::arg("template_name"))
        .def("validate_environment", &nlm::SimulationUtilities::validate_environment,
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("optimize_parameters", &nlm::SimulationUtilities::optimize_parameters,
             py::arg("brain"), py::arg("target_metrics"))
        .def("generate_report", &nlm::SimulationUtilities::generate_report,
             py::arg("results"), py::arg("output_file"));

    // Main factory functions
    m.def("create_default_advanced_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default advanced configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Batch simulation functions
    m.def("run_batch_simulation", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent,
                                  BatchSimulationParams params) {
        nlm::BatchSimulationRunner runner;
        return runner.run_batch_simulation(brain, world, agent, params);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("params"),
       "Run a batch simulation with advanced features");

    m.def("run_advanced_experiment", [](std::shared_ptr<Config> config,
                                      const std::vector<std::string>& experiment_names) {
        nlm::BatchSimulationRunner runner;
        return runner.run_experiments(config, experiment_names);
    }, py::arg("config"), py::arg("experiment_names"),
       "Run advanced experiments with multiple configurations");

    // Utility functions
    m.def("create_progress_tracker", []() -> std::shared_ptr<ProgressTracker> {
        return std::make_shared<ProgressTracker>();
    }, "Create a progress tracker");

    m.def("create_performance_analyzer", []() -> std::shared_ptr<PerformanceAnalyzer> {
        return std::make_shared<PerformanceAnalyzer>();
    }, "Create a performance analyzer");

    m.def("create_result_aggregator", []() -> std::shared_ptr<ResultAggregator> {
        return std::make_shared<ResultAggregator>();
    }, "Create a result aggregator");

    m.def("create_simulation_utilities", []() -> std::shared_ptr<SimulationUtilities> {
        return std::make_shared<SimulationUtilities>();
    }, "Create simulation utilities");

    // Error handling
    m.def("validate_simulation_setup", &validate_simulation_setup,
         py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Validate simulation setup and return errors if any");

    m.def("benchmark_simulation", &benchmark_simulation,
         py::arg("brain"), py::arg("world"), py::arg("agent"),
         py::arg("steps"), py::arg("repeats"),
         "Benchmark simulation performance");

    // Constants
    m.attr("SIMULATION_VERSION") = "2.0.0";
    m.attr("ADVANCED_FEATURES_AVAILABLE") = true;
    m.attr("PERFORMANCE_MONITORING_ENABLED") = true;

    // Type aliases
    m.def("create_simulation_result", []() -> nlm::SimulationResult {
        return nlm::SimulationResult();
    }, "Create an empty simulation result");

    m.def("create_performance_metrics", []() -> nlm::PerformanceMetrics {
        return nlm::PerformanceMetrics();
    }, "Create empty performance metrics");

    m.def("create_batch_simulation_params", []() -> nlm::BatchSimulationParams {
        return nlm::BatchSimulationParams();
    }, "Create default batch simulation parameters");

} // namespace nlm
