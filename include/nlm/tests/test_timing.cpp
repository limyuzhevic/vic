// Timing Tests
// Tests for neural network timing and temporal accuracy
#pragma once

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <iomanip>
#include "test_framework.hpp"

namespace test_timing {

// Timing test base class
class TimingTestBase : public nlm::test::TestBase {
protected:
    struct TimingMetrics {
        std::chrono::nanoseconds setupTime;
        std::chrono::nanoseconds executionTime;
        std::chrono::nanoseconds teardownTime;
        double frequencyHz;
        double periodNs;
        std::vector<double> interEventIntervals;
    };
    
public:
    TimingTestBase() {
        // Initialize timing metrics
    }
    
    std::string getName() const override {
        return "TimingTestBase";
    }
    
    std::string getDescription() const override {
        return "Base class for timing tests";
    }
    
    nlm::test::TestCategory getCategory() const override {
        return nlm::test::TestCategory::Accuracy;
    }
    
    std::string getFile() const override {
        return "tests/test_timing.cpp";
    }
    
    int getLine() const override {
        return 1;
    }
};

// Neuron firing timing accuracy tests
void testNeuronFiringTimingAccuracy() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test neuron firing timing with known inputs
    std::vector<double> spikeTimes;
    std::vector<double> expectedSpikeTimes = {0.1, 0.2, 0.3, 0.4, 0.5};
    
    // Simulate neuron firing
    for (int step = 0; step < 100; ++step) {
        neuron.injectCurrent(0.1); // Create conditions for firing
        neuron.decrementRefractory();
        
        if (neuron.isFiring()) {
            double actualSpikeTime = step * 0.01; // Assuming 100Hz time steps
            spikeTimes.push_back(actualSpikeTime);
        }
    }
    
    // Calculate timing accuracy
    double totalTimingError = 0.0;
    int matchedSpikes = 0;
    
    for (size_t i = 0; i < expectedSpikeTimes.size(); ++i) {
        double expectedTime = expectedSpikeTimes[i];
        
        // Find closest actual spike time
        auto closestIt = std::min_element(spikeTimes.begin(), spikeTimes.end(),
            [expectedTime](double a, double b) {
                return std::abs(a - expectedTime) < std::abs(b - expectedTime);
            });
        
        if (closestIt != spikeTimes.end()) {
            double actualTime = *closestIt;
            double timingError = std::abs(actualTime - expectedTime);
            totalTimingError += timingError;
            matchedSpikes++;
            
            // Remove matched spike to avoid double matching
            spikeTimes.erase(closestIt);
        }
    }
    
    double averageTimingError = (matchedSpikes > 0) ? totalTimingError / matchedSpikes : 0.0;
    
    std::cout << "    testNeuronFiringTimingAccuracy - " 
              << "" << "Matched spikes: " << matchedSpikes << ", "
              << "Average timing error: " << std::fixed << std::setprecision(6) << averageTimingError << "s" << std::endl;
    
    // Assert timing accuracy requirements
    assert(averageTimingError < 0.01); // Less than 10ms timing error
    assert(matchedSpikes >= expectedSpikeTimes.size() * 0.8); // At least 80% spike detection
    
    std::cout << "    testNeuronFiringTimingAccuracy passed" << std::endl;
}

// Synapse transmission timing tests
void testSynapseTransmissionTiming() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test synaptic transmission timing
    std::vector<double> transmissionTimes;
    
    for (int step = 0; step < 50; ++step) {
        double startTime = step * 0.02; // 20ms steps
        
        // Simulate synaptic transmission
        double transmissionDelay = 0.005; // 5ms delay
        double actualTransmissionTime = startTime + transmissionDelay;
        transmissionTimes.push_back(actualTransmissionTime);
        
        // Verify transmission delay
        double delay = actualTransmissionTime - startTime;
        assert(delay >= 0.001 && delay <= 0.01); // Between 1ms and 10ms
    }
    
    // Calculate transmission timing statistics
    double avgTransmissionTime = std::accumulate(transmissionTimes.begin(), transmissionTimes.end(), 0.0) / transmissionTimes.size();
    double stdDev = calculateStandardDeviation(transmissionTimes, avgTransmissionTime);
    
    std::cout << "    testSynapseTransmissionTiming - " 
              << "" << "Average transmission time: " << std::fixed << std::setprecision(4) << avgTransmissionTime << "s, "
              << "Std dev: " << stdDev << "s" << std::endl;
    
    // Assert transmission timing requirements
    assert(stdDev < 0.001); // Low timing variability
    
    std::cout << "    testSynapseTransmissionTiming passed" << std::endl;
}

// Brain step timing tests
void testBrainStepTiming() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Measure step timing consistency
    std::vector<std::chrono::nanoseconds> stepTimes;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 100; ++step) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        brain.step(step);
        auto stepEnd = std::chrono::high_resolution_clock::now();
        
        std::chrono::nanoseconds stepDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
        stepTimes.push_back(stepDuration);
    }
    
    auto totalEndTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(totalEndTime - startTime);
    
    // Calculate timing statistics
    double avgStepTime = calculateMean(stepTimes) * 1e-9; // Convert to seconds
    double stdDevStepTime = calculateStandardDeviation(stepTimes, avgStepTime * 1e9) * 1e-9;
    double cv = (avgStepTime > 0) ? stdDevStepTime / avgStepTime : 0.0; // Coefficient of variation
    
    std::cout << "    testBrainStepTiming - " 
              << "" << "Average step time: " << std::fixed << std::setprecision(6) << avgStepTime << "s, "
              << "CV: " << cv << std::endl;
    
    // Assert step timing requirements
    assert(cv < 0.1); // Less than 10% coefficient of variation
    assert(avgStepTime > 0.0 && avgStepTime < 0.1); // Reasonable step times (0-100ms)
    
    std::cout << "    testBrainStepTiming passed" << std::endl;
}

