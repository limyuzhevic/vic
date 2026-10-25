// ============================================================================
// ENHANCED BRAIN IMPLEMENTATION
// ============================================================================

#include "EnhancedBrain.hpp"
#include "core/Config/Config.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

// ============================================================================
// ENHANCED BRAIN IMPLEMENTATION
// ============================================================================

EnhancedBrain::EnhancedBrain(std::shared_ptr<Brain> brain) 
    : brain_(brain), initialized_(false), enabled_(true) {
    creationTime_ = std::chrono::steady_clock::now();
}

bool EnhancedBrain::initialize() {
    if (!brain_ || initialized_) return initialized_;
    initialized_ = brain_->initialize();
    return initialized_;
}

size_t EnhancedBrain::run_steps(size_t num_steps, double timestep) {
    if (!brain_ || !enabled_) return 0;
    size_t initial_spikes = brain_->getTotalSpikeCount();
    
    for (size_t i = 0; i < num_steps; ++i) {
        brain_->step(i, i * timestep);
    }
    
    return brain_->getTotalSpikeCount() - initial_spikes;
}

std::map<std::string, double> EnhancedBrain::get_stats() const {
    std::map<std::string, double> stats;
    if (!brain_) return stats;

    stats["total_neurons"] = brain_->getTotalNeuronCount();
    stats["total_synapses"] = brain_->getTotalSynapseCount();
    stats["active_neurons"] = brain_->getActiveNeuronCount();
    stats["firing_neurons"] = brain_->getFiringNeuronCount();
    stats["total_spikes"] = brain_->getTotalSpikeCount();
    stats["avg_firing_rate"] = brain_->getAverageFiringRate();
    stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
    
    // Calculate derived metrics
    double avgConnectionsPerNeuron = (brain_->getTotalNeuronCount() > 0) 
        ? (brain_->getTotalSynapseCount() / 2.0) / brain_->getTotalNeuronCount() 
        : 0.0;
    stats["avg_connections_per_neuron"] = avgConnectionsPerNeuron;
    
    stats["memory_usage_mb"] = brain_->getTotalNeuronCount() * 0.001; // Rough estimate
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - creationTime_);
    stats["brain_age_ms"] = duration.count();

    return stats;
}

EnhancedBrain EnhancedBrain::create_default_brain(int neuron_count) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuron_count));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(config);
    EnhancedBrain helper(brain);
    helper.initialize();
    return helper;
}

EnhancedBrain EnhancedBrain::create_with_config(const std::string& config_json) {
    auto config = std::make_shared<Config>();
    config->loadFromJSON(config_json);
    auto brain = std::make_shared<Brain>(config);
    EnhancedBrain helper(brain);
    helper.initialize();
    return helper;
}

std::string EnhancedBrain::create_snapshot() const {
    std::ostringstream oss;
    oss << "NLM_ENHANCED_BRAIN_SNAPSHOT\n";
    oss << "Version: 1.0\n";
    oss << "Initialized: " << (initialized_ ? "true" : "false") << "\n";
    oss << "Enabled: " << (enabled_ ? "true" : "false") << "\n";
    oss << "Neurons: " << brain_->getTotalNeuronCount() << "\n";
    oss << "Synapses: " << brain_->getTotalSynapseCount() << "\n";
    oss << "Development Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << "\n";
    
    time_t now = std::time(nullptr);
    oss << "Created: " << std::ctime(&now);
    
    return oss.str();
}

bool EnhancedBrain::restore_from_snapshot(const std::string& snapshot) {
    std::istringstream iss(snapshot);
    std::string line;
    
    if (!std::getline(iss, line) || line != "NLM_ENHANCED_BRAIN_SNAPSHOT") {
        return false;
    }
    
    // Simple restoration - in real implementation would need proper parsing
    // For now, just reset and reinitialize
    reset();
    return initialize();
}

// ============================================================================
// ENHANCED WORLD IMPLEMENTATION
// ============================================================================

void EnhancedWorld::configure(float width, float height, size_t vision_width, size_t vision_height) {
    world_->configure(width, height, vision_width, vision_height);
}

EnhancedWorld EnhancedWorld::create_standard_world(float width, float height) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, 16, 16);
    return EnhancedWorld(world);
}

EnhancedWorld EnhancedWorld::create_large_world(float width, float height) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, 32, 32);
    return EnhancedWorld(world, 0.05);
}

EnhancedWorld EnhancedWorld::create_small_world(float width, float height) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, 8, 8);
    return EnhancedWorld(world, 0.05);
}

// ============================================================================
// ENHANCED AGENT BRAIN IMPLEMENTATION
// ============================================================================

