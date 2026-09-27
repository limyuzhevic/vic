// ==============================================================================
// Spike Train Analyzer
// ==============================================================================
#ifndef SPIKETRAINANALYZER_HPP
#define SPIKETRAINANALYZER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <numeric>
#include <algorithm>
#include <cmath>
#include "../src/brain/Neuron.hpp"
#include "../src/dynamics/SpikeSystem.hpp"

namespace nlm {
namespace debug {

/**
 * @class SpikeTrainAnalyzer
 * @brief Spike train analysis and visualization tool for NLM
 * 
 * Provides tools for analyzing spike train data including statistics, patterns,
 * correlations, and visualizations.
 */
class SpikeTrainAnalyzer {
public:
    /**
     * @enum AnalysisType
     * @brief Type of analysis
     */
    enum class AnalysisType {
        BASIC_STATISTICS,
        TEMPORAL_PATTERN,
        FREQUENCY_ANALYSIS,
        AUTO_CORRELATION,
        CROSS_CORRELATION,
        BISTATISTICS,
        PEAK_DETECTION,
        INTERVAL_STATISTICS,
        RATE_FLOWS,
        DICHOTOMOUS_SPIKES
    };

    /**
     * @enum SpikeClass
     * @brief Class of spike
     */
    enum class SpikeClass {
        REGULAR,
        BURST,
        PAUSED,
        CHAOTIC,
        PATTERNED
    };

    /**
     * @struct SpikeAnalysisResult
     * @brief Result of spike analysis
     */
    struct SpikeAnalysisResult {
        std::vector<double> spikeTimes;
        double firingRate;
        double coefficientOfVariation;
        double meanInterspikeInterval;
        double standardDeviationInterspikeInterval;
        std::vector<double> autocorrelation;
        std::vector<double> crossCorrelation;
        std::vector<double> histograms;
        std::vector<SpikeClass> spikeClasses;
        std::map<AnalysisType, double> analysisScores;
    };

    SpikeTrainAnalyzer();
    ~SpikeTrainAnalyzer();

    /**
     * @brief Analyze spike train
     * @param spikeTimes Vector of spike times
     * @return Spike analysis result
     */
    SpikeAnalysisResult analyze(const std::vector<double>& spikeTimes);

    /**
     * @brief Analyze neuron spike activity
     * @param neuron Neuron reference
     * @return Spike analysis result
     */
    SpikeAnalysisResult analyze(const Neuron& neuron);

    /**
     * @brief Analyze spike train for specific type
     * @param spikeTimes Vector of spike times
     * @param analysisType Analysis type
     * @return Analysis result
     */
    std::map<AnalysisType, double> analyze(const std::vector<double>& spikeTimes, AnalysisType analysisType);

    /**
     * @brief Classify spike train
     * @param spikeTimes Vector of spike times
     * @return Spike class
     */
    SpikeClass classifySpikeTrain(const std::vector<double>& spikeTimes);

    /**
     * @brief Get spike statistics
     * @param spikeTimes Vector of spike times
     * @return Spike statistics
     */
    struct SpikeStatistics {
        size_t numSpikes;
        double totalDuration;
        double meanFiringRate;
        double standardDeviationFiringRate;
        double meanInterspikeInterval;
        double coefficientOfVariation;
        double cvAtFirstSpikes;
        double cvAtLastSpikes;
        double kurtosis;
        double skewness;
        double burstIndex;
        double pauseIndex;
        double regularity;
    };

    static SpikeStatistics calculateSpikeStatistics(const std::vector<double>& spikeTimes);

    /**
     * @brief Calculate firing rate
     * @param spikeTimes Vector of spike times
     * @param windowSize Analysis window size
     * @return Vector of firing rates
     */
    std::vector<double> calculateFiringRate(const std::vector<double>& spikeTimes, double windowSize = 0.01);

    /**
     * @brief Calculate autocorrelation
     * @param spikeTimes Vector of spike times
     * @param maxLag Maximum lag
     * @param binSize Bin size
     * @return Autocorrelation
     */
    std::vector<double> calculateAutocorrelation(const std::vector<double>& spikeTimes, double maxLag = 0.1, double binSize = 0.001);

    /**
     * @brief Calculate cross-correlation
     * @param spikeTimes1 First spike train
     * @param spikeTimes2 Second spike train
     * @param maxLag Maximum lag
     * @param binSize Bin size
     * @return Cross-correlation
     */
    std::vector<double> calculateCrossCorrelation(const std::vector<double>& spikeTimes1, const std::vector<double>& spikeTimes2, double maxLag = 0.1, double binSize = 0.001);

    /**
     * @brief Detect spikes
     * @param signal Signal vector
     * @param threshold Threshold
     * @return Vector of spike times
     */
    std::vector<double> detectSpikes(const std::vector<double>& signal, double threshold = 0.5);

