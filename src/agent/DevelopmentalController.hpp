#pragma once

#include <cmath>

namespace nlm {

enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult
};

class DevelopmentalController {
public:
    DevelopmentalController();
    ~DevelopmentalController();
    
    // Update development system
    void updateDevelopment(double timestep, double& developmentalAge,
                          float& plasticityModifier,
                          bool developmentEnabled = true);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage(double developmentalAge);
    
    // Get developmental stage string representation
    static const char* developmentalStageToString(DevelopmentalStage stage);
    
    // Check if development is active
    bool isDevelopmentActive(double developmentalAge) const;
    
    // Get plasticity modifier based on developmental stage
    static float getPlasticityForStage(DevelopmentalStage stage);
    
    // Reset development state
    void reset() {
        developmentalAge_ = 0.0;
        currentStage_ = DevelopmentalStage::Initial;
    }
    
    // Configuration
    void setDevelopmentEnabled(bool enabled) { developmentEnabled_ = enabled; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    void setStructuralPlasticityEnabled(bool enabled) { structuralPlasticityEnabled_ = enabled; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    // Developmental stage configuration
    struct DevelopmentalStageConfig {
        DevelopmentalStage stage;
        double ageThreshold; // Age in seconds when this stage begins
        float plasticityModifier; // Plasticity multiplier for this stage
    };
    
    static const DevelopmentalStageConfig* getStageConfig(DevelopmentalStage stage);

private:
    double developmentalAge_;
    DevelopmentalStage currentStage_;
    bool developmentEnabled_;
    bool structuralPlasticityEnabled_;
    
    static const DevelopmentalStageConfig stageConfigs_[];
};

} // namespace nlm