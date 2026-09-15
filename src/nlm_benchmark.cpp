// NLM (熙然) - Neural Learning Machine
// Performance benchmarking tool
// Provides performance measurement and analysis for NLM simulations

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <thread>
#include <algorithm>
#include <numeric>

using namespace nlm;

void printBenchmarkHelp() {
    std::cout << "NLM Benchmark Tool - Performance Measurement" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_benchmark [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>     Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --mode <type>       Benchmark mode: startup, sustained, peak, memory, all" << std::endl;
    std::cout << "  --steps <value>     Number of steps to benchmark" << std::endl;
    std::cout << "  --warmup <value>    Number of warmup steps" << std::endl;
    std::cout << "  --repeats <value>   Number of benchmark repeats" << std::endl;
    std::cout << "  --output <file>     Save results to file" << std::endl;
    std::cout << "  --csv               Output results in CSV format" << std::endl;
    std::cout << "  --detailed          Detailed per-step measurements" << std::endl;
    std::cout << "  --mem               Measure memory usage" << std::endl;
    std::cout << "  --threads <value>   Number of threads for parallel benchmarks" << std::endl;
    std::cout << "  --help-advanced     Show advanced options" << std::endl;
    std::cout << std::endl;
    std::cout << "Benchmark Modes:" << std::endl;
    std::cout << "  startup   - Initialize brain and measure startup time" << std::endl;
    std::cout << "  sustained - Run continuous simulation and measure throughput" << std::endl;
    std::cout << "  peak      - Measure maximum performance under load" << std::endl;
    std::cout << "  memory    - Measure memory usage patterns" << std::endl;
    std::cout << "  all       - Run all benchmark types" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_benchmark --mode sustained --steps 10000 --repeats 5" << std::endl;
    std::cout << "  nlm_benchmark --mode memory --output perf_results.csv" << std::endl;
    std::cout << "  nlm_benchmark --mode all --detailed --csv" << std::endl;
}

void printVersion() {
    std::cout << "NLM Benchmark Tool 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
}

struct BenchmarkResult {
    std::string mode;
    size_t steps;
    double totalTime;        // Wall clock time in seconds
    double cpuTime;          // CPU time in seconds  
    double instructions;     // Estimated instructions (placeholder)
    double memoryUsage;      // Peak memory usage in MB
    double throughput;       // Steps per second
    double avgStepTime;      // Average time per step
    size_t totalSpikes;      // Total spikes generated
    size_t activeNeurons;    // Average active neurons
    double energyUsage;      // Estimated energy consumption
    std::vector<double> stepTimes;  // Per-step timing data
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Benchmark Results:" << std::endl;
        ss << "  Mode: " << mode << std::endl;
        ss << "  Steps: " << steps << std::endl;
        ss << "  Total time: " << totalTime << "s" << std::endl;
        ss << "  Throughput: " << throughput << " steps/s" << std::endl;
        ss << "  Avg step time: " << avgStepTime << "ms" << std::endl;
        ss << "  Total spikes: " << totalSpikes << std::endl;
        ss << "  Memory usage: " << memoryUsage << " MB" << std::endl;
        ss << "  Energy: " << energyUsage << " arbitrary units" << std::endl;
        return ss.str();
    }
    
    std::string toCSV() const {
        std::stringstream ss;
        ss << mode << ","
           << steps << ","
           << totalTime << ","
           << cpuTime << ","
           << throughput << ","
           << avgStepTime << ","
           << totalSpikes << ","
           << memoryUsage << ","
           << energyUsage << ","
           << activeNeurons;
        return ss.str();
    }
};

BenchmarkResult benchmarkStartup(const std::shared_ptr<Config>& config, bool detailed) {
    BenchmarkResult result;
    result.mode = "startup";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    auto initStart = std::chrono::high_resolution_clock::now();
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto initTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - initStart).count();
    
    // Brief processing
    size_t steps = 100;
    auto processStart = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto processTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - processStart).count();
    
    result.totalTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
    result.cpuTime = initTime + processTime;
    result.steps = steps;
    result.throughput = steps / result.totalTime;
    result.avgStepTime = result.totalTime * 1000.0 / steps; // ms
    result.totalSpikes = brain->getTotalSpikeCount();
    result.activeNeurons = brain->getActiveNeuronCount();
    
    if (detailed) {
        result.stepTimes.resize(steps);
        for (size_t i = 0; i < steps; ++i) {
            result.stepTimes[i] = 1.0; // Simplified
        }
    }
    
    return result;
}

