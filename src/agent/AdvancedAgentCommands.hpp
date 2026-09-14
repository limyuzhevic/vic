//==========================
// ADVANCED AGENT COMMANDS AND EXPLORATION
//===========================

class AdvancedAgentCommands {
public:
    AdvancedAgentCommands(std::shared_ptr<AgentBrain> agent) : agent_(agent) {}
    
    // == Advanced Exploration Commands ==
    
    // "explore_deep" - Deep exploration with curiosity amplification
    void exploreDeep(float duration = 60.0f) {
        if (!agent_) return;
        
        auto startTime = agent_->getBrain()->getConfig()->getOr<double>("simulation_timestep", 0.001);
        auto startStep = 0;
        
        agent_->enableCuriosity(true);
        agent_->enableRewardModulation(true);
        
        std::cout << "DEEP EXPLORATION MODE ACTIVATED" << std::endl;
        std::cout << "Curiosity: MAXED" << std::endl;
        std::cout << "Reward modulation: ENABLED" << std::endl;
        std::cout << "Duration: " << duration << " seconds" << std::endl;
        
        for (float time = 0; time < duration; time += 0.1f) {
            if (agent_->getCuriosityLevel() > 0.7f) {
                // High curiosity = random exploration
                auto cmd = agent_->decodeMotorCommand();
                std::cout << "Step " << time << ": HIGHLY CURIOUS - Exploring randomly" << std::endl;
            } else {
                // Normal operation
                auto cmd = agent_->decodeMotorCommand();
            }
            agent_->getBrain()->step(startStep++);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "Deep exploration completed" << std::endl;
    }
    
    // "explore_structural" - Structural plasticity exploration
    void exploreStructural(int cycles = 5) {
        if (!agent_ || !agent_->isStructuralPlasticityEnabled()) return;
        
        std::cout << "STRUCTURAL PLASTICITY EXPLORATION" << std::endl;
        std::cout << "Cycles: " << cycles << std::endl;
        
        for (int cycle = 0; cycle < cycles; ++cycle) {
            std::cout << "Cycle " << cycle + 1 << ": Inducing synaptogenesis and pruning" << std::endl;
            
            // Force structural plasticity
            auto brain = agent_->getBrain();
            for (auto& region : brain->getRegions()) {
                region->applySynaptogenesis(0.01f);
                region->applyPruning(0.001f);
            }
            
            // Run some steps to see effects
            for (int step = 0; step < 100; ++step) {
                brain->step(step);
            }
            
            auto memory = brain->getEpisodicMemory();
            if (memory) {
                auto episodes = memory->getEpisodesForReplay(5);
                std::cout << "  Stored " << memory->getEpisodeCount() << " episodes" << std::endl;
            }
        }
        
        std::cout << "Structural exploration completed" << std::endl;
    }
    
    // "explore_development" - Developmental exploration
    void exploreDevelopment(int stages = 3) {
        if (!agent_) return;
        
        std::cout << "DEVELOPMENTAL EXPLORATION" << std::endl;
        std::cout << "Stages: " << stages << std::endl;
        
        for (int stage = 0; stage < stages; ++stage) {
            std::cout << "Stage " << stage + 1 << ": ";
            
            // Set developmental stage
            nlm::DevelopmentalStage devStage;
            if (stage == 0) devStage = nlm::DevelopmentalStage::Initial;
            else if (stage == 1) devStage = nlm::DevelopmentalStage::CriticalPeriod;
            else devStage = nlm::DevelopmentalStage::Adult;
            
            agent_->getBrain()->setDevelopmentalStage(devStage);
            
            // Run exploration at this stage
            float stageDuration = (stages - stage) * 30.0f;
            for (int step = 0; step < 300; ++step) {
                agent_->getBrain()->step(step);
                
                if (step % 100 == 0) {
                    auto curStage = agent_->getBrain()->getDevelopmentalStage();
                    std::cout << static_cast<int>(curStage) << " ";
                }
            }
            
            std::cout << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << "Developmental exploration completed" << std::endl;
    }
    
    // == Advanced Memory Commands ==
    
    // "memory_stats" - Detailed memory statistics
    void printMemoryStats() {
        if (!agent_) return;
        
        auto brain = agent_->getBrain();
        auto workingMemory = brain->getWorkingMemory();
        auto episodicMemory = brain->getEpisodicMemory();
        auto attention = brain->getAttention();
        auto conceptFormation = brain->getConceptFormation();
        auto planner = brain->getPlanner();
        
        std::cout << "=== MEMORY SYSTEM STATISTICS ===" << std::endl;
        
        // Working memory stats
        if (workingMemory) {
            std::cout << "Working Memory:" << std::endl;
            std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
            std::cout << "  Capacity: " << workingMemory->getCapacity() << std::endl;
            std::cout << "  Memory neurons: " << workingMemory->getMemoryNeurons().size() << std::endl;
        }
        
        // Episodic memory stats
        if (episodicMemory) {
            std::cout << "Episodic Memory:" << std::endl;
            std::cout << "  Total episodes: " << episodicMemory->getEpisodeCount() << std::endl;
            std::cout << "  Max episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
            
            // Replay opportunities
            auto replayable = episodicMemory->getEpisodesForReplay(10);
            std::cout << "  Replable episodes: " << replayable.size() << std::endl;
            
            if (!replayable.empty()) {
                std::cout << "  Example episode (timestamp, reward): " 
                         << replayable[0]->timestamp << ", " 
                         << replayable[0]->reward << std::endl;
            }
        }
        
        // Cognitive system stats
        if (attention) {
            std::cout << "Attention System:" << std::endl;
            // Get attention focus details
            std::cout << "  Attention enabled" << std::endl;
        }
        
        if (conceptFormation) {
            std::cout << "Concept Formation:" << std::endl;
            std::cout << "  Concept tracking active" << std::endl;
        }
        
        if (planner) {
            std::cout << "Neural Planner:" << std::endl;
            std::cout << "  Planning depth: " << planner->getPlanningDepth() << std::endl;
        }
        
        std::cout << "=== END MEMORY STATS ===" << std::endl;
    }
    
    // "memory_replay" - Replay specific episodes
    void replayMemory(int episodeCount = 5) {
        if (!agent_) return;
        
        auto brain = agent_->getBrain();
        auto episodicMemory = brain->getEpisodicMemory();
        
        if (!episodicMemory) {
            std::cout << "No episodic memory available for replay" << std::endl;
            return;
        }
        
        std::cout << "MEMORY REPLAY - Loading " << episodeCount << " episodes" << std::endl;
        
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(episodeCount);
        
        for (int i = 0; i < episodesToReplay.size() && i < episodeCount; ++i) {
            const auto* episode = episodesToReplay[i];
            std::cout << "Episode " << episode->timestamp << ": "
                     << episode->activeNeurons.size() << " active neurons, "
                     << episode->reward << " reward" << std::endl;
        }
        
        std::cout << "Replay completed" << std::endl;
    }
    
    // == Advanced Neuromodulation Commands ==
    
    // "neuromodulate" - Direct neuromodulation control
    void neuromodulate(const std::string& type, float level) {
        if (!agent_) return;
        
        auto brain = agent_->getBrain();
        
        if (type == "dopamine") {
            auto dopamine = brain->getDopamine();
            if (dopamine) {
                dopamine->setLevel(level);
                std::cout << "Dopamine level set to: " << level << std::endl;
            }
        } else if (type == "curiosity") {
            agent_->enableCuriosity(true);
            std::cout << "Curiosity enabled with target level: " << level << std::endl;
        } else if (type == "novelty") {
            // Novelty is typically auto-computed
            std::cout << "Novelty detection is typically auto-computed from sensory input" << std::endl;
        } else if (type == "prediction_error") {
            // Prediction error is computed from reward prediction
            std::cout << "Prediction error is computed during reward processing" << std::endl;
        }
    }
    
    // "neuro_state" - Get current neuromodulation state
    void printNeuroModState() {
        if (!agent_) return;
        
        std::cout << "=== NEUROMODULATION STATE ===" << std::endl;
        std::cout << "Dopamine level: " << agent_->getNeuromodulationLevel() << std::endl;
        std::cout << "Curiosity level: " << agent_->getCuriosityLevel() << std::endl;
        std::cout << "Novelty level: " << agent_->getNoveltyLevel() << std::endl;
        std::cout << "Prediction error: " << agent_->getPredictionError() << std::endl;
        std::cout << "Developmental stage: " << static_cast<int>(agent_->getDevelopmentalStage()) << std::endl;
        std::cout << "=== END NEURO STATE ===" << std::endl;
    }
    
    // == Advanced Simulation Commands ==
    
    // "checkpoint" - Advanced checkpoint management
    void checkpoint(const std::string& name = "auto") {
        if (!agent_) return;
        
        auto brain = agent_->getBrain();
        std::string filepath = "checkpoints/" + name + ".bin";
        
        std::cout << "Creating checkpoint: " << filepath << std::endl;
        
        bool success = brain->save(filepath);
        if (success) {
            std::cout << "Checkpoint created successfully" << std::endl;
        } else {
            std::cout << "Failed to create checkpoint (may not be fully implemented)" << std::endl;
        }
    }
    
    // "restore" - Restore from checkpoint
    void restore(const std::string& name) {
        if (!agent_) return;
        
        std::string filepath = "checkpoints/" + name + ".bin";
        
        std::cout << "Restoring from checkpoint: " << filepath << std::endl;
        
        bool success = agent_->getBrain()->load(filepath);
        if (success) {
            std::cout << "Restoration completed successfully" << std::endl;
        } else {
            std::cout << "Failed to restore from checkpoint" << std::endl;
        }
    }
    
    // "benchmark" - Performance benchmark
    void benchmark(int steps = 1000) {
        if (!agent_) return;
        
        std::cout << "PERFORMANCE BENCHMARK" << std::endl;
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            agent_->getBrain()->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double stepsPerSecond = (steps * 1000.0) / duration.count();
        
        std::cout << "Results:" << std::endl;
        std::cout << "  Total steps: " << steps << std::endl;
        std::cout << "  Duration: " << duration.count() << " ms" << std::endl;
        std::cout << "  Speed: " << stepsPerSecond << " steps/sec" << std::endl;
        std::cout << "  Average time per step: " << (duration.count() * 1000.0 / steps) << " µs" << std::endl;
        
        auto brain = agent_->getBrain();
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    }
    
    // == Advanced Utility Commands ==
    
    // "help" - Show all available advanced commands
    void help() {
        std::cout << "=== ADVANCED NLM AGENT COMMANDS ===" << std::endl << std::endl;
        
        std::cout << "=== Exploration Commands ===" << std::endl;
        std::cout << "  explore_deep <duration>    - Deep exploration with maximum curiosity" << std::endl;
        std::cout << "  explore_structural <cycles> - Structural plasticity exploration" << std::endl;
        std::cout << "  explore_development <stages> - Developmental stage exploration" << std::endl;
        
        std::cout << std::endl << "=== Memory Commands ===" << std::endl;
        std::cout << "  memory_stats              - Display detailed memory system statistics" << std::endl;
        std::cout << "  memory_replay <count>     - Replay stored episodes" << std::endl;
        
        std::cout << std::endl << "=== Neuromodulation Commands ===" << std::endl;
        std::cout << "  neuromodulate <type> <level> - Direct neuromodulation control" << std::endl;
        std::cout << "  neuro_state               - Get current neuromodulation state" << std::endl;
        
        std::cout << std::endl << "=== Checkpoint Commands ===" << std::endl;
        std::cout << "  checkpoint <name>         - Save brain state to checkpoint" << std::endl;
        std::cout << "  restore <name>            - Restore brain state from checkpoint" << std::endl;
        
        std::cout << std::endl << "=== Analysis Commands ===" << std::endl;
        std::cout << "  benchmark <steps>          - Performance benchmark" << std::endl;
        
        std::cout << std::endl << "=== Configuration Commands ===" << std::endl;
        std::cout << "  enable <feature>           - Enable a specific feature" << std::endl;
        std::cout << "  disable <feature>          - Disable a specific feature" << std::endl;
        
        std::cout << std::endl << "=== Other Commands ===" << std::endl;
        std::cout << "  help                      - Show this help" << std::endl;
        std::cout << "  status                    - Show agent status" << std::endl;
        
        std::cout << std::endl << "=== Available Features ===" << std::endl;
        std::cout << "  Reward modulation          - Enable reward-based learning" << std::endl;
        std::cout << "  Structural plasticity      - Enable synapse growth/pruning" << std::endl;
        std::cout << "  Development                - Enable developmental stages" << std::endl;
        std::cout << "  Curiosity                  - Enable curiosity-driven exploration" << std::endl;
        
        std::cout << std::endl << "=== END ADVANCED COMMANDS ===" << std::endl;
    }
    
    // "enable" - Enable a feature
    void enableFeature(const std::string& feature) {
        if (!agent_) return;
        
        if (feature == "reward_modulation") {
            agent_->enableRewardModulation(true);
            std::cout << "✓ Reward modulation enabled" << std::endl;
        } else if (feature == "structural_plasticity") {
            agent_->enableStructuralPlasticity(true);
            std::cout << "✓ Structural plasticity enabled" << std::endl;
        } else if (feature == "development") {
            agent_->enableDevelopment(true);
            std::cout << "✓ Development enabled" << std::endl;
        } else if (feature == "curiosity") {
            agent_->enableCuriosity(true);
            std::cout << "✓ Curiosity enabled" << std::endl;
        } else {
            std::cout << "Unknown feature: " << feature << std::endl;
        }
    }
    
    // "disable" - Disable a feature
    void disableFeature(const std::string& feature) {
        if (!agent_) return;
        
        if (feature == "reward_modulation") {
            agent_->enableRewardModulation(false);
            std::cout << "✓ Reward modulation disabled" << std::endl;
        } else if (feature == "structural_plasticity") {
            agent_->enableStructuralPlasticity(false);
            std::cout << "✓ Structural plasticity disabled" << std::endl;
        } else if (feature == "development") {
            agent_->enableDevelopment(false);
            std::cout << "✓ Development disabled" << std::endl;
        } else if (feature == "curiosity") {
            agent_->enableCuriosity(false);
            std::cout << "✓ Curiosity disabled" << std::endl;
        } else {
            std::cout << "Unknown feature: " << feature << std::endl;
        }
    }
    
    // "status" - Show agent status
    void status() {
        if (!agent_) return;
        
        std::cout << "=== AGENT STATUS ===" << std::endl;
        
        auto brain = agent_->getBrain();
        std::cout << "Brain Status:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        std::cout << std::endl << "AgentBrain Status:" << std::endl;
        std::cout << "  Reward modulation: " << (agent_->isRewardModulationEnabled() ? "enabled" : "disabled") << std::endl;
        std::cout << "  Structural plasticity: " << (agent_->isStructuralPlasticityEnabled() ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (agent_->isDevelopmentEnabled() ? "enabled" : "disabled") << std::endl;
        std::cout << "  Curiosity: " << (agent_->isCuriosityEnabled() ? "enabled" : "disabled") << std::endl;
        
        std::cout << std::endl << "Neuromodulation:" << std::endl;
        std::cout << "  Dopamine level: " << agent_->getNeuromodulationLevel() << std::endl;
        std::cout << "  Curiosity level: " << agent_->getCuriosityLevel() << std::endl;
        std::cout << "  Novelty level: " << agent_->getNoveltyLevel() << std::endl;
        std::cout << "  Prediction error: " << agent_->getPredictionError() << std::endl;
        
        std::cout << "  Developmental stage: " << static_cast<int>(agent_->getDevelopmentalStage()) << std::endl;
        
        std::cout << "=== END AGENT STATUS ===" << std::endl;
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
};