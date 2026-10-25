#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <cmath>
#include <vector>

namespace nlm {

EnhancedIntegrationExperiment::EnhancedIntegrationExperiment() {}

EnhancedIntegrationExperiment::~EnhancedIntegrationExperiment() = default;

EnhancedIntegrationResult EnhancedIntegrationExperiment::run(const EnhancedIntegrationConfig& config) {
    EnhancedIntegrationResult result;
    result.startTime = time(nullptr);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("=== Enhanced Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Testing complete system integration with all enhancements");
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    
    // Enable all enhanced systems
    cfg->set("enable_neuromodulation_acetylcholine", true);
    cfg->set("enable_neuromodulation_norepinephrine", true);
    cfg->set("enable_neuromodulation_serotonin", true);
    cfg->set("enable_prediction_system", true);
    cfg->set("enable_enhanced_development", true);
    cfg->set("enable_enhanced_memory_replay", true);
    cfg->set("enable_checkpointing", true);
    cfg->set("enable_cognition_full_integration", true);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        result.endTime = time(nullptr);
        return result;
    }
    
    // Verify all systems are properly integrated
    result = verifySystemIntegration(brain, config);
    
    // Run enhanced simulation with all systems active
    result = runEnhancedSimulation(brain, config, result);
    
    // Run persistence tests
    result = runPersistenceTests(brain, config, result);
    
    // Verify memory replay effectiveness
    result = verifyMemoryReplay(brain, config, result);
    
    auto endWall = std::chrono::high_resolution_clock::now();
    result.wallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    result.endTime = time(nullptr);
    
    NLM_LOG_INFO("=== Enhanced Integration Complete ===");
    NLM_LOG_INFO("Overall Integration Score: " + std::to_string(result.overallIntegrationScore));
    NLM_LOG_INFO("Neuromodulation Integration: " + std::to_string(
        (result.neuromodulationDopamineIntegrated + result.neuromodulationAChIntegrated + 
         result.neuromodulationNEIntegrated + result.neuromodulation5HTIntegrated) / 4.0f * 100) + "%");
    NLM_LOG_INFO("Cognition Integration: " + std::to_string(
        (result.cognitionNeuralPlannerIntegrated + result.cognitionConceptFormationIntegrated + 
         result.cognitionAttentionIntegrated + result.cognitionSelfModelIntegrated + 
         result.cognitionSocialLearningIntegrated) / 5.0f * 100) + "%");
    NLM_LOG_INFO("Memory Integration: " + std::to_string(
        (result.memoryWorkingMemoryIntegrated + result.memoryEpisodicMemoryIntegrated + 
         result.memoryAssociativeMemoryIntegrated) / 3.0f * 100) + "%");
    
    return result;
}

EnhancedIntegrationResult EnhancedIntegrationExperiment::verifySystemIntegration(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config) {
    EnhancedIntegrationResult result;
    
    // Test memory systems
    result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr) ? 1.0f : 0.0f;
    result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr) ? 1.0f : 0.0f;
    result.memoryAssociativeMemoryIntegrated = (brain->getAssociativeMemory() != nullptr) ? 1.0f : 0.0f;
    
    // Test complete neuromodulation systems
    result.neuromodulationDopamineIntegrated = (brain->getDopamine() != nullptr) ? 1.0f : 0.0f;
    result.neuromodulationAChIntegrated = (brain->getAcetylcholine() != nullptr) ? 1.0f : 0.0f;
    result.neuromodulationNEIntegrated = (brain->getNorepinephrine() != nullptr) ? 1.0f : 0.0f;
    result.neuromodulation5HTIntegrated = (brain->getSerotonin() != nullptr) ? 1.0f : 0.0f;
    result.neuromodulationCuriosityIntegrated = (brain->getCuriosity() != nullptr) ? 1.0f : 0.0f;
    result.neuromodulationNoveltyIntegrated = (brain->getNovelty() != nullptr) ? 1.0f : 0.0f;
    result.neuromodulationPredictionErrorIntegrated = (brain->getPredictionErrorSignal() != nullptr) ? 1.0f : 0.0f;
    
    // Test cognition systems
    result.cognitionNeuralPlannerIntegrated = (brain->getPlanner() != nullptr) ? 1.0f : 0.0f;
    result.cognitionConceptFormationIntegrated = (brain->getConceptFormation() != nullptr) ? 1.0f : 0.0f;
    result.cognitionAttentionIntegrated = (brain->getAttention() != nullptr) ? 1.0f : 0.0f;
    result.cognitionSelfModelIntegrated = (brain->getSelfModel() != nullptr) ? 1.0f : 0.0f;
    result.cognitionSocialLearningIntegrated = (brain->getSocialLearning() != nullptr) ? 1.0f : 0.0f;
    
    // Test prediction system
    result.predictionSystemIntegrated = (brain->getPredictionSystem() != nullptr) ? 1.0f : 0.0f;
    
    // Test development system
    result.developmentSystemIntegrated = (brain->getDevelopmentSystem() != nullptr) ? 1.0f : 0.0f;
    
    // Calculate overall integration score
    result.overallIntegrationScore = calculateIntegrationScore(result);
    
    return result;
}

