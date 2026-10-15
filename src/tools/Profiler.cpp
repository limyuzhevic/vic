#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>

// NLM core includes
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

using namespace nlm;
using namespace std::chrono;

// Simple performance profiler
class SimpleProfiler {
public:
    struct ProfileResult {
        std::string name;
        duration<double> elapsedTime;
        size_t memoryUsage;
        std::string unit;
    };
    
    std::vector<ProfileResult> results;
    
    void startProfile(const std::string& name) {
        profiles[name] = steady_clock::now();
    }
    
    void endProfile(const std::string& name, size_t memoryUsage = 0, const std::string& unit = "ms") {
        auto it = profiles.find(name);
        if (it != profiles.end()) {
            auto startTime = it->second;
            auto endTime = steady_clock::now();
            auto elapsed = duration_cast<duration<double>>(endTime - startTime);
            
            results.push_back({name, elapsed, memoryUsage, unit});
            profiles.erase(it);
        }
    }
    
    void printResults(std::ostream& out = std::cout) {
        out << "=== NLM Performance Profile ===\n\n";
        
        if (results.empty()) {
            out << "No profiling results available." << std::endl;
            return;
        }
        
        out << "Profile Results (sorted by elapsed time):" << std::endl;
        std::sort(results.begin(), results.end(),
            [](const ProfileResult& a, const ProfileResult& b) {
                return a.elapsedTime.count() > b.elapsedTime.count();
            });
        
        double totalTime = 0.0;
        for (const auto& result : results) {
            totalTime += result.elapsedTime.count();
            out << std::fixed << std::setprecision(4);
            out << "  " << result.name << ": " << result.elapsedTime.count() * 1000 << " " << result.unit;
            if (!result.unit.empty() && result.unit != "ms") {
                out << " (" << result.elapsedTime.count() * 1000 << " " << result.unit << ")";
            }
            out << std::endl;
        }
        
        out << "\nTotal Time: " << totalTime * 1000 << " ms" << std::endl;
        out << "\nProfile Statistics:" << std::endl;
        out << "  Total operations profiled: " << results.size() << std::endl;
        out << "  Average time per operation: " << (totalTime / results.size()) * 1000 << " ms" << std::endl;
        if (!results.empty()) {
            out << "  Fastest operation: " << results.back().name << " (" << results.back().elapsedTime.count() * 1000 << " " << results.back().unit << ")" << std::endl;
            out << "  Slowest operation: " << results.front().name << " (" << results.front().elapsedTime.count() * 1000 << " " << results.front().unit << ")" << std::endl;
        }
    }
    
private:
    std::map<std::string, steady_clock::time_point> profiles;
};

// Brain performance profiler
class BrainProfiler {
public:
    void profileBrainPerformance(const std::string& configFile = "") {
        std::cout << "=== NLM Brain Performance Profiler ===\n\n";
        
        SimpleProfiler profiler;
        
        try {
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); // Reduce log noise
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Load configuration
            auto config = std::make_shared<Config>();
            if (!configFile.empty()) {
                config->loadFromFile(configFile);
            }
            
            // Create brain
            profiler.startProfile("Brain Creation");
            auto brain = std::make_shared<Brain>(config);
            profiler.endProfile("Brain Creation", 0, "ms");
            
            if (!brain->initialize()) {
                std::cerr << "Error: Failed to initialize brain!" << std::endl;
                return;
            }
            
            // Create world
            profiler.startProfile("World Creation");
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            world->setAgentStart(10.0, 10.0);
            profiler.endProfile("World Creation", 0, "ms");
            
            // Run different simulation scenarios
            std::cout << "Running performance tests..." << std::endl;
            
            // Test 1: Basic connectivity
            profiler.startProfile("Basic Connectivity Test");
            testBasicConnectivity(*brain);
            profiler.endProfile("Basic Connectivity Test", 0, "ms");
            
            // Reset brain
            brain->reset();
            brain->initialize();
            
            // Test 2: Plasticity
            profiler.startProfile("Plasticity Experiment");
            testPlasticity(*brain);
            profiler.endProfile("Plasticity Experiment", 0, "ms");
            
            // Reset brain
            brain->reset();
            brain->initialize();
            
            // Test 3: Full agent-world simulation
            profiler.startProfile("Agent-World Simulation");
            testAgentWorldSimulation(*brain, *world);
            profiler.endProfile("Agent-World Simulation", 0, "ms");
            
            // Print profile results
            profiler.printResults();
            
            // Save profile results
            saveProfileResults(profiler.results, "profile_results.txt");
            
            std::cout << "\nProfile results saved to: profile_results.txt" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error during profiling: " << e.what() << std::endl;
            return;
        }
    }
    
private:
    void testBasicConnectivity(Brain& brain) {
        // Simple test to verify basic brain connectivity
        auto* region = brain.getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                // Inject current to test connectivity
                neurons[0]->injectCurrent(50.0f);
                
                // Run a few steps
                for (SimulationStep step = 0; step < 50; ++step) {
                    brain.step(step, step * 0.001);
                }
            }
        }
    }
    
    void testPlasticity(Brain& brain) {
        // Test plasticity mechanisms
        auto* region = brain.getRegion(RegionId(1));
        if (region) {
            // Enable plasticity
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
            
            // Apply learning experience
            for (SimulationStep step = 0; step < 1000; ++step) {
                // Inject correlated input to trigger learning
                for (size_t i = 0; i < 20 && i < brain.getTotalNeuronCount() / 4; ++i) {
                    brain.injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain.step(step, step * 0.001);
            }
        }
    }
    
    void testAgentWorldSimulation(Brain& brain, SimpleWorld& world) {
        // Test complete agent-world interaction
        auto agentBrain = std::make_shared<AgentBrain>(std::make_shared<Brain>(brain));
        agentBrain->initialize(world);
        
        // Enable agent subsystems
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        // Run agent simulation
        for (int step = 0; step < 500; ++step) {
            // Update world
            world.update(0.1);
            
            // Get sensory input from world
            const auto& percept = world.getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            // Run brain step
            brain.step(step);
            
            // Decode motor command from brain activity
            auto motorCmd = agentBrain->decodeMotorCommand();
            
            // Apply motor command to world
            world.applyMotorCommand(motorCmd, world.getSimulationTime());
            
            // Apply reward modulation
            float reward = 0.0f;
            if (world.getSensoryPercept().getInternal() && !world.getSensoryPercept().getInternal()->empty()) {
                reward = world.getSensoryPercept().getInternal()->at(0);
            }
            agentBrain->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agentBrain->updateDevelopment(0.1);
            
            // Print progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "  Step " << step << ": " << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
    }
    
    void saveProfileResults(const std::vector<SimpleProfiler::ProfileResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not save profile results to " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Profile Results" << std::endl;
        file << "Generated: " << std::endl;
        file << "===================================\n\n";
        
        for (const auto& result : results) {
            file << result.name << ": " << result.elapsedTime.count() * 1000 << " " << result.unit;
            if (!result.unit.empty() && result.unit != "ms") {
                file << " (" << result.elapsedTime.count() * 1000 << " " << result.unit << ")";
            }
            file << "\n";
        }
        
        file.close();
    }
};

int main(int argc, char* argv[]) {
    std::string configFile = "";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_profile [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config <file>    Configuration file" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        }
    }
    
    BrainProfiler profiler;
    profiler.profileBrainPerformance(configFile);
    
    return 0;
}
