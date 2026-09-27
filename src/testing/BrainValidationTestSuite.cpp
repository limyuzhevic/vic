// Brain Validation Test Framework Implementation for NLM

#include "BrainValidationTestSuite.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace testing {

BrainValidationTestSuite::BrainValidationTestSuite() {
    std::cout << "=== NLM Brain Validation Test Suite Initialization ===" << std::endl;
    sessionStartTime_ = std::chrono::high_resolution_clock::now();
    std::cout << "Brain validation test suite initialized successfully" << std::endl;
}

BrainValidationTestSuite::~BrainValidationTestSuite() {
    std::cout << "=== NLM Brain Validation Test Suite Shutdown ===" << std::endl;
}

void BrainValidationTestSuite::registerBrainValidationTest(const std::string& name,
                                                         const std::string& description,
                                                         std::function<bool(const BrainStateSnapshot&)> testFunction,
                                                         bool isCritical) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    brainValidationTests_.push_back(testFunction);
    testResults_[name] = false;
    testLogs_[name] = "Registered brain validation test: " + description + " (critical: " + 
                      std::string(isCritical ? "yes" : "no") + ")";
    std::cout << "Registered brain validation test: " << name << std::endl;
}

void BrainValidationTestSuite::registerNeuralDynamicsTest(const std::string& name,
                                                         const std::string& description,
                                                         std::function<bool(const NeuralDynamicsMeasurement&)> testFunction) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    neuralDynamicsTests_.push_back(testFunction);
    testResults_[name] = false;
    testLogs_[name] = "Registered neural dynamics test: " + description;
    std::cout << "Registered neural dynamics test: " << name << std::endl;
}

void BrainValidationTestSuite::registerSpikePatternTest(const std::string& name,
                                                       const std::string& description,
                                                       const std::vector<double>& expectedPattern,
                                                       std::function<bool(const std::vector<double>&)> testFunction) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    spikePatternTests_.push_back({name, expectedPattern, testFunction, description});
    testResults_[name] = false;
    testLogs_[name] = "Registered spike pattern test: " + description;
    std::cout << "Registered spike pattern test: " << name << std::endl;
}

void BrainValidationTestSuite::registerNetworkDynamicsTest(const std::string& name,
                                                          const std::string& description,
                                                          std::function<bool(double)> testFunction) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    networkDynamicsTests_.push_back(testFunction);
    testResults_[name] = false;
    testLogs_[name] = "Registered network dynamics test: " + description;
    std::cout << "Registered network dynamics test: " << name << std::endl;
}

void BrainValidationTestSuite::registerCrossRegionTest(const std::string& name,
                                                      const std::string& description,
                                                      std::function<bool(const std::unordered_map<size_t, std::vector<double>>&)> testFunction) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    crossRegionTests_.push_back(testFunction);
    testResults_[name] = false;
    testLogs_[name] = "Registered cross-region test: " + description;
    std::cout << "Registered cross-region test: " << name << std::endl;
}

