// NLM Configuration Examples
// Advanced configuration examples for users

#ifndef NLM_CONFIG_EXAMPLES_HPP
#define NLM_CONFIG_EXAMPLES_HPP

#include <string>
#include <iostream>
#include <fstream>

namespace nlm {

// Configuration file templates and examples
class ConfigurationExamples {
public:
    static void createINIExample(const std::string& filename = "nlm_config.ini");
    static void createJSONExample(const std::string& filename = "nlm_config.json");
    static void createYAMLExample(const std::string& filename = "nlm_config.yaml");
    static void createAdvancedINI(const std::string& filename = "nlm_advanced.ini");
    static void createResearchConfig(const std::string& filename = "nlm_research.cfg");
    static void createDevelopmentConfig(const std::string& filename = "nlm_dev.cfg");
    static void createPerformanceConfig(const std::string& filename = "nlm_perf.cfg");

    static void printConfigurationExamples();
    static void printCommandLineExamples();
    static void printResearchExamples();
    static void printDevelopmentExamples();
    static void printPerformanceExamples();

private:
    static void writeFile(const std::string& filename, const std::string& content);
    static std::string getTimestamp();
};

// Real-world configuration examples
inline void ConfigurationExamples::createINIExample(const std::string& filename) {
    std::string content = R"(
# NLM (熙然) Configuration File
# Neural Learning Machine Configuration Examples
# 
# This file provides configuration options for NLM Phase 2:
# - Real LIF neuron dynamics
# - Event-driven spike propagation with synaptic delays
# - STDP and Hebbian plasticity
# - Structural plasticity (synaptogenesis/pruning)
# 
# Format: key = value (comments start with #)

# Basic configuration
[general]
version = 2.0.0
author = NLM Team
creation_date = 2025-01-18

# Brain configuration
[neuron]
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42

# Neuron parameters
[neuron.params]
resting_potential = -70.0
threshold = -55.0
reset_potential = -70.0
leak_conductance = 0.3
refractory_period = 5.0

# Synaptic configuration
[synapse]
weight_range = [-1.0, 1.0]
delay_steps = 1
type = excitatory

# Plasticity configuration
[plasticity]
stdp_enabled = true
hebbian_enabled = true
structural_enabled = true
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
hebbian_learning_rate = 0.001

# Memory configuration
[memory]
working_memory_capacity = 100
episodic_max_episodes = 1000
associative_similarity_threshold = 0.8

# Neuromodulation configuration
[neuromodulation]
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true
prediction_error_enabled = true

# Development configuration
[development]
initial_plasticity = 1.0
critical_period_multiplier = 0.8
maturation_threshold = 60.0

# Simulation configuration
[simulation]
steps = 1000
time_unit = milliseconds
output_format = binary
auto_save = true
checkpoint_interval = 100

# Research configuration
[research]
record_brain_state = true
export_connections = true
analyze_topology = true
profile_performance = true

# Output configuration
[output]
log_level = info
console_output = true
file_output = true
log_file = nlm.log
""";
    writeFile(filename, content);
    std::cout << "INI configuration example created: " << filename << std::endl;
}

inline void ConfigurationExamples::createJSONExample(const std::string& filename) {
    std::string content = R"({
    "nlm_version": "2.0.0",
    "author": "NLM Team",
    "creation_date": "2025-01-18",
    
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "random_seed": 42
    },
    
    "neuron_params": {
        "resting_potential": -70.0,
        "threshold": -55.0,
        "reset_potential": -70.0,
        "leak_conductance": 0.3,
        "refractory_period": 5.0
    },
    
    "plasticity": {
        "stdp_enabled": true,
        "hebbian_enabled": true,
        "structural_enabled": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_time_constant": 20.0,
        "hebbian_learning_rate": 0.001
    },
    
    "memory": {
        "working_memory_capacity": 100,
        "episodic_max_episodes": 1000,
        "associative_similarity_threshold": 0.8
    },
    
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true,
        "prediction_error_enabled": true
    },
    
    "development": {
        "initial_plasticity": 1.0,
        "critical_period_multiplier": 0.8,
        "maturation_threshold": 60.0
    },
    
    "simulation": {
        "steps": 1000,
        "time_unit": "milliseconds",
        "output_format": "binary",
        "auto_save": true,
        "checkpoint_interval": 100
    },
    
    "research": {
        "record_brain_state": true,
        "export_connections": true,
        "analyze_topology": true,
        "profile_performance": true
    },
    
    "output": {
        "log_level": "info",
        "console_output": true,
        "file_output": true,
        "log_file": "nlm.log"
    }
}
""";
    writeFile(filename, content);
    std::cout << "JSON configuration example created: " << filename << std::endl;
}

inline void ConfigurationExamples::createAdvancedINI(const std::string& filename) {
    std::string content = R"(
# Advanced NLM Configuration
# High-performance configuration with research optimizations

[performance]
enable_multithreading = true
thread_count = 8
use_gpu_acceleration = false
cuda_device = 0

[precision]
neuron_precision = double
weight_precision = float
simulation_precision = single

[research]
enable_profiling = true
profile_interval = 1.0
memory_profiling = true
network_analysis = true

[export]
export_format = binary
export_compression = true
export_validate = true
export_metadata = true

[monitoring]
real_time_monitoring = true
monitoring_interval = 0.1
performance_alerts = true
health_check_interval = 60.0

[checkpoint]
auto_checkpoint = true
checkpoint_strategy = rolling
rolling_checkpoint_count = 50
checkpoint_compression = true

[debug]
debug_mode = true
verbose_logging = true
log_file = nlm_debug.log
max_log_size = 100MB
log_rotation = daily

[research.params]
snapshot_frequency = 100.0ns
experiment_id = exp_001
conditions = "baseline,learning,recovery"

# Override some basic settings
[neuron]
neuron_count = 5000
region_count = 4
connection_probability = 0.15
""";
    writeFile(filename, content);
    std::cout << "Advanced INI configuration example created: " << filename << std::endl;
}

inline void ConfigurationExamples::createResearchConfig(const std::string& filename) {
    std::string content = R"(
# Research Configuration for NLM
# Optimized for scientific experiments and reproducibility

[general]
experiment_name = "neural_dynamics_study"
experiment_id = "nlm_exp_2025_001"
author = "Research Team"
institution = "Neural Learning Lab"
contact_email = "research@nlm.ai"

[goals]
primary_objective = "study_plasticity_dynamics"
secondary_objectives = ["network_analysis", "behavior_correlation", "development_tracking"]

[parameters]
control_variables = [
    "neuron_count", "region_count", "connection_probability",
    "stdp_parameters", "hebbian_parameters"
]
experiment_variables = [
    "plasticity_strength", "simulation_duration", "input_patterns"
]

[control]
simulation_reproducibility = true
random_seed_fixed = true
random_seed_value = 12345
config_validation = true

[record]
brain_state_snapshots = true
neuron_traces = true
synaptic_weights = true
activation_patterns = true
network_metrics = true

[analysis]
connectivity_analysis = true
community_detection = true
hub_analysis = true
betweenness_centrality = true
network_dynamics = true

[export]
data_format = "parquet"
analysis_format = "json"
visualization_format = "pdf"
archive_results = true

[backup]
cloud_backup = true
backup_interval = 30
backup_retention_days = 365
""";
    writeFile(filename, content);
    std::cout << "Research configuration example created: " << filename << std::endl;
}

inline void ConfigurationExamples::createDevelopmentConfig(const std::string& filename) {
    std::string content = R"(
# Development Configuration for NLM
# Optimized for software development and testing

[build]
debug_build = true
optimization_level = 0
enable_assertions = true
enable_sanitizers = true
verbose_output = true

[tests]
unit_test_coverage = 0.90
integration_test_coverage = 0.85
performance_test_coverage = 0.80
fuzz_testing = true

[validation]
configuration_validation = true
consistency_checks = true
performance_benchmarks = true
memory_leak_detection = true

[plugins]
plugin_directory = "plugins"
auto_load_plugins = true
plugin_api_version = "2.0"

[extensions]
extension_system = true
extension_validation = true
custom_components = true

[debug]
debugger_attached = false
memory_profiler = true
profiler = true

[environment]
test_environment = true
staging_environment = false
production_environment = false
docker_container = true
container_image = "nlm:dev"

[tools]
code_coverage_tool = "gcov"
profiling_tool = "gprof"
memory_tool = "valgrind"
lint_tool = "clang-tidy"
format_tool = "clang-format"
""";
    writeFile(filename, content);
    std::cout << "Development configuration example created: " << filename << std::endl;
}

inline void ConfigurationExamples::createPerformanceConfig(const std::string& filename) {
    std::string content = R"(
# Performance Configuration for NLM
# Optimized for maximum computational efficiency

[parallelism]
distributed_processing = true
cluster_nodes = 8
threads_per_node = 16
communication_protocol = "mpi"

[memory_management]
dynamic_allocation = true
memory_pooling = true
memory_optimization_level = "aggressive"
garbage_collection = false  # Manual memory management

[caching]
enable_caching = true
cache_size = 1GB
cache_policy = "LRU"
result_caching = true
computation_caching = true

[hardware_acceleration]
gpu_acceleration = true
cuda_enabled = true
cuBLAS_enabled = true
cuRAND_enabled = true

[simulation_optimization]
vectorization = true
simd_instructions = true
loop_unrolling = true
prefetching = true
cache_blocking = true

[io_optimization]
async_io = true
compression = true
streaming = true
disk_buffering = true

[profiling]
performance_profiling = true
memory_profiling = true
bottleneck_detection = true
real_time_monitoring = true

[target]
target_architecture = "x86_64"
target_architecture = "AVX2"
target_architecture = "SSE4.2"

[benchmarks]
benchmark_frequency = "daily"
performance_targets = [
    "steps_per_second > 1000000",
    "memory_usage < 2GB",
    "latency < 1ms"
]
""";
    writeFile(filename, content);
    std::cout << "Performance configuration example created: " << filename << std::endl;
}

inline void ConfigurationExamples::printConfigurationExamples() {
    std::cout << "=== NLM Configuration Examples ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration files can be placed in:" << std::endl;
    std::cout << "  - ~/.config/nlm/nlm_config.ini" << std::endl;
    std::cout << "  - ./nlm_config.json" << std::endl;
    std::cout << "  - ./config/nlm.yaml" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic configuration file can be created with:" << std::endl;
    std::cout << "  nlm create-config --format ini --output nlm_config.ini" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced configuration can be created with:" << std::endl;
    std::cout << "  nlm create-config --format advanced --output nlm_advanced.ini" << std::endl;
}

inline void ConfigurationExamples::printCommandLineExamples() {
    std::cout << "=== NLM Command Line Examples ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic commands:" << std::endl;
    std::cout << "  nlm create --neurons 1000 --regions 2 --steps 100" << std::endl;
    std::cout << "  nlm run --test1 --test2 --test3" << std::endl;
    std::cout << "  nlm save --filename brain_state.nbl" << std::endl;
    std::cout << "  nlm load --filename brain_state.nbl" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced commands:" << std::endl;
    std::cout << "  nlm run --benchmark --steps 10000" << std::endl;
    std::cout << "  nlm export --format json --filename export.json" << std::endl;
    std::cout << "  nlm create-config --format ini --output my_config.ini" << std::endl;
    std::cout << "  nlm create-config --format advanced --output perf_config.ini" << std::endl;
    std::cout << std::endl;
    std::cout << "Research commands:" << std::endl;
    std::cout << "  nlm research save-snapshot --filename snapshot1.nbl" << std::endl;
    std::cout << "  nlm research compare-brains --file1 brain1.nbl --file2 brain2.nbl" << std::endl;
    std::cout << "  nlm research analyze-topology" << std::endl;
}

inline void ConfigurationExamples::printResearchExamples() {
    std::cout << "=== Research Configuration Examples ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Create research configuration:" << std::endl;
    std::cout << "  nlm create-config --format research --output nlm_research.cfg" << std::endl;
    std::cout << std::endl;
    std::cout << "Common research tasks:" << std::endl;
    std::cout << "  - Save brain snapshots for reproducibility" << std::endl;
    std::cout << "  - Compare different experimental conditions" << std::endl;
    std::cout << "  - Analyze network topology and connectivity" << std::endl;
    std::cout << "  - Profile performance and memory usage" << std::endl;
    std::cout << std::endl;
    std::cout << "Research output formats:" << std::endl;
    std::cout << "  - Binary format (.nbl): Efficient for large simulations" << std::endl;
    std::cout << "  - JSON format (.json): Human-readable, easy to parse" << std::endl;
    std::cout << "  - Parquet format: Optimized for data analysis" << std::endl;
}

inline void ConfigurationExamples::printDevelopmentExamples() {
    std::cout << "=== Development Configuration Examples ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Create development configuration:" << std::endl;
    std::cout << "  nlm create-config --format development --output nlm_dev.cfg" << std::endl;
    std::cout << std::endl;
    std::cout << "Development features:" << std::endl;
    std::cout << "  - Debug build with assertions and sanitizers" << std::endl;
    std::cout << "  - Unit and integration test coverage tracking" << std::endl;
    std::cout << "  - Plugin system for extensibility" << std::endl;
    std::cout << "  - Docker containerization for reproducible builds" << std::endl;
}

inline void ConfigurationExamples::printPerformanceExamples() {
    std::cout << "=== Performance Configuration Examples ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Create performance configuration:" << std::endl;
    std::cout << "  nlm create-config --format performance --output nlm_perf.cfg" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance optimization features:" << std::endl;
    std::cout << "  - Multi-threading and distributed processing" << std::endl;
    std::cout << "  - GPU acceleration with CUDA" << std::endl;
    std::cout << "  - Advanced memory management and caching" << std::endl;
    std::cout << "  - Simulation optimization (vectorization, prefetching)" << std::endl;
    std::cout << "  - I/O optimization (async, compression, streaming)" << std::endl;
}

inline void ConfigurationExamples::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    } else {
        std::cerr << "Error: Cannot create file " << filename << std::endl;
    }
}

inline std::string ConfigurationExamples::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

} // namespace nlm
