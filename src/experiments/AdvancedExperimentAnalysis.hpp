// Enhanced experiment analysis and development utilities
#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <memory>
#include <unordered_set>
#include <queue>
#include "Brain.hpp"
#include "Environment.hpp"
#include "Experiment.hpp"

namespace nlm {

// Advanced learning analysis metrics
struct LearningAnalysis {
    // Learning dynamics
    double learningRate;              // Rate of learning
    double convergenceSpeed;         // How quickly system converges
    double stability;                // Learning stability
    double explorationVsExploitation; // Balance between exploration and exploitation
    double noveltyDetection;         // Novelty detection capability
    double creditAssignment;         // Credit assignment effectiveness
    
    // Memory-related learning metrics
    double episodicMemoryUtilization; // Use of episodic memory
    double workingMemoryEfficacy;    // Effectiveness of working memory
    double associativeStrength;      // Associative learning strength
    
    // Neuromodulation effects
    double dopamineEffectiveness;    // Dopamine modulation impact
    double curiosityLevel;          // Exploration driven by curiosity
    double predictionErrorReduction; // Reduction in prediction error
    
    // Plasticity analysis
    double synapticWeightChange;     // Total synaptic weight change
    double structuralPlasticity;    // Structural plasticity effects
    double stdpEffectiveness;       // STDP learning effectiveness
    
    // Learning trajectory
    std::vector<double> learningCurve;  // Learning progression over time
    std::vector<double> performanceCurve; // Performance progression
    
    // Statistical measures
    double learningVariance;         // Variance in learning performance
    double learningConsistency;      // Consistency of learning
    double adaptability;             // Ability to adapt to changes
    
    LearningAnalysis() : learningRate(0.0), convergenceSpeed(0.0), stability(0.0),
                       explorationVsExploitation(0.0), noveltyDetection(0.0),
                       creditAssignment(0.0), episodicMemoryUtilization(0.0),
                       workingMemoryEfficacy(0.0), associativeStrength(0.0),
                       dopamineEffectiveness(0.0), curiosityLevel(0.0),
                       predictionErrorReduction(0.0), synapticWeightChange(0.0),
                       structuralPlasticity(0.0), stdpEffectiveness(0.0),
                       learningVariance(0.0), learningConsistency(0.0),
                       adaptability(0.0) {}
};

// Network development analysis
struct NetworkDevelopment {
    // Structural development
    double neuronGrowthRate;        // Rate of neuron growth
    double synapseFormationRate;    // Rate of synapse formation
    double connectivityIncrease;   // Increase in connectivity
    double modularization;         // Development of specialized modules
    double segregationIntegration; // Balance between segregation and integration
    
    // Functional development
    double functionalSpecialization; // Development of specialized functions
    double informationProcessingCapacity; // Capacity for information processing
    double computationalComplexity; // Computational complexity growth
    double efficiencyGain;         // Efficiency improvements over time
    
    // Dynamical development
    double stabilityOverTime;      // System stability over time
    double robustness;            // Robustness to perturbations
    double adaptability;          // Ability to adapt to new conditions
    double learningCapacity;      // Maximum learning capacity
    
    // Development trajectory
    std::vector<double> developmentTrajectory;  // Developmental path
    std::vector<double> functionalCapacity;    // Functional capacity over time
    
    // Statistical measures
    double developmentalVariability; // Variability in development
    double developmentalConsistency; // Consistency in development
    double developmentalMaturation; // Level of maturation
    
    NetworkDevelopment() : neuronGrowthRate(0.0), synapseFormationRate(0.0),
                         connectivityIncrease(0.0), modularization(0.0),
                         segregationIntegration(0.0), functionalSpecialization(0.0),
                         informationProcessingCapacity(0.0), computationalComplexity(0.0),
                         efficiencyGain(0.0), stabilityOverTime(0.0), robustness(0.0),
                         adaptability(0.0), learningCapacity(0.0),
                         developmentalVariability(0.0), developmentalConsistency(0.0),
                         developmentalMaturation(0.0) {}
};

// Advanced experiment configuration for simulation modes
struct AdvancedExperimentConfig {
    // Simulation mode selection
    std::string simulationMode;     // "standard", "enhanced", "research", "production"
    double simulationSpeed;        // Simulation speed multiplier
    bool enableRealTimeMonitoring;  // Enable real-time monitoring
    bool enableDetailedLogging;    // Enable detailed logging
    bool enableVisualization;      // Enable visualization output
    