void BrainValidationTestSuite::runBrainValidationTests() {
    std::cout << "\n=== Running NLM Brain Validation Tests ===" << std::endl;
    
    // Check if there's brain state data available
    if (brainStateHistory_.empty()) {
        std::cout << "Warning: No brain state data available. Tests will be skipped." << std::endl;
        generateBrainValidationReport();
        return;
    }
    
    // Use the most recent brain state for testing
    BrainStateSnapshot latestState = brainStateHistory_.back();
    
    size_t passedTests = 0, failedTests = 0;
    
    // Run brain validation tests
    for (size_t i = 0; i < brainValidationTests_.size(); ++i) {
        std::string testName = (i < static_cast<size_t>(brainValidationTests_.size()) ? 
                               "brain_test_" + std::to_string(i) : "unknown");
        
        std::cout << "\nRunning brain validation test: " << testName << std::endl;
        
        bool testSuccess = executeBrainValidationTest(testName, brainValidationTests_[i], latestState);
        
        if (testSuccess) {
            std::cout << "  PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "  FAILED" << std::endl;
            failedTests++;
        }
    }
    
    // Run neural dynamics tests
    for (size_t i = 0; i < neuralDynamicsTests_.size(); ++i) {
        std::string testName = (i < static_cast<size_t>(neuralDynamicsTests_.size()) ? 
                               "neural_dynamics_test_" + std::to_string(i) : "unknown");
        
        std::cout << "\nRunning neural dynamics test: " << testName << std::endl;
        
        // Calculate neural dynamics from spike times
        NeuralDynamicsMeasurement dynamics;
        if (!latestState.neuronFiringRates.empty()) {
            // Simple dynamics calculation for demonstration
            dynamics.firingRate = 0.0;
            for (const auto& pair : latestState.neuronFiringRates) {
                dynamics.firingRate += pair.second;
            }
            dynamics.firingRate /= latestState.neuronFiringRates.size();
            dynamics.averageFiringRate = dynamics.firingRate;
            dynamics.synchronyIndex = 0.5; // Placeholder
        }
        
        bool testSuccess = executeNeuralDynamicsTest(testName, neuralDynamicsTests_[i], dynamics);
        
        if (testSuccess) {
            std::cout << "  PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "  FAILED" << std::endl;
            failedTests++;
        }
    }
    
    // Run spike pattern tests
    for (const auto& spikeTest : spikePatternTests_) {
        std::cout << "\nRunning spike pattern test: " << spikeTest.name << std::endl;
        std::cout << "  Description: " << spikeTest.description << std::endl;
        
        // Generate spike pattern for testing (simplified)
        std::vector<double> testSpikes;
        if (!latestState.neuronFiringRates.empty()) {
            for (size_t i = 0; i < 100; ++i) { // Generate 100 spike times
                double time = static_cast<double>(i) * 10.0 + rand() % 100;
                testSpikes.push_back(time);
            }
        }
        
        bool testSuccess = spikeTest.testFunction(testSpikes);
        
        if (testSuccess) {
            std::cout << "  PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "  FAILED" << std::endl;
            failedTests++;
        }
    }
    
    // Run network dynamics tests
    for (size_t i = 0; i < networkDynamicsTests_.size(); ++i) {
        std::string testName = (i < static_cast<size_t>(networkDynamicsTests_.size()) ? 
                               "network_dynamics_test_" + std::to_string(i) : "unknown");
        
        std::cout << "\nRunning network dynamics test: " << testName << std::endl;
        
        double synchronyIndex = 0.5; // Placeholder value
        
        bool testSuccess = networkDynamicsTests_[i](synchronyIndex);
        
        if (testSuccess) {
            std::cout << "  PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "  FAILED" << std::endl;
            failedTests++;
        }
    }
    
    // Run cross-region tests
    for (size_t i = 0; i < crossRegionTests_.size(); ++i) {
        std::string testName = (i < static_cast<size_t>(crossRegionTests_.size()) ? 
                               "cross_region_test_" + std::to_string(i) : "unknown");
        
        std::cout << "\nRunning cross-region test: " << testName << std::endl;
        
        // Prepare cross-region data from brain state
        std::unordered_map<size_t, std::vector<double>> regionData;
        for (const auto& pair : latestState.regionNeuronIds) {
            regionData[pair.first] = std::vector<double>(pair.second.size(), 0.5);
        }
        
        bool testSuccess = crossRegionTests_[i](regionData);
        
        if (testSuccess) {
            std::cout << "  PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "  FAILED" << std::endl;
            failedTests++;
        }
    }
    
    std::cout << "\n=== Brain Validation Test Summary ===" << std::endl;
    std::cout << "Total Tests: " << (brainValidationTests_.size() + neuralDynamicsTests_.size() + 
                            spikePatternTests_.size() + networkDynamicsTests_.size() + 
                            crossRegionTests_.size()) << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    
    generateBrainValidationReport();
}

void BrainValidationTestSuite::runSingleBrainValidationTest(const std::string& testName) {
    std::cout << "\nRunning single brain validation test: " << testName << std::endl;
    
    // Find the test and execute it
    // For simplicity, we'll just run all brain validation tests
    runBrainValidationTests();
}

void BrainValidationTestSuite::captureBrainState(const BrainStateSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    brainStateHistory_.push_back(snapshot);
    
    // Keep only the most recent 100 states
    if (brainStateHistory_.size() > 100) {
        brainStateHistory_.erase(brainStateHistory_.begin());
    }
    
    std::cout << "Captured brain state with " << snapshot.neuronFiringRates.size() 
              << " neurons, " << snapshot.regionNeuronIds.size() << " regions" << std::endl;
}

void BrainValidationTestSuite::compareBrainStates(const BrainStateSnapshot& state1,
                                                 const BrainStateSnapshot& state2,
                                                 double tolerance) const {
    std::cout << "Comparing brain states with tolerance: " << tolerance << std::endl;
    
    double maxDifference = calculateStateDifference(state1, state2);
    
    if (maxDifference <= tolerance) {
        std::cout << "  States are within tolerance (difference: " << maxDifference << ")" << std::endl;
    } else {
        std::cout << "  States differ beyond tolerance (difference: " << maxDifference << ")" << std::endl;
    }
}

bool BrainValidationTestSuite::isBrainStateValid(const BrainStateSnapshot& snapshot) const {
    // Check if the brain state meets validation criteria
    bool isValid = true;
    
    // Check for reasonable firing rates
    for (const auto& pair : snapshot.neuronFiringRates) {
        if (pair.second < 0.0 || pair.second > 100.0) { // 0-100 Hz range
            std::cout << "  Invalid firing rate: " << pair.second << " Hz" << std::endl;
            isValid = false;
        }
    }
    
    // Check for reasonable energy consumption
    if (snapshot.totalEnergyConsumption < 0.0 || snapshot.totalEnergyConsumption > 1000.0) {
        std::cout << "  Invalid energy consumption: " << snapshot.totalEnergyConsumption << std::endl;
        isValid = false;
    }
    
    // Check for valid region sizes
    for (const auto& pair : snapshot.regionNeuronCounts) {
        if (pair.second <= 0 || pair.second > 10000) {
            std::cout << "  Invalid region neuron count: " << pair.second << std::endl;
            isValid = false;
        }
    }
    
    if (isValid) {
        std::cout << "  Brain state validation passed" << std::endl;
    } else {
        std::cout << "  Brain state validation failed" << std::endl;
    }
    
    return isValid;
}

NeuralDynamicsMeasurement BrainValidationTestSuite::calculateNeuralDynamics(const std::vector<double>& spikeTimes) const {
    NeuralDynamicsMeasurement dynamics;
    
    if (spikeTimes.empty()) {
        return dynamics;
    }
    
    // Calculate basic statistics
    double minTime = spikeTimes.front();
    double maxTime = spikeTimes.back();
    double duration = maxTime - minTime;
    
    dynamics.firingRate = spikeTimes.size() / duration;
    dynamics.averageFiringRate = dynamics.firingRate;
    
    // Calculate synchrony index (simplified)
    double timeInterval = 10.0; // 10ms bins
    size_t numBins = static_cast<size_t>(duration / timeInterval);
    std::vector<size_t> binCounts(numBins, 0);
    
    for (double spikeTime : spikeTimes) {
        size_t binIndex = static_cast<size_t>(spikeTime / timeInterval);
        if (binIndex < numBins) {
            binCounts[binIndex]++;
        }
    }
    
    // Calculate synchrony index based on bin variance
    double meanBins = std::accumulate(binCounts.begin(), binCounts.end(), 0.0) / numBins;
    double variance = 0.0;
    for (size_t count : binCounts) {
        variance += (count - meanBins) * (count - meanBins);
    }
    variance /= numBins;
    
    double stdDev = std::sqrt(variance);
    dynamics.synchronyIndex = (meanBins > 0) ? (stdDev / meanBins) : 0.0;
    
    dynamics.timestamp = std::chrono::high_resolution_clock::now();
    
    return dynamics;
}

double BrainValidationTestSuite::calculateSynchronyIndex(const std::vector<double>& spikeTimes) const {
    if (spikeTimes.size() < 2) {
        return 0.0;
    }
    
    double timeInterval = 10.0; // 10ms bins
    double duration = spikeTimes.back() - spikeTimes.front();
    size_t numBins = static_cast<size_t>(duration / timeInterval);
    
    std::vector<size_t> binCounts(numBins, 0);
    
    for (double spikeTime : spikeTimes) {
        size_t binIndex = static_cast<size_t>(spikeTime / timeInterval);
        if (binIndex < numBins) {
            binCounts[binIndex]++;
        }
    }
    
    // Calculate synchrony index based on coefficient of variation
    double meanBins = std::accumulate(binCounts.begin(), binCounts.end(), 0.0) / numBins;
    double variance = 0.0;
    for (size_t count : binCounts) {
        variance += (count - meanBins) * (count - meanBins);
    }
    variance /= numBins;
    
    double stdDev = std::sqrt(variance);
    double cv = (meanBins > 0) ? (stdDev / meanBins) : 0.0;
    
    return cv;
}

