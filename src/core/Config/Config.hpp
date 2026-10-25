// Forward declarations to avoid circular dependencies
class RandomGenerator;

namespace nlm {

// Configuration value types - extended for better type safety
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    float  // Added for compatibility
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation result
enum class ValidationResult {
    Success,
    MissingKey,
    InvalidType,
    InvalidValue,
    CustomError
};

// Configuration description for better documentation
struct ConfigDescription {
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    bool required;
    std::string typeHint;  // For user documentation
    
    ConfigDescription(const std::string& k, const std::string& desc, 
                     ConfigSource src = ConfigSource::Default, 
                     bool req = false, const std::string& type = "")
        : key(k), description(desc), defaultSource(src), required(req), typeHint(type) {}
};

// Type-safe configuration getter with error reporting
class Config {
public:
    // Default constructor with essential parameters
    explicit Config(uint64_t randomSeed = 42,
                   double timestep = 0.001,
                   size_t neuronCount = 1000,
                   size_t regionCount = 1,
                   float connectionProbability = 0.1f);
    
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML/INI format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Type-safe getters with error reporting
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Safe getters with default values and type checking
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe setters with documentation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Convenience type-specific setters with built-in validation
    void setRandomSeed(uint64_t seed) { set("random_seed", seed, ConfigSource::Runtime, "Random seed for reproducible experiments"); }
    void setTimestep(double timestep) { set("simulation_timestep", timestep, ConfigSource::Runtime, "Simulation timestep in seconds"); }
    void setNeuronCount(size_t count) { set("neuron_count", static_cast<int64_t>(count), ConfigSource::Runtime, "Number of neurons in the brain"); }
    void setRegionCount(size_t count) { set("region_count", static_cast<int64_t>(count), ConfigSource::Runtime, "Number of brain regions"); }
    void setConnectionProbability(float prob) { set("connection_probability", prob, ConfigSource::Runtime, "Connection probability for random connectivity"); }
    
    // Neuron parameters
    void setRestingPotential(double potential) { set("resting_potential", potential, ConfigSource::Runtime, "Resting membrane potential (mV)"); }
    void setThreshold(double threshold) { set("threshold", threshold, ConfigSource::Runtime, "Firing threshold (mV)"); }
    void setLeakConductance(double conductance) { set("leak_conductance", conductance, ConfigSource::Runtime, "Membrane leak conductance (nS)"); }
    void setResetPotential(double potential) { set("reset_potential", potential, ConfigSource::Runtime, "Post-spike reset potential (mV)"); }
    
    // Plasticity parameters
    void setSTDPParams(float ltpWeight, float ltdWeight, float tau) {
        set("stdp_ltp_weight", ltpWeight, ConfigSource::Runtime, "STDP LTP weight factor");
        set("stdp_ltd_weight", ltdWeight, ConfigSource::Runtime, "STDP LTD weight factor");
        set("stdp_tau", tau, ConfigSource::Runtime, "STDP time constant");
    }
    
    // Development parameters
    void setDevelopmentParams(float synaptogenesisRate, float pruningRate) {
        set("synaptogenesis_rate", synaptogenesisRate, ConfigSource::Runtime, "Synaptogenesis rate during development");
        set("pruning_rate", pruningRate, ConfigSource::Runtime, "Pruning rate during development");
    }
    
    // Neuromodulation parameters
    void setNeuromodulationParams(float dopamineBaseline, float curiosityRate) {
        set("dopamine_baseline", dopamineBaseline, ConfigSource::Runtime, "Baseline dopamine level");
        set("curiosity_rate", curiosityRate, ConfigSource::Runtime, "Curiosity drive rate");
    }
    
    // Reward parameters
    void setRewardParams(float discountFactor, float explorationBonus) {
        set("reward_discount_factor", discountFactor, ConfigSource::Runtime, "Reward discount factor");
        set("exploration_bonus", explorationBonus, ConfigSource::Runtime, "Exploration bonus for novel actions");
    }
    
    // Environment parameters
    void setEnvironmentParams(const std::string& name, size_t width, size_t height) {
        set("environment_name", name, ConfigSource::Runtime, "Environment name");
        set("environment_width", static_cast<int64_t>(width), ConfigSource::Runtime, "Environment width");
        set("environment_height", static_cast<int64_t>(height), ConfigSource::Runtime, "Environment height");
    }
    
    // Logging parameters
    void setLoggingParams(bool enabled, const std::string& level, const std::string& filename) {
        set("log_level", level, ConfigSource::Runtime, "Logging level (Debug, Info, Warning, Error, Critical)");
        set("log_to_file", enabled, ConfigSource::Runtime, "Enable file logging");
        set("log_filename", filename, ConfigSource::Runtime, "Log file name");
    }
    
    // Check existence and type safety
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all configuration descriptions
    static std::vector<ConfigDescription> getDefaultConfigSchema();
    
    // Validate configuration
    ValidationResult validate() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration description for a key
    std::string getDescription(const std::string& key) const;
    
    // Get configuration source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key was overridden
    bool isOverridden(const std::string& key) const;
    
    // Reset to defaults
    void resetToDefaults();
    
    // Apply random seed to a random number generator
    void applyToRandomGenerator(RandomGenerator& rng) const;
    
    // Create a default configuration
    static std::shared_ptr<Config> createDefault();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void initializeDefaults();
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Type conversion utilities
    static bool isNumericType(const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value) const;
    
    // Validation helpers
    static bool validateType(const ConfigValue& value, const std::string& typeName) const;
    static bool validateRange(const ConfigValue& value, const std::string& key) const;
    
    // Parse different value types from strings
    static ConfigValue parseValue(const std::string& str, const std::string& key) const;
    
    // Build configuration schema
    static void buildConfigSchema(std::vector<ConfigDescription>& schema);
    
    // Friend declaration for template specialization
    template<typename T> friend class ConfigValueAccessor;
};

// Type-safe value accessor for compile-time type checking
template<typename T>
class ConfigValueAccessor {
public:
    static T get(const ConfigValue& value) {
        if constexpr (std::is_same_v<T, int>) {
            return std::get<int>(value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(value);
        } else if constexpr (std::is_same_v<T, float>) {
            return static_cast<float>(std::get<double>(value));
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(value);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return std::get<std::vector<int>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::get<std::vector<double>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::get<std::vector<std::string>>(value);
        } else {
            static_assert(!std::is_same_v<T, T>, "Unsupported type for ConfigValueAccessor");
            return T();
        }
    }
    
    static bool isValid(const ConfigValue& value) {
        if constexpr (std::is_same_v<T, int>) {
            return std::holds_alternative<int>(value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::holds_alternative<int64_t>(value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::holds_alternative<double>(value);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::holds_alternative<double>(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::holds_alternative<bool>(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::holds_alternative<std::string>(value);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return std::holds_alternative<std::vector<int>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::holds_alternative<std::vector<double>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::holds_alternative<std::vector<std::string>>(value);
        } else {
            return false;
        }
    }
};

} // namespace nlm
