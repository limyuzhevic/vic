    // ... (rest of the code unchanged)

    // ========== STEP 8: Prediction error influences attention (bottom-up salience) ==========
    if (pImpl->predictionError) {
        float predictionError = pImpl->predictionError->getError();
        if (predictionError > 0.0f) {
            // Use prediction error magnitude as bottom-up salience
            // Apply to all active neurons to increase their saliency
            std::vector<float> workingMemoryPattern = pImpl->workingMemory ? 
                pImpl->workingMemory->retrieve() : std::vector<float>();
            
            if (!workingMemoryPattern.empty()) {
                for (size_t i = 0; i < workingMemoryPattern.size(); ++i) {
                    if (workingMemoryPattern[i] > 0.1f) {  // Only high-activity neurons
                        float salienceStrength = predictionError * 0.1f * workingMemoryPattern[i];
                        pImpl->attention->applyBottomUpSalience(
                            NeuronId(20000 + i), // Assign unique neuron IDs
                            salienceStrength);
                    }
                }
            }
        }
    }

    // ========== STEP 9: Attention influences concept formation ==========
    if (pImpl->attention && pImpl->conceptFormation) {
        // After attention updates, apply top-down bias based on attended neurons
        std::vector<float> attentionWorkingMemory = pImpl->workingMemory ? 
            pImpl->workingMemory->retrieve() : std::vector<float>();
        const auto& winners = pImpl->attention->getWinners();
        for (const auto& winnerId : winners) {
            // Get top-down bias from attention system for attended neurons
            float topDownBias = pImpl->attention->getInhibitionFor(winnerId);
            
            // Apply this as bias to concept formation
            if (topDownBias > 0.0f && !attentionWorkingMemory.empty()) {
                // Find related concepts or create new ones based on attended patterns
                const auto& concepts = pImpl->conceptFormation->getConcepts();
                for (const auto& concept : concepts) {
                    // If this concept's prototype overlaps with attended neurons,
                    // strengthen the concept through attention bias
                    float overlap = 0.0f;
                    for (size_t i = 0; i < std::min(concept.prototype.size(), attentionWorkingMemory.size()); ++i) {
                        if (concept.prototype[i] > 0.5f && winnerId.value >= 20000 && winnerId.value < 20000 + attentionWorkingMemory.size()) {
                            overlap += concept.prototype[i] * attentionWorkingMemory[i];
                        }
                    }
                    if (overlap > 0.3f) {
                        // Use concept update with attention-based bias
                        pImpl->conceptFormation->updateConcept(concept.id, 
                                                              attentionWorkingMemory,
                                                              attentionWorkingMemory,
                                                              0.5f * topDownBias);
                    }
                }
            }
        }
    }

    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // Extract working memory patterns and sensory input for concept formation
        
        // 0. FIRST: Use associative memory for pattern completion
        std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
        std::vector<float> completedPattern = workingMemoryPattern;
        if (pImpl->associativeMemory && !workingMemoryPattern.empty()) {
            // Use associative memory to complete partial patterns
            std::vector<std::vector<float>> completedPatterns = 
                pImpl->associativeMemory->retrieve(workingMemoryPattern, 3);
            
            if (!completedPatterns.empty()) {
                // Fill in missing information with the best completed pattern
                // Use the highest similarity pattern (first one) for completion
                completedPattern = completedPatterns[0];
                
                // Also use associative memory to spread activation and discover related patterns
                std::vector<std::vector<float>> activatedPatterns = 
                    pImpl->associativeMemory->spreadActivation(workingMemoryPattern, 1);
                
                // Incorporate spread activation into current pattern for enhanced processing
                for (const auto& activatedPattern : activatedPatterns) {
                    if (!activatedPattern.empty()) {
                        // Blend activated pattern with current pattern for richer input
                        for (size_t i = 0; i < std::min(completedPattern.size(), activatedPattern.size()); ++i) {
                            completedPattern[i] = (completedPattern[i] + activatedPattern[i]) * 0.5f;
                        }
                    }
                }
            }
        }
        
        // 1. Get working memory content (neural activity patterns) - enhance with completed pattern
        std::vector<float> enhancedWorkingMemoryPattern = workingMemoryPattern;
        
        // 1b. Enhance working memory with associative pattern completion
        if (!completedPattern.empty() && completedPattern != workingMemoryPattern) {
            // Store the completed pattern in working memory as well
            pImpl->workingMemory->store(completedPattern, 0.8f);
        }
        
        // 2. Present experience to concept formation system
        if (!enhancedWorkingMemoryPattern.empty()) {
            // Use dopamine level as feedback signal for reward-modulated learning
            float avgReward = pImpl->dopamine ? 
                pImpl->dopamine->getLevel() : 0.5f;
            
            // Get features from neural activity patterns (normalized)
            std::vector<float> features = enhancedWorkingMemoryPattern;
            
            // PRESENT TO CONCEPT FORMATION - this now includes associative memory context
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                enhancedWorkingMemoryPattern, features, avgReward, currentStep);
            
            // Store concept associations in associative memory
            if (pImpl->associativeMemory && conceptId > 0) {
                // Get concept prototype
                std::vector<float> conceptPattern = 
                    pImpl->conceptFormation->getConceptPrototype(conceptId);
                if (!conceptPattern.empty()) {
                    // Store concept pattern in associative memory with episodic context
                    if (pImpl->episodicMemory) {
                        // Associate concept with recent episodic memory
                        auto episodes = pImpl->episodicMemory->retrieveSimilar(
                            enhancedWorkingMemoryPattern, 3);
                        
                        for (const auto* episode : episodes) {
                            if (episode) {
                                // Add association between concept and episodic context
                                pImpl->associativeMemory->associate(
                                    conceptPattern, episode->sensoryState, 0.7f);
                            }
                        }
                    }
                    
                    // Self-associate the concept with itself for internal consistency
                    pImpl->associativeMemory->associate(
                        conceptPattern, conceptPattern, 0.9f);
                }
            }
            
            // 3. Link concepts to prediction system (concepts guide predictions)
            if (pImpl->predictionSystem && conceptId > 0) {
                // Use the concept pattern to guide predictions
                std::vector<float> conceptPattern = 
                    pImpl->conceptFormation->getConceptPrototype(conceptId);
                if (!conceptPattern.empty()) {
                    // Create InternalSignals for concept pattern
                    nlm::InternalSignals sensoryInput;
                    for (float val : conceptPattern) {
                        sensoryInput.addSignal(val);
                    }
                    sensoryInput.setTimestamp(static_cast<double>(currentStep) * pImpl->timestep);
                    
                    // Train prediction system with the concept
                    pImpl->predictionSystem->train(sensoryInput);
                }
            }
            
            // 4. Use concepts to influence attention (concept-based attention bias)
            if (pImpl->attention && conceptId > 0) {
                // Apply top-down bias based on learned concepts
                const auto& concepts = pImpl->conceptFormation->getConcepts();
                for (const auto& concept : concepts) {
                    // Apply attention bias to relevant neurons based on concept
                    for (size_t i = 0; i < concept.prototype.size(); ++i) {
                        if (concept.prototype[i] > 0.5f) {
                            // Apply top-down attention bias based on concept stability
                            pImpl->attention->applyTopDownBias(
                                NeuronId(10000 + concept.id), 
                                0.3f * concept.avgStability);
                        }
                    }
                }
            }
            
            // 5. Feed concepts to neural planner (concept-based planning)
            if (pImpl->planner && conceptId > 0) {
                // Set current goal based on the most relevant concept
                const auto& concepts = pImpl->conceptFormation->getConcepts();
                for (const auto& concept : concepts) {
                    if (concept.id == conceptId) {
                        // Use concept prototype as goal state for planning
                        pImpl->planner->setCurrentGoal(concept.prototype);
                        break;
                    }
                }
                
                // 6. Plan actions based on current goal using neural planner
                if (pImpl->workingMemory) {
                    // Get current sensory state (working memory content)
                    std::vector<float> currentState = pImpl->workingMemory->retrieve();
                    
                    if (!currentState.empty() && !pImpl->planner->getCurrentGoal().empty()) {
                        // Use neural planner to plan next action
                        ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
                        
                        // Evaluate the planned action sequence
                        std::vector<ActionType> actionSequence = {plannedAction};
                        PlanningCandidate evaluation = pImpl->planner->evaluateSequence(actionSequence, currentState);
                        
                        // Store planned actions in working memory for memory
                        // Create a structured plan entry in working memory
                        std::vector<float> plannedSequence;
                        plannedSequence.reserve(actionSequence.size());
                        for (ActionType action : actionSequence) {
                            plannedSequence.push_back(static_cast<float>(action));
                        }
                        
                        // Store the plan in working memory with its expected reward
                        pImpl->workingMemory->store(plannedSequence, evaluation.confidence);
                        
                        // 7. Enable planner to update action quality from experience
                        // Store the planned action for later quality update based on actual execution
                        if (pImpl->episodicMemory) {
                            // Store planning outcome in episodic memory for learning and feedback
                            EpisodicMemoryItem planningOutcome;
                            planningOutcome.timestamp = currentStep;
                            planningOutcome.sensoryState = currentState;
                            planningOutcome.action = plannedAction;
                            planningOutcome.reward = evaluation.expectedReward;
                            planningOutcome.novelty = 0.0f;  // Will be calculated if needed
                            
                            // Store this planning outcome
                            pImpl->episodicMemory->storeEpisode(planningOutcome);
                        }
                        
                        // 8. Planning outcomes feed back to prediction system (learn from action effects)
                        if (pImpl->predictionSystem) {
                            // Create prediction from planning outcome
                            nlm::InternalSignals actionOutcome;
                            for (float val : plannedSequence) {
                                actionOutcome.addSignal(val);
                            }
                            actionOutcome.setTimestamp(static_cast<double>(currentStep) * pImpl->timestep);
                            
                            // Update prediction system with actual action outcome
                            pImpl->predictionSystem->train(actionOutcome);
                            
                            // Compute prediction error for this action
                            // Get current prediction error signal
                            if (pImpl->predictionError) {
                                // Use the planned action as prediction and current state as actual
                                float predictedValue = evaluation.expectedReward;
                                pImpl->predictionError->computeError(predictedValue, evaluation.expectedReward);
                            }
                        }
                    }
                }
            }
        }
        
        // 9. Allow concept updates from episodic memory replay
        // Also, store episodic memory in associative memory for future pattern completion
        if (pImpl->episodicMemory && currentStep % pImpl->replayInterval == 0) {
            // Get episodes for replay (concept updates occur during replay)
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            
            // Use associative memory to store these episodic patterns for later completion
            if (pImpl->associativeMemory) {
                for (const auto* episode : episodesToReplay) {
                    if (episode && !episode->sensoryState.empty()) {
                        // Store the episode pattern in associative memory
                        // This creates associations between sensory patterns and outcomes
                        pImpl->associativeMemory->associateFromExperience(*episode);
                        
                        // Also spread activation to discover related patterns
                        std::vector<std::vector<float>> relatedPatterns = 
                            pImpl->associativeMemory->spreadActivation(episode->sensoryState, 1);
                    }
                }
            }
            
            for (const auto* episode : episodesToReplay) {
                // Extract neural patterns from episode
                std::vector<float> episodePattern = episode->sensoryState;
                
                // Present episode to concept formation for updating concepts
                if (!episodePattern.empty()) {
                    float episodeReward = episode->reward;
                    pImpl->conceptFormation->presentExperience(
                        episodePattern, episodePattern, episodeReward, 
                        episode->timestamp);
                }
            }
        }
        
        // 10. Implement concept consolidation and pruning
        if (currentStep % pImpl->consolidationInterval == 0) {
            // Consolidate concepts - merge similar concepts, prune unstable ones
            const auto& concepts = pImpl->conceptFormation->getConcepts();
            
            // Find concepts that need merging (similar enough)
            for (size_t i = 0; i < concepts.size(); ++i) {
                for (size_t j = i + 1; j < concepts.size(); ++j) {
                    float similarity = pImpl->conceptFormation->computeSimilarity(
                        concepts[i].prototype, concepts[j].prototype);
                    
                    if (similarity > 0.85f) {
                        // Merge similar concepts (Hebbian merging)
                        pImpl->conceptFormation->mergeConcepts(
                            concepts[i].id, concepts[j].id);
                    }
                }
            }
            
            // Prune unstable concepts (concept stability < threshold)
            for (const auto& concept : concepts) {
                if (pImpl->conceptFormation->getConceptStability(concept.id) < 0.5f) {
                    // Mark for removal (in a full implementation, we would remove the concept)
                    // For now, we log the pruning action
                }
            }
        }
    }
    
    // ... (rest of the code unchanged)
}
