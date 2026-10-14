// SimulationContext.cpp
// Implementation of SimulationContext class

#include "SimulationContext.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

struct SimulationContext::Impl {
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    
    SimulationStep current_step_;
    Timestamp current_time_;
    bool is_initialized_;
    bool monitoring_enabled_;
    int log_level_;
    double total_simulation_time_;
    size_t total_steps_completed_;
    
    // Timing for performance measurement
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point last_step_time_;
    
    // Statistics storage
    SimulationStatistics accumulated_stats_;
    PerformanceMetrics accumulated_performance_;
    
    Impl() : current_step_(0), current_time_(0.0), 
             is_initialized_(false), monitoring_enabled_(false),
             log_level_(0), total_simulation_time_(0.0), 
             total_steps_completed_(0) {}
    
    void reset_stats() {
        accumulated_stats_ = SimulationStatistics();
        accumulated_performance_ = PerformanceMetrics();
        accumulated_stats_.step_times.clear();
        accumulated_stats_.spike_counts.clear();
        accumulated_stats_.firing_rates.clear();
        accumulated_stats_.e_i_ratios.clear();
        accumulated_stats_.active_neurons.clear();
        accumulated_stats_.neuromod_levels.clear();
        accumulated_stats_.curiosity_levels.clear();
        accumulated_stats_.novelty_levels.clear();
        accumulated_stats_.action_counts.clear();
    }
    
    void record_step(double step_time, size_t spike_count, float firing_rate, 
                    float e_i_ratio, size_t active_neurons, 
                    float neuromod_level = 0.0f, float curiosity_level = 0.0f,
                    float novelty_level = 0.0f, size_t action_count = 0) {
        accumulated_stats_.step_times.push_back(step_time);
        accumulated_stats_.spike_counts.push_back(spike_count);
        accumulated_stats_.firing_rates.push_back(firing_rate);
        accumulated_stats_.e_i_ratios.push_back(e_i_ratio);
        accumulated_stats_.active_neurons.push_back(active_neurons);
        accumulated_stats_.neuromod_levels.push_back(neuromod_level);
        accumulated_stats_.curiosity_levels.push_back(curiosity_level);
        accumulated_stats_.novelty_levels.push_back(novelty_level);
        accumulated_stats_.action_counts.push_back(action_count);
    }
    
    void update_performance() {
        if (accumulated_stats_.total_steps > 0) {
            accumulated_performance_.avg_step_time = 
                accumulated_stats_.total_time / accumulated_stats_.total_steps;
        }
        if (!accumulated_stats_.firing_rates.empty()) {
            float sum = 0.0f;
            for (float rate : accumulated_stats_.firing_rates) {
                sum += rate;
            }
            accumulated_performance_.avg_firing_rate = sum / accumulated_stats_.firing_rates.size();
        }
        
        if (!accumulated_stats_.e_i_ratios.empty()) {
            float sum = 0.0f;
            for (float ratio : accumulated_stats_.e_i_ratios) {
                sum += ratio;
            }
            accumulated_performance_.e_i_ratio = sum / accumulated_stats_.e_i_ratios.size();
        }
        
        accumulated_performance_.total_spikes = 0;
        for (size_t count : accumulated_stats_.spike_counts) {
            accumulated_performance_.total_spikes += count;
        }
        
        accumulated_performance_.steps_completed = accumulated_stats_.total_steps;
    }
};

// Constructor
SimulationContext::SimulationContext() : pImpl_(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SimulationContext created");
}

// Destructor
SimulationContext::~SimulationContext() {
    cleanup();
    NLM_LOG_INFO("SimulationContext destroyed");
}

// Factory methods
std::shared_ptr<Config> SimulationContext::create_default_config() {
    return std::make_shared<Config>();
}

std::shared_ptr<Brain> SimulationContext::create_brain(const std::shared_ptr<Config>& config) {
    return std::make_shared<Brain>(config);
}

std::shared_ptr<AgentBrain> SimulationContext::create_agent_brain(const std::shared_ptr<Brain>& brain) {
    return std::make_shared<AgentBrain>(brain);
}

std::shared_ptr<SimpleWorld> SimulationContext::create_world() {
    return std::make_shared<SimpleWorld>();
}

// Initialize simulation
bool SimulationContext::initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<SimpleWorld>& world) {
    try {
        NLM_LOG_INFO("Initializing SimulationContext...");
        
        // Validate configuration
        auto validation_result = validate_config(config);
        auto& warnings = std::get<0>(validation_result);
        auto& errors = std::get<1>(validation_result);
        
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
        
        // Create components
        config_ = config;
        world_ = world;
        
        // Create brain
        brain_ = create_brain(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Create agent brain
        agent_ = create_agent_brain(brain_);
        agent_->initialize(*world_);
        
        // Configure agent subsystems
        agent_->enableRewardModulation(true);
        agent_->enableCuriosity(true);
        agent_->enableDevelopment(true);
        agent_->enableStructuralPlasticity(true);
        
        // Initialize simulation state
        pImpl_->current_step_ = 0;
        pImpl_->current_time_ = 0.0;
        pImpl_->is_initialized_ = true;
        pImpl_->total_simulation_time_ = 0.0;
        pImpl_->total_steps_completed_ = 0;
        
        pImpl_->reset_stats();
        pImpl_->start_time_ = std::chrono::high_resolution_clock::now();
        pImpl_->last_step_time_ = pImpl_->start_time_;
        
        NLM_LOG_INFO("SimulationContext initialized successfully");
        NLM_LOG_INFO("Brain: " + std::to_string(brain_->getTotalNeuronCount()) + " neurons");
        NLM_LOG_INFO("Agent configured with reward modulation, curiosity, and development");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        return false;
    }
}

// Step simulation
bool SimulationContext::step(size_t steps, bool autoreset) {
    if (!pImpl_->is_initialized_ || !brain_ || !agent_) {
        NLM_LOG_ERROR("Simulation not initialized");
        return false;
    }
    
    try {
        auto step_start = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            pImpl_->current_step_ = pImpl_->total_steps_completed_ + step;
            pImpl_->current_time_ = pImpl_->current_step_ * 0.1; // Assuming 0.1 timestep
            
            // Update world
            world_->update(0.1);
            
            // Process sensory input
            auto percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            
            // Brain step
            brain_->step(pImpl_->current_step_, pImpl_->current_time_);
            
            // Agent motor action
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, pImpl_->current_time_);
            
            // Apply reward modulation
            auto internal_signals = percept.getInternal();
            float reward = internal_signals.empty() ? 0.0f : internal_signals[0];
            agent_->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent_->updateDevelopment(0.1);
            
            // Record step time and statistics
            auto step_end = std::chrono::high_resolution_clock::now();
            double step_time = std::chrono::duration<double>(step_end - step_start).count();
            
            pImpl_->record_step(
                step_time,
                brain_->getFiringNeuronCount(),
                brain_->getAverageFiringRate(),
                brain_->getExcitationInhibitionRatio(),
                brain_->getActiveNeuronCount(),
                agent_->getNeuromodulationLevel(),
                agent_->getCuriosityLevel(),
                agent_->getNoveltyLevel(),
                static_cast<size_t>(agent_->decodeMotorCommand()) // Action count
            );
            
            pImpl_->total_steps_completed_++;
            pImpl_->total_simulation_time_ += step_time;
            
            if (pImpl_->monitoring_enabled_) {
                log_step_info(pImpl_->total_steps_completed_);
            }
            
            step_start = std::chrono::high_resolution_clock::now();
        }
        
        // Update accumulated performance metrics
        pImpl_->update_performance();
        
        if (autoreset) {
            reset();
        }
        
        NLM_LOG_INFO("Completed " + std::to_string(steps) + " simulation steps");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during simulation: ") + e.what());
        return false;
    }
}

// Run episode
bool SimulationContext::run_episode(size_t max_steps, bool autoreset) {
    return step(max_steps, autoreset);
}

