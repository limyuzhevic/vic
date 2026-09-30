// Advanced configuration system for NLM (Phase 2+)
// Provides advanced users with fine-grained control over neural simulation parameters

namespace nlm {

void Config::setAdvancedSimulationParameters(const std::string& prefix) {
    // Advanced simulation control parameters
    set(prefix + "simulation_advance.enable_realtime_mode", false, ConfigSource::Runtime);
    set(prefix + "simulation_advance.use_adaptive_timestep", false, ConfigSource::Runtime);
    set(prefix + "simulation_advance.max_time_step", 0.001, ConfigSource::Runtime);
    set(prefix + "simulation_advance.min_time_step", 0.000001, ConfigSource::Runtime);
    set(prefix + "simulation_advance.time_step_adaptation_factor", 0.1f, ConfigSource::Runtime);
    set(prefix + "simulation_advance.stability_threshold", 0.01f, ConfigSource::Runtime);
    set(prefix + "simulation_advance.enable_causal_filtering", false, ConfigSource::Runtime);
    set(prefix + "simulation_advance.causal_filter_order", 2, ConfigSource::Runtime);
    set(prefix + "simulation_advance.enable_concurrent_processing", true, ConfigSource::Runtime);
    set(prefix + "simulation_advance.max_concurrent_neuron_updates", 1024, ConfigSource::Runtime);
}

void Config::setAdvancedPlasticityParameters(const std::string& prefix) {
    // Advanced plasticity control parameters
    set(prefix + "plasticity_advance.enable_eligibility_traces", true, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.eligibility_trace_decay", 0.95f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.eligibility_trace_increment", 1.0f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.enable_weight_normalization", false, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.weight_normalization_factor", 1.0f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.enable_pruning_threshold", true, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.pruning_threshold_percentile", 5.0f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.enable_structural_plasticity", true, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.synaptogenesis_increment", 0.0001f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.pruning_increment", 0.00001f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.plasticity_modulation_rate", 0.001f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.enable_reinforcement_learning", false, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.reinforcement_learning_rate", 0.01f, ConfigSource::Runtime);
    set(prefix + "plasticity_advance.reward_prediction_error_decay", 0.9f, ConfigSource::Runtime);
}

void Config::setAdvancedMemoryParameters(const std::string& prefix) {
    // Advanced memory system parameters
    set(prefix + "memory_advance.working_memory_capacity_multiplier", 1.0f, ConfigSource::Runtime);
    set(prefix + "memory_advance.working_memory_decay_rate", 0.99f, ConfigSource::Runtime);
    set(prefix + "memory_advance.working_memory_competition_strength", 1.0f, ConfigSource::Runtime);
    set(prefix + "memory_advance.episodic_memory_max_episodes", 10000, ConfigSource::Runtime);
    set(prefix + "memory_advance.episodic_memory_decay_rate", 0.999f, ConfigSource::Runtime);
    set(prefix + "memory_advance.episodic_memory_compression_enabled", true, ConfigSource::Runtime);
    set(prefix + "memory_advance.associative_memory_max_associations", 50000, ConfigSource::Runtime);
    set(prefix + "memory_advance.associative_memory_similarity_threshold", 0.1f, ConfigSource::Runtime);
    set(prefix + "memory_advance.associative_memory_aging_rate", 0.0001f, ConfigSource::Runtime);
    set(prefix + "memory_advance.memory_replay_frequency", 100, ConfigSource::Runtime);
    set(prefix + "memory_advance.memory_replay_strength", 0.5f, ConfigSource::Runtime);
    set(prefix + "memory_advance.memory_replay_selectivity", 0.01f, ConfigSource::Runtime);
}

void Config::setAdvancedNeuromodulationParameters(const std::string& prefix) {
    // Advanced neuromodulation control parameters
    set(prefix + "neuromodulation_advance.dopamine_plasticity_modulation_enabled", true, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.dopamine_plasticity_modulation_strength", 0.1f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.curiosity_exploration_rate", 0.01f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.novelty_detection_sensitivity", 0.5f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.prediction_error_learning_rate", 0.1f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.rewards_prediction_error_weight", 0.5f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.neuromodulator_decay_rate", 0.99f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.neuromodulator_response_time_constant", 1000.0f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.neuromodulator_saturation_level", 1.0f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.neuromodulator_baseline_level", 0.1f, ConfigSource::Runtime);
    set(prefix + "neuromodulation_advance.cross_modulation_enabled", true, ConfigSource::Runtime);
}

void Config::setAdvancedDevelopmentParameters(const std::string& prefix) {
    // Advanced development system parameters
    set(prefix + "development_advance.enable_developmental_stages", true, ConfigSource::Runtime);
    set(prefix + "development_advance.critical_period_start", 100, ConfigSource::Runtime);
    set(prefix + "development_advance.critical_period_end", 1000, ConfigSource::Runtime);
    set(prefix + "development_advance.maturation_rate_multiplier", 0.5f, ConfigSource::Runtime);
    set(prefix + "development_advance.aging_rate", 0.00001f, ConfigSource::Runtime);
    set(prefix + "development_advance.development_plasticity_decay", 0.95f, ConfigSource::Runtime);
    set(prefix + "development_advance.neurogenesis_rate", 0.00005f, ConfigSource::Runtime);
    set(prefix + "development_advance.myelination_rate", 0.001f, ConfigSource::Runtime);
    set(prefix + "development_advance.synapse_pruning_intensity", 0.01f, ConfigSource::Runtime);
    set(prefix + "development_advance.connectivity_expansion_rate", 0.001f, ConfigSource::Runtime);
    set(prefix + "development_advance.environmental_influence_strength", 0.5f, ConfigSource::Runtime);
}

void Config::setPerformanceMonitoringParameters(const std::string& prefix) {
    // Performance monitoring and analysis parameters
    set(prefix + "performance.enable_memory_monitoring", true, ConfigSource::Runtime);
    set(prefix + "performance.memory_monitoring_interval", 1000, ConfigSource::Runtime);
    set(prefix + "performance.memory_monitoring_threshold", 0.8f, ConfigSource::Runtime);
    set(prefix + "performance.enable_cpu_profiling", false, ConfigSource::Runtime);
    set(prefix + "performance.cpu_profiling_interval", 10000, ConfigSource::Runtime);
    set(prefix + "performance.enable_neural_activity_monitoring", true, ConfigSource::Runtime);
    set(prefix + "performance.neural_activity_monitoring_interval", 100, ConfigSource::Runtime);
    set(prefix + "performance.enable_plasticity_monitoring", true, ConfigSource::Runtime);
    set(prefix + "performance.plasticity_monitoring_interval", 500, ConfigSource::Runtime);
    set(prefix + "performance.enable_behavioral_metrics", true, ConfigSource::Runtime);
    set(prefix + "performance.behavioral_metrics_interval", 1000, ConfigSource::Runtime);
    set(prefix + "performance.performance_log_level", "INFO", ConfigSource::Runtime);
    set(prefix + "performance.performance_metrics_file", "performance.log", ConfigSource::Runtime);
    set(prefix + "performance.enable_benchmarking", false, ConfigSource::Runtime);
    set(prefix + "performance.benchmark_duration", 3600, ConfigSource::Runtime);
}

void Config::setDebugParameters(const std::string& prefix) {
    // Advanced debugging and development parameters
    set(prefix + "debug.enable_debug_mode", false, ConfigSource::Runtime);
    set(prefix + "debug.debug_log_level", "INFO", ConfigSource::Runtime);
    set(prefix + "debug.debug_output_file", "nlm_debug.log", ConfigSource::Runtime);
    set(prefix + "debug.enable_trace_logging", false, ConfigSource::Runtime);
    set(prefix + "debug.trace_logging_depth", 5, ConfigSource::Runtime);
    set(prefix + "debug.enable_performance_profiling", false, ConfigSource::Runtime);
    set(prefix + "debug.profile_sampling_rate", 1000, ConfigSource::Runtime);
    set(prefix + "debug.enable_memory_leak_detection", false, ConfigSource::Runtime);
    set(prefix + "debug.memory_leak_detection_interval", 10000, ConfigSource::Runtime);
    set(prefix + "debug.enable_network_profiling", false, ConfigSource::Runtime);
    set(prefix + "debug.network_profiling_detail_level", 2, ConfigSource::Runtime);
    set(prefix + "debug.enable_visualization_data_capture", false, ConfigSource::Runtime);
    set(prefix + "debug.visualization_capture_rate", 100, ConfigSource::Runtime);
    set(prefix + "debug.enable_replay_debugging", false, ConfigSource::Runtime);
    set(prefix + "debug.replay_debugging_capture", "replay_data.json", ConfigSource::Runtime);
}

void Config::setVisualizationParameters(const std::string& prefix) {
    // Visualization and output parameters
    set(prefix + "visualization.enable_neural_visualization", true, ConfigSource::Runtime);
    set(prefix + "visualization.neural_visualization_resolution", 256, ConfigSource::Runtime);
    set(prefix + "visualization.neural_visualization_fps", 30, ConfigSource::Runtime);
    set(prefix + "visualization.enable_activity_heatmaps", true, ConfigSource::Runtime);
    set(prefix + "visualization.heatmap_update_rate", 10, ConfigSource::Runtime);
    set(prefix + "visualization.heatmap_sensitivity", 1.0f, ConfigSource::Runtime);
    set(prefix + "visualization.enable_connectivity_visualization", true, ConfigSource::Runtime);
    set(prefix + "visualization.connectivity_visualization_threshold", 0.01f, ConfigSource::Runtime);
    set(prefix + "visualization.enable_memory_visualization", true, ConfigSource::Runtime);
    set(prefix + "visualization.memory_visualization_update_rate", 100, ConfigSource::Runtime);
    set(prefix + "visualization.enable_developmental_visualization", true, ConfigSource::Runtime);
    set(prefix + "visualization.developmental_visualization_rate", 1000, ConfigSource::Runtime);
    set(prefix + "visualization.output_directory", "./visualizations", ConfigSource::Runtime);
    set(prefix + "visualization.output_format", "json", ConfigSource::Runtime);
    set(prefix + "visualization.enable_animation_export", false, ConfigSource::Runtime);
    set(prefix + "visualization.animation_fps", 15, ConfigSource::Runtime);
}

void Config::setAdvancedExperimentParameters(const std::string& prefix) {
    // Advanced experiment control parameters
    set(prefix + "experiment.enable_stimulus_control", true, ConfigSource::Runtime);
    set(prefix + "experiment.stimulus_adaptation_rate", 0.1f, ConfigSource::Runtime);
    set(prefix + "experiment.stimulus_complexity_threshold", 0.5f, ConfigSource::Runtime);
    set(prefix + "experiment.enable_environment_control", true, ConfigSource::Runtime);
    set(prefix + "experiment.environment_switch_interval", 1000, ConfigSource::Runtime);
    set(prefix + "experiment.enable_behavioral_feedback", true, ConfigSource::Runtime);
    set(prefix + "experiment.behavioral_feedback_gain", 0.5f, ConfigSource::Runtime);
    set(prefix + "experiment.enable_learning_objective_tracking", true, ConfigSource::Runtime);
    set(prefix + "experiment.objective_tracking_interval", 100, ConfigSource::Runtime);
    set(prefix + "experiment.enable_performance_objectives", true, ConfigSource::Runtime);
    set(prefix + "experiment.performance_objective_weights", std::vector<float>({1.0f, 1.0f, 1.0f}), ConfigSource::Runtime);
    set(prefix + "experiment.enable_reinforcement_learning_objectives", false, ConfigSource::Runtime);
    set(prefix + "experiment.rl_objective_reward_scale", 1.0f, ConfigSource::Runtime);
    set(prefix + "experiment.rl_objective_risk_penalty", 0.1f, ConfigSource::Runtime);
    set(prefix + "experiment.enable_constraint_based_objectives", true, ConfigSource::Runtime);
    set(prefix + "experiment.constraint_violation_penalty", 10.0f, ConfigSource::Runtime);
}

void Config::loadAdvancedConfiguration(const std::string& filename) {
    // Load advanced configuration from JSON file
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open advanced configuration file: " + filename);
        return;
    }
    
    try {
        // Parse JSON file for advanced parameters
        // This would typically use a JSON parser like nlohmann::json
        // For now, we'll use the existing file format
        loadFromFile(filename);
        
        NLM_LOG_INFO("Advanced configuration loaded from: " + filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load advanced configuration: ") + e.what());
    }
}

void Config::saveAdvancedConfiguration(const std::string& filename) const {
    // Save advanced configuration to JSON file
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open advanced configuration file for writing: " + filename);
        return;
    }
    
    try {
        // Write advanced configuration in JSON format
        file << "{\n";
        
        // Write all configuration entries
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            
            file << "  \"" << entry.key << "\": ";
            
            // Write value based on type
            std::visit([&file](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t> ||
                                     std::is_same_v<T, double>) {
                    file << value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << value[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << value[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << value[i] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        
        file << "}\n";
        
        NLM_LOG_INFO("Advanced configuration saved to: " + filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save advanced configuration: ") + e.what());
    }
}

void Config::exportConfiguration(const std::string& filename) const {
    // Export configuration to human-readable format
    saveToFile(filename);
    NLM_LOG_INFO("Configuration exported to: " + filename);
}

void Config::importConfiguration(const std::string& filename) {
    // Import configuration from human-readable format
    loadFromFile(filename);
    NLM_LOG_INFO("Configuration imported from: " + filename);
}

bool Config::validateAdvancedConfiguration() const {
    // Validate advanced configuration parameters
    bool isValid = true;
    
    // Check for required parameters
    std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            NLM_LOG_WARNING("Missing required configuration parameter: " + param);
            isValid = false;
        }
    }
    
    // Validate parameter values
    auto neuronCount = getOr<int64_t>("neuron_count", 0);
    auto regionCount = getOr<int64_t>("region_count", 0);
    auto timestep = getOr<double>("simulation_timestep", 0.0);
    auto connectionProb = getOr<float>("connection_probability", -1.0f);
    
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be positive.");
        isValid = false;
    }
    if (regionCount <= 0) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount) + ". Must be positive.");
        isValid = false;
    }
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be positive.");
        isValid = false;
    }
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProb) + ". Must be between 0 and 1.");
        isValid = false;
    }
    
    // Check advanced parameters
    auto synapticWeight = getOr<float>("stdp_ltp_weight", 0.0f);
    if (synapticWeight < 0.0f || synapticWeight > 1.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltp_weight: " + std::to_string(synapticWeight) + ". Must be between 0 and 1.");
        isValid = false;
    }
    
    if (isValid) {
        NLM_LOG_INFO("Advanced configuration validation passed.");
    } else {
        NLM_LOG_ERROR("Advanced configuration validation failed.");
    }
    
    return isValid;
}

