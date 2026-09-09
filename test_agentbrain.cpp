#include <iostream>
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Testing NLM AgentBrain improvements..." << std::endl;
    
    // Test 1: Basic AgentBrain creation
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
        
        std::cout << "✓ AgentBrain creation successful" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ AgentBrain creation failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 2: AgentBrain initialization
    try {
        nlm::SimpleWorld world;
        world.configure(20, 20, 8, 8);
        world.reset();
        
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(config));
        agent.initialize(world);
        
        std::cout << "✓ AgentBrain initialization successful" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ AgentBrain initialization failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 3: Sensor processing
    try {
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(config));
        nlm::SensoryPercept percept;
        
        // Set some test vision data
        std::vector<float> vision(256, 0.5f);
        percept.setVision(vision);
        
        agent.processSensoryInput(percept);
        
        std::cout << "✓ Sensory processing successful" << std::endl;
        std::cout << "  Novelty level: " << agent.getNoveltyLevel() << std::endl;
        std::cout << "  Curiosity level: " << agent.getCuriosityLevel() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Sensory processing failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 4: Motor command decoding
    try {
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(config));
        
        // Inject some activity into motor neurons to simulate movement intent
        // (This is a simplified test - in real usage, neurons would have activity)
        
        MotorCommand cmd = agent.decodeMotorCommand();
        std::cout << "✓ Motor command decoding successful" << std::endl;
        std::cout << "  Decoded command: " << static_cast<int>(cmd) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Motor command decoding failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 5: Reward modulation
    try {
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(config));
        
        agent.applyRewardModulation(1.0f, 0.5f);  // reward, predicted_reward
        
        std::cout << "✓ Reward modulation successful" << std::endl;
        std::cout << "  Neuromodulation level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "  Prediction error: " << agent.getPredictionError() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Reward modulation failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 6: Development updates
    try {
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(config));
        
        for (int i = 0; i < 10; ++i) {
            agent.updateDevelopment(0.1);  // small time steps
        }
        
        std::cout << "✓ Development updates successful" << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "  Plasticity modifier: " << agent.getPlasticityModifier() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Development updates failed: " << e.what() << std::endl;
        return 1;
    }
    
    // Test 7: Reset functionality
    try {
        nlm::AgentBrain agent(std::make_shared<nlm::Brain>(config));
        
        agent.reset();
        
        // Verify reset state
        assert(agent.getNeuromodulationLevel() == 0.0f);
        assert(agent.getNoveltyLevel() == 0.0f);
        assert(agent.getCuriosityLevel() == 0.0f);
        assert(agent.getPredictionError() == 0.0f);
        
        std::cout << "✓ Reset functionality successful" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Reset functionality failed: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nAll AgentBrain tests passed!" << std::endl;
    return 0;
}