double BrainValidationTestSuite::calculateFiringRate(const std::vector<double>& spikeTimes) const {
    if (spikeTimes.empty()) {
        return 0.0;
    }
    
    double duration = spikeTimes.back() - spikeTimes.front();
    if (duration <= 0.0) {
        return spikeTimes.size();
    }
    
    return spikeTimes.size() / duration;
}

void BrainValidationTestSuite::generateBrainValidationReport() const {
    std::cout << "\n=== Brain Validation Report ===" << std::endl;
    
    std::cout << "Session duration: " 
              << std::chrono::duration_cast<std::chrono::seconds>(
                  std::chrono::high_resolution_clock::now() - sessionStartTime_).count() 
              << " seconds" << std::endl;
    
    std::cout << "Brain states captured: " << brainStateHistory_.size() << std::endl;
    std::cout << "Neural dynamics measurements: " << neuralDynamicsHistory_.size() << std::endl;
    
    // Calculate average neural dynamics
    if (!neuralDynamicsHistory_.empty()) {
        double totalFiringRate = 0.0;
        for (const auto& dynamics : neuralDynamicsHistory_) {
            totalFiringRate += dynamics.firingRate;
        }
        double avgFiringRate = totalFiringRate / neuralDynamicsHistory_.size();
        
        std::cout << "Average firing rate: " << avgFiringRate << " Hz" << std::endl;
        std::cout << "Average synchrony index: " << 
                  (std::accumulate(neuralDynamicsHistory_.begin(), neuralDynamicsHistory_.end(), 0.0,
                                 [](double sum, const NeuralDynamicsMeasurement& d) { return sum + d.synchronyIndex; }) / 
                   neuralDynamicsHistory_.size()) << std::endl;
    }
    
    // Generate summary
    std::cout << "\nBrain validation test suite completed" << std::endl;
}

void BrainValidationTestSuite::exportBrainValidationData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing brain validation data: " << filename << std::endl;
        return;
    }
    
    file << "Brain Validation Data" << std::endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    file << "Session duration: " 
         << std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::high_resolution_clock::now() - sessionStartTime_).count() 
         << " seconds" << std::endl;
    
    file << "\nBrain States:" << std::endl;
    for (size_t i = 0; i < brainStateHistory_.size(); ++i) {
        file << "State " << i << ": " << brainStateHistory_[i].neuronFiringRates.size() 
             << " neurons, " << brainStateHistory_[i].regionNeuronIds.size() << " regions" << std::endl;
    }
    
    file << "\nNeural Dynamics Measurements:" << std::endl;
    for (size_t i = 0; i < neuralDynamicsHistory_.size(); ++i) {
        file << "Dynamics " << i << ": " << neuralDynamicsHistory_[i].firingRate 
             << " Hz, synchrony: " << neuralDynamicsHistory_[i].synchronyIndex << std::endl;
    }
    
    file.close();
    std::cout << "Brain validation data exported to: " << filename << std::endl;
}

bool BrainValidationTestSuite::validateBiologicalFidelity(const BrainStateSnapshot& snapshot) const {
    std::cout << "Validating biological fidelity..." << std::endl;
    
    bool isFidelitous = true;
    
    // Check for reasonable firing rates (0-100 Hz for biological neurons)
    for (const auto& pair : snapshot.neuronFiringRates) {
        if (pair.second < 0.0 || pair.second > 100.0) {
            std::cout << "  Invalid firing rate: " << pair.second << " Hz" << std::endl;
            isFidelitous = false;
        }
    }
    
    // Check for reasonable energy consumption (10-1000 mW per neuron)
    double avgEnergyPerNeuron = snapshot.totalEnergyConsumption / 
                               std::max(1.0, static_cast<double>(snapshot.neuronFiringRates.size()));
    
    if (avgEnergyPerNeuron < 10.0 || avgEnergyPerNeuron > 1000.0) {
        std::cout << "  Invalid energy efficiency: " << avgEnergyPerNeuron << " mW/neuron" << std::endl;
        isFidelitous = false;
    }
    
    // Check for reasonable membrane potentials (-100 to 50 mV)
    for (const auto& pair : snapshot.neuronMembranePotentials) {
        for (double potential : pair.second) {
            if (potential < -100.0 || potential > 50.0) {
                std::cout << "  Invalid membrane potential: " << potential << " mV" << std::endl;
                isFidelitous = false;
            }
        }
    }
    
    if (isFidelitous) {
        std::cout << "  Biological fidelity validation passed" << std::endl;
    } else {
        std::cout << "  Biological fidelity validation failed" << std::endl;
    }
    
    return isFidelitous;
}

