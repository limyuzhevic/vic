// AgentBrain - Intelligent Interface Layer
// Provides high-level control and interaction with the brain

#ifndef NLM_AGENT_BRAIN_HPP
#define NLM_AGENT_BRAIN_HPP

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "brain/Brain.hpp"
#include "core/CommandLine/CommandLineParser.hpp"
#include "core/BrainStateManager.hpp"

namespace nlm {

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();

    // Core brain control
    bool initialize();
    void reset();
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);

    // Sensory and motor interfaces
    void receiveSensoryInput(const SensoryPercept& percept);
    std::unique_ptr<class Action> produceAction();
    void applyReward(float reward, float predictedReward = 0.0f);

    // Brain state management
    bool saveState(const std::string& filename);
    bool loadState(const std::string& filename);
    bool createCheckpoint(const std::string& prefix, size_t interval = 100);

    // Query brain state
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    DevelopmentalStage getDevelopmentalStage() const;

    // Advanced querying
    std::vector<NeuronId> getMostActiveNeurons(size_t count = 10) const;
    std::vector<float> getNeuronActivations() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;

    // Configuration and control
    void setSimulationSpeed(float speed);
    void setCustomTimestep(float dt);
    void enableDebugMode(bool enable);
    void setLogLevel(const std::string& level);

    // Learning and plasticity control
    void enableAllPlasticity(bool enable = true);
    void setPlasticityStrength(float strength);
    void resetPlasticity();
    void applyExternalLearning(const std::string& learningData);

    // Memory management
    void consolidateMemory();
    void clearMemory();
    void updateMemoryWithExperience(const std::string& experience);

    // Development control
    void advanceDevelopment(double timeSteps);
    void setDevelopmentalStage(DevelopmentalStage stage);
    std::string getDevelopmentReport() const;

    // Performance and monitoring
    void benchmark(int steps = 1000);
    void profile(float duration = 10.0f);
    void analyzeNetworkTopology() const;
    std::string generateBrainReport() const;
    std::map<std::string, float> getPerformanceMetrics() const;

    // Export and import
    bool exportNeurons(const std::string& filename);
    bool importNeurons(const std::string& filename);
    bool exportConnections(const std::string& filename);
    bool importConnections(const std::string& filename);
    bool exportFullState(const std::string& filename);
    bool importFullState(const std::string& filename);

    // Command line interface integration
    CommandLineParser& getCommandLineInterface() { return cli_; }
    void integrateWithCLI(CommandLineParser& parser);

    // Agent-specific operations
    void processWorldInput(const SimpleWorld& world);
    void generateBehaviorPlan();
    void executeBehaviorPlan();
    void learnFromExperience(const std::string& outcome);

    // Advanced features
    void enableAutonomousExploration(bool enable = true);
    void setExplorationStrategy(const std::string& strategy);
    void setGoalPriority(const std::string& goal, float priority);
    void updateGoals(const std::vector<std::string>& goals);

    // Debugging and analysis
    void enableContinuousLogging(bool enable);
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    std::string getDebugInfo() const;
    std::vector<std::string> getSystemAlerts() const;

    // State snapshots for research
    bool createResearchSnapshot(const std::string& filename);
    bool restoreResearchSnapshot(const std::string& filename);
    std::vector<std::string> getAvailableSnapshots() const;

    // Utility functions
    static std::string getVersionInfo();
    static void printWelcomeMessage();
    static std::string formatBrainInfo(const AgentBrain& agent);
    static bool validateBrainState(const std::string& filename);

private:
    std::shared_ptr<Brain> brain_;
    CommandLineParser cli_;
    BrainStateManager stateManager_;

    // Agent state
    float simulationSpeed_;
    float customTimestep_;
    bool debugMode_;
    std::string logLevel_;
    bool autonomousExploration_;
    std::string explorationStrategy_;
    std::map<std::string, float> goalPriorities_;
    std::vector<std::string> currentGoals_;

    // Performance monitoring
    bool continuousLogging_;
    bool performanceMonitoring_;
    std::vector<std::pair<SimulationStep, double>> performanceData_;
    std::vector<std::string> systemAlerts_;

    // Learning and development
    double developmentalAge_;
    float plasticityModifier_;
    size_t stepsSinceLastEpisode_;

    // Action planning
    std::vector<std::string> behaviorPlan_;
    size_t currentPlanStep_;

    // Private helper methods
    void setupAdvancedCLI();
    void setupBrainIntegration();
    void setupMonitoring();
    void setupLearningSystems();
    void setupActionPlanning();

    // Advanced functionality implementations
    void processWorldInputAdvanced(const SimpleWorld& world);
    void generateBehaviorPlanAdvanced();
    void executeBehaviorPlanAdvanced();
    void learnFromExperienceAdvanced(const std::string& outcome);
    void updateDevelopmentAdvanced(double timestep);
    void updateNeuromodulationAdvanced(double timestep);
    void updateMemoryAdvanced(double timestep);
    void updatePlasticityAdvanced(double timestep);

    // Performance and monitoring
    void recordPerformanceData(SimulationStep step, double timeSpent);
    void analyzePerformanceData();
    void generateSystemAlerts();
    void logDebugInfo() const;

    // Configuration management
    void loadAgentConfiguration(const std::string& filename);
    void saveAgentConfiguration(const std::string& filename) const;
    void applyConfigurationDefaults();

    // Export/import utilities
    bool exportNeuronsAdvanced(const std::string& filename) const;
    bool importNeuronsAdvanced(const std::string& filename);
    bool exportConnectionsAdvanced(const std::string& filename) const;
    bool importConnectionsAdvanced(const std::string& filename);

    // Brain analysis
    float calculateComplexityScore() const;
    size_t countConnectedComponents() const;
    bool balanceNetworkConnectivity() const;
    std::vector<std::string> identifyNetworkBottlenecks() const;

    // Utility functions
    static std::string formatFloat(float value);
    static std::string formatDuration(double seconds);
    static std::string generateTimestamp();
    static bool isValidGoalName(const std::string& goal);
};

// Brain State Manager for AgentBrain
class AgentBrainStateManager : public BrainStateManager {
public:
    AgentBrainStateManager(std::shared_ptr<AgentBrain> agent);
    ~AgentBrainStateManager();

    // Research-oriented save/load operations
    bool saveResearchSnapshot(const std::string& filename, 
                             const std::map<std::string, std::string>& metadata = {});
    bool saveTrainingSession(const std::string& filename,
                           const std::vector<SimulationStep>& criticalSteps);
    bool loadTrainingSession(const std::string& filename);

    // Advanced comparison and analysis
    static bool compareBrainStates(const std::string& file1, 
                                  const std::string& file2,
                                  std::vector<std::string>& differences,
                                  const std::map<std::string, std::string>& options = {});

    // Statistical analysis
    static std::map<std::string, double> analyzeBrainStatistics(const std::string& filename);
    static std::vector<std::string> findSimilarBrains(const std::vector<std::string>& brainFiles,
                                                     const std::string& referenceFile);

private:
    std::shared_ptr<AgentBrain> agent_;
};

// Advanced command line interface for AgentBrain
class AdvancedAgentCLI {
public:
    AdvancedAgentCLI(std::shared_ptr<AgentBrain> agent);
    ~AdvancedAgentCLI();

    void registerAllCommands(CommandLineParser& parser);
    void setupResearchCommands(CommandLineParser& parser);
    void setupLearningCommands(CommandLineParser& parser);
    void setupMonitoringCommands(CommandLineParser& parser);
    void setupDevelopmentCommands(CommandLineParser& parser);

    // Research commands
    void researchSaveSnapshot(const std::map<std::string, std::string>& options);
    void researchCompareBrains(const std::vector<std::string>& files);
    void researchAnalyzeTopology();
    void researchFindSimilar(const std::string& reference);

    // Learning commands
    void learnApplyExternal(const std::string& data);
    void learnConsolidateMemory();
    void learnResetPlasticity();
    void learnSetStrength(float strength);

    // Monitoring commands
    void monitorStart();
    void monitorStop();
    void monitorBenchmark(int steps);
    void monitorProfile(float duration);

    // Development commands
    void developAdvance(double timeSteps);
    void developSetStage(const std::string& stage);
    void developGenerateReport();

private:
    std::shared_ptr<AgentBrain> agent_;
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_HPP
