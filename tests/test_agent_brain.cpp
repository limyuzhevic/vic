#include <iostream>
#include <memory>
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== Agent Brain Advanced Features Test ===\n" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->setDefaultValues();
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create AgentBrain with advanced features
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    
    // Initialize with simple world
    class TestWorld {
    public:
        size_t getVisionWidth() const { return 16; }
        size_t getVisionHeight() const { return 16; }
    } world;
    
    agentBrain->initialize(world);
    
    // Test advanced features
    std::cout << "\n1. Testing Agent Control:\n";
    std::cout << "   Initial state: Running = " << agentBrain->isRunning() 
              << ", Paused = " << agentBrain->isPaused() 
              << ", Stopped = " << agentBrain->isStopped() << std::endl;
    
    agentBrain->start();
    std::cout << "   After start: Running = " << agentBrain->isRunning() 
              << ", Paused = " << agentBrain->isPaused() 
              << ", Stopped = " << agentBrain->isStopped() << std::endl;
    
    agentBrain->pause();
    std::cout << "   After pause: Running = " << agentBrain->isRunning() 
              << ", Paused = " << agentBrain->isPaused() 
              << ", Stopped = " << agentBrain->isStopped() << std::endl;
    
    agentBrain->resume();
    std::cout << "   After resume: Running = " << agentBrain->isRunning() 
              << ", Paused = " << agentBrain->isPaused() 
              << ", Stopped = " << agentBrain->isStopped() << std::endl;
    
    agentBrain->stop();
    std::cout << "   After stop: Running = " << agentBrain->isRunning() 
              << ", Paused = " << agentBrain->isPaused() 
              << ", Stopped = " << agentBrain->isStopped() << std::endl;
    
    std::cout << "\n2. Testing Learning Configuration:\n";
    agentBrain->start();
    std::cout << "   Learning enabled: " << agentBrain->isLearningEnabled() << std::endl;
    std::cout << "   Learning rate: " << agentBrain->getLearningRate() << std::endl;
    std::cout << "   Exploration rate: " << agentBrain->getExplorationRate() << std::endl;
    
    agentBrain->setLearningEnabled(false);
    agentBrain->setLearningRate(0.05f);
    agentBrain->setExplorationRate(0.2f);
    
    std::cout << "   After config: Learning enabled = " << agentBrain->isLearningEnabled() 
              << ", Learning rate = " << agentBrain->getLearningRate() 
              << ", Exploration rate = " << agentBrain->getExplorationRate() << std::endl;
    
    std::cout << "\n3. Testing State Management:\n";
    // Create test checkpoint file
    std::string checkpointFile = "/tmp/agent_checkpoint_test.dat";
    
    // Checkpoint the agent state
    agentBrain->checkpoint(checkpointFile);
    std::cout << "   Checkpoint saved to: " << checkpointFile << std::endl;
    
    // Modify agent state
    agentBrain->setLearningEnabled(true);
    agentBrain->setExplorationRate(0.3f);
    
    // Restore from checkpoint
    agentBrain->restore(checkpointFile);
    std::cout << "   State restored from checkpoint";
    std::cout << "   (Learning should be back to original settings)" << std::endl;
    
    std::cout << "\n4. Testing Performance Monitoring:\n";
    agentBrain->enablePerformanceMonitoring(true);
    std::cout << "   Performance monitoring: " << "enabled" << std::endl;
    
    // Generate some statistics
    agentBrain->start();
    agentBrain->processSensoryInput(TestSensoryPercept{});
    
    std::cout << "   Agent statistics:\n";
    auto stats = agentBrain->collectStats();
    std::cout << "     Elapsed time: " << stats.elapsedTime << "s" << std::endl;
    std::cout << "     Steps processed: " << stats.stepsProcessed << std::endl;
    std::cout << "     Average firing rate: " << stats.averageFiringRate << "Hz" << std::endl;
    std::cout << "     Synaptic stability: " << stats.synapticStability << std::endl;
    std::cout << "     Learning progress: " << stats.learningProgress << std::endl;
    std::cout << "     Performance score: " << stats.performanceScore << std::endl;
    std::cout << "     Memory usage: " << stats.memoryUsage << " bytes" << std::endl;
    std::cout << "     Total spikes: " << stats.totalSpikes << std::endl;
    
    std::cout << "\n5. Testing Command Processing:\n";
    agentBrain->start();
    std::cout << "   Testing command 'status':\n";
    agentBrain->processCommand("status", "");
    
    std::cout << "\n6. Testing Agent Command Help:\n";
    std::cout << agentBrain->getAgentCommandHelp();
    
    std::cout << "\n=== Agent Brain Advanced Features Test Complete ===\n";
    return 0;
}

// Simple dummy struct for testing
struct TestSensoryPercept {
    std::vector<float> getVision() const { return std::vector<float>(256, 0.5f); }
    std::vector<float> getTouch() const { return std::vector<float>(8, 0.0f); }
    std::vector<float> getInternal() const { return std::vector<float>(4, 0.0f); }
    std::vector<float> getProprioception() const { return std::vector<float>(6, 0.0f); }
};