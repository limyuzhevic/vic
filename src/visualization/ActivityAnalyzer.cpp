// Implementation of activity analyzer
#include "ActivityAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <complex>
#include <unordered_set>

namespace nlm {

ActivityAnalyzer::ActivityAnalyzer()
    : analysisWindow_(100), minCorrelationThreshold_(0.5f), minPatternSignificance_(0.7f),
      detectBursts_(true), computeSpectral_(true), computeRecurrence_(true),
      maxWindowSize_(1000) {
}

ActivityAnalyzer::~ActivityAnalyzer() {
    shutdown();
}

bool ActivityAnalyzer::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize ActivityAnalyzer: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    timeSeriesData_.clear();
    activityStats_.clear();
    networkMetrics_.clear();
    patternDatabase_.clear();
    
    NLM_LOG_INFO("ActivityAnalyzer initialized successfully");
    return true;
}

void ActivityAnalyzer::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    brain_.reset();
    timeSeriesData_.clear();
    activityStats_.clear();
    networkMetrics_.clear();
    patternDatabase_.clear();
    
    while (!analysisWindow_.empty()) {
        analysisWindow_.pop();
    }
    
    NLM_LOG_INFO("ActivityAnalyzer shutdown complete");
}

void ActivityAnalyzer::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) return;
    
    // Convert VisualizationData to TimeSeriesData
    TimeSeriesData timeSeries;
    timeSeries.step = brain_->getCurrentStep();
    timeSeries.timestamp = brain_->getCurrentTime();
    
    // Extract neural data from brain
    size_t neuronCount = brain_->getTotalNeuronCount();
    timeSeries.neuronActivities.resize(neuronCount);
    timeSeries.neuronFiringRates.resize(neuronCount);
    timeSeries.membranePotentials.resize(neuronCount);
    timeSeries.spikeCounts.resize(neuronCount);
    
    // This is a placeholder - real implementation would extract data from brain
    // For now, we'll simulate some data
    for (size_t i = 0; i < neuronCount && i < timeSeries.neuronActivities.size(); ++i) {
        float activity = static_cast<float>(rand()) / RAND_MAX;
        timeSeries.neuronActivities[i] = activity;
        timeSeries.neuronFiringRates[i] = activity * 100.0f; // Hz
        timeSeries.membranePotentials[i] = -70.0f + activity * 50.0f; // mV
        timeSeries.spikeCounts[i] = static_cast<size_t>(activity * 10.0f);
    }
    
    // Extract neuromodulator levels
    timeSeries.neuromodulatorLevels.resize(4); // dopamine, curiosity, novelty, prediction_error
    timeSeries.neuromodulatorLevels[0] = 0.5f; // dopamine
    timeSeries.neuromodulatorLevels[1] = 0.3f; // curiosity
    timeSeries.neuromodulatorLevels[2] = 0.2f; // novelty
    timeSeries.neuromodulatorLevels[3] = 0.4f; // prediction error
    
    // Add to time series data
    timeSeriesData_.push_back(timeSeries);
    
    // Maintain window size
    if (timeSeriesData_.size() > analysisWindow_) {
        timeSeriesData_.erase(timeSeriesData_.begin());
    }
    
    // Update analysis window
    analysisWindow_.push(timeSeries);
    if (analysisWindow_.size() > maxWindowSize_) {
        analysisWindow_.pop();
    }
    
    // Perform real-time analysis
    analyze(timeSeries);
}

void ActivityAnalyzer::analyze(const TimeSeriesData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Compute statistics
    ActivityStats stats = computeStats(data);
    activityStats_.push_back(stats);
    
    // Maintain window size
    if (activityStats_.size() > analysisWindow_) {
        activityStats_.erase(activityStats_.begin());
    }
    
    // Compute network metrics
    NetworkMetrics metrics = computeNetworkMetrics(data);
    networkMetrics_.push_back(metrics);
    
    if (networkMetrics_.size() > analysisWindow_) {
        networkMetrics_.erase(networkMetrics_.begin());
    }
    
    // Detect patterns
    std::vector<PatternInfo> patterns = detectPatterns(data);
    for (const auto& pattern : patterns) {
        patternDatabase_[pattern.patternId].push_back(pattern);
    }
    
    // Maintain pattern database size
    for (auto it = patternDatabase_.begin(); it != patternDatabase_.end(); ) {
        if (it->second.size() > maxPatternInstances_) {
            it->second.erase(it->second.begin());
        }
        ++it;
    }
}

