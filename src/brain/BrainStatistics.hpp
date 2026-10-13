#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * BrainStatistics - Comprehensive statistics collection and reporting
 * Extracted from Brain.cpp to improve separation of concerns
 */
class BrainStatistics {
public:
    BrainStatistics() {
        reset();
    }
    
    ~BrainStatistics() = default;
    
    // Disable copying, enable moving
    BrainStatistics(const BrainStatistics&) = delete;
    BrainStatistics& operator=(const BrainStatistics&) = delete;
    BrainStatistics(BrainStatistics&&) noexcept;
    BrainStatistics& operator=(BrainStatistics&&) noexcept;
    
    // Statistics collection
    void recordStep(SimulationStep step, Timestamp time, size_t spikes, float firingRate);
    void recordMemoryEvent(const std::string& event, double value);
    void recordNeuromodulation(const std::string& type, float level);
    void recordPlasticityEvent(const std::string& type, float change);
    void recordCognitionEvent(const std::string& type, float value);
    void recordDevelopmentEvent(const std::string& type, float value);
    
    // Reporting
    void generateReport() const;
    void generatePerformanceReport() const;
    
    // Statistics access
    size_t getTotalSteps() const { return totalSteps_; }
    size_t getTotalSpikes() const { return totalSpikes_; }
    float getAverageFiringRate() const { return averageFiringRate_; }
    float getAverageMemoryUsage() const { return averageMemoryUsage_; }
    float getAverageNeuromodulationLevel() const { return averageNeuromodulationLevel_; }
    float getAveragePlasticityChange() const { return averagePlasticityChange_; }
    float getAverageConceptActivation() const { return averageConceptActivation_; }
    float getDevelopmentalAge() const { return developmentalAge_; }
    
    // State management
    void reset() {
        totalSteps_ = 0;
        totalSpikes_ = 0;
        averageFiringRate_ = 0.0f;
        averageMemoryUsage_ = 0.0f;
        averageNeuromodulationLevel_ = 0.0f;
        averagePlasticityChange_ = 0.0f;
        averageConceptActivation_ = 0.0f;
        developmentalAge_ = 0.0f;
        
        stepHistory_.clear();
        memoryHistory_.clear();
        neuromodulationHistory_.clear();
        plasticityHistory_.clear();
        cognitionHistory_.clear();
        developmentHistory_.clear();
    }
    
    // Get historical data
    const std::vector<std::pair<SimulationStep, size_t>>& getSpikeHistory() const { return spikeHistory_; }
    const std::vector<std::pair<std::string, double>>& getMemoryHistory() const { return memoryHistory_; }
    const std::vector<std::pair<std::string, float>>& getNeuromodulationHistory() const { return neuromodulationHistory_; }
    
private:
    // Basic statistics
    size_t totalSteps_ = 0;
    size_t totalSpikes_ = 0;
    float averageFiringRate_ = 0.0f;
    float averageMemoryUsage_ = 0.0f;
    float averageNeuromodulationLevel_ = 0.0f;
    float averagePlasticityChange_ = 0.0f;
    float averageConceptActivation_ = 0.0f;
    float developmentalAge_ = 0.0f;
    
    // Historical data
    std::vector<std::pair<SimulationStep, size_t>> spikeHistory_;
    std::vector<std::pair<SimulationStep, float>> firingRateHistory_;
    std::vector<std::pair<std::string, double>> memoryHistory_;
    std::vector<std::pair<std::string, float>> neuromodulationHistory_;
    std::vector<std::pair<std::string, float>> plasticityHistory_;
    std::vector<std::pair<std::string, float>> cognitionHistory_;
    std::vector<std::pair<std::string, float>> developmentHistory_;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime_;
};

} // namespace nlm