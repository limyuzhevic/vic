#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <map>
#include <any>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// Typed configuration parameters - replaces variant-based approach
struct ConfigParameters {
    // Simulation parameters
    uint64_t random_seed = 42;
    double simulation_timestep = 0.001;
    uint64_t neuron_count = 1000;
    uint64_t region_count = 2;
    double connection_probability = 0.1;
    double plasticity_learning_rate = 0.01;
    double stdp_ltp_weight = 0.01;
    double stdp_ltd_weight = 0.012;
    double stdp_time_constant = 20.0;
    double development_synaptogenesis_rate = 0.001;
    double development_pruning_rate = 0.0001;
    double dopamine_baseline = 0.1;
    double reward_discount_factor = 0.99;
    
    // Environment parameters
    std::string environment_name = "GridWorld";
    int environment_width = 10;
    int environment_height = 10;
    
    // Logging parameters
    std::string log_level = "INFO";
    bool log_to_file = false;
    std::string log_filename = "nlm.log";
    
    // Simulation parameters
    uint64_t max_simulation_steps = 10000;
    double simulation_time_limit = 0.0;
    
    // Visualization parameters
    bool visualization_enabled = false;
    double visualization_update_rate = 30.0;
};

// Configuration parameter types for validation and metadata
enum class ConfigParamType {
    Integer,
    Float,
    String,
    Boolean,
    VectorInt,
    VectorFloat,
    VectorString,
    Invalid
};

// Configuration metadata for parameters
struct ConfigParamMetadata {
    std::string name;
    ConfigParamType type;
    std::any default_value;
    std::string description;
    std::string category;
    double min_value = std::numeric_limits<double>::lowest();
    double max_value = std::numeric_limits<double>::max();
    bool required = false;
};

// Configuration class with typed parameter access
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from string
    bool loadFromString(const std::string& content);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> getOptional(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    template<typename T>
    void set(const std::string& key, const T& value, 
             const std::string& source = "Runtime");
    
    // Convenience methods for specific types
    void setRandomSeed(uint64_t seed) { set("random_seed", seed); }
    void setNeuronCount(uint64_t count) { set("neuron_count", count); }
    void setSimulationTimestep(double timestep) { set("simulation_timestep", timestep); }
    void setConnectionProbability(double prob) { set("connection_probability", prob); }
    void setDopamineBaseline(double baseline) { set("dopamine_baseline", baseline); }
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all parameters as ConfigParameters struct (convenience)
    ConfigParameters getParameters() const;
    
    // Set all parameters from ConfigParameters struct
    void setParameters(const ConfigParameters& params);
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate all parameters
    std::vector<std::string> validate() const;
    
    // Get parameter metadata
    static ConfigParamMetadata getParamMetadata(const std::string& key);
    
    // Get all parameter keys
    static std::vector<std::string> getAllParamKeys();
    
    // Check if key is a valid configuration parameter
    static bool isValidKey(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigParamType inferType(const std::string& valueStr);
    static bool validateValue(const std::string& key, const std::any& value);
    static std::any stringToAny(const std::string& key, const std::string& valueStr);
};

// Helper functions
inline bool isFloat(const std::string& str) {
    std::istringstream iss(str);
    float f;
    return (iss >> f) && iss.eof();
}

inline bool isInt(const std::string& str) {
    std::istringstream iss(str);
    int i;
    return (iss >> i) && iss.eof();
}

inline bool isBool(const std::string& str) {
    return str == "true" || str == "false" || str == "1" || str == "0";
}

inline bool isVector(const std::string& str) {
    return str.front() == '[' && str.back() == ']';
}

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm