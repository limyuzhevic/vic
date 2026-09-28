#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ExperimentMetrics.hpp"

namespace nlm {

/**
 * @brief Advanced neural activity analyzer class
 * 
 * This class provides comprehensive analysis of neural activity patterns, including
 * firing pattern analysis, oscillation detection, network cohesion computation,
 * neuron identification by activity, functional connectivity matrix computation,
 * and significant event detection (bursts, silent periods).
 * 
 * Designed for:
 * - Statistical analysis - Hypothesis testing and validation
 * - Machine learning - Activity patterns as ML features
 * - Performance analysis - Bottleneck identification and optimization
 * - Research visualization - Analysis for visualization
 */
class NeuralActivityAnalyzer {
public:
    /**
     * @brief Analysis window configuration
     */
    struct AnalysisWindow {
        SimulationStep startStep;
        SimulationStep endStep;
        Timestamp startTime;
        Timestamp endTime;
        bool useRelativeTime;
        
        AnalysisWindow()
            : startStep(0), endStep(0), startTime(0.0), endTime(0.0), useRelativeTime(true) {}
        
        AnalysisWindow(SimulationStep start, SimulationStep end)
            : startStep(start), endStep(end), startTime(0.0), endTime(0.0), useRelativeTime(true) {}
        
        AnalysisWindow(Timestamp start, Timestamp end)
            : startStep(0), endStep(0), startTime(start), endTime(end), useRelativeTime(false) {}
    };

    /**
     * @brief Oscillation detection result
     */
    struct OscillationResult {
        float frequency;
        float amplitude;
        float power;
        float phase;
        std::vector<SimulationStep> occurrenceSteps;
        float confidence;
    };

    /**
     * @brief Network cohesion result
     */
    struct CohesionResult {
        float overallCohesion;
        float withinRegionCohesion;
        float betweenRegionCohesion;
        std::map<RegionId, float> regionCohesion;
        std::map<PopulationId, float> populationCohesion;
        float temporalCohesion;
    };

    /**
     * @brief Event detection result
     */
    struct EventResult {
        enum class EventType {
            Burst,
            SilentPeriod,
            Transition,
            Synchronization,
            Desynchronization
        };
        
        EventType type;
        SimulationStep startStep;
        SimulationStep endStep;
        Timestamp startTime;
        Timestamp endTime;
        float intensity;
        float duration;
        std::vector<NeuronId> involvedNeurons;
        float significance;
    };

    /**
     * @brief Connectivity matrix result
     */
    struct ConnectivityMatrixResult {
        std::vector<std::vector<float>> functionalConnectivity;
        std::vector<std::vector<float>> effectiveConnectivity;
        std::vector<std::vector<float>> GrangerCausality;
        std::vector<std::vector<float>> transferEntropy;
        std::map<std::string, float> connectivityStats;
        SimulationStep windowSize;
    };

    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the brain instance to analyze
     * @param analysisWindow Analysis window configuration
     */
    NeuralActivityAnalyzer(
        std::shared_ptr<Brain> brain,
        const AnalysisWindow& analysisWindow = AnalysisWindow()
    );

    /**
     * @brief Destructor
     */
    ~NeuralActivityAnalyzer();

    /**
     * @brief Copy constructor
     */
    NeuralActivityAnalyzer(const NeuralActivityAnalyzer& other);

    /**
     * @brief Copy assignment operator
     */
    NeuralActivityAnalyzer& operator=(const NeuralActivityAnalyzer& other);

    /**
     * @brief Move constructor
     */
    NeuralActivityAnalyzer(NeuralActivityAnalyzer&& other) noexcept;

    /**
     * @brief Move assignment operator
     */
    NeuralActivityAnalyzer& operator=(NeuralActivityAnalyzer&& other) noexcept;

    /**
     * @brief Get the brain being analyzed
     * 
     * @return Shared pointer to the brain instance
     */
    std::shared_ptr<Brain> getBrain() const;

    /**
     * @brief Set analysis window
     * 
     * @param window Analysis window configuration
     */
    void setAnalysisWindow(const AnalysisWindow& window);

    /**
     * @brief Get analysis window
     * 
     * @return Current analysis window
     */
    AnalysisWindow getAnalysisWindow() const;

    /**
     * @brief Set analysis parameters
     * 
     * @param params Map of parameter names to values
     */
    void setAnalysisParameters(const std::map<std::string, float>& params);

    /**
     * @brief Get analysis parameters
     * 
     * @return Current analysis parameters
     */
    std::map<std::string, float> getAnalysisParameters() const;

    /**
     * @brief Analyze neuron firing patterns
     * 
     * Analyzes the firing patterns of neurons in the network, identifying regularity,
     * burstiness, bursting frequency, and other statistical properties.
     * 
     * @param neuronIds Optional list of specific neuron IDs to analyze (empty for all)
     * @return Map from neuron IDs to firing pattern analysis results
     */
    std::map<NeuronId, std::map<std::string, float>> analyzeFiringPatterns(
        const std::vector<NeuronId>& neuronIds = std::vector<NeuronId>()
    );

