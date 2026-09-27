// Simulation performance benchmarks
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <iomanip>

namespace benchmark_simulation {

struct SimulationBenchmarkResults {
    double timeMs;
    size_t steps;
    size_t neurons;
    double avgFiringRate;
    double avgActionCount;
    std::string description;
    
    SimulationBenchmarkResults(double time, size_t steps, size_t neurons, double avgFiring, double avgActions, const std::string& desc)
        : timeMs(time), steps(steps), neurons(neurons), avgFiringRate(avgFiring), avgActionCount(avgActions), description(desc) {}
    
    void print() const {
        std::cout << std::setw(100) << std::left << description
                  << std::setw(15) << std::right << timeMs << " ms"
                  << std::setw(12) << std::right << steps << " steps"
                  << std::setw(15) << std::right << neurons << " neurons"
                  << std::setw(15) << std::right << std::fixed << std::setprecision(2) << avgFiringRate << " Hz"
                  << std::setw(15) << std::right << std::fixed << std::setprecision(2) << avgActionCount << " actions" << std::endl;
    }
};

SimulationBenchmarkResults benchmarkBrainSimulation(size_t neuronCount, size_t steps, size_t worldWidth, size_t worldHeight) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalFiringRate = 0.0;
    double totalActionCount = 0.0;
    
    for (size_t step = 0; step < steps; ++step) {
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(static_cast<float>(worldWidth), static_cast<float>(worldHeight), 8, 8);
        
        // Create agent brain
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Run simulation step
        auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
        brain->step(simStep);
        
        // Get agent percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Get action
        auto action = agentBrain->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Record metrics
        totalFiringRate += brain->getAverageFiringRate();
        totalActionCount += 1.0; // One action per step
        
        // Reset for next iteration
        brain->reset();
        world->reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgFiringRate = totalFiringRate / steps;
    double avgActionCount = totalActionCount / steps;
    
    return SimulationBenchmarkResults(timeMs, steps, neuronCount, avgFiringRate, avgActionCount,
                                     "Brain-World Simulation: " + std::to_string(neuronCount) + " neurons, " + std::to_string(steps) + " steps, " +
                                     std::to_string(worldWidth) + "x" + std::to_string(worldHeight) + " world");
}

SimulationBenchmarkResults benchmarkComplexWorldSimulation(size_t neuronCount, size_t steps, size_t worldWidth, size_t worldHeight) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalFiringRate = 0.0;
    double totalActionCount = 0.0;
    size_t totalWorldObjects = 0;
    
