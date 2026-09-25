#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <optional>
#include <functional>
#include <chrono>
#include <thread>
#include <numeric>
#include <cmath>
#include <complex>
#include "src/core/Config/Config.hpp"

namespace nlm {

// Advanced Configuration Types

// Plasticity parameter configurations
struct AdvancedPlasticityParams {
    // STDP parameters
    struct STDPConfig {
        float ltp_weight = 0.01f;           // Long-term potentiation weight
        float ltd_weight = 0.012f;          // Long-term depression weight
        float tau = 20.0f;                   // Time constant for STDP
        float ltp_decay_rate = 0.95f;       // LTP weight decay rate
        float ltd_decay_rate = 0.95f;       // LTD weight decay rate
        bool neuromodulated = true;          // Whether STDP is affected by neuromodulators
        float neuromodulation_factor = 1.0f; // Scaling factor for neuromodulation
    } stdp;
    
    // Hebbian parameters
    struct HebbianConfig {
        float learning_rate = 0.001f;        // Learning rate for Hebbian learning
        float saturation_threshold = 0.8f;  // Weight saturation threshold
        bool hebbian_synaptic_pruning = false; // Enable synaptic pruning
        float pruning_threshold = 0.05f;    // Threshold for pruning weak synapses
        bool local_update = true;            // Local Hebbian updates vs global
    } hebbian;
    
    // Structural plasticity parameters
    struct StructuralConfig {
        float synaptogenesis_rate = 0.0001f;  // Rate of new synapse formation
        float pruning_rate = 0.00001f;       // Rate of synapse elimination
        bool developmental_pruning = true;    // Developmental-stage dependent pruning
        bool activity_dependent_pruning = true; // Prune based on activity
        float min_weight = 0.001f;            // Minimum weight for survival
        float activity_threshold = 0.1f;     // Activity threshold for structural changes
    } structural;
    
    // Neuromodulation parameters
    struct NeuromodulationConfig {
        bool enable_dopamine = true;
        bool enable_curiosity = true;
        bool enable_prediction_error = true;
        bool enable_novelty = true;
        
        // Dopamine parameters
        float dopamine_learning_rate = 0.01f;
        float dopamine_modulation_strength = 1.0f;
        float dopamine_timewindow = 5000.0f; // ms
        float baseline_dopamine = 0.1f;
        
        // Curiosity parameters
        float curiosity_threshold = 0.5f;
        float curiosity_decay_rate = 0.99f;
        float exploration_bonus = 0.5f;
        
        // Prediction error parameters
        float prediction_error_weight = 0.1f;
        float prediction_error_decay = 0.95f;
        
        // Novelty parameters
        float novelty_threshold = 0.7f;
        float novelty_decay_rate = 0.98f;
        bool novelty_based_plasticity = true;
    } neuromodulation;
    
    // Custom connectivity patterns
    enum class ConnectivityPattern {
        Random,               // Standard random connectivity
        SmallWorld,          // Small-world network (Watts-Strogatz)
        ScaleFree,           // Scale-free network (Barabási–Albert)
        Community,            // Community-structured networks
        Modularity,          // Modular network structure
        Hexagonal,           // Hexagonal lattice
        Ring,                // Ring lattice
        Geometric,            // Geometric distance-based
        Adaptive,            // Activity-dependent connectivity
        None                 // No connectivity (isolated neurons)
    };
    
    ConnectivityPattern connectivity_pattern = ConnectivityPattern::Random;
    float connectivity_strength = 1.0f;
    float modularity_parameter = 2.0f;     // For community structure
    float clustering_coefficient = 0.3f;   // For small-world networks
    size_t avg_degree = 4;                  // Average number of connections per neuron
    
    // Developmental stage customization
    struct DevelopmentalStageConfig {
        enum class Stage {
            Embryonic,
            Larval,
            Juvenile,
            Adolescent,
            Adult
        };
        
        std::map<Stage, float> plasticity_rates;      // Plasticity rate per stage
        std::map<Stage, float> excitability_factors;  // Neural excitability per stage
        std::map<Stage, float> connectivity_rates;   // Connectivity development rate per stage
        std::map<Stage, float> memory_capacity;       // Memory capacity limits per stage
        
        float critical_period_start = 0.2f;     // Start of critical period (0-1)
        float critical_period_end = 0.8f;       // End of critical period (0-1)
        bool enable_critical_period = true;
        bool enable_maturational_pruning = true;
        bool enable_activity_dependent_development = true;
    } development;
    
    // Advanced simulation parameters
    struct AdvancedSimulationConfig {
        bool enable_multi_agent = false;        // Enable multi-agent simulation
        bool enable_heterogeneous_populations = false; // Different neuron types
        bool enable_custom_world_models = false; // Custom environment models
        bool enable_advanced_sensory_processing = false; // Advanced sensory systems
        
        // Performance optimization
        bool enable_vectorization = true;      // SIMD vectorization
        bool enable_gpu_acceleration = false;   // GPU acceleration (placeholder)
        bool enable_parallel_processing = true; // Multi-threading
        size_t simulation_threads = 4;        // Number of worker threads
        
        // Time management
        float simulation_speed_factor = 1.0f;  // Speed up/slow down simulation
        bool enable_hierarchical_time = false; // Multi-scale time simulation
        float fast_forward_threshold = 1000.0f; // Fast-forward threshold in ms
        
        // Data management
        bool enable_real_time_logging = true;  // Log during simulation
        bool enable_checkpoint_compression = true;
        float checkpoint_interval = 100.0f;    // Time between checkpoints (steps)
        
        // Research features
        bool enable_parameter_sweeps = false;  // Automated parameter sweeps
        bool enable_machine_learning = false;   // ML integration
        bool enable_data_recording = true;      // Record simulation data
        bool enable_statistical_validation = true; // Statistical analysis
    } simulation;
    
    // Network analysis and metrics
    struct AnalysisConfig {
        bool enable_network_analysis = true;    // Analyze network properties
        bool enable_graph_theory_metrics = true; // Graph theory calculations
        bool enable_statistical_analysis = true; // Statistical analysis of activity
        bool enable_visualization_tools = true; // Visualization capabilities
        bool enable_data_export = true;         // Export results
        
        // Network analysis parameters
        bool calculate_clustering_coefficient = true;
        bool calculate_path_length = true;
        bool calculate_degree_distribution = true;
        bool calculate_small_worldness = true;
        bool calculate_betweenness_centrality = true;
        bool calculate_eigenvector_centrality = true;
        
        // Activity analysis parameters
        bool calculate_firing_rate_statistics = true;
        bool calculate_synchrony_measures = true;
        bool calculate_information_flow = true;
        bool calculate_entropy_measures = true;
        
        // Data export options
        bool export_to_matlab = true;
        bool export_to_python = true;
        bool export_to_csv = true;
        bool export_to_json = true;
        bool export_to_hdf5 = false;           // Requires HDF5 library
        
        // Performance profiling
        bool enable_performance_profiling = true;
        bool profile_cpu_usage = true;
        bool profile_memory_usage = true;
        bool profile_network_bandwidth = false;
        bool profile_simulation_speed = true;
    } analysis;
    