bool BrainValidationTestSuite::validateNeuralDynamicsFidelity(const NeuralDynamicsMeasurement& dynamics) const {
    std::cout << "Validating neural dynamics fidelity..." << std::endl;
    
    bool isFidelitous = true;
    
    // Check for reasonable firing rates
    if (dynamics.firingRate < 0.0 || dynamics.firingRate > 200.0) {
        std::cout << "  Invalid firing rate: " << dynamics.firingRate << " Hz" << std::endl;
        isFidelitous = false;
    }
    
    // Check for reasonable synchrony index (0-1)
    if (dynamics.synchronyIndex < 0.0 || dynamics.synchronyIndex > 1.0) {
        std::cout << "  Invalid synchrony index: " << dynamics.synchronyIndex << std::endl;
        isFidelitous = false;
    }
    
    // Check for reasonable coefficient of variation
    if (dynamics.coefficientOfVariation < 0.0 || dynamics.coefficientOfVariation > 1.0) {
        std::cout << "  Invalid coefficient of variation: " << dynamics.coefficientOfVariation << std::endl;
        isFidelitous = false;
    }
    
    if (isFidelitous) {
        std::cout << "  Neural dynamics fidelity validation passed" << std::endl;
    } else {
        std::cout << "  Neural dynamics fidelity validation failed" << std::endl;
    }
    
    return isFidelitous;
}

bool BrainValidationTestSuite::validateSpikeTimingPrecision(const std::vector<double>& spikeTimes,
                                                          double expectedPrecision) const {
    std::cout << "Validating spike timing precision..." << std::endl;
    
    if (spikeTimes.size() < 2) {
        std::cout << "  Warning: Not enough spikes for timing precision validation" << std::endl;
        return true;
    }
    
    // Calculate spike intervals
    std::vector<double> intervals;
    for (size_t i = 1; i < spikeTimes.size(); ++i) {
        intervals.push_back(spikeTimes[i] - spikeTimes[i-1]);
    }
    
    // Calculate standard deviation of intervals
    double meanInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
    double variance = 0.0;
    for (double interval : intervals) {
        variance += (interval - meanInterval) * (interval - meanInterval);
    }
    variance /= intervals.size();
    double stdDev = std::sqrt(variance);
    
    // Calculate coefficient of variation
    double cv = (meanInterval > 0) ? (stdDev / meanInterval) : 0.0;
    
    std::cout << "  Mean interval: " << meanInterval << "ms" << std::endl;
    std::cout << "  Coefficient of variation: " << cv << std::endl;
    std::cout << "  Expected precision: " << expectedPrecision << std::endl;
    
    bool isPrecise = cv <= expectedPrecision;
    
    if (isPrecise) {
        std::cout << "  Spike timing precision validation passed" << std::endl;
    } else {
        std::cout << "  Spike timing precision validation failed" << std::endl;
    }
    
    return isPrecise;
}

bool BrainValidationTestSuite::validateEnergyEfficiency(const BrainStateSnapshot& snapshot,
                                                       double targetEnergyEfficiency) const {
    std::cout << "Validating energy efficiency..." << std::endl;
    
    if (snapshot.neuronFiringRates.empty()) {
        return true;
    }
    
    // Calculate energy efficiency (spikes per unit energy)
    double totalSpikes = 0.0;
    for (const auto& pair : snapshot.neuronFiringRates) {
        totalSpikes += pair.second;
    }
    
    double energyEfficiency = totalSpikes / snapshot.totalEnergyConsumption;
    
    std::cout << "  Energy efficiency: " << energyEfficiency << " spikes/mW" << std::endl;
    std::cout << "  Target efficiency: " << targetEnergyEfficiency << " spikes/mW" << std::endl;
    
    bool isEfficient = energyEfficiency >= targetEnergyEfficiency;
    
    if (isEfficient) {
        std::cout << "  Energy efficiency validation passed" << std::endl;
    } else {
        std::cout << "  Energy efficiency validation failed" << std::endl;
    }
    
    return isEfficient;
}

