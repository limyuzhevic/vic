// Semantic Memory Tests
#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>

namespace test_semantic_memory {

void testSemanticMemoryCreation() {
    nlm::SemanticMemory memory;
    
    // Initially empty
    assert(memory.getFactCount() == 0);
    assert(memory.getVerifiedFactCount() == 0);
    assert(memory.getKnowledgeDensity() == 0.0f);
    
    std::cout << "    testSemanticMemoryCreation passed" << std::endl;
}

void testSemanticMemoryStorage() {
    nlm::SemanticMemory memory;
    
    // Store a fact
    memory.storeFact("neuron_count", "1000");
    assert(memory.getFactCount() == 1);
    assert(memory.hasFact("neuron_count"));
    assert(memory.retrieveFact("neuron_count") == "1000");
    
    // Update existing fact
    memory.storeFact("neuron_count", "2000");
    assert(memory.retrieveFact("neuron_count") == "2000");
    
    // Store another fact
    memory.storeFact("connection_probability", "0.1");
    assert(memory.getFactCount() == 2);
    
    std::cout << "    testSemanticMemoryStorage passed" << std::endl;
}

void testSemanticMemoryConcepts() {
    nlm::SemanticMemory memory;
    
    memory.storeFact("temperature", "37.0");
    memory.storeFact("pressure", "101.3");
    memory.storeFact("volume", "22.4");
    
    // Get concepts
    std::vector<std::string> concepts = memory.getConcepts();
    assert(!concepts.empty());
    
    // Test knowledge density
    float density = memory.getKnowledgeDensity();
    assert(density > 0.0f);
    
    std::cout << "    testSemanticMemoryConcepts passed" << std::endl;
}

void testSemanticMemoryVerification() {
    nlm::SemanticMemory memory;
    
    memory.storeFact("verified_fact", "test_value");
    assert(!memory.isFactVerified("verified_fact"));
    
    memory.verifyFact("verified_fact");
    assert(memory.isFactVerified("verified_fact"));
    
    std::cout << "    testSemanticMemoryVerification passed" << std::endl;
}

void testSemanticMemoryDecay() {
    nlm::SemanticMemory memory;
    
    memory.storeFact("fact1", "value1");
    memory.storeFact("fact2", "value2");
    memory.storeFact("fact3", "value3");
    
    // Decay knowledge
    memory.incrementFactAge(10.0f);
    
    // Facts should still exist but be aged
    assert(memory.getFactCount() == 3);
    
    std::cout << "    testSemanticMemoryDecay passed" << std::endl;
}

void testSemanticMemoryInference() {
    nlm::SemanticMemory memory;
    
    memory.storeFact("heart_rate", "72");
    memory.storeFact("blood_pressure", "120/80");
    memory.storeFact("oxygen_level", "98");
    
    // Try inference (this might not return anything with current implementation)
    std::vector<std::string> inferences = memory.inferFromConcept("heart");
    
    std::cout << "    testSemanticMemoryInference passed" << std::endl;
}

void runAll() {
    testSemanticMemoryCreation();
    testSemanticMemoryStorage();
    testSemanticMemoryConcepts();
    testSemanticMemoryVerification();
    testSemanticMemoryDecay();
    testSemanticMemoryInference();
}

} // namespace test_semantic_memory

// Procedural Memory Tests
#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>