    // Research and experiment parameters
    struct ResearchConfig {
        bool enable_experiment_parameter_sweeps = false; // Automated parameter exploration
        bool enable_machine_learning_integration = false;  // ML model integration
        bool enable_data_logging = true;                    // Comprehensive logging
        bool enable_statistical_validation = true;          // Statistical testing
        
        // Parameter sweep configuration
        bool sweep_enable_random_search = true;           // Random parameter search
        bool sweep_enable_grid_search = true;            // Grid-based parameter search
        bool sweep_enable_hill_climbing = true;          // Local optimization
        bool sweep_enable_genetic_algorithm = false;      // Evolutionary algorithms
        
        // Machine learning integration
        bool ml_enable_neural_network = false;            // Embed ML models
        bool ml_enable_reinforcement_learning = false;    // RL integration
        bool ml_enable_supervised_learning = false;       // Supervised learning
        bool ml_enable_unsupervised_learning = false;     // Clustering, etc.
        
        // Data logging
        bool log_enable_real_time = true;                // Real-time data capture
        bool log_enable_raw_spikes = true;               // Record individual spikes
        bool log_enable_network_state = true;            // Record network connectivity
        bool log_enable_memory_traces = true;            // Record memory operations
        bool log_enable_plasticity_events = true;        // Record synaptic changes
        
        // Statistical validation
        float validate_p_value_threshold = 0.05f;         // Statistical significance threshold
        float validate_confidence_interval = 0.95f;       // Confidence level
        bool validate_power_analysis = true;              // Statistical power calculation
        bool validate_effect_size = true;                 // Effect size calculation
    } research;
    
    // Advanced API configurations
    struct APIConfig {
        bool enable_batch_operations = true;              // Batch processing for large simulations
        bool enable_streaming_apis = true;               // Real-time data streaming
        bool enable_high_level_apis = true;              // Abstract, user-friendly APIs
        bool enable_low_level_apis = false;              // Performance-critical APIs
        
        // Streaming API options
        bool stream_enable_spike_streaming = true;        // Stream spike events
        bool stream_enable_state_streaming = true;        // Stream network state
        bool stream_enable_metric_streaming = true;       // Stream computed metrics
        bool stream_enable_plot_streaming = true;         // Stream visualizations
        
        // Performance APIs
        bool enable_performance_monitoring = true;        // Real-time performance metrics
        bool enable_memory_profiling = true;              // Memory usage tracking
        bool enable_computation_profiling = true;         // CPU usage tracking
        bool enable_network_io_profiling = false;         // Network I/O monitoring
        
        // High-level APIs
        bool enable_scenario_apis = true;                 // Pre-defined simulation scenarios
        bool enable_research_apis = true;                 // Research-oriented APIs
        bool enable_monitoring_apis = true;               // System monitoring APIs
    } api;
    
    // Default constructor with optimized values
    AdvancedPlasticityParams() {
        // Initialize with biologically plausible defaults
        initializeDefaults();
    }
    
    // Initialize with research-oriented defaults
    void initializeDefaults() {
        // Plasticity defaults
        stdp.ltp_weight = 0.01f;
        stdp.ltd_weight = 0.012f;
        stdp.tau = 20.0f;
        
        hebbian.learning_rate = 0.001f;
        hebbian.saturation_threshold = 0.8f;
        
        structural.synaptogenesis_rate = 0.0001f;
        structural.pruning_rate = 0.00001f;
        
        neuromodulation.dopamine_learning_rate = 0.01f;
        neuromodulation.dopamine_modulation_strength = 1.0f;
        neuromodulation.baseline_dopamine = 0.1f;
        
        // Development defaults
        development.plasticity_rates[DevelopmentalStageConfig::Stage::Embryonic] = 1.0f;
        development.plasticity_rates[DevelopmentalStageConfig::Stage::Larval] = 0.8f;
        development.plasticity_rates[DevelopmentalStageConfig::Stage::Juvenile] = 0.6f;
        development.plasticity_rates[DevelopmentalStageConfig::Stage::Adolescent] = 0.3f;
        development.plasticity_rates[DevelopmentalStageConfig::Stage::Adult] = 0.1f;
        
        development.excitability_factors[DevelopmentalStageConfig::Stage::Embryonic] = 0.5f;
        development.excitability_factors[DevelopmentalStageConfig::Stage::Adult] = 1.0f;
        
        // Simulation defaults
        simulation.enable_vectorization = true;
        simulation.enable_parallel_processing = true;
        simulation.simulation_threads = 4;
        simulation.simulation_speed_factor = 1.0f;
        
        // Analysis defaults
        analysis.calculate_clustering_coefficient = true;
        analysis.calculate_path_length = true;
        analysis.calculate_degree_distribution = true;
        
        // Research defaults
        research.log_enable_real_time = true;
        research.log_enable_raw_spikes = true;
        research.log_enable_network_state = true;
        
        // API defaults
        api.enable_batch_operations = true;
        api.enable_streaming_apis = true;
        api.enable_high_level_apis = true;
    }
    
    // Validate all parameters
    bool validate() const {
        // Check STDP parameters
        if (stdp.ltp_weight < 0.0f || stdp.ltp_weight > 1.0f) return false;
        if (stdp.ltd_weight < 0.0f || stdp.ltd_weight > 1.0f) return false;
        if (stdp.tau <= 0.0f) return false;
        
        // Check Hebbian parameters
        if (hebbian.learning_rate < 0.0f) return false;
        if (hebbian.saturation_threshold <= 0.0f || hebbian.saturation_threshold > 1.0f) return false;
        if (hebbian.pruning_threshold <= 0.0f || hebbian.pruning_threshold > 1.0f) return false;
        
        // Check structural parameters
        if (structural.synaptogenesis_rate < 0.0f || structural.synaptogenesis_rate > 1.0f) return false;
        if (structural.pruning_rate < 0.0f || structural.pruning_rate > 1.0f) return false;
        
        // Check neuromodulation parameters
        if (neuromodulation.dopamine_learning_rate < 0.0f || neuromodulation.dopamine_learning_rate > 1.0f) return false;
        if (neuromodulation.baseline_dopamine < 0.0f || neuromodulation.baseline_dopamine > 1.0f) return false;
        
        // Check developmental parameters
        if (development.critical_period_start >= development.critical_period_end) return false;
        if (development.critical_period_start < 0.0f || development.critical_period_start > 1.0f) return false;
        if (development.critical_period_end < 0.0f || development.critical_period_end > 1.0f) return false;
        
        // Check simulation parameters
        if (simulation.simulation_speed_factor <= 0.0f) return false;
        if (simulation.simulation_threads == 0) return false;
        
        // Check API parameters
        if (api.stream_enable_spike_streaming || api.stream_enable_state_streaming ||
            api.stream_enable_metric_streaming || api.stream_enable_plot_streaming) {
            if (!simulation.enable_real_time_logging) return false;
        }
        
        return true;
    }
    
