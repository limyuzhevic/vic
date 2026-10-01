// Target include path for easier code management
#include <nlm/> // Provides convenient access to core headers

// Research project structure for easier development
struct NLMResearch {
    // Core neural computation components
    struct NeuralCore {
        void initialize(const Config& config);
        std::shared_ptr<Brain> createBrain(const Config& config);
        void runSimulation(std::shared_ptr<Brain> brain, size_t steps);
    };

    // Agent-world interaction components  
    struct AgentWorld {
        void setupSimpleWorld(SimpleWorld& world, size_t width = 16, size_t height = 16);
        void runAgentWorldLoop(std::shared_ptr<Brain> brain, SimpleWorld& world, 
                             AgentBrain& agent, size_t steps);
        void computeAdvancedMetrics(const Phase6IntegrationResult& result);
    };

    // Advanced learning and development
    struct AdvancedLearning {
        void enableMemoryReplay(std::shared_ptr<Brain> brain, size_t replayInterval = 100);
        void enableDevelopmentStages(std::shared_ptr<Brain> brain, 
                                    DevelopmentalStage stage = DevelopmentalStage::Adult);
        void integrateCuriosityExploration(AgentBrain& agent);
        void setupPredictionBasedActionSelection(std::shared_ptr<Brain> brain, 
                                               AgentBrain& agent);
    };

    // Performance and optimization
    struct Performance {
        void enableMemoryPools(size_t poolSize = 1024 * 1024);
        void enableSparseConnectivityOptimization(float sparsity = 0.9f);
        void configureParallelProcessing(int threads = std::thread::hardware_concurrency());
        PerformanceBenchmark runBenchmarks(const std::vector<size_t>& neuronCounts);
    };

    // Experiment infrastructure
    struct Experimentation {
        void createPhase6DemoExecutable();
        void addAdvancedCommandsToBuild(const std::string& command);
        void integrateAblationStudies(std::shared_ptr<Brain> brain);
        void setupMultiAgentCompetition(const std::vector<std::string>& agentNames);
    };
};

// User-friendly research API
namespace nlm {
    // Simplified API for researchers
    class ResearchBrain {
    public:
        explicit ResearchBrain(const Config& config = Config::createDefault());
        ~ResearchBrain();

        // Core functionality
        void runPhase6Experiment(const Phase6Config& config);
        void testIntegration(std::shared_ptr<Brain> brain);
        Phase6IntegrationResult getIntegrationResults() const;

        // Advanced features
        void enableMemoryReplay(size_t interval = 100);
        void enableDevelopment(DevelopmentalStage stage = DevelopmentalStage::Adult);
        void integrateCuriosity(bool enable = true);
        void setupPredictionSystem(bool enable = true);

        // Monitoring and analysis
        void enableRealTimeMetrics(bool enable = true);
        void captureExperiences(size_t maxEpisodes = 1000);
        void setupReplaySystem(size_t replayInterval = 100);

        // State management
        bool saveState(const std::string& path) const;
        bool loadState(const std::string& path);
        void resetState();

        // Performance
        PerformanceBenchmark runBenchmarks(const std::vector<size_t>& neuronCounts);

    private:
        std::shared_ptr<Brain> brain_;
        std::unique_ptr<Phase6IntegratedExperiment> experiment_;
        bool integrationVerified_;
    };

    // Convenience functions for common research scenarios
    inline std::unique_ptr<ResearchBrain> createResearchBrain(const std::string& configFile = "configs/default.cfg");
    inline Phase6IntegrationResult runPhase6IntegrationTest(size_t neuronCount = 500, size_t steps = 2000);
    inline void createPhase6Demo();
}