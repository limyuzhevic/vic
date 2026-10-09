#include "ConfigBuilder.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

ConfigBuilder::ConfigBuilder(std::shared_ptr<Config> config) : config_(config) {}

void ConfigBuilder::applyDefaultPhase2Configuration() {
    // Core simulation parameters
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    config_->set("max_synapses_per_neuron", static_cast<int64_t>(1000), ConfigSource::Default);
    config_->set("refractory_period", static_cast<int64_t>(10), ConfigSource::Default);
    
    // Membrane potential parameters
    config_->set("neuron_leak_conductance", 0.001f, ConfigSource::Default);
    config_->set("neuron_capacity", 50.0f, ConfigSource::Default);
    config_->set("neuron_threshold", 20.0f, ConfigSource::Default);
    
    // Synaptic plasticity parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    config_->set("stdp_lambda", 0.01f, ConfigSource::Default);
    config_->set("stdp_mu", 0.01f, ConfigSource::Default);
    
    // Hebbian plasticity parameters
    config_->set("hebbian_learning_rate", 0.001f, ConfigSource::Default);
    config_->set("hebbian_time_window", 50.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    config_->set("min_synaptic_weight", 0.0f, ConfigSource::Default);
    config_->set("max_synaptic_weight", 1.0f, ConfigSource::Default);
    
    // Development parameters
    config_->set("initial_development_stage", static_cast<int64_t>(0), ConfigSource::Default);
    config_->set("development_step_interval", static_cast<int64_t>(100), ConfigSource::Default);
    
    // Neural dynamics parameters
    config_->set("spike_delay_range", static_cast<int64_t>(5), ConfigSource::Default);
    config_->set("max_spike_frequency", static_cast<double>(1000.0), ConfigSource::Default);
    config_->set("min_spike_frequency", static_cast<double>(0.1), ConfigSource::Default);
}
    
    // Noise and stochasticity
    config_->set("min_spike_frequency", static_cast<double>(0.1), ConfigSource::Default);
    
    // Neural dynamics parameters
    config_->set("spike_delay_range", static_cast<int64_t>(5), ConfigSource::Default);
    config_->set("max_spike_frequency", static_cast<double>(1000.0), ConfigSource::Default);
    config_->set("min_spike_frequency", static_cast<double>(0.1), ConfigSource::Default);
    
    // Performance optimization configuration
    config_->set("enable_memory_pool", true, ConfigSource::Default);
    config_->set("enable_object_pooling", true, ConfigSource::Default);
    config_->set("enable_simd_vectorization", true, ConfigSource::Default);
    config_->set("enable_parallel_processing", true, ConfigSource::Default);
    config_->set("enable_event_queue", true, ConfigSource::Default);
    config_->set("enable_sparse_connectivity", true, ConfigSource::Default);
    config_->set("optimization_level", std::string("balanced"), ConfigSource::Default);
    config_->set("thread_count", static_cast<size_t>(1), ConfigSource::Default);
    config_->set("simd_width", static_cast<size_t>(8), ConfigSource::Default);
    config_->set("memory_alignment", static_cast<size_t>(64), ConfigSource::Default);
}

