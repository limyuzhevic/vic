// NLM Configuration Options Structure
// Provides unified interface for command-line and file-based configuration

#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Configuration options for all NLM tests and demos
struct NLMConfig {
    // Simulation parameters
    int random_seed = 42;
    double simulation_timestep = 0.001;
    int neuron_count = 500;
    int region_count = 1;
    double connection_probability = 0.1;
    
    // STDP parameters
    double stdp_ltp_weight = 0.01;
    double stdp_ltd_weight = 0.012;
    double stdp_tau = 20.0;
    
    // Structural plasticity parameters
    double synaptogenesis_rate = 0.0001;
    double pruning_rate = 0.00001;
    
    // Neuromodulation parameters
    double dopamine_baseline = 0.1;
    double novelty_decay = 0.99;
    double curiosity_sensitivity = 1.0;
    
    // Reward and learning
    double reward_discount_factor = 0.99;
    
    // Environment parameters
    int environment_width = 20;
    int environment_height = 20;
    int vision_width = 16;
    int vision_height = 16;
    
    // Test/experiment parameters
    int max_steps = 1000;
    int steps_per_episode = 200;
    int num_episodes = 5;
    
    // Phase-specific parameters
    // Phase 2 test parameters
    int phase2_test_steps = 50;
    double phase2_injection_current = 50.0;
    
    // Phase 3 demo parameters
    int phase3_vision_width = 16;
    int phase3_vision_height = 16;
    double phase3_step_multiplier = 10.0;
    
    // Phase 4 experiment parameters
    double phase4_planning_horizon = 5.0;
    double phase4_exploration_rate = 0.1;
    
    // Phase 6 experiment parameters
    int phase6_neuron_count = 500;
    int phase6_max_steps = 2000;
    bool phase6_enable_checkpointing = true;
    bool phase6_enable_replay = true;
    bool phase6_enable_development = true;
    
    // Logging and output
    std::string log_level = "INFO";
    bool log_to_file = false;
    std::string log_filename = "nlm.log";
    bool output_json = false;
    std::string output_filename = "";
    
    // Debug and optimization
    bool debug_mode = false;
    bool verbose_logging = false;
    int optimization_level = 2;
    
    // Game/play control
    bool run_all_tests = false;
    bool continue_on_error = false;
    
    // Validation and parsing
    bool validate() const;
    
    // Helper methods
    template<typename T>
    T get(const std::string& key, T default_value) const;
    
    template<typename T>
    void set(const std::string& key, T value);
    
    // Config file parsing
    static bool loadFromFile(NLMConfig& config, const std::string& filename);
    static bool loadFromCommandLine(NLMConfig& config, int argc, char* argv[]);
    static bool loadFromEnvironment(NLMConfig& config);
    
    // Help and display
    static void printUsage(const std::string& program_name);
    void printConfig() const;
    void printTestInfo() const;
};

// Implementation details
namespace config_internal {
    // Parse key=value format from command line
    bool parseKeyValue(const std::string& arg, std::string& key, std::string& value);
    
    // Convert string to value (template specialization)
    template<typename T>
    T stringToValue(const std::string& str);
    
    // Get environment variable
    std::string getEnvVar(const std::string& name);
    
    // Check if string is boolean
    bool stringToBool(const std::string& str, bool& result);
    
    // Check if string is numeric
    bool stringToNumber(const std::string& str, double& result);
}

} // namespace nlm
