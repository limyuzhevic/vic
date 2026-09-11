class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // ========== ADVANCED BRAIN CONFIGURATION COMMANDS ==========

    // Load/save checkpoints with custom naming
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);

    // Batch configuration loading from files
    bool batchLoadFromFiles(const std::vector<std::string>& filepaths);

    // Advanced plasticity control commands
    void setPlasticityEnabled(bool enabled);
    bool isPlasticityEnabled() const;
    void adjustPlasticityParameters(float stdpLTP, float stdpLTD, float hebbianRate);

    // ========== EXPERT SIMULATION CONTROL ==========

    // Simulation time control (fast-forward, slow-motion)
    void setSimulationSpeed(float speed);
    float getSimulationSpeed() const;

    // Checkpoint-based simulation resumption
    bool resumeFromCheckpoint(const std::string& checkpointPath);

    // Parallel simulation execution
    void enableParallelExecution(bool enable);
    bool isParallelExecutionEnabled() const;

    // ========== MEMORY MANAGEMENT COMMANDS ==========

    // Manual memory compaction
    void compactWorkingMemory();

    // Memory profiling
    void profileMemoryUsage() const;

    // Memory state inspection
    std::string getMemoryState() const;

    // ========== ADVANCED ANALYSIS COMMANDS ==========

    // Network topology analysis
    std::string analyzeNetworkTopology() const;

    // Learning rate optimization suggestions
    std::vector<float> suggestLearningRates() const;

    // Pattern recognition analysis
    std::string analyzePatternRecognition() const;

    // ========== CONFIGURATION MANAGEMENT ==========

    // Schema validation
    bool validateSchema() const;

    // Configuration comparison
    std::string compareConfigurations(const Config& other) const;

    // Automated configuration optimization
    std::string optimizeConfiguration() const;

    // ========== MONITORING & DEBUGGING COMMANDS ==========

    // Real-time performance metrics
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();

    // Memory usage tracking
    std::string getMemoryUsage() const;

    // Neural activity profiling
    std::string profileNeuralActivity() const;