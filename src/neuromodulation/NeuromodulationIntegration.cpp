// Neuromodulation Integration

#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Noradrenaline.hpp"
#include "Serotonin.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void NeuromodulationIntegration::applyNeuromodulation(Brain* brain) {
    if (!brain) return;
    
    // Get neuromodulator levels
    float dopamineLevel = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
    float acetylcholineLevel = brain->getAcetylcholine() ? brain->getAcetylcholine()->getLevel() : 0.0f;
    float noradrenalineLevel = brain->getNoradrenaline() ? brain->getNoradrenaline()->getLevel() : 0.0f;
    float serotoninLevel = brain->getSerotonin() ? brain->getSerotonin()->getLevel() : 0.0f;
    
    // Get memory systems
    NeuralWorkingMemory* workingMemory = brain->getWorkingMemory();
    NeuralEpisodicMemory* episodicMemory = brain->getEpisodicMemory();
    NeuralAssociativeMemory* associativeMemory = brain->getAssociativeMemory();
    
    // Get attention system
    AttentionalSelection* attention = brain->getAttention();
    
    // Step 1: Neuromodulation effects on memory systems
    
    // Working memory: modulated by acetylcholine and dopamine
    if (workingMemory) {
        float workingMemoryMod = (acetylcholineLevel * 0.4f + dopamineLevel * 0.3f);
        workingMemory->setDecayRate(workingMemoryMod * 0.01f + 0.001f);
    }
    
    // Episodic memory: modulated by dopamine and norepinephrine
    if (episodicMemory) {
        float episodicMod = (dopamineLevel * 0.5f + noradrenalineLevel * 0.3f);
        // Enhance memory consolidation based on neuromodulatory state
        if (dopamineLevel > 0.3f) {
            // Strong dopamine enhances consolidation
            // This would be implemented by enhancing replay or synaptic strengthening
        }
    }
    
    // Associative memory: modulated by all neuromodulators
    if (associativeMemory) {
        float associativeMod = (dopamineLevel * 0.3f + acetylcholineLevel * 0.3f + 
                              noradrenalineLevel * 0.2f + serotoninLevel * 0.2f);
        // Association strength modulated by combined neuromodulatory state
    }
    
    // Step 2: Neuromodulation effects on attention system
    
    if (attention && workingMemory && !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        
        // Acetylcholine enhances attentional focus
        float achAttentionGain = acetylcholineLevel * 0.8f + 0.2f;
        attention->setInhibitionStrength(0.5f - achAttentionGain * 0.2f);
        attention->setExcitationStrength(1.5f + achAttentionGain * 0.5f);
        
        // Noradrenaline enhances arousal and vigilance
        float neVigilance = noradrenalineLevel;
        attention->processCompetition(competitors, neVigilance);
        
        // Dopamine modulates action selection
        float daActionMod = dopamineLevel * 0.5f + 0.5f;
        attention->applyTopDownBias(competitors[0], daActionMod * 0.3f);
    }
    
    // Step 3: Neuromodulation effects on neural plasticity
    
    // Calculate global neuromodulation factors for plasticity
    float globalPlasticityFactor = 1.0f;
    
    // Dopamine strongly enhances plasticity for reward prediction
    if (dopamineLevel > 0.1f) {
        globalPlasticityFactor *= (1.0f + dopamineLevel * 0.8f);
    }
    
    // Acetylcholine enhances memory-related plasticity
    if (acetylcholineLevel > 0.1f) {
        globalPlasticityFactor *= (1.0f + acetylcholineLevel * 0.5f);
    }
    
    // Noradrenaline enhances arousal-dependent plasticity
    if (noradrenalineLevel > 0.1f) {
        globalPlasticityFactor *= (1.0f + noradrenalineLevel * 0.6f);
    }
    
    // Serotonin modulates plasticity for emotional learning
    if (serotoninLevel > 0.1f) {
        globalPlasticityFactor *= (1.0f + serotoninLevel * 0.3f);
    }
    
    // Step 4: Neuromodulation integration with curiosity and prediction error
    
    // Get curiosity and prediction error signals
    float curiosityLevel = brain->getCuriosity() ? brain->getCuriosity()->getLevel() : 0.0f;
    float predictionErrorLevel = brain->getPredictionErrorSignal() ? 
                                brain->getPredictionErrorSignal()->getMagnitude() : 0.0f;
    
    // Integrate with curiosity (exploration motivation)
    float explorationDrive = curiosityLevel * 0.7f + predictionErrorLevel * 0.3f;
    
    // Neuromodulation-based exploration modulation
    if (explorationDrive > 0.3f) {
        // High exploration states enhance novelty seeking
        if (brain->getNovelty()) {
            float enhancedNovelty = std::min(1.0f, explorationDrive * 1.5f);
            brain->getNovelty()->setLevel(enhancedNovelty);
        }
        
        // Prepare for action exploration
        if (dopamineLevel < 0.5f) {
            dopamine->signalRewardPredictionError(0.5f); // Positive prediction error encourages exploration
        }
    }
    
    // Step 5: Neuromodulation effects on action selection
    
    // Calculate action selection bias based on neuromodulatory state
    float actionSelectionBias = 0.0f;
    
    // Dopamine bias towards high-reward actions
    actionSelectionBias += dopamineLevel * 0.4f;
    
    // Serotonin bias towards cautious, well-considered actions
    actionSelectionBias -= serotoninLevel * 0.2f;
    
    // Noradrenaline bias towards rapid responses in high-arousal states
    if (noradrenalineLevel > 0.5f) {
        actionSelectionBias += 0.2f;
    }
    
    // Step 6: Neuromodulatory consolidation of memory traces
    
    // Get active neurons from working memory for consolidation
    if (workingMemory && episodicMemory) {
        std::vector<NeuronId> memoryNeurons = workingMemory->getMemoryNeurons();
        
        // Use neuromodulators to enhance memory trace strength
        if (dopamineLevel > 0.2f) {
            // Dopamine enhances memory trace strength
            workingMemory->strengthenMemory(dopamineLevel * 1.5f);
        }
        
        if (acetylcholineLevel > 0.2f) {
            // Acetylcholine enhances memory encoding
            for (NeuronId neuronId : memoryNeurons) {
                brain->injectCurrent(neuronId, acetylcholineLevel * 0.3f);
            }
        }
    }
    
    NLM_LOG_DEBUG("Neuromodulation integration completed: DA={}, ACh={}, NE={}, 5-HT={}",
                  dopamineLevel, acetylcholineLevel, noradrenalineLevel, serotoninLevel);
}

} // namespace nlm
