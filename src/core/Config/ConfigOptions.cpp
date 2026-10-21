// Implementation of NLMConfig class

#include "core/Config/ConfigOptions.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

namespace nlm {
namespace config_internal {
    
    bool parseKeyValue(const string& arg, string& key, string& value) {
        size_t pos = arg.find('=');
        if (pos == string::npos) {
            return false;
        }
        key = arg.substr(0, pos);
        value = arg.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        return true;
    }
    
    template<typename T>
    T stringToValue(const string& str) {
        stringstream ss(str);
        T value;
        ss >> value;
        if (ss.fail()) {
            throw runtime_error("Failed to convert string to value: " + str);
        }
        return value;
    }
    
    string getEnvVar(const string& name) {
        const char* value = getenv(name.c_str());
        return value ? value : "";
    }
    
    bool stringToBool(const string& str, bool& result) {
        string lower = str;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            result = true;
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            result = false;
            return true;
        }
        return false;
    }
    
    bool stringToNumber(const string& str, double& result) {
        stringstream ss(str);
        ss >> result;
        return !ss.fail();
    }
    
    // Template specialization for string
    template<>
    string stringToValue<string>(const string& str) {
        return str;
    }
    
    // Template specialization for bool
    template<>
    bool stringToValue<bool>(const string& str) {
        bool result;
        if (!stringToBool(str, result)) {
            throw runtime_error("Invalid boolean value: " + str);
        }
        return result;
    }
    
    // Template specialization for double
    template<>
    double stringToValue<double>(const string& str) {
        return stringToValue<double>(str);
    }
    
    // Template specialization for int
    template<>
    int stringToValue<int>(const string& str) {
        return static_cast<int>(stringToValue<double>(str));
    }
    
} // namespace config_internal

// Validation implementation
bool NLMConfig::validate() const {
    vector<string> errors;
    
    if (neuron_count <= 0) errors.push_back("neuron_count must be positive");
    if (region_count <= 0) errors.push_back("region_count must be positive");
    if (connection_probability < 0.0 || connection_probability > 1.0) {
        errors.push_back("connection_probability must be between 0.0 and 1.0");
    }
    if (simulation_timestep <= 0.0) errors.push_back("simulation_timestep must be positive");
    if (stdp_ltp_weight < 0.0) errors.push_back("stdp_ltp_weight must be non-negative");
    if (stdp_ltd_weight < 0.0) errors.push_back("stdp_ltd_weight must be non-negative");
    if (stdp_tau <= 0.0) errors.push_back("stdp_tau must be positive");
    if (synaptogenesis_rate < 0.0) errors.push_back("synaptogenesis_rate must be non-negative");
    if (pruning_rate < 0.0) errors.push_back("pruning_rate must be non-negative");
    if (dopamine_baseline < 0.0) errors.push_back("dopamine_baseline must be non-negative");
    if (reward_discount_factor < 0.0 || reward_discount_factor > 1.0) {
        errors.push_back("reward_discount_factor must be between 0.0 and 1.0");
    }
    if (environment_width <= 0) errors.push_back("environment_width must be positive");
    if (environment_height <= 0) errors.push_back("environment_height must be positive");
    if (vision_width <= 0) errors.push_back("vision_width must be positive");
    if (vision_height <= 0) errors.push_back("vision_height must be positive");
    if (max_steps <= 0) errors.push_back("max_steps must be positive");
    if (steps_per_episode <= 0) errors.push_back("steps_per_episode must be positive");
    if (num_episodes <= 0) errors.push_back("num_episodes must be positive");
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    return true;
}