void ConfigBuilder::initializeParameterInfo(std::vector<ConfigParameterInfo>& parameters) {
    // Core simulation parameters
    parameters.emplace_back("random_seed", "int64_t", 
        "Random seed for reproducible simulations", PARAM_CATEGORY_CORE, 42);
    parameters.emplace_back("simulation_timestep", "double", 
        "Time step for simulation in seconds", PARAM_CATEGORY_CORE, 0.001);
    parameters.emplace_back("neuron_count", "int64_t", 
        "Total number of neurons in the brain", PARAM_CATEGORY_CORE, 500);
    parameters.emplace_back("region_count", "int64_t", 
        "Number of neural regions", PARAM_CATEGORY_CORE, 1);
    parameters.emplace_back("connection_probability", "float", 
        "Probability of synaptic connection between neurons", PARAM_CATEGORY_CORE, 0.15f);
    parameters.emplace_back("max_synapses_per_neuron", "int64_t", 
        "Maximum number of synapses per neuron", PARAM_CATEGORY_CORE, 1000);
    parameters.emplace_back("refractory_period", "int64_t", 
        "Refractory period in simulation steps", PARAM_CATEGORY_CORE, 10);
    
    // Membrane potential parameters
    parameters.emplace_back("neuron_leak_conductance", "float", 
        "Neuronal leak conductance (mS/cm²)", PARAM_CATEGORY_CORE, 0.001f);
    parameters.emplace_back("neuron_capacity", "float", 
        "Neuronal membrane capacity (pF)", PARAM_CATEGORY_CORE, 50.0f);
    parameters.emplace_back("neuron_threshold", "float", 
        "Neuronal firing threshold (mV)", PARAM_CATEGORY_CORE, 20.0f);
    
    // Synaptic plasticity parameters
    parameters.emplace_back("stdp_ltp_weight", "float", 
        "STDP long-term potentiation weight factor", PARAM_CATEGORY_SYNAPSES, 0.02f);
    parameters.emplace_back("stdp_ltd_weight", "float", 
        "STDP long-term depression weight factor", PARAM_CATEGORY_SYNAPSES, 0.015f);
    parameters.emplace_back("stdp_tau", "float", 
        "STDP time constant for trace decay", PARAM_CATEGORY_SYNAPSES, 20.0f);
    parameters.emplace_back("stdp_lambda", "float", 
        "STDP normalization factor", PARAM_CATEGORY_SYNAPSES, 0.01f);
    parameters.emplace_back("stdp_mu", "float", 
        "STDP weight normalization factor", PARAM_CATEGORY_SYNAPSES, 0.01f);
    
    // Hebbian plasticity parameters
    parameters.emplace_back("hebbian_learning_rate", "float", 
        "Hebbian learning rate constant", PARAM_CATEGORY_SYNAPSES, 0.001f);
    parameters.emplace_back("hebbian_time_window", "float", 
        "Hebbian time window for synaptic cooperation", PARAM_CATEGORY_SYNAPSES, 50.0f);
    
    // Structural plasticity parameters
    parameters.emplace_back("synaptogenesis_rate", "float", 
        "Rate of new synapse formation per simulation step", PARAM_CATEGORY_STRUCTUREAL, 0.0001f);
    parameters.emplace_back("pruning_rate", "float", 
        "Rate of synapse removal per simulation step", PARAM_CATEGORY_STRUCTUREAL, 0.00001f);
    parameters.emplace_back("min_synaptic_weight", "float", 
        "Minimum allowable synaptic weight", PARAM_CATEGORY_STRUCTUREAL, 0.0f);
    parameters.emplace_back("max_synaptic_weight", "float", 
        "Maximum allowable synaptic weight", PARAM_CATEGORY_STRUCTUREAL, 1.0f);
    
    // Development parameters
    parameters.emplace_back("initial_development_stage", "int64_t", 
        "Initial developmental stage (0-5)", PARAM_CATEGORY_STRUCTUREAL, 0);
    parameters.emplace_back("development_step_interval", "int64_t", 
        "Simulation steps between development updates", PARAM_CATEGORY_STRUCTUREAL, 100);
    
    parameters.emplace_back("max_spike_frequency", "double", 
        "Maximum firing rate (Hz)", PARAM_CATEGORY_SYNAPSES, 1000.0);
    parameters.emplace_back("min_spike_frequency", "double", 
        "Minimum firing rate (Hz)", PARAM_CATEGORY_SYNAPSES, 0.1);
    
    // Noise and stochasticity
    parameters.emplace_back("noise_level", "float", 
        "Noise level added to membrane potential", PARAM_CATEGORY_CORE, 0.0f);
    parameters.emplace_back("weight_jitter", "float", 
        "Random variation in initial synaptic weights", PARAM_CATEGORY_CORE, 0.01f);
    
    // Performance optimization parameters
    parameters.emplace_back("enable_memory_pool", "bool", 
        "Enable memory pool optimization for neurons and synapses", PARAM_CATEGORY_PERFORMANCE, true);
    parameters.emplace_back("enable_object_pooling", "bool", 
        "Enable object pooling for frequently allocated neural components", PARAM_CATEGORY_PERFORMANCE, true);
    parameters.emplace_back("enable_simd_vectorization", "bool", 
        "Enable SIMD vectorization for neural computation operations", PARAM_CATEGORY_PERFORMANCE, true);
    parameters.emplace_back("enable_parallel_processing", "bool", 
        "Enable parallel processing for neural population updates", PARAM_CATEGORY_PERFORMANCE, true);
    parameters.emplace_back("enable_event_queue", "bool", 
        "Enable event queue optimization for spike processing", PARAM_CATEGORY_PERFORMANCE, true);
    parameters.emplace_back("enable_sparse_connectivity", "bool", 
        "Enable sparse connectivity representation for synapses", PARAM_CATEGORY_PERFORMANCE, true);
    parameters.emplace_back("optimization_level", "string", 
        "Optimization level: aggressive, balanced, conservative", PARAM_CATEGORY_PERFORMANCE, std::string("balanced"));
    parameters.emplace_back("thread_count", "size_t", 
        "Number of threads for parallel processing (0 = auto-detect)", PARAM_CATEGORY_PERFORMANCE, static_cast<size_t>(0));
    parameters.emplace_back("simd_width", "size_t", 
        "SIMD vector width (AVX-512=16, AVX2=8, SSE=4, Scalar=1)", PARAM_CATEGORY_PERFORMANCE, static_cast<size_t>(8));
    parameters.emplace_back("memory_alignment", "size_t", 
        "Memory alignment for optimal SIMD access (bytes)", PARAM_CATEGORY_PERFORMANCE, static_cast<size_t>(64));
}

