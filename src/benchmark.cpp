// NLM Performance Benchmark Suite
// Comprehensive performance measurement for all NLM subsystems

#include "benchmark.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace nlm;

struct BenchmarkConfig {
    std::string name;
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;
    double opsPerSecond;
    double memoryUsage; // MB
    std::vector<double> metrics;
};

class NLMBenchmarkSuite {
public:
    static void runBenchmarks() {
        std::cout << "NLM Performance Benchmark Suite" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        std::vector<BenchmarkConfig> results;
        
        // Run basic operation benchmarks
        results.push_back(benchmarkNeuronCreation());
        results.push_back(benchmarkSynapseFormation());
        results.push_back(benchmarkSpikeDelivery());
        results.push_back(benchmarkMemoryOperations());
        
        // Run system-level benchmarks
        results.push_back(benchmarkHebbianUpdates());
        results.push_back(benchmarkStructuralPlasticity());
        results.push_back(benchmarkSynaptogenesis());
        results.push_back(benchmarkPruning());
        results.push_back(benchmarkMaturation());
        
        // Run neuromodulation benchmarks
        results.push_back(benchmarkCuriosity());
        results.push_back(benchmarkNovelty());
        results.push_back(benchmarkPredictionError());
        
        // Run memory system benchmarks
        results.push_back(benchmarkWorkingMemory());
        results.push_back(benchmarkEpisodicMemory());
        
        // Run integration benchmarks
        results.push_back(benchmarkFullBrainSimulation());
        
        // Print results
        printBenchmarkReport(results);
        
        // Save results to JSON
        saveBenchmarkResults(results, "benchmark_results.json");
    }
    