ActivityStats ActivityAnalyzer::computeStats(const TimeSeriesData& data) const {
    ActivityStats stats;
    stats.step = data.step;
    stats.timestamp = data.timestamp;
    
    if (data.neuronActivities.empty()) {
        return stats;
    }
    
    // Basic statistics
    stats.meanFiringRate = computeMean(data.neuronActivities);
    float mean = stats.meanFiringRate;
    stats.varianceFiringRate = computeVariance(data.neuronActivities, mean);
    stats.standardDeviationFiringRate = computeStandardDeviation(stats.varianceFiringRate);
    stats.skewnessFiringRate = computeSkewness(data.neuronActivities, mean, stats.standardDeviationFiringRate);
    stats.kurtosisFiringRate = computeKurtosis(data.neuronActivities, mean, stats.standardDeviationFiringRate);
    
    // Synchronization and coherence measures
    stats.synchronyLevel = computeSynchrony(data);
    stats.coherenceLevel = 0.0f; // Placeholder - would compute coherence with regions
    stats.correlationLength = computeCorrelationLength(data);
    
    // Firing rate histogram
    size_t numBins = 20;
    float minActivity = *std::min_element(data.neuronActivities.begin(), data.neuronActivities.end());
    float maxActivity = *std::max_element(data.neuronActivities.begin(), data.neuronActivities.end());
    float binWidth = (maxActivity - minActivity) / numBins;
    
    stats.firingRateHistogram.resize(numBins, 0.0f);
    for (float activity : data.neuronActivities) {
        size_t binIdx = static_cast<size_t>((activity - minActivity) / binWidth);
        if (binIdx >= numBins) binIdx = numBins - 1;
        stats.firingRateHistogram[binIdx]++;
    }
    
    // Normalize histogram
    float totalCount = static_cast<float>(data.neuronActivities.size());
    for (float& count : stats.firingRateHistogram) {
        count /= totalCount;
    }
    
    return stats;
}

std::vector<PatternInfo> ActivityAnalyzer::detectPatterns(const TimeSeriesData& data) const {
    std::vector<PatternInfo> patterns;
    
    if (data.neuronActivities.empty()) return patterns;
    
    // Extract features from neural activity
    std::vector<float> features;
    extractFeatures(data, features);
    
    // This is a placeholder - real implementation would use more sophisticated pattern detection
    // For now, we'll simulate pattern detection
    
    // Simulate detecting different patterns
    if (features.size() >= 5) {
        PatternInfo pattern1;
        pattern1.patternId = "oscillation_10hz";
        pattern1.patternType = "rhythmic_oscillation";
        pattern1.occurrenceRate = 0.3f;
        pattern1.similarityScore = 0.8f;
        pattern1.firstOccurrence = data.step;
        pattern1.lastOccurrence = data.step;
        pattern1.occurrenceSteps.push_back(data.step);
        patterns.push_back(pattern1);
    }
    
    if (features.size() >= 3) {
        PatternInfo pattern2;
        pattern2.patternId = "burst_activity";
        pattern2.patternType = "synchronized_burst";
        pattern2.occurrenceRate = 0.2f;
        pattern2.similarityScore = 0.7f;
        pattern2.firstOccurrence = data.step;
        pattern2.lastOccurrence = data.step;
        pattern2.occurrenceSteps.push_back(data.step);
        patterns.push_back(pattern2);
    }
    
    return patterns;
}

std::vector<CrossRegionCorrelation> ActivityAnalyzer::computeCrossRegionCorrelations(const TimeSeriesData& data) const {
    std::vector<CrossRegionCorrelation> correlations;
    
    // Placeholder - real implementation would compute correlations between regions
    // For now, we'll simulate some correlations
    
    size_t numRegions = 3; // Placeholder
    for (size_t i = 0; i < numRegions; ++i) {
        for (size_t j = i + 1; j < numRegions; ++j) {
            float correlation = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f; // [-1, 1]
            float lag = static_cast<float>((rand() % 10) - 5); // [-5, 5]
            
            CrossRegionCorrelation corr;
            corr.sourceRegion = i;
            corr.targetRegion = j;
            corr.correlationCoefficient = correlation;
            corr.lag = lag;
            corr.isSignificant = std::abs(correlation) > minCorrelationThreshold_;
            
            correlations.push_back(corr);
        }
    }
    
    return correlations;
}