    // Learning configuration
    std::string learningAlgorithm; // "stdp", "hebbian", "combined", "custom"
    double learningRate;           // Learning rate
    double explorationRate;       // Exploration rate
    double noveltyThreshold;      // Novelty detection threshold
    double rewardConsistency;      // Reward consistency requirement
    
    // Memory configuration
    bool enableEpisodicMemory;     // Enable episodic memory
    bool enableWorkingMemory;      // Enable working memory
    bool enableAssociativeMemory;  // Enable associative memory
    size_t episodicMemoryCapacity; // Capacity for episodic memory
    size_t workingMemoryCapacity;  // Capacity for working memory
    
    // Neuromodulation configuration
    bool enableDopamine;           // Enable dopamine neuromodulation
    bool enableCuriosity;         // Enable curiosity neuromodulation
    bool enableNovelty;            // Enable novelty detection
    bool enablePredictionError;   // Enable prediction error signaling
    
    // Development configuration
    bool enableDevelopment;        // Enable developmental processes
    double developmentSpeed;       // Developmental speed
    bool enableStructuralPlasticity; // Enable structural plasticity
    bool enableSynaptogenesis;     // Enable synaptogenesis
    bool enablePruning;            // Enable synaptic pruning
    
    // Analysis configuration
    bool enableLearningAnalysis;   // Enable learning analysis
    bool enableNetworkAnalysis;    // Enable network analysis
    bool enableDevelopmentAnalysis; // Enable development analysis
    bool enablePerformanceAnalysis; // Enable performance analysis
    
    // Output configuration
    std::string outputFormat;      // "json", "csv", "text", "xml"
    std::string outputDirectory;   // Directory for output files
    bool generateVisualizations;   // Generate visualizations
    bool generateReports;          // Generate analysis reports
    bool enableRealTimePlots;      // Enable real-time plotting
    
    AdvancedExperimentConfig() : simulationMode("enhanced"), simulationSpeed(1.0),
                                enableRealTimeMonitoring(true), enableDetailedLogging(true),
                                enableVisualization(true), learningAlgorithm("combined"),
                                learningRate(0.01), explorationRate(0.1), noveltyThreshold(0.5),
                                rewardConsistency(0.9), enableEpisodicMemory(true),
                                enableWorkingMemory(true), enableAssociativeMemory(true),
                                episodicMemoryCapacity(1000), workingMemoryCapacity(100),
                                enableDopamine(true), enableCuriosity(true),
                                enableNovelty(true), enablePredictionError(true),
                                enableDevelopment(true), developmentSpeed(0.1),
                                enableStructuralPlasticity(true), enableSynaptogenesis(true),
                                enablePruning(true), enableLearningAnalysis(true),
                                enableNetworkAnalysis(true), enableDevelopmentAnalysis(true),
                                enablePerformanceAnalysis(true), outputFormat("json"),
                                outputDirectory("./experiment_results"),
                                generateVisualizations(true), generateReports(true),
                                enableRealTimePlots(false) {}
};

// Advanced experiment runner with enhanced capabilities
class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner();
    ~AdvancedExperimentRunner();
    
    // Configuration
    void setConfig(const AdvancedExperimentConfig& config) { config_ = config; }
    AdvancedExperimentConfig getConfig() const { return config_; }
    
    // Advanced experiment setup
    bool setupAdvancedExperiment(std::shared_ptr<Experiment> experiment,
                                std::shared_ptr<Brain> brain,
                                std::shared_ptr<Environment> environment,
                                const std::string& mode = "enhanced");
    
    // Advanced simulation runs
    bool runAdvancedSimulation(std::shared_ptr<Experiment> experiment,
                              std::shared_ptr<Brain> brain,
                              std::shared_ptr<Environment> environment,
                              SimulationStep maxSteps,
                              const std::string& mode = "enhanced");
    
    // Learning analysis
    LearningAnalysis analyzeLearning(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain);
    
    // Network development analysis
    NetworkDevelopment analyzeNetworkDevelopment(std::shared_ptr<Experiment> experiment,
                                               std::shared_ptr<Brain> brain);
    
