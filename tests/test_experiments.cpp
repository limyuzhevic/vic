// Phase 3, 4, 6 Experiment Tests
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_experiments {

void testPhase3ExperimentBasic() {
    // Test basic Phase 3 experiment functionality
    nlm::Phase3Experiment experiment;
    
    // Create a simple brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    // Run experiment with simple brain
    experiment.run(config);
    
    std::cout << "    testPhase3ExperimentBasic passed" << std::endl;
}

void testPhase4TemporalPrediction() {
    nlm::TemporalPredictionExperiment tempPred;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Run temporal prediction experiment
    nlm::Phase4Results results = tempPred.run(&brain, 10);
    
    // Should have valid results
    assert(results.predictionErrorInitial >= 0.0f);
    assert(results.predictionErrorFinal >= 0.0f);
    
    std::cout << "    testPhase4TemporalPrediction passed" << std::endl;
}

void testPhase4WorkingMemoryExperiment() {
    nlm::WorkingMemoryExperiment wmExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = wmExp.run(&brain, 5);
    
    // Should measure working memory retention
    assert(results.workingMemoryCapacity >= 0.0f);
    
    std::cout << "    testPhase4WorkingMemoryExperiment passed" << std::endl;
}

void testPhase4EpisodicMemoryExperiment() {
    nlm::EpisodicRecallExperiment epExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = epExp.run(&brain, 5);
    
    // Should measure episodic recall accuracy
    assert(results.episodicRecallAccuracy >= 0.0f);
    
    std::cout << "    testPhase4EpisodicMemoryExperiment passed" << std::endl;
}

void testPhase4ConceptFormationExperiment() {
    nlm::ConceptFormationExperiment cfExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = cfExp.run(&brain, 10);
    
    // Should measure concept formation
    assert(results.conceptsFormed >= 0);
    assert(results.conceptStability >= 0.0f);
    
    std::cout << "    testPhase4ConceptFormationExperiment passed" << std::endl;
}

void testPhase4AttentionExperiment() {
    nlm::AttentionExperiment attnExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = attnExp.run(&brain, 5);
    
    // Should measure attention selectivity
    assert(results.attentionSelectivity >= 0.0f);
    
    std::cout << "    testPhase4AttentionExperiment passed" << std::endl;
}

void testPhase4PlanningExperiment() {
    nlm::PlanningExperiment planExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = planExp.run(&brain, 5);
    
    // Should measure planning accuracy
    assert(results.planningAccuracy >= 0.0f);
    
    std::cout << "    testPhase4PlanningExperiment passed" << std::endl;
}

void testPhase4SelfModelExperiment() {
    nlm::SelfModelExperiment selfExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = selfExp.run(&brain, 5);
    
    // Should measure self-model accuracy
    assert(results.selfPredictionAccuracy >= 0.0f);
    
    std::cout << "    testPhase4SelfModelExperiment passed" << std::endl;
}

void testPhase4ObjectPermanenceExperiment() {
    nlm::ObjectPermanenceExperiment opExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = opExp.run(&brain, 3);
    
    // Should measure behavior related to object permanence
    assert(results.totalReward >= 0.0f);
    
    std::cout << "    testPhase4ObjectPermanenceExperiment passed" << std::endl;
}

void testPhase4SocialLearningExperiment() {
    nlm::SocialLearningExperiment slExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = slExp.run(&brain, 3);
    
    // Should measure social learning accuracy
    assert(results.imitationAccuracy >= 0.0f);
    
    std::cout << "    testPhase4SocialLearningExperiment passed" << std::endl;
}

void testPhase4ContinualLearningExperiment() {
    nlm::ContinualLearningExperiment clExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = clExp.run(&brain, 10);
    
    // Should measure learning over multiple tasks
    assert(results.transferPerformance >= 0.0f);
    
    std::cout << "    testPhase4ContinualLearningExperiment passed" << std::endl;
}

void testPhase4GeneralizationExperiment() {
    nlm::GeneralizationExperiment genExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = genExp.run(&brain, 5);
    
    // Should measure transfer ability
    float transferRatio = genExp.getTransferRatio();
    assert(transferRatio >= 0.0f && transferRatio <= 1.0f);
    
    std::cout << "    testPhase4GeneralizationExperiment passed" << std::endl;
}

void testPhase4AblationExperiment() {
    nlm::AblationExperiment ablationExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::vector<nlm::AblationExperiment::AblationResult> results = ablationExp.run(&brain, 3);
    
    // Should test multiple mechanisms
    assert(!results.empty());
    
    std::cout << "    testPhase4AblationExperiment passed" << std::endl;
}

void testPhase4IntegratedExperiment() {
    nlm::Phase4IntegratedExperiment integratedExp;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::Phase4Results results = integratedExp.run(&brain, 5, 50);
    
    // Should have comprehensive results
    assert(results.totalReward >= 0.0f);
    assert(results.conceptsFormed >= 0);
    
    std::cout << "    testPhase4IntegratedExperiment passed" << std::endl;
}

void testPhase6Integration() {
    nlm::Phase6Config config;
    config.maxSteps = 100;
    config.neuronCount = 50;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = false;
    config.enableReplay = false;
    config.enableDevelopment = false;
    
    nlm::Phase6IntegratedExperiment phase6Exp;
    nlm::Phase6IntegrationResult result = phase6Exp.run(config);
    
    // Should report integration status
    assert(result.totalReward >= 0.0f);
    
    std::cout << "    testPhase6Integration passed" << std::endl;
}

void testPhase6MemoryIntegration() {
    nlm::Phase6IntegratedExperiment phase6Exp;
    
    nlm::Phase6Config config;
    config.maxSteps = 50;
    config.enableDevelopment = false;
    
    nlm::Phase6IntegrationResult result = phase6Exp.testMemoryIntegration();
    
    assert(result.memoryWorkingMemoryIntegrated == true);
    assert(result.memoryEpisodicMemoryIntegrated == true);
    
    std::cout << "    testPhase6MemoryIntegration passed" << std::endl;
}

void testPhase6NeuromodulationIntegration() {
    nlm::Phase6IntegratedExperiment phase6Exp;
    
    nlm::Phase6Config config;
    config.maxSteps = 50;
    config.enableDevelopment = false;
    
    nlm::Phase6IntegrationResult result = phase6Exp.testNeuromodulationIntegration();
    
    assert(result.neuromodulationIntegrated == true);
    
    std::cout << "    testPhase6NeuromodulationIntegration passed" << std::endl;
}

void testPhase6Checkpointing() {
    nlm::Phase6IntegratedExperiment phase6Exp;
    
    nlm::Phase6Config config;
    config.maxSteps = 20;
    config.enableCheckpointing = true;
    config.checkpointPath = "./test_checkpoint.bin";
    config.enableDevelopment = false;
    
    nlm::Phase6IntegrationResult result = phase6Exp.testCheckpointing();
    
    assert(result.checkpointingWorks == true);
    
    std::cout << "    testPhase6Checkpointing passed" << std::endl;
}

void testPhase6Replay() {
    nlm::Phase6IntegratedExperiment phase6Exp;
    
    nlm::Phase6Config config;
    config.maxSteps = 30;
    config.enableReplay = true;
    config.enableDevelopment = false;
    
    nlm::Phase6IntegrationResult result = phase6Exp.testReplay();
    
    assert(result.replayWorks == true);
    
    std::cout << "    testPhase6Replay passed" << std::endl;
}

void testExperimentMetrics() {
    nlm::ExperimentMetrics metrics;
    
    // Should be initial values
    assert(metrics.episode == 0);
    assert(metrics.steps == 0);
    assert(metrics.totalReward == 0.0f);
    assert(metrics.predictionError == 0.0f);
    
    metrics.reset();
    assert(metrics.episode == 0);
    assert(metrics.steps == 0);
    
    std::cout << "    testExperimentMetrics passed" << std::endl;
}

void testExperimentLogger() {
    nlm::ExperimentLogger logger("test_experiment.log");
    
    logger.log("Test message");
    logger.logEpisodeStart(1);
    logger.logEpisodeEnd(1, 10.0f, 5, 0.5f);
    logger.flush();
    
    // Should not crash
    std::cout << "    testExperimentLogger passed" << std::endl;
}

void runAll() {
    std::cout << "Running Phase 3, 4, 6 experiment tests..." << std::endl;
    
    testPhase3ExperimentBasic();
    
    testPhase4TemporalPrediction();
    testPhase4WorkingMemoryExperiment();
    testPhase4EpisodicMemoryExperiment();
    testPhase4ConceptFormationExperiment();
    testPhase4AttentionExperiment();
    testPhase4PlanningExperiment();
    testPhase4SelfModelExperiment();
    testPhase4ObjectPermanenceExperiment();
    testPhase4SocialLearningExperiment();
    testPhase4ContinualLearningExperiment();
    testPhase4GeneralizationExperiment();
    testPhase4AblationExperiment();
    testPhase4IntegratedExperiment();
    
    testPhase6Integration();
    testPhase6MemoryIntegration();
    testPhase6NeuromodulationIntegration();
    testPhase6Checkpointing();
    testPhase6Replay();
    
    testExperimentMetrics();
    testExperimentLogger();
}

} // namespace test_experiments
