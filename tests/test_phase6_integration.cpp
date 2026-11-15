// Phase 6 Integration Tests
// Tests all integrated systems from Phase 6
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_integration {

void testPhase6Configuration() {
    std::cout << "=== Testing Phase 6 Configuration ===" << std::endl;
    
    nlm::Config config;
    
    // Test that all default Phase 6 configuration options are present
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    
    // Test Phase 6 specific configuration options
    assert(config.has("plasticity_learning_rate"));
    assert(config.has("stdp_ltp_weight"));
    assert(config.has("stdp_ltd_weight"));
    assert(config.has("stdp_time_constant"));
    
    // Test neuromodulation options
    assert(config.has("dopamine_baseline"));
    assert(config.has("reward_discount_factor"));
    
    // Test memory system options
    assert(config.has("log_level"));
    assert(config.has("log_to_file"));
    
    std::cout << "    Phase 6 configuration test passed" << std::endl;
}

void testBrainInitializationWithPhase6() {
    std::cout << "=== Testing Brain Initialization with Phase 6 Systems ===" << std::endl;
    
    nlm::Config config;
    
    // Set Phase 6 specific values
    config.set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    
    // Set Phase 6 system parameters
    config.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Runtime);
    config.set("dopamine_baseline", 0.1, nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    assert(brain->initialize());
    
    // Verify Phase 6 systems are initialized
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
    assert(brain->getPlanner() != nullptr);
    assert(brain->getConceptFormation() != nullptr);
    assert(brain->getAttention() != nullptr);
    assert(brain->getDevelopmentSystem() != nullptr);
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getPredictionErrorSignal() != nullptr);
    
    std::cout << "    Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "    Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "    Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "    Working memory capacity: " << brain->getWorkingMemory()->getCapacity() << std::endl;
    
    std::cout << "    Brain initialization with Phase 6 systems test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "=== Testing Neuromodulation Integration ===" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test dopamine integration
    auto dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test curiosity integration
    auto curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test novelty integration
    auto novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    // Test prediction error integration
    auto predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "=== Testing Memory System Integration ===" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test working memory
    auto workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    workingMemory->setCapacity(50);
    assert(workingMemory->getCapacity() == 50);
    
    // Test episodic memory
    auto episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    episodicMemory->setMaxEpisodes(500);
    assert(episodicMemory->getMaxEpisodes() == 500);
    
    // Test associative memory
    auto associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::cout << "    Memory system integration test passed" << std::endl;
}

void testPredictionAndPlanningIntegration() {
    std::cout << "=== Testing Prediction and Planning Integration ===" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test prediction system
    auto predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test planning system
    auto planner = brain->getPlanner();
    assert(planner != nullptr);
    
    planner->setPlanningDepth(3);
    assert(planner->getPlanningDepth() == 3);
    
    // Test concept formation
    auto conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    std::cout << "    Prediction and planning integration test passed" << std::endl;
}

void testDevelopmentAndMaturation() {
    std::cout << "=== Testing Development and Maturation ===" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::MidDevelopment);
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::MidDevelopment);
    
    // Test development system
    auto developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    std::cout << "    Development and maturation test passed" << std::endl;
}

void testCheckpointAndPersistence() {
    std::cout << "=== Testing Checkpoint and Persistence ===" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Runtime);
    config.set("checkpoint_dir", std::string("./test_checkpoints"), nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Run a few steps to generate some state
    for (int i = 0; i < 10; ++i) {
        brain->step(i, static_cast<double>(i) * config.getOr<double>("simulation_timestep", 0.001));
    }
    
    // Test checkpointing
    assert(brain->save("./test_checkpoint.dat"));
    
    // Create a new brain for loading test
    auto brain2 = std::make_shared<nlm::Brain>(config);
    assert(brain2->initialize());
    
    // Load checkpoint
    assert(brain2->load("./test_checkpoint.dat"));
    
    // Verify key properties match
    assert(brain2->getTotalNeuronCount() == brain->getTotalNeuronCount());
    
    std::cout << "    Checkpoint and persistence test passed" << std::endl;
}

void runAllPhase6Tests() {
    std::cout << "Running Phase 6 Integration Tests" << std::endl;
    std::cout << "==============================" << std::endl;
    
    testPhase6Configuration();
    testBrainInitializationWithPhase6();
    testNeuromodulationIntegration();
    testMemorySystemIntegration();
    testPredictionAndPlanningIntegration();
    testDevelopmentAndMaturation();
    testCheckpointAndPersistence();
    
    std::cout << "==============================" << std::endl;
    std::cout << "All Phase 6 integration tests passed!" << std::endl;
}

int main() {
    runAllPhase6Tests();
    return 0;
}

} // namespace test_phase6_integration
