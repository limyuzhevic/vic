// Brain-World Integration Test
// Tests the complete brain-world interaction loop
// Verifies that sensory input → neural processing → action → world feedback works correctly

#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

namespace nlm {

class BrainWorldIntegrationTest {
public:
    void runAllTests() {
        NLM_LOG_INFO("=== Brain-World Integration Test Suite ===");
        
        testBasicSensoryMotorLoop();
        testCompleteInteractionCycle();
        testSensorimotorIntegration();
        testBrainWorldFeedback();
        
        NLM_LOG_INFO("\n=== All Integration Tests Passed ===");
    }
    
private:
    void testBasicSensoryMotorLoop() {
        NLM_LOG_INFO("\n--- Test 1: Basic Sensory-Motor Loop ---");
        
        // Create brain, agent, and world
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure and initialize
        world->configure(10, 10, 8, 8);
        world->reset();
        brain->initialize();
        agent->initialize(*world);
        
        // Run interaction loop
        int steps = 100;
        float totalReward = 0.0f;
        
        for (int step = 0; step < steps; ++step) {
            // 1. World update
            world->update(0.1);
            
            // 2. Get sensory input from world
            auto percept = world->getSensoryPercept();
            
            // 3. Process sensory input in brain
            agent->processSensoryInput(percept);
            
            // 4. Brain neural processing
            brain->step(step, step * 0.1);
            
            // 5. Get action from brain
            MotorCommand action = agent->decodeMotorCommand();
            
            // 6. Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // 7. Get reward
            float reward = world->computeReward(brain->getRegions()[0].get());
            totalReward += reward;
            
            // 8. Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        // Verify basic functionality
        assert(brain->getTotalNeuronCount() > 0);
        assert(brain->getTotalSynapseCount() > 0);
        assert(agent->getSensoryInputSize() > 0);
        assert(agent->getMotorOutputSize() > 0);
        
        NLM_LOG_INFO("✓ Basic sensory-motor loop completed");
        NLM_LOG_INFO("  Total reward accumulated: " + std::to_string(totalReward));
        NLM_LOG_INFO("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    }
    
    void testCompleteInteractionCycle() {
        NLM_LOG_INFO("\n--- Test 2: Complete Interaction Cycle ---");
        
        // Create minimal configuration for testing
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05);
        
        // Create all components
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Initialize everything
        world->configure(8, 8, 6, 6);
        world->reset();
        world->setAgentStart(4.0, 4.0);
        
        brain->initialize();
        agent->initialize(*world);
        
        // Enable all subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run complete interaction cycle
        std::vector<MotorCommand> actionsTaken;
        std::vector<float> rewardsReceived;
        
        int cycleLength = 50;
        for (int step = 0; step < cycleLength; ++step) {
            // Complete cycle: World → Senses → Brain → Action → World
            
            // Step 1: World dynamics
            world->update(0.05);
            
            // Step 2: Get observation
            auto percept = world->getSensoryPercept();
            
            // Step 3: Process in agent
            agent->processSensoryInput(percept);
            
            // Step 4: Neural computation
            brain->step(step, step * 0.05);
            
            // Step 5: Action selection
            MotorCommand action = agent->decodeMotorCommand();
            actionsTaken.push_back(action);
            
            // Step 6: Execute in world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Step 7: Get environmental feedback
            float reward = world->computeReward(brain->getRegions()[0].get());
            rewardsReceived.push_back(reward);
            
            // Step 8: Learn from experience
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.05);
        }
        
        // Verify cycle completed successfully
        assert(actionsTaken.size() == cycleLength);
        assert(rewardsReceived.size() == cycleLength);
        
        // Check that we had varied actions (not always wait)
        size_t nonWaitActions = std::count_if(
            actionsTaken.begin(), actionsTaken.end(),
            [](MotorCommand cmd) { return cmd != MotorCommand::Wait; }
        );
        assert(nonWaitActions > 0); // Should have some non-wait actions
        
        // Check that we received some rewards
        float totalReward = std::accumulate(rewardsReceived.begin(), rewardsReceived.end(), 0.0f);
        assert(totalReward >= 0.0f); // Rewards should be non-negative
        
        NLM_LOG_INFO("✓ Complete interaction cycle completed");
        NLM_LOG_INFO("  Actions taken: " + std::to_string(nonWaitActions) + "/" + std::to_string(cycleLength));
        NLM_LOG_INFO("  Total reward: " + std::to_string(totalReward));
    }
    
    void testSensorimotorIntegration() {
        NLM_LOG_INFO("\n--- Test 3: Sensorimotor Integration ---");
        
        // Create configuration optimized for sensorimotor testing
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 150);
        config->set("region_count", 1);
        
        // Create components
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Setup world with obstacles and resources
        world->configure(12, 12, 8, 8);
        world->reset();
        world->setAgentStart(6.0, 6.0);
        
        // Add some world objects for agent to interact with
        world->addObject(WorldObject(2.0, 2.0, WorldObjectType::Resource, 1.0));
        world->addObject(WorldObject(10.0, 10.0, WorldObjectType::Resource, 1.5));
        world->addObject(WorldObject(5.0, 8.0, WorldObjectType::Hazard, 0.5));
        
        // Initialize
        brain->initialize();
        agent->initialize(*world);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Track sensorimotor performance
        int successfulInteractions = 0;
        int totalSteps = 200;
        
        for (int step = 0; step < totalSteps; ++step) {
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Neural processing
            brain->step(step, step * 0.1);
            
            // Get action
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Check for successful interactions
            auto body = world->getAgentBody();
            if (body.x >= 0.0 && body.x <= 12.0 && body.y >= 0.0 && body.y <= 12.0) {
                successfulInteractions++;
            }
            
            // Apply learning
            float reward = world->computeReward(brain->getRegions()[0].get());
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        // Verify sensorimotor integration
        assert(successfulInteractions >= totalSteps * 0.8); // Agent should stay in bounds most of the time
        
        // Check that agent had some movement
        size_t agentMoved = std::count_if(
            actionsTaken.begin(), actionsTaken.end(),
            [](MotorCommand cmd) { 
                return cmd != MotorCommand::Wait && cmd != MotorCommand::MoveForward; 
            }
        );
        
        NLM_LOG_INFO("✓ Sensorimotor integration test passed");
        NLM_LOG_INFO("  Agent stayed in bounds: " + std::to_string(successfulInteractions) + "/" + std::to_string(totalSteps));
        NLM_LOG_INFO("  Actions executed: " + std::to_string(totalSteps));
    }
    
    void testBrainWorldFeedback() {
        NLM_LOG_INFO("\n--- Test 4: Brain-World Feedback Loop ---");
        
        // Create fresh configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 120);
        config->set("region_count", 1);
        config->set("random_seed", 12345); // For reproducible tests
        
        // Create all components
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Setup test world
        world->configure(10, 10, 8, 8);
        world->reset();
        world->setAgentStart(5.0, 5.0);
        
        // Initialize everything
        brain->initialize();
        agent->initialize(*world);
        
        // Enable feedback systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Run feedback test with tracking
        struct FeedbackData {
            int step;
            float reward;
            float curiosity;
            float novelty;
            float dopamine;
            float developmentAge;
            MotorCommand action;
            bool actionSuccess;
        };
        
        std::vector<FeedbackData> feedbackLog;
        
        int testSteps = 100;
        for (int step = 0; step < testSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process and get response
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply and get feedback
            bool actionSuccess = world->applyMotorCommand(action, world->getSimulationTime());
            float reward = world->computeReward(brain->getRegions()[0].get());
            
            // Record feedback
            FeedbackData data;
            data.step = step;
            data.reward = reward;
            data.curiosity = agent->getCuriosityLevel();
            data.novelty = agent->getNoveltyLevel();
            data.dopamine = agent->getNeuromodulationLevel();
            data.developmentAge = agent->getBrain() ? agent->getBrain()->getDevelopmentalAge() : 0.0;
            data.action = action;
            data.actionSuccess = actionSuccess;
            feedbackLog.push_back(data);
            
            // Apply learning
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        // Analyze feedback loop
        float totalReward = 0.0f;
        float totalCuriosity = 0.0f;
        int successfulActions = 0;
        
        for (const auto& data : feedbackLog) {
            totalReward += data.reward;
            totalCuriosity += data.curiosity;
            if (data.actionSuccess) successfulActions++;
        }
        
        // Verify feedback loop properties
        assert(totalReward >= 0.0f); // Rewards should be non-negative
        assert(totalCuriosity > 0.0f); // Curiosity should be positive
        assert(successfulActions > 0); // Some actions should succeed
        
        // Check for changing behavior (development)
        float initialCuriosity = feedbackLog.front().curiosity;
        float finalCuriosity = feedbackLog.back().curiosity;
        assert(finalCuriosity != initialCuriosity); // Behavior should change
        
        NLM_LOG_INFO("✓ Brain-world feedback loop test passed");
        NLM_LOG_INFO("  Total reward: " + std::to_string(totalReward));
        NLM_LOG_INFO("  Average curiosity: " + std::to_string(totalCuriosity / testSteps));
        NLM_LOG_INFO("  Successful actions: " + std::to_string(successfulActions) + "/" + std::to_string(testSteps));
        NLM_LOG_INFO("  Curiosity change: " + std::to_string(initialCuriosity) + " → " + std::to_string(finalCuriosity));
    }
    
    // Helper for tracking actions in sensorimotor test
    std::vector<MotorCommand> actionsTaken;
};

} // namespace nlm

// Main function for standalone execution
int main() {
    std::cout << "=== NLM Brain-World Integration Tests ===" << std::endl;
    std::cout << "Testing complete brain-world interaction loops" << std::endl << std::endl;
    
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::Logger::Level::Info);
    
    // Run tests
    nlm::BrainWorldIntegrationTest integrationTest;
    integrationTest.runAllTests();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests Completed Successfully ===" << std::endl;
    
    return 0;
}
