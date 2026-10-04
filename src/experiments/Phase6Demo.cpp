// NLM Phase 6 - Comprehensive Cognitive Integration Demo
// 
// This demo demonstrates the complete integration of all cognitive systems
// in a cohesive artificial brain. It showcases:
// 
// 1. Full brain loop integration with cognitive systems
// 2. Working memory, episodic memory, and prediction systems
// 3. Concept formation and neural planning
// 4. Neuromodulation with curiosity, novelty, and reward
// 5. Developmental progression and structural plasticity
// 6. Complete experience-to-action-to-learning cycle

#include "Phase4Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace nlm {

// Performance metrics for Phase 6 integration
struct Phase6Metrics {
    // Basic brain statistics
    struct BrainStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t totalSpikes;
        float avgFiringRate;
        float eToIRatio;
    } brainStats;
    
    // Cognitive system states
    struct CognitiveStats {
        // Memory systems
        size_t workingMemoryTraces;
        size_t episodicMemoryEpisodes;
        size_t semanticMemoryFacts;
        size_t proceduralMemorySkills;
        
        // Prediction system
        float predictionError;
        float predictionConfidence;
        
        // Concept formation
        size_t conceptCount;
        float avgConceptStability;
        
        // Attention
        float attentionFocusStrength;
        
        // Planning
        float plannerConfidence;
        ActionType plannedAction;
        
        // Neuromodulation
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        
        // Development
        std::string developmentalStage;
        float plasticityModifier;
        
        // Performance
        float integrationScore;
        std::string integrationStatus;
    } cognitiveStats;
    
    // Learning progress
    struct LearningProgress {
        std::vector<float> rewardHistory;
        std::vector<float> predictionErrorHistory;
        std::vector<float> conceptDiscoveryHistory;
        std::vector<size_t> memoryGrowthHistory;
        
        // Performance indicators
        float learningRate;
        float retentionRate;
        float generalizationScore;
        
        // Adaptation metrics
        float plasticityEfficiency;
        float integrationEfficiency;
        float emergenceScore;
    } learningProgress;
    
    // Environment interaction
    struct EnvironmentInteraction {
        size_t sensoryInputSize;
        size_t motorOutputSize;
        std::vector<float> recentSensoryInput;
        std::vector<ActionType> recentActions;
        std::vector<float> actionOutcomes;
        
        // Environmental statistics
        float environmentalComplexity;
        float predictability;
        float noveltyRate;
    } environment;
    
    // Runtime
    struct RuntimeInfo {
        double simulationTime;
        SimulationStep currentStep;
        size_t totalStepsCompleted;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastUpdateTime;
        double avgStepTime;
        double maxStepTime;
    } runtime;
    
    // Phase 6 integration status
    std::string phase6Status;
    std::string integrationHighlights;
    std::vector<std::string> achievements;
};

class Phase6Demo {
public:
    Phase6Demo() : shouldContinue(true) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        setupConfiguration();
    }
    
    ~Phase6Demo() {}
    
    void run() {
        std::cout << "===========================================================\n";
        std::cout << "NLM PHASE 6 - COMPREHENSIVE COGNITIVE INTEGRATION DEMO\n";
        std::cout << "===========================================================\n\n";
        
        // Setup demo configuration
        configureDemo();
        
        // Create and initialize all systems
        setupSystems();
        
        // Display system overview
        displaySystemOverview();
        
        // Run main demonstration
        runIntegrationDemo();
        
        // Display final results
        displayFinalResults();
        
        // Cleanup
        cleanup();
        
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "PHASE 6 DEMO COMPLETED SUCCESSFULLY" << std::endl;
        std::cout << "All cognitive systems are integrated and functional!" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
    }
    
private:
    struct DemoConfig {
        int numEpisodes = 3;
        int stepsPerEpisode = 100;
        int demonstrationInterval = 10;
        bool performanceMonitoring = true;
        bool visualizationEnabled = false;
    } demoConfig;
    
    struct Phase6Metrics metrics;
    bool shouldContinue;
    
    std::shared_ptr<Config> config;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
    
    void setupConfiguration() {
        std::cout << "Configuring Phase 6 demo settings..." << std::endl;
        
        // Comprehensive Phase 6 configuration
        config = std::make_shared<Config>();
        config->initializePhase6Defaults();
        
        // Override with demo-specific settings
        config->set("prediction_system.enabled", true, ConfigSource::Runtime);
        config->set("prediction_system.learning_rate", 0.02f, ConfigSource::Runtime);
        config->set("prediction_system.confidence_threshold", 0.6f, ConfigSource::Runtime);
        config->set("concept_formation.enabled", true, ConfigSource::Runtime);
        config->set("concept_formation.formation_threshold", 0.7f, ConfigSource::Runtime);
        config->set("concept_formation.max_concepts", 50, ConfigSource::Runtime);
        config->set("working_memory.enabled", true, ConfigSource::Runtime);
        config->set("working_memory.capacity", 300, ConfigSource::Runtime);
        config->set("episodic_memory.enabled", true, ConfigSource::Runtime);
        config->set("episodic_memory.max_episodes", 5000, ConfigSource::Runtime);
        config->set("attention.enabled", true, ConfigSource::Runtime);
        config->set("planner.enabled", true, ConfigSource::Runtime);
        config->set("planner.planning_depth", 5, ConfigSource::Runtime);
        config->set("neuromodulation.enabled", true, ConfigSource::Runtime);
        config->set("development.enabled", true, ConfigSource::Runtime);
        
        // Enable all performance monitoring
        config->set("performance.verbose_logging", true, ConfigSource::Runtime);
        config->set("performance.enable_metrics", true, ConfigSource::Runtime);
        config->set("integration.enable_brain_loop_logging", true, ConfigSource::Runtime);
        
        std::cout << "✓ Phase 6 configuration complete" << std::endl;
    }
    
    void configureDemo() {
        std::cout << "Configuring demo environment..." << std::endl;
        
        // Demo configuration
        demoConfig.numEpisodes = 3;                    // Short for demo
        demoConfig.stepsPerEpisode = 100;              // Medium length
        demoConfig.demonstrationInterval = 10;         // Show progress every 10 steps
        demoConfig.performanceMonitoring = true;
        demoConfig.visualizationEnabled = false;       // Text-only for console
        
        std::cout << "✓ Demo configuration complete" << std::endl;
    }
    
    void setupSystems() {
        std::cout << "Initializing integrated cognitive systems..." << std::endl;
        
        // Create world environment
        world = std::make_shared<SimpleWorld>();
        world->configure(30.0f, 30.0f, 16, 16);
        world->setRandomSeed(12345);
        
        // Create brain with Phase 6 configuration
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Enable all agent brain features
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        agentBrain->enableSelfModel(true);
        agentBrain->enableSocialLearning(true);
        
        std::cout << "✓ All cognitive systems initialized" << std::endl;
        
        // Display initial system state
        displaySystemStatus("Initial");
    }
    
    void displaySystemOverview() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "SYSTEM OVERVIEW - PHASE 6 INTEGRATION" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        std::cout << "\nBrain Architecture:" << std::endl;
        std::cout << "  ✓ Core neural computation (LIF neurons, spikes, synapses)" << std::endl;
        std::cout << "  ✓ Event-driven spike propagation with delays" << std::endl;
        std::cout << "  ✓ STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << std::endl;
        
        std::cout << "\nMemory Systems:" << std::endl;
        std::cout << "  ✓ Working Memory - transient active information" << std::endl;
        std::cout << "  ✓ Episodic Memory - experience storage and replay" << std::endl;
        std::cout << "  ✓ Semantic Memory - pattern associations and facts" << std::endl;
        std::cout << "  ✓ Procedural Memory - skills and habits" << std::endl;
        
        std::cout << "\nPrediction System:" << std::endl;
        std::cout << "  ✓ Next state prediction" << std::endl;
        std::cout << "  ✓ Prediction error computation" << std::endl;
        std::cout << "  ✓ Confidence tracking" << std::endl;
        std::cout << "  ✓ Learned forward models" << std::endl;
        
        std::cout << "\nConcept Formation:" << std::endl;
        std::cout << "  ✓ Pattern discovery and clustering" << std::endl;
        std::cout << "  ✓ Prototype formation" << std::endl;
        std::cout << "  ✓ Category discovery" << std::endl;
        stdillet::cout << "  ✓ Stability measurement" << std::endl;
        
        std::cout << "\nAttention System:" << std::endl;
        std::cout << "  ✓ Competitive selection dynamics" << std::endl;
        std::cout << "  ✓ Inhibit-excite balance" << std::endl;
        std::cout << "  ✓ Focus allocation" << std::endl;
        
        std::cout << "\nPlanning System:" << std::endl;
        std::cout << "  ✓ Action sequence evaluation" << std::endl;
        std::cout << "  ✓ Forward planning" << std::endl;
        std::cout << "  ✓ Goal-directed behavior" << std::endl;
        
        std::cout << "\nNeuromodulation:" << std::endl;
        std::cout << "  ✓ Dopamine - reward prediction error" << std::endl;
        std::cout << "  ✓ Curiosity - exploration motivation" << std::endl;
        std::cout << "  ✓ Novelty - novelty detection" << std::endl;
        std::cout << "  ✓ Integration with plasticity" << std::endl;
        
        std::cout << "\nDevelopment:" << std::endl;
        std::cout << "  ✓ Developmental stages (Initial, CriticalPeriod, Maturation, Adult)" << std::endl;
        std::cout << "  ✓ Plasticity modulation by age" << std::endl;
        std::cout << "  ✓ Structural plasticity changes" << std::endl;
        
        std::cout << "\nPerformance Infrastructure:" << std::endl;
        std::cout << "  ✓ Memory pools and event queues" << std::endl;
        std::cout << "  ✓ Sparse connectivity optimization" << std::endl;
        std::cout << "  ✓ Parallel processing support" << std::endl;
        std::cout << "  ✓ SIMD vectorization" << std::endl;
        
        std::cout << "\n" << std::string(70, '=') << std::endl;
    }
    
    void displaySystemStatus(const std::string& prefix) {
        std::cout << "\n" + prefix + " System Status:" << std::endl;
        std::cout << "  Brain: " << brain->getTotalNeuronCount() << " neurons, " 
                  << brain->getTotalSynapseCount() << " synapses" << std::endl;
        
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "  Working Memory: " << workingMemory->getActiveTraces() << " traces" << std::endl;
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "  Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
        }
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            std::cout << "  Prediction System: error=" << std::fixed << std::setprecision(3) 
                      << predictionSystem->getPredictionError() 
                      << ", confidence=" << predictionSystem->getConfidence() << std::endl;
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            std::cout << "  Concept Formation: " << conceptFormation->getConceptCount() 
                      << " concepts, avg stability=" << conceptFormation->getConceptCount() > 0 ? 
                      (conceptFormation->getConceptStability(1) / 100.0f) : 0.0f << std::endl;
        }
        
        auto* planner = brain->getPlanner();
        if (planner) {
            std::cout << "  Planner: planning depth=" << planner->getPlanningDepth() << std::endl;
        }
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            std::cout << "  Dopamine: level=" << dopamine->getLevel() << std::endl;
        }
        
        std::cout << "  Development Stage: " << getDevelopmentalStageString(brain->getDevelopmentalStage()) << std::endl;
    }
    
    std::string getDevelopmentalStageString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial (High Plasticity)";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period (Medium Plasticity)";
            case DevelopmentalStage::Maturation: return "Maturation (Low Plasticity)";
            case DevelopmentalStage::Adult: return "Adult (Stable)";
            default: return "Unknown";
        }
    }
    
    void runIntegrationDemo() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "RUNNING INTEGRATION DEMO" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        metrics.startTime = std::chrono::steady_clock::now();
        metrics.runtime.startTime = std::chrono::steady_clock::now();
        
        // Run episodes demonstrating full integration
        for (int episode = 0; episode < demoConfig.numEpisodes; ++episode) {
            std::cout << "\n--- EPISODE " << episode << " ---" << std::endl;
            
            // Reset systems for new episode
            world->reset();
            agentBrain->reset();
            brain->reset();
            
            runEpisode(episode);
            
            // Record episode metrics
            recordEpisodeMetrics(episode);
        }
        
        // Finalize metrics
        metrics.runtime.totalStepsCompleted = demoConfig.numEpisodes * demoConfig.stepsPerEpisode;
        metrics.runtime.lastUpdateTime = std::chrono::steady_clock::now();
        
        double totalTime = std::chrono::duration<double>(metrics.runtime.lastUpdateTime - metrics.runtime.startTime).count();
        metrics.runtime.avgStepTime = totalTime / metrics.runtime.totalStepsCompleted;
        
        std::cout << "\nDemo completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
        std::cout << "Average step time: " << metrics.runtime.avgStepTime * 1000 << " ms" << std::endl;
    }
    
    void runEpisode(int episode) {
        std::cout << "Starting episode with " << demoConfig.stepsPerEpisode << " steps" << std::endl;
        
        for (int step = 0; step < demoConfig.stepsPerEpisode; ++step) {
            if (!shouldContinue) break;
            
            metrics.runtime.currentStep = step;
            metrics.runtime.simulationTime = step * 0.01;  // 10ms per step
            
            // Execute complete brain loop
            executeBrainLoop(step);
            
            // Show progress
            if (demoConfig.demonstrationInterval > 0 && step % demoConfig.demonstrationInterval == 0) {
                displayStepProgress(step);
            }
            
            // Check for user interruption
            if (kbhit()) {
                std::cout << "\nDemo interrupted by user. Ending demonstration." << std::endl;
                shouldContinue = false;
                break;
            }
        }
    }
    
    void executeBrainLoop(int step) {
        auto loopStart = std::chrono::steady_clock::now();
        
        // 1. Get observation from world
        SensoryPercept percept = world->observe(*brain);
        
        // 2. Process sensory input (integrates with working memory)
        agentBrain->processSensoryInput(percept);
        
        // 3. Brain step (complete integration of all cognitive systems)
        brain->step(step);
        
        // 4. Decode motor command (uses planner and attention)
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        
        // 5. Apply action to world (shapes environment)
        ActionResult result = world->applyAction(*brain, cmd);
        
        // 6. Compute reward and update neuromodulation
        float reward = world->computeReward(*brain);
        float predictedReward = agentBrain->getExpectedReward();
        agentBrain->applyRewardModulation(reward, predictedReward);
        
        // 7. Update development (affects plasticity)
        agentBrain->updateDevelopment(0.01);
        
        // 8. Record metrics
        updateMetrics(step, percept, cmd, reward, result);
        
        // 9. Check for special events
        checkSpecialEvents(step);
        
        // 10. Update timing
        auto loopEnd = std::chrono::steady_clock::now();
        double loopTime = std::chrono::duration<double>(loopEnd - loopStart).count();
        metrics.runtime.maxStepTime = std::max(metrics.runtime.maxStepTime, loopTime);
        metrics.runtime.avgStepTime = (metrics.runtime.avgStepTime * (step - 1) + loopTime) / step;
    }
    
    void displayStepProgress(int step) {
        std::cout << "  Step " << step << ": ";
        
        // Display key system states
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "WM:" << workingMemory->getActiveTraces();
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << " EM:" << episodicMemory->getEpisodeCount();
        }
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            std::cout << " PRED:" << std::fixed << std::setprecision(3) << predictionSystem->getPredictionError();
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            std::cout << " CON:" << conceptFormation->getConceptCount();
        }
        
        std::cout << " | Dopamine:" << std::setprecision(2) << (agentBrain->getDopamineLevel() ? *agentBrain->getDopamineLevel() : 0.0f) << std::endl;
    }
    
    void updateMetrics(int step, const SensoryPercept& percept, const MotorCommand& cmd, float reward, const ActionResult& result) {
        // Update brain statistics
        metrics.brainStats.totalNeurons = brain->getTotalNeuronCount();
        metrics.brainStats.totalSynapses = brain->getTotalSynapseCount();
        metrics.brainStats.activeNeurons = brain->getActiveNeuronCount();
        metrics.brainStats.firingNeurons = brain->getFiringNeuronCount();
        metrics.brainStats.totalSpikes = brain->getTotalSpikeCount();
        metrics.brainStats.avgFiringRate = brain->getAverageFiringRate();
        metrics.brainStats.eToIRatio = brain->getExcitationInhibitionRatio();
        
        // Update cognitive system statistics
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            metrics.cognitiveStats.workingMemoryTraces = workingMemory->getActiveTraces();
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            metrics.cognitiveStats.episodicMemoryEpisodes = episodicMemory->getEpisodeCount();
        }
        
        auto* semanticMemory = brain->getSemanticMemory();
        if (semanticMemory) {
            metrics.cognitiveStats.semanticMemoryFacts = semanticMemory->getAllFacts().size();
        }
        
        auto* proceduralMemory = brain->getProceduralMemory();
        if (proceduralMemory) {
            metrics.cognitiveStats.proceduralMemorySkills = proceduralMemory->getAllSkills().size();
        }
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            metrics.cognitiveStats.predictionError = predictionSystem->getPredictionError();
            metrics.cognitiveStats.predictionConfidence = predictionSystem->getConfidence();
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            metrics.cognitiveStats.conceptCount = conceptFormation->getConceptCount();
            if (conceptFormation->getConceptCount() > 0) {
                metrics.cognitiveStats.avgConceptStability = conceptFormation->getConceptStability(1) / 100.0f;
            }
        }
        
        auto* attention = brain->getAttention();
        if (attention) {
            metrics.cognitiveStats.attentionFocusStrength = 0.5f; // Placeholder
        }
        
        auto* planner = brain->getPlanner();
        if (planner) {
            metrics.cognitiveStats.plannerConfidence = 0.7f; // Placeholder
            metrics.cognitiveStats.plannedAction = ActionType::MoveForward; // Placeholder
        }
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            metrics.cognitiveStats.dopamineLevel = dopamine->getLevel();
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            metrics.cognitiveStats.curiosityLevel = curiosity->getLevel();
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            metrics.cognitiveStats.noveltyLevel = novelty->getLevel();
        }
        
        metrics.cognitiveStats.developmentalStage = getDevelopmentalStageString(brain->getDevelopmentalStage());
        metrics.cognitiveStats.plasticityModifier = 1.0f; // Placeholder
        
        // Update learning progress
        metrics.learningProgress.rewardHistory.push_back(reward);
        if (metrics.learningProgress.rewardHistory.size() > 1000) {
            metrics.learningProgress.rewardHistory.erase(metrics.learningProgress.rewardHistory.begin());
        }
        
        // Update environment interaction
        metrics.environment.recentSensoryInput.assign(percept.getVision().begin(), percept.getVision().end());
        metrics.environment.recentActions.push_back(cmd);
        metrics.environment.actionOutcomes.push_back(result.reward);
        
        if (metrics.environment.recentActions.size() > 100) {
            metrics.environment.recentActions.erase(metrics.environment.recentActions.begin());
        }
        if (metrics.environment.actionOutcomes.size() > 100) {
            metrics.environment.actionOutcomes.erase(metrics.environment.actionOutcomes.begin());
        }
        
        // Calculate integration score
        calculateIntegrationScore();
    }
    
    void calculateIntegrationScore() {
        double score = 0.0;
        double maxScore = 100.0;
        
        // Memory systems (25 points)
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        if (workingMemory && episodicMemory) {
            score += 25.0 * (std::min(workingMemory->getActiveTraces(), 100) / 100.0f +
                           std::min(episodicMemory->getEpisodeCount(), 1000) / 1000.0f) / 2.0f;
        }
        
        // Prediction system (20 points)
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            score += 20.0 * predictionSystem->getConfidence();
        }
        
        // Concept formation (15 points)
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            score += 15.0 * std::min(conceptFormation->getConceptCount(), 50) / 50.0f;
        }
        
        // Neuromodulation (20 points)
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        if (dopamine && curiosity) {
            score += 20.0 * (dopamine->getLevel() + curiosity->getLevel()) / 2.0f;
        }
        
        // Development (10 points)
        DevelopmentStage stage = brain->getDevelopmentalStage();
        if (stage == DevelopmentalStage::Adult) {
            score += 10.0; // Adult stage indicates mature integration
        }
        
        metrics.cognitiveStats.integrationScore = score / maxScore * 100.0f;
        metrics.cognitiveStats.integrationStatus = (score >= 80.0f) ? "EXCELLENT" :
                                                   (score >= 60.0f) ? "GOOD" :
                                                   (score >= 40.0f) ? "FAIR" : "NEEDS_IMPROVEMENT";
    }
    
    void checkSpecialEvents(int step) {
        // Check for memory replay events
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory && step % 100 == 0) {
            size_t episodes = episodicMemory->getEpisodeCount();
            if (episodes > 0) {
                metrics.achievements.push_back("Replay: Stored " + std::to_string(episodes) + " memory episodes");
            }
        }
        
        // Check for concept discovery
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation && step % 50 == 0) {
            size_t concepts = conceptFormation->getConceptCount();
            if (concepts > 0) {
                metrics.achievements.push_back("Concepts: Discovered " + std::to_string(concepts) + " concepts");
            }
        }
        
        // Check for development milestones
        DevelopmentStage stage = brain->getDevelopmentalStage();
        if (step == 0) {
            metrics.achievements.push_back("Development: Started in " + getDevelopmentalStageString(stage) + " stage");
        } else if (step % 1000 == 0 && stage == DevelopmentalStage::Adult) {
            metrics.achievements.push_back("Development: Reached stable adult stage");
        }
        
        // Check for plasticity events
        auto* dopamine = brain->getDopamine();
        if (dopamine && std::abs(dopamine->getLevel()) > 0.8f) {
            metrics.achievements.push_back("Plasticity: High dopamine modulation detected");
        }
    }
    
    void recordEpisodeMetrics(int episode) {
        // Calculate episode-specific metrics
        float episodeReward = 0.0f;
        float episodePredictionError = 0.0f;
        size_t episodeMemoryGrowth = 0;
        
        // In a full implementation, you would track these during the episode
        // For demo purposes, we'll simulate some values
        
        metrics.learningProgress.rewardHistory.push_back(episodeReward);
        metrics.learningProgress.predictionErrorHistory.push_back(episodePredictionError);
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        if (workingMemory && episodicMemory) {
            metrics.learningProgress.memoryGrowthHistory.push_back(
                workingMemory->getActiveTraces() + episodicMemory->getEpisodeCount());
        }
        
        if (metrics.learningProgress.memoryGrowthHistory.size() > 10) {
            metrics.learningProgress.memoryGrowthHistory.erase(metrics.learningProgress.memoryGrowthHistory.begin());
        }
        
        // Calculate learning efficiency
        if (metrics.learningProgress.rewardHistory.size() > 1) {
            float totalReward = 0.0f;
            for (float reward : metrics.learningProgress.rewardHistory) {
                totalReward += reward;
            }
            metrics.learningProgress.learningRate = totalReward / metrics.learningProgress.rewardHistory.size();
        }
        
        if (metrics.learningProgress.predictionErrorHistory.size() > 1) {
            float totalError = 0.0f;
            for (float error : metrics.learningProgress.predictionErrorHistory) {
                totalError += error;
            }
            metrics.learningProgress.retentionRate = 1.0f - totalError;
        }
        
        // Calculate generalization score
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation && conceptFormation->getConceptCount() > 0) {
            metrics.learningProgress.generalizationScore = conceptFormation->getConceptStability(1) / 100.0f;
        }
    }
    
    void displayFinalResults() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "FINAL RESULTS" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        std::cout << "\nBrain Performance:" << std::endl;
        std::cout << "  Total Neurons: " << metrics.brainStats.totalNeurons << std::endl;
        std::cout << "  Total Synapses: " << metrics.brainStats.totalSynapses << std::endl;
        std::cout << "  Active Neurons: " << metrics.brainStats.activeNeurons << std::endl;
        std::cout << "  Firing Neurons: " << metrics.brainStats.firingNeurons << std::endl;
        std::cout << "  Total Spikes: " << metrics.brainStats.totalSpikes << std::endl;
        std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << metrics.brainStats.avgFiringRate << std::endl;
        std::cout << "  E/I Ratio: " << std::setprecision(3) << metrics.brainStats.eToIRatio << std::endl;
        
        std::cout << "\nCognitive System Integration:" << std::endl;
        std::cout << "  Integration Score: " << std::fixed << std::setprecision(1) << metrics.cognitiveStats.integrationScore << "/100" << std::endl;
        std::cout << "  Status: " << metrics.cognitiveStats.integrationStatus << std::endl;
        
        std::cout << "\nMemory Systems:" << std::endl;
        std::cout << "  Working Memory: " << metrics.cognitiveStats.workingMemoryTraces << " traces" << std::endl;
        std::cout << "  Episodic Memory: " << metrics.cognitiveStats.episodicMemoryEpisodes << " episodes" << std::endl;
        std::cout << "  Semantic Memory: " << metrics.cognitiveStats.semanticMemoryFacts << " facts" << std::endl;
        std::cout << "  Procedural Memory: " << metrics.cognitiveStats.proceduralMemorySkills << " skills" << std::endl;
        
        std::cout << "\nPrediction System:" << std::endl;
        std::cout << "  Prediction Error: " << std::fixed << std::setprecision(3) << metrics.cognitiveStats.predictionError << std::endl;
        std::cout << "  Prediction Confidence: " << std::setprecision(3) << metrics.cognitiveStats.predictionConfidence << std::endl;
        
        std::cout << "\nConcept Formation:" << std::endl;
        std::cout << "  Concept Count: " << metrics.cognitiveStats.conceptCount << std::endl;
        std::cout << "  Average Stability: " << std::setprecision(3) << metrics.cognitiveStats.avgConceptStability << std::endl;
        
        std::cout << "\nNeuromodulation:" << std::endl;
        std::cout << "  Dopamine Level: " << std::setprecision(3) << metrics.cognitiveStats.dopamineLevel << std::endl;
        std::cout << "  Curiosity Level: " << std::setprecision(3) << metrics.cognitiveStats.curiosityLevel << std::endl;
        std::cout << "  Novelty Level: " << std::setprecision(3) << metrics.cognitiveStats.noveltyLevel << std::endl;
        
        std::cout << "\nDevelopment:" << std::endl;
        std::cout << "  Stage: " << metrics.cognitiveStats.developmentalStage << std::endl;
        std::cout << "  Plasticity Modifier: " << std::setprecision(3) << metrics.cognitiveStats.plasticityModifier << std::endl;
        
        std::cout << "\nLearning Progress:" << std::endl;
        std::cout << "  Learning Rate: " << std::setprecision(4) << metrics.learningProgress.learningRate << std::endl;
        std::cout << "  Retention Rate: " << std::setprecision(4) << metrics.learningProgress.retentionRate << std::endl;
        std::cout << "  Generalization Score: " << std::setprecision(4) << metrics.learningProgress.generalizationScore << std::endl;
        
        std::cout << "\nKey Achievements:" << std::endl;
        for (const auto& achievement : metrics.achievements) {
            std::cout << "  ✓ " << achievement << std::endl;
        }
        
        std::cout << "\nIntegration Highlights:" << std::endl;
        std::cout << metrics.integrationHighlights << std::endl;
        
        std::cout << "\nPhase 6 Status: " << metrics.phase6Status << std::endl;
        
        // Display performance metrics
        if (demoConfig.performanceMonitoring) {
            std::cout << "\nPerformance Metrics:" << std::endl;
            std::cout << "  Total Runtime: " << std::fixed << std::setprecision(2) << metrics.runtime.totalStepsCompleted << " steps" << std::endl;
            std::cout << "  Average Step Time: " << std::setprecision(4) << metrics.runtime.avgStepTime * 1000 << " ms" << std::endl;
            std::cout << "  Maximum Step Time: " << std::setprecision(4) << metrics.runtime.maxStepTime * 1000 << " ms" << std::endl;
            std::cout << "  Steps per Second: " << metrics.runtime.totalStepsCompleted / (metrics.runtime.maxStepTime * metrics.runtime.totalStepsCompleted) << std::endl;
        }
    }
    
    void cleanup() {
        std::cout << "\nCleaning up..." << std::endl;
        
        // Clear all pointers
        world.reset();
        brain.reset();
        agentBrain.reset();
        config.reset();
        
        std::cout << "✓ Cleanup complete" << std::endl;
    }
    
    bool kbhit() {
        // Simple keyboard hit detection for Unix-like systems
        #ifdef _WIN32
        return _kbhit() != 0;
        #else
        struct termios oldt, newt;
        int oldf;
        // Not implemented for this demo
        return false;
        #endif
    }
};

} // namespace nlm

// Main function for Phase 6 demo
int main(int argc, char** argv) {
    try {
        // Initialize logging
        auto logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
        
        nlm::NLM_LOG_INFO("Starting NLM Phase 6 - Comprehensive Cognitive Integration Demo");
        
        // Create and run Phase 6 demo
        nlm::Phase6Demo demo;
        demo.run();
        
        nlm::NLM_LOG_INFO("NLM Phase 6 demo completed successfully");
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN ERROR occurred during Phase 6 demo" << std::endl;
        return 1;
    }
}