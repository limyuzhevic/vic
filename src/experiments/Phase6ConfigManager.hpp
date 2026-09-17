    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isNoveltyEnabled() const { return noveltyEnabled_; }
    bool isPredictionErrorEnabled() const { return predictionErrorEnabled_; }
    
    // Configuration validation
    bool validateConfiguration() const;
    std::string getValidationErrors() const;
    
    // Get default configuration
    static Phase6Config getDefaultConfig();
    
    // Create and configure brain from this config
    std::shared_ptr<Brain> createBrain() const;
    
    // Create and configure agent from this config
    std::shared_ptr<AgentBrain> createAgent(const SimpleWorld& world) const;
    
    // Create and configure world from this config
    std::shared_ptr<SimpleWorld> createWorld() const;
    
    // Create and configure world from parameters
    std::shared_ptr<SimpleWorld> createWorld(size_t width, size_t height) const;
    
    // Get configuration summary for logging
    std::string getConfigSummary() const;
    
    // Set configuration from parsed arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Save configuration to file
    bool saveToFile(const std::string& filename) const;

private:
    uint64_t maxSteps_;
    size_t neuronCount_;
    size_t regionCount_;
    float connectionProbability_;
    bool enableCheckpointing_;
    bool enableReplay_;
    bool enableDevelopment_;
    bool enableCuriosity_;
    bool enableNovelty_;
    bool enablePredictionError_;
    std::string checkpointPath_;
    std::vector<std::string> validationErrors_;
};

/**
 * Abstract interface for experiment runners
 */
class IPhase6ExperimentRunner {
public:
    virtual ~IPhase6ExperimentRunner() = default;
    
    virtual Phase6IntegrationResult run(const Phase6Config& config) = 0;
    virtual bool verifyIntegration() = 0;
    virtual bool testMemoryIntegration() = 0;
    virtual bool testNeuromodulationIntegration() = 0;
    virtual bool testCheckpointing() = 0;
    virtual bool testReplay() = 0;
};

/**
 * Abstract interface for system creation (Factory Pattern)
 */
class ISystemCreator {
public:
    virtual ~ISystemCreator() = default;
    
    virtual std::shared_ptr<Brain> createBrain(const Config& config) const = 0;
    virtual std::shared_ptr<AgentBrain> createAgent(const Brain& brain, const SimpleWorld& world) const = 0;
    virtual std::shared_ptr<SimpleWorld> createWorld(size_t width, size_t height) const = 0;
};

/**
 * Abstract interface for validation strategies
 */
class IValidationStrategy {
public:
    virtual ~IValidationStrategy() = default;
    virtual bool validate(const Phase6Config& config) const = 0;
    virtual std::string getName() const = 0;
};

} // namespace nlm
