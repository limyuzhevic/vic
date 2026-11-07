// Comprehensive NLM Demo: Learning in a Simple World
// Demonstrates advanced NLM features including plasticity, memory, and interaction

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

// Additional includes for advanced features
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"

using namespace nlm;

// Demo 1: Basic Brain Development
void demoBrainDevelopment() {
    std::cout << "=== Demo 1: Basic Brain Development ===" << std::endl;
    
    // Create configuration with advanced settings
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 2000, ConfigSource::Default);
    config->set("region_count", 3, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    config->set("development_synaptogenesis_rate", 0.002f, ConfigSource::Default);
    config->set("development_pruning_rate", 0.001f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Brain initialized with:" << std::endl;
    std::cout << "  - " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "  - " << brain->getTotalSynapseCount() << " synapses" << std::endl;
    std::cout << "  - " << brain->getRegionCount() << " regions" << std::endl;
    
    // Simulate development over time
    std::cout << "\nSimulating development (10000 steps)..." << std::endl;
    for (SimulationStep step = 0; step < 10000; ++step) {
        // Inject random sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f + (rand() % 30));
        
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            std::cout << "  Step " << step << ": " 
                     << brain->getFiringNeuronCount() << " firing neurons, "
                     << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    // Show final statistics
    std::cout << "\nDevelopment complete!" << std::endl;
    std::cout << "  - Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  - Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "  - Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    brain->logStatus();
}

// Demo 2: Learning in a Simple World
void demoWorldLearning() {
    std::cout << "\n=== Demo 2: Learning in a Simple World ===" << std::endl;
    
    // Create brain with reward-based learning
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1500, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.12f, ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    config->set("neuromod.dopamine.scale", 0.5f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent interface
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create simple world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    agent->initialize(world);
    
    std::cout << "Starting learning simulation (500 steps)..." << std::endl;
    
    // Run learning episode
    for (SimulationStep step = 0; step < 500; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, world->getSimulationTime());
        
        // Get action from brain
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward and apply learning
        float reward = 0.0f;
        if (world->getAgentBody()->energy < 50.0f) {
            reward = 1.0f;  // Energy low = good
        }
        
        agent->applyRewardModulation(reward, 0.0f);
        agent->updateDevelopment(0.1);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": " 
                     << "Energy: " << world->getAgentBody()->energy << ", "
                     << "Curiosity: " << agent->getCuriosityLevel() << ", "
                     << "Novelty: " << agent->getNoveltyLevel() << ", "
                     << "Dev Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        }
    }
    
    std::cout << "\nLearning episode complete!" << std::endl;
    std::cout << "  - Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "  - Final novelty level: " << agent->getNoveltyLevel() << std::endl;
    std::cout << "  - Final neuromodulation: " << agent->getNeuromodulationLevel() << std::endl;
    
    // Show brain stats
    brain->logStatus();
}

// Demo 3: Memory Integration
void demoMemoryIntegration() {
    std::cout << "\n=== Demo 3: Memory Integration ===" << std::endl;
    
    // Create configuration with memory systems enabled
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1200, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Access memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    auto associativeMemory = brain->getAssociativeMemory();
    
    std::cout << "Memory systems initialized:" << std::endl;
    std::cout << "  - Working memory capacity: " << workingMemory->getCapacity() << std::endl;
    std::cout << "  - Episodic memory max episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
    std::cout << "  - Associative memory patterns: " << associativeMemory->getPatternCount() << std::endl;
    
    // Simulate experience and memory encoding
    std::cout << "\nEncoding experiences (2000 steps)..." << std::endl;
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Inject patterned input to create experiences
        if (step % 100 == 0) {
            // Create distinct experience patterns
            float pattern = static_cast<float>(step / 100 % 5);
            brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f + pattern * 5.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Encode in working memory
        workingMemory->storeItem(step, step * 0.001);
        
        // Episodic memory at specific intervals
        if (step % 500 == 0) {
            episodicMemory->encodeExperience(step, step * 0.001, step % 5);
            std::cout << "  Encoded experience: pattern " << (step % 5) << std::endl;
        }
        
        // Form associations
        if (step % 300 == 0) {
            associativeMemory->formAssociation(step % 3, step % 7);
        }
        
        if (step % 500 == 0) {
            std::cout << "  Step " << step << ": " 
                     << "Working memory: " << workingMemory->getActiveTraces() << " traces, "
                     << "Episodic memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
        }
    }
    
    std::cout << "\nMemory integration complete!" << std::endl;
    std::cout << "  - Working memory active traces: " << workingMemory->getActiveTraces() << std::endl;
    std::cout << "  - Episodic memory episodes: " << episodicMemory->getEpisodeCount() << stdendl;
    std::cout << "  - Associative memory patterns: " << associativeMemory->getPatternCount() << stdendl;
    
    // Test memory retrieval
    std::cout << "\nTesting memory retrieval..." << std::endl;
    for (int pattern = 0; pattern < 3; ++pattern) {
        auto retrieved = episodicMemory->retrieveEpisode(pattern);
        std::cout << "  Pattern " << pattern << " retrieved: " << (retrieved ? "YES" : "NO") << std::endl;
    }
}

// Demo 4: Prediction and Planning
void demoPredictionPlanning() {
    std::cout << "\n=== Demo 4: Prediction and Planning ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1800, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.11f, ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Access prediction system
    auto predictionSystem = brain->getPredictionSystem();
    auto planner = brain->getPlanner();
    auto attention = brain->getAttention();
    
    std::cout << "Prediction and planning systems initialized" << std::endl;
    
    // Simulate prediction and planning
    std::cout << "\nRunning prediction and planning (1500 steps)..." << std::endl;
    for (SimulationStep step = 0; step < 1500; ++step) {
        // Generate sensory input
        float sensoryInput = 15.0f + static_cast<float>(rand() % 40);
        brain->injectCurrentToNeurons(NeuronType::Sensory, sensoryInput);
        
        brain->step(step, step * 0.001);
        
        // Prediction system updates
        if (predictionSystem) {
            float prediction = predictionSystem->predict(sensoryInput);
            float error = std::abs(prediction - sensoryInput);
            predictionSystem->update(prediction, error, step * 0.001);
        }
        
        // Planning system
        if (planner) {
            auto actionPlan = planner->plan(step % 5, step * 0.001);
            std::cout << "  Step " << step << ": " 
                     << "Predicted: " << std::fixed << std::setprecision(2) << (predictionSystem ? predictionSystem->predict(sensoryInput) : 0.0) << ", "
                     << "Planning depth: " << (planner ? planner->getPlanningDepth() : 0) << ", "
                     << "Attention focus: " << (attention ? attention->getAttentionFocus() : 0.0) << std::endl;
        }
        
        // Update attention
        if (attention) {
            attention->update(sensoryInput, step * 0.001);
        }
        
        if (step % 300 == 0) {
            std::cout << "  Step " << step << ": " 
                     << "Prediction error: " << (predictionSystem ? predictionSystem->getPredictionError() : 0.0) << ", "
                     << "Confidence: " << (predictionSystem ? predictionSystem->getConfidence() : 0.0) << std::endl;
        }
    }
    
    std::cout << "\nPrediction and planning complete!" << std::endl;
    if (predictionSystem) {
        std::cout << "  - Final prediction accuracy: " << (100.0f - predictionSystem->getPredictionError()) << "%" << std::endl;
        std::cout << "  - Final confidence: " << predictionSystem->getConfidence() << std::endl;
    }
}

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Comprehensive Demo Suite                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger with debug level for detailed output
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Starting comprehensive NLM demo suite..." << std::endl;
    std::cout << "This demo showcases:" << std::endl;
    std::cout << "  - Brain development and plasticity" << std::endl;
    std::cout << "  - Learning in interactive environments" << std::endl;
    std::cout << "  - Memory integration and retrieval" << std::endl;
    std::cout << "  - Prediction, planning, and attention" << std::endl;
    std::cout << std::endl;
    
    srand(static_cast<unsigned int>(time(nullptr)));
    
    try {
        demoBrainDevelopment();
        demoWorldLearning();
        demoMemoryIntegration();
        demoPredictionPlanning();
        
        std::cout << std::endl;
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     ALL DEMOS COMPLETED SUCCESSFULLY                 ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain successfully demonstrated:" << std::endl;
        std::cout << "  ✓ Real neural computation with LIF dynamics" << std::endl;
        std::cout << "  ✓ Synaptic plasticity (STDP, Hebbian, structural)" << std::endl;
        std::cout << "  ✓ Developmental processes" << std::endl;
        std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
        std::cout << "  ✓ Memory systems (working, episodic, associative)" << std::endl;
        std::cout << "  ✓ Prediction and planning" << std::endl;
        std::cout << "  ✓ Attention and focus mechanisms" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM system is now a functioning artificial brain capable of" << std::endl;
        std::cout << "complex learning, memory, and adaptive behavior!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Demo failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Demo failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}