    // Apply configuration to a Config object
    void applyToConfig(Config& config) const {
        // STDP parameters
        config.set("stdp_ltp_weight", stdp.ltp_weight);
        config.set("stdp_ltd_weight", stdp.ltd_weight);
        config.set("stdp_tau", stdp.tau);
        config.set("stdp_neuromodulated", stdp.neuromodulated);
        config.set("stdp_neuromodulation_factor", stdp.neuromodulation_factor);
        
        // Hebbian parameters
        config.set("hebbian_learning_rate", hebbian.learning_rate);
        config.set("hebbian_saturation_threshold", hebbian.saturation_threshold);
        config.set("hebbian_hebbian_synaptic_pruning", hebbian.hebbian_synaptic_pruning);
        config.set("hebbian_pruning_threshold", hebbian.pruning_threshold);
        config.set("hebbian_local_update", hebbian.local_update);
        
        // Structural parameters
        config.set("synaptogenesis_rate", structural.synaptogenesis_rate);
        config.set("pruning_rate", structural.pruning_rate);
        config.set("structural_developmental_pruning", structural.developmental_pruning);
        config.set("structural_activity_dependent_pruning", structural.activity_dependent_pruning);
        config.set("structural_min_weight", structural.min_weight);
        config.set("structural_activity_threshold", structural.activity_threshold);
        
        // Neuromodulation parameters
        config.set("neuromodulation_enable_dopamine", neuromodulation.enable_dopamine);
        config.set("neuromodulation_enable_curiosity", neuromodulation.enable_curiosity);
        config.set("neuromodulation_enable_prediction_error", neuromodulation.enable_prediction_error);
        config.set("neuromodulation_enable_novelty", neuromodulation.enable_novelty);
        config.set("neuromodulation_dopamine_learning_rate", neuromodulation.dopamine_learning_rate);
        config.set("neuromodulation_dopamine_modulation_strength", neuromodulation.dopamine_modulation_strength);
        config.set("neuromodulation_dopamine_timewindow", neuromodulation.dopamine_timewindow);
        config.set("neuromodulation_baseline_dopamine", neuromodulation.baseline_dopamine);
        config.set("neuromodulation_curiosity_threshold", neuromodulation.curiosity_threshold);
        config.set("neuromodulation_curiosity_decay_rate", neuromodulation.curiosity_decay_rate);
        config.set("neuromodulation_exploration_bonus", neuromodulation.exploration_bonus);
        
        // Connectivity pattern
        config.set("connectivity_pattern", static_cast<int>(connectivity_pattern));
        config.set("connectivity_strength", connectivity_strength);
        config.set("connectivity_modularity_parameter", modularity_parameter);
        config.set("connectivity_clustering_coefficient", clustering_coefficient);
        config.set("connectivity_avg_degree", static_cast<int>(avg_degree));
        
        // Development parameters
        config.set("development_critical_period_start", development.critical_period_start);
        config.set("development_critical_period_end", development.critical_period_end);
        config.set("development_enable_critical_period", development.enable_critical_period);
        config.set("development_enable_maturational_pruning", development.enable_maturational_pruning);
        config.set("development_enable_activity_dependent_development", development.enable_activity_dependent_development);
        
        // Simulation parameters
        config.set("simulation_enable_multi_agent", simulation.enable_multi_agent);
        config.set("simulation_enable_heterogeneous_populations", simulation.enable_heterogeneous_populations);
        config.set("simulation_enable_custom_world_models", simulation.enable_custom_world_models);
        config.set("simulation_enable_advanced_sensory_processing", simulation.enable_advanced_sensory_processing);
        config.set("simulation_enable_vectorization", simulation.enable_vectorization);
        config.set("simulation_enable_gpu_acceleration", simulation.enable_gpu_acceleration);
        config.set("simulation_enable_parallel_processing", simulation.enable_parallel_processing);
        config.set("simulation_simulation_threads", static_cast<int>(simulation.simulation_threads));
        config.set("simulation_simulation_speed_factor", simulation.simulation_speed_factor);
        config.set("simulation_enable_hierarchical_time", simulation.enable_hierarchical_time);
        config.set("simulation_fast_forward_threshold", simulation.fast_forward_threshold);
        config.set("simulation_enable_real_time_logging", simulation.enable_real_time_logging);
        config.set("simulation_enable_checkpoint_compression", simulation.enable_checkpoint_compression);
        config.set("simulation_checkpoint_interval", simulation.checkpoint_interval);
        
        // Analysis parameters
        config.set("analysis_enable_network_analysis", analysis.enable_network_analysis);
        config.set("analysis_enable_graph_theory_metrics", analysis.enable_graph_theory_metrics);
        config.set("analysis_enable_statistical_analysis", analysis.enable_statistical_analysis);
        config.set("analysis_enable_visualization_tools", analysis.enable_visualization_tools);
        config.set("analysis_enable_data_export", analysis.enable_data_export);
        config.set("analysis_calculate_clustering_coefficient", analysis.calculate_clustering_coefficient);
        config.set("analysis_calculate_path_length", analysis.calculate_path_length);
        config.set("analysis_calculate_degree_distribution", analysis.calculate_degree_distribution);
        config.set("analysis_calculate_small_worldness", analysis.calculate_small_worldness);
        config.set("analysis_calculate_betweenness_centrality", analysis.calculate_betweenness_centrality);
        config.set("analysis_calculate_eigenvector_centrality", analysis.calculate_eigenvector_centrality);
        config.set("analysis_calculate_firing_rate_statistics", analysis.calculate_firing_rate_statistics);
        config.set("analysis_calculate_synchrony_measures", analysis.calculate_synchrony_measures);
        config.set("analysis_calculate_information_flow", analysis.calculate_information_flow);
        config.set("analysis_calculate_entropy_measures", analysis.calculate_entropy_measures);
        config.set("analysis_export_to_matlab", analysis.export_to_matlab);
        config.set("analysis_export_to_python", analysis.export_to_python);
        config.set("analysis_export_to_csv", analysis.export_to_csv);
        config.set("analysis_export_to_json", analysis.export_to_json);
        config.set("analysis_export_to_hdf5", analysis.export_to_hdf5);
        config.set("analysis_enable_performance_profiling", analysis.enable_performance_profiling);
        config.set("analysis_profile_cpu_usage", analysis.profile_cpu_usage);
        config.set("analysis_profile_memory_usage", analysis.profile_memory_usage);
        config.set("analysis_profile_network_bandwidth", analysis.profile_network_bandwidth);
        config.set("analysis_profile_simulation_speed", analysis.profile_simulation_speed);
        
        // Research parameters
        config.set("research_enable_experiment_parameter_sweeps", research.enable_experiment_parameter_sweeps);
        config.set("research_enable_machine_learning_integration", research.enable_machine_learning_integration);
        config.set("research_enable_data_logging", research.enable_data_logging);
        config.set("research_enable_statistical_validation", research.enable_statistical_validation);
        config.set("research_sweep_enable_random_search", research.sweep_enable_random_search);
        config.set("research_sweep_enable_grid_search", research.sweep_enable_grid_search);
        config.set("research_sweep_enable_hill_climbing", research.sweep_enable_hill_climbing);
        config.set("research_sweep_enable_genetic_algorithm", research.sweep_enable_genetic_algorithm);
        config.set("research_ml_enable_neural_network", research.ml_enable_neural_network);
        config.set("research_ml_enable_reinforcement_learning", research.ml_enable_reinforcement_learning);
        config.set("research_ml_enable_supervised_learning", research.ml_enable_supervised_learning);
        config.set("research_ml_enable_unsupervised_learning", research.ml_enable_unsupervised_learning);
        config.set("research_log_enable_real_time", research.log_enable_real_time);
        config.set("research_log_enable_raw_spikes", research.log_enable_raw_spikes);
        config.set("research_log_enable_network_state", research.log_enable_network_state);
        config.set("research_log_enable_memory_traces", research.log_enable_memory_traces);
        config.set("research_log_enable_plasticity_events", research.log_enable_plasticity_events);
        config.set("research_validate_p_value_threshold", research.validate_p_value_threshold);
        config.set("research_validate_confidence_interval", research.validate_confidence_interval);
        config.set("research_validate_power_analysis", research.validate_power_analysis);
        config.set("research_validate_effect_size", research.validate_effect_size);
        
        // API parameters
        config.set("api_enable_batch_operations", api.enable_batch_operations);
        config.set("api_enable_streaming_apis", api.enable_streaming_apis);
        config.set("api_enable_high_level_apis", api.enable_high_level_apis);
        config.set("api_enable_low_level_apis", api.enable_low_level_apis);
        config.set("api_stream_enable_spike_streaming", api.stream_enable_spike_streaming);
        config.set("api_stream_enable_state_streaming", api.stream_enable_state_streaming);
        config.set("api_stream_enable_metric_streaming", api.stream_enable_metric_streaming);
        config.set("api_stream_enable_plot_streaming", api.stream_enable_plot_streaming);
        config.set("api_enable_performance_monitoring", api.enable_performance_monitoring);
        config.set("api_enable_memory_profiling", api.enable_memory_profiling);
        config.set("api_enable_computation_profiling", api.enable_computation_profiling);
        config.set("api_enable_network_io_profiling", api.enable_network_io_profiling);
        config.set("api_enable_scenario_apis", api.enable_scenario_apis);
        config.set("api_enable_research_apis", api.enable_research_apis);
        config.set("api_enable_monitoring_apis", api.enable_monitoring_apis);
    }
}; // End AdvancedPlasticityParams

// Advanced Configuration class
class AdvancedConfig {
public:
    AdvancedConfig() : plasticity_params() {}
    