// Run agent episode with learning
bool SimulationContext::run_agent_episode(size_t max_steps, bool enable_learning, bool autoreset) {
    try {
        // Enable/disable learning subsystems
        if (agent_) {
            agent_->enableRewardModulation(enable_learning);
            agent_->enableCuriosity(enable_learning);
            agent_->enableDevelopment(enable_learning);
        }
        
        // Run episode
        bool success = step(max_steps, autoreset);
        
        // Re-enable learning for next episode if requested
        if (agent_ && enable_learning) {
            agent_->enableRewardModulation(true);
            agent_->enableCuriosity(true);
            agent_->enableDevelopment(true);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during agent episode: ") + e.what());
        return false;
    }
}

// Convenience simulation runners
SimulationResult SimulationContext::run_silent_brain(size_t steps) {
    SimulationResult result;
    result.success = false;
    
    try {
        if (!brain_) {
            result.message = "Brain not initialized";
            return result;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double total_time = std::chrono::duration<double>(end_time - start_time).count();
        
        result.success = true;
        result.message = "Silent brain simulation completed successfully";
        result.brain = brain_;
        result.metrics.initialization_time = 0.0; // Not measured
        result.metrics.total_step_time = total_time;
        result.metrics.avg_step_time = total_time / steps;
        result.metrics.total_spikes = brain_->getTotalSpikeCount();
        result.metrics.avg_firing_rate = brain_->getAverageFiringRate();
        result.metrics.e_i_ratio = brain_->getExcitationInhibitionRatio();
        result.metrics.active_neurons = brain_->getActiveNeuronCount();
        result.metrics.steps_completed = steps;
        result.metrics.success = true;
        
        NLM_LOG_INFO("Silent brain simulation completed in " + std::to_string(total_time) + "s");
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception during silent brain simulation: ") + e.what();
        NLM_LOG_ERROR(result.message);
    }
    
    return result;
}

SimulationResult SimulationContext::run_agent_episode(std::shared_ptr<SimpleWorld> world, size_t max_steps, bool enable_learning) {
    SimulationResult result;
    result.success = false;
    
    try {
        // Store existing world if any
        auto original_world = world_;
        
        // Set new world
        world_ = world;
        
        // Re-initialize agent with new world if initialized
        if (agent_ && is_initialized_) {
            agent_->reset();
            agent_->initialize(*world_);
        }
        
        // Run episode
        result.success = run_agent_episode(max_steps, enable_learning, false);
        
        if (result.success) {
            result.message = "Agent episode completed successfully";
            result.brain = brain_;
            result.agent = agent_;
            result.world = world_;
            result.metrics = pImpl_->accumulated_performance_;
            result.statistics = get_statistics();
            
            NLM_LOG_INFO("Agent episode completed successfully");
        }
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception during agent episode: ") + e.what();
        NLM_LOG_ERROR(result.message);
    }
    
    return result;
}

// Getters and statistics
SimulationStatistics SimulationContext::get_statistics() const {
    SimulationStatistics stats = pImpl_->accumulated_stats_;
    stats.total_steps = pImpl_->total_steps_completed_;
    stats.total_time = pImpl_->total_simulation_time_;
    return stats;
}

PerformanceMetrics SimulationContext::get_performance() const {
    PerformanceMetrics metrics = pImpl_->accumulated_performance_;
    metrics.total_time = pImpl_->total_simulation_time_;
    metrics.steps_completed = pImpl_->total_steps_completed_;
    return metrics;
}

std::string SimulationContext::get_status_summary() const {
    std::stringstream ss;
    ss << "=== SimulationContext Status ===\n";
    ss << "Initialized: " << (is_initialized_ ? "Yes" : "No") << "\n";
    if (brain_) {
        ss << "Brain:\n";
        ss << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
        ss << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        ss << "  Active neurons: " << brain_->getActiveNeuronCount() << "\n";
        ss << "  Firing neurons: " << brain_->getFiringNeuronCount() << "\n";
        ss << "  Total spikes: " << brain_->getTotalSpikeCount() << "\n";
        ss << "  Avg firing rate: " << brain_->getAverageFiringRate() << "\n";
        ss << "  E/I ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
        ss << "  Developmental stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << "\n";
    }
    if (agent_) {
        ss << "Agent:\n";
        ss << "  Neuromodulation level: " << agent_->getNeuromodulationLevel() << "\n";
        ss << "  Curiosity level: " << agent_->getCuriosityLevel() << "\n";
        ss << "  Novelty level: " << agent_->getNoveltyLevel() << "\n";
        ss << "  Prediction error: " << agent_->getPredictionError() << "\n";
    }
    if (world_) {
        ss << "World:\n";
        ss << "  Time: " << world_->getSimulationTime() << "\n";
        ss << "  Width: " << world_->getWidth() << ", Height: " << world_->getHeight() << "\n";
    }
    
    return ss.str();
}

// Configuration presets
std::vector<std::pair<std::string, std::shared_ptr<Config>>> SimulationContext::get_presets() {
    std::vector<std::pair<std::string, std::shared_ptr<Config>>> presets;
    
    // Minimal configuration
    auto minimal = std::make_shared<Config>();
    minimal->set("neuron_count", 100);
    minimal->set("region_count", 1);
    presets.emplace_back("minimal", minimal);
    
    // Standard configuration
    auto standard = std::make_shared<Config>();
    standard->set("neuron_count", 1000);
    standard->set("region_count", 4);
    standard->set("connection_probability", 0.1f);
    standard->set("stdp_learning_rate", 0.001f);
    presets.emplace_back("standard", standard);
    
    // Development configuration
    auto development = std::make_shared<Config>();
    development->set("neuron_count", 2000);
    development->set("region_count", 6);
    development->set("connection_probability", 0.15f);
    development->set("stdp_learning_rate", 0.002f);
    development->set("plasticity.structural.enable", true);
    development->set("neuromod.curiosity.enable", true);
    development->set("neuromod.novelty.enable", true);
    presets.emplace_back("development", development);
    
    // Research configuration
    auto research = std::make_shared<Config>();
    research->set("neuron_count", 5000);
    research->set("region_count", 8);
    research->set("connection_probability", 0.08f);
    research->set("stdp_learning_rate", 0.001f);
    research->set("plasticity.structural.enable", true);
    research->set("plasticity.hebbian.enable", true);
    research->set("neuromod.curiosity.enable", true);
    research->set("neuromod.novelty.enable", true);
    research->set("neuromod.dopamine.scale", 1.5f);
    presets.emplace_back("research", research);
    
    return presets;
}

std::tuple<std::vector<std::string>, std::vector<std::string>> SimulationContext::validate_config(const std::shared_ptr<Config>& config) {
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    // Check neuron count reasonable
    auto neuron_count_opt = config->get<size_t>("neuron_count");
    if (neuron_count_opt) {
        size_t neuron_count = *neuron_count_opt;
        if (neuron_count < 10) {
            warnings.push_back("Neuron count very low (< 10) - simulation may be too simple");
        } else if (neuron_count > 100000) {
            warnings.push_back("Neuron count very high (> 100K) - may impact performance");
        }
    }
    
    // Check region count reasonable
    auto region_count_opt = config->get<size_t>("region_count");
    if (region_count_opt) {
        size_t region_count = *region_count_opt;
        if (region_count == 0) {
            errors.push_back("Region count must be at least 1");
        }
    }
    
    // Check connection probability
    auto conn_prob_opt = config->get<float>("connection_probability");
    if (conn_prob_opt) {
        float prob = *conn_prob_opt;
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("Connection probability must be between 0 and 1");
        }
    }
    
    // Check timestep
    auto timestep_opt = config->get<double>("simulation_timestep");
    if (timestep_opt) {
        double timestep = *timestep_opt;
        if (timestep <= 0.0) {
            errors.push_back("Simulation timestep must be positive");
        } else if (timestep > 0.1) {
            warnings.push_back("Large timestep (> 0.1s) may affect simulation accuracy");
        }
    }
    
    return std::make_tuple(warnings, errors);
}

// Batch operations
std::vector<SimulationResult> SimulationContext::benchmark_configs(const std::vector<std::shared_ptr<Config>>& configs, size_t steps) {
    std::vector<SimulationResult> results;
    results.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        NLM_LOG_INFO("Benchmarking configuration " + std::to_string(i + 1) + "/" + std::to_string(configs.size()));
        
        SimulationResult result;
        result.config_id = "config_" + std::to_string(i);
        
        try {
            // Create fresh brain for each configuration
            auto brain = create_brain(configs[i]);
            brain->initialize();
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration<double>(end_time - start_time).count();
            
            result.success = true;
            result.message = "Benchmark completed successfully";
            result.brain = brain;
            result.metrics.initialization_time = 0.0;
            result.metrics.total_step_time = total_time;
            result.metrics.avg_step_time = total_time / steps;
            result.metrics.total_spikes = brain->getTotalSpikeCount();
            result.metrics.avg_firing_rate = brain->getAverageFiringRate();
            result.metrics.e_i_ratio = brain->getExcitationInhibitionRatio();
            result.metrics.active_neurons = brain->getActiveNeuronCount();
            result.metrics.steps_completed = steps;
            result.metrics.success = true;
            
        } catch (const std::exception& e) {
            result.message = std::string("Benchmark failed: ") + e.what();
            NLM_LOG_ERROR(result.message);
        }
        
        results.push_back(result);
    }
    
    return results;
}

std::vector<SimulationResult> SimulationContext::run_multiple_episodes(const std::vector<size_t>& episode_lengths, bool enable_learning) {
    std::vector<SimulationResult> results;
    results.reserve(episode_lengths.size());
    
    for (size_t i = 0; i < episode_lengths.size(); ++i) {
        NLM_LOG_INFO("Running episode " + std::to_string(i + 1) + "/" + std::to_string(episode_lengths.size()));
        
        SimulationResult result;
        result.config_id = "episode_" + std::to_string(i);
        
        try {
            bool success = run_agent_episode(episode_lengths[i], enable_learning, false);
            
            if (success) {
                result.success = true;
                result.message = "Episode completed successfully";
                result.brain = brain_;
                result.agent = agent_;
                result.world = world_;
                result.metrics = pImpl_->accumulated_performance_;
                result.statistics = get_statistics();
            } else {
                result.message = "Episode failed to complete";
            }
            
        } catch (const std::exception& e) {
            result.message = std::string("Exception during episode: ") + e.what();
            NLM_LOG_ERROR(result.message);
        }
        
        results.push_back(result);
    }
    
    return results;
}

// Resource management
void SimulationContext::reset() {
    NLM_LOG_INFO("Resetting SimulationContext...");
    
    cleanup_components();
    
    // Reset components
    if (brain_) {
        brain_->reset();
    }
    
    if (agent_) {
        agent_->reset();
        if (world_) {
            agent_->initialize(*world_);
        }
    }
    
    // Reset internal state
    config_.reset();
    brain_.reset();
    agent_.reset();
    world_.reset();
    
    pImpl_->is_initialized_ = false;
    pImpl_->current_step_ = 0;
    pImpl_->current_time_ = 0.0;
    pImpl_->total_simulation_time_ = 0.0;
    pImpl_->total_steps_completed_ = 0;
    
    pImpl_->reset_stats();
    
    NLM_LOG_INFO("SimulationContext reset complete");
}

void SimulationContext::cleanup() {
    NLM_LOG_INFO("Cleaning up SimulationContext...");
    
    cleanup_components();
    
    config_.reset();
    brain_.reset();
    agent_.reset();
    world_.reset();
    
    pImpl_->is_initialized_ = false;
    pImpl_->current_step_ = 0;
    pImpl_->current_time_ = 0.0;
    
    NLM_LOG_INFO("SimulationContext cleanup complete");
}

// Helper methods
void SimulationContext::initialize_components() {
    if (!config_ || !world_) {
        NLM_LOG_ERROR("Cannot initialize components: config or world not set");
        return;
    }
    
    try {
        // Create brain
        brain_ = create_brain(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return;
        }
        
        // Create agent brain
        agent_ = create_agent_brain(brain_);
        agent_->initialize(*world_);
        
        // Configure agent subsystems
        agent_->enableRewardModulation(true);
        agent_->enableCuriosity(true);
        agent_->enableDevelopment(true);
        agent_->enableStructuralPlasticity(true);
        
        pImpl_->is_initialized_ = true;
        
        NLM_LOG_INFO("Components initialized successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during component initialization: ") + e.what());
        reset();
    }
}

void SimulationContext::cleanup_components() {
    if (agent_) {
        agent_->reset();
        agent_.reset();
    }
    
    if (brain_) {
        brain_.reset();
    }
}

void SimulationContext::log_step_info(size_t step) const {
    if (!brain_ || !agent_) return;
    
    NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                 "Firing: " + std::to_string(brain_->getFiringNeuronCount()) + "/" +
                 std::to_string(brain_->getTotalNeuronCount()) + ", " +
                 "Rate: " + std::to_string(brain_->getAverageFiringRate()) + ", " +
                 "Curiosity: " + std::to_string(agent_->getCuriosityLevel()) + ", " +
                 "Neuromod: " + std::to_string(agent_->getNeuromodulationLevel()));
}

double SimulationContext::get_current_time() const {
    return pImpl_->current_time_;
}

// Template functions for statistics collection
// (Implementation would be in the header file for template functions)

    } // namespace nlm

// Template function implementations for SimulationStatistics
namespace nlm {
// SimulationContext.cpp
// Implementation of SimulationContext class

#include "SimulationContext.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

// Constructor
SimulationContext::SimulationContext() : pImpl_(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SimulationContext created");
}

// Destructor
SimulationContext::~SimulationContext() {
    cleanup();
    NLM_LOG_INFO("SimulationContext destroyed");
}

// Factory methods
std::shared_ptr<Config> SimulationContext::create_default_config() {
    return std::make_shared<Config>();
}

std::shared_ptr<Brain> SimulationContext::create_brain(const std::shared_ptr<Config>& config) {
    return std::make_shared<Brain>(config);
}

std::shared_ptr<AgentBrain> SimulationContext::create_agent_brain(const std::shared_ptr<Brain>& brain) {
    return std::make_shared<AgentBrain>(brain);
}

std::shared_ptr<SimpleWorld> SimulationContext::create_world() {
    return std::make_shared<SimpleWorld>();
}

// Initialize simulation
bool SimulationContext::initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<SimpleWorld>& world) {
    try {
        NLM_LOG_INFO("Initializing SimulationContext...");
        
        // Validate configuration
        auto validation_result = validate_config(config);
        auto& warnings = std::get<0>(validation_result);
        auto& errors = std::get<1>(validation_result);
        
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
        
        // Create components
        config_ = config;
        world_ = world;
        
        // Create brain
        brain_ = create_brain(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Create agent brain
        agent_ = create_agent_brain(brain_);
        agent_->initialize(*world_);
        
        // Configure agent subsystems
        agent_->enableRewardModulation(true);
        agent_->enableCuriosity(true);
        agent_->enableDevelopment(true);
        agent_->enableStructuralPlasticity(true);
        
        // Initialize simulation state
        pImpl_->current_step_ = 0;
        pImpl_->current_time_ = 0.0;
        pImpl_->is_initialized_ = true;
        pImpl_->total_simulation_time_ = 0.0;
        pImpl_->total_steps_completed_ = 0;
        
        pImpl_->reset_stats();
        pImpl_->start_time_ = std::chrono::high_resolution_clock::now();
        pImpl_->last_step_time_ = pImpl_->start_time_;
        
        NLM_LOG_INFO("SimulationContext initialized successfully");
        NLM_LOG_INFO("Brain: " + std::to_string(brain_->getTotalNeuronCount()) + " neurons");
        NLM_LOG_INFO("Agent configured with reward modulation, curiosity, and development");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        return false;
    }
}

// Step simulation
bool SimulationContext::step(size_t steps, bool autoreset) {
    if (!pImpl_->is_initialized_ || !brain_ || !agent_) {
        NLM_LOG_ERROR("Simulation not initialized");
        return false;
    }
    
    try {
        auto step_start = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            pImpl_->current_step_ = pImpl_->total_steps_completed_ + step;
            pImpl_->current_time_ = pImpl_->current_step_ * 0.1; // Assuming 0.1 timestep
            
            // Update world
            world_->update(0.1);
            
            // Process sensory input
            auto percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            
            // Brain step
            brain_->step(pImpl_->current_step_, pImpl_->current_time_);
            
            // Agent motor action
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, pImpl_->current_time_);
            
            // Apply reward modulation
            auto internal_signals = percept.getInternal();
            float reward = internal_signals.empty() ? 0.0f : internal_signals[0];
            agent_->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent_->updateDevelopment(0.1);
            
            // Record step time and statistics
            auto step_end = std::chrono::high_resolution_clock::now();
            double step_time = std::chrono::duration<double>(step_end - step_start).count();
            
            pImpl_->record_step(
                step_time,
                brain_->getFiringNeuronCount(),
                brain_->getAverageFiringRate(),
                brain_->getExcitationInhibitionRatio(),
                brain_->getActiveNeuronCount(),
                agent_->getNeuromodulationLevel(),
                agent_->getCuriosityLevel(),
                agent_->getNoveltyLevel(),
                static_cast<size_t>(agent_->decodeMotorCommand()) // Action count
            );
            
            pImpl_->total_steps_completed_++;
            pImpl_->total_simulation_time_ += step_time;
            
            if (pImpl_->monitoring_enabled_) {
                log_step_info(pImpl_->total_steps_completed_);
            }
            
            step_start = std::chrono::high_resolution_clock::now();
        }
        
        // Update accumulated performance metrics
        pImpl_->update_performance();
        
        if (autoreset) {
            reset();
        }
        
        NLM_LOG_INFO("Completed " + std::to_string(steps) + " simulation steps");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during simulation: ") + e.what());
        return false;
    }
}

