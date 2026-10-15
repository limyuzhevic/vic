#pragma once

#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Curiosity.hpp"
#include "PredictionError.hpp"
#include "Novelty.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference for system-wide access
    void initialize(class Brain* brain);
    
    // Update all neuromodulators and their cross-modulations
    void update(TimestepDuration dt);
    
    // Apply neuromodulatory effects to neural plasticity
    void applyPlasticityEffects(const class Brain& brain);
    
    // Get individual neuromodulators
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();
    Curiosity* getCuriosity();
    PredictionError* getPredictionError();
    Novelty* getNovelty();
    
    // Get combined neuromodulatory state
    float getCombinedNeuromodulatoryState() const;
    float getAttentionModulation() const;
    float getArousalModulation() const;
    float getRewardModulation() const;
    float getMemoryModulation() const;
    
    // Reset all neuromodulators to baseline
    void reset();
    
    // Configure cross-modulation parameters
    void setCrossModulationStrength(float dopamineACh, float dopamineNE, 
                                    float dopamine5HT, float AChNE, 
                                    float ACh5HT, float NE5HT);
    
    // Set neuromodulation response curves
    void setResponseCurves(float dopamineCurvature, float AChCurvature,
                           float NECurvature, float 5HTCurvature);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
