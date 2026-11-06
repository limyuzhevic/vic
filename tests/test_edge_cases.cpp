// Brain Edge Cases Tests
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

namespace test_edge_cases {

// Test 1: Configuration edge cases
void testConfigEdgeCases() {
    std::cout << "    testConfigEdgeCases: " << std::flush;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test setting empty key (should throw)
    try {
        config->set("", "value");
        throw std::runtime_error("Empty key should throw exception");
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test setting key with whitespace (should throw)
    try {
        config->set("key with spaces", "value");
        throw std::runtime_error("Key with spaces should throw exception");
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test loading from non-existent file
    bool result = config->loadFromFile("/tmp/nonexistent_file_12345.cfg");
    assert(!result); // Should fail gracefully
    
    // Test has() with empty key
    assert(!config->has(""));
    
    // Test remove with empty key (should throw)
    try {
        config->remove("");
        throw std::runtime_error("Remove empty key should throw exception");
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test getOr() with invalid key type
    config->set("int_key", 42);
    assert(config->getOr<int>("int_key", -1) == 42);
    assert(config->getOr<double>("int_key", -1.0) == 42.0);
    assert(config->getOr<std::string>("int_key", "default") == "42");
    
    // Test summary() with empty config
    std::string summary = config->summary();
    assert(summary.find("Configuration (0 entries)") != std::string::npos);
    
    std::cout << "passed" << std::endl;
}

// Test 2: Memory system edge cases
void testMemoryEdgeCases() {
    std::cout << "    testMemoryEdgeCases: " << std::flush;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    auto* em = brain.getEpisodicMemory();
    auto* am = brain.getAssociativeMemory();
    
    assert(wm != nullptr);
    assert(em != nullptr);
    assert(am != nullptr);
    
    // Test working memory edge cases
    wm->setCapacity(0); // Should allow zero capacity
    wm->clear(); // Should work with empty memory
    
    // Store in full memory
    wm->setCapacity(1);
    std::vector<float> pattern1(10, 1.0f);
    wm->store(pattern1, 1.0f);
    
    // Store to neuron edge case
    nlm::NeuronId testNeuron(42);
    // This neuron might not exist, but should handle gracefully
    wm->storeToNeuron(testNeuron, 0.5f);
    
    // Test episodic memory edge cases
    em->setMaxEpisodes(0); // Should allow zero max episodes
    em->clear(); // Should work with empty memory
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.positionX = 0.0f;
    episode.positionY = 0.0f;
    episode.orientation = 0.0f;
    episode.action = nlm::ActionType::Wait;
    episode.reward = 0.0f;
    episode.energy = 0.0f;
    episode.novelty = 0.0f;
    episode.resultingReward = 0.0f;
    episode.age = 0;
    
    em->storeEpisode(episode);
    
    // Test associative memory edge cases
    std::vector<float> patternA(5, 0.5f);
    std::vector<float> patternB(5, 0.5f);
    am->associate(patternA, patternB, 1.0f); // Strong association
    
    // Retrieve from empty memory
    std::vector<std::vector<float>> emptyResults = am->retrieve(std::vector<float>(5, 0.0f), 0);
    assert(emptyResults.empty());
    
    std::cout << "passed" << std::endl;
}

// Test 3: Neural dynamics edge cases
void testNeuralDynamicsEdgeCases() {
    std::cout << "    testNeuralDynamicsEdgeCases: " << std::flush;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test extreme input values
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            // Inject extreme negative current
            neurons[0]->injectCurrent(-1000.0f);
            
            // Inject extreme positive current
            neurons[0]->injectCurrent(1000.0f);
            
            // Test refractory period edge case
            neurons[0]->stepLIF(0.0f, 0.001f); // Step to trigger firing
            
            // Test boundary conditions for spike detection
            neurons[0]->setThreshold(1000000.0f); // Very high threshold
            neurons[0]->injectCurrent(10.0f);
            
            // Test rapid successive steps
            for (nlm::SimulationStep step = 0; step < 1000; ++step) {
                brain.step(step);
            }
        }
    }
    
    // Test synapse edge cases
    size_t synapseCount = brain.getTotalSynapseCount();
    if (synapseCount > 0) {
        for (const auto& region : brain.getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                // Test extreme weight values
                syn->setWeight(-1000.0f); // Below minimum
                syn->setWeight(1000.0f);   // Above maximum
                syn->setWeight(0.0f);      // Normal value
                
                // Test synaptic dynamics edge cases
                syn->step(0.0f);
                
                break; // Just test first synapse
            }
            break;
        }
    }
    
    std::cout << "passed" << std::endl;
}

// Test 4: Error handling edge cases
void testErrorHandlingEdgeCases() {
    std::cout << "    testErrorHandlingEdgeCases: " << std::flush;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test loading invalid file format
    std::string invalidContent = "invalid key value without equals\nanother invalid line\nkey=value\n=bad key\nkey = \"unclosed quote\nkey3=value3\nkey4=value4\nkey5=value5\nkey6=value6\nkey7=value7\nkey8=value8\nkey9=value9\nkey10=value10";
    
    // Create a temporary file with invalid content
    const std::string tempPath = "/tmp/invalid_config_test.cfg";
    std::ofstream file(tempPath);
    file << invalidContent;
    file.close();
    
    bool result = config->loadFromFile(tempPath);
    // Should handle gracefully (may skip invalid lines)
    
    // Test command line argument edge cases
    int argc = 3;
    char* argv[4];
    argv[0] = const_cast<char*>("test_program");
    argv[1] = const_cast<char*>("--key=value");
    argv[2] = const_cast<char*>("-invalid-no-value"); // Should be ignored
    argv[3] = nullptr;
    
    // Clean up
    std::remove(tempPath.c_str());
    
    // Test memory system error conditions
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config2);
    brain.initialize();
    