const std::vector<ConfigParameterInfo>& ConfigBuilder::getParameterInfo() {
    static std::vector<ConfigParameterInfo> parameters;
    if (parameters.empty()) {
        initializeParameterInfo(parameters);
    }
    return parameters;
}

bool ConfigBuilder::validate(const Config& config) const {
    bool valid = true;
    
    // Core validation
    auto neuronCountOpt = config.get<int64_t>("neuron_count");
    if (!neuronCountOpt || *neuronCountOpt <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive");
        valid = false;
    }
    
    auto timestepOpt = config.get<double>("simulation_timestep");
    if (!timestepOpt || *timestepOpt <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be positive");
        valid = false;
    }
    
    auto connProbOpt = config.get<float>("connection_probability");
    if (!connProbOpt || *connProbOpt < 0.0f || *connProbOpt > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: must be between 0.0 and 1.0");
        valid = false;
    }
    
    // Plasticity parameter validation
    auto ltpWeightOpt = config.get<float>("stdp_ltp_weight");
    if (!ltpWeightOpt || *ltpWeightOpt < 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltp_weight: must be non-negative");
        valid = false;
    }
    
    auto ltdWeightOpt = config.get<float>("stdp_ltd_weight");
    if (!ltdWeightOpt || *ltdWeightOpt < 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltd_weight: must be non-negative");
        valid = false;
    }
    
    auto tauOpt = config.get<float>("stdp_tau");
    if (!tauOpt || *tauOpt <= 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_tau: must be positive");
        valid = false;
    }
    
    // Structural plasticity validation
    auto minWeightOpt = config.get<float>("min_synaptic_weight");
    if (!minWeightOpt || *minWeightOpt < 0.0f) {
        NLM_LOG_ERROR("Invalid min_synaptic_weight: must be non-negative");
        valid = false;
    }
    
    auto maxWeightOpt = config.get<float>("max_synaptic_weight");
    if (!maxWeightOpt || *maxWeightOpt < 0.0f) {
        NLM_LOG_ERROR("Invalid max_synaptic_weight: must be positive");
        valid = false;
    }
    else if (minWeightOpt && maxWeightOpt && *minWeightOpt > *maxWeightOpt) {
        NLM_LOG_ERROR("Invalid weight range: min_synaptic_weight must be <= max_synaptic_weight");
        valid = false;
    }
    
    return valid;
}

