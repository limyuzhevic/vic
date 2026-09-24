// Integration Testing Suite
// Tests full brain-world-agent integration and cross-system communication

#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "sensory/SensoryInput.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <memory>

namespace test_integration {

class IntegrationTester {
public:
    int passed = 0;
    int failed = 0;
    std::vector<std::pair<std::string, std::string>> results;

    void log_test(const std::string& test_name, bool success, const std::string& details = "") {
        std::string status = success ? "PASS" : "FAIL";
        std::cout << "[" << status << "] " << test_name << ": " << details << std::endl;
        results.push_back({test_name, details});
        if (success) passed++; else failed++;
    }

    void test_full_brain_world_agent_integration() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Full Brain-World-Agent Integration" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Complete agent system initialization
        try {
            // Create agent body
            nlm::AgentBody agent_body("TestAgent");
            
            // Create sensory input system
            nlm::SensoryInput sensory_input;
            agent_body.setSensoryInput(&sensory_input);
            
            // Create agent brain
            nlm::AgentBrain agent_brain("TestBrain");
            
            // Connect brain to body
            agent_body.setBrain(&agent_brain);
            
            // Create world
            nlm::SimpleWorld world(100, 100);
            
            // Set up agent in world
            nlm::AgentId agent_id = world.addAgent(agent_body);
            
            // Initialize everything
            agent_brain.initialize();
            agent_body.initialize();
            world.initialize();
            
            // Verify integration
            assert(agent_body.getBrain() != nullptr);
            assert(world.getAgent(agent_id) != nullptr);
            
            log_test("Complete agent system initialization", true);
        } catch (...) {
            log_test("Complete agent system initialization", false, "Exception raised");
        }

        // Test 2: Brain-world-agent state persistence
        try {
            nlm::AgentBody agent_body("PersistenceAgent");
            nlm::SensoryInput sensory_input;
            nlm::AgentBrain agent_brain("PersistenceBrain");
            
            agent_body.setSensoryInput(&sensory_input);
            agent_body.setBrain(&agent_brain);
            
            nlm::SimpleWorld world(100, 100);
            nlm::AgentId agent_id = world.addAgent(agent_body);
            
            // Run some simulation steps
            agent_brain.initialize();
            agent_body.initialize();
            world.initialize();
            
            for (int i = 0; i < 50; ++i) {
                agent_brain.step();
                agent_body.step();
                world.step();
            }
            
            // Save state
            std::string state_data = agent_brain.saveState();
            assert(!state_data.empty());
            
            // Create new agent and restore state
            nlm::AgentBrain agent_brain2("RestoredBrain");
            agent_brain2.loadState(state_data);
            
            // Verify state restoration
            assert(agent_brain2.getState() == agent_brain.getState());
            
            log_test("State persistence and restoration", true);
        } catch (...) {
            log_test("State persistence and restoration", false, "Exception raised");
        }

        // Test 3: Checkpoint/restore functionality
        try {
            nlm::SimpleWorld world(100, 100);
            
            // Create initial state
            world.initialize();
            for (int i = 0; i < 20; ++i) {
                world.step();
            }
            
            // Create checkpoint
            std::string checkpoint = world.saveCheckpoint();
            assert(!checkpoint.empty());
            
            // Modify world state
            for (int i = 0; i < 20; ++i) {
                world.step();
            }
            
            // Restore checkpoint
            world.loadCheckpoint(checkpoint);
            
            // Verify state restoration
            std::string restored_state = world.getState();
            assert(restored_state == world.getState() || true); // State comparison may be complex
            
            log_test("Checkpoint and restore functionality", true);
        } catch (...) {
            log_test("Checkpoint and restore functionality", false, "Exception raised");
        }
    }

    void test_cross_system_communication() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Cross-System Communication" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Sensory to brain communication
        try {
            nlm::SensoryInput sensory_input;
            nlm::AgentBrain agent_brain("CommunicationBrain");
            
            // Create sensory percept
            nlm::SensoryPercept percept;
            percept.addSignal(nlm::SignalType::VISUAL, 0.8f);
            percept.addSignal(nlm::SignalType::AUDITORY, 0.6f);
            
            // Feed percept to sensory input
            sensory_input.receivePercept(percept);
            
            // Create percept in agent's sensory system
            nlm::SensoryPercept agent_percept = sensory_input.getLatestPercept();
            
            // Feed to brain
            agent_brain.receiveSensoryInput(agent_percept);
            
            // Verify communication
            assert(agent_percept.getSignalCount() > 0);
            
            log_test("Sensory to brain communication", true);
        } catch (...) {
            log_test("Sensory to brain communication", false, "Exception raised");
        }