    // Advanced metrics collection
    struct AdvancedMetrics {
        std::map<std::string, double> basicMetrics;
        std::map<std::string, double> learningMetrics;
        std::map<std::string, double> networkMetrics;
        std::map<std::string, double> developmentMetrics;
        std::map<std::string, std::vector<double>> timeSeries;
        
        std::string toJSON() const;
        std::string toCSV() const;
        std::string toText() const;
    };
    
    // Advanced data export
    bool exportAdvancedResults(const std::string& filepath,
                              const AdvancedMetrics& metrics);
    
    // Visualization generation
    bool generateAdvancedVisualization(std::shared_ptr<Brain> brain,
                                       const std::string& outputPath);
    
    // Performance analysis
    double calculateLearningEfficiency(std::shared_ptr<Experiment> experiment);
    double calculateNetworkEfficiency(std::shared_ptr<Brain> brain);
    double calculateDevelopmentProgress(std::shared_ptr<Experiment> experiment);
    
    // Research mode simulations
    bool runResearchSimulation(std::shared_ptr<Experiment> experiment,
                              std::shared_ptr<Brain> brain,
                              std::shared_ptr<Environment> environment,
                              const std::string& hypothesis);
    
    // Production mode simulations
    bool runProductionSimulation(std::shared_ptr<Experiment> experiment,
                                std::shared_ptr<Brain> brain,
                                std::shared_ptr<Environment> environment,
                                SimulationStep maxSteps,
                                double qualityThreshold);
    
    // Quality assurance
    bool validateExperimentQuality(std::shared_ptr<Experiment> experiment);
    std::vector<std::string> getQualityIssues() const { return qualityIssues_; }
    
    // Reproducibility
    bool saveExperimentParameters(const std::string& filepath);
    bool loadExperimentParameters(const std::string& filepath);
    
    // Collaboration and sharing
    bool shareResults(const std::string& experimentId, const std::string& dataset);
    bool importResults(const std::string& experimentId);
    
    // Real-time monitoring
    void startRealTimeAnalysis();
    void stopRealTimeAnalysis();
    bool isAnalyzingRealTime() const { return realTimeAnalysis_; }
    
    // Machine learning integration
    bool integrateMachineLearning(const std::string& modelPath);
    std::vector<double> predictOutcomes(std::shared_ptr<Brain> brain);
    
    // Statistical analysis
    struct StatisticalAnalysis {
        double mean;
        double standardDeviation;
        double variance;
        double median;
        double mode;
        double skewness;
        double kurtosis;
        double confidenceInterval;
        double coefficientOfVariation;
        
        std::string toJSON() const;
        std::string toCSV() const;
    };
    
    StatisticalAnalysis performStatisticalAnalysis(const std::vector<double>& data);
    
    // Pattern detection
    std::vector<std::string> detectPatterns(const std::vector<double>& timeSeries);
    std::vector<double> extractFeatures(std::shared_ptr<Brain> brain);
    
    // Comparative analysis
    double compareWithBaseline(std::shared_ptr<Experiment> experiment,
                              const std::string& baselineName);
    std::vector<std::string> getComparativeMetrics(std::shared_ptr<Brain> brain);
    
    // Validation and verification
    bool validateExperimentResults(std::shared_ptr<Experiment> experiment);
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
private:
    AdvancedExperimentConfig config_;
    bool realTimeAnalysis_;
    std::vector<std::string> qualityIssues_;
    std::vector<std::string> validationErrors_;
    std::map<std::string, std::vector<double>> historicalData_;
    
    // Internal methods
    bool setupLearningInfrastructure(std::shared_ptr<Brain> brain);
    bool setupMemoryInfrastructure(std::shared_ptr<Brain> brain);
    bool setupNeuromodulationInfrastructure(std::shared_ptr<Brain> brain);
    bool setupDevelopmentInfrastructure(std::shared_ptr<Brain> brain);
    
    AdvancedMetrics collectAdvancedMetrics(std::shared_ptr<Experiment> experiment,
                                          std::shared_ptr<Brain> brain);
    
    LearningAnalysis calculateLearningAnalysis(std::shared_ptr<Experiment> experiment,
                                               std::shared_ptr<Brain> brain);
    
    NetworkDevelopment calculateNetworkDevelopment(std::shared_ptr<Experiment> experiment,
                                                   std::shared_ptr<Brain> brain);
    
    void analyzePerformance(std::shared_ptr<Experiment> experiment,
                           std::shared_ptr<Brain> brain);
    
    bool validateConfiguration();
    void generateQualityReport();
    void generateValidationReport();
    
    // File I/O
    bool writeJSONResults(const std::string& filepath,
                         const AdvancedMetrics& metrics);
    bool writeCSVResults(const std::string& filepath,
                        const AdvancedMetrics& metrics);
    bool writeTextResults(const std::string& filepath,
                         const AdvancedMetrics& metrics);
    
    // Utility functions
    std::string sanitizeFilename(const std::string& filename);
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    double calculateRegression(const std::vector<double>& x, const std::vector<double>& y);
};

// Enhanced metrics for learning analysis
class LearningMetrics {
public:
    LearningMetrics();
    ~LearningMetrics();
    
    // Basic learning metrics
    void recordLearningEvent(double timestamp, const std::string& eventType, double value);
    void recordPerformance(double timestamp, double performance);
    void recordBehavior(double timestamp, const std::string& behavior, double value);
    
    // Learning dynamics
    double calculateLearningRate(const std::string& metric = "performance");
    double calculateConvergenceSpeed();
    double calculateStability();
    double calculateExplorationExploitationBalance();
    
    // Memory-related learning
    double calculateEpisodicMemoryUtilization();
    double calculateWorkingMemoryEfficacy();
    double calculateAssociativeStrength();
    
    // Neuromodulation effects
    double calculateDopamineEffectiveness();
    double calculateCuriosityLevel();
    double calculateNoveltyDetection();
    double calculatePredictionErrorReduction();
    
    // Plasticity analysis
    double calculateSynapticWeightChange();
    double calculateStructuralPlasticity();
    double calculateSTDP effectiveness();
    
    // Learning trajectory analysis
    std::vector<double> calculateLearningCurve();
    std::vector<double> calculatePerformanceCurve();
    std::vector<double> calculateLearningVelocity();
    
    // Statistical analysis
    double calculateLearningVariance();
    double calculateLearningConsistency();
    double calculateAdaptability();
    
    // Export
    std::string toJSON() const;
    std::string toCSV() const;
    std::string toText() const;
    
    // Analysis
    std::string getLearningProfile() const;
    std::string getDevelopmentStage() const;
    std::string getPerformanceLevel() const;
    
private:
    struct LearningEvent {
        double timestamp;
        std::string eventType;
        double value;
        std::map<std::string, double> metadata;
    };
    
    std::vector<LearningEvent> events_;
    std::vector<double> performanceData_;
    std::vector<double> behaviorData_;
    
    double calculateRollingAverage(const std::vector<double>& data, size_t window);
    double calculateExponentialMovingAverage(const std::vector<double>& data, double alpha);
};

// Enhanced development analysis
class DevelopmentAnalyzer {
public:
    DevelopmentAnalyzer();
    ~DevelopmentAnalyzer();
    
    // Development metrics
    void recordDevelopmentEvent(double timestamp, const std::string& eventType, double value);
    void recordNetworkChange(double timestamp, double connectivity, double complexity);
    void recordFunctionalChange(double timestamp, double performance, double efficiency);
    
    // Structural development
    double calculateNeuronGrowthRate();
    double calculateSynapseFormationRate();
    double calculateConnectivityIncrease();
    double calculateModularization();
    double calculateSegregationIntegration();
    
    // Functional development
    double calculateFunctionalSpecialization();
    double calculateInformationProcessingCapacity();
    double calculateComputationalComplexity();
    double calculateEfficiencyGain();
    
    // Dynamical development
    double calculateStabilityOverTime();
    double calculateRobustness();
    double calculateAdaptability();
    double calculateLearningCapacity();
    
    // Development trajectory
    std::vector<double> calculateDevelopmentTrajectory();
    std::vector<double> calculateFunctionalCapacity();
    std::vector<double> calculateNetworkEvolution();
    
    // Statistical analysis
    double calculateDevelopmentalVariability();
    double calculateDevelopmentalConsistency();
    double calculateDevelopmentalMaturation();
    
    // Stage analysis
    std::string identifyDevelopmentStage();
    double calculateMaturationProgress();
    double calculateComplexityScore();
    
    // Export
    std::string toJSON() const;
    std::string toCSV() const;
    std::string toText() const;
    
    // Analysis
    std::string getDevelopmentProfile() const;
    std::string getEvolutionaryStage() const;
    std::string getMaturationLevel() const;
    
private:
    struct DevelopmentEvent {
        double timestamp;
        std::string eventType;
        double value;
        std::map<std::string, double> metrics;
    };
    
    std::vector<DevelopmentEvent> events_;
    std::vector<double> networkData_;
    std::vector<double> functionalData_;
    
    double calculateLinearRegression(const std::vector<double>& x, const std::vector<double>& y);
    double calculateExponentialFit(const std::vector<double>& x, const std::vector<double>& y);
    double calculateLogisticFit(const std::vector<double>& x, const std::vector<double>& y);
};

// Enhanced experiment database for research
class ExperimentDatabase {
public:
    ExperimentDatabase();
    ~ExperimentDatabase();
    
    // Experiment management
    bool addExperiment(std::shared_ptr<Experiment> experiment);
    bool removeExperiment(const std::string& name);
    bool updateExperiment(std::shared_ptr<Experiment> experiment);
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    std::vector<std::shared_ptr<Experiment>> getAllExperiments() const { return experiments_; }
    
    // Experiment search and filtering
    std::vector<std::shared_ptr<Experiment>> searchExperiments(const std::string& criteria);
    std::vector<std::shared_ptr<Experiment>> filterExperimentsByDate(double startDate, double endDate);
    std::vector<std::shared_ptr<Experiment>> filterExperimentsByMetrics(const std::string& metric, double minValue, double maxValue);
    
    // Experiment comparison
    double compareExperiments(std::shared_ptr<Experiment> exp1, std::shared_ptr<Experiment> exp2);
    std::vector<std::string> getComparativeMetrics(std::shared_ptr<Experiment> exp1, std::shared_ptr<Experiment> exp2);
    std::string generateComparisonReport(std::shared_ptr<Experiment> exp1, std::shared_ptr<Experiment> exp2);
    
    // Data management
    bool exportExperiments(const std::string& directory);
    bool importExperiments(const std::string& directory);
    bool backupExperiments(const std::string& backupPath);
    bool restoreExperiments(const std::string& backupPath);
    
    // Statistics and analysis
    size_t getExperimentCount() const { return experiments_.size(); }
    std::string getDatabaseStats() const;
    bool validateDatabase();
    void cleanupOldExperiments(double retentionDays);
    
    // Collaboration and sharing
    bool shareExperiment(const std::string& experimentName, const std::string& dataset);
    bool importSharedExperiment(const std::string& experimentId);
    std::vector<std::string> getSharedExperiments() const { return sharedExperiments_; }
    
    // Query and analytics
    std::vector<std::string> queryExperiments(const std::string& query);
    std::map<std::string, double> analyzeExperimentTrends(const std::string& experimentName);
    std::string generateAnalyticsReport();
    
private:
    std::vector<std::shared_ptr<Experiment>> experiments_;
    std::vector<std::string> sharedExperiments_;
    std::map<std::string, std::vector<double>> experimentMetrics_;
    std::map<std::string, std::vector<std::string>> experimentTags_;
    
    // Database file operations
    bool saveExperiment(std::shared_ptr<Experiment> experiment);
    bool loadExperiment(const std::string& name);
    bool deleteExperiment(const std::string& name);
    
    // Metadata management
    void updateExperimentMetadata(std::shared_ptr<Experiment> experiment);
    std::map<std::string, std::string> getExperimentMetadata(const std::string& name) const;
    
    // Search indexing
    void buildSearchIndex();
    std::vector<std::string> searchIndex_;
    
    // Analytics
    std::vector<std::string> getTrendingExperiments();
    std::map<std::string, double> calculateExperimentSuccessRates();
    std::string generatePerformanceLeaderboard();
};

} // namespace nlm

// Global function for easy access
inline nlm::AdvancedExperimentRunner& getAdvancedExperimentRunner() {
    static nlm::AdvancedExperimentRunner runner;
    return runner;
}

inline nlm::LearningMetrics& getLearningMetrics() {
    static nlm::LearningMetrics metrics;
    return metrics;
}

inline nlm::DevelopmentAnalyzer& getDevelopmentAnalyzer() {
    static nlm::DevelopmentAnalyzer analyzer;
    return analyzer;
}

inline nlm::ExperimentDatabase& getExperimentDatabase() {
    static nlm::ExperimentDatabase database;
    return database;
}