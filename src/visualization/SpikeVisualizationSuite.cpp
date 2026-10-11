// SpikeVisualizationSuite.cpp - Spike visualization test suite implementation

#include "SpikeVisualizationSuite.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "SpikeEvent.hpp"
#include "SpikeSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>
#include <random>

namespace nlm {

SpikeVisualizationSuite::SpikeVisualizationSuite() {
    // Initialize test configuration
    std::string currentDir = ".";
    std::string tempFile = currentDir + "/test_spike_data.bin";
    generateTestSpikeData(tempFile);
    
    // Load test data
    loadTestData(tempFile);
    
    // Generate test conditions
    generateTestConditions();
    
    // Initialize test results
    resetTestResults();
}

SpikeVisualizationSuite::~SpikeVisualizationSuite() {
}

void SpikeVisualizationSuite::setTestConfiguration(const TestConfiguration& config) {
    pImpl->config = config;
}

void SpikeVisualizationSuite::setTestData(const std::vector<SpikeEvent>& spikeData) {
    pImpl->testSpikeData = spikeData;
}

void SpikeVisualizationSuite::setTestConditions(const std::vector<TestCondition>& conditions) {
    pImpl->testConditions = conditions;
}

void SpikeVisualizationSuite::addTestCondition(const TestCondition& condition) {
    pImpl->testConditions.push_back(condition);
}

SpikeVisualizationSuite::TestResult SpikeVisualizationSuite::runTest(const TestCondition& condition) {
    TestResult result;
    result.condition = condition;
    result.startTime = std::chrono::system_clock::now();
    
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // Create spike visualizer
        SpikeVisualizer::SpikeVisualizationConfig spikeConfig;
        spikeConfig.mode = SpikeVisualizer::VisualizationMode::RASTER_PLOT;
        spikeConfig.timeScale = 1.0;
        spikeConfig.binWidth = 0.001;
        spikeConfig.showIndividualSpikes = true;
        spikeConfig.detectSpikes = condition.useSpikeDetection;
        spikeConfig.spikeThreshold = condition.spikeThreshold;
        
        SpikeVisualizer spikeVisualizer(spikeConfig);
        
        // Initialize
        if (!spikeVisualizer.initialize(pImpl->brainData)) {
            throw std::runtime_error("Failed to initialize spike visualizer");
        }
        
        // Run test based on condition
        if (condition.testType == TestType::RASTER_ANALYSIS) {
            result = runRasterAnalysisTest(spikeVisualizer, condition);
        } else if (condition.testType == TestType::FREQUENCY_ANALYSIS) {
            result = runFrequencyAnalysisTest(spikeVisualizer, condition);
        } else if (condition.testType == TestType::SYNCHRONY_ANALYSIS) {
            result = runSynchronyAnalysisTest(spikeVisualizer, condition);
        } else if (condition.testType == TestType::CORRELATION_ANALYSIS) {
            result = runCorrelationAnalysisTest(spikeVisualizer, condition);
        } else if (condition.testType == TestType::STATISTICAL_ANALYSIS) {
            result = runStatisticalAnalysisTest(spikeVisualizer, condition);
        } else {
            throw std::runtime_error("Unsupported test type: " + condition.testType);
        }
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.endTime = endTime;
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate statistics
    result.passed = (result.status == TestStatus::PASSED);
    result.failed = (result.status == TestStatus::FAILED);
    result.skipped = (result.status == TestStatus::SKIPPED);
    
    NLM_LOG_INFO("Test completed: " + condition.name + " (status: " + testStatusToString(result.status) + ", duration: " + std::to_string(result.duration) + "s)");
    
    return result;
}

std::vector<SpikeVisualizationSuite::TestResult> SpikeVisualizationSuite::runAllTests() {
    std::vector<TestResult> results;
    
    // Run tests for each condition
    for (const auto& condition : pImpl->testConditions) {
        TestResult result = runTest(condition);
        results.push_back(result);
        
        // Small delay between tests
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Calculate summary statistics
    calculateTestSummary(results);
    
    return results;
}

void SpikeVisualizationSuite::generateTestSpikeData(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return;
    }
    
    // Generate realistic spike data
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> firingRate(10.0, 5.0); // Hz
    std::exponential_distribution<double> interSpikeInterval(0.1); // 10 Hz average
    
    int numNeurons = 100;
    int totalSpikes = 0;
    
    for (int neuronId = 0; neuronId < numNeurons; ++neuronId) {
        // Neuron-specific properties
        double neuronFiringRate = firingRate(rng);
        double neuronThreshold = 0.5 + (std::rand() % 100) / 200.0; // 0.5 to 1.0
        
        // Generate spike train for this neuron
        int spikesForNeuron = static_cast<int>(neuronFiringRate * 10.0); // 10 seconds
        std::vector<double> spikeTimes;
        
        for (int i = 0; i < spikesForNeuron; ++i) {
            double spikeTime = interSpikeInterval(rng);
            spikeTimes.push_back(spikeTime);
        }
        
        // Sort spike times
        std::sort(spikeTimes.begin(), spikeTimes.end());
        
        // Write spike data
        int32_t neuronId32 = neuronId;
        int32_t spikeCount32 = spikeTimes.size();
        
        file.write(reinterpret_cast<const char*>(&neuronId32), sizeof(int32_t));
        file.write(reinterpret_cast<const char*>(&spikeCount32), sizeof(int32_t));
        
        for (double spikeTime : spikeTimes) {
            float spikeTime32 = static_cast<float>(spikeTime);
            file.write(reinterpret_cast<const char*>(&spikeTime32), sizeof(float));
        }
        
        totalSpikes += spikeTimes.size();
    }
    
    file.close();
    
    NLM_LOG_INFO("Generated test spike data: " + std::to_string(numNeurons) + " neurons, " + std::to_string(totalSpikes) + " spikes");
}

void SpikeVisualizationSuite::loadTestData(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open test data file: " + filename);
        return;
    }
    