std::string Config::generateConfigurationReport() const {
    // Generate a comprehensive configuration report
    std::ostringstream report;
    
    report << "=== NLM Configuration Report ===\n";
    report << "Generated: " << getCurrentTimestamp() << "\n";
    report << "Total parameters: " << pImpl->entries.size() << "\n\n";
    
    report << "=== Core Parameters ===\n";
    report << "Neuron Count: " << getOr<int64_t>("neuron_count", 0) << "\n";
    report << "Region Count: " << getOr<int64_t>("region_count", 0) << "\n";
    report << "Simulation Timestep: " << getOr<double>("simulation_timestep", 0.0) << " ms\n";
    report << "Connection Probability: " << getOr<float>("connection_probability", 0.0f) << "\n";
    report << "Random Seed: " << getOr<int64_t>("random_seed", 0) << "\n\n";
    
    report << "=== Plasticity Parameters ===\n";
    report << "STDP LTP Weight: " << getOr<float>("stdp_ltp_weight", 0.0f) << "\n";
    report << "STDP LTD Weight: " << getOr<float>("stdp_ltd_weight", 0.0f) << "\n";
    report << "STDP Tau: " << getOr<float>("stdp_tau", 0.0f) << " ms\n";
    report << "Synaptogenesis Rate: " << getOr<float>("synaptogenesis_rate", 0.0f) << "\n";
    report << "Pruning Rate: " << getOr<float>("pruning_rate", 0.0f) << "\n\n";
    
    report << "=== Memory Parameters ===\n";
    report << "Working Memory Capacity: " << getOr<size_t>("working_memory_capacity", 0) << "\n";
    report << "Episodic Memory Max Episodes: " << getOr<size_t>("episodic_memory_max_episodes", 0) << "\n";
    report << "Associative Memory Max Associations: " << getOr<size_t>("associative_memory_max_associations", 0) << "\n\n";
    
    report << "=== Neuromodulation Parameters ===\n";
    report << "Dopamine Level: " << getOr<float>("dopamine_level", 0.0f) << "\n";
    report << "Curiosity Level: " << getOr<float>("curiosity_level", 0.0f) << "\n";
    report << "Novelty Level: " << getOr<float>("novelty_level", 0.0f) << "\n";
    report << "Prediction Error Level: " << getOr<float>("prediction_error_level", 0.0f) << "\n\n";
    
    report << "=== Development Parameters ===\n";
    report << "Developmental Stage: " << static_cast<int>(getOr<int64_t>("developmental_stage", 0)) << "\n";
    report << "Critical Period: " << getOr<size_t>("critical_period_duration", 0) << " steps\n";
    report << "Maturation Progress: " << getOr<float>("development_maturation_progress", 0.0f) << "\n\n";
    
    report << "=== Configuration Sources ===\n";
    for (const auto& entry : pImpl->entries) {
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default:
                sourceStr = "Default";
                break;
            case ConfigSource::File:
                sourceStr = "File";
                break;
            case ConfigSource::CommandLine:
                sourceStr = "Command Line";
                break;
            case ConfigSource::Runtime:
                sourceStr = "Runtime";
                break;
            default:
                sourceStr = "Unknown";
                break;
        }
        report << entry.key << " (Source: " << sourceStr << ")\n";
    }
    
    report << "\n=== End of Report ===\n";
    
    return report.str();
}

std::string Config::getCurrentTimestamp() const {
    // Get current timestamp for logging
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm