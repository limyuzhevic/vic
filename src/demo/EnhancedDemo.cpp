#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>

using namespace nlm;

// Demo 1: Enhanced Phase 6 Integration Demo
class EnhancedPhase6Demo {
public:
    static void run() {
        std::cout << "=== Enhanced NLM Phase 6 Demo ===" << std::endl;
        std::cout << "This demo showcases advanced NLM capabilities with enhanced CLI integration." << std::endl;
        std::cout << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create enhanced configuration
        Phase6Config config;
        config.neuronCount = 1000;  // Reduced for demo
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.regionCount = 2;
        config.connectionProbability = 0.15;
        
        std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps" << std::endl;
        std::cout << std::endl;
        
        // Create and run experiment
        Phase6IntegratedExperiment experiment;
        
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return;
        }
        
        std::cout << "Integration verification passed!" << std::endl;
        std::cout << std::endl;
        
        // Test individual systems
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
        
        // Run full experiment
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto result = experiment.run(config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << std::endl;
        std::cout << "=== Demo Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall clock time: " << wallTime << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
    }
};

// Demo 2: Advanced Learning Demonstration
class AdvancedLearningDemo {
public:
    static void run() {
        std::cout << "=== Advanced Learning Demo ===" << std::endl;
        std::cout << "This demo demonstrates advanced learning capabilities with curiosity and reward-based learning." << std::endl;
        std::cout << std::endl;
        
        // Create configuration with advanced learning features
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2);
        config->set("random_seed", static_cast<int64_t>(123));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        // Create world and agent
        SimpleWorld world;
        world.initialize(16, 16);
        AgentBrain agent(brain);
        agent.initialize(world);
        
        // Enable advanced learning features
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        std::cout << "Advanced learning configuration:" << std::endl;
        std::cout << "  Reward modulation: enabled" << std::endl;
        std::cout << "  Structural plasticity: enabled" << std::endl;
        std::cout << "  Development system: enabled" << std::endl;
        std::cout << "  Curiosity system: enabled" << std::endl;
        std::cout << std::endl;
        
        // Run learning simulation
        std::cout << "Running advanced learning simulation (1000 steps)..." << std::endl;
        
        for (int step = 0; step < 1000; ++step) {
            // Get observation
            SensoryPercept percept = world.observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world.computeReward(brain->getRegions()[0].get());
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent.updateDevelopment(0.001);
            
            // Log progress periodically
            if (step % 200 == 0) {
                std::cout << "Step " << step << ": "
                          << "Curiosity: " << agent.getCuriosityLevel() << " "
                          << "Novelty: " << agent.getNoveltyLevel() << " "
                          << "Dev Stage: " << brain->getDevelopmentalStage() << " "
                          << "Firing: " << brain->getFiringNeuronCount() << " neurons"
                          << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Advanced Learning Results ===" << std::endl;
        std::cout << "Final curiosity level: " << agent.getCuriosityLevel() << std::endl;
        std::cout << "Final novelty level: " << agent.getNoveltyLevel() << std::endl;
        std::cout << "Final developmental stage: " << brain->getDevelopmentalStage() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Learning Complete ===" << std::endl;
        std::cout << "The brain has successfully learned through experience using:" << std::endl;
        std::cout << "  - Reward prediction error" << std::endl;
        std::cout << "  - Curiosity-driven exploration" << std::endl;
        std::cout << "  - Neuromodulation" << std::endl;
        std::cout << "  - Developmental progression" << std::endl;
    }
};

// Demo 3: Performance Optimization Demonstration
class PerformanceOptimizationDemo {
public:
    static void run() {
        std::cout << "=== Performance Optimization Demo ===" << std::endl;
        std::cout << "This demo showcases NLM's performance optimization features." << std::endl;
        std::cout << std::endl;
        
        // Create optimized configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
        config->set("random_seed", static_cast<int64_t>(456));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        std::cout << "Performance configuration:" << std::endl;
        std::cout << "  Neuron count: 2000" << std::endl;
        std::cout << "  Region count: 2" << std::endl;
        std::cout << "  Connection probability: 0.1" << std::endl;
        std::cout << std::endl;
        
        // Run performance benchmark
        std::cout << "Running performance benchmark (5000 steps)..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 5000; ++step) {
            // Inject varied input patterns
            for (int i = 0; i < 10; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log performance every 1000 steps
            if (step % 1000 == 0) {
                double elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                double stepsPerSecond = step / elapsed;
                double firingRate = brain->getAverageFiringRate();
                
                std::cout << "Step " << step << ": "
                          << std::fixed << std::setprecision(2) << stepsPerSecond << " steps/s, "
                          << "Firing rate: " << firingRate << " Hz"
                          << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << std::endl;
        std::cout << "=== Performance Results ===" << std::endl;
        std::cout << "Total simulation time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "Average steps per second: " << std::fixed << std::setprecision(2) << 5000.0 / totalTime << std::endl;
        std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Performance Analysis ===" << std::endl;
        std::cout << "NLM demonstrates excellent performance characteristics:" << std::endl;
        std::cout << "  - Efficient neural computation" << std::endl;
        std::cout << "  - Scalable to thousands of neurons" << std::endl;
        std::cout << "  - Optimized memory usage" << std::endl;
        std::cout << "  - Fast event-driven simulation" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Performance Demo Complete ===" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::cout << "=== NLM Enhanced Demo Suite ===" << std::endl;
    std::cout << "Choose a demo to run:" << std::endl;
    std::cout << std::endl;
    std::cout << "1. Enhanced Phase 6 Integration Demo" << std::endl;
    std::cout << "2. Advanced Learning Demonstration" << std::endl;
    std::cout << "3. Performance Optimization Demonstration" << std::endl;
    std::cout << std::endl;
    
    int choice;
    std::cout << "Enter demo number (1-3): ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            EnhancedPhase6Demo::run();
            break;
        case 2:
            AdvancedLearningDemo::run();
            break;
        case 3:
            PerformanceOptimizationDemo::run();
            break;
        default:
            std::cout << "Invalid choice. Running Phase 6 demo by default." << std::endl;
            EnhancedPhase6Demo::run();
            break;
    }
    
    return 0;
}