std::vector<BurstInfo> ActivityAnalyzer::detectBursts(const TimeSeriesData& data) const {
    std::vector<BurstInfo> bursts;
    
    if (!detectBursts_ || data.neuronActivities.empty()) return bursts;
    
    // Find burst boundaries
    std::vector<size_t> burstStartIndices;
    std::vector<size_t> burstEndIndices;
    findBurstBoundaries(data.neuronActivities, burstStartIndices, burstEndIndices);
    
    // Create burst info for each detected burst
    for (size_t i = 0; i < burstStartIndices.size(); ++i) {
        BurstInfo burst;
        burst.neuronId = i + 1; // Placeholder neuron ID
        burst.startStep = burstStartIndices[i];
        burst.endStep = burstEndIndices[i];
        
        float avgRate, peakRate;
        burst.averageFiringRate = computeBurstStatistics(data.neuronActivities, 
                                                         burstStartIndices[i], 
                                                         burstEndIndices[i], 
                                                         avgRate, peakRate);
        burst.peakFiringRate = peakRate;
        burst.totalSpikes = static_cast<size_t>(burst.averageFiringRate * 
                                               (burst.endStep - burst.startStep + 1));
        
        bursts.push_back(burst);
    }
    
    return bursts;
}

NetworkMetrics ActivityAnalyzer::computeNetworkMetrics(const TimeSeriesData& data) const {
    NetworkMetrics metrics;
    metrics.step = data.step;
    metrics.timestamp = data.timestamp;
    
    if (data.neuronActivities.empty()) {
        return metrics;
    }
    
    // Compute basic statistics
    metrics.meanFiringRate = computeMean(data.neuronActivities);
    float mean = metrics.meanFiringRate;
    metrics.varianceFiringRate = computeVariance(data.neuronActivities, mean);
    metrics.standardDeviationFiringRate = computeStandardDeviation(metrics.varianceFiringRate);
    metrics.skewnessFiringRate = computeSkewness(data.neuronActivities, mean, metrics.standardDeviationFiringRate);
    metrics.kurtosisFiringRate = computeKurtosis(data.neuronActivities, mean, metrics.standardDeviationFiringRate);
    
    // Network-level metrics
    metrics.synchronyLevel = computeSynchrony(data);
    metrics.coherenceLevel = 0.0f; // Placeholder
    metrics.correlationLength = computeCorrelationLength(data);
    metrics.globalEfficiency = computeGlobalEfficiency(data);
    
    // Regional metrics
    size_t numRegions = 3; // Placeholder
    metrics.regionalMetrics.resize(numRegions, 0.0f);
    
    return metrics;
}

float ActivityAnalyzer::computeSynchrony(const TimeSeriesData& data) const {
    if (data.neuronActivities.size() <= 1) return 0.0f;
    
    // Compute mean firing rate
    float meanRate = computeMean(data.neuronActivities);
    
    // Compute synchrony measure (squared correlation coefficient)
    float sum = 0.0f;
    for (float rate : data.neuronActivities) {
        float deviation = rate - meanRate;
        sum += deviation * deviation;
    }
    
    return sum / static_cast<float>(data.neuronActivities.size() * meanRate * meanRate);
}

float ActivityAnalyzer::computeCoherence(const TimeSeriesData& data, size_t region1, size_t region2) const {
    // Placeholder - real implementation would compute coherence between regions
    return static_cast<float>(rand()) / RAND_MAX;
}

float ActivityAnalyzer::computeCorrelationLength(const TimeSeriesData& data) const {
    // Placeholder - real implementation would compute correlation length
    return static_cast<float>(rand()) / RAND_MAX * 10.0f; // [0, 10]
}

float ActivityAnalyzer::computeGlobalEfficiency(const TimeSeriesData& data) const {
    // Placeholder - real implementation would compute global efficiency
    return static_cast<float>(rand()) / RAND_MAX; // [0, 1]
}

