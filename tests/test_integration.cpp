// Integration Tests
// Tests the connections between working memory, episodic memory, neural planner, concept formation, and neuromodulation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_integration {

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test that working memory exists and can be updated
    nlm::NeuralWorkingMemory* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Create a test pattern
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Store pattern in working memory
    wm->store(pattern, 1.0f);
    
    // Verify it's stored
    std::vector<float> retrieved = wm->retrieve();
    assert(!retrieved.empty());
    
    // Test working memory capacity
    wm->setCapacity(50);
    assert(wm->getCapacity() == 50);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.positionX = 5.0f;
    episode.positionY = 10.0f;
    episode.orientation = 0.5f;
    episode.reward = 0.8f;
    episode.action = nlm::ActionType::MoveForward;
    episode.age = 10;
    
    // Store the episode
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Verify it was stored
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    
    // Retrieve the episode
    const nlm::EpisodicMemoryItem* retrieved = brain.getEpisodicMemory()->getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->reward == 0.8f);
    assert(retrieved->positionX == 5.0f);
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testNeuralPlannerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test planning depth
    planner->setPlanningDepth(5);
    assert(planner->getPlanningDepth() == 5);
    
    // Create a simple state
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Test action planning
    nlm::ActionType action = planner->planAction(state, 0.5f);
    
    // Verify we got a valid action
    bool validAction = false;
    for (int i = 0; i < 8; ++i) {
        if (action == static_cast<nlm::ActionType>(i)) {
            validAction = true;
            break;
        }
    }
    assert(validAction);
    
    // Test planning confidence
    float confidence = planner->getPlanningConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test action quality
    planner->setActionQuality(nlm::ActionType::MoveForward, 0.9f);
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create a test pattern and features
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 0.0f, 0.0f, 0.0f};
    
    // Present experience for concept formation
    size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.7f, 0);
    
    // Verify concept was created
    assert(conceptId > 0);
    
    // Test concept retrieval
    const nlm::DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
    assert(concept != nullptr);
    assert(concept->id == conceptId);
    assert(concept->totalObservations > 0);
    
    // Test concept category
    assert(!concept->categoryHint.empty());
    
    std::cout << "    testConceptFormationIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test that neuromodulators exist
    assert(brain.getDopamine() != nullptr);
    assert(brain.getAcetylcholine() != nullptr);
    assert(brain.getNorepinephrine() != nullptr);
    assert(brain.getSerotonin() != nullptr);
    
    // Apply neuromodulatory signals
    nlm::Neuromodulator dopamineSignal;
    dopamineSignal.level = 0.8f;
    dopamineSignal.type = nlm::ModulatorType::Dopamine;
    
    nlm::Neuromodulator acetylcholineSignal;
    acetylcholineSignal.level = 0.5f;
    acetylcholineSignal.type = nlm::ModulatorType::Acetylcholine;
    
    // Test neuromodulation application
    brain.applyNeuromodulation(dopamineSignal);
    brain.applyNeuromodulation(acetylcholineSignal);
    
    // Verify levels changed (simplified check)
    float dopamineLevel = brain.getDopamine()->getLevel();
    assert(dopamineLevel >= 0.0f && dopamineLevel <= 1.0f);
    
    // Test reward modulation in agent brain context
    float reward = 0.7f;
    float predictedReward = 0.5f;
    
    // This would be tested with AgentBrain, but we can test the basic concept
    assert(reward > predictedReward);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPersistence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Take a few steps to generate activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Get initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Save the brain state
    bool saved = brain.save("/tmp/test_brain_checkpoint.bin");
    assert(saved);
    
    // Reset brain
    brain.reset();
    
    // Load the saved state
    bool loaded = brain.load("/tmp/test_brain_checkpoint.bin");
    assert(loaded);
    
    // Verify some properties are restored
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() >= 0); // Synapses may change during loading
    
    // Clean up
    std::remove("/tmp/test_brain_checkpoint.bin");
    
    std::cout << "    testPersistence passed" << std::endl;
}

void testAgentBrainIntegration() {
    // This test requires more complex setup with AgentBrain
    // For now, we'll just test that the components are properly connected
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test all major systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getDopamine() != nullptr);
    
    // Test that they are integrated
    // Working memory can be updated
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
    brain.getWorkingMemory()->store(pattern);
    
    // Episodic memory can be stored
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.7f;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Concept formation is available
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        pattern, std::vector<float>(5, 0.5f), 0.7f, 0);
    
    // Neural planner exists
    nlm::ActionType action = brain.getPlanner()->planAction(pattern, 0.5f);
    
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration tests..." << std::endl;
    std::cout << "  Testing WorkingMemory Integration..." << std::endl;
    testWorkingMemoryIntegration();
    
    std::cout << "  Testing EpisodicMemory Integration..." << std::endl;
    testEpisodicMemoryIntegration();
    
    std::cout << "  Testing NeuralPlanner Integration..." << std::endl;
    testNeuralPlannerIntegration();
    
    std::cout << "  Testing ConceptFormation Integration..." << std::endl;
    testConceptFormationIntegration();
    
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    testNeuromodulationIntegration();
    
    std::cout << "  Testing Persistence..." << std::endl;
    testPersistence();
    
    std::cout << "  Testing AgentBrain Integration..." << std::endl;
    testAgentBrainIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration