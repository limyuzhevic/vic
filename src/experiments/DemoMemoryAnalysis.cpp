// Memory Analysis Tools Demo
// This demo showcases the Memory Analysis Tools capabilities

#include "MemoryAnalysisTools.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace nlm;

class MemoryAnalysisToolsDemo {
public:
    MemoryAnalysisToolsDemo() {}
    ~MemoryAnalysisToolsDemo() {}
    
    void run() {
        std::cout << "========================================\n";
        std::cout << "MEMORY ANALYSIS TOOLS DEMO\n";
        std::cout << "========================================\n\n";
        
        // Initialize memory analysis tools
        MemoryAnalysisTools analyzer;
        
        // Create test brain with memory systems
        std::cout << "--- Creating Test Brain with Memory Systems ---\n";
        
        auto config = createConfig(1000, 4, 0.15f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable memory systems
        brain->enableWorkingMemory(true);
        brain->enableEpisodicMemory(true);
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        std::cout << "Working Memory enabled: " << (workingMemory ? "YES" : "NO") << "\n";
        std::cout << "Episodic Memory enabled: " << (episodicMemory ? "YES" : "NO") << "\n\n";
        
        // Simulate memory usage
        std::cout << "--- Simulating Memory Usage ---\n";
        
        // Create memory traces
        std::vector<uint64_t> traceIds;
        for (int i = 0; i < 100; ++i) {
            uint64_t traceId = workingMemory->createTrace(i, static_cast<float>(i) * 0.1f);
            traceIds.push_back(traceId);
        }
        
        std::cout << "Created " << traceIds.size() << " working memory traces\n";
        
        // Simulate episodic memory
        for (int i = 0; i < 50; ++i) {
            EpisodicMemory::Episode episode;
            episode.step = i * 10;
            episode.reward = static_cast<float>(i * 0.5f);
            episode.novelty = static_cast<float>(i * 0.01f);
            episode.action = static_cast<MotorCommand>(i % 6);
            
            std::string content = "Episode " + std::to_string(i) + ": Experience at step " + std::to_string(episode.step);
            episode.content = content;
            
            episodicMemory->storeEpisode(episode);
        }
        
        std::cout << "Stored " << episodicMemory->getEpisodeCount() << " episodic memories\n\n";
        
        // Analyze working memory
        std::cout << "--- Analyzing Working Memory ---\n";
        
        auto wmStats = analyzer.analyzeWorkingMemory(workingMemory);
        std::cout << "Working Memory Statistics:\n";
        std::cout << "  Active traces: " << wmStats.activeTraces << "\n";
        std::cout << "  Total capacity: " << wmStats.capacity << "\n";
        std::cout << "  Utilization ratio: " << wmStats.utilizationRatio << "\n";
        std::cout << "  Average trace strength: " << wmStats.averageTraceStrength << "\n";
        std::cout << "  Competition count: " << wmStats.competitionCount << "\n";
        std::cout << "  Conflict rate: " << wmStats.conflictRate << "\n\n";
        
        // Analyze episodic memory
        std::cout << "--- Analyzing Episodic Memory ---\n";
        
        auto emStats = analyzer.analyzeEpisodicMemory(episodicMemory);
        std::cout << "Episodic Memory Statistics:\n";
        std::cout << "  Episode count: " << emStats.episodeCount << "\n";
        std::cout << "  Average episode length: " << emStats.averageEpisodeLength << " steps\n";
        std::cout << "  Memory density: " << emStats.memoryDensity << " episodes/step\n";
        std::cout << "  Replay efficiency: " << emStats.replayEfficiency << "%\n";
        std::cout << "  Consolidation rate: " << emStats.consolidationRate << "\n";
        stdcout << "  Retrieval success rate: " << emStats.retrievalSuccessRate << "%\n\n";
        
        // Analyze memory consolidation
        std::cout << "--- Analyzing Memory Consolidation ---\n";
        
        // Simulate consolidation over time
        std::vector<double> consolidationMetrics;
        for (int timeStep = 0; timeStep < 100; ++timeStep) {
            // Simulate consolidation
            double consolidation = 0.01 * timeStep * exp(-timeStep / 50.0);
            consolidationMetrics.push_back(consolidation);
        }
        
        auto consolidationStats = analyzer.analyzeConsolidation(consolidationMetrics);
        std::cout << "Consolidation Statistics:\n";
        std::cout << "  Peak consolidation: " << consolidationStats.peakConsolidation << "\n";
        std::cout << "  Time to peak: " << consolidationStats.timeToPeak << " steps\n";
        std::cout << "  Consolidation half-life: " << consolidationStats.halfLife << " steps\n";
        std::cout << "  Final consolidation: " << consolidationStats.finalConsolidation << "\n\n";
        
        // Analyze memory replay
        std::cout << "--- Analyzing Memory Replay ---\n";
        
        // Replay some episodes
        size_t replayCount = episodicMemory->getEpisodeCount();
        auto replayedEpisodes = episodicMemory->getEpisodesForReplay(static_cast<size_t>(replayCount * 0.3f));
        
        std::cout << "Replay Analysis:\n";
        std::cout << "  Episodes replayed: " << replayedEpisodes.size() << "\n";
        std::cout << "  Replay ratio: " << (static_cast<double>(replayedEpisodes.size()) / replayCount) << "\n";
        
        // Analyze replay effectiveness
        auto replayStats = analyzer.analyzeReplay(replayedEpisodes);
        std::cout << "  Replay effectiveness: " << replayStats.effectiveness << "\n";
        std::cout << "  Pattern preservation: " << replayStats.patternPreservation << "\n";
        std::cout << "  Integration score: " << replayStats.integrationScore << "\n\n";
        
        // Profile memory allocation
        std::cout << "--- Memory Allocation Profiling ---\n";
        
        // Profile working memory allocation
        auto wmAllocProfile = analyzer.profileMemoryAllocation("working_memory", [workingMemory]() {
            for (int i = 0; i < 50; ++i) {
                uint64_t traceId = workingMemory->createTrace(i, 0.5f);
                workingMemory->updateTrace(traceId, 0.1f, i * 0.01f);
            }
        });
        
        std::cout << "Working Memory Allocation Profile:\n";
        std::cout << "  Total allocations: " << wmAllocProfile.allocationCount << "\n";
        std::cout << "  Average allocation time: " << wmAllocProfile.averageAllocationTime << "ms\n";
        std::cout << "  Peak memory usage: " << wmAllocProfile.peakMemoryUsage << "KB\n";
        std::cout << "  Memory growth rate: " << wmAllocProfile.memoryGrowthRate << "KB/step\n\n";
        
        // Profile episodic memory allocation
        auto emAllocProfile = analyzer.profileMemoryAllocation("episodic_memory", [episodicMemory]() {
            for (int i = 0; i < 20; ++i) {
                EpisodicMemory::Episode episode;
                episode.step = i * 5;
                episode.reward = static_cast<float>(i);
                episode.content = "Test episode " + std::to_string(i);
                episodicMemory->storeEpisode(episode);
            }
        });
        
        std::cout << "Episodic Memory Allocation Profile:\n";
        std::cout << "  Total allocations: " << emAllocProfile.allocationCount << "\n";
        std::cout << "  Average allocation time: " << emAllocProfile.averageAllocationTime << "ms\n";
        std::cout << "  Peak memory usage: " << emAllocProfile.peakMemoryUsage << "KB\n";
        std::cout << "  Memory growth rate: " << emAllocProfile.memoryGrowthRate << "KB/step\n\n";
        
        // Memory optimization analysis
        std::cout << "--- Memory Optimization Analysis ---\n";
        
        // Analyze memory usage patterns
        auto usagePattern = analyzer.analyzeUsagePatterns(workingMemory, episodicMemory);
        std::cout << "Usage Pattern Analysis:\n";
        std::cout << "  Peak usage time: " << usagePattern.peakUsageTime << " steps\n";
        std::cout << "  Usage periodicity: " << usagePattern.periodicity << "\n";
        std::cout << "  Predictive compression ratio: " << usagePattern.predictiveCompressionRatio << "x\n";
        std::cout << "  Compression efficiency: " << usagePattern.compressionEfficiency << "%\n\n";
        
        // Memory leak detection
        std::cout << "--- Memory Leak Detection ---\n";
        
        // Simulate memory leak detection
        std::vector<uint64_t> createdTraces;
        std::vector<uint64_t> deletedTraces;
        
        for (int i = 0; i < 200; ++i) {
            uint64_t traceId = workingMemory->createTrace(i, 0.5f);
            createdTraces.push_back(traceId);
            
            if (i % 20 == 0 && i > 0) {
                workingMemory->removeTrace(createdTraces.back());
                deletedTraces.push_back(createdTraces.back());
                createdTraces.pop_back();
            }
        }
        
        auto leakAnalysis = analyzer.detectMemoryLeaks(createdTraces, deletedTraces);
        std::cout << "Memory Leak Analysis:\n";
        std::cout << "  Leak detected: " << (leakAnalysis.leakDetected ? "YES" : "NO") << "\n";
        std::cout << "  Leak rate: " << leakAnalysis.leakRate << " leaks per 1000 operations\n";
        std::cout << "  Memory growth trend: " << leakAnalysis.growthTrend << "\n";
        std::cout << "  Suggested cleanup: " << leakAnalysis.suggestedCleanup << "\n\n";
        
        // Memory fragmentation analysis
        std::cout << "--- Memory Fragmentation Analysis ---\n";
        
        // Analyze memory fragmentation
        auto fragmentationAnalysis = analyzer.analyzeFragmentation(workingMemory);
        std::cout << "Fragmentation Analysis:\n";
        std::cout << "  Fragmentation level: " << fragmentationAnalysis.fragmentationLevel << "%\n";
        std::cout << "  Defragmentation needed: " << (fragmentationAnalysis.needsDefragmentation ? "YES" : "NO") << "\n";
        std::cout << "  Memory waste: " << fragmentationAnalysis.wastedMemory << "KB\n";
        std::cout << "  Optimization potential: " << fragmentationAnalysis.optimizationPotential << "%\n\n";
        
        // Export memory analysis
        std::cout << "--- Exporting Memory Analysis ---\n";
        
        std::string analysisPath = "/tmp/memory_analysis.json";
        analyzer.exportAnalysis(analysisPath);
        std::cout << "✓ Memory analysis exported to: " << analysisPath << "\n";
        
        // Generate memory optimization report
        std::cout << "\n--- Memory Optimization Report ---\n";
        
        std::string report = analyzer.generateOptimizationReport();
        std::cout << "Memory optimization report generated (" << report.length() << " characters)\n";
        
        // Memory comparison
        std::cout << "\n--- Memory Configuration Comparison ---\n";
        
        // Create different memory configurations
        auto config1 = createConfig(500, 2, 0.1f);
        auto config2 = createConfig(2000, 8, 0.2f);
        auto config3 = createConfig(1000, 4, 0.15f);
        
        // Analyze memory usage for each configuration
        auto config1Stats = analyzer.analyzeMemoryUsage(config1);
        auto config2Stats = analyzer.analyzeMemoryUsage(config2);
        auto config3Stats = analyzer.analyzeMemoryUsage(config3);
        
        std::cout << "Configuration Memory Usage:\n";
        std::cout << "  Config 1 (500 neurons): " << config1Stats.totalMemoryUsage << "KB\n";
        std::cout << "  Config 2 (2000 neurons): " << config2Stats.totalMemoryUsage << "KB\n";
        std::cout << "  Config 3 (1000 neurons): " << config3Stats.totalMemoryUsage << "KB\n";
        std::cout << "  Memory efficiency (config3 vs config2): " << (config3Stats.memoryEfficiency / config2Stats.memoryEfficiency) << "x\n\n";
        
        // Memory benchmarking
        std::cout << "--- Memory Benchmarking ---\n";
        
        // Benchmark memory allocation performance
        auto benchmarkConfig = createConfig(1000, 4, 0.15f);
        
        auto allocBenchmark = analyzer.benchmarkMemoryAllocation(benchmarkConfig);
        std::cout << "Memory Allocation Benchmark:\n";
        std::cout << "  Allocations per second: " << allocBenchmark.allocationsPerSecond << "\n";
        std::cout << "  Average allocation time: " << allocBenchmark.averageAllocationTime << "ms\n";
        std::cout << "  Peak throughput: " << allocBenchmark.peakThroughput << " allocations/ms\n\n";
        
        // Predict memory requirements
        std::cout << "--- Memory Prediction ---\n";
        
        auto prediction = analyzer.predictMemoryUsage(5000, 8, 0.2f);
        std::cout << "Memory Prediction for 5000 neurons:\n";
        std::cout << "  Predicted memory usage: " << prediction.predictedMemoryUsage << "KB\n";
        std::cout << "  Prediction confidence: " << prediction.confidence << "\n";
        std::cout << "  Uncertainty bounds: +/- " << prediction.uncertainty << "KB\n\n";
        
        std::cout << "========================================\n";
        std::cout << "MEMORY ANALYSIS TOOLS DEMO COMPLETE\n";
        std::cout << "========================================\n";
    }
    
private:
    std::shared_ptr<Config> createConfig(int neuronCount, int regionCount, float connectionProb) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        return config;
    }
};

int main(int argc, char* argv[]) {
    nlm::MemoryAnalysisToolsDemo demo;
    demo.run();
    return 0;
}
