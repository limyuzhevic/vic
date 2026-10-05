#pragma once

#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;
class BrainManager {
public:
    struct PerformanceStats {
        double totalTime;
        size_t totalSpikes;
        size_t activeNeurons;
        float excitationInhibitionRatio;
        std::vector<float> weightChanges;
    };
    
    BrainManager(std::shared_ptr<Config> config) : config_(config) {}
    bool initializeBrain();
    void resetBrain();
    
    // Test modes
    PerformanceStats runConnectivityTest();
    PerformanceStats runPlasticityExperiment();
    PerformanceStats runStdpVerification();
    PerformanceStats runCustomExperiment(const std::string& experimentName);
    
    // Statistics
    void logPerformanceStats(const PerformanceStats& stats, const std::string& testName);
    void saveResults(const PerformanceStats& stats, const std::string& testName, 
                     const std::string& outputDir);
    
    // Advanced features
    void enableAdvancedLearning(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableNeuromodulation(bool enable);
    
    std::shared_ptr<Brain> getBrain() { return brain_; }
    const PerformanceStats& getLastStats() const { return lastStats_; }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    PerformanceStats lastStats_;
    bool advancedFeaturesEnabled_ = false;
};

} // namespace nlm