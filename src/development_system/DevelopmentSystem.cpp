#include "DevelopmentSystem.h"
#include "core/Logger/Logger.hpp"
#include "plasticity/StructuralPlasticity.hpp"

namespace nlm {

class DevelopmentSystemImpl : public DevelopmentSystem {
private:
    DevelopmentalStage currentStage = DevelopmentalStage::Initial;
    float developmentalAge = 0.0f;
    float timeInCurrentStage = 0.0f;
    
    // Development timing parameters
    float stageDurations[5] = {100.0f, 200.0f, 300.0f, 400.0f, 500.0f};  // Simplified stage durations
    
    // Developmental state
    float plasticityRate = 1.0f;
    float maturationLevel = 0.0f;
    
public:
    DevelopmentSystemImpl() {
        NLM_LOG_INFO("DevelopmentSystem initialized");
    }
    
    void initialize(Brain* brain) override {
        currentStage = DevelopmentalStage::Initial;
        developmentalAge = 0.0f;
        timeInCurrentStage = 0.0f;
        plasticityRate = 1.0f;
        maturationLevel = 0.0f;
        
        NLM_LOG_INFO("Development system initialized in Initial stage");
    }
    
    void update(Brain* brain, TimestepDuration timestep) override {
        developmentalAge += timestep;
        timeInCurrentStage += timestep;
        
        // Update developmental stage based on age
        updateDevelopmentalStage(timestep);
        
        // Update plasticity rate based on development
        plasticityRate = calculatePlasticityRate();
        
        // Apply developmental effects
        if (brain && brain->getStructuralPlasticity()) {
            applyDevelopmentalPlasticity(brain->getStructuralPlasticity());
        }
        
        NLM_LOG_DEBUG("Development update: Age=" + std::to_string(developmentalAge) + 
                     " Stage=" + std::to_string(static_cast<int>(currentStage)) +
                     " PlasticityRate=" + std::to_string(plasticityRate));
    }
    
    DevelopmentalStage getCurrentStage() const override {
        return currentStage;
    }
    
    void setCurrentStage(DevelopmentalStage stage) override {
        currentStage = stage;
        timeInCurrentStage = 0.0f;
        
        float stageTime = stageDurations[static_cast<int>(stage)];
        NLM_LOG_INFO("Developmental stage changed to " + std::to_string(static_cast<int>(stage)) +
                    " (duration: " + std::to_string(stageTime) + "s)");
    }
    
    void applyDevelopmentalPlasticity(StructuralPlasticity* structuralPlasticity) override {
        if (structuralPlasticity) {
            float originalSynaptogenesis = 0.0001f;
            float originalPruning = 0.00001f;
            
            float plasticityMod = 1.0f;
            
            switch (currentStage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    maturationLevel = 0.0f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    maturationLevel = 0.25f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    maturationLevel = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    maturationLevel = 0.75f;
                    break;
                case DevelopmentalStage::Aging:
                    plasticityMod = 0.1f;  // Reduced plasticity
                    maturationLevel = 1.0f;
                    break;
            }
            
            structuralPlasticity->setSynaptogenesisRate(originalSynaptogenesis * plasticityMod);
            structuralPlasticity->setPruningRate(originalPruning * (2.0f - plasticityMod));
            
            NLM_LOG_DEBUG("Development plasticity applied: synaptogenesis=" +
                         std::to_string(originalSynaptogenesis * plasticityMod) +
                         " pruning=" + std::to_string(originalPruning * (2.0f - plasticityMod)));
        }
    }
    
    void updateDevelopmentalStage(float time) override {
        float stageTime = stageDurations[static_cast<int>(currentStage)];
        
        if (timeInCurrentStage >= stageTime) {
            // Transition to next stage
            int nextStage = static_cast<int>(currentStage) + 1;
            if (nextStage < static_cast<int>(DevelopmentalStage::Aging) + 1) {
                setCurrentStage(static_cast<DevelopmentalStage>(nextStage));
            } else {
                NLM_LOG_INFO("Development complete - reached final stage");
            }
        }
    }
    
    bool isDevelopmentComplete() const override {
        return currentStage == DevelopmentalStage::Aging;
    }
    
    float getDevelopmentalAge() const override {
        return developmentalAge;
    }
    
    float getPlasticityRate() const override {
        return plasticityRate;
    }
    
private:
    float calculatePlasticityRate() {
        float rate = 1.0f;
        
        switch (currentStage) {
            case DevelopmentalStage::Initial:
                rate = 1.0f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                rate = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                rate = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                rate = 0.2f;
                break;
            case DevelopmentalStage::Aging:
                rate = 0.1f;
                break;
        }
        
        // Apply maturation effects
        rate *= (1.0f - (maturationLevel * 0.5f));
        
        return rate;
    }
};

} // namespace nlm