bool BrainValidationTestSuite::executeBrainValidationTest(const std::string& testName,
                                                         std::function<bool(const BrainStateSnapshot&)> testFunction,
                                                         const BrainStateSnapshot& snapshot) {
    bool testSuccess = testFunction(snapshot);
    testResults_[testName] = testSuccess;
    
    if (testSuccess) {
        testLogs_[testName] += " (PASSED)";
    } else {
        testLogs_[testName] += " (FAILED)";
    }
    
    return testSuccess;
}

bool BrainValidationTestSuite::executeNeuralDynamicsTest(const std::string& testName,
                                                         std::function<bool(const NeuralDynamicsMeasurement&)> testFunction,
                                                         const NeuralDynamicsMeasurement& dynamics) {
    bool testSuccess = testFunction(dynamics);
    testResults_[testName] = testSuccess;
    
    // Record dynamics measurement
    neuralDynamicsHistory_.push_back(dynamics);
    
    if (testSuccess) {
        testLogs_[testName] += " (PASSED)";
    } else {
        testLogs_[testName] += " (FAILED)";
    }
    
    return testSuccess;
}

void BrainValidationTestSuite::updateBrainStateHistory(const BrainStateSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(testsMutex_);
    brainStateHistory_.push_back(snapshot);
    
    if (brainStateHistory_.size() > 100) {
        brainStateHistory_.erase(brainStateHistory_.begin());
    }
}

BrainStateSnapshot BrainValidationTestSuite::averageBrainStates(const std::vector<BrainStateSnapshot>& states) const {
    BrainStateSnapshot avgState;
    
    if (states.empty()) {
        return avgState;
    }
    
    // Average neuron firing rates
    std::unordered_map<size_t, double> sumFiringRates;
    for (const auto& snapshot : states) {
        for (const auto& pair : snapshot.neuronFiringRates) {
            sumFiringRates[pair.first] += pair.second;
        }
    }
    
    for (const auto& pair : sumFiringRates) {
        avgState.neuronFiringRates[pair.first] = pair.second / states.size();
    }
    
    // Average region neuron counts
    std::unordered_map<size_t, size_t> sumRegionCounts;
    for (const auto& snapshot : states) {
        for (const auto& pair : snapshot.regionNeuronCounts) {
            sumRegionCounts[pair.first] += pair.second;
        }
    }
    
    for (const auto& pair : sumRegionCounts) {
        avgState.regionNeuronCounts[pair.first] = pair.second / states.size();
    }
    
    // Average membrane potentials
    for (const auto& snapshot : states) {
        for (const auto& pair : snapshot.neuronMembranePotentials) {
            size_t neuronId = pair.first;
            if (avgState.neuronMembranePotentials.find(neuronId) == avgState.neuronMembranePotentials.end()) {
                avgState.neuronMembranePotentials[neuronId] = std::vector<double>();
            }
            avgState.neuronMembranePotentials[neuronId].insert(avgState.neuronMembranePotentials[neuronId].end(),
                                                             pair.second.begin(), pair.second.end());
        }
    }
    
    for (auto& pair : avgState.neuronMembranePotentials) {
        double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
        if (!pair.second.empty()) {
            avgState.neuronMembranePotentials[pair.first] = sum / pair.second.size();
        }
    }
    
    return avgState;
}

double BrainValidationTestSuite::calculateStateDifference(const BrainStateSnapshot& state1,
                                                         const BrainStateSnapshot& state2) const {
    double maxDifference = 0.0;
    
    // Compare neuron firing rates
    for (const auto& pair : state1.neuronFiringRates) {
        auto it = state2.neuronFiringRates.find(pair.first);
        if (it != state2.neuronFiringRates.end()) {
            double difference = std::abs(pair.second - it->second);
            maxDifference = std::max(maxDifference, difference);
        }
    }
    
    return maxDifference;
}

void BrainValidationTestSuite::writeBrainTestResult(std::ofstream& file,
                                                    const std::string& testName,
                                                    bool isPassed,
                                                    const std::string& details) const {
    file << "Test: " << testName << std::endl;
    file << "Status: " << (isPassed ? "PASSED" : "FAILED") << std::endl;
    file << "Details: " << details << std::endl;
    file << std::endl;
}

} // namespace testing
} // namespace nlm