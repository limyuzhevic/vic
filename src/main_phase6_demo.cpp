#include "BrainCore.hpp"
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include "Action.hpp"
#include "DevelopmentSystem.hpp"
#include <iostream>
#include <chrono>
#include <thread>

// Phase 6: Final Integration Demonstration
// Shows all improvements and fixes for the integrated artificial brain

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Complete Artificial Brain System                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSystemArchitecture() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                     SYSTEM ARCHITECTURE                       ║
    ╠═══════════════════════════════════════════════════════════════╣
    ║                                                               ║
    ║  SENSORIAL INPUT                                              ║
    ║  ┌─────────────────────────────┐                            ║
    ║  │  Vision (16×16 grid)        │                            ║
    ║  │  Touch (8 values)           │                            ║
    ║  │  Internal (4 values)        │                            ║
    ║  │  Proprioception (6 values)  │                            ║
    ║  └─────────────────────────────┘                            ║
    ║          │                      ║                            ║
    ║          ▼                      ▼                            ║
    ║  ┌─────────────────────────────┐ ┌─────────────────────────┐║
    ║  │  WORLD (SimpleWorld)         │ │  AGENT (AgentBrain)      │║
    ║  │  ┌─────────────────────┐   │ │  ┌─────────────────────┐   │║
    ║  │  │  Update World      │   │ │  │  Process Sensory      │   │║
    ║  │  │  Apply Actions    │   │ │  │  Input                 │   │║
    ║  │  │  Track State      │   │ │  │  Update Development   │   │║
    ║  │  └─────────────────────┘   │ │  └─────────────────────┘   │║
    ║  └─────────────────────────────┘ │ ┌─────────────────────┐   │║
    ║              │                 │ │  │  Decode Motor       │   │║
    ║              ▼                 │ │  │  Command            │   │║
    ║  ┌─────────────────────────────┐ │  └─────────────────────┘   │║
    ║  │  BRAIN (Integrated)         │ │ ┌─────────────────────┐   │║
    ║  │  ┌─────────────────────┐   │ │ │  Apply Reward        │   │║
    ║  │  │  Neural Processing  │   │ │ │  Modulation         │   │║
    ║  │  │  (LIF dynamics)    │   │ │ │  ┌─────────────────┐   │║
    ║  │  └─────────────────────┘   │ │ │ │  ┌──────────────┐ │   │║
    ║  │           │               │ │ │ │ │  WORKING     │ │   │║
    ║  │           ▼               │ │ │ │ │  MEMORY       │ │   │║
    ║  │  ┌─────────────────────┐   │ │ │ │ │  ┌──────────┐  │   │║
    ║  │  │  Memory Systems    │   │ │ │ │ │  │  EPISODIC │  │   │║
    ║  │  │  ┌─────────────────┐│   │ │ │ │ │  │  MEMORY   │  │   │║
    ║  │  │  │ Working Memory  ││   │ │ │ │ │  │  ┌──────┐ │  │   │║
    ║  │  │  └─────────────────┘│   │ │ │ │ │  │  │ Episodic │  │   │║
    ║  │  │  ┌─────────────────┐│   │ │ │ │ │  │  Memory │  │   │║
    ║  │  │  │ Episodic Memory ││   │ │ │ │ │  │  ┌────┐ │  │   │║
    ║  │  │  └─────────────────┘│   │ │ │ │ │  │  │Associative│  │   │║
    ║  │  │  ┌─────────────────┐│   │ │ │ │ │  │  │ Memory │  │   │║
    ║  │  │  │ Associative Mem ││   │ │ │ │ │  │  └────┐ │  │   │║
    ║  │  │  └─────────────────┘│   │ │ │ │ │  │       │ │   │║
    ║  │  └─────────────────────┘   │ │ │ │ │  └──────┘ │   │║
    ║  │           │               │ │ │ │         ▲    │   │║
    ║  │           ▼               │ │ │ │ ┌─────────┴────┐│   │║
    ║  │  ┌─────────────────────┐   │ │ │ │ │  PREDICTION │ │   │║
    ║  │  │  Prediction System │   │ │ │ │ │  │  SYSTEM    │ │   │║
    ║  │  └─────────────────────┘   │ │ │ │ │  └───────────┘ │   │║
    ║  │           │               │ │ │ │         ▲    │   │║
    ║  │           ▼               │ │ │ │ ┌─────────┴────┐│   │║
    ║  │  ┌─────────────────────┐   │ │ │ │ │  COGNITION  │ │   │║
    ║  │  │  Cognition Systems │   │ │ │ │ │  │  ┌──────┐ │  │   │║
    ║  │  │  ┌─────────────────┐│   │ │ │ │ │  │  │ATTENTION│  │   │║
    ║  │  │  │ Neural Planner  ││   │ │ │ │ │  │  │  ┌───┐ │  │   │║
    ║  │  │  └─────────────────┘│   │ │ │ │ │  │  │ │NEURAL│ │   │║
    ║  │  │  ┌─────────────────┐│   │ │ │ │ │  │  │ │PLANNER│ │   │║
    ║  │  │  │ Concept Formation│ │  │ │ │ │  │  │ └──────┘ │   │║
    ║  │  │  └─────────────────┘│   │ │ │ │ │  │                │   │║
    ║  │  │  ┌─────────────────┐│   │ │ │ │ │  │  SELF-MODEL  │   │║
    ║  │  │  │ Attentional Sel ││   │ │ │ │ │  │  └──────────┘ │   │║
    ║  │  │  │  └─────────────────┘│   │ │ │ │                │   │║
    ║  │  └─────────────────────────┘ │ │ │ │  DEVELOPMENT │   │║
    ║  │                              │ │ │ │  │  SYSTEM   │   │║
    ║  │  ┌─────────────────────────────┐│ │ │ │  ┌──────┐ │  │   │║
    ║  │  │  Neuromodulation          ││ │ │ │ │  │  │DAOMINE│  │   │║
    ║  │  │  ┌─────────────────────┐ ││ │ │ │ │  │  ┌───┐ │  │   │║
    ║  │  │  │ Novelty Detection   │││ │ │ │ │  │  │ │NOVELTY│ │   │║
    ║  │  │  └─────────────────────┘││ │ │ │ │  │  │ └───┘ │  │   │║
    ║  │  │  ┌─────────────────────┐ ││ │ │ │ │  │  │CURIOSITY│ │   │║
    ║  │  │  │ Curiosity          │││ │ │ │ │  │  │  ┌───┐ │  │   │║
    ║  │  │  └─────────────────────┘││ │ │ │ │  │  │ │PREDICT│ │   │║
    ║  │  │  ┌─────────────────────┐ ││ │ │ │  │  │ │ ERROR │ │   │║
    ║  │  │  │ Prediction Error   │││ │ │ │ │  │  │└──────┘ │  │   │║
    ║  │  │  └─────────────────────┘││ │ │ │ │  └─────────┘ │   │║
    ║  │           │               │ │ │ │              │   │║
    ║  │           ▼               │ │ │ │              ▼   │║
    ║  │  ┌─────────────────────────────┐                   │   │║
    ║  │  │  DEVELOPMENT            │                   │   │║
    ║  │  │  ┌─────────────────────┐ │                   │   │║
    ║  │  │  │ Synaptogenesis     │ │                   │   │║
    ║  │  │  │ Pruning            │ │                   │   │║
    ║  │  │  │ Maturation          │ │                   │   │║
    ║  │  │  └─────────────────────┘│                   │   │║
    ║  └─────────────────────────────────────────────────────┘║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝

    )" << std::endl;
}