void ConfigBuilder::logConfigurationSummary(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Summary ===");
    
    // Log core parameters
    NLM_LOG_INFO("Core Simulation Parameters:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config.getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config.getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config.getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("  max_synapses_per_neuron: " + std::to_string(config.getOr<int64_t>("max_synapses_per_neuron", 1000)));
    NLM_LOG_INFO("");
    
    // Log membrane potential parameters
    NLM_LOG_INFO("Membrane Potential Parameters:");
    NLM_LOG_INFO("  neuron_leak_conductance: " + std::to_string(config.getOr<float>("neuron_leak_conductance", 0.001f)));
    NLM_LOG_INFO("  neuron_capacity: " + std::to_string(config.getOr<float>("neuron_capacity", 50.0f)));
    NLM_LOG_INFO("  neuron_threshold: " + std::to_string(config.getOr<float>("neuron_threshold", 20.0f)));
    NLM_LOG_INFO("");
    
    // Log synaptic plasticity parameters
    NLM_LOG_INFO("Synaptic Plasticity Parameters:");
    NLM_LOG_INFO("  stdp_ltp_weight: " + std::to_string(config.getOr<float>("stdp_ltp_weight", 0.02f)));
    NLM_LOG_INFO("  stdp_ltd_weight: " + std::to_string(config.getOr<float>("stdp_ltd_weight", 0.015f)));
    NLM_LOG_INFO("  stdp_tau: " + std::to_string(config.getOr<float>("stdp_tau", 20.0f)));
    NLM_LOG_INFO("  stdp_lambda: " + std::to_string(config.getOr<float>("stdp_lambda", 0.01f)));
    NLM_LOG_INFO("  stdp_mu: " + std::to_string(config.getOr<float>("stdp_mu", 0.01f)));
    NLM_LOG_INFO("");
    
    // Log hebbian parameters
    NLM_LOG_INFO("Hebbian Plasticity Parameters:");
    NLM_LOG_INFO("  hebbian_learning_rate: " + std::to_string(config.getOr<float>("hebbian_learning_rate", 0.001f)));
    NLM_LOG_INFO("  hebbian_time_window: " + std::to_string(config.getOr<float>("hebbian_time_window", 50.0f)));
    NLM_LOG_INFO("");
    
    // Log structural plasticity parameters
    NLM_LOG_INFO("Structural Plasticity Parameters:");
    NLM_LOG_INFO("  synaptogenesis_rate: " + std::to_string(config.getOr<float>("synaptogenesis_rate", 0.0001f)));
    NLM_LOG_INFO("  pruning_rate: " + std::to_string(config.getOr<float>("pruning_rate", 0.00001f)));
    NLM_LOG_INFO("  min_synaptic_weight: " + std::to_string(config.getOr<float>("min_synaptic_weight", 0.0f)));
    NLM_LOG_INFO("  max_synaptic_weight: " + std::to_string(config.getOr<float>("max_synaptic_weight", 1.0f)));
    NLM_LOG_INFO("");
    
    // Log performance optimization settings
    NLM_LOG_INFO("Performance Optimization Settings:");
    NLM_LOG_INFO("  enable_memory_pool: " + std::to_string(config.getOr<bool>("enable_memory_pool", true)));
    NLM_LOG_INFO("  enable_object_pooling: " + std::to_string(config.getOr<bool>("enable_object_pooling", true)));
    NLM_LOG_INFO("  enable_simd_vectorization: " + std::to_string(config.getOr<bool>("enable_simd_vectorization", true)));
    NLM_LOG_INFO("  enable_parallel_processing: " + std::to_string(config.getOr<bool>("enable_parallel_processing", true)));
    NLM_LOG_INFO("  enable_event_queue: " + std::to_string(config.getOr<bool>("enable_event_queue", true)));
    NLM_LOG_INFO("  enable_sparse_connectivity: " + std::to_string(config.getOr<bool>("enable_sparse_connectivity", true)));
    NLM_LOG_INFO("  optimization_level: " + std::to_string(config.getOr<std::string>("optimization_level", "balanced")));
    NLM_LOG_INFO("  thread_count: " + std::to_string(config.getOr<size_t>("thread_count", 0)));
    NLM_LOG_INFO("  simd_width: " + std::to_string(config.getOr<size_t>("simd_width", 8)));
    NLM_LOG_INFO("  memory_alignment: " + std::to_string(config.getOr<size_t>("memory_alignment", 64)));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("=== End Configuration Summary ===");
}

std::vector<std::string> ConfigBuilder::getParametersByCategory(const std::string& category) {
    std::vector<std::string> params;
    const auto& allParams = getParameterInfo();
    
    for (const auto& param : allParams) {
        if (param.category == category) {
            params.push_back(param.key);
        }
    }
    
    return params;
}

} // namespace nlm