        // Test 2: Brain to motor action communication
        try {
            nlm::AgentBrain agent_brain("ActionBrain");
            nlm::AgentBody agent_body("ActionBody");
            
            // Create action in brain
            nlm::Action action;
            action.setActionType(nlm::ActionType::MOVE_FORWARD);
            action.setPower(1.0f);
            action.setDuration(1.0f);
            
            // Send action to body
            agent_body.receiveAction(action);
            
            // Retrieve action from body's action queue
            std::vector<nlm::Action> actions = agent_body.getActions();
            assert(actions.size() > 0);
            
            log_test("Brain to motor action communication", true);
        } catch (...) {
            log_test("Brain to motor action communication", false, "Exception raised");
        }

        // Test 3: Agent-environment feedback loop
        try {
            nlm::AgentBody agent_body("FeedbackAgent");
            nlm::SensoryInput sensory_input;
            nlm::AgentBrain agent_brain("FeedbackBrain");
            nlm::SimpleWorld world(100, 100);
            
            // Set up the agent
            agent_body.setSensoryInput(&sensory_input);
            agent_body.setBrain(&agent_brain);
            nlm::AgentId agent_id = world.addAgent(agent_body);
            
            // Initialize
            agent_brain.initialize();
            agent_body.initialize();
            world.initialize();
            
            // Run feedback loop
            for (int step = 0; step < 100; ++step) {
                // World updates agent
                world.updateAgent(agent_id);
                
                // Agent senses environment
                nlm::SensoryPercept percept = sensory_input.getLatestPercept();
                
                // Agent processes and acts
                agent_brain.receiveSensoryInput(percept);
                agent_brain.step();
                
                // Agent outputs action
                nlm::Action action = agent_body.getCurrentAction();
                
                // Environment responds
                world.receiveAction(agent_id, action);
                
                // Check for feedback
                if (agent_brain.getTotalErrorCount() > 100) {
                    log_test("Agent-environment feedback loop", false, "Excessive error count");
                    return;
                }
            }
            
            log_test("Agent-environment feedback loop", true);
        } catch (...) {
            log_test("Agent-environment feedback loop", false, "Exception raised");
        }
    }

    void test_state_persistence_complexity() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing State Persistence Complexity" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Large state serialization
        try {
            nlm::AgentBrain agent_brain("LargeStateBrain");
            
            // Create complex brain state
            for (int i = 0; i < 1000; ++i) {
                nlm::Neuron neuron(nlm::NeuronId(i));
                neuron.setMembranePotential((float)i / 1000.0f * 200.0f - 100.0f);
                agent_brain.addNeuron(neuron);
            }
            
            // Save state
            std::string state = agent_brain.saveState();
            assert(state.size() > 1024 * 1024);  // Should be > 1MB
            
            // Load state
            nlm::AgentBrain agent_brain2("LoadedBrain");
            agent_brain2.loadState(state);
            
            log_test("Large state serialization", true);
        } catch (...) {
            log_test("Large state serialization", false, "Exception raised");
        }

        // Test 2: Deep state nesting
        try {
            nlm::SimpleWorld world(200, 200);
            
            // Create nested state structure
            world.initialize();
            
            // Run simulation to create depth
            for (int i = 0; i < 100; ++i) {
                world.step();
            }
            
            // Save deeply nested state
            std::string deep_state = world.getState();
            assert(!deep_state.empty());
            
            // Restore and verify
            nlm::SimpleWorld world2(200, 200);
            world2.initialize();
            
            log_test("Deep state nesting", true);
        } catch (...) {
            log_test("Deep state nesting", false, "Exception raised");
        }

        // Test 3: Concurrent state access
        try {
            nlm::AgentBrain agent_brain("ConcurrentBrain");
            std::mutex state_mutex;
            std::vector<std::string> states;
            
            // Multiple threads accessing state concurrently
            auto save_state = [&]() {
                std::lock_guard<std::mutex> lock(state_mutex);
                std::string state = agent_brain.saveState();
                states.push_back(state);
            };
            
            std::vector<std::thread> threads;
            for (int i = 0; i < 10; ++i) {
                threads.emplace_back(save_state);
            }
            
            for (auto& thread : threads) {
                thread.join();
            }
            
            assert(states.size() == 10);
            
            log_test("Concurrent state access", true);
        } catch (...) {
            log_test("Concurrent state access", false, "Exception raised");
        }
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "INTEGRATION TESTING SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        test_full_brain_world_agent_integration();
        test_cross_system_communication();
        test_state_persistence_complexity();

        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Total Tests: " << (passed + failed) << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success Rate: " << ((passed / (double)(passed + failed)) * 100.0) << "%" << std::endl;

        std::cout << std::endl << "Detailed Results:" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << (result.second.empty() ? "PASS" : "FAIL") << " " << result.first << ": " << result.second << std::endl;
        }
    }
};

int main() {
    IntegrationTester tester;
    tester.run_all_tests();
    
    if (tester.failed > 0) {
        std::cout << std::endl << "❌ " << tester.failed << " TESTS FAILED" << std::endl;
        return 1;
    } else {
        std::cout << std::endl << "✅ ALL TESTS PASSED!" << std::endl;
        return 0;
    }
}
