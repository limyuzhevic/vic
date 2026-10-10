#pragma once

/**
 * Phase 6 Demo - Comprehensive Integration and Demonstration System
 * 
 * This system provides a complete demonstration of all Phase 6 capabilities:
 * - Real-time visualization of neural dynamics
 * - Performance benchmarking and validation
 * - Comprehensive system integration testing
 * - Development stage demonstration
 * - Learning and adaptation demonstration
 * - Memory consolidation and replay demonstration
 * - Action planning and execution demonstration
 * - Educational and research demonstration capabilities
 * 
 * Phase 6 Integration Features:
 * - Initialize all brain systems (neuromodulation, prediction, memory, cognition)
 * - Real-time visualization of neural activity and dynamics
 * - System performance monitoring and metrics collection
 * - Development stage progression demonstration
 * - Reward-based learning and adaptation
 * - Curiosity-driven exploration demonstration
 * - Prediction-based control demonstration
 * - Attention and working memory usage demonstration
 * - Episodic memory formation demonstration
 * - System coordination and interaction demonstration
 * 
 * Design Philosophy:
 * - Modular and extensible architecture
 * - Real-time data acquisition and visualization
 * - Comprehensive performance metrics
 * - Educational demonstration mode
 * - Research-grade benchmarking
 * - Thread-safe for concurrent operations
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/SimpleWorld.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "performance/Performance.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Real-time neural activity data structure
 */
struct NeuralActivityData {
    SimulationStep step;
    double wallClockTime;
    double avgFiringRate;
    double totalFiringRate;
    double avgSynapticWeight;
    double memoryUtilization;
    double attentionLevel;
    double curiosityLevel;
    double dopamineLevel;
    double rewardValue;
    size_t activeNeurons;
    size_t newConnections;
    size_t prunedConnections;
    float developmentalAge;
    std::string developmentalStage;
    std::vector<float> firingRateHistory;
    std::vector<float> rewardHistory;
    std::vector<size_t> connectionCountHistory;
};

/**
 * Performance metrics for system validation
 */
struct PerformanceMetrics {
    double initializationTime;
    double learningTime;
    double peakFiringRate;
    double avgFiringRate;
    double rewardEfficiency;
    double memoryEfficiency;
    double plasticityEfficiency;
    double convergenceTime;
    std::vector<double> stabilityMetrics;
    std::vector<double> learningCurves;
    std::vector<double> performanceTrajectories;
};

/**
 * Development stage progression data
 */
struct DevelopmentProgression {
    SimulationStep stageStartStep;
    SimulationStep stepsInStage;
    double stageAge;
    double plasticityModifier;
    double criticalPeriodProgress;
    std::vector<std::string> achievedMilestones;
    std::map<std::string, float> stageCharacteristics;
    std::map<std::string, float> stageCapabilities;
};

/**
 * Phase 6 comprehensive demonstration system
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Initialize all brain systems
     */
    bool initialize(const Phase6Config& config);
    
    /**
     * Run comprehensive demonstration
     */
    bool run(const Phase6Config& config);
    
    /**
     * Stop demonstration
     */
    void stop();
    
    /**
     * Check if demo is running
     */
    bool isRunning() const;
    
    /**
     * Get current neural activity data
     */
    NeuralActivityData getNeuralActivity() const;
    
    /**
     * Get performance metrics
     */
    PerformanceMetrics getPerformanceMetrics() const;
    
    /**
     * Get development progression data
     */
    DevelopmentProgression getDevelopmentProgression() const;
    
    /**
     * Get current system status
     */
    std::string getSystemStatus() const;
    
    /**
     * Get demonstration statistics
     */
    std::unordered_map<std::string, float> getStatistics() const;
    
    /**
     * Export neural activity data
     */
    bool exportNeuralData(const std::string& filename) const;
    
    /**
     * Export performance metrics
     */
    bool exportPerformanceMetrics(const std::string& filename) const;
    
    /**
     * Export development progression data
     */
    bool exportDevelopmentData(const std::string& filename) const;
    
    /**
     * Set demonstration mode
     */
    void setDemoMode(bool enabled);
    
    /**
     * Set visualization mode
     */
    void setVisualizationMode(bool enabled);
    
    /**
     * Set benchmarking mode
     */
    void setBenchmarkingMode(bool enabled);
    
    /**
     * Set real-time monitoring mode
     */
    void setRealTimeMonitoring(bool enabled);
    
    /**
     * Get demonstration progress (0.0 to 1.0)
     */
    double getProgress() const;
    
    /**
     * Force stage transition for demonstration
     */
    void forceStageTransition(DevelopmentalStage stage);
    
    /**
     * Trigger memory consolidation
     */
    void triggerMemoryConsolidation();
    
    /**
     * Trigger neural replay
     */
    void triggerNeuralReplay();
    
    /**
     * Trigger synaptogenesis
     */
    void triggerSynaptogenesis();
    
    /**
     * Trigger synaptic pruning
     */
    void triggerPruning();
    
    /**
     * Trigger maturation process
     */
    void triggerMaturation();
    
    /**
     * Get current developmental stage
     */
    DevelopmentalStage getCurrentStage() const;
    
    /**
     * Print system status report
     */
    void printStatusReport() const;
    
    /**
     * Print performance benchmarks
     */
    void printPerformanceReport() const;
    
    /**
     * Print development progression report
     */
    void printDevelopmentReport() const;

