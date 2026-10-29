// Memory System Tests
// Phase 2.5: Integrated Neural Computation Tests

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/ProceduralMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testWorkingMemory() {
    nlm::WorkingMemory memory;
    
    // Test initial state
    assert(memory.getCapacity() > 0);
    assert(memory.getCurrentSize() == 0);
    assert(!memory.contains(nlm::NeuronId(1)));
    
    // Test storage and retrieval
    nlm::NeuronId neuron(1);
    float value = 0.5f;
    memory.store(neuron, value);
    
    assert(memory.contains(neuron));
    assert(memory.getNeuronActivation(neuron) == value);
    
    // Test decay
    float originalValue = memory.getNeuronActivation(neuron);
    memory.decay(0.1f);
    assert(memory.getNeuronActivation(neuron) < originalValue);
    
    // Test clear
    memory.clear();
    assert(!memory.contains(neuron));
    assert(memory.getCurrentSize() == 0);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    nlm::EpisodicMemory memory;
    
    // Test initial state
    assert(memory.getEpisodeCount() == 0);
    assert(memory.getRecentEpisodes(5).empty());
    
    // Test storage
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.5f, 0.7f, 0.3f};
    episode.positionX = 5.0f;
    episode.positionY = 3.0f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 1.0f;
    episode.novelty = 0.8f;
    
    memory.storeEpisode(episode);
    assert(memory.getEpisodeCount() == 1);
    
    // Test retrieval
    auto episodes = memory.getRecentEpisodes(10);
    assert(episodes.size() == 1);
    assert(episodes[0]->reward == 1.0f);
    
    // Test temporal retrieval
    auto temporalEpisodes = memory.retrieveTemporal(0, 10, 5);
    assert(temporalEpisodes.size() == 1);
    
    // Test location retrieval
    auto locationEpisodes = memory.retrieveByLocation(5.0f, 3.0f, 1.0f, 5);
    assert(locationEpisodes.size() == 1);
    
    // Test action retrieval
    auto actionEpisodes = memory.retrieveAfterAction(nlm::ActionType::MoveForward, 5);
    assert(actionEpisodes.size() == 1);
    
    // Test replay
    memory.replayEpisode(episodes[0]);
    
    // Test consolidation
    memory.consolidate(0.5f);
    
    // Test clear
    memory.clear();
    assert(memory.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testSemanticMemory() {
    nlm::SemanticMemory memory;
    
    // Test initial state
    assert(!memory.hasFact("color"));
    assert(memory.retrieveFact("color") == "");
    
    // Test storage
    memory.storeFact("color", "red");
    assert(memory.hasFact("color"));
    assert(memory.retrieveFact("color") == "red");
    
    // Test overwriting
    memory.storeFact("color", "blue");
    assert(memory.retrieveFact("color") == "blue");
    
    // Test multiple facts
    memory.storeFact("shape", "circle");
    memory.storeFact("size", "large");
    
    auto allFacts = memory.getAllFacts();
    assert(allFacts.size() == 3);
    
    // Test clear
    memory.clear();
    assert(memory.getAllFacts().empty());
    assert(!memory.hasFact("color"));
    assert(!memory.hasFact("shape"));
    
    std::cout << "    testSemanticMemory passed" << std::endl;
}

void testProceduralMemory() {
    nlm::ProceduralMemory memory;
    
    // Test initial state
    assert(memory.getAllSkills().empty());
    
    // Test skill learning
    std::vector<nlm::NeuronId> pattern1;
    pattern1.push_back(nlm::NeuronId(1));
    pattern1.push_back(nlm::NeuronId(2));
    
    memory.learnSkill("move_forward", pattern1);
    
    auto skills = memory.getAllSkills();
    assert(skills.size() == 1);
    assert(skills[0]->name == "move_forward");
    assert(skills[0]->proficiency == 0.0f);
    
    // Test proficiency update
    memory.updateProficiency("move_forward", 0.5f);
    assert(skills[0]->proficiency == 0.5f);
    
    memory.updateProficiency("move_forward", 0.8f);
    assert(skills[0]->proficiency == 1.0f);  // Should be capped at 1.0
    
    // Test get skill
    nlm::Skill* skill = memory.getSkill("move_forward");
    assert(skill != nullptr);
    assert(skill->name == "move_forward");
    
    nlm::Skill* nonExistent = memory.getSkill("non_existent");
    assert(nonExistent == nullptr);
    
    // Test clear
    memory.clear();
    assert(memory.getAllSkills().empty());
    
    std::cout << "    testProceduralMemory passed" << std::endl;
}

void testAssociativeMemory() {
    nlm::AssociativeMemory memory;
    
    // Test initial state
    assert(memory.getAssociations(nlm::NeuronId(1)).empty());
    assert(memory.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.0f);
    
    // Test association creation
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    memory.associate(neuron1, neuron2, 0.8f);
    
    auto associations = memory.getAssociations(neuron1);
    assert(associations.size() == 1);
    assert(associations[0] == neuron2);
    
    assert(memory.getAssociationStrength(neuron1, neuron2) == 0.8f);
    
    // Test overwriting association
    memory.associate(neuron1, neuron2, 0.9f);
    assert(memory.getAssociationStrength(neuron1, neuron2) == 0.9f);
    
    // Test update association
    memory.updateAssociation(neuron1, neuron2, 0.1f);
    assert(memory.getAssociationStrength(neuron1, neuron2) == 1.0f);  // Should be capped at 1.0
    
    // Test clear
    memory.clear();
    assert(memory.getAssociations(neuron1).empty());
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testMemoryIntegration() {
    // Test that memory systems work together
    nlm::Config config;
    config.set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create memory systems
    nlm::WorkingMemory workingMemory;
    nlm::EpisodicMemory episodicMemory;
    nlm::SemanticMemory semanticMemory;
    nlm::ProceduralMemory proceduralMemory;
    nlm::AssociativeMemory associativeMemory;
    
    // Test working memory
    nlm::NeuronId neuron1(1);
    workingMemory.store(neuron1, 0.5f);
    assert(workingMemory.contains(neuron1));
    assert(workingMemory.getNeuronActivation(neuron1) == 0.5f);
    
    // Test episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.5f, 0.7f, 0.3f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 1.0f;
    
    episodicMemory.storeEpisode(episode);
    assert(episodicMemory.getEpisodeCount() == 1);
    
    // Test semantic memory
    semanticMemory.storeFact("experience", "moved_forward");
    assert(semanticMemory.hasFact("experience"));
    
    // Test procedural memory
    std::vector<nlm::NeuronId> pattern;
    pattern.push_back(neuron1);
    proceduralMemory.learnSkill("move_forward", pattern);
    
    // Test associative memory
    associativeMemory.associate(neuron1, nlm::NeuronId(2), 0.6f);
    assert(associativeMemory.getAssociationStrength(neuron1, nlm::NeuronId(2)) == 0.6f);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testAgentBrain() {
    nlm::Config config;
    config.set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test configuration
    assert(!agent.isRewardModulationEnabled());
    assert(!agent.isStructuralPlasticityEnabled());
    assert(!agent.isDevelopmentEnabled());
    assert(!agent.isCuriosityEnabled());
    
    // Test enable/disable
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isStructuralPlasticityEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    // Test state
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    assert(agent.getNeuromodulationLevel() == 0.0f);
    
    // Test reset
    agent.reset();
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    
    std::cout << "    testAgentBrain passed" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testSemanticMemory();
    testProceduralMemory();
    testAssociativeMemory();
    testMemoryIntegration();
    testAgentBrain();
}

} // namespace test_memory