// Run episode
bool SimulationContext::run_episode(size_t max_steps, bool autoreset) {
    return step(max_steps, autoreset);
}

// Run agent episode with learning
bool SimulationContext::run_agent_episode(size_t max_steps, bool enable_learning, bool autoreset) {
    try {
        // Enable/disable learning subsystems
        if (agent_) {
            agent_->enableRewardModulation(enable_learning);
            agent_->enableCuriosity(enable_learning);
            agent_->enableDevelopment(enable_learning);
        }
        
        // Run episode
        bool success = step(max_steps, autoreset);
        
        // Re-enable learning for next episode if requested
        if (agent_ && enable_learning) {
            agent_->enableRewardModulation(true);
            agent_->enableCuriosity(true);
            agent_->enableDevelopment(true);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during agent episode: ") + e.what());
        return false;
    }
}

// Convenience simulation runners
SimulationResult SimulationContext::run_silent_brain(size_t steps) {
    SimulationResult result;
    result.success = false;
    
    try {
        if (!brain_) {
            result.message = "Brain not initialized";
            return result;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double total_time = std::chrono::duration<double>(end_time - start_time).count();
        
        result.success = true;
        result.message = "Silent brain simulation completed successfully";
        result.brain = brain_;
        result.metrics.initialization_time = 0.0; // Not measured
        result.metrics.total_step_time = total_time;
        result.metrics.avg_step_time = total_time / steps;
        result.metrics.total_spikes = brain_->getTotalSpikeCount();
        result.metrics.avg_firing_rate = brain_->getAverageFiringRate();
        result.metrics.e_i_ratio = brain_->getExcitationInhibitionRatio();
        result.metrics.active_neurons = brain_->getActiveNeuronCount();
        result.metrics.steps_completed = steps;
        result.metrics.success = true;
        
        NLM_LOG_INFO("Silent brain simulation completed in " + std::to_string(total_time) + "s");
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception during silent brain simulation: ") + e.what();
        NLM_LOG_ERROR(result.message);
    }
    
    return result;
}

SimulationResult SimulationContext::run_agent_episode(std::shared_ptr<SimpleWorld> world, size_t max_steps, bool enable_learning) {
    SimulationResult result;
    result.success = false;
    
    try {
        // Store existing world if any
        auto original_world = world_;
        
        // Set new world
        world_ = world;
        
        // Re-initialize agent with new world if initialized
        if (agent_ && is_initialized_) {
            agent_->reset();
            agent_->initialize(*world_);
        }
        
        // Run episode
        result.success = run_agent_episode(max_steps, enable_learning, false);
        
        if (result.success) {
            result.message = "Agent episode completed successfully";
            result.brain = brain_;
            result.agent = agent_;
            result.world = world_;
            result.metrics = pImpl_->accumulated_performance_;
            result.statistics = get_statistics();
            
            NLM_LOG_INFO("Agent episode completed successfully");
        }
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception during agent episode: ") + e.what();
        NLM_LOG_ERROR(result.message);
    }
    
    return result;
}

// Getters and statistics
SimulationStatistics SimulationContext::get_statistics() const {
    SimulationStatistics stats = pImpl_->accumulated_stats_;
    stats.total_steps = pImpl_->total_steps_completed_;
    stats.total_time = pImpl_->total_simulation_time_;
    return stats;
}

PerformanceMetrics SimulationContext::get_performance() const {
    PerformanceMetrics metrics = pImpl_->accumulated_performance_;
    metrics.total_time = pImpl_->total_simulation_time_;
    metrics.steps_completed = pImpl_->total_steps_completed_;
    return metrics;
}

std::string SimulationContext::get_status_summary() const {
    std::stringstream ss;
    ss << "=== SimulationContext Status ===\n";
    ss << "Initialized: " << (is_initialized_ ? "Yes" : "No") << "\n";
    if (brain_) {
        ss << "Brain:\n";
        ss << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
        ss << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        ss << "  Active neurons: " << brain_->getActiveNeuronCount() << "\n";
        ss << "  Firing neurons: " << brain_->getFiringNeuronCount() << "\n";
        ss << "  Total spikes: " << brain_->getTotalSpikeCount() << "\n";
        ss << "  Avg firing rate: " << brain_->getAverageFiringRate() << "\n";
        ss << "  E/I ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
        ss << "  Developmental stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << "\n";
    }
    if (agent_) {
        ss << "Agent:\n";
        ss << "  Neuromodulation level: " << agent_->getNeuromodulationLevel() << "\n";
        ss << "  Curiosity level: " << agent_->getCuriosityLevel() << "\n";
        ss << "  Novelty level: " << agent_->getNoveltyLevel() << "\n";
        ss << "  Prediction error: " << agent_->getPredictionError() << "\n";
    }
    if (world_) {
        ss << "World:\n";
        ss << "  Time: " << world_->getSimulationTime() << "\n";
        ss << "  Width: " << world_->getWidth() << ", Height: " << world_->getHeight() << "\n";
    }
    
    return ss.str();
}

// Configuration presets
std::vector<std::pair<std::string, std::shared_ptr<Config>>> SimulationContext::get_presets() {
    std::vector<std::pair<std::string, std::shared_ptr<Config>>> presets;
    
    // Minimal configuration
    auto minimal = std::make_shared<Config>();
    minimal->set("neuron_count", 100);
    minimal->set("region_count", 1);
    presets.emplace_back("minimal", minimal);
    
    // Standard configuration
    auto standard = std::make_shared<Config>();
    standard->set("neuron_count", 1000);
    standard->set("region_count", 4);
    standard->set("connection_probability", 0.1f);
    standard->set("stdp_learning_rate", 0.001f);
    presets.emplace_back("standard", standard);
    
    // Development configuration
    auto development = std::make_shared<Config>();
    development->set("neuron_count", 2000);
    development->set("region_count", 6);
    development->set("connection_probability", 0.15f);
    development->set("stdp_learning_rate", 0.002f);
    development->set("plasticity.structural.enable", true);
    development->set("neuromod.curiosity.enable", true);
    development->set("neuromod.novelty.enable", true);
    presets.emplace_back("development", development);
    
    // Research configuration
    auto research = std::make_shared<Config>();
    research->set("neuron_count", 5000);
    research->set("region_count", 8);
    research->set("connection_probability", 0.08f);
    research->set("stdp_learning_rate", 0.001f);
    research->set("plasticity.structural.enable", true);
    research->set("plasticity.hebbian.enable", true);
    research->set("neuromod.curiosity.enable", true);
    research->set("neuromod.novelty.enable", true);
    research->set("neuromod.dopamine.scale", 1.5f);
    presets.emplace_back("research", research);
    
    return presets;
}

std::tuple<std::vector<std::string>, std::vector<std::string>> SimulationContext::validate_config(const std::shared_ptr<Config>& config) {
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    // Check neuron count reasonable
    auto neuron_count_opt = config->get<size_t>("neuron_count");
    if (neuron_count_opt) {
        size_t neuron_count = *neuron_count_opt;
        if (neuron_count < 10) {
            warnings.push_back("Neuron count very low (< 10) - simulation may be too simple");
        } else if (neuron_count > 100000) {
            warnings.push_back("Neuron count very high (> 100K) - may impact performance");
        }
    }
    
    // Check region count reasonable
    auto region_count_opt = config->get<size_t>("region_count");
    if (region_count_opt) {
        size_t region_count = *region_count_opt;
        if (region_count == 0) {
            errors.push_back("Region count must be at least 1");
        }
    }
    
    // Check connection probability
    auto conn_prob_opt = config->get<float>("connection_probability");
    if (conn_prob_opt) {
        float prob = *conn_prob_opt;
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("Connection probability must be between 0 and 1");
        }
    }
    
    // Check timestep
    auto timestep_opt = config->get<double>("simulation_timestep");
    if (timestep_opt) {
        double timestep = *timestep_opt;
        if (timestep <= 0.0) {
            errors.push_back("Simulation timestep must be positive");
        } else if (timestep > 0.1) {
            warnings.push_back("Large timestep (> 0.1s) may affect simulation accuracy");
        }
    }
    
    return std::make_tuple(warnings, errors);
}

// Batch operations
std::vector<SimulationResult> SimulationContext::benchmark_configs(const std::vector<std::shared_ptr<Config>>& configs, size_t steps) {
    std::vector<SimulationResult> results;
    results.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        NLM_LOG_INFO("Benchmarking configuration " + std::to_string(i + 1) + "/" + std::to_string(configs.size()));
        
        SimulationResult result;
        result.config_id = "config_" + std::to_string(i);
        
        try {
            // Create fresh brain for each configuration
            auto brain = create_brain(configs[i]);
            brain->initialize();
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration<double>(end_time - start_time).count();
            
            result.success = true;
            result.message = "Benchmark completed successfully";
            result.brain = brain;
            result.metrics.initialization_time = 0.0;
            result.metrics.total_step_time = total_time;
            result.metrics.avg_step_time = total_time / steps;
            result.metrics.total_spikes = brain->getTotalSpikeCount();
            result.metrics.avg_firing_rate = brain->getAverageFiringRate();
            result.metrics.e_i_ratio = brain->getExcitationInhibitionRatio();
            result.metrics.active_neurons = brain->getActiveNeuronCount();
            result.metrics.steps_completed = steps;
            result.metrics.success = true;
            
        } catch (const std::exception& e) {
            result.message = std::string("Benchmark failed: ") + e.what();
            NLM_LOG_ERROR(result.message);
        }
        
        results.push_back(result);
    }
    
    return results;
}