float ActivityAnalyzer::computeLocalEfficiency(const TimeSeriesData& data, size_t regionId) const {
    // Placeholder - real implementation would compute local efficiency
    return static_cast<float>(rand()) / RAND_MAX; // [0, 1]
}

void ActivityAnalyzer::computeFiringRateMaps(const TimeSeriesData& data,
                                            std::vector<std::vector<float>>& rateMaps) const {
    // Placeholder - real implementation would compute spatial firing rate maps
    rateMaps.clear();
    size_t numRegions = 3; // Placeholder
    rateMaps.resize(numRegions, std::vector<float>(100, 0.0f)); // 100x100 grid per region
}

void ActivityAnalyzer::computeActivityHeatmaps(const TimeSeriesData& data,
                                              std::vector<std::vector<float>>& heatmaps) const {
    // Placeholder - real implementation would compute activity heatmaps
    heatmaps.clear();
    size_t numRegions = 3; // Placeholder
    heatmaps.resize(numRegions, std::vector<float>(100, 0.0f)); // 100x100 grid per region
}

void ActivityAnalyzer::computeProbabilityDistributions(const TimeSeriesData& data,
                                                     std::vector<float>& firingRateDist,
                                                     std::vector<float>& membranePotentialDist) const {
    // Placeholder - real implementation would compute probability distributions
    firingRateDist.clear();
    membranePotentialDist.clear();
    
    // Create histograms
    size_t numBins = 50;
    firingRateDist.resize(numBins, 0.0f);
    membranePotentialDist.resize(numBins, 0.0f);
    
    if (data.neuronActivities.size() > 0) {
        float minRate = *std::min_element(data.neuronActivities.begin(), data.neuronActivities.end());
        float maxRate = *std::max_element(data.neuronActivities.begin(), data.neuronActivities.end());
        float rateBinWidth = (maxRate - minRate) / numBins;
        
        for (float rate : data.neuronActivities) {
            size_t binIdx = static_cast<size_t>((rate - minRate) / rateBinWidth);
            if (binIdx >= numBins) binIdx = numBins - 1;
            firingRateDist[binIdx]++;
        }
        
        float totalRate = static_cast<float>(data.neuronActivities.size());
        for (float& count : firingRateDist) {
            count /= totalRate;
        }
    }
}

void ActivityAnalyzer::computeSpectralAnalysis(const TimeSeriesData& data,
                                              std::vector<float>& powerSpectrum,
                                              std::vector<float>& dominantFrequencies) const {
    if (!computeSpectral_ || data.neuronActivities.empty()) {
        return;
    }
    
    // Placeholder - real implementation would compute spectral analysis
    // For now, we'll simulate a power spectrum
    powerSpectrum.resize(64, 0.0f);
    dominantFrequencies.resize(10, 0.0f);
    
    // Generate simulated power spectrum
    for (size_t i = 0; i < powerSpectrum.size(); ++i) {
        float freq = static_cast<float>(i) / 10.0f; // Normalized frequency
        powerSpectrum[i] = std::exp(-freq * freq) * (1.0f + static_cast<float>(rand()) / RAND_MAX * 0.1f);
    }
}

void ActivityAnalyzer::computeRecurrenceAnalysis(const TimeSeriesData& data,
                                               float& determinism,
                                               float& laminarity,
                                               float& entropy) const {
    if (!computeRecurrence_ || data.neuronActivities.empty()) {
        determinism = 0.0f;
        laminarity = 0.0f;
        entropy = 0.0f;
        return;
    }
    
    // Placeholder - real implementation would compute recurrence analysis
    // For now, we'll simulate recurrence quantification analysis
    determinism = static_cast<float>(rand()) / RAND_MAX;
    laminarity = static_cast<float>(rand()) / RAND_MAX;
    entropy = -determinism * std::log(determinism + 0.001f);
}

bool ActivityAnalyzer::recognizePattern(const TimeSeriesData& data, const std::string& patternId) const {
    // Check if pattern exists in database
    auto it = patternDatabase_.find(patternId);
    if (it == patternDatabase_.end()) {
        return false;
    }
    
    // Compare current activity with stored patterns
    // Placeholder - real implementation would compute similarity
    return (static_cast<float>(rand()) / RAND_MAX) > 0.5f;
}

std::vector<PatternInfo> ActivityAnalyzer::recognizeAllPatterns(const TimeSeriesData& data) const {
    std::vector<PatternInfo> recognizedPatterns;
    
    // Check all patterns in database
    for (const auto& pair : patternDatabase_) {
        for (const auto& pattern : pair.second) {
            bool matches = recognizePattern(data, pattern.patternId);
            if (matches) {
                recognizedPatterns.push_back(pattern);
            }
        }
    }
    
    return recognizedPatterns;
}

void ActivityAnalyzer::classifyPatternType(const TimeSeriesData& data, std::vector<std::string>& patternTypes) const {
    // Placeholder - real implementation would classify pattern types
    patternTypes.clear();
    
    // Simulate pattern classification
    if (data.neuronActivities.size() > 0) {
        patternTypes.push_back("oscillatory");
        patternTypes.push_back("bursty");
        patternTypes.push_back("random");
    }
}

void ActivityAnalyzer::prepareActivityVisualizationData(const TimeSeriesData& data,
                                                      std::vector<float>& activityData,
                                                      std::vector<float>& regionalActivity) const {
    activityData = data.neuronActivities;
    
    // Compute regional activity
    size_t numRegions = 3; // Placeholder
    regionalActivity.resize(numRegions, 0.0f);
    
    if (!activityData.empty()) {
        float regionSize = static_cast<float>(activityData.size()) / numRegions;
        for (size_t i = 0; i < numRegions; ++i) {
            size_t startIdx = static_cast<size_t>(i * regionSize);
            size_t endIdx = std::min(startIdx + static_cast<size_t>(regionSize), 
                                   activityData.size());
            
            float sum = 0.0f;
            for (size_t j = startIdx; j < endIdx; ++j) {
                sum += activityData[j];
            }
            
            regionalActivity[i] = sum / static_cast<float>(endIdx - startIdx);
        }
    }
}

void ActivityAnalyzer::prepareConnectivityVisualizationData(const TimeSeriesData& data,
                                                         std::vector<std::vector<float>>& connectivityMatrix) const {
    // Placeholder - real implementation would prepare connectivity data for visualization
    size_t numNeurons = data.neuronActivities.size();
    connectivityMatrix.resize(numNeurons, std::vector<float>(numNeurons, 0.0f));
    
    if (numNeurons > 0) {
        // Fill with simulated connectivity
        for (size_t i = 0; i < numNeurons; ++i) {
            for (size_t j = 0; j < numNeurons; ++j) {
                if (i != j) {
                    float correlation = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
                    connectivityMatrix[i][j] = std::abs(correlation);
                }
            }
        }
    }
}

void ActivityAnalyzer::extractFeatures(const TimeSeriesData& data, std::vector<float>& features) const {
    // Extract features from neural activity for pattern recognition
    features.clear();
    
    if (data.neuronActivities.empty()) return;
    
    // Compute basic statistics as features
    features.push_back(computeMean(data.neuronActivities));
    features.push_back(computeVariance(data.neuronActivities, computeMean(data.neuronActivities)));
    features.push_back(computeStandardDeviation(features[1]));
    
    // Compute higher-order statistics
    features.push_back(computeSkewness(data.neuronActivities, features[0], features[2]));
    features.push_back(computeKurtosis(data.neuronActivities, features[0], features[2]));
    
    // Compute synchronization measures
    features.push_back(computeSynchrony(data));
}

float ActivityAnalyzer::computePatternSimilarity(const std::vector<float>& pattern1,
                                               const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.0f;
    }
    
    // Compute cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f, norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    float denominator = std::sqrt(norm1) * std::sqrt(norm2);
    if (denominator < 1e-10f) return 0.0f;
    
    return dotProduct / denominator;
}

void ActivityAnalyzer::clusterPatterns(const std::vector<std::vector<float>>& featureVectors,
                                      std::vector<size_t>& clusterAssignments) const {
    // Placeholder - real implementation would use clustering algorithms
    // For now, we'll assign all patterns to the same cluster
    clusterAssignments.resize(featureVectors.size(), 0);
}