// Template get method implementation
template<typename T>
T NLMConfig::get(const string& key, T default_value) const {
    static unordered_map<string, T> default_map = {
        {"random_seed", static_cast<T>(random_seed)},
        {"simulation_timestep", static_cast<T>(simulation_timestep)},
        {"neuron_count", static_cast<T>(neuron_count)},
        {"region_count", static_cast<T>(region_count)},
        {"connection_probability", static_cast<T>(connection_probability)},
        {"stdp_ltp_weight", static_cast<T>(stdp_ltp_weight)},
        {"stdp_ltd_weight", static_cast<T>(stdp_ltd_weight)},
        {"stdp_tau", static_cast<T>(stdp_tau)},
        {"synaptogenesis_rate", static_cast<T>(synaptogenesis_rate)},
        {"pruning_rate", static_cast<T>(pruning_rate)},
        {"dopamine_baseline", static_cast<T>(dopamine_baseline)},
        {"novelty_decay", static_cast<T>(novelty_decay)},
        {"curiosity_sensitivity", static_cast<T>(curiosity_sensitivity)},
        {"reward_discount_factor", static_cast<T>(reward_discount_factor)},
        {"environment_width", static_cast<T>(environment_width)},
        {"environment_height", static_cast<T>(environment_height)},
        {"vision_width", static_cast<T>(vision_width)},
        {"vision_height", static_cast<T>(vision_height)},
        {"max_steps", static_cast<T>(max_steps)},
        {"steps_per_episode", static_cast<T>(steps_per_episode)},
        {"num_episodes", static_cast<T>(num_episodes)},
        {"phase2_test_steps", static_cast<T>(phase2_test_steps)},
        {"phase2_injection_current", static_cast<T>(phase2_injection_current)},
        {"phase3_vision_width", static_cast<T>(phase3_vision_width)},
        {"phase3_vision_height", static_cast<T>(phase3_vision_height)},
        {"phase4_planning_horizon", static_cast<T>(phase4_planning_horizon)},
        {"phase4_exploration_rate", static_cast<T>(phase4_exploration_rate)},
        {"phase6_neuron_count", static_cast<T>(phase6_neuron_count)},
        {"phase6_max_steps", static_cast<T>(phase6_max_steps)},
        {"phase6_enable_checkpointing", static_cast<T>(phase6_enable_checkpointing)},
        {"phase6_enable_replay", static_cast<T>(phase6_enable_replay)},
        {"phase6_enable_development", static_cast<T>(phase6_enable_development)},
        {"optimization_level", static_cast<T>(optimization_level)},
    };
    
    auto it = default_map.find(key);
    if (it != default_map.end()) {
        return it->second;
    }
    
    // If key not found in defaults and T is a type that can be default-initialized
    // (which covers all the types we need), return default_value
    return default_value;
}

// Template set method implementation
template<typename T>
void NLMConfig::set(const string& key, T value) {
    // In a real implementation, this would update the corresponding member variable
    // based on the key. For now, this is a placeholder.
    NLM_LOG_INFO("Setting config " + key + " = " + to_string(value));
}

// File loading implementation
bool NLMConfig::loadFromFile(NLMConfig& config, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_INFO("Config file not found: " + filename);
        return false;
    }
    
    string line;
    int line_num = 0;
    while (getline(file, line)) {
        line_num++;
        
        // Skip comments and empty lines
        size_t comment_pos = line.find('#');
        if (comment_pos != string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) continue;
        
        string key, value;
        if (!config_internal::parseKeyValue(line, key, value)) {
            NLM_LOG_ERROR("Invalid config line " + to_string(line_num) + ": " + line);
            return false;
        }
        
        try {
            // Try to parse as boolean
            bool bool_val;
            if (config_internal::stringToBool(value, bool_val)) {
                config.set<bool>(key, bool_val);
                continue;
            }
            
            // Try to parse as number
            double num_val;
            if (config_internal::stringToNumber(value, num_val)) {
                // Determine if it's an int or double based on the presence of decimal
                bool is_int = (value.find('.') == string::npos && value.find('e') == string::npos && value.find('E') == string::npos);
                if (is_int) {
                    config.set<int>(key, static_cast<int>(num_val));
                } else {
                    config.set<double>(key, num_val);
                }
                continue;
            }
            
            // Fall back to string
            config.set<string>(key, value);
            
        } catch (const exception& e) {
            NLM_LOG_ERROR("Error parsing config value for " + key + ": " + string(e.what()));
            return false;
        }
    }
    
    return true;
}

