// Helper functions and utilities for common patterns
namespace helper_utils {

// Simulation helpers
struct SimulationOptions {
    int num_steps;
    float timestep;
    bool log_stats;
    bool enable_learning;
    bool enable_development;
    bool enable_curiosity;
    
    SimulationOptions() : num_steps(1000), timestep(0.1f), log_stats(false),
                         enable_learning(true), enable_development(true), enable_curiosity(true) {}
};

// Common brain configurations
struct BrainConfigPresets {
    static std::shared_ptr<Config> default_brain() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.connection_probability", 0.1);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("brain.tau_ref", 2.0f);
        return config;
    }
    
    static std::shared_ptr<Config> compact_brain() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.v_thresh", -45.0f);
        config->set("brain.v_rest", -60.0f);
        config->set("brain.v_reset", -65.0f);
        config->set("brain.tau_mem", 15.0f);
        config->set("brain.tau_ref", 1.0f);
        return config;
    }
    
    static std::shared_ptr<Config> large_brain() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("brain.connection_probability", 0.15);
        config->set("brain.v_thresh", -55.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        config->set("brain.tau_mem", 25.0f);
        config->set("brain.tau_ref", 3.0f);
        return config;
    }
};

// Statistical analysis helpers
struct SimulationStats {
    std::vector<float> firing_rates;
    std::vector<float> spike_counts;
    std::vector<float> e_i_ratios;
    std::vector<float> curiosity_levels;
    std::vector<float> novelty_levels;
    std::vector<float> prediction_errors;
    int total_steps;
    float mean_firing_rate;
    float std_firing_rate;
    float max_firing_rate;
    float min_firing_rate;
    float mean_curiosity;
    float mean_novelty;
    float mean_prediction_error;
    
    void compute_statistics() {
        if (firing_rates.empty()) return;
        
        mean_firing_rate = 0.0f;
        for (float rate : firing_rates) mean_firing_rate += rate;
        mean_firing_rate /= firing_rates.size();
        
        float sum_sq = 0.0f;
        max_firing_rate = firing_rates[0];
        min_firing_rate = firing_rates[0];
        
        for (float rate : firing_rates) {
            sum_sq += (rate - mean_firing_rate) * (rate - mean_firing_rate);
            if (rate > max_firing_rate) max_firing_rate = rate;
            if (rate < min_firing_rate) min_firing_rate = rate;
        }
        
        std_firing_rate = std::sqrt(sum_sq / firing_rates.size());
        
        mean_curiosity = 0.0f;
        for (float curiosity : curiosity_levels) mean_curiosity += curiosity;
        if (!curiosity_levels.empty()) mean_curiosity /= curiosity_levels.size();
        
        mean_novelty = 0.0f;
        for (float novelty : novelty_levels) mean_novelty += novelty;
        if (!novelty_levels.empty()) mean_novelty /= novelty_levels.size();
        
        mean_prediction_error = 0.0f;
        for (float error : prediction_errors) mean_prediction_error += error;
        if (!prediction_errors.empty()) mean_prediction_error /= prediction_errors.size();
    }
    
    std::string report() const {
        std::stringstream ss;
        ss << "=== Simulation Statistics ===\n";
        ss << "Total steps: " << total_steps << "\n";
        ss << "Firing rate: mean=" << mean_firing_rate 
           << " ±" << std_firing_rate << " (range: " << min_firing_rate << "-" << max_firing_rate << ")\n";
        ss << "Curiosity level: mean=" << mean_curiosity << "\n";
        ss << "Novelty level: mean=" << mean_novelty << "\n";
        ss << "Prediction error: mean=" << mean_prediction_error << "\n";
        return ss.str();
    }
};

// Environment setup helpers
struct WorldConfig {
    int width;
    int height;
    int vision_width;
    int vision_height;
    float max_energy;
    float energy_decay_rate;
    float agent_start_x;
    float agent_start_y;
    
    WorldConfig() : width(20), height(20), vision_width(8), vision_height(8),
                   max_energy(100.0f), energy_decay_rate(0.01f),
                   agent_start_x(10.0f), agent_start_y(10.0f) {}
};

// Learning configuration
struct LearningConfig {
    bool enable_reward_modulation;
    bool enable_structural_plasticity;
    bool enable_development;
    bool enable_curiosity;
    float reward_scale;
    float curiosity_threshold;
    
    LearningConfig() : enable_reward_modulation(true), enable_structural_plasticity(false),
                      enable_development(true), enable_curiosity(true), reward_scale(1.0f),
                      curiosity_threshold(0.5f) {}
};

} // namespace helper_utils
