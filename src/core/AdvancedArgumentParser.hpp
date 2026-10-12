#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

// Advanced CLI options structure
struct AdvancedCLIOptions {
    // Logging Control
    std::string log_level;
    std::string log_file;
    std::string log_format;
    
    // Simulation Control
    int simulation_steps;
    double simulation_timestep;
    double simulation_time;
    std::string simulation_stop_condition;
    
    // Experiment Control
    std::string experiment_name;
    int experiment_trial;
    uint64_t random_seed;
    
    // Output Control
    std::string output_format;
    std::string output_file;
    bool output_quiet;
    bool output_verbose;
    
    // Debugging & Analysis
    bool debug_mode;
    bool enable_profiling;
    bool run_benchmark;
    bool validate_only;
    bool dry_run;
    
    // Visualization & Monitoring
    bool enable_monitor;
    int monitor_interval;
    bool enable_neural_visualization;
    bool enable_memory_visualization;
    
    // Batch Processing
    bool batch_mode;
    std::string batch_config_file;
    int parallel_simulations;
    
    // Legacy options for backward compatibility
    std::string legacy_config_file;
    std::map<std::string, std::string> legacy_overrides;
    
    AdvancedCLIOptions() : 
        log_level("info"),
        log_file(""),
        log_format("simple"),
        simulation_steps(-1),
        simulation_timestep(0.001),
        simulation_time(0.0),
        simulation_stop_condition(""),
        experiment_name(""),
        experiment_trial(1),
        random_seed(42),
        output_format("text"),
        output_file(""),
        output_quiet(false),
        output_verbose(false),
        debug_mode(false),
        enable_profiling(false),
        run_benchmark(false),
        validate_only(false),
        dry_run(false),
        enable_monitor(false),
        monitor_interval(1000),
        enable_neural_visualization(false),
        enable_memory_visualization(false),
        batch_mode(false),
        parallel_simulations(1) {}
};

// Modern argument parser using simple approach
class AdvancedArgumentParser {
public:
    static bool parse(int argc, char** argv, AdvancedCLIOptions& options);
    static bool parse(int argc, char** argv, AdvancedCLIOptions& options, const std::string& configFile);
    static void printHelp(const std::string& programName);
    static bool validateOptions(const AdvancedCLIOptions& options);
    static std::string valueToString(const std::string& value);
    static std::string valueToString(double value);
    static std::string valueToString(int value);
    static std::string valueToString(bool value);
    static bool parseConfigFile(const std::string& filepath, std::map<std::string, std::string>& configMap);
};

} // namespace nlm