std::map<std::string, double> EnhancedAgentBrain::get_stats() const {
    std::map<std::string, double> stats;
    if (!agent_) return stats;

    stats["curiosity_level"] = agent_->getCuriosityLevel();
    stats["novelty_level"] = agent_->getNoveltyLevel();
    stats["prediction_error"] = agent_->getPredictionError();
    stats["neuromodulation_level"] = agent_->getNeuromodulationLevel();
    stats["development_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
    
    // Calculate cognitive load
    double cognitive_load = (agent_->getCuriosityLevel() + agent_->getNoveltyLevel() + 
                            std::abs(agent_->getPredictionError())) / 3.0;
    stats["cognitive_load"] = cognitive_load;

    return stats;
}

EnhancedAgentBrain EnhancedAgentBrain::create_for_brain(std::shared_ptr<Brain> brain) {
    auto agent = std::make_shared<AgentBrain>(brain);
    return EnhancedAgentBrain(agent, brain);
}

// ============================================================================
// SIMULATION CONTROLLER IMPLEMENTATION
// ============================================================================

std::map<std::string, double> SimulationController::run_learning_experiment(
    EnhancedBrain& brain, EnhancedAgentBrain& agent, EnhancedWorld& world,
    size_t exploration_steps, size_t exploitation_steps, double timestep) {
    
    std::map<std::string, double> results;
    
    // Phase 1: Exploration
    brain.reset();
    agent.reset();
    world.reset();
    
    agent.configure_learning(true, true, false, false); // Enable curiosity, disable others
    
    for (size_t step = 0; step < exploration_steps; ++step) {
        world.update(timestep);
        auto percept = world.get_percept();
        agent.process_percept(percept);
        agent.get_raw_brain()->step(step, world.get_simulation_time());
        auto action = agent.decode_action();
        world.apply_action(action, world.get_simulation_time());
        
        double reward = (percept.getInternal().size() > 0) ? percept.getInternal()[0] : 0.0;
        agent.apply_reward(reward, 0.0f);
    }
    
    auto exploration_stats = brain.get_stats();
    results["exploration_spikes"] = exploration_stats["total_spikes"];
    results["exploration_firing_rate"] = exploration_stats["avg_firing_rate"];
    
    // Phase 2: Exploitation
    agent.configure_learning(false, false, true, true); // Enable development and structural plasticity
    
    for (size_t step = exploration_steps; step < exploration_steps + exploitation_steps; ++step) {
        world.update(timestep);
        auto percept = world.get_percept();
        agent.process_percept(percept);
        agent.get_raw_brain()->step(step, world.get_simulation_time());
        auto action = agent.decode_action();
        world.apply_action(action, world.get_simulation_time());
        
        double reward = (percept.getInternal().size() > 0) ? percept.getInternal()[0] : 0.0;
        agent.apply_reward(reward, 0.0f);
    }
    
    auto exploitation_stats = brain.get_stats();
    results["exploitation_spikes"] = exploitation_stats["total_spikes"] - exploration_stats["total_spikes"];
    results["exploitation_firing_rate"] = exploitation_stats["avg_firing_rate"];
    
    // Calculate improvement metrics
    double spike_improvement = (exploration_stats["total_spikes"] > 0) 
        ? ((exploitation_stats["total_spikes"] - exploration_stats["total_spikes"]) / 
           exploration_stats["total_spikes"]) * 100.0
        : 0.0;
    
    double learning_efficiency = spike_improvement / exploration_steps;
    
    results["spike_improvement_percent"] = spike_improvement;
    results["learning_efficiency"] = learning_efficiency;
    
    return results;
}

std::vector<std::map<std::string, double>> SimulationController::run_batch_simulation(
    const std::vector<std::shared_ptr<Config>>& configs,
    size_t steps_per_sim, double timestep) {
    
    std::vector<std::map<std::string, double>> results;
    
    for (const auto& config : configs) {
        auto brain = std::make_shared<Brain>(config);
        EnhancedBrain enhanced_brain(brain);
        
        if (enhanced_brain.initialize()) {
            auto stats = enhanced_brain.get_stats();
            stats["simulation_id"] = configs.size();
            results.push_back(stats);
            
            enhanced_brain.run_steps(steps_per_sim, timestep);
        }
    }
    
    return results;
}

std::vector<std::map<std::string, double>> SimulationController::run_parallel_simulation(
    const std::vector<std::shared_ptr<Config>>& configs,
    size_t steps_per_sim, double timestep,
    size_t num_threads) {
    
    std::vector<std::map<std::string, double>> results;
    std::mutex results_mutex;
    std::vector<std::thread> threads;
    
    size_t configs_per_thread = configs.size() / num_threads;
    
    auto worker = [&](size_t start_idx, size_t end_idx) {
        std::vector<std::map<std::string, double>> local_results;
        
        for (size_t i = start_idx; i < end_idx; ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            EnhancedBrain enhanced_brain(brain);
            
            if (enhanced_brain.initialize()) {
                auto stats = enhanced_brain.get_stats();
                stats["simulation_id"] = i;
                local_results.push_back(stats);
                
                enhanced_brain.run_steps(steps_per_sim, timestep);
            }
        }
        
        std::lock_guard<std::mutex> lock(results_mutex);
        results.insert(results.end(), local_results.begin(), local_results.end());
    };
    
    size_t start_idx = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t end_idx = (i == num_threads - 1) ? configs.size() : start_idx + configs_per_thread;
        threads.emplace_back(worker, start_idx, end_idx);
        start_idx = end_idx;
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return results;
}

std::map<std::string, double> SimulationController::analyze_simulation_performance(
    const std::vector<std::map<std::string, double>>& results) {
    
    std::map<std::string, double> analysis;
    
    if (results.empty()) {
        return analysis;
    }
    
    double total_spikes = 0.0, total_neurons = 0.0;
    double min_firing = std::numeric_limits<double>::max();
    double max_firing = 0.0;
    
    for (const auto& result : results) {
        total_spikes += result.at("total_spikes");
        total_neurons += result.at("total_neurons");
        
        double firing_rate = result.at("avg_firing_rate");
        min_firing = std::min(min_firing, firing_rate);
        max_firing = std::max(max_firing, firing_rate);
    }
    
    size_t count = results.size();
    
    analysis["avg_total_spikes"] = total_spikes / count;
    analysis["avg_total_neurons"] = total_neurons / count;
    analysis["avg_firing_rate"] = (total_spikes / total_neurons) * 1000.0; // Convert to Hz
    analysis["firing_rate_std"] = (max_firing - min_firing) / 2.0;
    analysis["performance_consistency"] = 1.0 - (analysis["firing_rate_std"] / 
                                               std::max(analysis["avg_firing_rate"], 1.0));
    
    return analysis;
}

std::string SimulationController::generate_visualization_data(
    const std::map<std::string, double>& brain_stats,
    const std::map<std::string, double>& agent_stats,
    const std::map<std::string, double>& world_stats) {
    
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"brain_stats\": {\n";
    for (const auto& pair : brain_stats) {
        oss << "    \"" << pair.first << "\": " << pair.second << ",\n";
    }
    oss << "  },\n";
    
    oss << "  \"agent_stats\": {\n";
    for (const auto& pair : agent_stats) {
        oss << "    \"" << pair.first << "\": " << pair.second << ",\n";
    }
    oss << "  },\n";
    
    oss << "  \"world_stats\": {\n";
    for (const auto& pair : world_stats) {
        oss << "    \"" << pair.first << "\": " << pair.second << ",\n";
    }
    oss << "  }\n";
    oss << "}\n";
    
    return oss.str();
}

// ============================================================================
// SIMULATION MONITOR IMPLEMENTATION
// ============================================================================

double SimulationMonitor::calculate_std_dev(const std::vector<double>& values, double mean) {
    double sum_sq = 0.0;
    for (double v : values) {
        sum_sq += (v - mean) * (v - mean);
    }
    return std::sqrt(sum_sq / values.size());
}

std::map<std::string, double> SimulationMonitor::benchmark_simulation(
    std::function<void()> simulation_func,
    size_t num_runs,
    const std::string& benchmark_name) {
    
    std::vector<double> times;
    
    for (size_t i = 0; i < num_runs; ++i) {
        auto start = std::chrono::steady_clock::now();
        simulation_func();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        times.push_back(duration.count() / 1000.0); // Convert to seconds
    }
    
    std::map<std::string, double> results;
    double sum_time = std::accumulate(times.begin(), times.end(), 0.0);
    
    results["benchmark_name"] = benchmark_name;
    results["runs"] = num_runs;
    results["avg_time_seconds"] = sum_time / num_runs;
    results["min_time_seconds"] = *std::min_element(times.begin(), times.end());
    results["max_time_seconds"] = *std::max_element(times.begin(), times.end());
    results["std_time_seconds"] = calculate_std_dev(times, sum_time / num_runs);
    results["performance_score"] = 1.0 / (results["avg_time_seconds"] + 0.001); // Higher is better
    
    return results;
}

// ============================================================================
// UTILITY FUNCTIONS IMPLEMENTATION
// ============================================================================

namespace Utils {

    std::vector<std::string> ProgressTracker::find_files_with_extension(const std::string& directory, const std::string& extension) {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                files.push_back(entry.path().string());
            }
        }
        return files;
    }

} // namespace Utils

} // namespace nlm
