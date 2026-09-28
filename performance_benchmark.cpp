#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

class PerformanceBenchmarker {
public:
    struct BenchmarkResults {
        std::string benchmarkName;
        double executionTime;  // seconds
        uint64_t operationsPerformed;
        double operationsPerSecond;
        std::map<std::string, double> metrics;
        std::string timestamp;
        std::string configSummary;
    };
    
    PerformanceBenchmarker() {
        setupBenchmarks();
    }
    
    void runBenchmark(const std::string& benchmarkName, const std::string& description) {
        std::cout << "=== Running Benchmark: " << benchmarkName << " ===" << std::endl;
        std::cout << "Description: " << description << std::endl;
        std::cout << "Starting benchmark..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        BenchmarkPhase(benchmarkName);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResults result;
        result.benchmarkName = benchmarkName;
        result.executionTime = duration.count() / 1000.0;
        result.operationsPerformed = getOperationsCount(benchmarkName);
        result.operationsPerSecond = result.operationsPerformed / result.executionTime;
        result.timestamp = getCurrentTimestamp();
        
        std::cout << "Benchmark completed in " << std::fixed << std::setprecision(3) 
                  << result.executionTime << " seconds" << std::endl;
        std::cout << "Operations performed: " << result.operationsPerformed << std::endl;
        std::cout << "Operations per second: " << std::fixed << std::setprecision(2) 
                  << result.operationsPerSecond << std::endl;
        
        storeResults(result);
        displayResults(result);
    }
    
    void runSuite() {
        std::cout << "=== NLM Performance Benchmark Suite ===" << std::endl;
        std::cout << "Running comprehensive benchmark suite..." << std::endl;
        std::cout << std::endl;
        
        for (const auto& benchmark : benchmarks_) {
            std::cout << "\n";
            runBenchmark(benchmark.name, benchmark.description);
            std::cout << std::endl;
        }
        
        generateReport();
    }
    
    void exportResults(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Benchmark Results" << std::endl;
        file << "Generated: " << getCurrentTimestamp() << std::endl;
        file << std::endl;
        
        for (const auto& result : results_) {
            file << "=== " << result.benchmarkName << " ===" << std::endl;
            file << "Execution time: " << std::fixed << std::setprecision(3) << result.executionTime << "s" << std::endl;
            file << "Operations: " << result.operationsPerformed << std::endl;
            file << "Ops/sec: " << std::fixed << std::setprecision(2) << result.operationsPerSecond << std::endl;
            file << "Timestamp: " << result.timestamp << std::endl;
            
            if (!result.configSummary.empty()) {
                file << "Configuration: " << result.configSummary << std::endl;
            }
            
            file << "Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                file << "  " << metric.first << ": " << metric.second << std::endl;
            }
            
            file << std::endl;
        }
        
        file.close();
        std::cout << "Benchmark results exported to: " << filename << std::endl;
    }
    
    void displayResults(const BenchmarkResults& result) {
        std::cout << "\nBenchmark Details:" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Name: " << result.benchmarkName << std::endl;
        std::cout << "Time: " << std::fixed << std::setprecision(3) << result.executionTime << "s" << std::endl;
        std::cout << "Operations: " << result.operationsPerformed << std::endl;
        std::cout << "Performance: " << std::fixed << std::setprecision(2) << result.operationsPerSecond << " ops/sec" << std::endl;
        
        if (!result.metrics.empty()) {
            std::cout << "\nDetailed Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                std::cout << "  " << std::left << std::setw(25) << metric.first << ": " 
                          << std::fixed << std::setprecision(4) << metric.second << std::endl;
            }
        }
    }
    
    void compareWithBaseline(const std::string& baselineFile) {
        std::cout << "Comparing with baseline results..." << std::endl;
        
        std::vector<BenchmarkResults> baselineResults = loadBaseline(baselineFile);
        if (baselineResults.empty()) {
            std::cout << "Could not load baseline results" << std::endl;
            return;
        }
        
        std::cout << "\nBenchmark Comparison:" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        for (const auto& current : results_) {
            auto baselineIt = std::find_if(baselineResults.begin(), baselineResults.end(),
                [&current](const BenchmarkResults& b) { return b.benchmarkName == current.benchmarkName; });
            
            if (baselineIt != baselineResults.end()) {
                double timeImprovement = (baselineIt->executionTime - current.executionTime) / baselineIt->executionTime * 100.0;
                double perfImprovement = (baselineIt->operationsPerSecond - current.operationsPerSecond) / baselineIt->operationsPerSecond * 100.0;
                
                std::cout << std::left << std::setw(25) << current.benchmarkName << " (current)" << std::endl;
                std::cout << std::left << std::setw(25) << "Time: " << std::fixed << std::setprecision(3) 
                          << current.executionTime << "s (baseline: " << baselineIt->executionTime << "s)" << std::endl;
                std::cout << std::left << std::setw(25) << "Performance: " << std::fixed << std::setprecision(2) 
                          << current.operationsPerSecond << " ops/sec (baseline: " << baselineIt->operationsPerSecond << " ops/sec)" << std::endl;
                std::cout << std::left << std::setw(25) << "Time improvement: " << std::fixed << std::setprecision(2) 
                          << timeImprovement << "%" << std::endl;
                std::cout << std::left << std::setw(25) << "Performance improvement: " << std::fixed << std::setprecision(2) 
                          << perfImprovement << "%" << std::endl;
                std::cout << std::endl;
            }
        }
    }
    
private:
    struct Benchmark {
        std::string name;
        std::string description;
        std::function<void()> function;
        std::function<uint64_t()> getOperationCount;
    };
    
    std::vector<Benchmark> benchmarks_;
    std::vector<BenchmarkResults> results_;
    
    void setupBenchmarks() {
        // Neuron processing benchmark
        benchmarks_.push_back({
            "neuron_processing",
            "Benchmark neuron update operations in LIF dynamics",
            [this]() { runNeuronProcessingBenchmark(); },
            [this]() { return getNeuronProcessingOperations(); }
        });
        
        // Synapse connectivity benchmark
        benchmarks_.push_back({
            "synapse_connectivity",
            "Benchmark synaptic weight updates and plasticity",
            [this]() { runSynapseConnectivityBenchmark(); },
            [this]() { return getSynapseConnectivityOperations(); }
        });
        
        // Memory system benchmark
        benchmarks_.push_back({
            "memory_systems",
            "Benchmark working memory and episodic memory operations",
            [this]() { runMemorySystemsBenchmark(); },
            [this]() { return getMemorySystemsOperations(); }
        });
        
        // Neuromodulation benchmark
        benchmarks_.push_back({
            "neuromodulation",
            "Benchmark neuromodulator update and integration",
            [this]() { runNeuromodulationBenchmark(); },
            [this]() { return getNeuromodulationOperations(); }
        });
        
        // Spike propagation benchmark
        benchmarks_.push_back({
            "spike_propagation",
            "Benchmark spike event processing and delayed spike delivery",
            [this]() { runSpikePropagationBenchmark(); },
            [this]() { return getSpikePropagationOperations(); }
        });
        
        // Integration test benchmark
        benchmarks_.push_back({
            "integration_test",
            "Benchmark complete integration experiment execution",
            [this]() { runIntegrationTestBenchmark(); },
            [this]() { return getIntegrationTestOperations(); }
        });
        
        // Agent control benchmark
        benchmarks_.push_back({
            "agent_control",
            "Benchmark agent sensory processing and motor decoding",
            [this]() { runAgentControlBenchmark(); },
            [this]() { return getAgentControlOperations(); }
        });
    }
    
    void BenchmarkPhase(const std::string& name) {
        // Create configuration for benchmark
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.05f);
        
        // Initialize brain
        currentBrain_ = std::make_shared<Brain>(config);
        if (currentBrain_->initialize()) {
            std::cout << "Brain initialized successfully" << std::endl;
        }
        
        // Create world and agent for certain benchmarks
        if (name == "agent_control" || name == "integration_test") {
            currentWorld_ = std::make_shared<SimpleWorld>();
            currentWorld_->initialize(16, 16);
            
            currentAgent_ = std::make_shared<AgentBrain>(currentBrain_);
            currentAgent_->initialize(*currentWorld_);
        }
        
        // Store config summary for results
        configSummary_ = config->summary();
    }
    
    void runNeuronProcessingBenchmark() {
        uint64_t totalUpdates = 0;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 1000; ++step) {
            for (auto& region : currentBrain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->stepLIF(step * 0.001, 0.001);
                        totalUpdates++;
                    }
                }
            }
        }
        
        neuronUpdates_ = totalUpdates;
    }
    
    uint64_t getNeuronProcessingOperations() const {
        return neuronUpdates_;
    }
    
    void runSynapseConnectivityBenchmark() {
        uint64_t totalUpdates = 0;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 500; ++step) {
            for (auto& region : currentBrain_->getRegions()) {
                for (auto& syn : region->getSynapses()) {
                    // Simulate STDP and Hebbian updates
                    syn->step(step * 0.001);
                    totalUpdates++;
                }
            }
        }
        
        synapseUpdates_ = totalUpdates;
    }
    
    uint64_t getSynapseConnectivityOperations() const {
        return synapseUpdates_;
    }
    
    void runMemorySystemsBenchmark() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Store some episodes
        for (int i = 0; i < 100; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.reward = 0.5f;
            
            for (size_t j = 0; j < 10; ++j) {
                episode.activeNeurons.push_back(j);
                episode.neuronActivations.push_back(0.1f * j);
            }
            
            if (currentBrain_->getEpisodicMemory()) {
                currentBrain_->getEpisodicMemory()->storeEpisode(episode);
            }
        }
        
        // Retrieve episodes
        if (currentBrain_->getEpisodicMemory()) {
            auto episodes = currentBrain_->getEpisodicMemory()->getRecentEpisodes(10);
            // Do something with episodes to prevent optimization
            (void)episodes.size();
        }
        
        memoryOperations_ = 100; // 100 stores + 100 retrieves
    }
    
    uint64_t getMemorySystemsOperations() const {
        return memoryOperations_;
    }
    
    void runNeuromodulationBenchmark() {
        uint64_t totalUpdates = 0;
        
        for (int step = 0; step < 200; ++step) {
            // Update neuromodulators
            if (currentAgent_) {
                currentAgent_->applyRewardModulation(0.5f, 0.0f);
                totalUpdates++;
            }
        }
        
        neuromodUpdates_ = totalUpdates;
    }
    
    uint64_t getNeuromodulationOperations() const {
        return neuromodUpdates_;
    }
    
    void runSpikePropagationBenchmark() {
        uint64_t totalSpikes = 0;
        
        // Inject some current to generate spikes
        for (int step = 0; step < 100; ++step) {
            // Inject current to create spikes
            for (size_t i = 0; i < currentBrain_->getTotalNeuronCount() / 10; ++i) {
                currentBrain_->injectCurrentToNeurons(NeuronType::Internal, 20.0f);
            }
            
            currentBrain_->step(step, step * 0.001);
            totalSpikes += currentBrain_->getTotalSpikeCount();
        }
        
        spikeEvents_ = totalSpikes;
    }
    
    uint64_t getSpikePropagationOperations() const {
        return spikeEvents_;
    }
    
    void runIntegrationTestBenchmark() {
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 1000;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        currentExperiment_ = Phase6IntegratedExperiment();
        auto result = currentExperiment_.run(config);
        
        integrationSteps_ = config.maxSteps;
        integrationReward_ = result.totalReward;
    }
    
    uint64_t getIntegrationTestOperations() const {
        return integrationSteps_;
    }
    
    void runAgentControlBenchmark() {
        if (!currentAgent_ || !currentWorld_) {
            return;
        }
        
        uint64_t totalOperations = 0;
        
        for (int step = 0; step < 300; ++step) {
            // Create a simple percept
            SensoryPercept percept;
            percept.vision.resize(16, 0.0f);
            
            // Process sensory input
            currentAgent_->processSensoryInput(percept);
            
            // Decode motor command
            MotorCommand cmd = currentAgent_->decodeMotorCommand();
            
            // Apply reward
            currentAgent_->applyRewardModulation(0.1f, 0.0f);
            
            totalOperations += 3; // process, decode, reward
        }
        
        agentOperations_ = totalOperations;
    }
    
    uint64_t getAgentControlOperations() const {
        return agentOperations_;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void storeResults(const BenchmarkResults& result) {
        results_.push_back(result);
    }
    
    void generateReport() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARK SUITE SUMMARY" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        std::cout << "Total benchmarks run: " << results_.size() << std::endl;
        double totalTime = 0.0;
        uint64_t totalOps = 0;
        
        for (const auto& result : results_) {
            totalTime += result.executionTime;
            totalOps += result.operationsPerformed;
        }
        
        std::cout << "Total execution time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        std::cout << "Total operations: " << totalOps << std::endl;
        std::cout << "Overall performance: " << std::fixed << std::setprecision(2) 
                  << (totalOps / totalTime) << " ops/sec" << std::endl;
        
        std::cout << "\nIndividual benchmark results:" << std::endl;
        std::cout << std::left << std::setw(25) << "Benchmark" 
                  << std::setw(12) << "Time (s)" 
                  << std::setw(15) << "Operations" 
                  << std::setw(15) << "Ops/sec" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (const auto& result : results_) {
            std::cout << std::left << std::setw(25) << result.benchmarkName
                      << std::setw(12) << std::fixed << std::setprecision(3) << result.executionTime
                      << std::setw(15) << result.operationsPerformed
                      << std::setw(15) << std::fixed << std::setprecision(2) << result.operationsPerSecond
                      << std::endl;
        }
    }
    
    std::vector<BenchmarkResults> loadBaseline(const std::string& filename) {
        std::vector<BenchmarkResults> results;
        // Implement baseline loading logic
        return results;
    }
    
    // Variables to store benchmark state
    std::shared_ptr<Config> currentConfig_;
    std::shared_ptr<Brain> currentBrain_;
    std::shared_ptr<SimpleWorld> currentWorld_;
    std::shared_ptr<AgentBrain> currentAgent_;
    Phase6IntegratedExperiment currentExperiment_;
    std::string configSummary_;
    
    // Benchmark counters
    uint64_t neuronUpdates_ = 0;
    uint64_t synapseUpdates_ = 0;
    uint64_t memoryOperations_ = 0;
    uint64_t neuromodUpdates_ = 0;
    uint64_t spikeEvents_ = 0;
    uint64_t integrationSteps_ = 0;
    float integrationReward_ = 0.0f;
    uint64_t agentOperations_ = 0;
};  // namespace nlm

