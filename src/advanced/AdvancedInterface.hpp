#pragma once

#include "AdvancedBrainTypes.hpp"
#include "ExperimentSystem.hpp"

// Advanced commands interface for integration with main application
namespace nlm {

class AdvancedCommandsInterface {
public:
    virtual ~AdvancedCommandsInterface() = default;
    
    // Advanced Brain Commands
    virtual bool exportBrainWithMetadata(const std::string& filepath,
                                         std::shared_ptr<Brain> brain,
                                         const std::string& metadata = "") = 0;
    
    virtual bool importBrainWithValidation(const std::string& filepath,
                                          std::shared_ptr<Brain> brain,
                                          bool validateConnectivity = true) = 0;
    
    virtual std::string generatePerformanceReport(std::shared_ptr<Brain> brain,
                                                   int reportDuration = 1000) = 0;
    
    virtual void advancedSimulationControl(std::shared_ptr<Brain> brain,
                                           const SimulationControlParams& params) = 0;
    
    virtual NetworkTopologyAnalysis analyzeNetworkTopology(std::shared_ptr<Brain> brain) = 0;
    
    virtual PlasticityPatternAnalysis analyzePlasticityPatterns(std::shared_ptr<Brain> brain,
                                                               int windowSize = 100) = 0;
    
    virtual MemorySystemAnalysis analyzeMemorySystems(std::shared_ptr<Brain> brain) = 0;
    
    virtual bool redistributeNeurons(std::shared_ptr<Brain> brain,
                                   const std::vector<NeuronType>& newDistribution) = 0;
    
    virtual bool modifyConnectivityPattern(std::shared_ptr<Brain> brain,
                                           const ConnectivityPattern& pattern) = 0;
    
    virtual bool tuneNeuromodulationParameters(std::shared_ptr<Brain> brain,
                                               const NeuromodulationTuning& tuning) = 0;
    
    virtual bool simulateDevelopmentalStage(std::shared_ptr<Brain> brain,
                                           DevelopmentalStage stage,
                                           float durationMultiplier = 1.0f) = 0;
    
    virtual std::vector<ExperimentResult> runBatchExperiments(
        const std::vector<ExperimentConfig>& experiments) = 0;
    
    virtual bool exportVisualizationData(const std::string& filepath,
                                        std::shared_ptr<Brain> brain,
                                        const VisualizationExportOptions& options) = 0;
    
    virtual void startRealTimeMonitor(std::shared_ptr<Brain> brain,
                                    const MonitorConfiguration& config) = 0;
    
    virtual EmergentPropertyReport discoverEmergentProperties(std::shared_ptr<Brain> brain,
                                                             const DiscoveryParameters& params) = 0;
    
    // Advanced Brain State Management
    virtual bool createFullCheckpoint(const std::string& filepath,
                                     std::shared_ptr<Brain> brain,
                                     const std::string& description = "") = 0;
    
    virtual bool restoreStateWithRollback(const std::string& filepath,
                                        std::shared_ptr<Brain> brain,
                                        const RollbackOptions& options = {}) = 0;
    
    virtual StateComparisonAnalysis compareStates(const std::vector<std::string>& filepaths) = 0;
    
    virtual void startLiveStateMonitoring(std::shared_ptr<Brain> brain,
                                        const MonitoringThresholds& thresholds) = 0;
    
    // Advanced Simulation Features
    virtual void runMultiScaleSimulation(std::shared_ptr<Brain> brain,
                                       const MultiScaleParams& params) = 0;
    
    virtual StressTestResult runStressTest(std::shared_ptr<Brain> brain,
                                         const StressTestConfig& config) = 0;
    
    virtual EvolutionResult runEvolutionarySimulation(std::shared_ptr<Brain> brain,
                                                     const EvolutionConfig& config) = 0;
    
    virtual ReinforcementLearningResult integrateReinforcementLearning(
        std::shared_ptr<Brain> brain,
        const RLConfig& config,
        const std::vector<EnvironmentAction>& actions) = 0;
    
    virtual MetaCognitionResult simulateMetaCognition(std::shared_ptr<Brain> brain,
                                                     const MetaCognitiveParams& params) = 0;
    
    virtual QuantumNeuralResult simulateQuantumNeuralDynamics(std::shared_ptr<Brain> brain,
                                                             const QuantumParams& params) = 0;
    
    // Advanced Analysis Tools
    virtual RobustnessAnalysis analyzeNetworkRobustness(std::shared_ptr<Brain> brain,
                                                       const RobustnessParameters& params) = 0;
    
    virtual LearningCurveAnalysis analyzeLearningCurves(std::shared_ptr<Brain> brain,
                                                       const LearningParameters& params) = 0;
    
    virtual PatternRecognitionResult recognizeNeuralPatterns(std::shared_ptr<Brain> brain,
                                                           const PatternParameters& params) = 0;
    
    virtual CausalAnalysisResult performCausalAnalysis(std::shared_ptr<Brain> brain,
                                                     const CausalParameters& params) = 0;
    
    virtual PredictiveModelResult predictNeuralDynamics(std::shared_ptr<Brain> brain,
                                                       const PredictionParameters& params) = 0;
    
    virtual InformationTheoryResult analyzeInformationFlow(std::shared_ptr<Brain> brain,
                                                         const InformationParameters& params) = 0;
    
    virtual ComplexNetworkResult analyzeComplexNetworkProperties(std::shared_ptr<Brain> brain,
                                                                const ComplexNetworkParams& params) = 0;
    
    // Development and Debugging Tools
    virtual void advancedDebugSession(std::shared_ptr<Brain> brain,
                                    const DebugConfiguration& config) = 0;
    
    virtual PerformanceProfile profileBrainPerformance(std::shared_ptr<Brain> brain,
                                                      const ProfileParameters& params) = 0;
    
    virtual MemoryLeakReport detectMemoryLeaks(std::shared_ptr<Brain> brain) = 0;
    
    virtual bool enableCodeHotSwapping(std::shared_ptr<Brain> brain,
                                     const CodeSwapConfiguration& config) = 0;
    
    virtual TestSuiteResult runAdvancedTestSuite(std::shared_ptr<Brain> brain,
                                               const TestSuiteConfig& config) = 0;
    
    virtual void enableAdvancedLogging(std::shared_ptr<Brain> brain,
                                      const LoggingConfiguration& config) = 0;
    
    // User Interface Extensions
    virtual CLICommandSet getAdvancedCLICommands() = 0;
    
    virtual void startWebInterface(std::shared_ptr<Brain> brain,
                                 const WebInterfaceConfig& config) = 0;
    
    virtual void startAdvancedVisualization(std::shared_ptr<Brain> brain,
                                           const VisualizationConfig& config) = 0;
    
    virtual void startInteractiveExploration(std::shared_ptr<Brain> brain,
                                           const ExplorationConfig& config) = 0;
    
    virtual ScriptExecutionResult executeBrainScripts(const std::string& scriptPath,
                                                    std::shared_ptr<Brain> brain,
                                                    const ScriptConfig& config) = 0;
};

// Main factory function for creating advanced commands interface
AdvancedCommandsInterface* createAdvancedCommandsInterface();
void destroyAdvancedCommandsInterface(AdvancedCommandsInterface* interface);

} // namespace nlm
