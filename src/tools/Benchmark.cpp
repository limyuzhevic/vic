#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

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

// Simple benchmark results
struct BenchmarkResult {
    std::string testName;
    duration<double> elapsedTime;
    size_t memoryUsage;
    size_t neuronCount;
    size_t synapseCount;
    size_t totalSpikes;
    double avgFiringRate;
    std::string unit;
};

// NLM Benchmark Suite
class NLMBenchmark {
public:
    void runAllBenchmarks(const std::string& configFile = "") {
        std::cout << "=== NLM Performance Benchmark Suite ===\n\n";
        
        try {
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Load configuration
            auto config = std::make_shared<Config>();
            if (!configFile.empty()) {
                config->loadFromFile(configFile);
            } else {
                // Set reasonable defaults for benchmarking
                config->set("brain.neuron_count", 1000, ConfigSource::Default);
                config->set("brain.connection_probability", 0.05, ConfigSource::Default);
                config->set("simulation.timestep", 0.001, ConfigSource::Default);
                config->set("random_seed", 42, ConfigSource::Default);
            }
            
            std::vector<BenchmarkResult> results;
            
            // Benchmark 1: Brain Initialization
            results.push_back(benchmarkBrainInitialization(config));
            
            // Benchmark 2: Step Performance
            results.push_back(benchmarkStepPerformance(config));
            
            // Benchmark 3: Plasticity Performance
            results.push_back(benchmarkPlasticityPerformance(config));
            
            // Benchmark 4: Agent-World Integration
            results.push_back(benchmarkAgentWorldIntegration(config));
            
            // Print benchmark results
            printBenchmarkResults(results);
            
            // Save benchmark results
            saveBenchmarkResults(results, "benchmark_results.txt");
            
            std::cout << "\nBenchmark results saved to: benchmark_results.txt" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error during benchmarking: " << e.what() << std::endl;
            return;
        }
    }
    
private:
    BenchmarkResult benchmarkBrainInitialization(const std::shared_ptr<Config>& config) {
        std::cout << "Benchmark 1: Brain Initialization" << std::endl;
        
        auto startTime = steady_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        auto elapsed = duration_cast<duration<double>>(steady_clock::now() - startTime);
        
        if (brain->initialize()) {
            auto resultTime = steady_clock::now();
            auto resultElapsed = duration_cast<duration<double>>(resultTime - startTime);
            
            return {
                "Brain Initialization",
                resultElapsed,
                0, // Memory usage not measured
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                "ms"
            };
        } else {
            throw std::runtime_error("Brain initialization failed");
        }
    }
    
    BenchmarkResult benchmarkStepPerformance(const std::shared_ptr<Config>& config) {
        std::cout << "Benchmark 2: Step Performance" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = steady_clock::now();
        auto stepsToRun = 1000;
        
        for (SimulationStep step = 0; step < stepsToRun; ++step) {
            brain->step(step, step * config->getOr<double>("simulation.timestep", 0.001));
            
            if (step % 200 == 0) {
                std::cout << "  Step " << step << " / " << stepsToRun << std::endl;
            }
        }
        
        auto elapsed = duration_cast<duration<double>>(steady_clock::now() - startTime);
        
        return {
            "Step Performance",
            elapsed,
            0,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            "ms"
        };
    }
    
    BenchmarkResult benchmarkPlasticityPerformance(const std::shared_ptr<Config>& config) {
        std::cout << "Benchmark 3: Plasticity Performance" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable all plasticity mechanisms
        auto* region = brain->getRegion(RegionId(1);
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        auto startTime = steady_clock::now();
        auto stepsToRun = 2000;
        
        for (SimulationStep step = 0; step < stepsToRun; ++step) {
            // Inject input to trigger plasticity
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * config->getOr<double>("simulation.timestep", 0.001));
            
            if (step % 400 == 0) {
                std::cout << "  Step " << step << " / " << stepsToRun << std::endl;
            }
        }
        
        auto elapsed = duration_cast<duration<double>>(steady_clock::now() - startTime);
        
        return {
            "Plasticity Performance",
            elapsed,
            0,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            "ms"
        };
    }
    
    BenchmarkResult benchmarkAgentWorldIntegration(const std::shared_ptr<Config>& config) {
        std::cout << "Benchmark 4: Agent-World Integration" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Enable agent subsystems
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        auto startTime = steady_clock::now();
        auto stepsToRun = 500;
        
        for (int step = 0; step < stepsToRun; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input from world
            const auto& percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command from brain activity
            auto motorCmd = agentBrain->decodeMotorCommand();
            
            // Apply motor command to world
            world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = 0.0f;
            if (world->getSensoryPercept().getInternal() && !world->getSensoryPercept().getInternal()->empty()) {
                reward = world->getSensoryPercept().getInternal()->at(0);
            }
            agentBrain->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agentBrain->updateDevelopment(0.1);
            
            if (step % 100 == 0) {
                std::cout << "  Step " << step << " / " << stepsToRun << std::endl;
            }
        }
        
        auto elapsed = duration_cast<duration<double>>(steady_clock::now() - startTime);
        
        return {
            "Agent-World Integration",
            elapsed,
            0,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            "ms"
        };
    }
    
    void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n=== Benchmark Results ===\n\n";
        
        double totalTime = 0.0;
        for (const auto& result : results) {
            totalTime += result.elapsedTime.count();
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "  " << result.testName << ": " << result.elapsedTime.count() * 1000 << " " << result.unit;
            if (!result.unit.empty() && result.unit != "ms") {
                std::cout << " (" << result.elapsedTime.count() * 1000 << " " << result.unit << ")";
            }
            std::cout << std::endl;
            std::cout << "    Neurons: " << result.neuronCount 
                      << " | Synapses: " << result.synapseCount 
                      << " | Total Spikes: " << result.totalSpikes 
                      << " | Avg Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        }
        
        std::cout << "\nTotal Benchmark Time: " << totalTime * 1000 << " ms" << std::endl;
        std::cout << "\nBenchmark Statistics:" << std::endl;
        std::cout << "  Total benchmarks: " << results.size() << std::endl;
        std::cout << "  Average time per benchmark: " << (totalTime / results.size()) * 1000 << " ms" << std::endl;
        if (!results.empty()) {
            std::cout << "  Fastest benchmark: " << results.back().testName << " (" << results.back().elapsedTime.count() * 1000 << " " << results.back().unit << ")" << std::endl;
            std::cout << "  Slowest benchmark: " << results.front().testName << " (" << results.front().elapsedTime.count() * 1000 << " " << results.front().unit << ")" << std::endl;
        }
    }
    
    void saveBenchmarkResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not save benchmark results to " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Benchmark Results" << std::endl;
        file << "Generated: " << std::endl;
        file << "===================================\n\n";
        
        for (const auto& result : results) {
            file << result.testName << ": " << result.elapsedTime.count() * 1000 << " " << result.unit;
            if (!result.unit.empty() && result.unit != "ms") {
                file << " (" << result.elapsedTime.count() * 1000 << " " << result.unit << ")";
            }
            file << "\n";
            file << "  Neurons: " << result.neuronCount 
                 << " | Synapses: " << result.synapseCount 
                 << " | Total Spikes: " << result.totalSpikes 
                 << " | Avg Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
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
            std::cout << "Usage: nlm_benchmark [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config <file>    Configuration file" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        }
    }
    
    NLMBenchmark benchmark;
    benchmark.runAllBenchmarks(configFile);
    
    return 0;
}