    // Set advanced plasticity parameters
    void setPlasticityParams(const AdvancedPlasticityParams& params) {
        plasticity_params = params;
        validate();
    }
    
    // Get advanced plasticity parameters
    const AdvancedPlasticityParams& getPlasticityParams() const { return plasticity_params; }
    
    // Get mutable reference to parameters
    AdvancedPlasticityParams& getMutablePlasticityParams() { return plasticity_params; }
    
    // Update configuration from Config object
    void updateFromConfig(const Config& config) {
        // Extract plasticity parameters from config
        if (auto val = config.get<int>("stdp_ltp_weight")) {
            plasticity_params.stdp.ltp_weight = static_cast<float>(*val);
        }
        if (auto val = config.get<int>("stdp_ltd_weight")) {
            plasticity_params.stdp.ltd_weight = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("stdp_tau")) {
            plasticity_params.stdp.tau = *val;
        }
        if (auto val = config.get<bool>("stdp_neuromodulated")) {
            plasticity_params.stdp.neuromodulated = *val;
        }
        if (auto val = config.get<double>("stdp_neuromodulation_factor")) {
            plasticity_params.stdp.neuromodulation_factor = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("hebbian_learning_rate")) {
            plasticity_params.hebbian.learning_rate = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("hebbian_saturation_threshold")) {
            plasticity_params.hebbian.saturation_threshold = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("hebbian_hebbian_synaptic_pruning")) {
            plasticity_params.hebbian.hebbian_synaptic_pruning = *val;
        }
        if (auto val = config.get<double>("hebbian_pruning_threshold")) {
            plasticity_params.hebbian.pruning_threshold = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("hebbian_local_update")) {
            plasticity_params.hebbian.local_update = *val;
        }
        if (auto val = config.get<double>("synaptogenesis_rate")) {
            plasticity_params.structural.synaptogenesis_rate = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("pruning_rate")) {
            plasticity_params.structural.pruning_rate = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("structural_developmental_pruning")) {
            plasticity_params.structural.developmental_pruning = *val;
        }
        if (auto val = config.get<bool>("structural_activity_dependent_pruning")) {
            plasticity_params.structural.activity_dependent_pruning = *val;
        }
        if (auto val = config.get<double>("structural_min_weight")) {
            plasticity_params.structural.min_weight = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("structural_activity_threshold")) {
            plasticity_params.structural.activity_threshold = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("neuromodulation_enable_dopamine")) {
            plasticity_params.neuromodulation.enable_dopamine = *val;
        }
        if (auto val = config.get<bool>("neuromodulation_enable_curiosity")) {
            plasticity_params.neuromodulation.enable_curiosity = *val;
        }
        if (auto val = config.get<bool>("neuromodulation_enable_prediction_error")) {
            plasticity_params.neuromodulation.enable_prediction_error = *val;
        }
        if (auto val = config.get<bool>("neuromodulation_enable_novelty")) {
            plasticity_params.neuromodulation.enable_novelty = *val;
        }
        if (auto val = config.get<double>("neuromodulation_dopamine_learning_rate")) {
            plasticity_params.neuromodulation.dopamine_learning_rate = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("neuromodulation_dopamine_modulation_strength")) {
            plasticity_params.neuromodulation.dopamine_modulation_strength = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("neuromodulation_dopamine_timewindow")) {
            plasticity_params.neuromodulation.dopamine_timewindow = *val;
        }
        if (auto val = config.get<double>("neuromodulation_baseline_dopamine")) {
            plasticity_params.neuromodulation.baseline_dopamine = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("neuromodulation_curiosity_threshold")) {
            plasticity_params.neuromodulation.curiosity_threshold = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("neuromodulation_curiosity_decay_rate")) {
            plasticity_params.neuromodulation.curiosity_decay_rate = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("neuromodulation_exploration_bonus")) {
            plasticity_params.neuromodulation.exploration_bonus = static_cast<float>(*val);
        }
        if (auto val = config.get<int>("connectivity_pattern")) {
            plasticity_params.connectivity_pattern = static_cast<AdvancedPlasticityParams::ConnectivityPattern>(*val);
        }
        if (auto val = config.get<double>("connectivity_strength")) {
            plasticity_params.connectivity_strength = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("connectivity_modularity_parameter")) {
            plasticity_params.modularity_parameter = *val;
        }
        if (auto val = config.get<double>("connectivity_clustering_coefficient")) {
            plasticity_params.clustering_coefficient = static_cast<float>(*val);
        }
        if (auto val = config.get<int>("connectivity_avg_degree")) {
            plasticity_params.avg_degree = *val;
        }
        if (auto val = config.get<double>("development_critical_period_start")) {
            plasticity_params.development.critical_period_start = *val;
        }
        if (auto val = config.get<double>("development_critical_period_end")) {
            plasticity_params.development.critical_period_end = *val;
        }
        if (auto val = config.get<bool>("development_enable_critical_period")) {
            plasticity_params.development.enable_critical_period = *val;
        }
        if (auto val = config.get<bool>("development_enable_maturational_pruning")) {
            plasticity_params.development.enable_maturational_pruning = *val;
        }
        if (auto val = config.get<bool>("development_enable_activity_dependent_development")) {
            plasticity_params.development.enable_activity_dependent_development = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_multi_agent")) {
            plasticity_params.simulation.enable_multi_agent = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_heterogeneous_populations")) {
            plasticity_params.simulation.enable_heterogeneous_populations = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_custom_world_models")) {
            plasticity_params.simulation.enable_custom_world_models = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_advanced_sensory_processing")) {
            plasticity_params.simulation.enable_advanced_sensory_processing = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_vectorization")) {
            plasticity_params.simulation.enable_vectorization = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_gpu_acceleration")) {
            plasticity_params.simulation.enable_gpu_acceleration = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_parallel_processing")) {
            plasticity_params.simulation.enable_parallel_processing = *val;
        }
        if (auto val = config.get<int>("simulation_simulation_threads")) {
            plasticity_params.simulation.simulation_threads = *val;
        }
        if (auto val = config.get<double>("simulation_simulation_speed_factor")) {
            plasticity_params.simulation.simulation_speed_factor = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("simulation_enable_hierarchical_time")) {
            plasticity_params.simulation.enable_hierarchical_time = *val;
        }
        if (auto val = config.get<double>("simulation_fast_forward_threshold")) {
            plasticity_params.simulation.fast_forward_threshold = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("simulation_enable_real_time_logging")) {
            plasticity_params.simulation.enable_real_time_logging = *val;
        }
        if (auto val = config.get<bool>("simulation_enable_checkpoint_compression")) {
            plasticity_params.simulation.enable_checkpoint_compression = *val;
        }
        if (auto val = config.get<double>("simulation_checkpoint_interval")) {
            plasticity_params.simulation.checkpoint_interval = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("analysis_enable_network_analysis")) {
            plasticity_params.analysis.enable_network_analysis = *val;
        }
        if (auto val = config.get<bool>("analysis_enable_graph_theory_metrics")) {
            plasticity_params.analysis.enable_graph_theory_metrics = *val;
        }
        if (auto val = config.get<bool>("analysis_enable_statistical_analysis")) {
            plasticity_params.analysis.enable_statistical_analysis = *val;
        }
        if (auto val = config.get<bool>("analysis_enable_visualization_tools")) {
            plasticity_params.analysis.enable_visualization_tools = *val;
        }
        if (auto val = config.get<bool>("analysis_enable_data_export")) {
            plasticity_params.analysis.enable_data_export = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_clustering_coefficient")) {
            plasticity_params.analysis.calculate_clustering_coefficient = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_path_length")) {
            plasticity_params.analysis.calculate_path_length = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_degree_distribution")) {
            plasticity_params.analysis.calculate_degree_distribution = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_small_worldness")) {
            plasticity_params.analysis.calculate_small_worldness = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_betweenness_centrality")) {
            plasticity_params.analysis.calculate_betweenness_centrality = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_eigenvector_centrality")) {
            plasticity_params.analysis.calculate_eigenvector_centrality = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_firing_rate_statistics")) {
            plasticity_params.analysis.calculate_firing_rate_statistics = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_synchrony_measures")) {
            plasticity_params.analysis.calculate_synchrony_measures = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_information_flow")) {
            plasticity_params.analysis.calculate_information_flow = *val;
        }
        if (auto val = config.get<bool>("analysis_calculate_entropy_measures")) {
            plasticity_params.analysis.calculate_entropy_measures = *val;
        }
        if (auto val = config.get<bool>("analysis_export_to_matlab")) {
            plasticity_params.analysis.export_to_matlab = *val;
        }
        if (auto val = config.get<bool>("analysis_export_to_python")) {
            plasticity_params.analysis.export_to_python = *val;
        }
        if (auto val = config.get<bool>("analysis_export_to_csv")) {
            plasticity_params.analysis.export_to_csv = *val;
        }
        if (auto val = config.get<bool>("analysis_export_to_json")) {
            plasticity_params.analysis.export_to_json = *val;
        }
        if (auto val = config.get<bool>("analysis_export_to_hdf5")) {
            plasticity_params.analysis.export_to_hdf5 = *val;
        }
        if (auto val = config.get<bool>("analysis_enable_performance_profiling")) {
            plasticity_params.analysis.enable_performance_profiling = *val;
        }
        if (auto val = config.get<bool>("analysis_profile_cpu_usage")) {
            plasticity_params.analysis.profile_cpu_usage = *val;
        }
        if (auto val = config.get<bool>("analysis_profile_memory_usage")) {
            plasticity_params.analysis.profile_memory_usage = *val;
        }
        if (auto val = config.get<bool>("analysis_profile_network_bandwidth")) {
            plasticity_params.analysis.profile_network_bandwidth = *val;
        }
        if (auto val = config.get<bool>("analysis_profile_simulation_speed")) {
            plasticity_params.analysis.profile_simulation_speed = *val;
        }
        if (auto val = config.get<bool>("research_enable_experiment_parameter_sweeps")) {
            plasticity_params.research.enable_experiment_parameter_sweeps = *val;
        }
        if (auto val = config.get<bool>("research_enable_machine_learning_integration")) {
            plasticity_params.research.enable_machine_learning_integration = *val;
        }
        if (auto val = config.get<bool>("research_enable_data_logging")) {
            plasticity_params.research.enable_data_logging = *val;
        }
        if (auto val = config.get<bool>("research_enable_statistical_validation")) {
            plasticity_params.research.enable_statistical_validation = *val;
        }
        if (auto val = config.get<bool>("research_sweep_enable_random_search")) {
            plasticity_params.research.sweep_enable_random_search = *val;
        }
        if (auto val = config.get<bool>("research_sweep_enable_grid_search")) {
            plasticity_params.research.sweep_enable_grid_search = *val;
        }
        if (auto val = config.get<bool>("research_sweep_enable_hill_climbing")) {
            plasticity_params.research.sweep_enable_hill_climbing = *val;
        }
        if (auto val = config.get<bool>("research_sweep_enable_genetic_algorithm")) {
            plasticity_params.research.sweep_enable_genetic_algorithm = *val;
        }
        if (auto val = config.get<bool>("research_ml_enable_neural_network")) {
            plasticity_params.research.ml_enable_neural_network = *val;
        }
        if (auto val = config.get<bool>("research_ml_enable_reinforcement_learning")) {
            plasticity_params.research.ml_enable_reinforcement_learning = *val;
        }
        if (auto val = config.get<bool>("research_ml_enable_supervised_learning")) {
            plasticity_params.research.ml_enable_supervised_learning = *val;
        }
        if (auto val = config.get<bool>("research_ml_enable_unsupervised_learning")) {
            plasticity_params.research.ml_enable_unsupervised_learning = *val;
        }
        if (auto val = config.get<bool>("research_log_enable_real_time")) {
            plasticity_params.research.log_enable_real_time = *val;
        }
        if (auto val = config.get<bool>("research_log_enable_raw_spikes")) {
            plasticity_params.research.log_enable_raw_spikes = *val;
        }
        if (auto val = config.get<bool>("research_log_enable_network_state")) {
            plasticity_params.research.log_enable_network_state = *val;
        }
        if (auto val = config.get<bool>("research_log_enable_memory_traces")) {
            plasticity_params.research.log_enable_memory_traces = *val;
        }
        if (auto val = config.get<bool>("research_log_enable_plasticity_events")) {
            plasticity_params.research.log_enable_plasticity_events = *val;
        }
        if (auto val = config.get<double>("research_validate_p_value_threshold")) {
            plasticity_params.research.validate_p_value_threshold = static_cast<float>(*val);
        }
        if (auto val = config.get<double>("research_validate_confidence_interval")) {
            plasticity_params.research.validate_confidence_interval = static_cast<float>(*val);
        }
        if (auto val = config.get<bool>("research_validate_power_analysis")) {
            plasticity_params.research.validate_power_analysis = *val;
        }
        if (auto val = config.get<bool>("research_validate_effect_size")) {
            plasticity_params.research.validate_effect_size = *val;
        }
        if (auto val = config.get<bool>("api_enable_batch_operations")) {
            plasticity_params.api.enable_batch_operations = *val;
        }
        if (auto val = config.get<bool>("api_enable_streaming_apis")) {
            plasticity_params.api.enable_streaming_apis = *val;
        }
        if (auto val = config.get<bool>("api_enable_high_level_apis")) {
            plasticity_params.api.enable_high_level_apis = *val;
        }
        if (auto val = config.get<bool>("api_enable_low_level_apis")) {
            plasticity_params.api.enable_low_level_apis = *val;
        }
        if (auto val = config.get<bool>("api_stream_enable_spike_streaming")) {
            plasticity_params.api.stream_enable_spike_streaming = *val;
        }
        if (auto val = config.get<bool>("api_stream_enable_state_streaming")) {
            plasticity_params.api.stream_enable_state_streaming = *val;
        }
        if (auto val = config.get<bool>("api_stream_enable_metric_streaming")) {
            plasticity_params.api.stream_enable_metric_streaming = *val;
        }
        if (auto val = config.get<bool>("api_stream_enable_plot_streaming")) {
            plasticity_params.api.stream_enable_plot_streaming = *val;
        }
        if (auto val = config.get<bool>("api_enable_performance_monitoring")) {
            plasticity_params.api.enable_performance_monitoring = *val;
        }
        if (auto val = config.get<bool>("api_enable_memory_profiling")) {
            plasticity_params.api.enable_memory_profiling = *val;
        }
        if (auto val = config.get<bool>("api_enable_computation_profiling")) {
            plasticity_params.api.enable_computation_profiling = *val;
        }
        if (auto val = config.get<bool>("api_enable_network_io_profiling")) {
            plasticity_params.api.enable_network_io_profiling = *val;
        }
        if (auto val = config.get<bool>("api_enable_scenario_apis")) {
            plasticity_params.api.enable_scenario_apis = *val;
        }
        if (auto val = config.get<bool>("api_enable_research_apis")) {
            plasticity_params.api.enable_research_apis = *val;
        }
        if (auto val = config.get<bool>("api_enable_monitoring_apis")) {
            plasticity_params.api.enable_monitoring_apis = *val;
        }
        
        validate();
    }
    