std::vector<SimulationResult> SimulationContext::run_multiple_episodes(const std::vector<size_t>& episode_lengths, bool enable_learning) {
    std::vector<SimulationResult> results;
    results.reserve(episode_lengths.size());
    
    for (size_t i = 0; i < episode_lengths.size(); ++i) {
        NLM_LOG_INFO("Running episode " + std::to_string(i + 1) + "/" + std::to_string(episode_lengths.size()));
        
        SimulationResult result;
        result.config_id = "episode_" + std::to_string(i);
        
        try {
            bool success = run_agent_episode(episode_lengths[i], enable_learning, false);
            
            if (success) {
                result.success = true;
                result.message = "Episode completed successfully";
                result.brain = brain_;
                result.agent = agent_;
                result.world = world_;
                result.metrics = pImpl_->accumulated_performance_;
                result.statistics = get_statistics();
            } else {
                result.message = "Episode failed to complete";
            }
            
        } catch (const std::exception& e) {
            result.message = std::string("Exception during episode: ") + e.what();
            NLM_LOG_ERROR(result.message);
        }
        
        results.push_back(result);
    }
    
    return results;
}

// Resource management
void SimulationContext::reset() {
    NLM_LOG_INFO("Resetting SimulationContext...");
    
    cleanup_components();
    
    // Reset components
    if (brain_) {
        brain_->reset();
    }
    
    if (agent_) {
        agent_->reset();
        if (world_) {
            agent_->initialize(*world_);
        }
    }
    
    // Reset internal state
    config_.reset();
    brain_.reset();
    agent_.reset();
    world_.reset();
    
    pImpl_->is_initialized_ = false;
    pImpl_->current_step_ = 0;
    pImpl_->current_time_ = 0.0;
    pImpl_->total_simulation_time_ = 0.0;
    pImpl_->total_steps_completed_ = 0;
    
    pImpl_->reset_stats();
    
    NLM_LOG_INFO("SimulationContext reset complete");
}

