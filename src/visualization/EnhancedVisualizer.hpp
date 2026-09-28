#ifndef NLM_VISUALIZATION_H
#define NLM_VISUALIZATION_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace nlm {
namespace visualization {

/**
 * @brief Enhanced visualization and debugging utilities for NLM
 * 
 * This module provides comprehensive visualization and debugging capabilities
 * for the NLM neural simulation framework, including:
 * - Real-time neural activity visualization
 * - Brain state analysis and visualization
 * - Learning progress tracking and visualization
 * - Performance monitoring and profiling
 * - Export utilities for external visualization tools
 * - Interactive debugging interfaces
 */
class EnhancedVisualizer {
public:
    /**
     * @brief Visualization output format
     */
    enum OutputFormat {
        ASCII_ART,
        JSON,
        CSV,
        PLAIN_TEXT,
        HTML,
        SVG,
        NETWORKX,
        CUSTOM
    };
    
    /**
     * @brief Visualization data structure
     */
    struct VisualizationData {
        std::string name;
        std::string description;
        std::map<std::string, std::vector<float>> timeSeriesData;
        std::map<std::string, std::vector<std::vector<float>>> matrixData;
        std::map<std::string, std::string> metadata;
        double timestamp;
        
        VisualizationData(const std::string& name, const std::string& description)
            : name(name), description(description), timestamp(0.0) {}
    };
    
    /**
     * @brief Neural activity visualizer
     */
    class NeuralActivityVisualizer {
    public:
        struct NeuronActivity {
            NeuronId neuronId;
            NeuronType type;
            float firingRate;
            float membranePotential;
            float synapticConductance;
            float lastSpikeTime;
            Timestamp lastUpdate;
            
            NeuronActivity(NeuronId id, NeuronType t) 
                : neuronId(id), type(t), firingRate(0.0f), membranePotential(0.0f),
                  synapticConductance(0.0f), lastSpikeTime(-1.0f), lastUpdate(0.0) {}
        };
        
        struct BrainActivitySnapshot {
            size_t step;
            double time;
            float totalSpikes;
            float averageFiringRate;
            float excitationInhibitionRatio;
            size_t firingNeuronCount;
            size_t activeSynapseCount;
            std::vector<NeuronActivity> neuronActivities;
            std::vector<std::vector<float>> connectivityMatrix;
            
            BrainActivitySnapshot(size_t step, double time)
                : step(step), time(time), totalSpikes(0.0f), averageFiringRate(0.0f),
                  excitationInhibitionRatio(1.0f), firingNeuronCount(0), activeSynapseCount(0) {}
        };
        
        NeuralActivityVisualizer(size_t maxHistorySize = 1000);
        
        /**
         * @brief Capture current brain activity
         */
        BrainActivitySnapshot captureBrainActivity(std::shared_ptr<Brain> brain);
        
        /**
         * @brief Add activity data to history
         */
        void addActivity(const BrainActivitySnapshot& snapshot);
        
        /**
         * @brief Get activity history
         */
        std::vector<BrainActivitySnapshot> getActivityHistory() const;
        
        /**
         * @brief Get activity for specific time range
         */
        std::vector<BrainActivitySnapshot> getActivityInRange(
            double startTime, double endTime) const;
        
        /**
         * @brief Generate neural network visualization
         */
        std::string generateNetworkVisualization(OutputFormat format = OutputFormat::ASCII_ART,
                                                const std::string& style = "default");
        
        /**
         * @brief Generate activity time series visualization
         */
        std::string generateActivityTimeSeriesVisualization(
            OutputFormat format = OutputFormat::CSV,
            const std::string& metrics = "firing_rate,total_spikes");
        
        /**
         * @brief Generate synaptic strength visualization
         */
        std::string generateSynapticVisualization(
            OutputFormat format = OutputFormat::HTML,
            bool showConnections = true,
            bool showWeights = true);
        
        /**
         * @brief Generate development stage visualization
         */
        std::string generateDevelopmentVisualization(
            OutputFormat format = OutputFormat::ASCII_ART);
        
        /**
         * @brief Generate learning progress visualization
         */
        std::string generateLearningProgressVisualization(
            OutputFormat format = OutputFormat::HTML);
        
        /**
         * @brief Get connectivity analysis
         */
        py::dict getConnectivityAnalysis() const;
        
        /**
         * @brief Get firing statistics
         */
        py::dict getFiringStatistics() const;
        
        /**
         * @brief Get developmental statistics
         */
        py::dict getDevelopmentStatistics() const;
        
        /**
         * @brief Find active regions in brain
         */
        std::vector<RegionId> findActiveRegions(std::shared_ptr<Brain> brain) const;
        
        /**
         * @brief Analyze neural ensembles
         */
        py::dict analyzeNeuralEnsembles(std::shared_ptr<Brain> brain) const;
        
        /**
         * @brief Export data to file
         */
        bool exportToFile(const std::string& filepath, OutputFormat format = OutputFormat::CSV);
        
        /**
         * @brief Clear history
         */
        void clear();
        
        /**
         * @brief Get data summary
         */
        std::string getSummary() const;
        
    private:
        std::vector<BrainActivitySnapshot> activityHistory_;
        size_t maxHistorySize_;
        mutable std::mutex historyMutex_;
        
        std::string generateASCIIArtNetworkVisualization() const;
        std::string generateJSONNetworkVisualization() const;
        std::string generateHTMLNetworkVisualization() const;
        std::string generateCSVActivityVisualization() const;
        std::string generateMatrixVisualization() const;
        
        std::vector<float> extractFiringRates(const BrainActivitySnapshot& snapshot) const;
        std::vector<float> extractMembranePotentials(const BrainActivitySnapshot& snapshot) const;
        std::vector<float> extractSynapticWeights(const BrainActivitySnapshot& snapshot) const;
    };
    
    /**
     * @brief Learning progress tracker
     */
    class LearningProgressTracker {
    public:
        struct LearningMetrics {
            size_t step;
            double time;
            float learningRate;
            float performance;
            float curiosityLevel;
            float noveltyLevel;
            float predictionError;
            float explorationExploitationBalance;
            float memoryRetention;
            float generalizationAbility;
            
            LearningMetrics(size_t step, double time)
                : step(step), time(time), learningRate(0.0f), performance(0.0f),
                  curiosityLevel(0.0f), noveltyLevel(0.0f), predictionError(0.0f),
                  explorationExploitationBalance(0.5f), memoryRetention(0.0f),
                  generalizationAbility(0.0f) {}
        };
        
        LearningProgressTracker();
        
        /**
         * @brief Record learning metrics
         */
        void recordMetrics(const LearningMetrics& metrics);
        
        /**
         * @brief Update metrics from brain and agent
         */
        void updateMetrics(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                          size_t step, double time);
        
        /**
         * @brief Get learning history
         */
        std::vector<LearningMetrics> getLearningHistory() const;
        
        /**
         * @brief Get learning phases
         */
        std::vector<std::string> identifyLearningPhases() const;
        
        /**
         * @brief Generate learning curve
         */
        std::string generateLearningCurve(OutputFormat format = OutputFormat::CSV);
        
        /**
         * @brief Generate performance metrics visualization
         */
        std::string generatePerformanceVisualization(
            OutputFormat format = OutputFormat::HTML);
        
        /**
         * @brief Generate exploration-exploitation analysis
         */
        std::string generateExplorationExploitationAnalysis(
            OutputFormat format = OutputFormat::ASCII_ART);
        
        /**
         * @brief Calculate learning rate adaptation
         */
        float calculateLearningRateAdaptation() const;
        
        /**
         * @brief Calculate curiosity growth
         */
        float calculateCuriosityGrowth() const;
        
        /**
         * @brief Calculate novelty processing efficiency
         */
        float calculateNoveltyProcessingEfficiency() const;
        
        /**
         * @brief Get overall learning score
         */
        float getOverallLearningScore() const;
        
        /**
         * @brief Export metrics to file
         */
        bool exportMetrics(const std::string& filepath);
        
        /**
         * @brief Clear history
         */
        void clear();
        
        /**
         * @brief Get learning summary
         */
        std::string getSummary() const;
        
    private:
        std::vector<LearningMetrics> learningHistory_;
        mutable std::mutex historyMutex_;
        
        std::string generateASCIIArtLearningCurve() const;
        std::string generateJSONLearningMetrics() const;
        std::string generateHTMLLearningDashboard() const;
        
        float calculateMeanPerformance() const;
        float calculatePerformanceTrend() const;
        float calculateStabilityScore() const;
    };
    
    /**
     * @brief Performance profiler
     */
    class PerformanceProfiler {
    public:
        struct PerformanceMetrics {
            size_t step;
            double time;
            double simulationTime;
            double brainStepTime;
            double totalSynapseCount;
            double totalNeuronCount;
            double memoryUsage;
            double cpuUsage;
            double networkActivity;
            size_t spikeEventsProcessed;
            
            PerformanceMetrics(size_t step, double time)
                : step(step), time(time), simulationTime(0.0), brainStepTime(0.0),
                  totalSynapseCount(0.0), totalNeuronCount(0.0), memoryUsage(0.0),
                  cpuUsage(0.0), networkActivity(0.0), spikeEventsProcessed(0) {}
        };
        
        PerformanceProfiler();
        
        /**
         * @brief Start performance measurement
         */
        void startMeasurement();
        
        /**
         * @brief Record performance metrics
         */
        void recordMetrics(const PerformanceMetrics& metrics);
        
        /**
         * @brief Get performance history
         */
        std::vector<PerformanceMetrics> getPerformanceHistory() const;
        
        /**
         * @brief Generate performance report
         */
        std::string generatePerformanceReport(OutputFormat format = OutputFormat::HTML);
        
        /**
         * @brief Generate benchmark results
         */
        std::string generateBenchmarkResults(OutputFormat format = OutputFormat::CSV);
        