    pImpl->testSpikeData.clear();
    
    int32_t neuronId;
    int32_t spikeCount;
    float spikeTime;
    
    while (file.read(reinterpret_cast<char*>(&neuronId), sizeof(int32_t))) {
        file.read(reinterpret_cast<char*>(&spikeCount), sizeof(int32_t));
        
        std::vector<double> spikeTimes;
        for (int i = 0; i < spikeCount; ++i) {
            file.read(reinterpret_cast<char*>(&spikeTime), sizeof(float));
            spikeTimes.push_back(spikeTime);
        }
        
        for (double spikeTime : spikeTimes) {
            SpikeEvent spike;
            spike.neuronId = neuronId;
            spike.time = spikeTime;
            spike.amplitude = 1.0; // Default amplitude
            spike.duration = 0.001; // Default duration
            pImpl->testSpikeData.push_back(spike);
        }
    }
    
    file.close();
    
    NLM_LOG_INFO("Loaded test data: " + std::to_string(pImpl->testSpikeData.size()) + " spike events");
}

void SpikeVisualizationSuite::generateTestConditions() {
    pImpl->testConditions.clear();
    
    // Create various test conditions
    std::vector<TestCondition> conditions;
    
    // Condition 1: High firing rate
    TestCondition highFiring;
    highFiring.name = "High Firing Rate";
    highFiring.description = "Test visualization with high firing rates (20-50 Hz)";
    highFiring.testType = TestType::FREQUENCY_ANALYSIS;
    highFiring.useSpikeDetection = true;
    highFiring.spikeThreshold = 0.1;
    highFiring.expectedResult = "high_firing_rate";
    highFiring.weight = 1.0;
    conditions.push_back(highFiring);
    
    // Condition 2: Low firing rate
    TestCondition lowFiring;
    lowFiring.name = "Low Firing Rate";
    lowFiring.description = "Test visualization with low firing rates (1-5 Hz)";
    lowFiring.testType = TestType::FREQUENCY_ANALYSIS;
    lowFiring.useSpikeDetection = true;
    lowFiring.spikeThreshold = 0.5;
    lowFiring.expectedResult = "low_firing_rate";
    lowFiring.weight = 1.0;
    conditions.push_back(lowFiring);
    
    // Condition 3: Synchronous activity
    TestCondition synchronous;
    synchronous.name = "Synchronous Activity";
    synchronous.description = "Test visualization with synchronous spike trains";
    synchronous.testType = TestType::SYNCHRONY_ANALYSIS;
    synchronous.useSpikeDetection = true;
    synchronous.spikeThreshold = 0.2;
    synchronous.expectedResult = "synchronous_activity";
    synchronous.weight = 1.5;
    conditions.push_back(synchronous);
    
    // Condition 4: Asynchronous activity
    TestCondition asynchronous;
    asynchronous.name = "Asynchronous Activity";
    asynchronous.description = "Test visualization with asynchronous spike trains";
    asynchronous.testType = TestType::SYNCHRONY_ANALYSIS;
    asynchronous.useSpikeDetection = true;
    asynchronous.spikeThreshold = 0.3;
    asynchronous.expectedResult = "asynchronous_activity";
    asynchronous.weight = 1.5;
    conditions.push_back(asynchronous);
    
    // Condition 5: Statistical analysis
    TestCondition statistical;
    statistical.name = "Statistical Analysis";
    statistical.description = "Test statistical analysis of spike patterns";
    statistical.testType = TestType::STATISTICAL_ANALYSIS;
    statistical.useSpikeDetection = true;
    statistical.spikeThreshold = 0.1;
    statistical.expectedResult = "statistical_analysis";
    statistical.weight = 1.0;
    conditions.push_back(statistical);
    
    // Condition 6: Raster plot analysis
    TestCondition raster;
    raster.name = "Raster Plot Analysis";
    raster.description = "Test raster plot visualization";
    raster.testType = TestType::RASTER_ANALYSIS;
    raster.useSpikeDetection = true;
    raster.spikeThreshold = 0.1;
    raster.expectedResult = "raster_plot";
    raster.weight = 1.0;
    conditions.push_back(raster);
    
    // Condition 7: Cross-correlation analysis
    TestCondition correlation;
    correlation.name = "Cross-Correlation Analysis";
    correlation.description = "Test cross-correlation analysis";
    correlation.testType = TestType::CORRELATION_ANALYSIS;
    correlation.useSpikeDetection = true;
    correlation.spikeThreshold = 0.1;
    correlation.expectedResult = "correlation_analysis";
    correlation.weight = 1.0;
    conditions.push_back(correlation);
    
    pImpl->testConditions = conditions;
}