    static void runScalabilityTests() {
        std::cout << "\nNLM Scalability Tests" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::vector<size_t> neuronCounts = {100, 1000, 10000, 50000};
        std::vector<BenchmarkConfig> results;
        
        for (size_t neuronCount : neuronCounts) {
            std::cout << "Testing with " << neuronCount << " neurons..." << std::endl;
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < 100; ++step) {
                for (size_t i = 0; i < std::min(size_t(5), neuronCount / 1000); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            BenchmarkConfig result;
            result.name = "Scalability: " + std::to_string(neuronCount) + " neurons";
            result.neurons = brain->getTotalNeuronCount();
            result.synapses = brain->getTotalSynapseCount();
            result.steps = 100;
            result.totalTime = totalTime;
            result.opsPerSecond = result.steps / totalTime;
            
            results.push_back(result);
            printBenchmarkResult(result);
        }
        
        printScalabilityReport(results);
        saveBenchmarkResults(results, "scalability_results.json");
    }
    
    static void runMemoryUsageTests() {
        std::cout << "\nNLM Memory Usage Tests" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::vector<BenchmarkConfig> results;
        
        // Test memory operations with different network sizes
        for (size_t neuronCount : {100, 1000, 10000}) {
            std::cout << "Memory test with " << neuronCount << " neurons..." << std::endl;
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Perform memory-intensive operations
            for (size_t step = 0; step < 500; ++step) {
                brain->step(step, step * 0.001);
                
                // Check spike history (memory access)
                size_t totalSpikes = 0;
                for (auto& region : brain->getRegions()) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        totalSpikes += neuron->getSpikeHistory().size();
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Estimate memory usage (rough approximation)
            double estimatedMemory = neuronCount * 100.0 + neuronCount * 50.0; // neurons + synapses
            
            BenchmarkConfig result;
            result.name = "Memory Usage: " + std::to_string(neuronCount) + " neurons";
            result.neurons = brain->getTotalNeuronCount();
            result.synapses = brain->getTotalSynapseCount();
            result.steps = 500;
            result.totalTime = totalTime;
            result.opsPerSecond = result.steps / totalTime;
            result.memoryUsage = estimatedMemory / 1024.0 / 1024.0; // MB
            
            results.push_back(result);
            printBenchmarkResult(result);
        }
        
        saveBenchmarkResults(results, "memory_usage_results.json");
    }
    
    static void runIntegrationPerformanceTests() {
        std::cout << "\nNLM Integration Performance Tests" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::vector<BenchmarkConfig> results;
        
        // Test full integration with all systems active
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.03f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Initialize all systems
        Hebbian hebbian;
        StructuralPlasticity sp;
        Synaptogenesis synaptogenesis;
        Pruning pruning;
        Maturation maturation;
        Curiosity curiosity;
        Novelty novelty;
        PredictionError predictionError;
        
        curiosity.initialize(brain.get());
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 1000; ++step) {
            // Inject inputs
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            
            // Update plasticity systems
            hebbian.setLearningRate(0.01f);
            
            // Update development systems
            maturation.update(brain.get(), *brain->getRandomGenerator());
            pruning.update(brain.get(), *brain->getRandomGenerator());
            synaptogenesis.update(brain.get(), *brain->getRandomGenerator());
            
            // Update neuromodulation systems
            curiosity.update(novelty.getLevel(), predictionError.getLevel(), static_cast<float>(step));
            
            // Step brain
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Full Integration Performance";
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = 1000;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        
        results.push_back(result);
        printBenchmarkResult(result);
        saveBenchmarkResults(results, "integration_performance_results.json");
    }
    
private:
    static BenchmarkConfig benchmarkNeuronCreation() {
        std::cout << "Benchmarking neuron creation..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t totalNeurons = 0;
        for (size_t i = 0; i < 10000; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            neuron.setType(static_cast<nlm::NeuronType>(i % 3));
            neuron.setMembranePotential(-70.0f + (static_cast<float>(rand()) / RAND_MAX) * 20.0f);
            neuron.setThreshold(-55.0f);
            ++totalNeurons;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Neuron Creation";
        result.neurons = totalNeurons;
        result.steps = 10000;
        result.totalTime = totalTime;
        result.opsPerSecond = totalNeurons / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkSynapseFormation() {
        std::cout << "Benchmarking synapse formation..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t totalSynapses = 0;
        for (size_t i = 0; i < 5000; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
            synapse.setWeight(static_cast<float>(rand()) / RAND_MAX);
            synapse.setType(nlm::SynapseType::Excitatory);
            ++totalSynapses;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Synapse Formation";
        result.synapses = totalSynapses;
        result.steps = 5000;
        result.totalTime = totalTime;
        result.opsPerSecond = totalSynapses / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkSpikeDelivery() {
        std::cout << "Benchmarking spike delivery..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t totalSpikes = 0;
        for (size_t i = 0; i < 20000; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            neuron.setFiringState(nlm::FiringState::Active);
            neuron.recordSpike(static_cast<float>(i));
            ++totalSpikes;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Spike Delivery";
        result.steps = 20000;
        result.totalTime = totalTime;
        result.opsPerSecond = totalSpikes / totalTime;
        result.metrics = {static_cast<double>(totalSpikes)};
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkMemoryOperations() {
        std::cout << "Benchmarking memory operations..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::vector<std::vector<float>> data(1000, std::vector<float>(1000, 0.0f));
        size_t accessCount = 0;
        
        for (size_t i = 0; i < 1000; ++i) {
            for (size_t j = 0; j < 1000; ++j) {
                data[i][j] = static_cast<float>(i * j) / 1000.0f;
                if (data[i][j] > 0.5f) ++accessCount;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Memory Operations";
        result.steps = 1000000;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        result.metrics = {static_cast<double>(accessCount), totalTime * 1024 * 1024}; // access count, memory MB
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkHebbianUpdates() {
        std::cout << "Benchmarking Hebbian updates..." << std::endl;
        
        Hebbian hebbian;
        hebbian.setLearningRate(0.01f);
        
        std::vector<nlm::Synapse> synapses;
        for (size_t i = 0; i < 1000; ++i) {
            synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
            synapses.back().setWeight(0.5f);
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t updateCount = 0;
        for (size_t step = 0; step < 500; ++step) {
            for (auto& synapse : synapses) {
                std::vector<nlm::Timestamp> preSpikes = {static_cast<nlm::Timestamp>(step), static_cast<nlm::Timestamp>(step + 10)};
                std::vector<nlm::Timestamp> postSpikes = {static_cast<nlm::Timestamp>(step + 5), static_cast<nlm::Timestamp>(step + 15)};
                hebbian.update(&synapse, preSpikes, postSpikes, 0.001f);
                ++updateCount;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Hebbian Updates";
        result.synapses = synapses.size();
        result.steps = 500;
        result.totalTime = totalTime;
        result.opsPerSecond = updateCount / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkStructuralPlasticity() {
        std::cout << "Benchmarking Structural Plasticity..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        StructuralPlasticity sp;
        sp.setSynaptogenesisRate(0.0005f);
        sp.setPruningRate(0.000005f);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 200; ++step) {
            sp.update(brain.get(), *brain->getRandomGenerator());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Structural Plasticity";
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = 200;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkSynaptogenesis() {
        std::cout << "Benchmarking Synaptogenesis..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        Synaptogenesis synaptogenesis;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 300; ++step) {
            synaptogenesis.update(brain.get(), *brain->getRandomGenerator());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Synaptogenesis";
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = 300;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkPruning() {
        std::cout << "Benchmarking Pruning..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(800), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        Pruning pruning;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 250; ++step) {
            pruning.update(brain.get(), *brain->getRandomGenerator());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Pruning";
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = 250;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkMaturation() {
        std::cout << "Benchmarking Maturation..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(600), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        Maturation maturation;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 200; ++step) {
            maturation.update(brain.get(), *brain->getRandomGenerator());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Maturation";
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = 200;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkCuriosity() {
        std::cout << "Benchmarking Curiosity system..." << std::endl;
        
        Curiosity curiosity;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t updateCount = 0;
        for (size_t step = 0; step < 10000; ++step) {
            float novelty = static_cast<float>(rand()) / RAND_MAX;
            float predictionError = static_cast<float>(rand()) / RAND_MAX;
            curiosity.update(novelty, predictionError, 0.001f);
            ++updateCount;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Curiosity";
        result.steps = 10000;
        result.totalTime = totalTime;
        result.opsPerSecond = updateCount / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkNovelty() {
        std::cout << "Benchmarking Novelty system..." << std::endl;
        
        Novelty novelty;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t updateCount = 0;
        for (size_t step = 0; step < 10000; ++step) {
            novelty.update(static_cast<float>(step) * 0.001f);
            ++updateCount;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Novelty";
        result.steps = 10000;
        result.totalTime = totalTime;
        result.opsPerSecond = updateCount / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkPredictionError() {
        std::cout << "Benchmarking PredictionError system..." << std::endl;
        
        PredictionError predictionError;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t updateCount = 0;
        for (size_t step = 0; step < 10000; ++step) {
            predictionError.update(static_cast<float>(step) * 0.001f);
            ++updateCount;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "PredictionError";
        result.steps = 10000;
        result.totalTime = totalTime;
        result.opsPerSecond = updateCount / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkWorkingMemory() {
        std::cout << "Benchmarking WorkingMemory system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("memory_capacity", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("trace_length", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto memory = std::make_shared<NeuralWorkingMemory>(config);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t operationCount = 0;
        for (size_t step = 0; step < 5000; ++step) {
            memory->store(static_cast<float>(step), step % 100, step % 10);
            float value = memory->recall(step % 100, step % 10);
            if (value != 0.0f) ++operationCount;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "WorkingMemory";
        result.steps = 5000;
        result.totalTime = totalTime;
        result.opsPerSecond = operationCount / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkEpisodicMemory() {
        std::cout << "Benchmarking EpisodicMemory system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("episode_length", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto memory = std::make_shared<NeuralEpisodicMemory>(config);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t operationCount = 0;
        for (size_t step = 0; step < 3000; ++step) {
            memory->store(step, static_cast<float>(step), std::vector<float>(10, static_cast<float>(step % 10)));
            auto episode = memory->recall(step);
            if (episode != nullptr) ++operationCount;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "EpisodicMemory";
        result.steps = 3000;
        result.totalTime = totalTime;
        result.opsPerSecond = operationCount / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static BenchmarkConfig benchmarkFullBrainSimulation() {
        std::cout << "Benchmarking full brain simulation..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.03f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 800; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkConfig result;
        result.name = "Full Brain Simulation";
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = 800;
        result.totalTime = totalTime;
        result.opsPerSecond = result.steps / totalTime;
        
        printBenchmarkResult(result);
        return result;
    }
    
    static void printBenchmarkResult(const BenchmarkConfig& result) {
        std::cout << "  " << std::left << std::setw(30) << result.name
                  << " | Neurons: " << std::setw(6) << result.neurons
                  << " | Synapses: " << std::setw(6) << result.synapses
                  << " | Steps/sec: " << std::fixed << std::setprecision(1) << result.opsPerSecond
                  << " | Time: " << std::fixed << std::setprecision(3) << result.totalTime << "s";
        
        if (result.memoryUsage > 0) {
            std::cout << " | Memory: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB";
        }
        
        std::cout << std::endl;
    }
    
    static void printBenchmarkReport(const std::vector<BenchmarkConfig>& results) {
        std::cout << "\nNLM Performance Benchmark Report" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        std::cout << std::left << std::setw(30) << "Test Name"
                  << " | Neurons" << std::setw(6) << " | Synapses"
                  << " | Steps/sec" << std::setw(10) << " | Time (s)";
        if (containsMemoryMetrics(results)) {
            std::cout << " | Memory (MB)";
        }
        std::cout << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(30) << result.name
                      << " | " << std::setw(6) << result.neurons
                      << " | " << std::setw(6) << result.synapses
                      << " | " << std::fixed << std::setprecision(1) << std::setw(9) << result.opsPerSecond
                      << " | " << std::fixed << std::setprecision(3) << std::setw(9) << result.totalTime;
            
            if (result.memoryUsage > 0) {
                std::cout << " | " << std::fixed << std::setprecision(2) << std::setw(10) << result.memoryUsage;
            } else {
                std::cout << " | ";
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::string(70, '=') << std::endl;
        std::cout << "Total tests run: " << results.size() << std::endl;
        double totalTime = 0.0;
        for (const auto& result : results) totalTime += result.totalTime;
        std::cout << "Total benchmark time: " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    }
    
    static void printScalabilityReport(const std::vector<BenchmarkConfig>& results) {
        std::cout << "\nNLM Scalability Report" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        std::cout << std::left << std::setw(40) << "Network Size"
                  << " | Neurons" << std::setw(6) << " | Synapses"
                  << " | Steps/sec" << std::setw(10) << " | Time (s)";
        std::cout << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(40) << result.name
                      << " | " << std::setw(6) << result.neurons
                      << " | " << std::setw(6) << result.synapses
                      << " | " << std::fixed << std::setprecision(1) << std::setw(9) << result.opsPerSecond
                      << " | " << std::fixed << std::setprecision(3) << std::setw(9) << result.totalTime;
            std::cout << std::endl;
        }
        
        std::cout << std::string(70, '=') << std::endl;
    }
    
    static bool containsMemoryMetrics(const std::vector<BenchmarkConfig>& results) {
        for (const auto& result : results) {
            if (result.memoryUsage > 0) return true;
        }
        return false;
    }
    
    static void saveBenchmarkResults(const std::vector<BenchmarkConfig>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open file " << filename << " for writing" << std::endl;
            return;
        }
        
        file << "{\"benchmarks\": [" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "  {\"name\": \"" << result.name << "\", \"neurons\": " << result.neurons
                 << ", \"synapses\": " << result.synapses << ", \"steps\": " << result.steps
                 << ", \"totalTime\": " << result.totalTime << ", \"opsPerSecond\": " << result.opsPerSecond
                 << ", \"memoryUsage\": " << result.memoryUsage << "}";
            
            if (i < results.size() - 1) {
                file << ",";
            }
            file << std::endl;
        }
        file << "],\"totalBenchmarks\": " << results.size() << "}" << std::endl;
        
        file.close();
        std::cout << "Benchmark results saved to " << filename << std::endl;
    }
};

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM Benchmark Suite — 熙然                            ║
    ║     Neural Learning Machine Performance Testing                ║
    ║                                                               ║
    ║     Comprehensive Performance Measurements                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Starting NLM Performance Benchmark Suite..." << std::endl;
    std::cout << "This will run comprehensive benchmarks for all NLM subsystems." << std::endl;
    std::cout << std::endl;
    
    // Run all benchmarks
    NLMBenchmarkSuite::runBenchmarks();
    
    // Run scalability tests
    NLMBenchmarkSuite::runScalabilityTests();
    
    // Run memory usage tests
    NLMBenchmarkSuite::runMemoryUsageTests();
    
    // Run integration performance tests
    NLMBenchmarkSuite::runIntegrationPerformanceTests();
    
    std::cout << std::endl;
    std::cout << "All benchmark suites completed successfully!" << std::endl;
    
    return 0;
}