    // Validate all parameters
    bool validate() const {
        return plasticity_params.validate();
    }
    
    // Get configuration as string
    std::string toString() const {
        std::ostringstream oss;
        oss << "=== Advanced NLM Configuration ===\n";
        oss << "Plasticity Parameters:\n";
        oss << "  STDP: ltp_weight=" << plasticity_params.stdp.ltp_weight
            << ", ltd_weight=" << plasticity_params.stdp.ltd_weight
            << ", tau=" << plasticity_params.stdp.tau << "\n";
        oss << "  Hebbian: learning_rate=" << plasticity_params.hebbian.learning_rate
            << ", saturation_threshold=" << plasticity_params.hebbian.saturation_threshold << "\n";
        oss << "  Structural: synaptogenesis_rate=" << plasticity_params.structural.synaptogenesis_rate
            << ", pruning_rate=" << plasticity_params.structural.pruning_rate << "\n";
        oss << "Connectivity: pattern=" << static_cast<int>(plasticity_params.connectivity_pattern)
            << ", strength=" << plasticity_params.connectivity_strength << "\n";
        oss << "Development: critical_period_start=" << plasticity_params.development.critical_period_start
            << ", critical_period_end=" << plasticity_params.development.critical_period_end << "\n";
        oss << "Simulation: enable_multi_agent=" << plasticity_params.simulation.enable_multi_agent
            << ", enable_vectorization=" << plasticity_params.simulation.enable_vectorization
            << ", threads=" << plasticity_params.simulation.simulation_threads << "\n";
        oss << "Analysis: enable_network_analysis=" << plasticity_params.analysis.enable_network_analysis
            << ", calculate_clustering=" << plasticity_params.analysis.calculate_clustering_coefficient << "\n";
        oss << "Research: enable_experiments_sweeps=" << plasticity_params.research.enable_experiment_parameter_sweeps
            << ", enable_ml=" << plasticity_params.research.enable_machine_learning_integration << "\n";
        oss << "API: enable_batch_ops=" << plasticity_params.api.enable_batch_operations
            << ", enable_streaming=" << plasticity_params.api.enable_streaming_apis << "\n";
        
        return oss.str();
    }
    