// Command line loading implementation
bool NLMConfig::loadFromCommandLine(NLMConfig& config, int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage("nlm");
            return false;
        }
        
        if (arg.find("--config=") == 0) {
            string config_file = arg.substr(8);
            if (!loadFromFile(config, config_file)) {
                NLM_LOG_ERROR("Failed to load config file from command line: " + config_file);
                return false;
            }
            continue;
        }
        
        if (arg == "--config" && i + 1 < argc) {
            string config_file = argv[++i];
            if (!loadFromFile(config, config_file)) {
                NLM_LOG_ERROR("Failed to load config file from command line: " + config_file);
                return false;
            }
            continue;
        }
        
        if (arg.find('=') != string::npos) {
            string key, value;
            if (config_internal::parseKeyValue(arg, key, value)) {
                try {
                    bool bool_val;
                    if (config_internal::stringToBool(value, bool_val)) {
                        config.set<bool>(key, bool_val);
                        continue;
                    }
                    
                    double num_val;
                    if (config_internal::stringToNumber(value, num_val)) {
                        bool is_int = (value.find('.') == string::npos && value.find('e') == string::npos && value.find('E') == string::npos);
                        if (is_int) {
                            config.set<int>(key, static_cast<int>(num_val));
                        } else {
                            config.set<double>(key, num_val);
                        }
                        continue;
                    }
                    
                    config.set<string>(key, value);
                    
                } catch (const exception& e) {
                    NLM_LOG_ERROR("Invalid command line argument: " + arg + " - " + string(e.what()));
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument: " + arg);
                return false;
            }
        } else if (arg.find("--") == 0) {
            // Boolean flag
            string flag_name = arg.substr(2);
            config.set<bool>(flag_name, true);
        }
    }
    
    return true;
}

// Environment variable loading implementation
bool NLMConfig::loadFromEnvironment(NLMConfig& config) {
    vector<string> env_vars = {
        "NLM_RANDOM_SEED",
        "NLM_SIMULATION_TIMESTEP",
        "NLM_NEURON_COUNT",
        "NLM_REGION_COUNT",
        "NLM_CONNECTION_PROBABILITY",
        "NLM_STDP_LTP_WEIGHT",
        "NLM_STDP_LTD_WEIGHT",
        "NLM_STDP_TAU",
        "NLM_SYNAPTOGENESIS_RATE",
        "NLM_PRUNING_RATE",
        "NLM_DOPAMINE_BASELINE",
        "NLM_REWARD_DISCOUNT_FACTOR",
        "NLM_ENVIRONMENT_WIDTH",
        "NLM_ENVIRONMENT_HEIGHT",
        "NLM_VISION_WIDTH",
        "NLM_VISION_HEIGHT",
        "NLM_DEBUG_MODE",
        "NLM_VERBOSE_LOGGING",
        "NLM_OUTPUT_JSON",
    };
    
    bool loaded = false;
    for (const auto& env_var : env_vars) {
        string value = config_internal::getEnvVar(env_var);
        if (!value.empty()) {
            string key = env_var.substr(4); // Remove NLM_ prefix
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            try {
                bool bool_val;
                if (config_internal::stringToBool(value, bool_val)) {
                    config.set<bool>(key, bool_val);
                    loaded = true;
                    continue;
                }
                
                double num_val;
                if (config_internal::stringToNumber(value, num_val)) {
                    bool is_int = (value.find('.') == string::npos && value.find('e') == string::npos && value.find('E') == string::npos);
                    if (is_int) {
                        config.set<int>(key, static_cast<int>(num_val));
                    } else {
                        config.set<double>(key, num_val);
                    }
                    loaded = true;
                    continue;
                }
                
                config.set<string>(key, value);
                loaded = true;
                
            } catch (const exception& e) {
                NLM_LOG_ERROR("Invalid environment variable " + env_var + ": " + value + " - " + string(e.what()));
            }
        }
    }
    
    return loaded;
}

// Usage printing implementation
void NLMConfig::printUsage(const string& program_name) {
    cout << "NLM - Neural Learning Machine" << endl;
    cout << "=======================" << endl << endl;
    
    cout << "Usage:" << endl;
    cout << "  " << program_name << " [options]" << endl << endl;
    
    cout << "Basic Options:" << endl;
    cout << "  --help, -h                    Show this help message" << endl;
    cout << "  --version                     Show version information" << endl << endl;
    
    cout << "Configuration:" << endl;
    cout << "  --config=file.cfg             Load configuration from file" << endl;
    cout << "  key=value                     Set configuration option" << endl << endl;
    
    cout << "Common Options:" << endl;
    cout << "  --neuron_count=N              Number of neurons (default: 500)" << endl;
    cout << "  --region_count=N              Number of regions (default: 1)" << endl;
    cout << "  --connection_probability=F    Connection probability (0.0-1.0, default: 0.1)" << endl;
    cout << "  --simulation_timestep=F       Simulation timestep in seconds (default: 0.001)" << endl;
    cout << "  --random_seed=N               Random seed for reproducibility (default: 42)" << endl << endl;
    
    cout << "Test/MODE Options:" << endl;
    cout << "  --phase2-test-steps=N         Number of steps for Phase 2 test (default: 50)" << endl;
    cout << "  --phase2-injection-current=F  Current injection for Phase 2 test (default: 50.0)" << endl;
    cout << "  --phase3-episodes=N           Number of episodes for Phase 3 demo (default: 5)" << endl;
    cout << "  --phase3-steps-per-episode=N  Steps per episode for Phase 3 demo (default: 200)" << endl;
    cout << "  --phase4-planning-horizon=F   Planning horizon for Phase 4 experiment (default: 5.0)" << endl;
    cout << "  --phase4-exploration-rate=F   Exploration rate for Phase 4 (default: 0.1)" << endl;
    cout << "  --phase6-neuron-count=N       Neuron count for Phase 6 demo (default: 500)" << endl;
    cout << "  --phase6-max-steps=N          Max steps for Phase 6 demo (default: 2000)" << endl << endl;
    
    cout << "Output Options:" << endl;
    cout << "  --output-json                 Output results in JSON format" << endl;
    cout << "  --output-file=FILE            Output file for results" << endl;
    cout << "  --log-level=LEVEL             Log level (DEBUG, INFO, WARN, ERROR)" << endl << endl;
    
    cout << "Debug Options:" << endl;
    cout << "  --debug-mode                  Enable debug mode" << endl;
    cout << "  --verbose-logging             Enable verbose logging" << endl;
    cout << "  --continue-on-error           Continue execution even if errors occur" << endl << endl;
    
    cout << "Environment Variables:" << endl;
    cout << "  NLM_                        All NLM_ environment variables override defaults" << endl;
    cout << "  NLM_RANDOM_SEED              " << (getEnvVar("NLM_RANDOM_SEED")) << endl;
    cout << "  NLM_NEURON_COUNT             " << (getEnvVar("NLM_NEURON_COUNT")) << endl;
    cout << "  ..." << endl << endl;
    
    cout << "Examples:" << endl;
    cout << "  " << program_name << " --neuron_count=1000 --region_count=2" << endl;
    cout << "  " << program_name << " --phase3-episodes=10 --phase3-steps-per-episode=500" << endl;
    cout << "  " << program_name << " --phase6-neuron-count=200 --phase6-max-steps=5000" << endl;
    cout << "  NLM_NEURON_COUNT=2000 " << program_name << " --phase6-neuron-count=200" << endl;
}

// Config printing implementation
void NLMConfig::printConfig() const {
    cout << "NLM Configuration:" << endl;
    cout << "=================" << endl;
    
    cout << "Simulation:" << endl;
    cout << "  random_seed: " << random_seed << endl;
    cout << "  simulation_timestep: " << fixed << setprecision(6) << simulation_timestep << "s" << endl;
    cout << "  neuron_count: " << neuron_count << endl;
    cout << "  region_count: " << region_count << endl;
    cout << "  connection_probability: " << setprecision(3) << connection_probability << endl;
    cout << endl;
    
    cout << "STDP:" << endl;
    cout << "  stdp_ltp_weight: " << setprecision(4) << stdp_ltp_weight << endl;
    cout << "  stdp_ltd_weight: " << setprecision(4) << stdp_ltd_weight << endl;
    cout << "  stdp_tau: " << setprecision(2) << stdp_tau << endl;
    cout << endl;
    
    cout << "Plasticity:" << endl;
    cout << "  synaptogenesis_rate: " << setprecision(6) << synaptogenesis_rate << endl;
    cout << "  pruning_rate: " << setprecision(6) << pruning_rate << endl;
    cout << endl;
    
    cout << "Neuromodulation:" << endl;
    cout << "  dopamine_baseline: " << setprecision(3) << dopamine_baseline << endl;
    cout << "  novelty_decay: " << setprecision(3) << novelty_decay << endl;
    cout << "  curiosity_sensitivity: " << setprecision(3) << curiosity_sensitivity << endl;
    cout << endl;
    
    cout << "Reward:" << endl;
    cout << "  reward_discount_factor: " << setprecision(3) << reward_discount_factor << endl;
    cout << endl;
    
    cout << "Environment:" << endl;
    cout << "  environment_width: " << environment_width << endl;
    cout << "  environment_height: " << environment_height << endl;
    cout << "  vision_width: " << vision_width << endl;
    cout << "  vision_height: " << vision_height << endl;
    cout << endl;
    
    cout << "Test Parameters:" << endl;
    cout << "  max_steps: " << max_steps << endl;
    cout << "  steps_per_episode: " << steps_per_episode << endl;
    cout << "  num_episodes: " << num_episodes << endl;
    cout << "  run_all_tests: " << (run_all_tests ? "true" : "false") << endl;
    cout << endl;
    
    cout << "Phase 6 Specific:" << endl;
    cout << "  phase6_neuron_count: " << phase6_neuron_count << endl;
    cout << "  phase6_max_steps: " << phase6_max_steps << endl;
    cout << "  phase6_enable_checkpointing: " << (phase6_enable_checkpointing ? "true" : "false") << endl;
    cout << "  phase6_enable_replay: " << (phase6_enable_replay ? "true" : "false") << endl;
    cout << "  phase6_enable_development: " << (phase6_enable_development ? "true" : "false") << endl;
    cout << endl;
    
    cout << "Output:" << endl;
    cout << "  output_json: " << (output_json ? "true" : "false") << endl;
    if (!output_filename.empty()) {
        cout << "  output_filename: " << output_filename << endl;
    }
    if (!log_filename.empty()) {
        cout << "  log_filename: " << log_filename << endl;
    }
    cout << "  log_level: " << log_level << endl;
    cout << "  verbose_logging: " << (verbose_logging ? "true" : "false") << endl;
    cout << endl;
    
    cout << "Debug:" << endl;
    cout << "  debug_mode: " << (debug_mode ? "true" : "false") << endl;
    cout << "  continue_on_error: " << (continue_on_error ? "true" : "false") << endl;
}

void NLMConfig::printTestInfo() const {
    cout << "=== NLM Test Configuration ===" << endl;
    cout << "Mode: " << (run_all_tests ? "FULL TEST SUITE" : "STANDARD TEST") << endl;
    cout << "Neurons: " << neuron_count << " regions: " << region_count << endl;
    cout << "Simulation: " << setprecision(6) << simulation_timestep << "s timestep" << endl;
    cout << "Learning: STDP(LTP=" << setprecision(4) << stdp_ltp_weight << ", LTD=" << stdp_ltd_weight << ")" << endl;
    cout << "Plasticity: synaptogenesis=" << setprecision(6) << synaptogenesis_rate << ", pruning=" << pruning_rate << endl;
    cout << "Neuromodulation: dopamine baseline=" << dopamine_baseline << endl;
    cout << "=======================" << endl << endl;
}

} // namespace nlm
