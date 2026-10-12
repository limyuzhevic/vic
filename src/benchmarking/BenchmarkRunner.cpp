// NLM Performance Benchmark Runner
// Comprehensive benchmarking for NLM performance and scalability

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <mutex>
#include <future>
#include <random>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include "modern_utils/ModernUtils.hpp"

using namespace nlm;
using namespace nlm::testing;
using namespace std::chrono;

class BenchmarkResult {
public:
    std::string name;
    double timeSeconds;
    size_t iterations;
    double opsPerSecond;
    double memoryUsageMB;
    std::map<std::string, std::string> metadata;
    std::vector<double> timeline;
    
    BenchmarkResult(const std::string& benchmarkName, double time, size_t iterations)
        : name(benchmarkName), timeSeconds(time), iterations(iterations),
          opsPerSecond(static_cast<double>(iterations) / time), memoryUsageMB(0.0) {}
    
    std::string summary() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << "Benchmark: " << name << std::endl;
        oss << "  Time: " << timeSeconds << "s" << std::endl;
        oss << "  Iterations: " << iterations << std::endl;
        oss << "  Performance: " << opsPerSecond << " ops/sec" << std::endl;
        oss << "  Memory: " << memoryUsageMB << " MB" << std::endl;
        return oss.str();
    }
};

class NLMBenchmark {
public:
    struct BenchmarkConfig {
        bool runNeurons = true;
        bool runSynapses = true;
        bool runMemory = true;
        bool runPlasticity = true;
        bool runIntegration = true;
        bool runParallel = false;
        size_t neuronCount = 1000;
        size_t steps = 1000;
        double timestep = 0.001;
        int numThreads = std::thread::hardware_concurrency();
        std::string outputFile;
        bool verbose = true;
    };
    
    NLMBenchmark(const BenchmarkConfig& config) : config_(config) {}
    
    void runAllBenchmarks() {
        std::cout << "NLM Performance Benchmark Suite" << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config_.neuronCount << std::endl;
        std::cout << "  Steps: " << config_.steps << std::endl;
        std::cout << "  Timestep: " << config_.timestep << "s" << std::endl;
        std::cout << "  Threads: " << config_.numThreads << std::endl;
        std::cout << "  Parallel: " << (config_.runParallel ? "Yes" : "No") << std::endl;
        
        std::vector<BenchmarkResult> results;
        
        // Neuron benchmarks
        if (config_.runNeurons) {
            results.push_back(benchmarkNeuronDynamics());
        }
        
        // Synapse benchmarks
        if (config_.runSynapses) {
            results.push_back(benchmarkSynapseDynamics());
        }
        
        // Memory benchmarks
        if (config_.runMemory) {
            results.push_back(benchmarkMemorySystems());
        }
        
        // Plasticity benchmarks
        if (config_.runPlasticity) {
            results.push_back(benchmarkPlasticity());
        }
        
        // Integration benchmark
        if (config_.runIntegration) {
            results.push_back(benchmarkIntegration());
        }
        
        // Parallel benchmark
        if (config_.runParallel) {
            results.push_back(benchmarkParallelProcessing());
        }
        
        // Generate report
        generateReport(results);
        
        if (!config_.outputFile.empty()) {
            saveResults(config_.outputFile, results);
        }
    }
    
private:
    BenchmarkResult benchmarkNeuronDynamics() {
        std::cout << "\nBenchmarking neuron dynamics..." << std::endl;
        
        auto config = createBrainConfig(500, 1, 100);
        
        auto timer = utils::Timer();
        timer.start();
        
        // Run multiple simulations
        for (int i = 0; i < 10; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        
        timer.stop();
        double time = timer.elapsedSeconds();
        
        std::cout << "  Completed in " << time << "s" << std::endl;
        return BenchmarkResult("Neuron Dynamics", time, 1000);
    }
    
    BenchmarkResult benchmarkSynapseDynamics() {
        std::cout << "\nBenchmarking synapse dynamics..." << std::endl;
        
        auto config = createBrainConfig(1000, 2, 500);
        
        auto timer = utils::Timer();
        timer.start();
        
        // Focus on synaptic operations
        for (int i = 0; i < 5; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Inject synaptic input and process
            for (int step = 0; step < 200; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
                brain->step(step, step * 0.001);
            }
        }
        
        timer.stop();
        double time = timer.elapsedSeconds();
        
        std::cout << "  Completed in " << time << "s" << std::endl;
        return BenchmarkResult("Synapse Dynamics", time, 500);
    }
    
    BenchmarkResult benchmarkMemorySystems() {
        std::cout << "\nBenchmarking memory systems..." << std::endl;
        
        auto config = createBrainConfig(800, 1, 200);
        
        auto timer = utils::Timer();
        timer.start();
        
        // Memory-intensive operations
        for (int i = 0; i < 3; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Use memory systems
            for (int step = 0; step < 150; ++step) {
                brain->step(step, step * 0.001);
                
                // Simulate memory operations
                if (brain->getTotalSpikeCount() % 50 == 0) {
                    brain->getWorkingMemory()->store(NeuronId(i), static_cast<float>(i * 0.1));
                }
            }
        }
        
        timer.stop();
        double time = timer.elapsedSeconds();
        
        std::cout << "  Completed in " << time << "s" << std::endl;
        return BenchmarkResult("Memory Systems", time, 300);
    }
    
    BenchmarkResult benchmarkPlasticity() {
        std::cout << "\nBenchmarking plasticity mechanisms..." << std::endl;
        
        auto config = createBrainConfig(600, 2, 300);
        
        // Configure plasticity
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", false, ConfigSource::Default);
        
        auto timer = utils::Timer();
        timer.start();
        
        // Plasticity-intensive operations
        for (int i = 0; i < 4; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Enable all plasticity types
            for (const auto& region : brain->getRegions()) {
                for (const auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            // Apply learning patterns
            for (int step = 0; step < 250; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                brain->step(step, step * 0.001);
                
                if (step % 50 == 0) {
                    brain->applyRewardModulation(0.5f, 0.0f);
                }
            }
        }
        
        timer.stop();
        double time = timer.elapsedSeconds();
        
        std::cout << "  Completed in " << time << "s" << std::endl;
        return BenchmarkResult("Plasticity Mechanisms", time, 200);
    }
    
    BenchmarkResult benchmarkIntegration() {
        std::cout << "\nBenchmarking system integration..." << std::endl;
        
        auto config = createBrainConfig(700, 3, 400);
        
        auto timer = utils::Timer();
        timer.start();
        
        // Full integration test with all systems
        for (int i = 0; i < 2; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create agent and world
            auto agent = std::make_shared<AgentBrain>(brain);
            auto world = std::make_unique<GridWorldEnvironment>(15, 15);
            
            // Run full simulation loop
            for (int step = 0; step < 300; ++step) {
                // Environment update
                world->update(0.1);
                
                // Agent processing
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(*percept);
                
                // Brain step
                brain->step(step, step * 0.001);
                
                // Motor command
                auto motorCmd = agent->decodeMotorCommand();
                world->applyMotorCommand(*motorCmd, step * 0.1);
                
                // Neuromodulation
                auto reward = world->getSensoryPercept()->getInternal()[0] if world->getSensoryPercept()->getInternal() else 0.0f;
                agent->applyRewardModulation(reward, 0.0f);
            }
        }
        
        timer.stop();
        double time = timer.elapsedSeconds();
        
        std::cout << "  Completed in " << time << "s" << std::endl;
        return BenchmarkResult("System Integration", time, 150);
    }
    
    BenchmarkResult benchmarkParallelProcessing() {
        std::cout << "\nBenchmarking parallel processing..." << std::endl;
        
        auto config = createBrainConfig(300, 1, 100);
        
        // Thread pool benchmark
        utils::ThreadPool pool(config_.numThreads);
        
        auto timer = utils::Timer();
        timer.start();
        
        // Parallel brain simulations
        std::vector<std::future<void>> futures;
        for (int i = 0; i < config_.numThreads; ++i) {
            futures.push_back(pool.enqueue([this, config, i]() {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                for (int step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                }
            }));
        }
        
        // Wait for all to complete
        for (auto& future : futures) {
            future.get();
        }
        
        timer.stop();
        double time = timer.elapsedSeconds();
        
        std::cout << "  Completed in " << time << "s" << std::endl;
        return BenchmarkResult("Parallel Processing", time, config_.numThreads * 50);
    }
    
    std::shared_ptr<Config> createBrainConfig(size_t neurons, size_t regions, size_t steps) {
        auto config = std::make_shared<Config>();
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regions), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("max_steps", static_cast<int64_t>(steps), ConfigSource::Default);
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.0005f, ConfigSource::Default);
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        return config;
    }
    
    void generateReport(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "BENCHMARK REPORT" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        double totalTime = 0.0;
        double totalOps = 0.0;
        
        std::cout << std::left << std::setw(25) << "Benchmark" 
                 << std::right << std::setw(12) << "Time (s)" 
                 << std::right << std::setw(15) << "Ops/sec" 
                 << std::right << std::setw(10) << "Memory (MB)" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        for (const auto& result : results) {
            totalTime += result.timeSeconds;
            totalOps += result.opsPerSecond;
            
            std::cout << std::left << std::setw(25) << result.name
                     << std::right << std::setw(12) << std::fixed << std::setprecision(3) << result.timeSeconds
                     << std::right << std::setw(15) << std::fixed << std::setprecision(0) << result.opsPerSecond
                     << std::right << std::setw(10) << std::fixed << std::setprecision(1) << result.memoryUsageMB << std::endl;
        }
        
        std::cout << std::string(60, '-') << std::endl;
        std::cout << "Total: " << std::setw(10) << std::fixed << std::setprecision(3) << totalTime << "s "
                 << "Performance: " << std::setw(15) << std::fixed << std::setprecision(0) << (totalOps / results.size()) << " ops/sec" << std::endl;
    }
    
    void saveResults(const std::string& filename, const std::vector<BenchmarkResult>& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open results file: " + filename);
        }
        
        file << "NLM Performance Benchmark Results" << std::endl;
        file << "================================" << std::endl;
        file << "Generated on: " << utils::formatString("%Y-%m-%d %H:%M:%S") << std::endl;
        file << "Benchmark Configuration:" << std::endl;
        file << "  Neurons: " << config_.neuronCount << std::endl;
        file << "  Steps: " << config_.steps << std::endl;
        file << "  Timestep: " << config_.timestep << "s" << std::endl;
        file << "  Threads: " << config_.numThreads << std::endl;
        file << "  Parallel: " << (config_.runParallel ? "Yes" : "No") << std::endl;
        file << "\nDetailed Results:" << std::endl;
        
        for (const auto& result : results) {
            file << result.summary() << std::endl;
        }
        
        file.close();
        std::cout << "\nResults saved to: " << filename << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        // Default configuration
        NLMBenchmark::BenchmarkConfig config;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--neurons" && i + 1 < argc) {
                config.neuronCount = std::stoull(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                config.steps = std::stoull(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                config.timestep = std::stod(argv[++i]);
            } else if (arg == "--threads" && i + 1 < argc) {
                config.numThreads = std::stoi(argv[++i]);
            } else if (arg == "--output" && i + 1 < argc) {
                config.outputFile = argv[++i];
            } else if (arg == "--no-neurons") {
                config.runNeurons = false;
            } else if (arg == "--no-synapses") {
                config.runSynapses = false;
            } else if (arg == "--no-memory") {
                config.runMemory = false;
            } else if (arg == "--no-plasticity") {
                config.runPlasticity = false;
            } else if (arg == "--no-integration") {
                config.runIntegration = false;
            } else if (arg == "--parallel") {
                config.runParallel = true;
            } else if (arg == "--quiet") {
                config.verbose = false;
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "NLM Benchmark Runner" << std::endl;
                std::cout << "====================" << std::endl;
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --neurons N       Number of neurons (default: 1000)" << std::endl;
                std::cout << "  --steps S         Number of steps (default: 1000)" << std::endl;
                std::cout << "  --timestep T      Simulation timestep (default: 0.001)" << std::endl;
                std::cout << "  --threads N       Number of threads (default: CPU count)" << std::endl;
                std::cout << "  --output FILE     Save results to file" << std::endl;
                std::cout << "  --no-neurons      Skip neuron benchmarks" << std::endl;
                std::cout << "  --no-synapses     Skip synapse benchmarks" << std::endl;
                std::cout << "  --no-memory       Skip memory benchmarks" << std::endl;
                std::cout << "  --no-plasticity   Skip plasticity benchmarks" << std::endl;
                std::cout << "  --no-integration   Skip integration benchmarks" << std::endl;
                std::cout << "  --parallel        Enable parallel processing benchmarks" << std::endl;
                std::cout << "  --quiet           Quiet mode" << std::endl;
                std::cout << "  --help, -h        Show this help" << std::endl;
                return 0;
            }
        }
        
        // Run benchmarks
        NLMBenchmark benchmark(config);
        benchmark.runAllBenchmarks();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