    /**
     * @brief Generate spike train
     * @param firingRate Firing rate
     * @param duration Duration
     * @param seed Random seed
     * @return Spike train
     */
    std::vector<double> generateSpikeTrain(double firingRate, double duration, unsigned int seed = 12345);

    /**
     * @brief Generate Poisson spike train
     * @param firingRate Firing rate
     * @param duration Duration
     * @param seed Random seed
     * @return Poisson spike train
     */
    std::vector<double> generatePoissonSpikeTrain(double firingRate, double duration, unsigned int seed = 12345);

    /**
     * @brief Generate regular spike train
     * @param firingRate Firing rate
     * @param duration Duration
     * @param noiseLevel Noise level
     * @param seed Random seed
     * @return Regular spike train
     */
    std::vector<double> generateRegularSpikeTrain(double firingRate, double duration, double noiseLevel = 0.1, unsigned int seed = 12345);

    /**
     * @brief Generate burst spike train
     * @param firingRate Firing rate
     * @param duration Duration
     * @param burstParameters Burst parameters
     * @param seed Random seed
     * @return Burst spike train
     */
    std::vector<double> generateBurstSpikeTrain(double firingRate, double duration, const std::vector<double>& burstParameters, unsigned int seed = 12345);

    /**
     * @brief Format spike times for display
     * @param spikeTimes Vector of spike times
     * @param format Format string
     * @return Formatted string
     */
    std::string formatSpikeTimes(const std::vector<double>& spikeTimes, const std::string& format = "%.6f") const;

    /**
     * @brief Format spike statistics for display
     * @param stats Spike statistics
     * @return Formatted string
     */
    std::string formatSpikeStatistics(const SpikeStatistics& stats) const;

    /**
     * @brief Generate spike train visualization
     * @param spikeTimes Vector of spike times
     * @param filename Output file path
     */
    void generateSpikeTrainVisualization(const std::vector<double>& spikeTimes, const std::string& filename);

    /**
     * @brief Generate autocorrelation visualization
     * @param spikeTimes Vector of spike times
     * @param filename Output file path
     */
    void generateAutocorrelationVisualization(const std::vector<double>& spikeTimes, const std::string& filename);

    /**
     * @brief Generate cross-correlation visualization
     * @param spikeTimes1 First spike train
     * @param spikeTimes2 Second spike train
     * @param filename Output file path
     */
    void generateCrossCorrelationVisualization(const std::vector<double>& spikeTimes1, const std::vector<double>& spikeTimes2, const std::string& filename);

    /**
     * @brief Get analysis types
     * @return Vector of analysis types
     */
    std::vector<AnalysisType> getAvailableAnalysisTypes() const;

    /**
     * @brief Get spike classes
     * @return Vector of spike classes
     */
    std::vector<SpikeClass> getAvailableSpikeClasses() const;

protected:
    /**
     * @brief Calculate coefficients of variation
     * @param spikeTimes Vector of spike times
     * @return Coefficients of variation
     */
    std::vector<double> calculateCoefficientsOfVariation(const std::vector<double>& spikeTimes) const;

    /**
     * @brief Detect bursts
     * @param spikeTimes Vector of spike times
     * @param minBurstDuration Minimum burst duration
     * @param maxBurstDuration Maximum burst duration
     * @param minInterburstInterval Minimum interburst interval
     * @return Vector of burst detections
     */
    std::vector<std::pair<size_t, size_t>> detectBursts(const std::vector<double>& spikeTimes, double minBurstDuration = 0.02, double maxBurstDuration = 0.1, double minInterburstInterval = 0.05);

    /**
     * @brief Detect pauses
     * @param spikeTimes Vector of spike times
     * @param minPauseDuration Minimum pause duration
     * @param maxPauseDuration Maximum pause duration
     * @param minInterpauseInterval Minimum interpause interval
     * @return Vector of pause detections
     */
    std::vector<std::pair<size_t, size_t>> detectPauses(const std::vector<double>& spikeTimes, double minPauseDuration = 0.02, double maxPauseDuration = 0.1, double minInterpauseInterval = 0.05);

    /**
     * @brief Classify spike as regular, burst, paused, chaotic, or patterned
     * @param spikeTimes Vector of spike times
     * @param spikeClass Spike class
     * @return true if classification was successful
     */
    bool classifySpike(const std::vector<double>& spikeTimes, SpikeClass& spikeClass);

    /**
     * @brief Calculate entropy of spike train
     * @param spikeTimes Vector of spike times
     * @param binSize Bin size
     * @return Entropy
     */
    double calculateEntropy(const std::vector<double>& spikeTimes, double binSize = 0.001);

    /**
     * @brief Calculate fractal dimension
     * @param spikeTimes Vector of spike times
     * @param minDistance Minimum distance
     * @return Fractal dimension
     */
    double calculateFractalDimension(const std::vector<double>& spikeTimes, double minDistance = 0.001);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // SPIKETRAINANALYZER_HPP