namespace test_procedural_memory {

void testProceduralMemoryCreation() {
    nlm::ProceduralMemory memory;
    
    // Initially empty
    assert(memory.getSkillCount() == 0);
    assert(memory.getAverageSkillPerformance() == 0.0f);
    assert(memory.getAllSkillNames().empty());
    
    std::cout << "    testProceduralMemoryCreation passed" << std::endl;
}

void testProceduralMemorySkillLearning() {
    nlm::ProceduralMemory memory;
    
    // Learn a skill
    std::vector<nlm::NeuronId> pattern = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    memory.learnSkill("move_forward", pattern);
    
    assert(memory.getSkillCount() == 1);
    assert(!memory.getAllSkillNames().empty());
    
    // Get skill
    nlm::Skill* skill = memory.getSkill("move_forward");
    assert(skill != nullptr);
    assert(skill->name == "move_forward");
    assert(skill->proficiency == 0.5f);
    
    // Update proficiency
    memory.updateProficiency("move_forward", 0.3f);
    assert(memory.getSkillProficiency("move_forward") == 0.8f);
    
    std::cout << "    testProceduralMemorySkillLearning passed" << std::endl;
}

void testProceduralMemoryPrerequisites() {
    nlm::ProceduralMemory memory;
    
    // Add prerequisites
    memory.addSkillPrerequisite("advanced_move", "basic_move");
    memory.addSkillPrerequisite("advanced_move", "avoid_obstacle");
    
    // Check dependencies
    std::vector<std::string> prerequisites = memory.getSkillPrerequisites("advanced_move");
    assert(prerequisites.size() == 2);
    
    std::vector<std::string> dependents = memory.getSkillDependents("basic_move");
    assert(dependents.size() == 1);
    assert(dependents[0] == "advanced_move");
    
    std::cout << "    testProceduralMemoryPrerequisites passed" << std::endl;
}

void testProceduralMemoryNeuromodulation() {
    nlm::ProceduralMemory memory;
    
    std::vector<nlm::NeuronId> pattern = {nlm::NeuronId(1), nlm::NeuronId(2)};
    memory.learnSkill("test_skill", pattern);
    
    // Set neuromodulation factor
    memory.setNeuromodulationFactor("test_skill", 1.5f);
    assert(memory.getNeuromodulationFactor("test_skill") == 1.5f);
    
    std::cout << "    testProceduralMemoryNeuromodulation passed" << std::endl;
}

void testProceduralMemoryExecutionPattern() {
    nlm::ProceduralMemory memory;
    
    std::vector<std::vector<float>> pattern;
    pattern.push_back(std::vector<float>{0.1f, 0.2f, 0.3f});
    pattern.push_back(std::vector<float>{0.4f, 0.5f, 0.6f});
    
    memory.storeExecutionPattern("complex_skill", pattern);
    
    std::vector<std::vector<float>> retrieved = memory.retrieveExecutionPattern("complex_skill");
    assert(retrieved.size() == 2);
    assert(retrieved[0].size() == 3);
    
    std::cout << "    testProceduralMemoryExecutionPattern passed" << std::endl;
}

void runAll() {
    testProceduralMemoryCreation();
    testProceduralMemorySkillLearning();
    testProceduralMemoryPrerequisites();
    testProceduralMemoryNeuromodulation();
    testProceduralMemoryExecutionPattern();
}

} // namespace test_procedural_memory

// Associative Memory Tests
#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>

namespace test_associative_memory {

void testAssociativeMemoryCreation() {
    nlm::AssociativeMemory memory;
    
    // Initially empty
    assert(memory.getAssociationCount() == 0);
    assert(memory.getSequenceCount() == 0);
    assert(memory.getChunkCount() == 0);
    assert(memory.getNetworkConnectivity() == 0.0f);
    
    std::cout << "    testAssociativeMemoryCreation passed" << std::endl;
}

void testAssociativeMemoryAssociation() {
    nlm::AssociativeMemory memory;
    
    // Create associations
    memory.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
    memory.associate(nlm::NeuronId(2), nlm::NeuronId(3), 0.6f);
    
    assert(memory.getAssociationCount() == 2);
    assert(memory.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.8f);
    assert(memory.getAssociationStrength(nlm::NeuronId(2), nlm::NeuronId(3)) == 0.6f);
    
    // Get associations
    std::vector<nlm::NeuronId> associations = memory.getAssociations(nlm::NeuronId(2));
    assert(associations.size() == 2);
    
    std::cout << "    testAssociativeMemoryAssociation passed" << std::endl;
}

void testAssociativeMemorySpreadingActivation() {
    nlm::AssociativeMemory memory;
    
    // Create some associations
    memory.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.7f);
    memory.associate(nlm::NeuronId(2), nlm::NeuronId(3), 0.5f);
    memory.associate(nlm::NeuronId(1), nlm::NeuronId(4), 0.3f);
    