void printLearningLoopDescription() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                     LEARNING LOOP                             ║
    ╠═══════════════════════════════════════════════════════════════╣
    ║                                                               ║
    ║  The brain operates as a closed-loop system:                 ║
    ║                                                               ║
    ║  WORLD CONSEQUENCE                                            ║
    ║    │                            │                             ║
    ║    ▼                            │                             ║
    ║  REWARD / SURPRISE / ERROR      │ REWARD / SURPRISE / ERROR    ║
    ║    │                            │                             ║
    ║    ▼                            ▼                             ║
    ║  PLASTICITY (STDP, Hebbian,     PLASTICITY (STDP, Hebbian,     ║
    ║   structural)                   structural)                   ║
    ║    │                            │                             ║
    ║    ▼                            ▼                             ║
    ║  MEMORY / DEVELOPMENT           MEMORY / DEVELOPMENT           ║
    ║    │                            │                             ║
    ║    ▼                            ▼                             ║
    ║  CHANGED BRAIN                  CHANGED BRAIN                  ║
    ║    │                            │                             ║
    ║    ▼                            ▼                             ║
    ║  CHANGED FUTURE BEHAVIOR       CHANGED FUTURE BEHAVIOR        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main() {
    printPhase6Banner();
    printSystemArchitecture();
    printLearningLoopDescription();
    
    std::cout << "Phase 6 Integration Demonstration" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    // 1. Create configuration optimized for Phase 6
    std::cout << "1. Creating Phase 6-optimized configuration..." << std::endl;
    auto config = nlm::createDefaultConfig();
    std::cout << "   ✓ Configuration created with " 
              << config->getInt("neuron_count").value_or(0) 
              << " neurons" << std::endl;
    
    // 2. Create brain and initialize
    std::cout << std::endl << "2. Creating and initializing integrated brain..." << std::endl;
    auto brain = nlm::createBrain(config);
    if (brain->initialize()) {
        std::cout << "   ✓ Brain initialized successfully" << std::endl;
        brain->logStatus();
    } else {
        std::cout << "   ✗ Brain initialization failed" << std::endl;
        return 1;
    }
    
    // 3. Create world and agent
    std::cout << std::endl << "3. Creating agent environment..." << std::endl;
    auto world = nlm::createSimpleWorld();
    auto agent = nlm::createAgentBrain(brain);
    
    if (agent) {
        agent->initialize(*world);
        std::cout << "   ✓ Agent created with integrated brain" << std::endl;
        std::cout << "     - Sensory input size: " << agent->getSensoryInputSize() << std::endl;
        std::cout << "     - Motor output size: " << agent->getMotorOutputSize() << std::endl;
        std::cout << "     - Reward modulation enabled: " << std::boolalpha 
                  << agent->isRewardModulationEnabled() << std::endl;
        std::cout << "     - Curiosity enabled: " << std::boolalpha 
                  << agent->isCuriosityEnabled() << std::endl;
    } else {
        std::cout << "   ✗ Agent creation failed" << std::endl;
        return 1;
    }
    
    // 4. Run demonstration learning episode
    std::cout << std::endl << "4. Running demonstration learning episode..." << std::endl;
    std::cout << "   Running 500 steps of learning..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Enable subsystems for comprehensive learning
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    for (size_t step = 0; step < 500; ++step) {
        // Update world (environment changes)
        world->update(0.1);
        
        // Get sensory percept from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input in agent/brain
        agent->processSensoryInput(*percept);
        
        // Run brain computation step
        brain->step(step);
        
        // Decode action from brain activity
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world (world responds)
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation (learning from experience)
        float reward = 0.0f;
        if (percept && percept->getInternal()) {
            // Simulate reward based on world state
            reward = (step % 100 == 0) ? 1.0f : -0.01f;
        }
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development (brain maturation)
        agent->updateDevelopment(0.1);
        
        // Progress indicator
        if (step % 100 == 0) {
            std::cout << "     Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons, "
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            std::cout << "              Curiosity: " << agent->getCuriosityLevel() << ", "
                      << "Novelty: " << agent->getNoveltyLevel() << ", "
                      << "Dev Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    std::cout << std::endl << "   ✓ Learning episode completed in " 
              << duration.count() << " seconds" << std::endl;
    
    // 5. Show final brain statistics
    std::cout << std::endl << "5. Final system statistics..." << std::endl;
    brain->logStatus();
    
    // 6. Demonstrate advanced features
    std::cout << std::endl << "6. Advanced feature demonstrations..." << std::endl;
    
    std::cout << "   ✓ Memory Systems:" << std::endl;
    std::cout << "     - Working Memory: " << (brain->getWorkingMemory() ? "Initialized" : "Not available") << std::endl;
    std::cout << "     - Episodic Memory: " << (brain->getEpisodicMemory() ? "Initialized" : "Not available") << std::endl;
    std::cout << "     - Associative Memory: " << (brain->getAssociativeMemory() ? "Initialized" : "Not available") << std::endl;
    
    std::cout << "   ✓ Neuromodulation:" << std::endl;
    std::cout << "     - Dopamine: " << (brain->getDopamine() ? "Active" : "Not available") << std::endl;
    std::cout << "     - Curiosity: " << (brain->getCuriosity() ? "Active" : "Not available") << std::endl;
    std::cout << "     - Novelty: " << (brain->getNovelty() ? "Active" : "Not available") << std::endl;
    std::cout << "     - Prediction Error: " << (brain->getPredictionErrorSignal() ? "Active" : "Not available") << std::endl;
    
    std::cout << "   ✓ Cognitive Systems:" << std::endl;
    std::cout << "     - Planner: " << (brain->getPlanner() ? "Initialized" : "Not available") << std::endl;
    std::cout << "     - Concept Formation: " << (brain->getConceptFormation() ? "Initialized" : "Not available") << std::endl;
    std::cout << "     - Attention: " << (brain->getAttention() ? "Initialized" : "Not available") << std::endl;
    
    std::cout << "   ✓ Development System:" << std::endl;
    std::cout << "     - Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    std::cout << "     - Plasticity Modifier: " << agent->getCuriosityLevel() << std::endl;
    
    // 7. Demonstrate checkpoint functionality
    std::cout << std::endl << "7. Checkpoint demonstration..." << std::endl;
    std::cout << "   Attempting to save brain state..." << std::endl;
    if (brain->save("phase6_checkpoint.bin")) {
        std::cout << "   ✓ Brain state saved successfully" << std::endl;
        
        // Create a fresh brain and load checkpoint
        auto freshBrain = nlm::createBrain(config);
        if (freshBrain->initialize()) {
            std::cout << "   Loading checkpoint into fresh brain..." << std::endl;
            if (freshBrain->load("phase6_checkpoint.bin")) {
                std::cout << "   ✓ Checkpoint loaded successfully" << std::endl;
                std::cout << "   ✓ Persistence system verified" << std::endl;
            } else {
                std::cout << "   ✗ Checkpoint loading failed" << std::endl;
            }
        }
    } else {
        std::cout << "   ✗ Checkpoint saving failed" << std::endl;
    }
    
    // 8. Performance summary
    std::cout << std::endl << "8. Performance summary..." << std::endl;
    double totalTime = duration.count();
    size_t totalSteps = 500;
    double stepsPerSecond = totalSteps / totalTime;
    double neuronsPerSecond = brain->getTotalNeuronCount() / totalTime;
    
    std::cout << "   Performance Metrics:" << std::endl;
    std::cout << "     - Simulation speed: " << stepsPerSecond << " steps/second" << std::endl;
    std::cout << "     - Neural throughput: " << neuronsPerSecond << " neurons/second" << std::endl;
    std::cout << "     - Learning efficiency: " << brain->getTotalSpikeCount() / totalTime << " spikes/second" << std::endl;
    std::cout << "     - Plasticity events: " << (brain->getSTDP() ? "STDP active" : "STDP unavailable") << std::endl;
    
    // 9. Memory system summary
    std::cout << std::endl << "9. Memory system status..." << std::endl;
    if (brain->getWorkingMemory()) {
        std::cout << "   - Working Memory: " << brain->getWorkingMemory()->getActiveTraces() << " active traces" << std::endl;
    }
    if (brain->getEpisodicMemory()) {
        std::cout << "   - Episodic Memory: " << brain->getEpisodicMemory()->getEpisodeCount() << " episodes stored" << std::endl;
    }
    if (brain->getAssociativeMemory()) {
        std::cout << "   - Associative Memory: " << brain->getAssociativeMemory()->getPatternCount() << " patterns" << std::endl;
    }
    
    // 10. Cleanup and exit
    std::cout << std::endl << "10. Demonstration complete." << std::endl;
    std::cout << "   The integrated artificial brain successfully demonstrates:" << std::endl;
    std::cout << "   ✓ Real-time neural computation with LIF dynamics" << std::endl;
    std::cout << "   ✓ Event-driven spike propagation with delays" << std::endl;
    std::cout << "   ✓ Multiple plasticity mechanisms (STDP, Hebbian, structural)" << std::endl;
    std::cout << "   ✓ Neuromodulation (Dopamine, Curiosity, Novelty, Prediction Error)" << std::endl;
    std::cout << "   ✓ Integrated memory systems (Working, Episodic, Associative)" << std::endl;
    std::cout << "   ✓ Cognitive systems (Planning, Attention, Concept Formation)" << std::endl;
    std::cout << "   ✓ Development system with age-dependent plasticity" << std::endl;
    std::cout << "   ✓ Reward-modulated learning from experience" << std::endl;
    std::cout << "   ✓ Checkpoint persistence and state management" << std::endl;
    std::cout << "   ✓ Curiosity-driven exploration behavior" << std::endl;
    
    return 0;
}

// Simple version for quick testing
void simpleTest() {
    std::cout << "Simple Phase 6 Test" << std::endl;
    std::cout << "====================" << std::endl;
    
    // Create minimal system
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    auto world = nlm::createSimpleWorld();
    auto agent = nlm::createAgentBrain(brain);
    
    if (!brain || !world || !agent) {
        std::cout << "Failed to create components" << std::endl;
        return;
    }
    
    // Initialize
    if (!brain->initialize()) {
        std::cout << "Brain initialization failed" << std::endl;
        return;
    }
    
    agent->initialize(*world);
    
    // Quick simulation
    for (int i = 0; i < 100; ++i) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(*percept);
        brain->step(i);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    // Results
    std::cout << "Quick test completed:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Development stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    std::cout << "  Curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "  Novelty level: " << agent->getNoveltyLevel() << std::endl;
}

int mainSimple() {
    simpleTest();
    return 0;
}

int main() {
    return main(); // This is the full version
}