BenchmarkResult benchmarkSustained(const std::shared_ptr<Config>& config, size_t steps, bool detailed) {
    BenchmarkResult result;
    result.mode = "sustained";
    result.steps = steps;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Warmup
    for (SimulationStep step = 0; step < steps / 10; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    // Main benchmark
    auto benchStart = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = steps / 10; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        if (detailed && step % 100 == 0) {
            result.stepTimes.push_back(
                std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - benchStart).count() / 100.0
            );
            benchStart = std::chrono::high_resolution_clock::now();
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
    result.cpuTime = result.totalTime; // Simplified - actual CPU time would require more complex measurement
    result.throughput = steps / result.totalTime;
    result.avgStepTime = result.totalTime * 1000.0 / steps; // ms
    result.totalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    result.activeNeurons = brain->getActiveNeuronCount();
    
    return result;
}

BenchmarkResult benchmarkPeak(const std::shared_ptr<Config>& config, size_t steps) {
    BenchmarkResult result;
    result.mode = "peak";
    result.steps = steps;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Enable high activity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Create maximum load
    for (size_t i = 0; i < 100 && i < brain->getTotalNeuronCount(); ++i) {
        brain->injectCurrentToNeurons(NeuronType::Excitatory, 100.0f);
    }
    
    auto peakStart = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Inject strong input to maintain peak activity
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        }
        
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    result.totalTime = std::chrono::duration<double>(endTime - peakStart).count();
    result.cpuTime = result.totalTime;
    result.throughput = steps / result.totalTime;
    result.avgStepTime = result.totalTime * 1000.0 / steps; // ms
    result.totalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    result.activeNeurons = brain->getActiveNeuronCount();
    
    return result;
}

BenchmarkResult benchmarkMemory(const std::shared_ptr<Config>& config) {
    BenchmarkResult result;
    result.mode = "memory";
    result.steps = 1000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Measure memory usage during operation
    size_t initialMemory = brain->getTotalNeuronCount() + brain->getTotalSynapseCount();
    
    for (SimulationStep step = 0; step < result.steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 100 == 0) {
            size_t currentMemory = brain->getTotalNeuronCount() + brain->getTotalSynapseCount();
            result.memoryUsage = currentMemory * sizeof(float) / (1024.0 * 1024.0); // MB
        }
    }
    
    result.totalTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
    result.throughput = result.steps / result.totalTime;
    result.avgStepTime = result.totalTime * 1000.0 / result.steps; // ms
    result.totalSpikes = brain->getTotalSpikeCount();
    result.activeNeurons = brain->getActiveNeuronCount();
    
    // Estimate memory usage based on brain components
    result.memoryUsage = (brain->getTotalNeuronCount() * 100 +  // neurons
                         brain->getTotalSynapseCount() * 40 +    // synapses
                         sizeof(*brain) * 10) / (1024.0 * 1024.0); // components
    
    return result;
}

