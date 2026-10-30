// Performance Analysis Tool Main
// Command-line tool for analyzing brain performance and collecting statistics

#include "performance/PerformanceMonitoring.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

using namespace nlm;

class PerformanceAnalysisTool {
public:
    PerformanceAnalysisTool(int argc, char** argv) : argc_(argc), argv_(argv) {}
    
    int run() {
        if (argc_ < 2) {
            printHelp();
            return 0;
        }
        
        std::string command = argv_[1];
        
        if (command == "analyze") {
            return runAnalyzeCommand();
        } else if (command == "profile") {
            return runProfileCommand();
        } else if (command == "benchmark") {
            return runBenchmarkCommand();
        } else if (command == "stats") {
            return runStatsCommand();
        } else if (command == "help") {
            printHelp();
            return 0;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            printHelp();
            return 1;
        }
    }
    
private:
    int runAnalyzeCommand() {
        std::cout << "=== Brain Performance Analysis ===" << std::endl;
        
        std::string brain_file = "brain.bin";
        std::string output_file = "analysis_report.txt";
        
        if (argc_ >= 3) {
            brain_file = argv_[2];
        }
        if (argc_ >= 4) {
            output_file = argv_[3];
        }
        
        // Load brain from checkpoint
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(brain_file)) {
            std::cout << "Error: Failed to load brain from " << brain_file << std::endl;
            return 1;
        }
        
        std::cout << "✓ Brain loaded successfully!" << std::endl;
        
        // Create performance analyzer
        BrainPerformanceAnalyzer analyzer(brain);
        auto stats = analyzer.analyze();
        
        // Display analysis
        analyzer.printBrainStatistics(stats);
        
        // Save analysis to file
        std::ofstream out_file(output_file);
        if (out_file.is_open()) {
            out_file << "Brain Performance Analysis Report" << std::endl;
            out_file << "=================================" << std::endl;
            out_file << std::endl;
            
            out_file << "Configuration Summary:" << std::endl;
            out_file << "  Neurons: " << stats.neuronCount << std::endl;
            out_file << "  Synapses: " << stats.synapseCount << std::endl;
            out_file << "  Regions: " << stats.regionCount << std::endl;
            out_file << "  Populations: " << stats.populationCount << std::endl;
            out_file << std::endl;
            
            out_file << "Activity Metrics:" << std::endl;
            out_file << "  Firing rate: " << stats.averageFiringRate << " Hz" << std::endl;
            out_file << "  E/I ratio: " << stats.excitationInhibitionRatio << std::endl;
            out_file << "  Developmental stage: " << stats.developmentalStage << std::endl;
            out_file << std::endl;
            
            out_file << "Memory State:" << std::endl;
            out_file << "  Working memory: " << stats.workingMemoryActive << std::endl;
            out_file << "  Episodic memory: " << stats.episodicMemoryEpisodes << std::endl;
            out_file << "  Semantic memory: " << stats.semanticMemoryConcepts << std::endl;
            out_file << std::endl;
            
            out_file << "System Metrics:" << std::endl;
            out_file << "  Memory usage: " << stats.memoryUsageMB << " MB" << std::endl;
            out_file << std::endl;
            
            out_file.close();
            std::cout << "✓ Analysis saved to: " << output_file << std::endl;
        }
        
