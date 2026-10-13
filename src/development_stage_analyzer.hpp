// DevelopmentStageAnalyzer class header
// This file provides the header declaration for the DevelopmentStageAnalyzer class

#ifndef DEVELOPMENT_STAGE_ANALYZER_HPP
#define DEVELOPMENT_STAGE_ANALYZER_HPP

#include <string>
#include <vector>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Analyze development stages and progress
 */
class DevelopmentStageAnalyzer {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    DevelopmentStageAnalyzer(std::shared_ptr<Brain> brain);
    
    ~DevelopmentStageAnalyzer() = default;
    
    // Stage analysis
    std::string getCurrentStage() const;
    double getStageProgress() const;
    double getDevelopmentalAge() const;
    FloatArray getMaturationIndicators() const;
    
    // Critical period analysis
    bool isInCriticalPeriod() const;
    double getCriticalPeriodProgress() const;
    FloatArray getPlasticityModifiers() const;
    
    // Growth analysis
    FloatArray getBrainGrowthMetrics() const;
    FloatArray getSynaptogenesisRate() const;
    FloatArray getPruningEfficiency() const;
    
    // Export
    void exportDevelopmentProfile(const std::string& filepath) const;
    std::string getDevelopmentSummary() const;
};

#endif // DEVELOPMENT_STAGE_ANALYZER_HPP