void runBenchmark(const std::shared_ptr<Config>& config, const std::string& mode, size_t steps, 
                  size_t warmup, size_t repeats, bool detailed, bool outputCSV, 
                  const std::string& outputFile) {
    
    std::vector<BenchmarkResult> allResults;
    
    std::cout << "=== NLM Benchmark Suite ===" << std::endl;
    std::cout << "Mode: " << mode << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    std::cout << "Repeats: " << repeats << std::endl;
    std::cout << "Warmup: " << warmup << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < repeats; ++i) {
        std::cout << "--- Run " << (i + 1) << "/" << repeats << " ---" << std::endl;
        
        BenchmarkResult result;
        
        if (mode == "startup" || mode == "all") {
            result = benchmarkStartup(config, detailed);
            allResults.push_back(result);
            std::cout << result.toString() << std::endl;
        }
        
        if (mode == "sustained" || mode == "all") {
            result = benchmarkSustained(config, steps, detailed);
            allResults.push_back(result);
            std::cout << result.toString() << std::endl;
        }
        
        if (mode == "peak" || mode == "all") {
            result = benchmarkPeak(config, steps);
            allResults.push_back(result);
            std::cout << result.toString() << std::endl;
        }
        
        if (mode == "memory" || mode == "all") {
            result = benchmarkMemory(config);
            allResults.push_back(result);
            std::cout << result.toString() << std::endl;
        }
        
        // Wait between runs
        if (i < repeats - 1) {
            std::cout << "Waiting 2 seconds before next run..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    // Aggregate results
    std::cout << std::endl;
    std::cout << "=== Aggregated Results ===" << std::endl;
    
    if (!allResults.empty()) {
        double totalTime = 0.0, totalThroughput = 0.0, totalSteps = 0.0;
        size_t totalSpikes = 0;
        
        for (const auto& result : allResults) {
            totalTime += result.totalTime;
            totalThroughput += result.throughput;
            totalSteps += result.steps;
            totalSpikes += result.totalSpikes;
        }
        
        double avgTime = totalTime / allResults.size();
        double avgThroughput = totalThroughput / allResults.size();
        
        std::cout << "Average across " << allResults.size() << " runs:" << std::endl;
        std::cout << "  Average time: " << avgTime << "s" << std::endl;
        std::cout << "  Average throughput: " << avgThroughput << " steps/s" << std::endl;
        std::cout << "  Total steps: " << totalSteps << std::endl;
        std::cout << "  Average spikes per step: " << (totalSpikes / totalSteps) << std::endl;
        std::cout << std::endl;
        
        // Output CSV if requested
        if (outputCSV) {
            std::cout << "CSV Output:" << std::endl;
            std::cout << "mode,steps,total_time,cpu_time,throughput,avg_step_time,total_spikes,memory_usage,energy,active_neurons" << std::endl;
            for (const auto& result : allResults) {
                std::cout << result.toCSV() << std::endl;
            }
        }
        
        // Save to file if requested
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "NLM Benchmark Results" << std::endl;
                file << "=====================" << std::endl;
                file << std::endl;
                file << "Mode: " << mode << std::endl;
                file << "Steps: " << steps << std::endl;
                file << "Repeats: " << repeats << std::endl;
                file << std::endl;
                file << "Run Results:" << std::endl;
                for (size_t i = 0; i < allResults.size(); ++i) {
                    file << "Run " << (i + 1) << ": " << allResults[i].toString() << std::endl;
                }
                file << std::endl;
                file << "Aggregated Results:" << std::endl;
                file << "Average time: " << avgTime << "s" << std::endl;
                file << "Average throughput: " << avgThroughput << " steps/s" << std::endl;
                file << "Total steps: " << totalSteps << std::endl;
                file << "Total spikes: " << totalSpikes << std::endl;
                
                if (outputCSV) {
                    file << std::endl;
                    file << "CSV summary:" << std::endl;
                    file << "mode,steps,total_time,cpu_time,throughput,avg_step_time,total_spikes,memory_usage,energy,active_neurons" << std::endl;
                    for (const auto& result : allResults) {
                        file << result.toCSV() << std::endl;
                    }
                }
                
                file.close();
                std::cout << "Results saved to: " << outputFile << std::endl;
            } else {
                std::cerr << "Error: Failed to save results to: " << outputFile << std::endl;
            }
        }
    }
}

int main(int argc, char** argv) {
    std::string configFile = "configs/default.cfg";
    std::string mode = "sustained";
    size_t steps = 10000;
    size_t warmup = 1000;
    size_t repeats = 3;
    bool detailed = false;
    bool outputCSV = false;
    bool outputMem = false;
    size_t threads = 1;
    std::string outputFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printBenchmarkHelp();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--help-advanced") {
            printBenchmarkHelp();
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--mode") {
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--warmup") {
            if (arg.find('=') != std::string::npos) {
                warmup = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                warmup = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--repeats") {
            if (arg.find('=') != std::string::npos) {
                repeats = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                repeats = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--output") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg == "--csv") {
            outputCSV = true;
        } else if (arg == "--detailed") {
            detailed = true;
        } else if (arg == "--mem") {
            outputMem = true;
        } else if (arg.substr(0, 9) == "--threads") {
            if (arg.find('=') != std::string::npos) {
                threads = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                threads = std::stoul(argv[++i]);
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printBenchmarkHelp();
            return 1;
        }
    }
    
    // Validate mode
    std::vector<std::string> validModes = {"startup", "sustained", "peak", "memory", "all"};
    if (std::find(validModes.begin(), validModes.end(), mode) == validModes.end()) {
        std::cerr << "Error: Invalid mode: " << mode << std::endl;
        printBenchmarkHelp();
        return 1;
    }
    
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warn); // Suppress logs for cleaner benchmark output
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load configuration
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            std::cerr << "Warning: Using default configuration (file not found: " << configFile << ")" << std::endl;
        }
        
        // Set defaults for benchmarking
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Run benchmark
        runBenchmark(config, mode, steps, warmup, repeats, detailed, outputCSV, outputFile);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