void ActivityAnalyzer::findBurstBoundaries(const std::vector<float>& firingRates,
                                         std::vector<size_t>& burstStartIndices,
                                         std::vector<size_t>& burstEndIndices) const {
    // Simple burst detection based on firing rate thresholds
    burstStartIndices.clear();
    burstEndIndices.clear();
    
    if (firingRates.empty()) return;
    
    float meanRate = computeMean(firingRates);
    float stddev = computeStandardDeviation(computeVariance(firingRates, meanRate));
    float threshold = meanRate + 2.0f * stddev; // 2 standard deviations above mean
    
    bool inBurst = false;
    size_t burstStart = 0;
    
    for (size_t i = 0; i < firingRates.size(); ++i) {
        if (firingRates[i] > threshold && !inBurst) {
            // Start of burst
            inBurst = true;
            burstStart = i;
        } else if (firingRates[i] <= threshold && inBurst) {
            // End of burst
            inBurst = false;
            burstStartIndices.push_back(burstStart);
            burstEndIndices.push_back(i - 1);
        }
    }
    
    // Handle burst that extends to end of data
    if (inBurst) {
        burstStartIndices.push_back(burstStart);
        burstEndIndices.push_back(firingRates.size() - 1);
    }
}

float ActivityAnalyzer::computeBurstStatistics(const std::vector<float>& firingRates,
                                              size_t startIdx, size_t endIdx,
                                              float& avgRate, float& peakRate) const {
    if (startIdx > endIdx || endIdx >= firingRates.size()) {
        avgRate = 0.0f;
        peakRate = 0.0f;
        return 0.0f;
    }
    
    float sum = 0.0f;
    peakRate = 0.0f;
    
    for (size_t i = startIdx; i <= endIdx; ++i) {
        sum += firingRates[i];
        if (firingRates[i] > peakRate) {
            peakRate = firingRates[i];
        }
    }
    
    avgRate = sum / static_cast<float>(endIdx - startIdx + 1);
    return avgRate;
}

void ActivityAnalyzer::computePowerSpectrum(const std::vector<float>& signal,
                                           std::vector<float>& powerSpectrum,
                                           float samplingRate) const {
    // Compute power spectrum using FFT
    size_t signalLength = signal.size();
    size_t spectrumSize = powerSpectrum.size();
    
    if (signalLength < 2 || spectrumSize < 2) {
        return;
    }
    
    // This is a placeholder - real implementation would use FFT
    // For now, we'll generate a simulated power spectrum
    for (size_t i = 0; i < spectrumSize; ++i) {
        float freq = static_cast<float>(i) * samplingRate / spectrumSize;
        float amplitude = 1.0f / (1.0f + freq * freq);
        powerSpectrum[i] = amplitude * amplitude;
    }
}

void ActivityAnalyzer::computeFFT(const std::vector<float>& real, const std::vector<float>& imag,
                                 std::vector<float>& spectrum) const {
    // Placeholder for FFT computation
    spectrum.resize(real.size(), 0.0f);
}

void ActivityAnalyzer::computeRecurrencePlot(const std::vector<float>& signal,
                                           std::vector<std::vector<bool>>& recurrencePlot) const {
    // Compute recurrence plot from signal
    size_t signalLength = signal.size();
    recurrencePlot.resize(signalLength, std::vector<bool>(signalLength, false));
    
    float epsilon = 0.1f; // Threshold distance
    
    for (size_t i = 0; i < signalLength; ++i) {
        for (size_t j = 0; j < signalLength; ++j) {
            if (std::abs(signal[i] - signal[j]) < epsilon) {
                recurrencePlot[i][j] = true;
            }
        }
    }
}