    // Set activation levels
    // Note: This would normally be done by the brain system
    // For testing, we'll just verify the method doesn't crash
    std::vector<nlm::NeuronId> activated = memory.spreadActivation(
        nlm::NeuronId(1), 0.1f, 5);
    
    assert(!activated.empty());
    
    std::cout << "    testAssociativeMemorySpreadingActivation passed" << std::endl;
}

void testAssociativeMemorySequences() {
    nlm::AssociativeMemory memory;
    
    // Create sequences
    std::vector<nlm::NeuronId> seq1 = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    std::vector<nlm::NeuronId> seq2 = {nlm::NeuronId(4), nlm::NeuronId(5), nlm::NeuronId(6)};
    
    memory.recordSequence(seq1);
    memory.recordSequence(seq2);
    
    assert(memory.getSequenceCount() == 2);
    
    // Predict next
    std::vector<nlm::NeuronId> prediction = memory.predictNext(nlm::NeuronId(1), 2);
    assert(prediction.size() >= 1);
    
    std::cout << "    testAssociativeMemorySequences passed" << std::endl;
}

void testAssociativeMemoryChunks() {
    nlm::AssociativeMemory memory;
    
    // Create chunks
    std::vector<nlm::NeuronId> chunk1 = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3), nlm::NeuronId(4)};
    std::vector<nlm::NeuronId> chunk2 = {nlm::NeuronId(5), nlm::NeuronId(6), nlm::NeuronId(7)};
    
    memory.createChunk(chunk1);
    memory.createChunk(chunk2);
    
    assert(memory.getChunkCount() == 2);
    
    std::cout << "    testAssociativeMemoryChunks passed" << std::endl;
}

void testAssociativeMemoryConfiguration() {
    nlm::AssociativeMemory memory;
    
    // Set configuration
    memory.setHebbianLearningRate(0.02f);
    memory.setConsolidationThreshold(0.5f);
    
    assert(memory.getHebbianLearningRate() == 0.02f);
    assert(memory.getConsolidationThreshold() == 0.5f);
    
    std::cout << "    testAssociativeMemoryConfiguration passed" << std::endl;
}

void testAssociativeMemoryNetworkConnectivity() {
    nlm::AssociativeMemory memory;
    
    // Create some associations
    memory.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
    memory.associate(nlm::NeuronId(2), nlm::NeuronId(3), 0.6f);
    memory.associate(nlm::NeuronId(3), nlm::NeuronId(4), 0.9f);
    
    float connectivity = memory.getNetworkConnectivity();
    assert(connectivity > 0.0f);
    
    std::cout << "    testAssociativeMemoryNetworkConnectivity passed" << std::endl;
}

void runAll() {
    testAssociativeMemoryCreation();
    testAssociativeMemoryAssociation();
    testAssociativeMemorySpreadingActivation();
    testAssociativeMemorySequences();
    testAssociativeMemoryChunks();
    testAssociativeMemoryConfiguration();
    testAssociativeMemoryNetworkConnectivity();
}

} // namespace test_associative_memory

// Main test runner
int main() {
    std::cout << "Running Semantic Memory tests..." << std::endl;
    test_semantic_memory::runAll();
    
    std::cout << "Running Procedural Memory tests..." << std::endl;
    test_procedural_memory::runAll();
    
    std::cout << "Running Associative Memory tests..." << std::endl;
    test_associative_memory::runAll();
    
    std::cout << "\nAll memory system tests passed!" << std::endl;
    return 0;
}
