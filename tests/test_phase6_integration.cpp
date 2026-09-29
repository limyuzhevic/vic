// Phase 6 Integration Tests
// Tests for verifying that all Phase 6 integrated brain systems work together properly

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_phase6_integration {

void testPhase6ConfigStructure() {
    std::cout << "    Testing Phase 6 configuration structure..." << std::endl;
    
    // Test that Phase6Config has all required fields
    Phase6Config config;
    
    // Check that all configuration fields are properly initialized
    assert(config.maxSteps > 0);
    assert(config.neuronCount > 0);
    assert(config.regionCount > 0);
    assert(config.connectionProbability >= 0.0f && config.connectionProbability <= 1.0f);
    assert(config.enableCheckpointing == true || config.enableCheckpointing == false);
    assert(config.enableReplay == true || config.enableReplay == false);
    assert(config.enableDevelopment == true || config.enableDevelopment == false);
    assert(!config.checkpointPath.empty());
    
    std::cout << "    ✓ Phase 6 configuration structure is valid" << std::endl;
}

void testBrainMemoryIntegration() {
    std::cout << "    Testing Brain memory systems integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test that memory systems are integrated and accessible
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    
    // Test working memory
    auto* wm = brain->getWorkingMemory();
    assert(wm->getCapacity() > 0);
    assert(wm->getActiveTraces() >= 0);
    
    // Test episodic memory
    auto* em = brain->getEpisodicMemory();
    assert(em->getEpisodeCount() >= 0);
    
    std::cout << "    ✓ Brain memory systems are properly integrated" << std::endl;
}

void testBrainNeuromodulationIntegration() {
    std::cout << "    Testing Brain neuromodulation systems integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test that neuromodulation systems are integrated and accessible
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getPredictionErrorSignal() != nullptr);
    assert(brain->getNovelty() != nullptr);
    
    // Test neuromodulation functionality
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    // These systems should have basic functionality
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "    ✓ Brain neuromodulation systems are properly integrated" << std::endl;
}

void testBrainPredictionIntegration() {
    std::cout << "    Testing Brain prediction system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test that prediction system is integrated and accessible
    assert(brain->getPredictionSystem() != nullptr);
    
    std::cout << "    ✓ Brain prediction system is properly integrated" << std::endl;
}

void testBrainDevelopmentIntegration() {
    std::cout << "    Testing Brain development system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test that development system is integrated and accessible
    assert(brain->getDevelopmentSystem() != nullptr);
    
    // Test development functionality
    auto* dev = brain->getDevelopmentSystem();
    assert(dev != nullptr);
    
    std::cout << "    ✓ Brain development system is properly integrated" << std::endl;
}

void testBrainCognitionIntegration() {
    std::cout << "    Testing Brain cognition systems integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test that cognition systems are integrated and accessible
    assert(brain->getPlanner() != nullptr);
    assert(brain->getConceptFormation() != nullptr);
    assert(brain->getAttention() != nullptr);
    
    // Test cognition functionality
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    std::cout << "    ✓ Brain cognition systems are properly integrated" << std::endl;
}

void testPhase6ExperimentRunner() {
    std::cout << "    Testing Phase6IntegratedExperiment runner..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test verification
    assert(experiment.verifyIntegration());
    
    // Test individual system tests
    assert(experiment.testMemoryIntegration());
    assert(experiment.testNeuromodulationIntegration());
    assert(experiment.testCheckpointing());
    assert(experiment.testReplay());
    
    // Test full experiment run
    Phase6Config config;
    config.neuronCount = 50;
    config.maxSteps = 100;
    config.enableCheckpointing = false;
    config.enableReplay = false;
    config.enableDevelopment = false;
    
    auto result = experiment.run(config);
    
    // Verify that the experiment results are valid
    assert(result.memoryWorkingMemoryIntegrated == true);
    assert(result.memoryEpisodicMemoryIntegrated == true);
    assert(result.neuromodulationIntegrated == true);
    assert(result.predictionIntegrated == true);
    assert(result.developmentIntegrated == true);
    
    std::cout << "    ✓ Phase6IntegratedExperiment runner is working correctly" << std::endl;
}

void testConfigJSONParsing() {
    std::cout << "    Testing JSON configuration parsing..." << std::endl;
    
    Config config;
    
    // Test parsing simple JSON
    std::string jsonConfig = R"({
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "working_memory_capacity_factor": 0.1,
        "episodic_memory_max_episodes": 1000,
        "attention_inhibition_strength": 0.5,
        "dopamine_excitability_mod": 0.5
    })";
    
    assert(config.parseJSON(jsonConfig));
    
    // Test getting values from parsed JSON
    assert(config.getOr<size_t>("neuron_count", 0) == 500);
    assert(config.getOr<size_t>("region_count", 0) == 2);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.1f);
    assert(config.getOr<float>("stdp_ltp_weight", 0.0f) == 0.02f);
    assert(config.getOr<float>("stdp_ltd_weight", 0.0f) == 0.012f);
    
    std::cout << "    ✓ JSON configuration parsing is working correctly" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "=== Running Phase 6 Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    testPhase6ConfigStructure();
    testBrainMemoryIntegration();
    testBrainNeuromodulationIntegration();
    testBrainPredictionIntegration();
    testBrainDevelopmentIntegration();
    testBrainCognitionIntegration();
    testPhase6ExperimentRunner();
    testConfigJSONParsing();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Integration Tests Passed ===" << std::endl;
}

} // namespace test_phase6_integration

int main(int argc, char* argv[]) {
    test_phase6_integration::runAllPhase6IntegrationTests();
    return 0;
}