    /**
     * @brief Detect oscillatory activity
     * 
     * Detects oscillatory activity in the neural network using Fourier analysis,
     * wavelet transform, and other signal processing techniques. Can detect
     * oscillations at different frequency bands (gamma, beta, alpha, theta, delta).
     * 
     * @param frequencyBand Minimum frequency for oscillation detection
     * @param minAmplitude Minimum amplitude for oscillation detection
     * @return Vector of oscillation detection results
     */
    std::vector<OscillationResult> detectOscillations(
        float frequencyBand = 0.1f,
        float minAmplitude = 0.01f
    );

    /**
     * @brief Compute network cohesion
     * 
     * Computes network cohesion metrics, including overall cohesion, within-region
     * cohesion, between-region cohesion, and temporal cohesion. Measures how
     * well-connected and integrated the network is.
     * 
     * @return Network cohesion analysis results
     */
    CohesionResult computeCohesion();

    /**
     * @brief Identify neurons by activity
     * 
     * Identifies specific neurons based on their activity patterns, firing rates,
     * or membership in specific activity patterns. Can identify neurons that are
     * outliers, leaders, or followers in the network.
     * 
     * @param activityThreshold Activity threshold for identification
     * @param patternSimilarity Threshold for pattern similarity
     * @return Map from neuron IDs to activity scores and classifications
     */
    std::map<NeuronId, std::map<std::string, float>> identifyNeurons(
        float activityThreshold = 0.5f,
        float patternSimilarity = 0.8f
    );

    /**
     * @brief Compute functional connectivity matrix
     * 
     * Computes the functional connectivity matrix between neurons or populations,
     * using Pearson correlation, mutual information, or other connectivity measures.
     * Also computes effective connectivity and causal relationships.
     * 
     * @param connectivityType Type of connectivity to compute
     * @param windowSize Window size for time-varying connectivity (0 for static)
     * @return Functional connectivity analysis results
     */
    ConnectivityMatrixResult computeConnectivityMatrix(
        const std::string& connectivityType = "correlation",
        SimulationStep windowSize = 100
    );

    /**
     * @brief Detect significant events
     * 
     * Detects significant events in neural activity, including bursts, silent periods,
     * transitions, synchronization, and desynchronization events. Events are
     * detected using change-point detection, thresholding, and pattern matching.
     * 
     * @param eventTypes Types of events to detect
     * @param significanceThreshold Significance threshold for event detection
     * @return Vector of detected event results
     */
    std::vector<EventResult> detectEvents(
        const std::vector<EventResult::EventType>& eventTypes = std::vector<EventResult::EventType>(),
        float significanceThreshold = 0.95f
    );

    /**
     * @brief Compute time-varying activity statistics
     * 
     * Computes time-varying activity statistics including firing rate, synchrony,
     * burst index, and other dynamic measures over the analysis window.
     * 
     * @param windowSize Window size for sliding window analysis
     * @return Map from time windows to activity statistics
     */
    std::map<SimulationStep, std::map<std::string, float>> computeTimeVaryingStats(
        SimulationStep windowSize = 50
    );

    /**
     * @brief Cluster neurons by activity patterns
     * 
     * Clusters neurons based on their activity patterns using hierarchical clustering,
     * k-means clustering, or other clustering algorithms. Identifies groups of
     * neurons with similar activity patterns.
     * 
     * @param numClusters Number of clusters to identify
     * @param distanceMetric Distance metric for clustering
     * @return Map from cluster IDs to neuron IDs in each cluster
     */
    std::map<size_t, std::vector<NeuronId>> clusterNeurons(
        size_t numClusters = 5,
        const std::string& distanceMetric = "euclidean"
    );

    /**
     * @brief Compute network entropy
     * 
     * Computes various entropy measures for the neural network, including Shannon
     * entropy of firing patterns, mutual information between neuron pairs,
     * and network complexity measures.
     * 
     * @return Map of entropy measures and their values
     */
    std::map<std::string, float> computeNetworkEntropy();

    /**
     * @brief Generate activity profile
     * 
     * Generates a comprehensive activity profile for the network, including
     * summary statistics, activity patterns, and statistical tests.
     * 
     * @return Map containing all activity profile metrics
     */
    std::map<std::string, float> generateActivityProfile();

    /**
     * @brief Export analysis results
     * 
     * @param filepath Path to save analysis results (JSON format)
     * @return true if export was successful, false otherwise
     */
    bool exportResults(const std::string& filepath);

    /**
     * @brief Import analysis results
     * 
     * @param filepath Path to analysis results file (JSON format)
     * @return true if import was successful, false otherwise
     */
    bool importResults(const std::string& filepath);

    /**
     * @brief Get analysis summary
     * 
     * @return String summary of analysis results
     */
    std::string getAnalysisSummary() const;

    /**
     * @brief Reset analysis state
     */
    void reset();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
