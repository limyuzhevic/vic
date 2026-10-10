// Forward declarations for development tools
class BrainDebugger;
class BrainSimulator;
class ParameterController;
class NetworkBuilder;
class BrainVisualizer;
class ActivityMonitor;
class LearningAnalyzer;
class SimulationRunner;
class ExperimentDesigner;
class ScenarioPlayer;
class DataCollector;
class ResultAnalyzer;
class ConnectionManager;
class BrainExporter;
class BrainImporter;

namespace nlm {

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
    // Initialize with brain and neuromodulation systems
    bool initialize(std::shared_ptr<NeuromodulationEngine> neuromodulationEngine);
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply advanced neuromodulation based on reward and learning
    void applyAdvancedRewardModulation(float reward, float predictedReward, float novelty, float curiosity);
    
    // Update development system with neuromodulation
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get advanced neuromodulation level
    float getAdvancedNeuromodulationLevel() const;
    
    // Get neuromodulation system states
    float getDopamineLevel() const { return neuromodulationState_.dopamineLevel; }
    float getAcetylcholineLevel() const { return neuromodulationState_.acetylcholineLevel; }
    float getNorepinephrineLevel() const { return neuromodulationState_.norepinephrineLevel; }
    float getSerotoninLevel() const { return neuromodulationState_.serotoninLevel; }
    float getCuriosityLevel() const { return neuromodulationState_.curiosityLevel; }
    float getNoveltyLevel() const { return neuromodulationState_.noveltyLevel; }
    float getPredictionError() const { return neuromodulationState_.predictionError; }
    
    // Get advanced neuromodulation states
    float getSignalSynthesisLevel() const { return neuromodulationState_.signalSynthesizedLevel; }
    float getAdaptiveSignalLevel() const { return neuromodulationState_.adaptiveSignalLevel; }
    float getLearningSignalLevel() const { return neuromodulationState_.learningSignalLevel; }
    
    // Get network states
    float getNetworkEfficiency() const { return neuromodulationState_.networkEfficiency; }
    float getSignalPropagationSpeed() const { return neuromodulationState_.signalPropagationSpeed; }
    float getNeuromodulatorCrossTalk() const { return neuromodulationState_.neuromodulatorCrossTalk; }
    
    // Get plasticity states
    float getOverallPlasticity() const { return neuromodulationState_.overallPlasticity; }
    float getGatedPlasticity() const { return neuromodulationState_.gatedPlasticity; }
    float getScaledPlasticity() const { return neuromodulationState_.scaledPlasticity; }
    float getTimingDependentPlasticity() const { return neuromodulationState_.timingDependentPlasticity; }
    
    // Get performance metrics
    float getSignalQuality() const { return neuromodulationState_.signalQuality; }
    float getSignalReliability() const { return neuromodulationState_.signalReliability; }
    float getAdaptationSpeed() const { return neuromodulationState_.adaptationSpeed; }
    float getLearningRate() const { return neuromodulationState_.learningRate; }
    
    // Get safety and stability metrics
    float getSafetyMargin() const { return neuromodulationState_.safetyMargin; }
    float getStabilityMargin() const { return neuromodulationState_.stabilityMargin; }
    float getNumericalStability() const { return neuromodulationState_.numericalStability; }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableAdvancedNeuromodulation(bool enable) { advancedNeuromodulationEnabled_ = enable; }
    void enableSignalSynthesis(bool enable) { signalSynthesisEnabled_ = enable; }
    void enableAdaptiveLearning(bool enable) { adaptiveLearningEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isAdvancedNeuromodulationEnabled() const { return advancedNeuromodulationEnabled_; }
    bool isSignalSynthesisEnabled() const { return signalSynthesisEnabled_; }
    bool isAdaptiveLearningEnabled() const { return adaptiveLearningEnabled_; }
    
    // Neuromodulation system access
    NeuromodulationEngine* getNeuromodulationEngine() { return neuromodulationEngine_.get(); }
    const NeuromodulationEngine* getNeuromodulationEngine() const { return neuromodulationEngine_.get(); }
    
    DopamineSystem* getDopamineSystem() { return neuromodulationEngine_ ? neuromodulationEngine_->getDopamineSystem() : nullptr; }
    const DopamineSystem* getDopamineSystem() const { return neuromodulationEngine_ ? neuromodulationEngine_->getDopamineSystem() : nullptr; }
    
    CuriositySystem* getCuriositySystem() { return neuromodulationEngine_ ? neuromodulationEngine_->getCuriositySystem() : nullptr; }
    const CuriositySystem* getCuriositySystem() const { return neuromodulationEngine_ ? neuromodulationEngine_->getCuriositySystem() : nullptr; }
    
    PredictionErrorSystem* getPredictionErrorSystem() { return neuromodulationEngine_ ? neuromodulationEngine_->getPredictionErrorSystem() : nullptr; }
    const PredictionErrorSystem* getPredictionErrorSystem() const { return neuromodulationEngine_ ? neuromodulationEngine_->getPredictionErrorSystem() : nullptr; }
    
    NoveltySystem* getNoveltySystem() { return neuromodulationEngine_ ? neuromodulationEngine_->getNoveltySystem() : nullptr; }
    const NoveltySystem* getNoveltySystem() const { return neuromodulationEngine_ ? neuromodulationEngine_->getNoveltySystem() : nullptr; }
    
    NeuromodulatorNetwork* getNeuromodulatorNetwork() { return neuromodulationEngine_ ? neuromodulationEngine_->getNeuromodulatorNetwork() : nullptr; }
    const NeuromodulatorNetwork* getNeuromodulatorNetwork() const { return neuromodulationEngine_ ? neuromodulationEngine_->getNeuromodulatorNetwork() : nullptr; }
    
    PlasticityController* getPlasticityController() { return neuromodulationEngine_ ? neuromodulationEngine_->getPlasticityController() : nullptr; }
    const PlasticityController* getPlasticityController() const { return neuromodulationEngine_ ? neuromodulationEngine_->getPlasticityController() : nullptr; }
    
    // Visualization integration
    bool initializeVisualization();
    void updateVisualization();
    void renderVisualization();
    void closeVisualization();
    bool isVisualizationActive() const;
    void setVisualizationDisplayMode(int mode);
    int getVisualizationDisplayMode() const;
    bool exportVisualizationData(const std::string& filename, const std::string& format = "ascii");
    void loadVisualizationConfiguration(const std::string& configFile);
    void saveVisualizationConfiguration() const;
    
    // Visualization state management
    void enableVisualization(bool enable) { visualizationEnabled_ = enable; }
    bool isVisualizationEnabled() const { return visualizationEnabled_; }
    void setVisualizationBrainReference(bool useReference) { useVisualizationBrainReference_ = useReference; }
    bool isUsingVisualizationBrainReference() const { return useVisualizationBrainReference_; }
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable) { performanceMonitoringEnabled_ = enable; }
    bool isPerformanceMonitoringEnabled() const { return performanceMonitoringEnabled_; }
    void updatePerformanceMetrics();
    std::string getPerformanceSummary() const;
    
    // Advanced neuromodulation operations
    void updateNeuromodulation(double timestep);
    void synthesizeNeuromodulatorySignals();
    void adaptToEnvironment(const SimpleWorld& world);
    void maintainNeuromodulatoryHomeostasis();
    void updateLearningSystems(double timestep);
    void updatePlasticitySystems(double timestep);
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Get neuromodulation engine pointer
    NeuromodulationEngine* getNeuromodulationEnginePtr() { return neuromodulationEngine_.get(); }
    
    // Configuration
    void setSignalSynthesisParams(const class SignalSynthesisParams& params);
    void setAdaptationParams(const class AdaptationParams& params);
    void setPlasticityParams(const class PlasticityParams& params);
    
    // Development tool integration
    bool initializeDevelopmentTools(std::shared_ptr<Config> config);
    void shutdownDevelopmentTools();
    bool isDevelopmentToolsInitialized() const;
    
    // Development tool attachment
    bool attachBrainDebugger(std::shared_ptr<BrainDebugger> debugger);
    bool attachBrainSimulator(std::shared_ptr<BrainSimulator> simulator);
    bool attachParameterController(std::shared_ptr<ParameterController> controller);
    bool attachNetworkBuilder(std::shared_ptr<NetworkBuilder> builder);
    bool attachBrainVisualizer(std::shared_ptr<BrainVisualizer> visualizer);
    bool attachActivityMonitor(std::shared_ptr<ActivityMonitor> monitor);
    bool attachLearningAnalyzer(std::shared_ptr<LearningAnalyzer> analyzer);
    bool attachSimulationRunner(std::shared_ptr<SimulationRunner> runner);
    bool attachExperimentDesigner(std::shared_ptr<ExperimentDesigner> designer);
    bool attachScenarioPlayer(std::shared_ptr<ScenarioPlayer> player);
    bool attachDataCollector(std::shared_ptr<DataCollector> collector);
    bool attachResultAnalyzer(std::shared_ptr<ResultAnalyzer> analyzer);
    bool attachConnectionManager(std::shared_ptr<ConnectionManager> manager);
    bool attachBrainExporter(std::shared_ptr<BrainExporter> exporter);
    bool attachBrainImporter(std::shared_ptr<BrainImporter> importer);
    
    // Development tool access
    std::shared_ptr<BrainDebugger> getBrainDebugger() const;
    std::shared_ptr<BrainSimulator> getBrainSimulator() const;
    std::shared_ptr<ParameterController> getParameterController() const;
    std::shared_ptr<NetworkBuilder> getNetworkBuilder() const;
    std::shared_ptr<BrainVisualizer> getBrainVisualizer() const;
    std::shared_ptr<ActivityMonitor> getActivityMonitor() const;
    std::shared_ptr<LearningAnalyzer> getLearningAnalyzer() const;
    std::shared_ptr<SimulationRunner> getSimulationRunner() const;
    std::shared_ptr<ExperimentDesigner> getExperimentDesigner() const;
    std::shared_ptr<ScenarioPlayer> getScenarioPlayer() const;
    std::shared_ptr<DataCollector> getDataCollector() const;
    std::shared_ptr<ResultAnalyzer> getResultAnalyzer() const;
    std::shared_ptr<ConnectionManager> getConnectionManager() const;
    std::shared_ptr<BrainExporter> getBrainExporter() const;
    std::shared_ptr<BrainImporter> getBrainImporter() const;
    
    // Development tool control
    bool enableBrainAnalysis();
    bool disableBrainAnalysis();
    bool enableCustomSimulation();
    bool disableCustomSimulation();
    bool enableParameterModification();
    bool disableParameterModification();
    bool enableNetworkConstruction();
    bool disableNetworkConstruction();
    bool enableVisualization();
    bool disableVisualization();
    bool enableActivityMonitoring();
    bool disableActivityMonitoring();
    bool enableLearningAnalysis();
    bool disableLearningAnalysis();
    bool enableSimulationRunning();
    bool disableSimulationRunning();
    bool enableExperimentDesign();
    bool disableExperimentDesign();
    bool enableScenarioPlayback();
    bool disableScenarioPlayback();
    bool enableDataCollection();
    bool disableDataCollection();
    bool enableResultAnalysis();
    bool disableResultAnalysis();
    bool enableConnectionManagement();
    bool disableConnectionManagement();
    bool enableBrainExport();
    bool disableBrainExport();
    bool enableBrainImport();
    bool disableBrainImport();
    
    // Development tool configuration
    void setDevelopmentToolConfig(const DevelopmentToolConfig& config);
    DevelopmentToolConfig getDevelopmentToolConfig() const;
    
    // Development tool statistics
    DevelopmentToolStatistics getDevelopmentToolStatistics() const;
    
    // Development tool validation
    bool validateDevelopmentTools() const;
    std::vector<std::string> getDevelopmentToolValidationErrors() const;
    
    // Development tool lifecycle management
    void cleanupDevelopmentTools();
    bool isDevelopmentToolsHealthy() const;
    std::string getDevelopmentToolStatus() const;
    
    // Integration with development tools
    void syncWithDevelopmentTools();
    void updateDevelopmentTools(double timestep);
    void renderDevelopmentTools();
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Visualization integration
    std::unique_ptr<VisualizationInterface> visualization_;
    
    // Performance metrics
    struct PerformanceMetrics {
        double fps;
        double frameTime;
        size_t memoryUsage;
        size_t activeNeurons;
        size_t totalSpikes;
        float cpuUsage;
        float memoryUsagePercent;
        double lastUpdateTime;
        
        PerformanceMetrics() : fps(0.0), frameTime(0.0), memoryUsage(0), activeNeurons(0),
                              totalSpikes(0), cpuUsage(0.0), memoryUsagePercent(0.0), lastUpdateTime(0.0) {}
    };
    
    PerformanceMetrics performanceMetrics_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> neuromodulationEngine_;
    
    // Advanced neuromodulation state
    AdvancedNeuromodulationState neuromodulationState_;
    
    // Development tool integration
    DevelopmentToolIntegration developmentTools_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool advancedNeuromodulationEnabled_;
    bool signalSynthesisEnabled_;
    bool adaptiveLearningEnabled_;
    
    // Visualization configuration flags
    bool visualizationEnabled_;
    bool useVisualizationBrainReference_;
    bool performanceMonitoringEnabled_;
    
    // Configuration
    std::string visualizationConfigFile_;
    std::string exportDirectory_;
    bool autoExportEnabled_;
    
    void updateVisualizationState();
    void updatePerformanceMetrics(double frameTime);
    void initializeVisualizationComponents();
    void validateVisualizationSetup() const;
    std::string getVisualizationStatus() const;
    
    // Neuromodulation computation
    void computeNeuromodulation(double timestep);
    void computeDopamineSignal(double timestep);
    void computeCuriositySignal(double timestep);
    void computeNoveltySignal(double timestep);
    void computePredictionError(double timestep);
    void computeRewardPrediction(double timestep);
    void computeNetworkEffects(double timestep);
    void computePlasticityEffects(double timestep);
    void computeLearningEffects(double timestep);
    void computeSafetyChecks();
};

} // namespace nlm