    for (size_t step = 0; step < steps; ++step) {
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
        config->set("world.object_density", 0.1f, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Create complex world
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(static_cast<float>(worldWidth), static_cast<float>(worldHeight), 12, 12);
        
        // Add multiple objects to the world
        for (size_t i = 0; i < 20; ++i) {
            nlm::WorldObject obj;
            obj.x = static_cast<float>(rand() % worldWidth);
            obj.y = static_cast<float>(rand() % worldHeight);
            obj.type = static_cast<nlm::WorldObjectType>(rand() % 5);
            obj.value = static_cast<float>(rand() % 100);
            obj.active = true;
            
            world->addObject(obj);
        }
        
        // Create agent brain
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Run simulation with complex interactions
        for (size_t i = 0; i < 10; ++i) { // Multiple sub-steps per main step
            auto simStep = nlm::SimulationStep{step * 10 + i, static_cast<nlm::Timestamp>((step * 10 + i) * 0.01f)};
            brain->step(simStep);
            
            auto percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            auto action = agentBrain->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Remove some objects occasionally
            if (i % 3 == 0 && !world->getObjects().empty()) {
                auto& objects = const_cast<std::vector<nlm::WorldObject>&>(world->getObjects());
                if (!objects.empty()) {
                    objects.pop_back();
                }
            }
        }
        
        // Record metrics
        totalFiringRate += brain->getAverageFiringRate();
        totalActionCount += 10.0; // 10 actions per main step
        totalWorldObjects += 20;
        
        // Reset
        brain->reset();
        world->reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgFiringRate = totalFiringRate / steps;
    double avgActionCount = totalActionCount / steps;
    
    return SimulationBenchmarkResults(timeMs, steps, neuronCount, avgFiringRate, avgActionCount,
                                     "Complex World Simulation: " + std::to_string(neuronCount) + " neurons, " + std::to_string(steps) + " steps, " +
                                     std::to_string(worldWidth) + "x" + std::to_string(worldHeight) + " world with objects");
}

SimulationBenchmarkResults benchmarkMultiAgentSimulation(size_t neuronCountPerAgent, size_t numAgents, size_t steps) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalFiringRate = 0.0;
    double totalActionCount = 0.0;
    
    for (size_t step = 0; step < steps; ++step) {
        // Create multiple agent brains
        std::vector<std::shared_ptr<nlm::Brain>> brains;
        std::vector<std::shared_ptr<nlm::AgentBrain>> agentBrains;
        std::vector<std::shared_ptr<nlm::SimpleWorld>> worlds;
        
        for (size_t agent = 0; agent < numAgents; ++agent) {
            // Create unique configuration for each agent
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int>(neuronCountPerAgent), nlm::ConfigSource::Runtime);
            config->set("agent.id", static_cast<int>(agent), nlm::ConfigSource::Runtime);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<nlm::SimpleWorld>();
            world->configure(15, 15, 8, 8);
            
            auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
            agentBrain->initialize(*world);
            
            brains.push_back(brain);
            agentBrains.push_back(agentBrain);
            worlds.push_back(world);
        }
        
        // Run multi-agent simulation
        for (size_t agent = 0; agent < numAgents; ++agent) {
            auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
            brains[agent]->step(simStep);
            
            auto percept = worlds[agent]->getSensoryPercept();
            agentBrains[agent]->processSensoryInput(percept);
            
            auto action = agentBrains[agent]->decodeMotorCommand();
            worlds[agent]->applyMotorCommand(action, worlds[agent]->getSimulationTime());
            
            totalFiringRate += brains[agent]->getAverageFiringRate();
            totalActionCount += 1.0;
            
            brains[agent]->reset();
            worlds[agent]->reset();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgFiringRate = totalFiringRate / (steps * numAgents);
    double avgActionCount = totalActionCount / (steps * numAgents);
    
    return SimulationBenchmarkResults(timeMs, steps, neuronCountPerAgent * numAgents, avgFiringRate, avgActionCount,
                                     "Multi-Agent Simulation: " + std::to_string(numAgents) + " agents, " + std::to_string(neuronCountPerAgent) + " neurons each, " + std::to_string(steps) + " steps");
}

SimulationBenchmarkResults benchmarkScalabilityTest(size_t baseNeuronCount, size_t scaleFactor) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalFiringRate = 0.0;
    double totalActionCount = 0.0;
    
    // Test scalability with different neuron counts
    for (size_t scale = 1; scale <= scaleFactor; ++scale) {
        size_t neuronCount = baseNeuronCount * scale;
        size_t steps = 20 / scale; // Reduce steps for larger networks
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Run simulation
        for (size_t step = 0; step < steps; ++step) {
            auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
            brain->step(simStep);
            
            totalFiringRate += brain->getAverageFiringRate();
            totalActionCount += 1.0;
        }
        
        brain->reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgFiringRate = totalFiringRate / (baseNeuronCount * scaleFactor);
    double avgActionCount = totalActionCount / (baseNeuronCount * scaleFactor);
    
    return SimulationBenchmarkResults(timeMs, baseNeuronCount * scaleFactor, baseNeuronCount * scaleFactor, avgFiringRate, avgActionCount,
                                     "Scalability Test: " + std::to_string(baseNeuronCount) + " * " + std::to_string(scaleFactor) + " neurons");
}

void runSimulationBenchmarks() {
    std::cout << "========================================" << std::endl;
    std::cout << "Simulation Performance Benchmarks" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Seed random number generator
    std::srand(42);
    
    // Benchmark brain-world simulation
    std::cout << "Brain-World Simulation Benchmarks:" << std::endl;
    std::cout << std::setw(100) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Steps"
              << std::setw(15) << std::right << "Neurons"
              << std::setw(15) << std::right << "Avg Firing Rate"
              << std::setw(15) << std::right << "Avg Actions" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto simResults1 = benchmarkBrainSimulation(1000, 50, 10, 10);
    simResults1.print();
    
    auto simResults2 = benchmarkBrainSimulation(5000, 20, 15, 15);
    simResults2.print();
    
    std::cout << std::endl;
    
    // Benchmark complex world simulation
    std::cout << "Complex World Simulation Benchmarks:" << std::endl;
    std::cout << std::setw(100) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Steps"
              << std::setw(15) << std::right << "Neurons"
              << std::setw(15) << std::right << "Avg Firing Rate"
              << std::setw(15) << std::right << "Avg Actions" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto complexResults1 = benchmarkComplexWorldSimulation(1000, 20, 12, 12);
    complexResults1.print();
    
    auto complexResults2 = benchmarkComplexWorldSimulation(5000, 10, 20, 20);
    complexResults2.print();
    
    std::cout << std::endl;
    
    // Benchmark multi-agent simulation
    std::cout << "Multi-Agent Simulation Benchmarks:" << std::endl;
    std::cout << std::setw(100) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Steps"
              << std::setw(15) << std::right << "Neurons"
              << std::setw(15) << std::right << "Avg Firing Rate"
              << std::setw(15) << std::right << "Avg Actions" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto multiResults1 = benchmarkMultiAgentSimulation(500, 5, 30);
    multiResults1.print();
    
    auto multiResults2 = benchmarkMultiAgentSimulation(300, 10, 20);
    multiResults2.print();
    
    std::cout << std::endl;
    
    // Benchmark scalability test
    std::cout << "Scalability Test Benchmarks:" << std::endl;
    std::cout << std::setw(100) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Steps"
              << std::setw(15) << std::right << "Neurons"
              << std::setw(15) << std::right << "Avg Firing Rate"
              << std::setw(15) << std::right << "Avg Actions" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto scaleResults = benchmarkScalabilityTest(1000, 5);
    scaleResults.print();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Simulation benchmark completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
}

} // namespace benchmark_simulation

int main() {
    std::cout << "Running Simulation Performance Benchmarks..." << std::endl;
    benchmark_simulation::runSimulationBenchmarks();
    return 0;
}
