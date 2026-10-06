// Memory Analysis Tools Demo
// This demo showcases the Memory Analysis Tools capabilities
// Integrates with the NLM memory management system

#include "MemoryAnalysisTools.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
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
        std::cout << "NLM MEMORY ANALYSIS TOOLS DEMO\n";
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
        
        // Integrate memory analysis tools with brain
        analyzer.integrateWithBrain(brain.get());
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        std::cout << "Working Memory enabled: " << (workingMemory ? "YES" : "NO") << "\n";
        std::cout << "Episodic Memory enabled: " << (episodicMemory ? "YES" : "NO") << "\n\n";
        
        // Simulate memory usage
        std::cout << "--- Simulating Memory Usage ---\n";
        
        // Create memory traces in working memory
        std::vector<uint64_t> traceIds;
        for (int i = 0; i < 100; ++i) {
            uint64_t traceId = workingMemory->createTrace(i, static_cast<float>(i) * 0.1f);
            traceIds.push_back(traceId);
        }
        
        std::cout << "Created " << traceIds.size() << " working memory traces\n";
        
        // Simulate episodic memory usage
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
        
        // 1. Memory Profiling
        std::cout << "--- 1. Memory Profiling ---\n";
        
        auto wmStats = analyzer.analyzeWorkingMemory(workingMemory);
        std::cout << "Working Memory Statistics:\n";
        std::cout << "  Active traces: " << wmStats.activeTraces << "\n";
        std::cout << "  Total capacity: " << wmStats.capacity << "\n";
        std::cout << "  Utilization ratio: " << std::fixed << std::setprecision(2) << wmStats.utilizationRatio << "\n";
        std::cout << "  Average trace strength: " << wmStats.averageTraceStrength << "\n";
        std::cout << "  Competition count: " << wmStats.competitionCount << "\n";
        std::cout << "  Conflict rate: " << std::fixed << std::setprecision(4) << wmStats.conflictRate << "\n\n";
        
        // Profile memory allocation
        auto wmAllocProfile = analyzer.profileMemoryAllocation("working_memory", [workingMemory]() {
            for (int i = 0; i < 50; ++i) {
                uint64_t traceId = workingMemory->createTrace(i, 0.5f);
                workingMemory->updateTrace(traceId, 0.1f, i * 0.01f);
            }
        });
        
        std::cout << "Memory Allocation Profile:\n";
        std::cout << "  Total allocations: " << wmAllocProfile.allocationCount << "\n";
        std::cout << "  Average allocation time: " << std::fixed << std::setprecision(2) << wmAllocProfile.averageAllocationTime << "ms\n";
        std::cout << "  Peak memory usage: " << wmAllocProfile.peakMemoryUsage << "KB\n";
        std::cout << "  Memory growth rate: " << std::fixed << std::setprecision(2) << wmAllocProfile.memoryGrowthRate << "KB/step\n\n";
        
        // 2. Memory Leak Detection
        std::cout << "--- 2. Memory Leak Detection ---\n";
        
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
        std::cout << "  Leak rate: " << std::fixed << std::setprecision(2) << leakAnalysis.leakRate << " leaks per 1000 operations\n";
        std::cout << "  Memory growth trend: " << leakAnalysis.growthTrend << "\n";
        std::cout << "  Suggested cleanup: " << leakAnalysis.suggestedCleanup << "\n\n";
        
        // 3. Memory Fragmentation Analysis
        std::cout << "--- 3. Memory Fragmentation Analysis ---\n";
        
        auto fragmentationAnalysis = analyzer.analyzeFragmentation(workingMemory);
        std::cout << "Fragmentation Analysis:\n";
        std::cout << "  Fragmentation level: " << std::fixed << std::setprecision(2) << fragmentationAnalysis.fragmentationLevel << "%\n";
        std::cout << "  Defragmentation needed: " << (fragmentationAnalysis.needsDefragmentation ? "YES" : "NO") << "\n";
        std::cout << "  Memory waste: " << fragmentationAnalysis.wastedMemory << "KB\n";
        std::cout << "  Optimization potential: " << std::fixed << std::setprecision(2) << fragmentationAnalysis.optimizationPotential << "%\n\n";
        
        // 4. Memory Optimization
        std::cout << "--- 4. Memory Optimization ---\n";
        
        auto usagePattern = analyzer.analyzeUsagePatterns(workingMemory, episodicMemory);
        std::cout << "Usage Pattern Analysis:\n";
        std::cout << "  Peak usage time: " << usagePattern.peakUsageTime << " steps\n";
        std::cout << "  Usage periodicity: " << std::fixed << std::setprecision(2) << usagePattern.periodicity << "\n";
        std::cout << "  Predictive compression ratio: " << std::fixed << std::setprecision(2) << usagePattern.predictiveCompressionRatio << "x\n";
        std::cout << "  Compression efficiency: " << std::fixed << std::setprecision(2) << usagePattern.compressionEfficiency << "%\n\n";
        
        auto recommendations = analyzer.generateOptimizationRecommendations();
        std::cout << "Optimization Recommendations:\n";
        for (size_t i = 0; i < recommendations.size(); ++i) {
            const auto& rec = recommendations[i];
            std::cout << "  " << i + 1 << ". [" << rec.area << "] " << rec.suggestion << "\n";
            std::cout << "     Potential improvement: " << std::fixed << std::setprecision(2) << rec.potentialImprovement << "%\n";
            std::cout << "     Implementation complexity: " << rec.implementationComplexity << "\n\n";
        }
        
        // 5. Memory Prediction
        std::cout << "--- 5. Memory Prediction ---\n";
        
        auto prediction = analyzer.predictMemoryUsage(5000, 8, 0.2f);
        std::cout << "Memory Prediction for 5000 neurons:\n";
        std::cout << "  Predicted memory usage: " << prediction.predictedMemoryUsage << "KB\n";
        std::cout << "  Prediction confidence: " << std::fixed << std::setprecision(2) << prediction.confidence << "\n";
        std::cout << "  Uncertainty bounds: +/- " << prediction.uncertainty << "KB\n\n";
        
        // Configuration Comparison
        std::cout << "--- Configuration Comparison ---\n";
        
        auto config1 = createConfig(500, 2, 0.1f);
        auto config2 = createConfig(2000, 8, 0.2f);
        auto config3 = createConfig(1000, 4, 0.15f);
        
        auto config1Stats = analyzer.analyzeMemoryUsage(config1);
        auto config2Stats = analyzer.analyzeMemoryUsage(config2);
        auto config3Stats = analyzer.analyzeMemoryUsage(config3);
        
        std::cout << "Configuration Memory Usage:\n";
        std::cout << "  Config 1 (500 neurons): " << config1Stats.totalMemoryUsage << "KB\n";
        std::cout << "  Config 2 (2000 neurons): " << config2Stats.totalMemoryUsage << "KB\n";
        std::cout << "  Config 3 (1000 neurons): " << config3Stats.totalMemoryUsage << "KB\n";
        std::cout << "  Memory efficiency (config3 vs config2): " << std::fixed << std::setprecision(2) << (config3Stats.memoryEfficiency / config2Stats.memoryEfficiency) << "x\n\n";
        
        // Benchmarking
        std::cout << "--- Memory Benchmarking ---\n";
        
        auto benchmarkConfig = createConfig(1000, 4, 0.15f);
        
        auto allocBenchmark = analyzer.benchmarkMemoryAllocation(benchmarkConfig);
        std::cout << "Memory Allocation Benchmark:\n";
        std::cout << "  Allocations per second: " << std::fixed << std::setprecision(2) << allocBenchmark.allocationsPerSecond << "\n";
        std::cout << "  Average allocation time: " << std::fixed << std::setprecision(2) << allocBenchmark.averageAllocationTime << "ms\n";
        std::cout << "  Peak throughput: " << std::fixed << std::setprecision(2) << allocBenchmark.peakThroughput << " allocations/ms\n\n";
        
        // Export analysis
        std::cout << "--- Exporting Memory Analysis ---\n";
        
        std::string analysisPath = "/tmp/memory_analysis.json";
        analyzer.exportAnalysis(analysisPath);
        std::cout << "✓ Memory analysis exported to: " << analysisPath << "\n\n";
        
        // Generate optimization report
        std::cout << "--- Memory Optimization Report ---\n";
        
        std::string report = analyzer.generateOptimizationReport();
        std::cout << "Memory optimization report generated (" << report.length() << " characters)\n";
        
        // Real-time monitoring demo
        std::cout << "\n--- Real-time Monitoring Demo ---\n";
        
        analyzer.startRealTimeMonitoring();
        std::cout << "Started real-time memory monitoring for 3 seconds...\n";
        
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        analyzer.stopRealTimeMonitoring();
        std::cout << "Real-time monitoring stopped.\n\n";
        
        std::cout << "========================================\n";
        std::cout << "NLM MEMORY ANALYSIS TOOLS DEMO COMPLETE\n";
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