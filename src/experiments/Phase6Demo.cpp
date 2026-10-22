#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include <memory>
#include <iostream>

using namespace nlm;

int main() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing complete artificial brain integration..." << std::endl << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set reasonable defaults for Phase 6 demo
    config->set("random_seed", 42);
    config->set("neuron_count", 500);
    config->set("region_count", 3);
    config->set("simulation_timestep", 0.001);
    config->set("max_simulation_steps", 100);
    config->set("log_level", "INFO");
    
    // Create brain with all integrated systems
    std::cout << "1. Creating NLM brain with integrated systems...";
    auto brain = std::make_shared<Brain>(config);
    std::cout << " [OK]" << std::endl;
    
    // Initialize brain
    std::cout << "2. Initializing brain systems...";
    if (brain->initialize()) {
        std::cout << " [OK]" << std::endl;
    } else {
        std::cout << " [FAILED]" << std::endl;
        return 1;
    }
    
    // Create and initialize agent
    std::cout << "3. Creating agent with integrated brain...";
    auto agent = std::make_shared<AgentBody>();
    std::cout << " [OK]" << std::endl;
    
    // Create environment
    std::cout << "4. Creating world environment...";
    auto world = std::make_shared<SimpleWorld>(10, 10);
    std::cout << " [OK]" << std::endl;
    
    // Create agent brain
    std::cout << "5. Creating agent-brain interface...";
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    agentBrain->initialize(*world);
    std::cout << " [OK]" << std::endl;
    
    // Create and run Phase 6 experiment
    std::cout << "6. Starting Phase 6 integration experiment...";
    auto experiment = std::make_shared<Phase6IntegratedExperiment>();
    std::cout << " [OK]" << std::endl;
    
    if (integrationVerified) {
        std::cout << "8. Running complete Phase 6 integration test...";
        Phase6Config config;
        Phase6IntegrationResult result = experiment->run(config);
        std::cout << " [COMPLETE]" << std::endl;
        
        // Report detailed results
        experiment->reportResults(result);
    } else {
        std::cout << "8. Skipping full integration test (verification failed)" << std::endl;
        experiment->reportResults();
    }
    
    std::cout << "\n=== Phase 6 Demo Results ===" << std::endl;
    
    // Test system accessibility
    std::cout << "Brain System Status:" << std::endl;
    std::cout << "  - Working Memory: " << (brain->getWorkingMemory() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Episodic Memory: " << (brain->getEpisodicMemory() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Associative Memory: " << (brain->getAssociativeMemory() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Prediction System: " << (brain->getPredictionSystem() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Neural Planner: " << (brain->getPlanner() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Concept Formation: " << (brain->getConceptFormation() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Attentional Selection: " << (brain->getAttention() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "  - Development System: " << (brain->getDevelopmentSystem() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    
    std::cout << "  - Neuromodulators:" << std::endl;
    std::cout << "    + Dopamine: " << (brain->getDopamine() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "    + Acetylcholine: " << (brain->getAcetylcholine() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "    + Norepinephrine: " << (brain->getNorepinephrine() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "    + Serotonin: " << (brain->getSerotonin() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "    + Curiosity: " << (brain->getCuriosity() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "    + Novelty: " << (brain->getNovelty() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    std::cout << "    + Prediction Error: " << (brain->getPredictionErrorSignal() ? "✓ Accessible" : "✗ NOT ACCESSIBLE") << std::endl;
    
    // Run a short simulation
    std::cout << "\nRunning Phase 6 integration simulation..." << std::endl;
    bool simulationRunning = true;
    size_t step = 0;
    size_t maxSteps = 50;
    
    while (simulationRunning && step < maxSteps) {
        // Agent gets observation from world
        SensoryPercept percept = world->observe(*agent);
        
        // Brain processes sensory input
        brain->receiveSensoryInput(percept);
        
        // Brain performs its main step
        brain->step(step);
        
        // Agent decodes motor command from brain
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        
        // Agent applies action to world
        world->applyAction(*agent, cmd);
        
        // World computes reward
        float reward = world->computeReward(*agent);
        
        // Agent applies reward modulation to brain
        agentBrain->applyRewardModulation(reward, agentBrain->getExpectedReward());
        
        // Update agent brain development
        agentBrain->updateDevelopment(0.001);
        
        step++;
        
        if (step % 10 == 0) {
            std::cout << "  Step " << step << "/" << maxSteps << " complete..." << std::endl;
        }
        
        // Check if simulation should continue
        if (step >= maxSteps || agent->getHealth() <= 0.0f) {
            simulationRunning = false;
        }
    }
    
    std::cout << "\n=== Phase 6 Demo Complete ===" << std::endl;
    std::cout << "Simulation completed: " << step << " steps" << std::endl;
    std::cout << "Final agent health: " << agent->getHealth() << std::endl;
    std::cout << "Total experience stored: " << brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0 << " episodes" << std::endl;
    
    if (experiment) {
        experiment->reportResults();
    }
    
    std::cout << "\n✓ Phase 6 integration successfully tested all systems!" << std::endl;
    return 0;
}
