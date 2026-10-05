#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <algorithm>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    // Get developmental stage for rate modulation
    float plasticityMod = 1.0f;
    if (brain->getDevelopmentSystem()) {
        plasticityMod = brain->getDevelopmentSystem()->getPlasticityModifier();
    }
    
    // Calculate maturation progress based on time
    float maturationRate = 0.01f * plasticityMod;  // Slower during high plasticity periods
    pImpl->progress = std::min(1.0f, pImpl->progress + maturationRate);
    
    /*
     * Implement real maturation:
     * 
     * 1. Neural properties mature gradually over time
     * 2. Synaptic weights stabilize and refine
     * 3. Membrane properties optimize for energy efficiency
     * 4. Integration with neuromodulatory systems (dopamine, curiosity)
     * 5. Activity-dependent refinement of neural circuits
     */
    
    // Update mature properties based on progress
    float thresholdShift = pImpl->progress * 15.0f;  // Threshold increases by up to 15mV
    pImpl->matureThreshold = -55.0f + thresholdShift;
    
    float restingShift = pImpl->progress * 8.0f;  // Resting potential shifts by up to 8mV
    pImpl->matureRestingPotential = -70.0f + restingShift;
    
    float timeConstantShift = pImpl->progress * 30.0f;  // Time constant increases up to 50ms
    pImpl->matureTimeConstant = 20.0f + timeConstantShift;
    
    // Apply maturation effects to the brain's neurons
    if (brain->getDevelopmentSystem()) {
        DevelopmentalStage stage = brain->getDevelopmentSystem()->getStage();
        
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    
                    // Update neuron properties based on maturation progress
                    float maturityFactor = 1.0f - pImpl->progress;  // Less mature = more plasticity
                    
                    // Adjust neuron threshold based on maturation
                    float targetThreshold = pImpl->matureThreshold;
                    float currentThreshold = neuron->getThreshold();
                    
                    // Gradual approach to mature threshold
                    float thresholdDiff = targetThreshold - currentThreshold;
                    neuron->setThreshold(currentThreshold + thresholdDiff * 0.01f * maturityFactor);
                    
                    // Adjust resting potential
                    float targetResting = pImpl->matureRestingPotential;
                    float currentResting = neuron->getRestingPotential();
                    
                    float restingDiff = targetResting - currentResting;
                    neuron->setRestingPotential(currentResting + restingDiff * 0.01f * maturityFactor);
                    
                    // Adjust leak conductance (decreases with maturity for energy efficiency)
                    float targetLeak = 0.3f - pImpl->progress * 0.2f;  // From 0.3 to 0.1
                    float currentLeak = neuron->getLeakConductance();
                    
                    float leakDiff = targetLeak - currentLeak;
                    neuron->setLeakConductance(currentLeak + leakDiff * 0.01f * maturityFactor);
                }
            }
        }
        
        // Stage-specific maturation patterns
        switch (stage) {
            case DevelopmentalStage::Initial:
                // Initial burst of structural changes
                if (brain->getStructuralPlasticity()) {
                    brain->getStructuralPlasticity()->setSynaptogenesisRate(0.0002f);
                    brain->getStructuralPlasticity()->setPruningRate(0.000005f);
                }
                break;
                
            case DevelopmentalStage::CriticalPeriod:
                // Critical period: enhanced refinement
                if (brain->getStructuralPlasticity()) {
                    brain->getStructuralPlasticity()->setSynaptogenesisRate(0.00005f);
                    brain->getStructuralPlasticity()->setPruningRate(0.00002f);
                }
                
                // Enhanced neuromodulation during critical period
                if (brain->getDopamine()) {
                    float dopamineLevel = brain->getDopamine()->getLevel();
                    // High dopamine sensitivity for learning
                    brain->getDopamine()->setLevel(dopamineLevel * 1.2f);
                }
                // Curiosity doesn't have setLevel, so we'll just use the natural level
                // (Curiosity levels evolve naturally based on novelty and prediction error)
                break;
                
            case DevelopmentalStage::Maturation:
                // Maturation: stabilization and efficiency
                if (brain->getStructuralPlasticity()) {
                    brain->getStructuralPlasticity()->setSynaptogenesisRate(0.00001f);
                    brain->getStructuralPlasticity()->setPruningRate(0.00001f);
                }
                
                // Begin to stabilize neuromodulation
                if (brain->getDopamine()) {
                    float dopamineLevel = brain->getDopamine()->getLevel();
                    brain->getDopamine()->setLevel(dopamineLevel * 0.8f);
                }
                // Curiosity doesn't have setLevel, so we'll just use the natural level
                break;
                
            case DevelopmentalStage::Adult:
                // Adult: maintenance and pruning
                if (brain->getStructuralPlasticity()) {
                    brain->getStructuralPlasticity()->setSynaptogenesisRate(0.000005f);
                    brain->getStructuralPlasticity()->setPruningRate(0.00001f);
                }
                
                // Stable neuromodulation
                if (brain->getDopamine()) {
                    float dopamineLevel = brain->getDopamine()->getLevel();
                    brain->getDopamine()->setLevel(dopamineLevel * 0.5f);
                }
                // Curiosity doesn't have setLevel, so we'll just use the natural level
                break;
                
            case DevelopmentalStage::Aging:
                // Aging: reduced plasticity and efficiency
                if (brain->getStructuralPlasticity()) {
                    brain->getStructuralPlasticity()->setSynaptogenesisRate(0.000001f);
                    brain->getStructuralPlasticity()->setPruningRate(0.00002f);
                }
                
                // Reduced neuromodulation
                if (brain->getDopamine()) {
                    float dopamineLevel = brain->getDopamine()->getLevel();
                    brain->getDopamine()->setLevel(dopamineLevel * 0.3f);
                }
                // Curiosity doesn't have setLevel, so we'll just use the natural level
                break;
        }
        
        // Integration with procedural memory development
        if (stage == DevelopmentalStage::Maturation || stage == DevelopmentalStage::Adult) {
            if (brain->getProceduralMemory()) {
                // Procedural memory proficiency develops with maturation
                float proficiencyGain = 0.01f * plasticityMod;  // Small gains for stability
                
                // Increase proficiency for all learned skills
                for (auto& skill : brain->getProceduralMemory()->getAllSkills()) {
                    skill.proficiency = std::min(1.0f, skill.proficiency + proficiencyGain);
                    // Improve success rate based on proficiency
                    skill.successRate = skill.proficiency * (0.5f + skill.proficiency * 0.5f);
                    // Reduce execution time with proficiency
                    skill.averageExecutionTime = std::max(0.1f, skill.averageExecutionTime * 0.95f);
                }
                
                // Neural pattern refinement for procedural memories
                // Mature motor patterns become more stable and efficient
                if (stage == DevelopmentalStage::Adult) {
                    // Adult refinement - reduce noise in motor patterns
                    for (auto& skill : brain->getProceduralMemory()->getAllSkills()) {
                        if (skill.proficiency > 0.7f) {
                            // Sharpen neural patterns for highly proficient skills
                            // This would involve modifying the neuralPattern in the skill
                            // In a full implementation, this would strengthen specific neural pathways
                        }
                    }
                }
            }
        }
        
        // Refinement of STDP and Hebbian plasticity parameters
        if (brain->getSTDP()) {
            // STDP parameters become more stable with maturity
            float ltpWeight = brain->getSTDP()->getLTPWeight();
            float ltdWeight = brain->getSTDP()->getLTDWeight();
            
            if (stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging) {
                // Stabilize STDP weights
                ltpWeight = std::min(0.03f, ltpWeight * 0.95f);
                ltdWeight = std::min(0.04f, ltdWeight * 0.95f);
                brain->getSTDP()->configure(ltpWeight, ltdWeight, brain->getSTDP()->getTimeConstant());
            }
        }
        
        if (brain->getHebbian()) {
            // Hebbian learning rate decreases with maturity
            float hebbianRate = brain->getHebbian()->getLearningRate();
            
            if (stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging) {
                hebbianRate *= 0.9f;
                brain->getHebbian()->setLearningRate(hebbianRate);
            }
        }
        
        // ========== DEVELOPMENT EFFECTS ON CONCEPT FORMATION ==========
        
        if (brain->getConceptFormation()) {
            // Concept abstraction develops
            float abstractionLevel = 0.3f + static_cast<float>(stage) * 0.7f / static_cast<float>(DevelopmentalStage::Aging);  // From 0.3 to 1.0
            brain->getConceptFormation()->setAbstractionLevel(abstractionLevel);
            
            // Concept binding strength develops
            float bindingStrength = 0.5f + static_cast<float>(stage) * 0.5f / static_cast<float>(DevelopmentalStage::Aging);
            brain->getConceptFormation()->setBindingStrength(bindingStrength);
        }
    }
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm