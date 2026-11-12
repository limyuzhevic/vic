// Brain step processing - extracted from monolithic Brain.cpp
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    NLM_LOG_DEBUG("Brain step " + std::to_string(currentStep) + " at time " + std::to_string(currentTime) + "s");
    
    // Reset step statistics
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->checkThreshold()) {
                    neuron->recordSpike(currentTime);
                    pImpl->totalSpikesThisStep++;
                    pImpl->totalSpikesTotal++;
                    
                    // Notify spike handlers
                    pImpl->spikeSystem->deliverSpike(neuron->getId(), currentTime);
                    
                    // For now, all spikes go to spike system
                    // TODO: Route spikes to specific handlers based on neuron type
                }
            }
        }
    }
    
    // ========== STEP 4: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f, 
                               pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f, 
                               pImpl->timestep);
    }
    
    // Update acetylcholine (attention and memory)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on attention and memory consolidation
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        if (acetylcholineLevel > 0.1f) {
            // Enhance attention gain
            if (pImpl->attention) {
                pImpl->attention->setInhibitionStrength(
                    std::max(0.1f, 0.5f - acetylcholineLevel * 0.3f));
                pImpl->attention->setExcitationStrength(
                    std::min(1.0f, 1.0f + acetylcholineLevel * 1.5f));
            }
            
            // Promote memory consolidation
            if (pImpl->workingMemory && acetylcholineLevel > 0.3f) {
                pImpl->workingMemory->setDecayRate(
                    std::max(0.01f, 0.1f - acetylcholineLevel * 0.05f));
            }
            
            if (pImpl->episodicMemory && acetylcholineLevel > 0.2f) {
                float consolidationBoost = acetylcholineLevel * 0.5f;
                pImpl->episodicMemory->consolidate(1.0f - consolidationBoost);
            }
        }
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 5: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    Timestamp preTime = preSpikes.back();
                    Timestamp postTime = postSpikes.back();
                    
                    if (postTime > preTime) {
                        // LTP: post fires after pre
                        float delta = pImpl->stdp->computeLTP(postTime - preTime) * plasticityMod;
                        syn->updateWeight(delta);
                    } else {
                        // LTD: pre fires after post
                        float delta = pImpl->stdp->computeLTD(preTime - postTime) * plasticityMod;
                        syn->updateWeight(delta);
                    }
                }
            }
            
            // Apply Hebbian plasticity
            if (syn->getPlasticityFlags().hebbian) {
                bool preActive = pImpl->spikeSystem->isNeuronActive(syn->getSourceNeuron(), currentTime);
                bool postActive = pImpl->spikeSystem->isNeuronActive(syn->getDestinationNeuron(), currentTime);
                
                if (preActive && postActive) {
                    float delta = pImpl->hebbian->computeDelta(pImpl->timestep);
                    syn->updateWeight(delta);
                }
            }
        }
    }
    
    // ========== STEP 6: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 7: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with current sensory observations
        auto* predSys = pImpl->predictionSystem.get();
        
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<float> currentPattern = pImpl->workingMemory->retrieve();
            if (!currentPattern.empty()) {
                predSys->updatePrediction(currentPattern, currentTime);
            }
        }
        
        float predictionError = predSys->computePredictionError(currentTime);
        
        if (pImpl->predictionError) {
            pImpl->predictionError->setLevel(predictionError);
        }
        
        if (pImpl->attention && predictionError > 0.1f) {
            pImpl->attention->applyBottomUpSalience(
                INVALID_NEURON_ID, predictionError * 2.0f);
        }
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        
        // Apply attention to episodic memory for relevant episodes
        if (pImpl->episodicMemory) {
            auto relevantEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            if (!relevantEpisodes.empty()) {
                std::vector<NeuronId> episodeNeurons;
                for (const auto& episode : relevantEpisodes) {
                    episodeNeurons.insert(episodeNeurons.end(), 
                                        episode.activeNeurons.begin(), 
                                        episode.activeNeurons.end());
                }
                if (!episodeNeurons.empty()) {
                    pImpl->attention->processCompetition(episodeNeurons);
                }
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            std::vector<float> activations = pImpl->workingMemory->retrieve();
            
            if (!memoryNeurons.empty() && !activations.empty()) {
                Pattern pattern;
                for (size_t i = 0; i < std::min(memoryNeurons.size(), activations.size()); ++i) {
                    pattern.neurons.push_back(memoryNeurons[i]);
                    pattern.activations.push_back(activations[i]);
                }
                
                pImpl->conceptFormation->processPattern(pattern, currentTime);
                
                std::vector<NeuronId> conceptNeurons = pImpl->conceptFormation->getConceptNeurons();
                if (!conceptNeurons.empty()) {
                    pImpl->attention->processCompetition(conceptNeurons);
                }
            }
        }
        
        if (pImpl->episodicMemory && currentStep % 100 == 0) {
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
            for (const auto& episode : recentEpisodes) {
                Pattern episodePattern;
                for (size_t i = 0; i < std::min(episode.activeNeurons.size(), episode.neuronActivations.size()); ++i) {
                    episodePattern.neurons.push_back(episode.activeNeurons[i]);
                    episodePattern.activations.push_back(episode.neuronActivations[i]);
                }
                
                pImpl->conceptFormation->reinforceConcept(episodePattern, currentTime);
            }
        }
    }
    
    // ========== STEP 11: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 12: Apply development effects ==========
    if (currentStep % 1000 == 0) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial: plasticityMod = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: plasticityMod = 0.8f; break;
                case DevelopmentalStage::Maturation: plasticityMod = 0.5f; break;
                case DevelopmentalStage::Adult: plasticityMod = 0.2f; break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    // ========== STEP 13: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 14: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm