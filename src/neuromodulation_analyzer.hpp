// NeuromodulationAnalyzer class header
// This file provides the header declaration for the NeuromodulationAnalyzer class

#ifndef NEUROMODULATION_ANALYZER_HPP
#define NEUROMODULATION_ANALYZER_HPP

#include <string>
#include <vector>
#include <map>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Analyze neuromodulation patterns
 */
class NeuromodulationAnalyzer {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    NeuromodulationAnalyzer(std::shared_ptr<Brain> brain);
    
    ~NeuromodulationAnalyzer() = default;
    
    // Neuromodulator analysis
    FloatArray getDopamineActivity() const;
    FloatArray getCuriosityActivity() const;
    FloatArray getNoveltyActivity() const;
    FloatArray getPredictionErrorActivity() const;
    FloatArray getRewardActivity() const;
    
    // Pattern analysis
    std::map<std::string, FloatArray> getModulationPatterns() const;
    FloatArray computeModulationCorrelation() const;
    FloatArray computeModulationTiming() const;
    
    // Functional analysis
    double getMotivationalDrive() const;
    double getExplorationExploitationBalance() const;
    double getLearningRateEstimate() const;
    
    // Export
    void exportModulationProfile(const std::string& filepath) const;
    std::string getModulationSummary() const;
};

#endif // NEUROMODULATION_ANALYZER_HPP