    // Convert to standard Config
    Config toStandardConfig() const {
        Config config;
        plasticity_params.applyToConfig(config);
        return config;
    }
    
private:
    AdvancedPlasticityParams plasticity_params;
}; // End AdvancedConfig

// Multi-agent related types
struct AgentInfo {
    uint64_t agent_id;
    std::string agent_type;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Environment> environment;
    float reward;
    uint64_t steps_completed;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_update;
    
    AgentInfo(uint64_t id, const std::string& type) : agent_id(id), agent_type(type), 
        reward(0.0f), steps_completed(0) {}
}; // End AgentInfo

struct MultiAgentConfig {
    size_t num_agents;
    bool enable_agent_communication;      // Inter-agent communication
    bool enable_agent_cooperation;       // Cooperative behavior
    bool enable_agent_competition;       // Competitive dynamics
    float inter_agent_communication_strength; // Strength of inter-agent connections
    std::vector<std::string> agent_types; // Types of agents
    std::map<std::string, std::string> agent_parameters; // Per-agent parameters
    
    MultiAgentConfig() : num_agents(1), enable_agent_communication(true),
        enable_agent_cooperation(false), enable_agent_competition(true),
        inter_agent_communication_strength(0.1f) {}
}; // End MultiAgentConfig

// Custom world model interface
class CustomWorldModel {
public:
    virtual ~CustomWorldModel() = default;
    
    // Virtual world step
    virtual void step(float dt, const std::vector<float>& agent_inputs) = 0;
    
    // Get observations for all agents
    virtual std::vector<std::vector<float>> getObservations() const = 0;
    