        /**
         * @brief Get performance statistics
         */
        py::dict getPerformanceStatistics() const;
        
        /**
         * @brief Get bottlenecks
         */
        std::vector<std::string> identifyBottlenecks() const;
        
        /**
         * @brief Get optimization recommendations
         */
        std::vector<std::string> getOptimizationRecommendations() const;
        
        /**
         * @brief Export to file
         */
        bool exportToFile(const std::string& filepath);
        
        /**
         * @brief Clear history
         */
        void clear();
        
    private:
        std::vector<PerformanceMetrics> performanceHistory_;
        mutable std::mutex historyMutex_;
        std::chrono::high_resolution_clock::time_point lastStartTime_;
        
        std::string generateASCIIArtPerformanceChart() const;
        std::string generateJSONPerformanceData() const;
        std::string generateHTMLPerformanceDashboard() const;
        
        double calculateAverageSimulationTime() const;
        double calculateAverageBrainStepTime() const;
        double calculatePerformanceScaling() const;
    };
    
    /**
     * @brief Debug console and utilities
     */
    class DebugConsole {
    public:
        DebugConsole();
        
        /**
         * @brief Log message
         */
        void log(const std::string& message, const std::string& level = "INFO");
        
        /**
         * @brief Debug brain state
         */
        void debugBrain(std::shared_ptr<Brain> brain);
        
        /**
         * @brief Debug agent state
         */
        void debugAgent(std::shared_ptr<AgentBrain> agent);
        
        /**
         * @brief Debug world state
         */
        void debugWorld(std::shared_ptr<SimpleWorld> world);
        
        /**
         * @brief Get debug log
         */
        std::vector<std::string> getDebugLog() const;
        
        /**
         * @brief Clear debug log
         */
        void clearDebugLog();
        
        /**
         * @brief Set log level
         */
        void setLogLevel(const std::string& level);
        
        /**
         * @brief Get current log level
         */
        std::string getLogLevel() const;
        
    private:
        std::vector<std::string> debugLog_;
        std::string logLevel_;
        mutable std::mutex logMutex_;
    };
    
    // Main interface
    EnhancedVisualizer();
    
    /**
     * @brief Create neural activity visualizer
     */
    static NeuralActivityVisualizer createNeuralActivityVisualizer(size_t maxHistorySize = 1000);
    
    /**
     * @brief Create learning progress tracker
     */
    static LearningProgressTracker createLearningProgressTracker();
    
    /**
     * @brief Create performance profiler
     */
    static PerformanceProfiler createPerformanceProfiler();
    
    /**
     * @brief Create debug console
     */
    static DebugConsole createDebugConsole();
    
    /**
     * @brief Run comprehensive visualization
     */
    void runComprehensiveVisualization(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t steps,
        double timestep = 0.1);
    
    /**
     * @brief Generate integrated report
     */
    std::string generateIntegratedReport(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Export all data to files
     */
    bool exportAllData(const std::string& outputDirectory);
    
    /**
     * @brief Get visualization summary
     */
    py::dict getVisualizationSummary() const;
    
    /**
     * @brief Reset visualizers
     */
    void reset();
    
private:
    NeuralActivityVisualizer neuralVisualizer_;
    LearningProgressTracker learningTracker_;
    PerformanceProfiler profiler_;
    DebugConsole debugConsole_;
    
    std::vector<std::chrono::high_resolution_clock::time_point> timestamps_;
    size_t simulationStep_;
};

/**
 * @brief Factory functions for visualization
 */
inline std::shared_ptr<EnhancedVisualizer> createEnhancedVisualizer() {
    return std::make_shared<EnhancedVisualizer>();
}

/**
 * @brief Run visualization session
 */
inline py::dict runVisualizationSession(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<SimpleWorld> world,
    size_t steps,
    double timestep = 0.1) {
    
    auto visualizer = createEnhancedVisualizer();
    visualizer->runComprehensiveVisualization(brain, agent, world, steps, timestep);
    
    return visualizer->getVisualizationSummary();
}

/**
 * @brief Create neural activity visualizer
 */
inline EnhancedVisualizer::NeuralActivityVisualizer createNeuralActivityVisualizer(size_t maxHistorySize) {
    return EnhancedVisualizer::NeuralActivityVisualizer(maxHistorySize);
}

/**
 * @brief Create learning progress tracker
 */
inline EnhancedVisualizer::LearningProgressTracker createLearningProgressTracker() {
    return EnhancedVisualizer::LearningProgressTracker();
}

/**
 * @brief Create performance profiler
 */
inline EnhancedVisualizer::PerformanceProfiler createPerformanceProfiler() {
    return EnhancedVisualizer::PerformanceProfiler();
}

/**
 * @brief Create debug console
 */
inline EnhancedVisualizer::DebugConsole createDebugConsole() {
    return EnhancedVisualizer::DebugConsole();
}

} // namespace visualization
} // namespace nlm

#endif // NLM_VISUALIZATION_H