        return 0;
    }
    
    int runProfileCommand() {
        std::cout << "=== Brain Performance Profile ===" << std::endl;
        
        size_t steps = 1000;
        std::string brain_file = "brain.bin";
        std::string output_file = "profile_data.json";
        
        if (argc_ >= 3) {
            steps = std::stoul(argv_[2]);
        }
        if (argc_ >= 4) {
            brain_file = argv_[3];
        }
        if (argc_ >= 5) {
            output_file = argv_[4];
        }
        
        // Load or create brain
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(brain_file)) {
            std::cout << "Brain file not found, creating new brain..." << std::endl;
            config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("brain.region_count", static_cast<int64_t>(2), ConfigSource::Default);
            brain = std::make_shared<Brain>(config);
            brain->initialize();
        }
        
        std::cout << "Running performance profile for " << steps << " steps..." << std::endl;
        
        // Run performance profile
        PerformanceProfiler profiler;
        profiler.start();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            profiler.recordStep(1, brain->getFiringNeuronCount());
            
            if (step % 100 == 0) {
                std::cout << "  Progress: " << (step * 100.0 / steps) << "% ";
                std::cout << "Spike rate: " << profiler.getMetrics().spikesPerSecond << " Hz" << std::endl;
            }
        }
        
        profiler.stop();
        profiler.finalize();
        
        // Display profile results
        profiler.printReport();
        
        // Save profile data
        std::ofstream out_file(output_file);
        if (out_file.is_open()) {
            out_file << profiler.getFormattedMetrics();
            out_file.close();
            std::cout << "✓ Profile data saved to: " << output_file << std::endl;
        }
        
        return 0;
    }
    
    int runBenchmarkCommand() {
        std::cout << "=== Brain Performance Benchmark ===" << std::endl;
        
        size_t steps = 1000;
        
        if (argc_ >= 3) {
            steps = std::stoul(argv_[2]);
        }
        
        SimulationProfiler profiler;
        std::vector<SimulationProfiler::BenchmarkResult> results;
        
        // Run benchmarks with different configurations
        std::vector<std::pair<std::string, std::string>> configs = {
            {"small", "Small brain (100 neurons)"},
            {"medium", "Medium brain (500 neurons)"},
            {"large", "Large brain (1000 neurons)"}
        };
        
        for (const auto& config : configs) {
            std::cout << "Benchmarking " << config.second << "..." << std::endl;
            
            auto benchmark_config = std::make_shared<Config>();
            size_t neuron_count = (config.first == "small" ? 100 : 
                                 config.first == "medium" ? 500 : 1000);
            
            benchmark_config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            benchmark_config->set("brain.region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(benchmark_config);
            brain->initialize();
            
            auto result = profiler.benchmark(brain, config.first, config.second, steps);
            results.push_back(result);
            
            std::cout << "  Time: " << result.total_time << "s" << std::endl;
            std::cout << "  Spikes/sec: " << result.spikes_per_second << std::endl;
            std::cout << "  Memory efficiency: " << result.memory_efficiency << std::endl;
            std::cout << std::endl;
        }
        
        profiler.printBenchmarkResults(results);
        
        return 0;
    }
    
    int runStatsCommand() {
        std::cout << "=== Brain Statistics ===" << std::endl;
        
        std::string brain_file = "brain.bin";
        
        if (argc_ >= 3) {
            brain_file = argv_[2];
        }
        
        // Load brain from checkpoint
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(brain_file)) {
            std::cout << "Error: Failed to load brain from " << brain_file << std::endl;
            std::cout << "Using default configuration instead..." << std::endl;
            
            config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            brain = std::make_shared<Brain>(config);
            brain->initialize();
        }
        
        // Get brain statistics
        BrainPerformanceAnalyzer analyzer(brain);
        auto stats = analyzer.analyze();
        
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  Total neurons: " << stats.neuronCount << std::endl;
        std::cout << "  Total synapses: " << stats.synapseCount << std::endl;
        std::cout << "  Regions: " << stats.regionCount << std::endl;
        std::cout << "  Populations: " << stats.populationCount << std::endl;
        std::cout << "  Current firing: " << stats.currentFiring << std::endl;
        std::cout << "  Total spikes: " << stats.totalSpikes << std::endl;
        std::cout << "  Average firing rate: " << stats.averageFiringRate << " Hz" << std::endl;
        std::cout << "  E/I ratio: " << stats.excitationInhibitionRatio << std::endl;
        std::cout << "  Developmental stage: " << stats.developmentalStage << std::endl;
        std::cout << "  Memory usage: " << stats.memoryUsageMB << " MB" << std::endl;
        
        return 0;
    }
    
    void printHelp() {
        std::cout << "NLM Performance Analysis Tool" << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm_perf <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  analyze <brain_file> [output_file]    - Analyze brain state and performance" << std::endl;
        std::cout << "  profile <steps> [brain_file] [output] - Profile brain performance over time" << std::endl;
        std::cout << "  benchmark <steps>                      - Run performance benchmarks" << std::endl;
        std::cout << "  stats [brain_file]                    - Show brain statistics" << std::endl;
        std::cout << "  help                                  - Show this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm_perf analyze brain.bin" << std::endl;
        std::cout << "  ./nlm_perf profile 5000 brain.bin profile.json" << std::endl;
        std::cout << "  ./nlm_perf benchmark 1000" << std::endl;
        std::cout << "  ./nlm_perf stats brain.bin" << std::endl;
    }
    
    int argc_;
    char** argv_;
};

int main(int argc, char** argv) {
    PerformanceAnalysisTool tool(argc, argv);
    return tool.run();
}
