// Command-line argument parser with help and validation
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "";
        std::string checkpointFile = "";
        bool verbose = false;
        bool debug = false;
        bool quiet = false;
        std::string logLevel = "info";
        std::vector<std::string> experimentModes;
        std::vector<std::string> simulationModes;
        double simulationSpeed = 1.0;
        size_t maxSteps = 0;
        bool enableGPU = false;
        bool enableMonitoring = false;
        std::string monitoringPort = "8080";
        std::string pluginPath = "./plugins";
        bool listPlugins = false;
        bool showVersion = false;
        bool showHelp = false;
        
        // NLM-specific options
        std::string agentMode = "auto";  // auto, basic, advanced, custom
        std::string learningMode = "auto";  // auto, episodic, procedural, semantic
        std::string memoryMode = "auto";  // auto, working, episodic, semantic
        std::string neuromodulationMode = "auto";  // auto, dopamine, curiosity, novelty
        double curiosityThreshold = 0.3;
        double noveltyThreshold = 0.5;
        float plasticityRate = 0.01f;
        bool enableReplay = true;
        bool enableCheckpointing = true;
        bool enableDevelopment = true;
        std::string developmentProfile = "standard";
    };

    static Options parse(int argc, char** argv);
    static void printHelp(const std::string& programName);
    static void printVersion();
    static bool validateOptions(const Options& options);
};

// Enhanced configuration manager with better defaults and validation
class EnhancedConfig : public Config {
public:
    EnhancedConfig();
    
    // Load configuration with fallbacks and validation
    bool loadWithValidation(const std::string& filepath = "");
    
    // Get configuration with better defaults for NLM
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, const std::string& description = "") const {
        auto value = Config::getOr<T>(key, defaultValue);
        // Log if value differs from documented default
        return value;
    }
    
    // Apply configuration to brain and agent systems
    void applyToBrain(std::shared_ptr<Brain> brain) const;
    void applyToAgentBrain(std::shared_ptr<AgentBrain> agentBrain) const;
    void applyToWorld(std::shared_ptr<SimpleWorld> world) const;
    
    // Get configuration summary for debugging
    std::string getConfigurationReport() const;
    
    // Check if configuration is valid for the current mode
    bool validateConfiguration(const std::string& mode = "auto") const;

private:
    std::string currentMode_;
    std::vector<std::string> validationErrors_;
};

// Plugin manager for extending functionality
class PluginManager {
public:
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string path;
        bool enabled = true;
        std::vector<std::string> dependencies;
    };

    PluginManager();
    ~PluginManager();
    
    // Load plugins from directory
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Get plugin by name
    PluginInfo* getPlugin(const std::string& name);
    
    // Enable/disable plugin
    bool enablePlugin(const std::string& name, bool enable = true);
    
    // List all available plugins
    std::vector<PluginInfo> listPlugins() const;
    
    // Execute plugin function
    template<typename T>
    bool executePluginFunction(const std::string& pluginName, const std::string& functionName, T& result);
    
    // Get plugins suitable for specific mode
    std::vector<PluginInfo> getPluginsForMode(const std::string& mode) const;

private:
    std::unordered_map<std::string, PluginInfo> plugins_;
    std::unordered_map<std::string, void*> pluginHandles_;
};

// Advanced experiment runner with more control
class AdvancedExperimentRunner {
public:
    struct ExperimentOptions {
        std::string mode;  // basic, advanced, integrated, custom
        std::vector<std::string> phases;  // phase1, phase2, phase3, phase4, phase5, phase6
        std::unordered_map<std::string, std::string> phaseConfigs;
        bool continuous = false;
        double checkpointInterval = 60.0;  // seconds
        size_t maxMemoryEpisodes = 100;
        float rewardDecay = 0.95f;
        float noveltyDecay = 0.99f;
        float curiosityThreshold = 0.3f;
        std::string outputFormat = "json";  // json, csv, text, binary
        std::string outputPath = ".";
        bool enableVisualization = false;
        bool enableProfiling = false;
        std::string profilerOutput = "profile.dat";
    };

    AdvancedExperimentRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    // Run experiment with advanced options
    bool runExperiment(const ExperimentOptions& options);
    
    // Get experiment statistics
    struct ExperimentStats {
        double totalTime;
        size_t totalSteps;
        float totalReward;
        float avgFiringRate;
        size_t memoryEntries;
        size_t checkpointCount;
        std::unordered_map<std::string, float> metrics;
    };
    
    ExperimentStats getExperimentStats() const;
    
    // Save experiment data
    bool saveExperimentData(const std::string& filepath, const ExperimentStats& stats) const;
    
    // Load experiment configuration
    bool loadExperimentConfig(const std::string& filepath);

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    ExperimentStats stats_;
    std::chrono::steady_clock::time_point startTime_;
};

// Enhanced monitoring and logging system
class MonitoringSystem {
public:
    struct Metric {
        std::string name;
        double value;
        double timestamp;
        std::unordered_map<std::string, std::string> tags;
    };

    MonitoringSystem();
    
    // Record metric
    void recordMetric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags = {});
    
    // Get metrics for time range
    std::vector<Metric> getMetrics(const std::string& name, double startTime, double endTime) const;
    
    // Get latest metric value
    std::optional<double> getLatestMetric(const std::string& name) const;
    
    // Start monitoring thread
    void startMonitoring(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    // Stop monitoring
    void stopMonitoring();
    
    // Get system health status
    struct HealthStatus {
        bool healthy = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double cpuUsage;
        double memoryUsage;
        size_t activeNeurons;
        size_t totalSynapses;
    };
    
    HealthStatus getHealthStatus() const;

private:
    std::vector<Metric> metrics_;
    mutable std::mutex metricsMutex_;
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_;
};

// Utility functions for common tasks
namespace Utils {
    // Create configuration from defaults and command line
    EnhancedConfig createConfiguration(int argc, char** argv);
    
    // Create agent with optimal settings for mode
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain, const std::string& mode = "auto");
    
    // Create world with specific settings
    std::shared_ptr<SimpleWorld> createWorld(const std::string& mode = "standard");
    
    // Run predefined experiment suites
    bool runPhase1Tests(std::shared_ptr<Brain> brain);
    bool runPhase2Tests(std::shared_ptr<Brain> brain);
    bool runPhase3Tests(std::shared_ptr<Brain> brain);
    bool runPhase4Tests(std::shared_ptr<Brain> brain);
    bool runPhase6Tests(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    // Save brain state with metadata
    bool saveBrainWithMetadata(std::shared_ptr<Brain> brain, const std::string& filepath, const std::string& metadata = "");
    
    // Load brain state with validation
    std::shared_ptr<Brain> loadBrainWithValidation(const std::string& filepath);
    
    // Generate configuration template
    void generateConfigTemplate(const std::string& outputFile = "configs/template.cfg");
    
    // Validate configuration file
    bool validateConfigFile(const std::string& filepath);
    
    // Get system information
    struct SystemInfo {
        std::string platform;
        std::string compiler;
        std::string version;
        size_t cpuCores;
        size_t totalMemory;
        bool hasGPU;
        std::string cudaVersion;
    };
    
    SystemInfo getSystemInfo();
}