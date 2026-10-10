// Phase 6 test - Advanced integrated systems
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include "world/SimpleWorld.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <iostream>

void runPhase6Test() {
    std::cout << "Running Phase 6: Advanced Integrated Systems" << std::endl;
    
    // Create integrated agent
    nlm::agent::AgentBrain agent(200, 20, 15.0);
    
    // Create environment
    nlm::environment::Environment env(200.0, 100.0, 50.0);
    
    // Create world
    nlm::world::SimpleWorld world(&env);
    
    // Create cognitive planner
    nlm::cognition::NeuralPlanner planner(50, 10);
    
    // Run integrated simulation
    for (int step = 0; step < 20; ++step) {
        // Agent perception and action
        auto observation = env.getObservation();
        agent.sense(observation);
        
        auto action = agent.think();
        env.applyAction(action);
        
        // World dynamics
        world.update(0.1);
        
        // Cognitive planning
        auto goal = env.getTargetGoal();
        auto plan = planner.plan(goal);
        
        // Memory updates
        agent.updateMemory(observation);
    }
    
    std::cout << "Phase 6 test completed successfully" << std::endl;
}
