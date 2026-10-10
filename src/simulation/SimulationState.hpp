#pragma once

/**
 * Simulation State for NLM
 * 
 * Manages simulation state including step history, metrics, and checkpointing
 * 
 * Features:
 * - Step history recording
 * - Performance metrics tracking
 * - Checkpoint management
 * - State serialization
 * - State comparison
 * - State analysis
 * 
 * Usage:
 * ```cpp
 * #include <nlm/simulation/SimulationState.hpp>
 * 
 * int main() {
 *     auto state = nlm::createSimulationState();
 *     
 *     // Record simulation steps
 *     for (int step = 0; step < 100; ++step) {
 *         // Run simulation step
 *         // ...
 *         
 *         // Record state
 *         state->recordStep(step, currentTime);
 *     }
 *     
 *     // Save state to file
 *     state->saveToFile("simulation_state.bin");
 *     
 *     // Generate report
 *     std::string report = state->generateReport();
 * }
 * ```
 */

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nlm {

/**
 * Simulation step record
 */
struct StepRecord {
    int step;                           // Step number
    double simulationTime;              // Simulation time at this step
    double wallClockTime;               // Wall clock time at this step
    double cpuTime;                     // CPU time at this step
    size_t activeNeurons;               // Number of active neurons
    size_t firingNeurons;                // Number of firing neurons
    double averageFiringRate;            // Average firing rate
    double excitationInhibitionRatio;   // Excitation/inhibition ratio
    double totalSpikeCount;              // Total spike count
    double energyLevel;                  // Energy level
    double curiosityLevel;               // Curiosity level
    double noveltyLevel;                 // Novelty level
    double dopamineLevel;                // Dopamine level
    double predictionError;             // Prediction error
    std::map<std::string, double> customMetrics; // Custom metrics
    
    StepRecord()
        : step(0), simulationTime(0.0), wallClockTime(0.0), cpuTime(0.0),
          activeNeurons(0), firingNeurons(0), averageFiringRate(0.0),
          excitationInhibitionRatio(0.0), totalSpikeCount(0.0), energyLevel(0.0),
          curiosityLevel(0.0), noveltyLevel(0.0), dopamineLevel(0.0),
          predictionError(0.0) {}
    
    StepRecord(int step, double simTime, double wallTime, double cpuTime)
        : step(step), simulationTime(simTime), wallClockTime(wallTime), cpuTime(cpuTime),
          activeNeurons(0), firingNeurons(0), averageFiringRate(0.0),
          excitationInhibitionRatio(0.0), totalSpikeCount(0.0), energyLevel(0.0),
          curiosityLevel(0.0), noveltyLevel(0.0), dopamineLevel(0.0),
          predictionError(0.0) {}
};

/**
 * Simulation state management
 */
class SimulationState {
public:
    SimulationState();
    ~SimulationState();
    
    // Recording functionality
    void recordStep(int step, double simulationTime, 
                   double wallClockTime = 0.0, double cpuTime = 0.0);
    
    void updateStepMetrics(int step, size_t activeNeurons, size_t firingNeurons,
                           double averageFiringRate, double eir,
                           double totalSpikeCount, double energyLevel,
                           double curiosityLevel, double noveltyLevel,
                           double dopamineLevel, double predictionError);
    
    void addCustomMetric(int step, const std::string& name, double value);
    
    // State access
    const StepRecord& getStepRecord(int step) const;
    int getFirstStep() const { return steps_.empty() ? -1 : steps_.front().step; }
    int getLastStep() const { return steps_.empty() ? -1 : steps_.back().step; }
    int getStepCount() const { return static_cast<int>(steps_.size()); }
    bool hasStep(int step) const;
    
    // Time-related functionality
    double getSimulationTime(int step) const;
    double getWallClockTime(int step) const;
    double getCPUTime(int step) const;
    double getStepDuration(int step) const;
    
    // Metrics access
    size_t getActiveNeurons(int step) const;
    size_t getFiringNeurons(int step) const;
    double getAverageFiringRate(int step) const;
    double getExcitationInhibitionRatio(int step) const;
    double getTotalSpikeCount(int step) const;
    double getEnergyLevel(int step) const;
    double getCuriosityLevel(int step) const;
    double getNoveltyLevel(int step) const;
    double getDopamineLevel(int step) const;
    double getPredictionError(int step) const;
    double getCustomMetric(int step, const std::string& name) const;
    
    // Analysis functionality
    double calculateAverageFiringRate(int startStep, int endStep) const;
    double calculateAverageEIR(int startStep, int endStep) const;
    double calculateAverageSpikeCount(int startStep, int endStep) const;
    double calculateAverageEnergyLevel(int startStep, int endStep) const;
    double calculateAverageCuriosityLevel(int startStep, int endStep) const;
    double calculateAverageNoveltyLevel(int startStep, int endStep) const;
    
    std::vector<int> findSteps(double minFiringRate, double maxFiringRate) const;
    std::vector<int> findSteps(double minSpikeCount, double maxSpikeCount) const;
    std::vector<int> findSteps(double minEnergyLevel, double maxEnergyLevel) const;
    
    // State comparison
    int compareSteps(const SimulationState& other) const;
    bool isIdentical(const SimulationState& other) const;
    bool isSimilar(const SimulationState& other, double tolerance) const;
    
    // Serialization
    bool saveToFile(const std::string& filePath) const;
    bool loadFromFile(const std::string& filePath);
    std::string toJSON() const;
    bool fromJSON(const std::string& jsonStr);
    
    // Report generation
    std::string generateReport() const;
    std::string generateStepReport(int step) const;
    std::string generateMetricsReport() const;
    std::string generateTrendsReport() const;
    
    // State manipulation
    void clear();
    void reset();
    void pruneSteps(int maxSteps);
    void interpolateSteps(int targetStepCount);
    
    // Statistical analysis
    struct Statistics {
        double mean = 0.0;
        double std = 0.0;
        double min = 0.0;
        double max = 0.0;
        double median = 0.0;
        double q1 = 0.0;  // 25th percentile
        double q3 = 0.0;  // 75th percentile
        
        Statistics() {}
    };
    
    Statistics calculateStatistics(int step, const std::string& metricName) const;
    Statistics calculateStatistics(int startStep, int endStep, 
                                   const std::string& metricName) const;
    
    // Trending analysis
    struct Trend {
        std::string metricName;
        double slope;        // Rate of change per step
        double rSquared;     // Goodness of fit
        bool significant;    // Whether trend is statistically significant
        std::string direction; // "increasing", "decreasing", or "stable"
        
        Trend() : slope(0.0), rSquared(0.0), significant(false), direction("stable") {}
    };
    
    std::vector<Trend> analyzeTrends(int startStep, int endStep) const;
    
    // Pattern detection
    enum class PatternType {
        NONE,
        PEAK,
        VALLEY,
        TREND,
        SEASONAL,
        NOISE
    };
    
    struct Pattern {
        int step;                      // Step where pattern starts
        PatternType type;              // Type of pattern
        double confidence;             // Confidence in pattern detection (0.0 to 1.0)
        double strength;               // Strength of pattern (0.0 to 1.0)
        std::map<std::string, double> parameters; // Pattern parameters
        
        Pattern() : step(0), type(PatternType::NONE), confidence(0.0), strength(0.0) {}
    };
    
    std::vector<Pattern> detectPatterns(int startStep, int endStep) const;
    
    // Anomaly detection
    bool detectAnomaly(int step, const std::string& metricName, 
                      double threshold = 2.0) const;
    std::vector<int> detectAnomalies(const std::string& metricName, 
                                    double threshold = 2.0) const;
    
private:
    std::vector<StepRecord> steps_;
    std::map<int, std::map<std::string, double>> customMetrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Helper functions
    double calculateLinearRegression(const std::vector<double>& x, 
                                     const std::vector<double>& y,
                                     double& slope, double& intercept) const;
    
    double calculateStandardDeviation(const std::vector<double>& values, 
                                      double mean) const;
    
    double calculatePercentile(const std::vector<double>& values, 
                               double percentile) const;
    
    PatternType identifyPattern(const std::vector<double>& values) const;
    
    bool detectOutlier(const std::vector<double>& values, int index,
                      double threshold) const;
};

// Factory functions
std::unique_ptr<SimulationState> createSimulationState();

} // namespace nlm
