// Agent Integration Tests
// Tests AgentBrain + SimpleWorld sensorimotor coordination

#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "environment/Action.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/MotorOutput.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <memory>

namespace test_agent_integration {

class AgentIntegrationTest {
public:
    void runAllTests() {
        std::cout << "=== Agent Integration Tests ===" << std::endl;
        
        testSensorimotorCoordination();
        testAgentWorldInteraction();
        testAgentBrainInterface();
        testEnvironmentalPerception();
        testActionSelection();
        testAgentPerformanceBenchmarks();
        
        std::cout << "\n=== Agent Integration Tests Complete ===" << std::endl;
    }
    
private:
    void testSensorimotorCoordination() {
        std::cout << "\n[TEST] Sensorimotor Coordination" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        // Create agent-brain interface
        nlm::AgentBrain agentBrain(config);
        assert(agentBrain.initialize());
        
        // Create world interface
        nlm::SimpleWorld world;
        assert(world.initialize());
        
        // Create environment
        nlm::Environment environment;
        assert(environment.initialize());
        
        // Test sensorimotor loop
        nlm::SimulationStep step = 0;
        for (int i = 0; i < 10; ++i) {
            // Get observation from environment
            nlm::Observation observation = environment.getObservation(step);
            
            // Process observation with agent brain
            nlm::Action action = agentBrain.processObservation(observation);
            
            assert(action != nullptr);
            
            // Execute action in world
            world.executeAction(action);
            
            // Update environment
            environment.step(step + i);
            
            // Agent brain processes world feedback
            agentBrain.processFeedback(action, 0.5f);
        }
        
        // Verify sensorimotor coordination
        assert(agentBrain.getActionHistory().size() > 0);
        assert(world.getInternalState().size() > 0);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Sensorimotor coordination test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testAgentWorldInteraction() {
        std::cout << "\n[TEST] Agent-World Interaction" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        
        // Create agent-brain with integrated brain
        nlm::AgentBrain agentBrain(config);
        assert(agentBrain.initialize());
        
        // Create world with sensory input
        nlm::SimpleWorld world;
        assert(world.initialize());
        
        // Test bidirectional interaction
        nlm::SimulationStep step = 0;
        
        // Agent perceives world state
        nlm::Observation observation = world.getWorldState(step);
        nlm::Action action = agentBrain.processObservation(observation);
        
        // World responds to agent action
        world.update(step, action);
        
        // Agent receives world feedback
        float feedback = world.getFeedback(action);
        agentBrain.processFeedback(action, feedback);
        
        // Agent produces next action based on feedback
        nlm::Action nextAction = agentBrain.produceAction();
        assert(nextAction != nullptr);
        
        // Verify interaction
        assert(agentBrain.getSensoryPercept().getLastObservation() != nullptr);
        assert(world.getActionHistory().size() > 0);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Agent-world interaction test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testAgentBrainInterface() {
        std::cout << "\n[TEST] AgentBrain Interface" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
        
        // Create agent brain with full integration
        nlm::AgentBrain agentBrain(config);
        assert(agentBrain.initialize());
        
        // Test neural interface access
        auto& brain = agentBrain.getBrain();
        assert(brain.getTotalNeuronCount() > 0);
        
        // Test sensory percept integration
        auto& sensoryPercept = agentBrain.getSensoryPercept();
        assert(sensoryPercept.getInputBuffer().size() > 0);
        
        // Test action production interface
        nlm::Action action = agentBrain.produceAction();
        assert(action != nullptr);
        
        // Test feedback processing
        agentBrain.processFeedback(action, 0.8f);
        agentBrain.processFeedback(action, 0.3f);
        
        // Test state management
        agentBrain.update(0.01f);
        assert(agentBrain.getInternalState().size() > 0);
        
        // Test configuration access
        auto agentConfig = agentBrain.getConfig();
        assert(agentConfig != nullptr);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: AgentBrain interface test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testEnvironmentalPerception() {
        std::cout << "\n[TEST] Environmental Perception" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::AgentBrain agentBrain(config);
        assert(agentBrain.initialize());
        
        nlm::Environment environment;
        assert(environment.initialize());
        
        nlm::SimulationStep step = 0;
        
        // Test multi-sensory perception
        nlm::Observation observation = environment.getObservation(step);
        
        // Process through agent's sensory percept
        auto& sensoryPercept = agentBrain.getSensoryPercept();
        sensoryPercept.processObservation(observation);
        
        // Verify sensory processing
        assert(sensoryPercept.getProcessedSensory().size() > 0);
        assert(sensoryPercept.getFeatureExtractors().size() > 0);
        
        // Test perception history
        sensoryPercept.addToHistory(observation, step);
        assert(sensoryPercept.getPerceptionHistory().size() > 0);
        
        // Test environmental feedback integration
        environment.step(step, observation);
        float environmentFeedback = environment.getEnvironmentalFeedback();
        
        agentBrain.processEnvironmentalFeedback(environmentFeedback);
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Environmental perception test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testActionSelection() {
        std::cout << "\n[TEST] Action Selection" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
        
        nlm::AgentBrain agentBrain(config);
        assert(agentBrain.initialize());
        
        nlm::Environment environment;
        assert(environment.initialize());
        
        nlm::SimulationStep step = 0;
        
        // Test various action selection scenarios
        for (int i = 0; i < 20; ++i) {
            nlm::Observation observation = environment.getObservation(step + i);
            
            // Get action from agent brain
            nlm::Action action = agentBrain.processObservation(observation);
            assert(action != nullptr);
            
            // Process action feedback
            float reward = environment.getReward(action);
            agentBrain.processFeedback(action, reward);
            
            // Agent brain updates action selection based on feedback
            agentBrain.updateActionSelection(action, reward);
        }
        
        // Verify action selection optimization
        auto& actionHistory = agentBrain.getActionHistory();
        std::unordered_map<nlm::ActionType, int> actionCounts;
        for (const auto& action : actionHistory) {
            actionCounts[action->getType()]++;
        }
        
        // Check that action selection is diverse and optimized
        assert(actionCounts.size() > 1);  // Multiple action types selected
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Action selection test completed in " << duration_ms << "ms" << std::endl;
    }
    
    void testAgentPerformanceBenchmarks() {
        std::cout << "\n[TEST] Agent Performance Benchmarks" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        struct BenchmarkConfig {
            int neuron_count;
            int simulation_steps;
            int perception_cycles;
        };
        
        std::vector<BenchmarkConfig> configs = {
            {50, 100, 50},
            {100, 200, 100},
            {200, 300, 150}
        };
        
        for (const auto& config : configs) {
            std::cout << "    Testing with " << config.neuron_count << " neurons..." << std::endl;
            
            auto test_config = std::make_shared<nlm::Config>();
            test_config->set("neuron_count", static_cast<int64_t>(config.neuron_count), nlm::ConfigSource::Default);
            
            nlm::AgentBrain agentBrain(test_config);
            assert(agentBrain.initialize());
            
            nlm::Environment environment;
            assert(environment.initialize());
            
            nlm::SimulationStep step = 0;
            
            // Run perception and action cycles
            for (int i = 0; i < config.perception_cycles; ++i) {
                nlm::Observation observation = environment.getObservation(step + i);
                nlm::Action action = agentBrain.processObservation(observation);
                
                float reward = environment.getReward(action);
                agentBrain.processFeedback(action, reward);
                
                agentBrain.update(0.01f);
            }
            
            // Run simulation steps
            for (int i = 1; i < config.simulation_steps; ++i) {
                agentBrain.update(0.01f);
            }
        }
        
        auto duration = std::chrono::high_resolution_clock::now() - start_time;
        double duration_ms = std::chrono::duration<double, std::milli>(duration).count();
        
        std::cout << "    PASSED: Agent performance benchmarks completed in " << duration_ms << "ms" << std::endl;
    }
};

void runAllAgentIntegrationTests() {
    AgentIntegrationTest tester;
    tester.runAllTests();
}

} // namespace test_agent_integration