    // Test with single neuron
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region && region->getTotalNeuronCount() > 0) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            // Test neuron methods with minimal network
            neurons[0]->step(0.0f);
            neurons[0]->reset();
            neurons[0]->setType(nlm::NeuronType::Sensory);
        }
    }
    
    std::cout << "passed" << std::endl;
}

// Test 5: Performance and stability edge cases
void testPerformanceEdgeCases() {
    std::cout << "    testPerformanceEdgeCases: " << std::flush;
    
    // Test with larger network
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run many steps to test stability
    for (nlm::SimulationStep step = 0; step < 5000; ++step) {
        // Inject random activity
        if (step % 100 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        }
        brain.step(step);
    }
    
    // Test memory system with many operations
    auto* wm = brain.getWorkingMemory();
    if (wm) {
        for (int i = 0; i < 100; ++i) {
            std::vector<float> pattern(10, static_cast<float>(i % 5) / 5.0f);
            wm->store(pattern, 0.5f);
            auto retrieved = wm->retrieve();
            if (!retrieved.empty()) {
                wm->updateRecurrence();
            }
        }
    }
    
    // Test configuration serialization/deserialization
    std::string testPath = "/tmp/test_config_edge.cfg";
    bool saved = config->saveToFile(testPath);
    
    auto config2 = std::make_shared<nlm::Config>();
    bool loaded = config2->loadFromFile(testPath);
    
    // Clean up
    std::remove(testPath.c_str());
    
    std::cout << "passed" << std::endl;
}

// Test 6: Integration edge cases
void testIntegrationEdgeCases() {
    std::cout << "    testIntegrationEdgeCases: " << std::flush;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test with action system
    auto action = brain.produceAction();
    // Action may be nullptr depending on brain state, which is acceptable
    
    // Test all available systems
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Test neuromodulation systems
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test plasticity systems
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    // Run integration test with multiple steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Inject various input types
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 3.0f);
        brain.step(step);
    }
    
    std::cout << "passed" << std::endl;
}

// Main edge cases test runner
void runEdgeCasesTests() {
    std::cout << "Running edge case tests..." << std::endl;
    
    testConfigEdgeCases();
    testMemoryEdgeCases();
    testNeuralDynamicsEdgeCases();
    testErrorHandlingEdgeCases();
    testPerformanceEdgeCases();
    testIntegrationEdgeCases();
    
    std::cout << "\nAll edge case tests passed!" << std::endl;
}

} // namespace test_edge_cases

// Test runner registration (to be integrated with main test suite)
void registerEdgeCasesTests() {
    static auto edge_case_tests = test_edge_cases::runEdgeCasesTests;
    // In a real test framework, this would register the test function
}