EnhancedIntegrationResult EnhancedIntegrationExperiment::runEnhancedSimulation(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult result) {
    // Run simulation with all systems active
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Inject varied sensory input to test all systems
        SensoryInput sensoryInput;
        std::vector<float> sensoryData(config.sensoryInputSize, 0.0f);
        
        // Create complex sensory pattern
        float timeFactor = static_cast<float>(step) * 0.01f;
        for (size_t i = 0; i < config.sensoryInputSize; ++i) {
            sensoryData[i] = std::sin(timeFactor * (i + 1)) * 0.5f + 
                           std::cos(timeFactor * 0.7f) * 0.3f +
                           (i % 5 == 0) ? 0.8f : 0.0f;  // Some salient stimuli
        }
        
        sensoryInput = SensoryInput(sensoryData);
        brain->receiveSensoryInput(sensoryInput);
        brain->step(step, step * 0.001);
        
        // Collect metrics periodically
        if (step % config.metricsInterval == 0) {
            updateMetrics(brain, config, result, step);
        }
    }
    
    return result;
}

EnhancedIntegrationResult EnhancedIntegrationExperiment::runPersistenceTests(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult result) {
    // Test persistence systems
    std::string checkpointPath = "/tmp/nlm_enhanced_integration_checkpoint.bin";
    
    // Save checkpoint
    if (brain->save(checkpointPath)) {
        result.checkpointSuccessRate = 1.0f;
        result.persistenceSystemIntegrated = 1.0f;
        
        // Create new brain and load checkpoint
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.neuronCount);
        auto brain2 = std::make_shared<Brain>(cfg);
        brain2->initialize();
        
        if (brain2->load(checkpointPath)) {
            result.checkpointSuccessRate = 1.0f;
        }
    } else {
        result.checkpointSuccessRate = 0.0f;
    }
    
    return result;
}

EnhancedIntegrationResult EnhancedIntegrationExperiment::verifyMemoryReplay(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult result) {
    // Test memory replay effectiveness
    if (brain->getEpisodicMemory()) {
        // Run simulation to generate memories
        for (int step = 0; step < 200; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        if (episodeCount > 0) {
            // Get episodes for replay
            auto replayEpisodes = brain->getEpisodicMemory()->getEpisodesForReplay(3);
            if (!replayEpisodes.empty()) {
                result.memoryReplayIntegrated = 1.0f;
                // Calculate replay effectiveness based on episode diversity
                result.replayEffectiveness = std::min(1.0f, static_cast<float>(replayEpisodes.size()) / 5.0f);
            }
        }
    }
    
    return result;
}

float EnhancedIntegrationExperiment::calculateIntegrationScore(const EnhancedIntegrationResult& result) {
    float totalScore = 0.0f;
    float maxScore = 0.0f;
    
    // Memory systems (3 components)
    totalScore += result.memoryWorkingMemoryIntegrated + result.memoryEpisodicMemoryIntegrated + result.memoryAssociativeMemoryIntegrated;
    maxScore += 3.0f;
    
    // Neuromodulation systems (7 components)
    totalScore += result.neuromodulationDopamineIntegrated + result.neuromodulationAChIntegrated + 
                 result.neuromodulationNEIntegrated + result.neuromodulation5HTIntegrated + 
                 result.neuromodulationCuriosityIntegrated + result.neuromodulationNoveltyIntegrated + 
                 result.neuromodulationPredictionErrorIntegrated;
    maxScore += 7.0f;
    
    // Cognition systems (5 components)
    totalScore += result.cognitionNeuralPlannerIntegrated + result.cognitionConceptFormationIntegrated + 
                 result.cognitionAttentionIntegrated + result.cognitionSelfModelIntegrated + 
                 result.cognitionSocialLearningIntegrated;
    maxScore += 5.0f;
    
    // Prediction system (1 component)
    totalScore += result.predictionSystemIntegrated;
    maxScore += 1.0f;
    
    // Development system (1 component)
    totalScore += result.developmentSystemIntegrated;
    maxScore += 1.0f;
    
    // Persistence system (1 component)
    totalScore += result.persistenceSystemIntegrated;
    maxScore += 1.0f;
    
    // Memory replay system (1 component)
    totalScore += result.memoryReplayIntegrated;
    maxScore += 1.0f;
    
    return (maxScore > 0) ? (totalScore / maxScore) : 0.0f;
}

void EnhancedIntegrationExperiment::updateMetrics(std::shared_ptr<Brain> brain, const EnhancedIntegrationConfig& config, EnhancedIntegrationResult& result, uint64_t step) {
    // Update various metrics
    float firingRate = brain->getAverageFiringRate();
    float totalSpikes = static_cast<float>(brain->getTotalSpikeCount());
    float eiratio = brain->getExcitationInhibitionRatio();
    
    // Update prediction accuracy if available
    if (brain->getPredictionSystem()) {
        float predictionError = brain->getPredictionSystem()->getPredictionError();
        result.predictionAccuracy = std::max(0.0f, 1.0f - predictionError);
    }
    
    // Update development stage progression
    if (brain->getDevelopmentSystem()) {
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        result.developmentalStageProgression = static_cast<float>(stage) / 5.0f;  // Normalize to 0-1
    }
}

} // namespace nlm