// Advanced benchmarking interface functions
void runBenchmarks() {
    PerformanceBenchmarker benchmarker;
    benchmarker.runSuite();
}

void runSingleBenchmark(const std::string& benchmarkName) {
    PerformanceBenchmarker benchmarker;
    // Implement single benchmark functionality
}

void exportBenchmarkResults(const std::string& filename) {
    PerformanceBenchmarker benchmarker;
    benchmarker.exportResults(filename);
}

void compareWithPreviousRun() {
    PerformanceBenchmarker benchmarker;
    benchmarker.compareWithBaseline("baseline_results.json");
}

int main(int argc, char** argv) {
    std::cout << "=== NLM Performance Benchmarking Tool ===" << std::endl;
    std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl;
    std::cout << std::endl;
    
    PerformanceBenchmarker benchmarker;
    
    std::string line;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        if (line == "exit" || line == "quit") {
            std::cout << "Exiting benchmarking tool. Goodbye!" << std::endl;
            break;
        }
        
        if (line == "help") {
            std::cout << "=== Available Commands ===" << std::endl;
            std::cout << "  run_suite       - Run complete benchmark suite" << std::endl;
            std::cout << "  export_results  - Export benchmark results to file" << std::endl;
            std::cout << "  compare_baseline - Compare with previous baseline" << std::endl;
            std::cout << "  help           - Show this help message" << std::endl;
            std::cout << "  exit/quit      - Exit the benchmarking tool" << std::endl;
            continue;
        }
        
        if (line == "run_suite") {
            benchmarker.runSuite();
        } else if (line == "export_results") {
            std::cout << "Enter filename for export: ";
            std::string filename;
            std::getline(std::cin, filename);
            if (!filename.empty()) {
                benchmarker.exportResults(filename);
            }
        } else if (line == "compare_baseline") {
            benchmarker.compareWithBaseline("baseline_results.json");
        } else {
            std::cout << "Unknown command: " << line << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    return 0;
}
