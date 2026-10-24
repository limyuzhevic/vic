#include "AgentBrainImproved.hpp"
#include "SensoryPerceptImproved.hpp"
#include "AdvancedAgentBrainSystems.hpp"
#include "tests/AgentSystemTestFramework.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>

void demonstrateImprovedAgentBrain() {
    std::cout << "=== Demonstrating Improved AgentBrain System ===" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create a simple brain mock for demonstration
    auto config = std::make_shared<Config>();
    
    // Create advanced agent brain with default configuration
    auto brain = std::make_shared<Brain>(config);
    AdvancedAgentBrain agent(brain);
    
    std::cout << "1. AgentBrain Configuration:" << std::endl;
    std::cout << "   Motor groups: " << agent.getConfig().numMotorGroups << std::endl;
    std::cout << "   Sensory groups: " << agent.getConfig().numSensoryGroups << std::endl;
    std::cout << "   Curiosity enabled: " << (agent.getConfig().curiosityEnabled ? "Yes" : "No") << std::endl;
    std::cout << "   Reward modulation enabled: " << (agent.getConfig().rewardModulationEnabled ? "Yes" : "No") << std::endl;
    std::cout << "   Development enabled: " << (agent.getConfig().developmentEnabled ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Create and configure sensory percept
    SensoryPerceptConfig perceptConfig = SensoryPerceptConfig::getDefaultConfig();
    perceptConfig.visionWidth = 32;
    perceptConfig.visionHeight = 32;
    perceptConfig.enableTimestamps = true;
    
    SensoryPercept percept(perceptConfig);
    
    // Set some sample sensory data
    std::vector<float> visionData(32 * 32, 0.0f);
    for (size_t i = 0; i < 32 * 32; ++i) {
        visionData[i] = static_cast<float>(rand()) / RAND_MAX; // Random vision data
    }
    percept.setVision(32, 32, visionData);
    
    // Set other sensory data
    std::vector<float> touchData(12, 0.0f);
    std::vector<float> internalData(4, 0.5f);
    std::vector<float> proprioData(6, 0.0f);
    
    for (size_t i = 0; i < 12; ++i) touchData[i] = static_cast<float>(rand()) / RAND_MAX;
    for (size_t i = 0; i < 6; ++i) proprioData[i] = static_cast<float>(rand()) / RAND_MAX - 0.5f;
    
    percept.setTouch(touchData);
    percept.setInternal(internalData);
    percept.setProprioception(proprioData);
    
    std::cout << "2. SensoryPercept Configuration:" << std::endl;
    std::cout << "   Vision size: " << perceptConfig.visionWidth << "x" << perceptConfig.visionHeight << std::endl;
    std::cout << "   Touch sensors: " << perceptConfig.touchCount << std::endl;
    std::cout << "   Internal signals: " << perceptConfig.internalSignalCount << std::endl;
    std::cout << "   Proprioception signals: " << perceptConfig.proprioceptionCount << std::endl;
    std::cout << "   Audio enabled: " << (perceptConfig.audioEnabled ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Demonstrate sensory signal processing
    std::cout << "3. Sensory Signal Processing:" << std::endl;
    std::vector<float> allSignals = percept.getAllSignals();
    std::cout << "   Total signals processed: " << allSignals.size() << std::endl;
    
    // Demonstrate signal statistics
    percept.logSignalStats();
    std::cout << std::endl;
    
    // Demonstrate advanced motor decoding
    std::cout << "4. Advanced Motor Decoding:" << std::endl;
    
    // Update agent with sensory input
    agent.processSensoryInput(percept);
    
    // Get agent's state
    auto state = agent.getState();
    std::cout << "   State analysis:" << std::endl;
    std::cout << "     - Curiosity level: " << state.curiosityLevel << std::endl;
    std::cout << "     - Novelty level: " << state.noveltyLevel << std::endl;
    std::cout << "     - Prediction error: " << state.predictionError << std::endl;
    std::cout << "     - Dopamine level: " << state.dopamineLevel << std::endl;
    std::cout << "     - Developmental stage: " << static_cast<int>(state.developmentalStage) << std::endl;
    std::cout << "     - Exploration state: " << state.explorationState << std::endl;
    std::cout << "     - Plasticity modifier: " << state.plasticityModifier << std::endl;
    std::cout << "     - Sensory input size: " << state.sensoryInputSize << std::endl;
    std::cout << "     - Motor output size: " << state.motorOutputSize << std::endl;
    std::cout << std::endl;
    
    // Demonstrate curiosity-based exploration
    std::cout << "5. Curiosity-Based Exploration:" << std::endl;
    float curiosity = agent.getCuriosityLevel();
    std::cout << "   Current curiosity level: " << curiosity << std::endl;
    
    if (curiosity > agent.getConfig().curiosityThreshold) {
        std::cout << "   Curiosity threshold exceeded - exploration likely" << std::endl;
    } else {
        std::cout << "   Curiosity below threshold - standard behavior" << std::endl;
    }
    std::cout << std::endl;
    
    // Demonstrate reward modulation
    std::cout << "6. Reward Modulation System:" << std::endl;
    agent.applyRewardModulation(0.5f, 0.3f); // Reward=0.5, predicted=0.3
    std::cout << "   Applied reward modulation with prediction error: " << agent.getPredictionError() << std::endl;
    std::cout << "   Dopamine signal: " << agent.getNeuromodulationLevel() << std::endl;
    std::cout << std::endl;
    
    // Demonstrate development system
    std::cout << "7. Development System:" << std::endl;
    agent.updateDevelopment(10.0); // Update with 10 time units
    std::cout << "   Updated development with 10 time units" << std::endl;
    std::cout << "   Current developmental stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
    std::cout << "   Current plasticity modifier: " << agent.getConfig().plasticityModifiers[0] << std::endl;
    std::cout << std::endl;
    
    // Demonstrate performance analysis
    std::cout << "8. Performance Analysis:" << std::endl;
    std::string analysis = agent.getPerformanceAnalysis();
    std::cout << analysis << std::endl;
    
    std::string suggestions = agent.getOptimizationSuggestions();
    std::cout << "   Optimization suggestions: " << suggestions << std::endl;
    std::cout << std::endl;
    
    // Demonstrate configuration flexibility
    std::cout << "9. Configuration Flexibility:" << std::endl;
    std::cout << "   Original configuration:" << std::endl;
    std::cout << "     - Motor groups: " << agent.getConfig().numMotorGroups << std::endl;
    std::cout << "     - Curiosity enabled: " << (agent.getConfig().curiosityEnabled ? "Yes" : "No") << std::endl;
    std::cout << "     - Reward modulation enabled: " << (agent.getConfig().rewardModulationEnabled ? "Yes" : "No") << std::endl;
    
    // Create custom configuration
    AgentBrainConfig customConfig = AgentBrainConfig::getDefaultConfig();
    customConfig.numMotorGroups = 12;
    customConfig.curiosityEnabled = false;
    customConfig.maxExplorationChance = 0.5f;
    
    // Note: In real implementation, you would create a new agent with custom config
    std::cout << "   Custom configuration (example):" << std::endl;
    std::cout << "     - Motor groups: " << customConfig.numMotorGroups << std::endl;
    std::cout << "     - Curiosity enabled: " << (customConfig.curiosityEnabled ? "Yes" : "No") << std::endl;
    std::cout << "     - Max exploration chance: " << customConfig.maxExplorationChance << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Demonstration Complete ===" << std::endl;
    std::cout << "The improved AgentBrain system now provides:" << std::endl;
    std::cout << "✓ Configurable parameters instead of hardcoded values" << std::endl;
    std::cout << "✓ Enhanced sensory processing with dynamic dimensions" << std::endl;
    std::cout << "✓ Advanced motor decoding with noise reduction" << std::endl;
    std::cout << "✓ Multi-component curiosity system" << std::endl;
    std::cout << "✓ Improved reward modulation" << std::endl;
    std::cout << "✓ Configurable development stages" << std::endl;
    std::cout << "✓ Performance monitoring and optimization" << std::endl;
    std::cout << "✓ Integration framework for all systems" << std::endl;
    std::cout << "✓ Command-line configuration support" << std::endl;
}

int main() {
    std::cout << "NLM Agent System Demonstration" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    demonstrateImprovedAgentBrain();
    
    std::cout << std::endl;
    std::cout << "To run the comprehensive test suite:" << std::endl;
    std::cout << "  nlm::runAgentSystemTests();" << std::endl;
    std::cout << std::endl;
    
    return 0;
}