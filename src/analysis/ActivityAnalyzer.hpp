#pragma once

#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <map>
#include "brain/Brain.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

class ActivityAnalyzer {
public:
    struct SpikeStatistics {
        size_t totalSpikeCount;
        double averageFiringRate;
        double standardDeviation;
        double coefficientOfVariation;
        double peakFiringRate;
        double baselineFiringRate;
        Timestamp firstSpikeTime;
        Timestamp lastSpikeTime;
        std::vector<double> firingRateHistory;
    };
    
    struct CorrelationMetrics {
        std::vector<std::vector<double>> correlationMatrix;
        double meanCorrelation;
        double maxCorrelation;
        double minCorrelation;
    };
    
    struct ActivityPattern {
        std::vector<SpikeEvent> spikeEvents;
        std::vector<double> instantaneousRates;
        std::vector<double> filteredActivity;
        double burstFrequency;
        double silenceDuration;
        bool isBursting;
        bool isNoisy;
        bool isRegular;
        bool isSynchronous;
    };
    
    static std::unique_ptr<ActivityAnalyzer> create() {
        return std::make_unique<ActivityAnalyzer>();
    }
    
    // Spike statistics and analysis
    SpikeStatistics analyzeSpikeStatistics(const Brain& brain, size_t windowSize = 1000) const;
    ActivityPattern analyzeActivityPattern(const std::vector<SpikeEvent>& spikes, double samplingRate = 1000.0) const;
    
    // Correlation and cross-correlation analysis
    CorrelationMetrics calculateSpikeCorrelations(const std::vector<SpikeEvent>& spikes, 
                                                  const std::vector<NeuronId>& neuronIds) const;
    std::vector<double> crossCorrelation(const std::vector<SpikeEvent>& spikes1, 
                                         const std::vector<SpikeEvent>& spikes2,
                                         int maxLag) const;
    
    // Frequency analysis
    std::vector<double> computeFiringRate(const std::vector<SpikeEvent>& spikes, 
                                           double samplingRate = 1000.0) const;
    std::vector<double> computePowerSpectrum(const std::vector<double>& signal, 
                                             double samplingRate = 1000.0) const;
    
    // Burst and rhythm detection
    std::vector<std::pair<size_t, size_t>> detectBursts(const std::vector<SpikeEvent>& spikes,
                                                        double threshold = 3.0) const;
    std::vector<double> detectOscillations(const std::vector<SpikeEvent>& spikes,
                                           double samplingRate = 1000.0) const;
    
    // Activity visualization data generation
    std::vector<double> generateSpikeRaster(const std::vector<SpikeEvent>& spikes,
                                             size_t timeBins = 100) const;
    std::vector<std::vector<double>> generateMembraneTraces(const Brain& brain,
                                                             const std::vector<NeuronId>& neuronIds,
                                                             size_t timeSteps = 1000) const;
    
    // Activity analysis export
    bool exportSpikeStatistics(const std::string& filepath, const SpikeStatistics& stats) const;
    bool exportActivityPatterns(const std::string& filepath, 
                                const std::vector<ActivityPattern>& patterns) const;
    bool exportCorrelationMatrix(const std::string& filepath, const CorrelationMetrics& correlations) const;
    
private:
    ActivityAnalyzer() = default;
    
    // Helper methods
    std::vector<double> smoothSignal(const std::vector<double>& signal, int windowSize) const;
    std::vector<double> computeAutocorrelation(const std::vector<SpikeEvent>& spikes,
                                              int maxLag) const;
    bool isBurst(const std::vector<SpikeEvent>& spikes, size_t startIndex, 
                 size_t endIndex, double threshold) const;
};

} // namespace nlm