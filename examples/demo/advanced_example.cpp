#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "plasticity/PlasticityRule.hpp"

int main() {
    std::cout << "NLM Neural Simulation - Advanced Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Advanced neural network with multiple modules
    nlm::brain::Brain brain(500, 2000.0);
    nlm::memory::Memory memory(5000);
    nlm::cognition::NeuralPlanner planner(100, 10);
    nlm::plasticity::PlasticityRule plasticity(nlm::plasticity::PlasticityType::STDP, 0.01);
    
    std::cout << "Advanced neural network created with 500 neurons" << std::endl;
    std::cout << "Advanced memory system with 5000 slots" << std::endl;
    std::cout << "Cognitive planner with 100 nodes" << std::endl;
    std::cout << "Plasticity system with STDP learning" << std::endl;
    
    // Complex simulation with learning
    std::vector<double> goals = {10.0, 20.0, 30.0, 40.0, 50.0};
    std::vector<double> rewards = {1.0, 2.0, 1.5, 3.0, 2.5};
    
    for (int i = 0; i < goals.size(); ++i) {
        std::cout << "\nIteration " << i + 1 << ": Targeting goal " << goals[i] << std::endl;
        
        // Brain processing
        brain.update(0.1);
        
        // Cognitive planning
        auto plan = planner.plan(goals[i]);
        std::cout << "  Planned " << plan.planned_actions.size() << " actions" << std::endl;
        
        // Plasticity learning
        plasticity.learn(brain.getOutput(), rewards[i]);
        
        // Memory consolidation
        memory.consolidate();
        std::cout << "  Memory consolidated with " << memory.getUsedCapacity() << "/" << memory.getTotalCapacity() << " used" << std::endl;
        
        // Reward-based memory update
        memory.updateFromExperience(plan, rewards[i]);
    }
    
    std::cout << "\nAdvanced demo completed successfully!" << std::endl;
    std::cout << "All modules working in concert with complex learning dynamics" << std::endl;
    
    return 0;
}