    // Get rewards for all agents
    virtual std::vector<float> getRewards() const = 0;
    
    // Check if world is done
    virtual bool isDone() const = 0;
    
    // Reset world state
    virtual void reset() = 0;
    
    // Get world metadata
    virtual std::string getWorldInfo() const = 0;
    
    // Custom sensory processing
    virtual std::vector<float> processSensoryInput(const std::vector<float>& raw_input) const {
        return raw_input; // Default: pass through
    }
}; // End CustomWorldModel

// Heterogeneous neuron population configuration
struct PopulationConfig {
    std::string population_name;
    NeuronType neuron_type;
    size_t neuron_count;
    float excitation_inhibition_ratio; // E/I ratio
    float adaptation_strength;          // Spike frequency adaptation
    float threshold_modulation;         // Dynamic threshold adjustment
    float refractory_period_modulation; // Refractory period variation
    float plasticity_modulation;        // Plasticity strength modifier
    float noise_level;                   // Intrinsic noise level
    bool enable_stochasticity;           // Enable stochastic processes
    
    PopulationConfig() : neuron_type(NeuronType::Internal), neuron_count(100),
        excitation_inhibition_ratio(1.0f), adaptation_strength(0.01f),
        threshold_modulation(1.0f), refractory_period_modulation(1.0f),
        plasticity_modulation(1.0f), noise_level(0.1f), enable_stochasticity(false) {}
}; // End PopulationConfig

// Network analysis metrics
struct NetworkMetrics {
    // Graph theory metrics
    float clustering_coefficient;
    float characteristic_path_length;
    float small_worldness;
    float modularity;
    float assortativity;
    float efficiency;
    float betweenness_centrality;
    float eigenvector_centrality;
    
    // Degree distribution metrics
    std::vector<size_t> degree_distribution;
    float avg_degree;
    float max_degree;
    float degree_skewness;
    
    // Functional metrics
    float synchrony_measure;
    float information_flow;
    float mutual_information;
    float entropy;
    
    // Topological metrics
    float local_clustering_variance;
    float global_efficiency;
    float participation_coefficient;
    
    // Performance metrics
    float firing_rate_mean;
    float firing_rate_variance;
    float burst_index;
    float adaptation_index;
    
    // Development metrics
    float developmental_plasticity;
    float structural_maturation;
    float memory_capacity_utilization;
    
    NetworkMetrics() : clustering_coefficient(0.0f), characteristic_path_length(0.0f),
        small_worldness(1.0f), modularity(0.0f), assortativity(0.0f), efficiency(0.0f),
        betweenness_centrality(0.0f), eigenvector_centrality(0.0f), avg_degree(0.0f),
        max_degree(0.0f), degree_skewness(0.0f), synchrony_measure(0.0f),
        information_flow(0.0f), mutual_information(0.0f), entropy(0.0f),
        local_clustering_variance(0.0f), global_efficiency(0.0f), participation_coefficient(0.0f),
        firing_rate_mean(0.0f), firing_rate_variance(0.0f), burst_index(0.0f),
        adaptation_index(0.0f), developmental_plasticity(0.0f),
        structural_maturation(0.0f), memory_capacity_utilization(0.0f) {}
}; // End NetworkMetrics

// Advanced data export formats
struct ExportedData {
    std::string filename;
    std::string format;
    std::string timestamp;
    std::chrono::duration<double> duration;
    size_t data_size_bytes;
    bool compression_applied;
    std::vector<std::string> metadata;
    
    ExportedData() : format("auto"), compression_applied(false) {}
}; // End ExportedData

// Performance profiling data
struct PerformanceMetrics {
    // CPU performance
    float cpu_usage_percent;
    float cpu_time_seconds;
    size_t cpu_instructions;
    
    // Memory performance
    size_t memory_used_bytes;
    size_t memory_peak_bytes;
    size_t memory_allocation_count;
    float memory_bandwidth_mb_s;
    
    // Network performance (if applicable)
    float network_bandwidth_mbps;
    size_t network_packets_sent;
    size_t network_packets_received;
    
    // Simulation performance
    float simulation_speed_factor;
    float time_step_duration_ms;
    size_t steps_per_second;
    size_t total_spikes_processed;
    size_t total_connections_activated;
    
    // I/O performance
    float file_write_speed_mb_s;
    float file_read_speed_mb_s;
    float checkpoint_save_speed_mb_s;
    
    PerformanceMetrics() : cpu_usage_percent(0.0f), cpu_time_seconds(0.0f),
        cpu_instructions(0), memory_used_bytes(0), memory_peak_bytes(0),
        memory_allocation_count(0), memory_bandwidth_mb_s(0.0f),
        network_bandwidth_mbps(0.0f), network_packets_sent(0), network_packets_received(0),
        simulation_speed_factor(1.0f), time_step_duration_ms(1.0f),
        steps_per_second(0), total_spikes_processed(0), total_connections_activated(0),
        file_write_speed_mb_s(0.0f), file_read_speed_mb_s(0.0f),
        checkpoint_save_speed_mb_s(0.0f) {}
}; // End PerformanceMetrics

// Experiment sweep configuration
struct ParameterSweep {
    std::string sweep_name;
    std::vector<std::string> parameter_names;      // Parameters to sweep
    std::vector<std::vector<float>> parameter_ranges; // Range for each parameter
    std::string sweep_method;                     // random, grid, hill_climbing, genetic
    float tolerance;                              // Convergence tolerance
    size_t max_iterations;                        // Maximum iterations
    bool enable_parallel_sweeps;                  // Run multiple sweeps in parallel
    std::map<std::string, float> initial_values;  // Starting parameter values
    
    ParameterSweep() : sweep_method("grid"), tolerance(0.01f), max_iterations(1000),
        enable_parallel_sweeps(false) {}
}; // End ParameterSweep

// Streaming data buffer
class StreamingBuffer {
public:
    struct StreamData {
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
        std::string stream_type; // "spike", "state", "metrics", "plot"
        std::vector<float> data;
        size_t stream_id;
        std::map<std::string, std::string> metadata;
        
        StreamData(const std::string& type, const std::vector<float>& d, size_t id)
            : stream_type(type), data(d), stream_id(id) {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };
    
    StreamingBuffer(size_t max_buffer_size = 10000) 
        : max_buffer_size(max_buffer_size) {}
    
    // Add data to stream
    void addData(const std::string& stream_type, const std::vector<float>& data, size_t stream_id = 0) {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        
        StreamData stream_data(stream_type, data, stream_id);
        stream_buffer.push_back(stream_data);
        
        // Trim buffer if too large
        while (stream_buffer.size() > max_buffer_size) {
            stream_buffer.pop_front();
        }
    }
    
    // Get data from a specific stream
    std::vector<StreamData> getStreamData(size_t stream_id) const {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        std::vector<StreamData> result;
        for (const auto& data : stream_buffer) {
            if (data.stream_id == stream_id) {
                result.push_back(data);
            }
        }
        return result;
    }
    
    // Get all data
    std::deque<StreamData> getAllData() const {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        return stream_buffer;
    }
    
    // Clear buffer
    void clear() {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        stream_buffer.clear();
    }
    