void SimulationContext::cleanup() {
    NLM_LOG_INFO("Cleaning up SimulationContext...");
    
    cleanup_components();
    
    config_.reset();
    brain_.reset();
    agent_.reset();
    world_.reset();
    
    pImpl_->is_initialized_ = false;
    pImpl_->current_step_ = 0;
    pImpl_->current_time_ = 0.0;
    
    NLM_LOG_INFO("SimulationContext cleanup complete");
}

// Helper methods
void SimulationContext::initialize_components() {
    if (!config_ || !world_) {
        NLM_LOG_ERROR("Cannot initialize components: config or world not set");
        return;
    }
    
    try {
        // Create brain
        brain_ = create_brain(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return;
        }
        
        // Create agent brain
        agent_ = create_agent_brain(brain_);
        agent_->initialize(*world_);
        
        // Configure agent subsystems
        agent_->enableRewardModulation(true);
        agent_->enableCuriosity(true);
        agent_->enableDevelopment(true);
        agent_->enableStructuralPlasticity(true);
        
        pImpl_->is_initialized_ = true;
        
        NLM_LOG_INFO("Components initialized successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during component initialization: ") + e.what());
        reset();
    }
}

void SimulationContext::cleanup_components() {
    if (agent_) {
        agent_->reset();
        agent_.reset();
    }
    
    if (brain_) {
        brain_.reset();
    }
}

void SimulationContext::log_step_info(size_t step) const {
    if (!brain_ || !agent_) return;
    
    NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                 "Firing: " + std::to_string(brain_->getFiringNeuronCount()) + "/" +
                 std::to_string(brain_->getTotalNeuronCount()) + ", " +
                 "Rate: " + std::to_string(brain_->getAverageFiringRate()) + ", " +
                 "Curiosity: " + std::to_string(agent_->getCuriosityLevel()) + ", " +
                 "Neuromod: " + std::to_string(agent_->getNeuromodulationLevel()));
}

double SimulationContext::get_current_time() const {
    return pImpl_->current_time_;
}

// Additional helper implementations
void SimulationContext::generate_html_dashboard(const std::string& output_path) const {
    // Implementation for generating HTML dashboard
    NLM_LOG_INFO("HTML dashboard generation not yet implemented: " + output_path);
}

void SimulationContext::generate_performance_report(const std::string& output_path) const {
    // Implementation for generating performance report
    NLM_LOG_INFO("Performance report generation not yet implemented: " + output_path);
}

void SimulationContext::create_monitoring_script(const std::string& output_path) const {
    // Implementation for creating monitoring script
    NLM_LOG_INFO("Monitoring script creation not yet implemented: " + output_path);
}

} // namespace nlm