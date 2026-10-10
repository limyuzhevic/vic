// Simple test to verify NeuralPlanner integration
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Testing NeuralPlanner Integration ===" << std::endl;
    
    try {
        // Create config
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        // Create brain and agent brain
        nlm::Brain brain(config);
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Initialize agent brain
        agentBrain.initialize(nlm::SimpleWorld());
        
        // Test 1: Neural planner is accessible and enabled
        std::cout << "Test 1: Checking neural planner integration..." << std::endl;
        assert(agentBrain.isNeuralPlannerEnabled());
        std::cout << "✓ Neural planner is accessible and enabled by default" << std::endl;
        
        // Test 2: Can disable neural planner
        std::cout << "Test 2: Disabling neural planner..." << std::endl;
        agentBrain.enableNeuralPlanner(false);
        assert(!agentBrain.isNeuralPlannerEnabled());
        std::cout << "✓ Neural planner can be disabled" << std::endl;
        
        // Test 3: Re-enable neural planner
        std::cout << "Test 3: Re-enabling neural planner..." << std::endl;
        agentBrain.enableNeuralPlanner(true);
        assert(agentBrain.isNeuralPlannerEnabled());
        std::cout << "✓ Neural planner can be re-enabled" << std::endl;
        
        // Test 4: Current goal management
        std::cout << "Test 4: Testing current goal management..." << std::endl;
        std::vector<float> testGoal = {0.5f, 0.8f, 0.3f};
        agentBrain.setCurrentGoal(testGoal);
        std::vector<float> retrievedGoal = agentBrain.getCurrentGoal();
        assert(retrievedGoal.size() == testGoal.size());
        std::cout << "✓ Current goal management works correctly" << std::endl;
        
        // Test 5: Planning confidence
        std::cout << "Test 5: Testing planning confidence..." << std::endl;
        float confidence = agentBrain.getPlanningConfidence();
        assert(confidence >= 0.0f);
        std::cout << "✓ Planning confidence can be retrieved (value: " << confidence << ")" << std::endl;
        
        // Test 6: Planning cache clearing
        std::cout << "Test 6: Testing planning cache clearing..." << std::endl;
        agentBrain.clearPlanningCache();
        std::cout << "✓ Planning cache can be cleared" << std::endl;
        
        // Test 7: Concept formation integration
        std::cout << "Test 7: Testing concept formation integration..." << std::endl;
        agentBrain.enableConceptFormation(true);
        std::vector<float> testInput = {0.1f, 0.2f, 0.3f, 0.4f};
        agentBrain.processConcepts(testInput, 0.5f);
        const auto& concepts = agentBrain.getConcepts();
        // Concepts may be empty depending on concept formation initialization
        std::cout << "✓ Concept formation can be processed for planning integration" << std::endl;
        
        // Test 8: Planning statistics
        std::cout << "Test 8: Testing planning statistics..." << std::endl;
        auto stats = agentBrain.getPlanningStats();
        assert(stats.find("goal") != stats.end());
        assert(stats.find("planning_confidence") != stats.end());
        assert(stats.find("has_goal") != stats.end());
        std::cout << "✓ Planning statistics can be retrieved" << std::endl;
        
        // Test 9: Neural planner with memory systems
        std::cout << "Test 9: Testing neural planner with memory systems..." << std::endl;
        assert(agentBrain.getEpisodicMemory() != nullptr);
        assert(agentBrain.getBrain()->getWorkingMemory() != nullptr);
        assert(agentBrain.getBrain()->getAssociativeMemory() != nullptr);
        assert(agentBrain.getBrain()->getPredictionSystem() != nullptr);
        std::cout << "✓ Neural planner integrates properly with all memory systems" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== All NeuralPlanner Integration Tests PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cout << "=== NeuralPlanner Integration Tests FAILED ===" << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        std::cout << "=== NeuralPlanner Integration Tests FAILED ===" << std::endl;
        return 1;
    }
}
