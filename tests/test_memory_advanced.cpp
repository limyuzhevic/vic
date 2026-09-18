#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Maturation.hpp"
#include "development/Pruning.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

namespace test_memory_advanced {

void testWorkingMemory() {
    std::cout << "Testing working memory..." << std::endl;
    
    nlm::NeuralWorkingMemory workingMem(10, 100.0f, 0.5f);
    assert(workingMem.getCapacity() == 10);
    assert(workingMem.getDecayRate() == 0.5f);
    assert(workingMem.getConsolidationThreshold() == 100.0f);
    assert(workingMem.getCurrentLoad() == 0);
    
    nlm::MemoryItem item1;
    item1.type = nlm::MemoryItemType::Working;
    item1.content = std::vector<float>{1.0f, 2.0f, 3.0f};
    item1.timestamp = 1000.0f;
    item1.associationStrength = 0.9f;
    item1.importance = 0.8f;
    
    bool encode_result = workingMem.encode(item1);
    assert(encode_result);
    assert(workingMem.getCurrentLoad() == 1);
    
    std::vector<float> content{4.0f, 5.0f, 6.0f};
    bool store_result = workingMem.store("test_item", content, 0.7f);
    assert(store_result);
    assert(workingMem.getCurrentLoad() == 2);
    
    nlm::MemoryItem retrieved = workingMem.retrieve(500.0f);
    assert(retrieved.type == nlm::MemoryItemType::Working);
    assert(retrieved.associationStrength > 0.0f);
    
    workingMem.update(1.0f);
    assert(workingMem.getCurrentLoad() == 2);
    
    workingMem.clear();
    assert(workingMem.getCurrentLoad() == 0);
    
    std::cout << "  PASSED" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "Testing episodic memory..." << std::endl;
    
    nlm::NeuralEpisodicMemory episodicMem(100, 0.3f, 0.1f);
    assert(episodicMem.getCapacity() == 100);
    assert(episodicMem.getConsolidationRate() == 0.3f);
    assert(episodicMem.getRecallProbability() == 0.1f);
    assert(episodicMem.getCurrentLoad() == 0);
    
    nlm::EpisodicEvent event;
    event.timestamp = 1000.0f;
    event.context = std::vector<float>{0.1f, 0.2f, 0.3f};
    event.outcome = std::vector<float>{1.0f, 0.0f, 1.0f};
    event.significance = 0.8f;
    event.predictionError = 0.5f;
    
    bool encode_result = episodicMem.encode(event);
    assert(encode_result);
    assert(episodicMem.getCurrentLoad() == 1);
    
    nlm::EpisodicEvent retrieved = episodicMem.retrieve(1000.0f);
    assert(retrieved.timestamp == 1000.0f);
    assert(retrieved.significance == 0.8f);
    
    episodicMem.update(100.0f);
    assert(episodicMem.getCurrentLoad() == 1);
    
    episodicMem.clear();
    assert(episodicMem.getCurrentLoad() == 0);
    
    std::cout << "  PASSED" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "Testing associative memory..." << std::endl;
    
    nlm::AssociativeMemory assocMem(500, 0.8f, 0.2f);
    assert(assocMem.getCapacity() == 500);
    assert(assocMem.getStrengthDecay() == 0.8f);
    assert(assocMem.getForgettingRate() == 0.2f);
    assert(assocMem.getCurrentAssociations() == 0);
    
    bool learn_result = assocMem.learn(std::vector<float>{1.0f, 2.0f}, std::vector<float>{3.0f, 4.0f}, 0.9f);
    assert(learn_result);
    assert(assocMem.getCurrentAssociations() == 1);
    
    std::vector<float> result = assocMem.retrieve(std::vector<float>{1.0f, 2.0f});
    assert(result.size() == 2);
    assert(fabs(result[0] - 3.0f) < 0.001);
    assert(fabs(result[1] - 4.0f) < 0.001);
    
    assocMem.update(1.0f);
    assert(assocMem.getCurrentAssociations() == 1);
    
    assocMem.clear();
    assert(assocMem.getCurrentAssociations() == 0);
    
    std::cout << "  PASSED" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing memory integration..." << std::endl;
    
    nlm::NeuralWorkingMemory workingMem(10, 100.0f, 0.5f);
    nlm::NeuralEpisodicMemory episodicMem(100, 0.3f, 0.1f);
    
    nlm::MemoryItem working_item;
    working_item.type = nlm::MemoryItemType::Working;
    working_item.content = std::vector<float>{1.0f, 2.0f};
    working_item.timestamp = 1000.0f;
    
    workingMem.encode(working_item);
    
    nlm::EpisodicEvent episodic_item;
    episodic_item.timestamp = 1001.0f;
    episodic_item.context = std::vector<float>{0.5f, 0.5f};
    episodic_item.outcome = std::vector<float>{1.0f};
    episodic_item.significance = 0.7f;
    episodic_item.predictionError = 0.3f;
    
    episodicMem.encode(episodic_item);
    
    float working_strength = workingMem.getItemStrength("working_item");
    float episodic_strength = episodicMem.getItemStrength(1001.0f);
    
    assert(working_strength > 0.0f);
    assert(episodic_strength > 0.0f);
    
    workingMem.update(50.0f);
    episodicMem.update(100.0f);
    
    nlm::MemoryItem retrieved_working = workingMem.retrieve(0.0f);
    nlm::EpisodicEvent retrieved_episodic = episodicMem.retrieve(1001.0f);
    
    assert(retrieved_working.type == nlm::MemoryItemType::Working);
    assert(retrieved_episodic.timestamp == 1001.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Memory Tests ===" << std::endl;
    
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testMemoryIntegration();
    
    std::cout << "=== All Advanced Memory Tests PASSED ===" << std::endl;
}

} // namespace test_memory_advanced