void ActivityAnalyzer::computeRecurrenceQuantification(const std::vector<std::vector<bool>>& recurrencePlot,
                                                      float& determinism,
                                                      float& laminarity,
                                                      float& entropy) const {
    // Compute recurrence quantification analysis metrics
    if (recurrencePlot.empty()) {
        determinism = 0.0f;
        laminarity = 0.0f;
        entropy = 0.0f;
        return;
    }
    
    size_t totalPoints = recurrencePlot.size() * recurrencePlot.size();
    if (totalPoints == 0) return;
    
    // Compute diagonal line lengths
    std::vector<size_t> diagonalLengths;
    size_t maxLen = recurrencePlot.size();
    
    // This is a simplified calculation
    size_t numDiagonalLines = std::min(maxLen / 2, static_cast<size_t>(10));
    for (size_t len = 1; len <= numDiagonalLines; ++len) {
        size_t count = 0;
        for (size_t i = 0; i < maxLen - len; ++i) {
            for (size_t j = 0; j < maxLen - len; ++j) {
                bool isDiagonal = true;
                for (size_t k = 0; k < len; ++k) {
                    if (!recurrencePlot[i + k][j + k]) {
                        isDiagonal = false;
                        break;
                    }
                }
                if (isDiagonal) count++;
            }
        }
        diagonalLengths.push_back(count);
    }
    
    // Compute RQA metrics
    float totalPointsInRecurrence = 0.0f;
    for (const auto& row : recurrencePlot) {
        for (bool point : row) {
            if (point) totalPointsInRecurrence++;
        }
    }
    
    if (totalPointsInRecurrence < 1e-10f) {
        determinism = 0.0f;
        laminarity = 0.0f;
        entropy = 0.0f;
        return;
    }
    
    // Compute determinism (proportion of diagonal lines)
    size_t totalDiagonalPoints = 0;
    for (size_t len : diagonalLengths) {
        totalDiagonalPoints += len;
    }
    determinism = static_cast<float>(totalDiagonalPoints) / totalPointsInRecurrence;
    
    // Compute laminarity (proportion of long diagonal lines)
    size_t longDiagonalPoints = 0;
    for (size_t i = 0; i < diagonalLengths.size(); ++i) {
        if (i >= 5) { // Consider lines of length 5 or more as long
            longDiagonalPoints += diagonalLengths[i];
        }
    }
    laminarity = static_cast<float>(longDiagonalPoints) / totalDiagonalPoints;
    
    // Compute entropy
    float entropySum = 0.0f;
    for (size_t len : diagonalLengths) {
        if (len > 0) {
            float proportion = static_cast<float>(len) / totalDiagonalPoints;
            entropySum -= proportion * std::log(proportion);
        }
    }
    entropy = entropySum;
}

void ActivityAnalyzer::normalizeForVisualization(std::vector<float>& data, float minVal, float maxVal) const {
    if (data.empty()) return;
    
    // Normalize data to [0, 1] range
    if (maxVal - minVal < 1e-10f) {
        for (float& value : data) {
            value = 0.5f;
        }
        return;
    }
    
    for (float& value : data) {
        value = (value - minVal) / (maxVal - minVal);
    }
}

void ActivityAnalyzer::quantizeForVisualization(std::vector<float>& data, size_t numQuantiles) const {
    if (data.empty() || numQuantiles < 2) return;
    
    // Quantize data into discrete levels
    std::vector<float> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());
    
    size_t numValues = data.size();
    std::vector<size_t> quantileBoundaries;
    for (size_t i = 1; i < numQuantiles; ++i) {
        quantileBoundaries.push_back(static_cast<size_t>(static_cast<float>(i) * numValues / numQuantiles));
    }
    
    // Assign quantized values
    for (float& value : data) {
        size_t quantileIndex = 0;
        for (size_t boundary : quantileBoundaries) {
            if (quantileIndex < sortedData.size() && value > sortedData[boundary]) {
                quantileIndex++;
            }
        }
        value = static_cast<float>(quantileIndex) / static_cast<float>(numQuantiles - 1);
    }
}

void ActivityAnalyzer::applyColormap(const std::vector<float>& data, std::vector<Color>& colors) const {
    colors.resize(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        float normalized = std::max(0.0f, std::min(1.0f, data[i]));
        
        // Apply viridis colormap
        float hue = 0.267f + 0.733f * normalized;
        float saturation = 0.8f + 0.2f * std::sin(2.0f * M_PI * hue);
        float lightness = 0.5f + 0.25f * std::cos(2.0f * M_PI * hue);
        
        // Convert HSL to RGB
        float r, g, b;
        if (saturation < 1e-10f) {
            r = g = b = lightness;
        } else {
            float q = lightness < 0.5f ? lightness * (1.0f + saturation) : lightness + saturation - lightness * saturation;
            float p = 2.0f * lightness - q;
            float hk = hue * 6.0f;
            if (hk < 1.0f) {
                r = q; g = p + (q - p) * hk; b = p;
            } else if (hk < 2.0f) {
                r = (q - p) * (hk - 1.0f) + p; g = q; b = p;
            } else if (hk < 3.0f) {
                r = p; g = q; b = (q - p) * (hk - 2.0f) + p;
            } else if (hk < 4.0f) {
                r = p; g = (q - p) * (hk - 3.0f) + p; b = q;
            } else if (hk < 5.0f) {
                r = (q - p) * (hk - 4.0f) + p; g = p; b = q;
            } else {
                r = q; g = p; b = (q - p) * (hk - 5.0f) + p;
            }
        }
        
        colors[i] = {r, g, b, 1.0f};
    }
}

float ActivityAnalyzer::computeMean(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / static_cast<float>(values.size());
}

float ActivityAnalyzer::computeVariance(const std::vector<float>& values, float mean) const {
    if (values.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float value : values) {
        float deviation = value - mean;
        sum += deviation * deviation;
    }
    
    return sum / static_cast<float>(values.size());
}

float ActivityAnalyzer::computeStandardDeviation(float variance) const {
    return std::sqrt(variance);
}

float ActivityAnalyzer::computeSkewness(const std::vector<float>& values, float mean, float stddev) const {
    if (values.empty() || stddev < 1e-10f) return 0.0f;
    
    float sum = 0.0f;
    for (float value : values) {
        float deviation = value - mean;
        sum += deviation * deviation * deviation;
    }
    
    return sum / (static_cast<float>(values.size()) * stddev * stddev * stddev);
}

float ActivityAnalyzer::computeKurtosis(const std::vector<float>& values, float mean, float stddev) const {
    if (values.empty() || stddev < 1e-10f) return 0.0f;
    
    float sum = 0.0f;
    for (float value : values) {
        float deviation = value - mean;
        sum += deviation * deviation * deviation * deviation;
    }
    
    float kurt = sum / (static_cast<float>(values.size()) * stddev * stddev * stddev * stddev);
    return kurt - 3.0f; // Excess kurtosis
}

void ActivityAnalyzer::computeCrossCorrelation(const std::vector<float>& signal1,
                                              const std::vector<float>& signal2,
                                              std::vector<float>& crossCorrelation,
                                              std::vector<float>& lags) const {
    // Compute cross-correlation between two signals
    if (signal1.empty() || signal2.empty() || signal1.size() != signal2.size()) {
        return;
    }
    
    size_t signalLength = signal1.size();
    crossCorrelation.resize(signalLength * 2 - 1, 0.0f);
    lags.resize(crossCorrelation.size(), 0.0f);
    
    // This is a simplified cross-correlation calculation
    // Real implementation would use FFT for efficiency
    for (size_t lag = -static_cast<ptrdiff_t>(signalLength) + 1; lag < static_cast<ptrdiff_t>(signalLength); ++lag) {
        size_t lagIndex = static_cast<size_t>(lag + signalLength - 1);
        float correlation = 0.0f;
        
        for (size_t i = 0; i < signalLength; ++i) {
            size_t j = static_cast<size_t>(i + lag);
            if (j < signalLength) {
                correlation += signal1[i] * signal2[j];
            }
        }
        
        crossCorrelation[lagIndex] = correlation;
        lags[lagIndex] = static_cast<float>(lag);
    }
}

void ActivityAnalyzer::exportTimeSeriesData(const std::string& filepath) const {
    // Export time series data to file
    // This would use a file I/O library (JSON, CSV, etc.)
    // Placeholder implementation
}

void ActivityAnalyzer::exportActivityStats(const std::string& filepath) const {
    // Export activity statistics to file
    // This would use a file I/O library (JSON, CSV, etc.)
    // Placeholder implementation
}

void ActivityAnalyzer::exportPatternDatabase(const std::string& filepath) const {
    // Export pattern database to file
    // This would use a file I/O library (JSON, CSV, etc.)
    // Placeholder implementation
}

void ActivityAnalyzer::exportNetworkMetrics(const std::string& filepath) const {
    // Export network metrics to file
    // This would use a file I/O library (JSON, CSV, etc.)
    // Placeholder implementation
}

void ActivityAnalyzer::prepareForRealTimeVisualization(std::vector<float>& visualizationData) const {
    // Prepare data for real-time visualization (streaming)
    // This would apply real-time optimizations
}

void ActivityAnalyzer::prepareForStaticVisualization(const std::string& format, std::vector<float>& visualizationData) const {
    // Prepare data for static visualization (export)
    // This would apply format-specific optimizations
}

} // namespace nlm