void SpikeVisualizationSuite::resetTestResults() {
    pImpl->testResults.clear();
}

void SpikeVisualizationSuite::calculateTestSummary(std::vector<TestResult>& results) {
    int totalPassed = 0;
    int totalFailed = 0;
    int totalSkipped = 0;
    double totalDuration = 0.0;
    double totalScore = 0.0;
    
    for (const auto& result : results) {
        switch (result.status) {
            case TestStatus::PASSED:
                totalPassed++;
                break;
            case TestStatus::FAILED:
                totalFailed++;
                break;
            case TestStatus::SKIPPED:
                totalSkipped++;
                break;
        }
        
        totalDuration += result.duration;
        totalScore += result.score;
    }
    
    pImpl->summary.totalTests = results.size();
    pImpl->summary.passedTests = totalPassed;
    pImpl->summary.failedTests = totalFailed;
    pImpl->summary.skippedTests = totalSkipped;
    pImpl->summary.totalDuration = totalDuration;
    pImpl->summary.averageScore = results.empty() ? 0.0 : totalScore / results.size();
}

SpikeVisualizationSuite::TestResult SpikeVisualizationSuite::runRasterAnalysisTest(SpikeVisualizer& spikeVisualizer, const TestCondition& condition) {
    TestResult result;
    result.testName = condition.name;
    result.testType = condition.testType;
    
    try {
        // Update spike visualizer with test data
        spikeVisualizer.update(pImpl->brainData, pImpl->testSpikeData, 0.1);
        
        // Render raster plot
        if (!spikeVisualizer.render()) {
            throw std::runtime_error("Failed to render raster plot");
        }
        
        // Get spike statistics
        SpikeVisualizer::SpikeStats spikeStats = spikeVisualizer.getSpikeStats();
        
        // Analyze raster plot
        double rasterMetrics = analyzeRasterPlot(spikeStats, pImpl->testSpikeData);
        
        // Evaluate result
        double expectedMetric = getExpectedMetric(condition.expectedResult, rasterMetrics);
        double score = calculateScore(rasterMetrics, expectedMetric);
        
        result.status = (score >= pImpl->config.passingScore) ? TestStatus::PASSED : TestStatus::FAILED;
        result.message = "Raster plot analysis completed";
        result.score = score;
        result.details["raster_metrics"] = std::to_string(rasterMetrics);
        result.details["expected_metric"] = std::to_string(expectedMetric);
        result.details["score"] = std::to_string(score);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Raster analysis test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

SpikeVisualizationSuite::TestResult SpikeVisualizationSuite::runFrequencyAnalysisTest(SpikeVisualizer& spikeVisualizer, const TestCondition& condition) {
    TestResult result;
    result.testName = condition.name;
    result.testType = condition.testType;
    
    try {
        // Update spike visualizer with test data
        spikeVisualizer.update(pImpl->brainData, pImpl->testSpikeData, 0.1);
        
        // Render spike train visualization
        if (!spikeVisualizer.render()) {
            throw std::runtime_error("Failed to render spike train visualization");
        }
        
        // Get spike statistics
        SpikeVisualizer::SpikeStats spikeStats = spikeVisualizer.getSpikeStats();
        
        // Analyze firing patterns
        double frequencyMetrics = analyzeFiringPatterns(spikeStats, pImpl->testSpikeData);
        
        // Evaluate result
        double expectedMetric = getExpectedMetric(condition.expectedResult, frequencyMetrics);
        double score = calculateScore(frequencyMetrics, expectedMetric);
        
        result.status = (score >= pImpl->config.passingScore) ? TestStatus::PASSED : TestStatus::FAILED;
        result.message = "Frequency analysis completed";
        result.score = score;
        result.details["frequency_metrics"] = std::to_string(frequencyMetrics);
        result.details["expected_metric"] = std::to_string(expectedMetric);
        result.details["score"] = std::to_string(score);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Frequency analysis test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

SpikeVisualizationSuite::TestResult SpikeVisualizationSuite::runSynchronyAnalysisTest(SpikeVisualizer& spikeVisualizer, const TestCondition& condition) {
    TestResult result;
    result.testName = condition.name;
    result.testType = condition.testType;
    
    try {
        // Update spike visualizer with test data
        spikeVisualizer.update(pImpl->brainData, pImpl->testSpikeData, 0.1);
        
        // Render synchrony visualization
        if (!spikeVisualizer.render()) {
            throw std::runtime_error("Failed to render synchrony visualization");
        }
        
        // Get spike statistics
        SpikeVisualizer::SpikeStats spikeStats = spikeVisualizer.getSpikeStats();
        
        // Analyze synchronization
        double synchronyMetrics = analyzeSynchrony(spikeStats, pImpl->testSpikeData);
        
        // Evaluate result
        double expectedMetric = getExpectedMetric(condition.expectedResult, synchronyMetrics);
        double score = calculateScore(synchronyMetrics, expectedMetric);
        
        result.status = (score >= pImpl->config.passingScore) ? TestStatus::PASSED : TestStatus::FAILED;
        result.message = "Synchrony analysis completed";
        result.score = score;
        result.details["synchrony_metrics"] = std::to_string(synchronyMetrics);
        result.details["expected_metric"] = std::to_string(expectedMetric);
        result.details["score"] = std::to_string(score);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Synchrony analysis test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

SpikeVisualizationSuite::TestResult SpikeVisualizationSuite::runCorrelationAnalysisTest(SpikeVisualizer& spikeVisualizer, const TestCondition& condition) {
    TestResult result;
    result.testName = condition.name;
    result.testType = condition.testType;
    
    try {
        // Update spike visualizer with test data
        spikeVisualizer.update(pImpl->brainData, pImpl->testSpikeData, 0.1);
        
        // Render correlation visualization
        if (!spikeVisualizer.render()) {
            throw std::runtime_error("Failed to render correlation visualization");
        }
        
        // Get spike statistics
        SpikeVisualizer::SpikeStats spikeStats = spikeVisualizer.getSpikeStats();
        
        // Analyze cross-correlation
        double correlationMetrics = analyzeCrossCorrelation(spikeStats, pImpl->testSpikeData);
        
        // Evaluate result
        double expectedMetric = getExpectedMetric(condition.expectedResult, correlationMetrics);
        double score = calculateScore(correlationMetrics, expectedMetric);
        
        result.status = (score >= pImpl->config.passingScore) ? TestStatus::PASSED : TestStatus::FAILED;
        result.message = "Correlation analysis completed";
        result.score = score;
        result.details["correlation_metrics"] = std::to_string(correlationMetrics);
        result.details["expected_metric"] = std::to_string(expectedMetric);
        result.details["score"] = std::to_string(score);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Correlation analysis test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

SpikeVisualizationSuite::TestResult SpikeVisualizationSuite::runStatisticalAnalysisTest(SpikeVisualizer& spikeVisualizer, const TestCondition& condition) {
    TestResult result;
    result.testName = condition.name;
    result.testType = condition.testType;
    
    try {
        // Update spike visualizer with test data
        spikeVisualizer.update(pImpl->brainData, pImpl->testSpikeData, 0.1);
        
        // Render statistical visualization
        if (!spikeVisualizer.render()) {
            throw std::runtime_error("Failed to render statistical visualization");
        }
        
        // Get spike statistics
        SpikeVisualizer::SpikeStats spikeStats = spikeVisualizer.getSpikeStats();
        
        // Analyze statistical patterns
        double statisticalMetrics = analyzeStatisticalPatterns(spikeStats, pImpl->testSpikeData);
        
        // Evaluate result
        double expectedMetric = getExpectedMetric(condition.expectedResult, statisticalMetrics);
        double score = calculateScore(statisticalMetrics, expectedMetric);
        
        result.status = (score >= pImpl->config.passingScore) ? TestStatus::PASSED : TestStatus::FAILED;
        result.message = "Statistical analysis completed";
        result.score = score;
        result.details["statistical_metrics"] = std::to_string(statisticalMetrics);
        result.details["expected_metric"] = std::to_string(expectedMetric);
        result.details["score"] = std::to_string(score);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Statistical analysis test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

double SpikeVisualizationSuite::analyzeRasterPlot(const SpikeVisualizer::SpikeStats& stats, const std::vector<SpikeEvent>& spikes) const {
    // Analyze raster plot metrics
    double rasterScore = 0.0;
    
    // Calculate density
    double density = static_cast<double>(spikes.size()) / 100.0; // Normalized
    rasterScore += density * 0.4;
    
    // Calculate regularity (inverse of coefficient of variation)
    double regularity = 1.0 / (stats.cvFiringRate + 0.1);
    rasterScore += regularity * 0.3;
    
    // Calculate total spike count
    double spikeCountScore = std::min(stats.totalSpikes / 100.0, 1.0); // Normalized
    rasterScore += spikeCountScore * 0.3;
    
    return rasterScore;
}

double SpikeVisualizationSuite::analyzeFiringPatterns(const SpikeVisualizer::SpikeStats& stats, const std::vector<SpikeEvent>& spikes) const {
    // Analyze firing pattern metrics
    double patternScore = 0.0;
    
    // Calculate firing rate consistency
    double firingRateConsistency = 1.0 - (stats.cvFiringRate / 10.0); // Normalized
    patternScore += firingRateConsistency * 0.4;
    
    // Calculate spike count distribution
    std::unordered_map<int64_t, int> neuronSpikeCounts;
    for (const auto& spike : spikes) {
        neuronSpikeCounts[spike.neuronId]++;
    }
    
    double spikeCountConsistency = 1.0 / (stats.cvFiringRate + 0.1);
    patternScore += spikeCountConsistency * 0.3;
    
    // Calculate overall firing rate
    double firingRateScore = std::min(stats.meanFiringRate / 50.0, 1.0); // Normalized
    patternScore += firingRateScore * 0.3;
    
    return patternScore;
}

double SpikeVisualizationSuite::analyzeSynchrony(const SpikeVisualizer::SpikeStats& stats, const std::vector<SpikeEvent>& spikes) const {
    // Analyze synchrony metrics
    double synchronyScore = 0.0;
    
    // Calculate pairwise correlations (simplified)
    std::vector<double> correlations;
    int numNeurons = 10; // Simplified
    
    for (int i = 0; i < numNeurons; ++i) {
        for (int j = i + 1; j < numNeurons; ++j) {
            double correlation = calculatePairwiseCorrelation(i, j, spikes);
            correlations.push_back(std::abs(correlation));
        }
    }
    
    if (!correlations.empty()) {
        double avgCorrelation = std::accumulate(correlations.begin(), correlations.end(), 0.0) / correlations.size();
        synchronyScore += avgCorrelation * 0.7;
        
        // Calculate coherence
        double coherence = avgCorrelation * 0.8; // Simplified
        synchronyScore += coherence * 0.3;
    }
    
    return synchronyScore;
}

double SpikeVisualizationSuite::analyzeCrossCorrelation(const SpikeVisualizer::SpikeStats& stats, const std::vector<SpikeEvent>& spikes) const {
    // Analyze cross-correlation metrics
    double correlationScore = 0.0;
    
    // Calculate cross-correlation between neurons
    std::vector<double> crossCorrelations;
    int numNeurons = 10; // Simplified
    
    for (int i = 0; i < numNeurons; ++i) {
        for (int j = i + 1; j < numNeurons; ++j) {
            double correlation = calculatePairwiseCorrelation(i, j, spikes);
            crossCorrelations.push_back(correlation);
        }
    }
    
    if (!crossCorrelations.empty()) {
        double avgCorrelation = std::accumulate(crossCorrelations.begin(), crossCorrelations.end(), 0.0) / crossCorrelations.size();
        correlationScore += std::abs(avgCorrelation) * 0.8;
        
        // Calculate lag correlations
        double lagScore = calculateLagCorrelations(spikes);
        correlationScore += lagScore * 0.2;
    }
    
    return correlationScore;
}

double SpikeVisualizationSuite::analyzeStatisticalPatterns(const SpikeVisualizer::SpikeStats& stats, const std::vector<SpikeEvent>& spikes) const {
    // Analyze statistical pattern metrics
    double statisticalScore = 0.0;
    
    // Calculate distribution metrics
    std::vector<double> spikeCounts;
    std::unordered_map<int64_t, int> neuronSpikeCounts;
    
    for (const auto& spike : spikes) {
        neuronSpikeCounts[spike.neuronId]++;
    }
    
    for (const auto& pair : neuronSpikeCounts) {
        spikeCounts.push_back(pair.second);
    }
    
    if (!spikeCounts.empty()) {
        // Calculate skewness
        double mean = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0) / spikeCounts.size();
        double variance = 0.0;
        for (double count : spikeCounts) {
            variance += (count - mean) * (count - mean);
        }
        variance /= spikeCounts.size();
        double stdDev = std::sqrt(variance);
        
        double skewness = (stdDev > 0) ? ((spikeCounts.size() - 1) / (spikeCounts.size() * stdDev)) * std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0,
            [mean](double sum, double count) { return sum + (count - mean) * (count - mean) * (count - mean); }) / (spikeCounts.size() * variance * variance) : 0.0;
        
        statisticalScore += std::min(std::abs(skewness), 1.0) * 0.4;
        
        // Calculate kurtosis
        double kurtosis = (stdDev > 0) ? ((spikeCounts.size() * (spikeCounts.size() + 1)) / ((spikeCounts.size() - 1) * (spikeCounts.size() - 2) * (spikeCounts.size() - 3)) * 
            std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0,
                [mean](double sum, double count) { return sum + std::pow(count - mean, 4); }) / (spikeCounts.size() * variance * variance * variance)) : 0.0;
        
        statisticalScore += std::min(kurtosis, 1.0) * 0.3;
        
        // Calculate entropy
        double entropy = calculateEntropy(spikeCounts);
        statisticalScore += entropy * 0.3;
    }
    
    return statisticalScore;
}

double SpikeVisualizationSuite::calculatePairwiseCorrelation(int64_t neuron1Id, int64_t neuron2Id, const std::vector<SpikeEvent>& spikes) const {
    // Calculate correlation between two neurons
    std::vector<double> neuron1Spikes;
    std::vector<double> neuron2Spikes;
    
    for (const auto& spike : spikes) {
        if (spike.neuronId == neuron1Id) {
            neuron1Spikes.push_back(spike.time);
        } else if (spike.neuronId == neuron2Id) {
            neuron2Spikes.push_back(spike.time);
        }
    }
    
    if (neuron1Spikes.empty() || neuron2Spikes.empty()) {
        return 0.0;
    }
    
    // Simple correlation calculation
    double mean1 = std::accumulate(neuron1Spikes.begin(), neuron1Spikes.end(), 0.0) / neuron1Spikes.size();
    double mean2 = std::accumulate(neuron2Spikes.begin(), neuron2Spikes.end(), 0.0) / neuron2Spikes.size();
    
    double covariance = 0.0;
    for (double t1 : neuron1Spikes) {
        for (double t2 : neuron2Spikes) {
            covariance += (t1 - mean1) * (t2 - mean2);
        }
    }
    covariance /= neuron1Spikes.size() * neuron2Spikes.size();
    
    double stdDev1 = 0.0, stdDev2 = 0.0;
    for (double t : neuron1Spikes) {
        stdDev1 += (t - mean1) * (t - mean1);
    }
    stdDev1 = std::sqrt(stdDev1 / neuron1Spikes.size());
    
    for (double t : neuron2Spikes) {
        stdDev2 += (t - mean2) * (t - mean2);
    }
    stdDev2 = std::sqrt(stdDev2 / neuron2Spikes.size());
    
    return (stdDev1 * stdDev2 > 0) ? covariance / (stdDev1 * stdDev2) : 0.0;
}

double SpikeVisualizationSuite::calculateLagCorrelations(const std::vector<SpikeEvent>& spikes) const {
    // Calculate lag correlations between spike trains
    double lagScore = 0.0;
    int lagCount = 0;
    
    // Simplified implementation
    for (int lag = -5; lag <= 5; ++lag) {
        if (lag != 0) {
            double correlation = calculateLagCorrelation(spikes, lag);
            lagScore += std::abs(correlation);
            lagCount++;
        }
    }
    
    return lagCount > 0 ? lagScore / lagCount : 0.0;
}

double SpikeVisualizationSuite::calculateLagCorrelation(const std::vector<SpikeEvent>& spikes, int lag) const {
    // Calculate correlation with lag
    std::unordered_map<int64_t, std::vector<double>> neuronSpikes;
    
    for (const auto& spike : spikes) {
        neuronSpikes[spike.neuronId].push_back(spike.time);
    }
    
    if (neuronSpikes.size() < 2) return 0.0;
    
    std::vector<int64_t> neuronIds;
    for (const auto& pair : neuronSpikes) {
        neuronIds.push_back(pair.first);
    }
    
    // Calculate correlation with lag
    double totalCorrelation = 0.0;
    int correlationCount = 0;
    
    for (size_t i = 0; i < neuronIds.size(); ++i) {
        for (size_t j = i + 1; j < neuronIds.size(); ++j) {
            std::vector<double> spikes1 = neuronSpikes[neuronIds[i]];
            std::vector<double> spikes2 = neuronSpikes[neuronIds[j]];
            
            // Apply lag (shift spikes2)
            for (auto& t : spikes2) {
                t += lag * 0.001; // 1 ms lag
            }
            
            double correlation = calculatePairwiseCorrelation(neuronIds[i], neuronIds[j], spikes);
            totalCorrelation += std::abs(correlation);
            correlationCount++;
        }
    }
    
    return correlationCount > 0 ? totalCorrelation / correlationCount : 0.0;
}

double SpikeVisualizationSuite::calculateEntropy(const std::vector<double>& values) const {
    // Calculate Shannon entropy
    if (values.empty()) return 0.0;
    
    // Normalize values
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    std::vector<double> probabilities;
    for (double value : values) {
        probabilities.push_back(value / sum);
    }
    
    double entropy = 0.0;
    for (double prob : probabilities) {
        if (prob > 0) {
            entropy -= prob * std::log(prob);
        }
    }
    
    return entropy;
}

double SpikeVisualizationSuite::getExpectedMetric(const std::string& resultType, double actualMetric) const {
    // Get expected metric for test result type
    if (resultType == "high_firing_rate") {
        return 0.8;
    } else if (resultType == "low_firing_rate") {
        return 0.2;
    } else if (resultType == "synchronous_activity") {
        return 0.7;
    } else if (resultType == "asynchronous_activity") {
        return 0.3;
    } else if (resultType == "statistical_analysis") {
        return 0.6;
    } else if (resultType == "raster_plot") {
        return 0.7;
    } else if (resultType == "correlation_analysis") {
        return 0.5;
    }
    
    return 0.5; // Default
}

double SpikeVisualizationSuite::calculateScore(double actual, double expected) const {
    // Calculate test score based on actual vs expected metric
    double difference = std::abs(actual - expected);
    double score = 1.0 - difference;
    return std::max(0.0, std::min(score, 1.0));
}

std::string SpikeVisualizationSuite::testStatusToString(TestStatus status) const {
    switch (status) {
        case TestStatus::PASSED: return "PASSED";
        case TestStatus::FAILED: return "FAILED";
        case TestStatus::SKIPPED: return "SKIPPED";
        default: return "UNKNOWN";
    }
}

} // namespace nlm
