// brain/BrainStepProcessor.cpp - Brain step processing implementation
// Contains the main simulation step loop with all 15 steps

#include "brain/BrainStepProcessor.hpp"

namespace nlm {

void Brain::Impl::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        this->currentStep = currentStep;
        this->currentTime = currentTime;
        
        // =========== STEP 1: SENSORY INPUT ===========
        // Inject sensory input into sensory neurons based on environmental input
        try {
            for (NeuronId neuronId : sensoryNeurons) {
                for (auto& region : regions) {
                    for (auto& neuron : region->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            try {
                                neuron->injectSensoryInput(currentTime);
                            } catch (const std::exception& e) {
                                NLM_LOG_WARNING(std::string("Error injecting sensory input into neuron ") + 
                                               std::to_string(neuronId.index()) + ": " + std::string(e.what()));
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in sensory input step: ") + e.what());
        }
        
        // =========== STEP 2: SYNAPTIC DYNAMICS ===========
        // Update synaptic weights and connections based on plasticity rules
        try {
            for (const auto& connection : interRegionConnections) {
                try {
                    // Find source and target neurons
                    for (const auto& sourceRegion : regions) {
                        for (const auto& sourceNeuron : sourceRegion->getNeurons()) {
                            if (sourceNeuron->getId() == connection.sourceRegion) {
                                for (const auto& targetRegion : regions) {
                                    for (const auto& targetNeuron : targetRegion->getNeurons()) {
                                        if (targetNeuron->getId() == connection.targetRegion) {
                                            try {
                                                SynapseId synId = targetNeuron->addSynapse(sourceNeuron->getId(), connection.weight);
                                                if (auto* synapse = targetNeuron->getSynapse(synId)) {
                                                    synapse->update(currentTime, connection.delay);
                                                } else {
                                                    NLM_LOG_WARNING("Failed to get synapse after adding");
                                                }
                                            } catch (const std::exception& e) {
                                                NLM_LOG_ERROR(std::string("Error updating synapse: ") + e.what());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Exception in synaptic dynamics step: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in synaptic dynamics: ") + e.what());
        }
        
        // =========== STEP 3: SPIKE GENERATION ===========
        // Generate action potentials in neurons that reach threshold
        try {
            for (const auto& region : regions) {
                for (const auto& neuron : region->getNeurons()) {
                    try {
                        if (neuron->shouldFire(currentTime)) {
                            if (spikeSystem) {
                                spikeSystem->recordSpike(neuron->getId(), currentTime);
                                // Record motor spike for motor neurons
                                if (neuron->getType() == NeuronType::Motor) {
                                    spikeSystem->recordMotorSpike(neuron->getId(), currentTime);
                                }
                            }
                        }
                    } catch (const std::exception& e) {
                        NLM_LOG_WARNING(std::string("Error in spike generation for neuron ") + 
                                       std::to_string(neuron->getId().index()) + ": " + std::string(e.what()));
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in spike generation: ") + e.what());
        }
        
        // =========== STEP 4: PROPAGATION ===========
        // Propagate spike signals through neural networks
        try {
            if (spikeSystem) {
                spikeSystem->propagate(currentTime);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in spike propagation: ") + e.what());
        }
        
        // =========== STEP 5: PLASTICITY ===========
        // Update synaptic plasticity based on spike timing and activity
        try {
            // STDP: Spike-Timing Dependent Plasticity
            if (stdp) {
                try {
                    stdp->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error in STDP update: ") + e.what());
                }
            }
            
            // Hebbian plasticity: Activity-dependent strengthening
            if (hebbian) {
                try {
                    hebbian->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error in Hebbian update: ") + e.what());
                }
            }
            
            // Structural plasticity: Synapse formation and pruning
            if (structuralPlasticity) {
                try {
                    structuralPlasticity->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error in structural plasticity update: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in plasticity step: ") + e.what());
        }
        
        // =========== STEP 6: NEUROMODULATION ===========
        // Apply neuromodulatory signals to influence neural activity and plasticity
        try {
            if (dopamine) {
                try {
                    dopamine->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in dopamine update: ") + e.what());
                }
            }
            if (curiosity) {
                try {
                    curiosity->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in curiosity update: ") + e.what());
                }
            }
            if (predictionError) {
                try {
                    predictionError->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in prediction error update: ") + e.what());
                }
            }
            if (novelty) {
                try {
                    novelty->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in novelty update: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in neuromodulation: ") + e.what());
        }
        
        // =========== STEP 7: INTEGRATION ===========
        // Integrate information across memory systems
        try {
            if (workingMemory) {
                try {
                    workingMemory->integrate(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in working memory integration: ") + e.what());
                }
            }
            if (episodicMemory) {
                try {
                    episodicMemory->integrate(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in episodic memory integration: ") + e.what());
                }
            }
            if (associativeMemory) {
                try {
                    associativeMemory->integrate(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in associative memory integration: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in integration step: ") + e.what());
        }
        
        // =========== STEP 8: PREDICTION ===========
        // Update predictive models and prediction errors
        try {
            if (predictionSystem) {
                try {
                    predictionSystem->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in prediction system update: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in prediction step: ") + e.what());
        }
        
        // =========== STEP 9: COGNITION ===========
        // Execute cognitive processes: planning, concept formation, attention
        try {
            if (planner) {
                try {
                    planner->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in planner update: ") + e.what());
                }
            }
            if (conceptFormation) {
                try {
                    conceptFormation->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in concept formation update: ") + e.what());
                }
            }
            if (attention) {
                try {
                    attention->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in attention update: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in cognition step: ") + e.what());
        }
        
        // =========== STEP 10: DEVELOPMENT ===========
        // Apply developmental changes and plasticity modulation
        try {
            if (developmentSystem) {
                try {
                    developmentSystem->update(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in development system update: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in development step: ") + e.what());
        }
        
        // =========== STEP 11: MEMORY CONSOLIDATION ===========
        // Consolidate episodic memories into long-term storage
        try {
            if (currentStep % consolidationInterval == 0 && episodicMemory) {
                try {
                    episodicMemory->consolidate(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in memory consolidation: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in memory consolidation step: ") + e.what());
        }
        
        // =========== STEP 12: EPISODIC REPLAY ===========
        // Replay recent experiences to strengthen memory traces
        try {
            if (currentStep % replayInterval == 0 && !isResting && episodicMemory) {
                try {
                    episodicMemory->replay(currentTime);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error in episodic replay: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in episodic replay step: ") + e.what());
        }
        
        // =========== STEP 13: MOTOR OUTPUT ===========
        // Generate motor actions based on neural activity and planning
        // Motor neurons produce action potentials that drive behavior
        try {
            for (NeuronId neuronId : motorNeurons) {
                for (auto& region : regions) {
                    for (auto& neuron : region->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            try {
                                if (neuron->shouldFire(currentTime) && spikeSystem) {
                                    spikeSystem->recordMotorSpike(neuronId, currentTime);
                                }
                            } catch (const std::exception& e) {
                                NLM_LOG_WARNING(std::string("Error in motor neuron processing ") + 
                                               std::to_string(neuronId.index()) + ": " + std::string(e.what()));
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in motor output step: ") + e.what());
        }
        
        // =========== STEP 14: REST/DISABLE ===========
        // Enter rest state where neural activity is suppressed
        try {
            if (isResting) {
                for (const auto& region : regions) {
                    for (const auto& neuron : region->getNeurons()) {
                        try {
                            neuron->setActive(false);
                        } catch (const std::exception& e) {
                            NLM_LOG_WARNING(std::string("Error deactivating neuron ") + 
                                           std::to_string(neuron->getId().index()) + ": " + std::string(e.what()));
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in rest step: ") + e.what());
        }
        
        // =========== STEP 15: STATISTICS ===========
        // Update and collect neural activity statistics
        try {
            totalSpikesThisStep = 0;
            if (spikeSystem) {
                try {
                    totalSpikesThisStep = spikeSystem->getTotalSpikeCount();
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error getting spike count: ") + e.what());
                }
            }
            totalSpikesTotal += totalSpikesThisStep;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in statistics step: ") + e.what());
        }
        
        // Log status periodically for monitoring
        try {
            if (currentStep % 1000 == 0) {
                logStatus();
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in status logging: ") + e.what());
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in Brain step function: ") + e.what());
    }
}

} // namespace nlm