private:
    /**
     * Initialize brain systems
     */
    bool initializeBrainSystems(const Phase6Config& config);
    
    /**
     * Initialize visualization system
     */
    bool initializeVisualizationSystem();
    
    /**
     * Initialize performance monitoring
     */
    bool initializePerformanceMonitoring();
    
    /**
     * Run main demonstration loop
     */
    void runDemonstrationLoop(const Phase6Config& config);
    
    /**
     * Update neural activity data
     */
    void updateNeuralActivityData(const Brain* brain, AgentBrain* agentBrain);
    
    /**
     * Update performance metrics
     */
    void updatePerformanceMetrics(double timestep);
    
    /**
     * Update development progression
     */
    void updateDevelopmentProgression();
    
    /**
     * Update system statistics
     */
    void updateStatistics();
    
    /**
     * Process simulation step
     */
    void processStep(const Brain* brain, AgentBrain* agentBrain, SimpleWorld* world);
    
    /**
     * Process neural activity step
     */
    void processNeuralStep(Brain* brain);
    
    /**
     * Process development step
     */
    void processDevelopmentStep(Brain* brain, DevelopmentSystem* devSystem);
    
    /**
     * Process memory system step
     */
    void processMemoryStep(Brain* brain, AgentBrain* agentBrain);
    
    /**
     * Process neuromodulation step
     */
    void processNeuromodulationStep(AgentBrain* agentBrain);
    
    /**
     * Process prediction step
     */
    void processPredictionStep(Brain* brain, AgentBrain* agentBrain);
    
    /**
     * Process cognition step
     */
    void processCognitionStep(Brain* brain, AgentBrain* agentBrain);
    
    /**
     * Process action step
     */
    void processActionStep(AgentBrain* agentBrain, SimpleWorld* world);
    
    /**
     * Generate neural visualization data
     */
    void generateNeuralVisualizationData(const Brain* brain, NeuralActivityData& data);
    
    /**
     * Generate system status string
     */
    std::string generateSystemStatus() const;
    
    /**
     * Save checkpoint
     */
    bool saveCheckpoint(const std::string& filename);
    
    /**
     * Load checkpoint
     */
    bool loadCheckpoint(const std::string& filename);
    
    /**
     * Export metrics to CSV
     */
    bool exportMetricsToCSV(const std::string& filename) const;
    
    /**
     * Print progress update
     */
    void printProgressUpdate(const NeuralActivityData& activity) const;
    
    /**
     * Print stage progression
     */
    void printStageProgression(const DevelopmentProgression& devProg) const;
    
    /**
     * Print learning summary
     */
    void printLearningSummary() const;

private:
    // Core systems
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<DevelopmentSystem> developmentSystem_;
    std::shared_ptr<Phase6IntegratedExperiment> experiment_;
    
    // Visualization system
    std::shared_ptr<VisualizationInterface> visualization_;
    
    // Performance monitoring
    std::shared_ptr<Performance> performance_;
    
    // Neural plasticity systems
    std::shared_ptr<STDP> stdp_;
    std::shared_ptr<Hebbian> hebbian_;
    
    // Memory systems
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    
    // Neuromodulation systems
    std::shared_ptr<Neuromodulator> neuromodulator_;
    
    // Prediction systems
    std::shared_ptr<NeuralPrediction> predictionSystem_;
    
    // Cognitive systems
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<NeuralPlanner> neuralPlanner_;
    
    // Synaptic development systems
    std::shared_ptr<Synaptogenesis> synaptogenesis_;
    std::shared_ptr<Pruning> pruning_;
    std::shared_ptr<Maturation> maturation_;
    
    // Threading and synchronization
    mutable std::mutex dataMutex_;
    std::condition_variable stopCondition_;
    std::atomic<bool> running_;
    std::atomic<bool> demoMode_;
    std::atomic<bool> visualizationEnabled_;
    std::atomic<bool> benchmarkingEnabled_;
    std::atomic<bool> realTimeMonitoring_;
    
    // Data collection and storage
    std::vector<NeuralActivityData> neuralActivityHistory_;
    std::vector<PerformanceMetrics> performanceHistory_;
    std::vector<DevelopmentProgression> developmentHistory_;
    std::unordered_map<std::string, float> statistics_;
    
    // Timing and progression
    SimulationStep currentStep_;
    double wallClockTime_;
    double startTime_;
    double lastUpdateTime_;
    
    // State tracking
    NeuralActivityData currentActivity_;
    PerformanceMetrics currentPerformance_;
    DevelopmentProgression currentDevelopment_;
    
    // Configuration
    Phase6Config demoConfig_;
    
    // Resource tracking
    size_t totalStepsProcessed_;
    size_t totalNeuronsCreated_;
    size_t totalConnectionsCreated_;
    size_t totalConnectionsPruned_;
};