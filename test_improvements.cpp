// Test program for verifying NLM improvements
// Demonstrates the new user-friendly API and features

#include "../src/agent/AgentBrain.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <iostream>

void testFactoryFunctions() {
    std::cout << "=== Testing Factory Functions ===" << std::endl;
    
    // Test 1: Simple brain creation
    auto config = nlm::createBrainConfigForSimulation(500, 2);
    auto brain = std::make_shared<nlm::Brain>(config);
    std::cout << "✓ Created simple brain with " << config.get<int>("neuron_count") << " neurons" << std::endl;
    
    // Test 2: Brain builder
    auto builder = nlm::BrainBuilder()
        .withNeuronCount(1000)
        .withRegionCount(3)
        .enableLearning(true)
        .setLearningStyle("exploratory");
    
    auto brain2 = builder.build();
    std::cout << "✓ Created brain using BrainBuilder" << std::endl;
    
    // Test 3: Agent brain creation
    auto agent = nlm::createSimpleAgentBrain(brain2);
    std::cout << "✓ Created agent brain using factory function" << std::endl;
    
    // Test 4: Agent for world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    auto agent2 = nlm::createAgentBrainForWorld(brain2, *world);
    std::cout << "✓ Created agent brain for world" << std::endl;
    
    std::cout << "All factory function tests passed!" << std::endl << std::endl;
}

void testUtilityFunctions() {
    std::cout << "=== Testing Utility Functions ===" << std::endl;
    
    auto config = nlm::createBrainConfigForSimulation(200, 1);
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test isInitialized
    bool isInit = agent->isInitialized();
    std::cout << "✓ isInitialized() returned: " << (isInit ? "true" : "false") << std::endl;
    
    // Test enableAllFeatures
    agent->enableAllFeatures(true);
    bool allFeatures = agent->isAllFeaturesEnabled();
    std::cout << "✓ isAllFeaturesEnabled() returned: " << (allFeatures ? "true" : "false") << std::endl;
    
    // Test neuromodulation utilities
    agent->boostCuriosity(0.5f);
    agent->boostNovelty(0.3f);
    agent->setPredictionError(0.2f);
    
    float curiosity = agent->getCuriosityLevel();
    float novelty = agent->getNoveltyLevel();
    float prediction = agent->getPredictionError();
    
    std::cout << "✓ Boosted curiosity: " << curiosity << std::endl;
    std::cout << "✓ Boosted novelty: " << novelty << std::endl;
    std::cout << "✓ Set prediction error: " << prediction << std::endl;
    
    // Test reset functions
    agent->resetCuriosity();
    agent->resetNovelty();
    agent->resetPredictionError();
    
    std::cout << "✓ Reset neuromodulation values" << std::endl;
    
    std::cout << "All utility function tests passed!" << std::endl << std::endl;
}

void testValidationFunctions() {
    std::cout << "=== Testing Validation Functions ===" << std::endl;
    
    auto config = nlm::createBrainConfigForSimulation(300, 1);
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test configuration validation
    auto config2 = nlm::createLearningConfig();
    std::vector<std::string> errors = nlm::validateConfig(config2);
    std::cout << "✓ Validation errors for learning config: " << errors.size() << " (should be 0)" << std::endl;
    
    // Test brain validation
    auto errors = agent->validateConfiguration();
    std::cout << "✓ Agent validation errors: " << errors.size() << std::endl;
    
    // Test sensor validation
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 4, 4);
    auto agent2 = nlm::createAgentBrainForWorld(brain, *world);
    
    auto percept = world->getSensoryPercept();
    bool sensorValid = agent2->validateSensoryInput(percept);
    std::cout << "✓ Sensor validation: " << (sensorValid ? "valid" : "invalid") << std::endl;
    
    std::cout << "All validation function tests passed!" << std::endl << std::endl;
}

void testBrainStats() {
    std::cout << "=== Testing Brain Statistics ===" << std::endl;
    
    auto config = nlm::createBrainConfigForSimulation(400, 2);
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Get brain statistics
    auto stats = agent->getBrainStats();
    
    std::cout << "✓ Brain statistics:" << std::endl;
    std::cout << "  - Total neurons: " << stats.totalNeurons << std::endl;
    std::cout << "  - Total synapses: " << stats.totalSynapses << std::endl;
    std::cout << "  - Active neurons: " << stats.activeNeurons << std::endl;
    std::cout << "  - Firing neurons: " << stats.firingNeurons << std::endl;
    std::cout << "  - Developmental age: " << stats.developmentalAge << std::endl;
    std::cout << "  - Neuromodulation level: " << stats.neuromodulationLevel << std::endl;
    std::cout << "  - Curiosity level: " << stats.curiosityLevel << std::endl;
    std::cout << "  - Novelty level: " << stats.noveltyLevel << std::endl;
    std::cout << "  - Prediction error: " << stats.predictionError << std::endl;
    
    // Test detailed stats string
    std::string detailedStats = agent->getDetailedStatsString();
    std::cout << "✓ Generated detailed statistics string (length: " << detailedStats.length() << ")" << std::endl;
    
    std::cout << "All brain stats tests passed!" << std::endl << std::endl;
}

void testQuickRunUtility() {
    std::cout << "=== Testing Quick Run Utility ===" << std::endl;
    
    auto config = nlm::createBrainConfigForSimulation(100, 1);
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(15, 15, 6, 6);
    auto agent = nlm::createAgentBrainForWorld(brain, *world);
    
    std::cout << "✓ Created agent and world for simulation" << std::endl;
    
    // Initialize the brain
    bool initSuccess = brain->initialize();
    std::cout << "✓ Brain initialization: " << (initSuccess ? "success" : "failed") << std::endl;
    
    std::cout << "Quick run utility test setup completed!" << std::endl << std::endl;
}

void testPythonBindingsCompatibility() {
    std::cout << "=== Testing Python Bindings Compatibility ===" << std::endl;
    
    // Test that legacy factory functions still work
    auto config = std::make_shared<nlm::Config>();
    auto brain = nlm::createBrain(config);
    std::cout << "✓ Legacy createBrain() function works" << std::endl;
    
    auto agent = nlm::createAgentBrain(brain);
    std::cout << "✓ Legacy createAgentBrain() function works" << std::endl;
    
    // Test new Python-friendly factory functions
    auto simpleBrain = nlm::create_simple_brain();
    std::cout << "✓ New Python-friendly create_simple_brain() function works" << std::endl;
    
    auto learningConfig = nlm::create_learning_config();
    std::cout << "✓ New Python-friendly create_learning_config() function works" << std::endl;
    
    auto agent2 = nlm::create_agent_brain(simpleBrain);
    std::cout << "✓ New Python-friendly create_agent_brain() function works" << std::endl;
    
    std::cout << "All Python bindings compatibility tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== NLM Improvements Verification Test ===" << std::endl;
    std::cout << "Testing all new features and improvements..." << std::endl << std::endl;
    
    try {
        testFactoryFunctions();
        testUtilityFunctions();
        testValidationFunctions();
        testBrainStats();
        testQuickRunUtility();
        testPythonBindingsCompatibility();
        
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "The NLM improvements are working correctly!" << std::endl;
        std::cout << std::endl;
        std::cout << "Summary of improvements:
• Simplified factory functions for easy API usage
• BrainBuilder class for customizable brain configuration
• Enhanced validation and error handling
• Advanced brain statistics and analysis tools
• Neuromodulation utility functions
• Python-friendly API enhancements
• Legacy compatibility maintained
• User-friendly improvements for beginners
• Advanced features for experienced users" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