    // Get buffer size
    size_t size() const {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        return stream_buffer.size();
    }
    
private:
    std::deque<StreamData> stream_buffer;
    size_t max_buffer_size;
    mutable std::mutex buffer_mutex;
}; // End StreamingBuffer

// High-level API classes
class NLMApi {
public:
    NLMApi() : advanced_config(), streaming_buffer(), agent_counter(0) {}
    
    // Configure advanced settings
    void configureAdvanced(const AdvancedConfig& config) {
        advanced_config = config;
        // Apply to brain if available
        if (brain) {
            applyAdvancedConfig(*brain);
        }
    }
    
    // Run multi-agent simulation
    std::vector<AgentInfo> runMultiAgentSimulation(const MultiAgentConfig& config,
        std::vector<std::shared_ptr<Environment>> environments,
        const AdvancedConfig& agent_config) {
        
        std::vector<AgentInfo> agents;
        
        // Create agents
        for (size_t i = 0; i < config.num_agents; ++i) {
            auto agent_brain = std::make_shared<Brain>(advanced_config.getConfigAsStandard());
            if (agent_brain->initialize()) {
                agents.emplace_back(agent_counter++, config.agent_types[i % config.agent_types.size()], agent_brain, environments[i], 0.0f, 0);
            }
        }
        
        // Run simulation
        bool simulation_running = true;
        uint64_t step = 0;
        
        while (simulation_running && step < 10000) { // Safety limit
            // Update each agent
            for (auto& agent : agents) {
                // Process sensory input
                auto observation = agent.environment->reset(); // Reset each step for simplicity
                agent.brain->receiveSensoryInput(*observation);
                
                // Brain step
                agent.brain->step(step, step * 0.001f);
                
                // Produce action
                auto action = agent.brain->produceAction();
                
                // Apply action to environment
                auto action_result = agent.environment->step(*action);
                
                // Update agent
                agent.steps_completed++;
                agent.last_update = std::chrono::high_resolution_clock::now();
                
                // Record metrics
                collectMetrics(agent);
            }
            
            step++;
            
            // Check if all environments are done
            simulation_running = false;
            for (const auto& agent : agents) {
                if (!agent.environment->isDone()) {
                    simulation_running = true;
                    break;
                }
            }
        }
        
        return agents;
    }
    
    // Get network metrics from a brain
    NetworkMetrics getNetworkMetrics(std::shared_ptr<Brain> brain) {
        NetworkMetrics metrics;
        
        if (!brain) return metrics;
        
        // Calculate basic network properties
        size_t total_neurons = brain->getTotalNeuronCount();
        size_t total_synapses = brain->getTotalSynapseCount();
        
        if (total_neurons > 0) {
            metrics.firing_rate_mean = brain->getAverageFiringRate();
            metrics.avg_degree = (total_synapses * 2) / total_neurons; // Approximate
        }
        
        // Calculate clustering coefficient (placeholder)
        metrics.clustering_coefficient = 0.5f; // Should be calculated from actual network structure
        
        // Calculate path length (placeholder)
        metrics.characteristic_path_length = 3.0f; // Should be calculated from network topology
        
        // Calculate small-worldness (placeholder)
        metrics.small_worldness = 1.2f; // Should be calculated from network properties
        
        // Calculate other metrics...
        
        return metrics;
    }
    
    // Export data in various formats
    ExportedData exportData(const std::string& filename, const std::string& format,
        const std::vector<float>& data, const std::map<std::string, std::string>& metadata) {
        
        ExportedData exported;
        exported.filename = filename;
        exported.format = format;
        exported.timestamp = getCurrentTimestamp();
        exported.data_size_bytes = data.size() * sizeof(float);
        exported.compression_applied = false;
        exported.metadata = std::vector<std::string>();
        
        for (const auto& pair : metadata) {
            exported.metadata.push_back(pair.first + "=" + pair.second);
        }
        
        // TODO: Implement actual export logic based on format
        // For now, just record that export was attempted
        
        return exported;
    }
    
    // Get performance metrics
    PerformanceMetrics getPerformanceMetrics() {
        PerformanceMetrics perf;
        
        // TODO: Implement actual performance monitoring
        // For now, return placeholder values
        
        return perf;
    }
    
    // Apply advanced configuration to brain
    void applyAdvancedConfig(Brain& brain) {
        // This would apply the advanced configuration to the brain's internal systems
        // For now, it's a placeholder
    }
    
    // Get current advanced configuration
    const AdvancedConfig& getAdvancedConfig() const { return advanced_config; }
    
    // Set streaming buffer size
    void setStreamingBufferSize(size_t size) {
        streaming_buffer = StreamingBuffer(size);
    }
    
    // Add data to streaming buffer
    void streamData(const std::string& stream_type, const std::vector<float>& data, size_t stream_id = 0) {
        streaming_buffer.addData(stream_type, data, stream_id);
    }
    
    // Get streaming buffer data
    std::vector<typename StreamingBuffer::StreamData> getStreamData(size_t stream_id) {
        return streaming_buffer.getStreamData(stream_id);
    }
    
private:
    AdvancedConfig advanced_config;
    StreamingBuffer streaming_buffer;
    uint64_t agent_counter;
    
    // Helper functions
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void collectMetrics(AgentInfo& agent) {
        // Placeholder for metric collection
    }
}; // End NLMApi

// High-level research API
class ResearchApi {
public:
    ResearchApi(std::shared_ptr<NLMApi> nlm_api) : nlm_api(nlm_api) {}
    
    // Run parameter sweep
    std::vector<std::map<std::string, float>> runParameterSweep(const ParameterSweep& sweep) {
        std::vector<std::map<std::string, float>> results;
        
        // TODO: Implement parameter sweep logic
        // This would run simulations with different parameter combinations
        
        return results;
    }
    
    // Machine learning integration
    bool integrateMachineLearning(const std::string& model_type, 
        const std::map<std::string, std::vector<float>>& training_data) {
        // TODO: Implement ML integration
        return false;
    }
    
    // Statistical validation
    bool validateStatisticalSignificance(const std::vector<float>& data, 
        float p_value_threshold = 0.05f) {
        // TODO: Implement statistical validation
        return false;
    }
    
private:
    std::shared_ptr<NLMApi> nlm_api;
}; // End ResearchApi

// Low-level performance API
class LowLevelApi {
public:
    // Direct memory access (placeholder)
    bool accessMemoryDirect(size_t address, void* buffer, size_t size) {
        // TODO: Implement direct memory access
        return false;
    }
    
    // Performance optimization hints
    bool optimizePerformance(const std::string& optimization_type, 
        const std::map<std::string, float>& parameters) {
        // TODO: Implement performance optimizations
        return false;
    }
    
    // Custom neuron/synapse operations
    bool customPlasticityUpdate(Synapse* synapse, float weight_change, 
        float neuromodulator_level = 1.0f) {
        if (!synapse) return false;
        
        // Apply custom plasticity update
        float new_weight = synapse->getWeight() + weight_change * neuromodulator_level;
        if (new_weight < 0.0f || new_weight > 1.0f) return false;
        
        synapse->setWeight(new_weight);
        return true;
    }
}; // End LowLevelApi

} // namespace nlm