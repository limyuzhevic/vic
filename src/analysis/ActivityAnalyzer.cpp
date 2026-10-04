#include "ActivityAnalyzer.hpp"

namespace nlm {

ActivityAnalyzer::SpikeStatistics ActivityAnalyzer::analyzeSpikeStatistics(const Brain& brain, 
                                                                          size_t windowSize) const {
    SpikeStatistics stats;
    
    // Get spike system
    auto* spikeSystem = brain.getSpikeSystem();
    if (!spikeSystem) {
        return stats;
    }
    
    const auto& spikeHistory = spikeSystem->getSpikeHistory();
    stats.totalSpikeCount = spikeHistory.size();
    
    if (stats.totalSpikeCount == 0) {
        return stats;
    }
    
    // Calculate statistics from spike timestamps
    std::vector<Timestamp> spikeTimes;
    for (const auto& spike : spikeHistory) {
        spikeTimes.push_back(spike.timestamp);
    }
    
    std::sort(spikeTimes.begin(), spikeTimes.end());
    
    stats.firstSpikeTime = spikeTimes.front();
    stats.lastSpikeTime = spikeTimes.back();
    
    // Calculate firing rates
    double timeSpan = stats.lastSpikeTime - stats.firstSpikeTime;
    if (timeSpan > 0.0) {
        stats.averageFiringRate = stats.totalSpikeCount / timeSpan;
        
        // Calculate instantaneous firing rates
        stats.firingRateHistory.resize(windowSize, 0.0);
        double binWidth = timeSpan / windowSize;
        
        for (const auto& spikeTime : spikeTimes) {
            size_t binIndex = static_cast<size_t>((spikeTime - stats.firstSpikeTime) / binWidth);
            if (binIndex < windowSize) {
                stats.firingRateHistory[binIndex]++;
            }
        }
        
        // Normalize to firing rate (spikes per second)
        for (auto& rate : stats.firingRateHistory) {
            rate = rate / binWidth;
        }
        
        // Calculate statistics
        double sum = std::accumulate(stats.firingRateHistory.begin(), 
                                    stats.firingRateHistory.end(), 0.0);
        stats.averageFiringRate = sum / windowSize;
        
        double sumSq = 0.0;
        for (double rate : stats.firingRateHistory) {
            sumSq += (rate - stats.averageFiringRate) * (rate - stats.averageFiringRate);
        }
        stats.standardDeviation = std::sqrt(sumSq / windowSize);
        
        if (stats.averageFiringRate > 0.0) {
            stats.coefficientOfVariation = stats.standardDeviation / stats.averageFiringRate;
        }
        
        // Find peak and baseline firing rates
        stats.peakFiringRate = *std::max_element(stats.firingRateHistory.begin(), 
                                                stats.firingRateHistory.end());
        stats.baselineFiringRate = *std::min_element(stats.firingRateHistory.begin(), 
                                                     stats.firingRateHistory.end());
    }
    
    return stats;
}

ActivityAnalyzer::ActivityPattern ActivityAnalyzer::analyzeActivityPattern(const std::vector<SpikeEvent>& spikes,
                                                                           double samplingRate) const {
    ActivityPattern pattern;
    pattern.spikeEvents = spikes;
    
    if (spikes.empty()) {
        return pattern;
    }
    
    // Calculate instantaneous firing rates
    Timestamp firstTime = spikes.front().timestamp;
    Timestamp lastTime = spikes.back().timestamp;
    double duration = lastTime - firstTime;
    
    if (duration > 0.0) {
        size_t numBins = static_cast<size_t>(duration * samplingRate);
        pattern.instantaneousRates.resize(numBins, 0.0);
        
        for (const auto& spike : spikes) {
            size_t binIndex = static_cast<size_t>((spike.timestamp - firstTime) * samplingRate);
            if (binIndex < numBins) {
                pattern.instantaneousRates[binIndex]++;
            }
        }
        
        // Convert to firing rate (spikes per second)
        double binWidth = 1.0 / samplingRate;
        for (auto& rate : pattern.instantaneousRates) {
            rate = rate / binWidth;
        }
        
        // Apply smoothing
        pattern.filteredActivity = smoothSignal(pattern.instantaneousRates, 5);
    }
    
    // Detect bursting
    auto burstIntervals = detectBursts(spikes);
    pattern.isBursting = !burstIntervals.empty();
    if (pattern.isBursting) {
        pattern.burstFrequency = burstIntervals.size() / duration;
        pattern.silenceDuration = duration / burstIntervals.size();
    }
    
    // Detect regularity
    std::vector<double> isi(pattern.spikeEvents.size() - 1);
    for (size_t i = 1; i < pattern.spikeEvents.size(); ++i) {
        isi[i - 1] = pattern.spikeEvents[i].timestamp - pattern.spikeEvents[i - 1].timestamp;
    }
    
    if (!isi.empty()) {
        double meanISI = std::accumulate(isi.begin(), isi.end(), 0.0) / isi.size();
        double varISI = 0.0;
        for (double interval : isi) {
            varISI += (interval - meanISI) * (interval - meanISI);
        }
        varISI /= isi.size();
        
        pattern.isRegular = (varISI / (meanISI * meanISI)) < 0.1; // Low coefficient of variation
        pattern.isNoisy = (varISI / (meanISI * meanISI)) > 1.0; // High coefficient of variation
    }
    
    // Detect synchronization (placeholder - would need multiple neuron data)
    pattern.isSynchronous = false;
    
    return pattern;
}

ActivityAnalyzer::CorrelationMetrics ActivityAnalyzer::calculateSpikeCorrelations(
    const std::vector<SpikeEvent>& spikes,
    const std::vector<NeuronId>& neuronIds) const {
    CorrelationMetrics metrics;
    
    if (spikes.empty() || neuronIds.empty()) {
        return metrics;
    }
    
    size_t numNeurons = neuronIds.size();
    metrics.correlationMatrix.resize(numNeurons, std::vector<double>(numNeurons, 0.0));
    
    // For now, create a simple correlation matrix based on spike counts
    // In a real implementation, this would use actual spike timing correlations
    for (size_t i = 0; i < numNeurons; ++i) {
        for (size_t j = 0; j < numNeurons; ++j) {
            if (i == j) {
                metrics.correlationMatrix[i][j] = 1.0;
            } else {
                metrics.correlationMatrix[i][j] = 0.5; // Placeholder value
            }
        }
    }
    
    // Calculate correlation statistics
    metrics.meanCorrelation = 0.0;
    metrics.maxCorrelation = 0.0;
    metrics.minCorrelation = 1.0;
    
    for (size_t i = 0; i < numNeurons; ++i) {
        for (size_t j = 0; j < numNeurons; ++j) {
            double corr = metrics.correlationMatrix[i][j];
            metrics.meanCorrelation += corr;
            metrics.maxCorrelation = std::max(metrics.maxCorrelation, corr);
            metrics.minCorrelation = std::min(metrics.minCorrelation, corr);
        }
    }
    
    metrics.meanCorrelation /= (numNeurons * numNeurons);
    
    return metrics;
}

std::vector<double> ActivityAnalyzer::crossCorrelation(const std::vector<SpikeEvent>& spikes1,
                                                        const std::vector<SpikeEvent>& spikes2,
                                                        int maxLag) const {
    std::vector<double> correlations(maxLag * 2 + 1, 0.0);
    
    if (spikes1.empty() || spikes2.empty()) {
        return correlations;
    }
    
    // Placeholder implementation - would compute actual cross-correlation
    for (int lag = -maxLag; lag <= maxLag; ++lag) {
        correlations[lag + maxLag] = std::sin(lag * 0.1); // Placeholder sinusoidal correlation
    }
    
    return correlations;
}

std::vector<double> ActivityAnalyzer::computeFiringRate(const std::vector<SpikeEvent>& spikes,
                                                         double samplingRate) const {
    std::vector<double> firingRates;
    
    if (spikes.empty()) {
        return firingRates;
    }
    
    Timestamp firstTime = spikes.front().timestamp;
    Timestamp lastTime = spikes.back().timestamp;
    double duration = lastTime - firstTime;
    
    if (duration <= 0.0) {
        return firingRates;
    }
    
    size_t numBins = static_cast<size_t>(duration * samplingRate);
    firingRates.resize(numBins, 0.0);
    
    double binWidth = 1.0 / samplingRate;
    
    for (const auto& spike : spikes) {
        size_t binIndex = static_cast<size_t>((spike.timestamp - firstTime) * samplingRate);
        if (binIndex < numBins) {
            firingRates[binIndex]++;
        }
    }
    
    // Convert to firing rate (spikes per second)
    for (auto& rate : firingRates) {
        rate = rate / binWidth;
    }
    
    return firingRates;
}

std::vector<double> ActivityAnalyzer::computePowerSpectrum(const std::vector<double>& signal,
                                                           double samplingRate) const {
    std::vector<double> powerSpectrum;
    
    if (signal.empty()) {
        return powerSpectrum;
    }
    
    size_t n = signal.size();
    size_t spectrumSize = n / 2 + 1;
    powerSpectrum.resize(spectrumSize, 0.0);
    
    // Simple FFT implementation (placeholder)
    // In a real implementation, this would use a proper FFT library
    for (size_t i = 0; i < spectrumSize; ++i) {
        powerSpectrum[i] = 1.0 / (i + 1); // Placeholder power spectrum
    }
    
    return powerSpectrum;
}

std::vector<std::pair<size_t, size_t>> ActivityAnalyzer::detectBursts(const std::vector<SpikeEvent>& spikes,
                                                                        double threshold) const {
    std::vector<std::pair<size_t, size_t>> bursts;
    
    if (spikes.size() < 10) {
        return bursts;
    }
    
    // Compute instantaneous firing rate using sliding window
    std::vector<double> rates(spikes.size(), 0.0);
    double windowSize = 0.01; // 10 ms window
    
    for (size_t i = 0; i < spikes.size(); ++i) {
        Timestamp windowStart = spikes[i].timestamp - windowSize / 2;
        Timestamp windowEnd = spikes[i].timestamp + windowSize / 2;
        
        size_t spikeCount = 0;
        for (const auto& spike : spikes) {
            if (spike.timestamp >= windowStart && spike.timestamp <= windowEnd) {
                spikeCount++;
            }
        }
        
        double rate = spikeCount / windowSize;
        rates[i] = rate;
    }
    
    // Detect bursts where rate exceeds threshold
    for (size_t i = 0; i < rates.size(); ++i) {
        if (rates[i] > threshold) {
            size_t start = i;
            while (i < rates.size() && rates[i] > threshold) {
                i++;
            }
            size_t end = i - 1;
            bursts.push_back(std::make_pair(start, end));
        }
    }
    
    return bursts;
}

std::vector<double> ActivityAnalyzer::detectOscillations(const std::vector<SpikeEvent>& spikes,
                                                          double samplingRate) const {
    std::vector<double> oscillations;
    
    if (spikes.size() < 20) {
        return oscillations;
    }
    
    // Compute autocorrelation
    auto autocorr = computeAutocorrelation(spikes, static_cast<int>(samplingRate * 1.0)); // 1 second lag
    
    // Find peaks in autocorrelation
    for (size_t i = 1; i < autocorr.size(); ++i) {
        if (autocorr[i] > autocorr[i - 1] && autocorr[i] > autocorr[i + 1]) {
            oscillations.push_back(autocorr[i]);
        }
    }
    
    return oscillations;
}

std::vector<double> ActivityAnalyzer::generateSpikeRaster(const std::vector<SpikeEvent>& spikes,
                                                            size_t timeBins) const {
    std::vector<double> raster;
    
    if (spikes.empty()) {
        return raster;
    }
    
    raster.resize(timeBins, 0.0);
    
    Timestamp firstTime = spikes.front().timestamp;
    Timestamp lastTime = spikes.back().timestamp;
    double duration = lastTime - firstTime;
    
    if (duration <= 0.0) {
        return raster;
    }
    
    double binWidth = duration / timeBins;
    
    for (const auto& spike : spikes) {
        size_t binIndex = static_cast<size_t>((spike.timestamp - firstTime) / binWidth);
        if (binIndex < timeBins) {
            raster[binIndex]++;
        }
    }
    
    return raster;
}

std::vector<std::vector<double>> ActivityAnalyzer::generateMembraneTraces(
    const Brain& brain,
    const std::vector<NeuronId>& neuronIds,
    size_t timeSteps) const {
    std::vector<std::vector<double>> traces;
    
    if (neuronIds.empty()) {
        return traces;
    }
    
    traces.resize(neuronIds.size(), std::vector<double>(timeSteps, 0.0));
    
    // Placeholder - would get actual membrane potential traces from neurons
    for (size_t i = 0; i < neuronIds.size(); ++i) {
        for (size_t t = 0; t < timeSteps; ++t) {
            // Generate some dummy membrane potential trace
            traces[i][t] = 0.5 + 0.3 * std::sin(2.0 * M_PI * t / timeSteps) +
                          0.2 * (rand() / static_cast<double>(RAND_MAX) - 0.5);
        }
    }
    
    return traces;
}

std::vector<double> ActivityAnalyzer::smoothSignal(const std::vector<double>& signal,
                                                   int windowSize) const {
    std::vector<double> smoothed(signal.size(), 0.0);
    
    if (signal.empty() || windowSize <= 1) {
        return signal;
    }
    
    int halfWindow = windowSize / 2;
    int signalSize = static_cast<int>(signal.size());
    
    for (int i = 0; i < signalSize; ++i) {
        int start = std::max(0, i - halfWindow);
        int end = std::min(signalSize, i + halfWindow + 1);
        
        double sum = 0.0;
        int count = 0;
        for (int j = start; j < end; ++j) {
            sum += signal[j];
            count++;
        }
        
        smoothed[i] = sum / count;
    }
    
    return smoothed;
}

std::vector<double> ActivityAnalyzer::computeAutocorrelation(const std::vector<SpikeEvent>& spikes,
                                                            int maxLag) const {
    std::vector<double> autocorr(maxLag + 1, 0.0);
    
    if (spikes.size() <= static_cast<size_t>(maxLag)) {
        return autocorr;
    }
    
    // Simple autocorrelation implementation
    size_t n = spikes.size();
    std::vector<Timestamp> spikeTimes;
    for (const auto& spike : spikes) {
        spikeTimes.push_back(spike.timestamp);
    }
    
    for (int lag = 0; lag <= maxLag; ++lag) {
        double sum = 0.0;
        for (size_t i = 0; i < n - lag; ++i) {
            sum += (spikeTimes[i] - spikeTimes[i + lag]) * (spikeTimes[i] - spikeTimes[i + lag]);
        }
        autocorr[lag] = sum / (n - lag);
    }
    
    return autocorr;
}

bool ActivityAnalyzer::isBurst(const std::vector<SpikeEvent>& spikes, size_t startIndex,
                               size_t endIndex, double threshold) const {
    if (startIndex >= spikes.size() || endIndex >= spikes.size()) {
        return false;
    }
    
    double duration = spikes[endIndex].timestamp - spikes[startIndex].timestamp;
    size_t spikeCount = endIndex - startIndex + 1;
    
    double rate = spikeCount / duration;
    return rate > threshold;
}

bool ActivityAnalyzer::exportSpikeStatistics(const std::string& filepath, const SpikeStatistics& stats) const {
    // Placeholder implementation - would write to CSV/JSON file
    return true;
}

bool ActivityAnalyzer::exportActivityPatterns(const std::string& filepath,
                                              const std::vector<ActivityPattern>& patterns) const {
    // Placeholder implementation - would write to CSV/JSON file
    return true;
}

bool ActivityAnalyzer::exportCorrelationMatrix(const std::string& filepath, const CorrelationMetrics& correlations) const {
    // Placeholder implementation - would write to CSV/JSON file
    return true;
}

} // namespace nlm