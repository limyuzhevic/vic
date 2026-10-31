// MultiAgentEnvironment: Environment supporting multiple interacting agents
// Enables collaborative learning and multi-agent coordination
//
// Key mechanisms:
// - Multiple agent support with independent brain instances
// - Inter-agent communication and collaboration
// - Shared resources and competition
// - Joint learning and knowledge transfer
// - Swarm intelligence behaviors

class MultiAgentEnvironment {
public:
    MultiAgentEnvironment();
    ~MultiAgentEnvironment();

    // Initialize with configuration
    bool initialize(const std::shared_ptr<Config>& config);

    // Add an agent to the environment
    bool addAgent(std::shared_ptr<AgentBrain> agent);

    // Remove an agent from the environment
    bool removeAgent(size_t agentId);

    // Get agent by ID
    std::shared_ptr<AgentBrain> getAgent(size_t agentId) const;

    // Get all agents
    std::vector<std::shared_ptr<AgentBrain>> getAllAgents() const;

    // Get number of agents
    size_t getAgentCount() const;

    // Update environment with current state
    void update(double timestep);

    // Execute actions for all agents
    std::vector<Action> executeActions() const;

    // Get sensory percepts for all agents
    std::vector<std::shared_ptr<SensoryPercept>> getSensoryPercepts() const;

    // Get agent positions
    std::vector<std::pair<float, float>> getAgentPositions() const;

    // Set resource positions
    void setResourcePosition(size_t agentId, float x, float y, float value);

    // Get resource value at position
    float getResourceValue(float x, float y) const;

    // Check if position is occupied
    bool isOccupied(float x, float y) const;

    // Get environment dimensions
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

    // Set environment type
    void setEnvironmentType(const std::string& type); // "swarm", "cooperative", "competitive", "mixed"

    // Enable inter-agent communication
    void enableCommunication(bool enable);

    // Get communication strength between agents
    float getCommunicationStrength(size_t agentId1, size_t agentId2) const;

    // Set communication strength
    void setCommunicationStrength(size_t agentId1, size_t agentId2, float strength);

    // Enable knowledge transfer between agents
    void enableKnowledgeTransfer(bool enable);

    // Get knowledge transfer matrix
    std::vector<std::vector<float>> getKnowledgeTransferMatrix() const;

    // Set knowledge transfer strength
    void setKnowledgeTransfer(size_t fromAgent, size_t toAgent, float strength);

    // Reset environment
    void reset();

    // Save environment state
    bool save(const std::string& filepath) const;

    // Load environment state
    bool load(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    float width_;
    float height_;
    std::string environmentType_;
    bool communicationEnabled_;
    bool knowledgeTransferEnabled_;
};

// MultiAgentExperiment: Framework for multi-agent learning experiments
// Enables systematic study of collaborative and competitive behaviors
//
// Key features:
// - Configurable agent populations
// - Task definition and evaluation
// - Metrics collection for coordination, cooperation, competition
// - Statistical analysis of emergent behaviors
// - Comparison with single-agent baselines

class MultiAgentExperiment {
public:
    MultiAgentExperiment();
    ~MultiAgentExperiment();

    // Initialize experiment with configuration
    bool initialize(const std::shared_ptr<Config>& config);

    // Add agent population
    bool addPopulation(size_t count, const std::string& behaviorType);

    // Set task
    void setTask(const std::string& taskType, const std::map<std::string, float>& parameters);

    // Set metrics to collect
    void setMetrics(const std::vector<std::string>& metricNames);

    // Add baseline agent (for comparison)
    bool addBaseline(const std::string& behaviorType);

    // Run experiment
    MultiAgentResults run(size_t maxSteps, size_t maxEpisodes);

    // Get experiment results
    const MultiAgentResults& getResults() const;

    // Save results
    bool saveResults(const std::string& filepath) const;

    // Analyze results
    void analyzeResults();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// MultiAgentResults: Results from multi-agent experiment
// Contains comprehensive metrics for analyzing multi-agent behavior

struct MultiAgentResults {
    // Basic statistics
    size_t totalSteps;
    size_t totalEpisodes;
    double totalTime;

    // Agent statistics
    struct AgentStats {
        size_t agentId;
        std::string behaviorType;
        size_t episodesCompleted;
        double finalFitness;
        double averageFitness;
        double successRate;
        size_t totalStepsTaken;
        double averageStepSize;
        double coordinationScore;
        double learningRate;
    };

    std::vector<AgentStats> agentStats;

    // Population statistics
    struct PopulationStats {
        std::string behaviorType;
        size_t count;
        double averageFitness;
        double stdDevFitness;
        double successRate;
        double averageCoordination;
    };

    std::vector<PopulationStats> populationStats;

    // Task-specific metrics
    std::map<std::string, double> taskMetrics;

    // Emergent behavior metrics
    double socialCohesion;
    double collectiveIntelligence;
    double adaptationSpeed;
    double explorationExploitationBalance;

    // Comparison with baselines
    std::map<std::string, double> baselineComparisons;

    // Time series data
    std::vector<std::vector<double>> fitnessTimeSeries;
    std::vector<std::vector<double>> coordinationTimeSeries;
    std::vector<std::vector<double>> learningTimeSeries;

    MultiAgentResults() : totalSteps(0), totalEpisodes(0), totalTime(0.0),
                         socialCohesion(0.0), collectiveIntelligence(0.0),
                         adaptationSpeed(0.0), explorationExploitationBalance(0.0) {}
};

} // namespace nlm