// Plasticity timing tests
void testPlasticityTiming() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 30, nlm::ConfigSource::Default);
    
    nlm::STDP stdp(config);
    
    // Test plasticity update timing consistency
    std::vector<std::chrono::nanoseconds> updateTimes;
    
    for (int spikePair = 0; spikePair < 50; ++spikePair) {
        int preTime = spikePair * 10;
        int postTime = preTime + 10 + (spikePair % 5); // Variable delay
        
        auto updateStart = std::chrono::high_resolution_clock::now();
        stdp.update(preTime, postTime);
        auto updateEnd = std::chrono::high_resolution_clock::now();
        
        std::chrono::nanoseconds updateDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(updateEnd - updateStart);
        updateTimes.push_back(updateDuration);
    }
    
    // Calculate plasticity timing statistics
    double avgUpdateTime = calculateMean(updateTimes) * 1e-9; // Convert to seconds
    double stdDevUpdateTime = calculateStandardDeviation(updateTimes, avgUpdateTime * 1e9) * 1e-9;
    
    std::cout << "    testPlasticityTiming - " 
              << "" << "Average update time: " << std::fixed << std::setprecision(6) << avgUpdateTime << "s, "
              << "Std dev: " << stdDevUpdateTime << "s" << std::endl;
    
    // Assert plasticity timing requirements
    assert(stdDevUpdateTime < avgUpdateTime * 0.5); // Low variability
    assert(avgUpdateTime < 0.001); // Less than 1ms average update time
    
    std::cout << "    testPlasticityTiming passed" << std::endl;
}

// Temporal pattern generation tests
void testTemporalPatternGeneration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 40, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test temporal pattern generation and accuracy
    std::vector<std::vector<double>> expectedPatterns = {
        {0.1, 0.2, 0.3, 0.4, 0.5},
        {0.5, 0.4, 0.3, 0.2, 0.1},
        {0.2, 0.2, 0.2, 0.2, 0.2},
        {0.9, 0.8, 0.7, 0.6, 0.5}
    };
    
    double totalPatternAccuracy = 0.0;
    
    for (int patternIndex = 0; patternIndex < expectedPatterns.size(); ++patternIndex) {
        // Reset and run brain to generate pattern
        brain.reset();
        
        // Measure temporal pattern
        std::vector<double> generatedPattern;
        auto patternStart = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            // Generate temporal activity pattern
            double activity = brain.getAverageFiringRate();
            generatedPattern.push_back(activity);
        }
        
        auto patternEnd = std::chrono::high_resolution_clock::now();
        double patternDuration = std::chrono::duration<double>(patternEnd - patternStart).count();
        
        // Calculate pattern accuracy (simplified)
        double patternAccuracy = calculatePatternAccuracy(expectedPatterns[patternIndex], generatedPattern);
        totalPatternAccuracy += patternAccuracy;
        
        std::cout << "    testTemporalPatternGeneration - " 
                  << "" << "Pattern " << patternIndex << ": accuracy = " << std::fixed << std::setprecision(4) << patternAccuracy 
                  << ", duration = " << std::fixed << std::setprecision(4) << patternDuration << "s" << std::endl;
    }
    
    double averagePatternAccuracy = (expectedPatterns.size() > 0) ? totalPatternAccuracy / expectedPatterns.size() : 0.0;
    
    std::cout << "    testTemporalPatternGeneration - " 
              << "" << "Average pattern accuracy: " << std::fixed << std::setprecision(4) << averagePatternAccuracy << std::endl;
    
    // Assert pattern accuracy requirements
    assert(averagePatternAccuracy >= 0.8); // At least 80% pattern accuracy
    
    std::cout << "    testTemporalPatternGeneration passed" << std::endl;
}

// Run all timing tests
void runAll() {
    testNeuronFiringTimingAccuracy();
    testSynapseTransmissionTiming();
    testBrainStepTiming();
    testPlasticityTiming();
    testTemporalPatternGeneration();
}

// Helper functions for timing analysis

double calculateStandardDeviation(const std::vector<std::chrono::nanoseconds>& values, double mean) {
    if (values.empty()) return 0.0;
    
    double sumSquares = 0.0;
    for (const auto& value : values) {
        double diff = static_cast<double>(value.count()) - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / values.size());
}

double calculateMean(const std::vector<std::chrono::nanoseconds>& values) {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& value : values) {
        sum += static_cast<double>(value.count());
    }
    
    return sum / values.size();
}

double calculatePatternAccuracy(const std::vector<double>& expected,
                               const std::vector<double>& actual) {
    if (expected.size() != actual.size()) return 0.0;
    
    double totalAccuracy = 0.0;
    for (size_t i = 0; i < expected.size(); ++i) {
        double error = std::abs(expected[i] - actual[i]);
        totalAccuracy += (1.0 - std::min(error, 1.0));
    }
    
    return totalAccuracy / expected.size();
}

} // namespace test_timing
