// ScalingBenchmark.cpp - Implementation of scaling benchmark with checkpoint system integration

#include "ScalingBenchmark.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <random>
#include <map>

namespace nlm {

ScalingBenchmark::ScalingBenchmark() {}

ScalingBenchmark::~ScalingBenchmark() {}

std::vector<ScaleLevel> ScaleLevel::getStandardLevels() {
    std::vector<ScaleLevel> levels;
    
    // Define scale levels from small to large
    std::vector<std::pair<size_t, std::string>> scaleConfigs = {
        {1000, "single_core"},
        {5000, "single_core"},
        {10000, "multi_core"},
        {50000, "multi_core"},
        {100000, "gpu"},
        {500000, "gpu"},
        {1000000, "gpu"},
    };
    
    for (const auto& config : scaleConfigs) {
        ScaleLevel level;
        level.name = "Scale_" + std::to_string(config.first) + "neurons";
        level.neurons = config.first;
        level.expectedSynapses = static_cast<size_t>(config.first * 1000 * config.second == "gpu" ? 0.15f : 0.1f);
        level.connectionProbability = config.second == "gpu" ? 0.05f : 0.1f;
        level.hardwareTarget = config.second;
        level.notes = "Automated scale level for benchmarking";
        
        levels.push_back(level);
    }
    
    return levels;
}

BenchmarkMetrics::BenchmarkMetrics() {}

std::string BenchmarkMetrics::toCSV() const {
    std::stringstream ss;
    ss << neuronsPerSecond << ","
       << synapsesPerSecond << ","
       << spikesPerSecond << ","
       << synapticEventsPerSecond << ","
       << realTimeFactor << ","
       << avgStepTimeMs << ","
       << minStepTimeMs << ","
       << maxStepTimeMs << ","
       << stepTimeStdDev << ","
       << neuronMemoryMB << ","
       << synapseMemoryMB << ","
       << eventQueueMemoryMB << ","
       << totalMemoryMB << ","
       << peakMemoryMB << ","
       << activeNeurons << ","
       << firingNeurons << ","
       << avgFiringRateHz << ","
       << spikeDensity << ","
       << synapticWeightMean << ","
       << synapticWeightStdDev << ","
       << excitatoryRatio << ","
       << connectionDensity << ","
       << cpuUtilizationPercent << ","
       << gpuUtilizationPercent << ","
       << threadCount;
    return ss.str();
}

std::string BenchmarkMetrics::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"neuronsPerSecond\": " << neuronsPerSecond << ",\n";
    ss << "  \"synapsesPerSecond\": " << synapsesPerSecond << ",\n";
    ss << "  \"spikesPerSecond\": " << spikesPerSecond << ",\n";
    ss << "  \"realTimeFactor\": " << realTimeFactor << ",\n";
    ss << "  \"avgStepTimeMs\": " << avgStepTimeMs << ",\n";
    ss << "  \"totalMemoryMB\": " << totalMemoryMB << ",\n";
    ss << "  \"peakMemoryMB\": " << peakMemoryMB << ",\n";
    ss << "  \"avgFiringRateHz\": " << avgFiringRateHz << ",\n";
    ss << "  \"cpuUtilizationPercent\": " << cpuUtilizationPercent << ",\n";
    ss << "  \"threadCount\": " << threadCount << "\n";
    ss << "}\n";
    return ss.str();
}

BenchmarkRun::BenchmarkRun() {}

std::string BenchmarkRun::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << name << "\",\n";
    ss << "  \"neurons\": " << scale.neurons << ",\n";
    ss << "  \"synapses\": " << scale.expectedSynapses << ",\n";
    ss << "  \"randomSeed\": " << randomSeed << ",\n";
    ss << "  \"stepsRun\": " << stepsRun << ",\n";
    ss << "  \"wallClockTime\": " << wallClockTime << ",\n";
    ss << "  \"simulatedTime\": " << simulatedTime << ",\n";
    ss << "  \"metrics\": {\n";
    
    // Add metrics
    bool first = true;
    for (const auto& pair : metrics) {
        if (!first) ss << ",\n";
        ss << "    \"" << pair.first << "\": " << pair.second;
        first = false;
    }
    ss << "\n  }\n";
    ss << "}\n";
    return ss.str();
}

BenchmarkConfig::BenchmarkConfig() {}

ScalingAnalysis::ScalingAnalysis() {}

ScalingAnalysis ScalingBenchmark::analyze(
    const std::vector<BenchmarkRun>& runs,
    const BenchmarkConfig& config
) {
    ScalingAnalysis analysis;
    
    if (runs.empty()) return analysis;
    
    analysis.runs = runs;
    
    // Calculate scaling efficiency
    if (runs.size() >= 2) {
        double prevPerf = 0.0;
        double totalEfficiency = 0.0;
        
        for (size_t i = 0; i < runs.size(); ++i) {
            double neuronsPerSecond = runs[i].metrics.at("neurons_per_second");
            double neuronCount = static_cast<double>(runs[i].scale.neurons);
            
            if (i > 0 && prevPerf > 0.0) {
                double neuronRatio = neuronCount / runs[i-1].scale.neurons;
                double perfRatio = neuronsPerSecond / prevPerf;
                double efficiency = perfRatio / neuronRatio;
                totalEfficiency += efficiency;
            }
            
            prevPerf = neuronsPerSecond;
        }
        
        analysis.scalingEfficiency = totalEfficiency / (runs.size() - 1);
    }
    
    // Identify bottleneck based on resource utilization
    double maxCpuUtil = 0.0;
    double maxMemUse = 0.0;
    
    for (const auto& run : runs) {
        maxCpuUtil = std::max(maxCpuUtil, run.metrics.at("cpu_utilization_percent"));
        maxMemUse = std::max(maxMemUse, static_cast<double>(run.metrics.at("total_memory_mb")));
    }
    
    if (maxCpuUtil > 90.0 && maxMemUse > 8000) {
        analysis.identifiedBottleneck = "memory_and_cpu";
        analysis.bottleneckSeverity = std::min(1.0, (maxCpuUtil - 90.0) / 50.0 + (maxMemUse - 8000) / 8000.0);
    } else if (maxCpuUtil > 90.0) {
        analysis.identifiedBottleneck = "cpu";
        analysis.bottleneckSeverity = std::min(1.0, (maxCpuUtil - 90.0) / 50.0);
    } else if (maxMemUse > 8000) {
        analysis.identifiedBottleneck = "memory";
        analysis.bottleneckSeverity = std::min(1.0, (maxMemUse - 8000) / 8000.0);
    }
    
    // Generate recommendations
    if (analysis.identifiedBottleneck == "memory") {
        analysis.recommendations.push_back("Implement sparse connectivity optimizations");
        analysis.recommendations.push_back("Add memory pooling and caching");
        analysis.recommendations.push_back("Use more efficient data structures");
    } else if (analysis.identifiedBottleneck == "cpu") {
        analysis.recommendations.push_back("Implement SIMD optimizations");
        analysis.recommendations.push_back("Add parallel processing");
        analysis.recommendations.push_back("Use thread-level parallelism");
    }
    
    analysis.recommendations.push_back("Consider hardware-specific optimizations");
    analysis.recommendations.push_back("Implement dynamic scaling");
    
    // Projections (rough estimates)
    analysis.projectedPerformance10x = analysis.scalingEfficiency * 10.0;
    analysis.projectedPerformance100x = analysis.scalingEfficiency * 100.0;
    
    return analysis;
}

std::string ScalingBenchmark::generateReport(const ScalingAnalysis& analysis) {
    std::stringstream ss;
    ss << "=== NLM Scaling Benchmark Report ===\n\n";
    
    ss << "Summary Statistics:\n";
    ss << "  Total benchmark runs: " << analysis.runs.size() << "\n";
    ss << "  Scaling efficiency: " << analysis.scalingEfficiency << "\n";
    ss << "  Identified bottleneck: " << analysis.identifiedBottleneck << "\n";
    ss << "  Bottleneck severity: " << analysis.bottleneckSeverity << "\n\n";
    
    ss << "Recommendations:\n";
    for (size_t i = 0; i < analysis.recommendations.size(); ++i) {
        ss << "  " << (i + 1) << ". " << analysis.recommendations[i] << "\n";
    }
    ss << "\n";
    
    ss << "Projections:\n";
    ss << "  10× scale: " << analysis.projectedPerformance10x << "× baseline\n";
    ss << "  100× scale: " << analysis.projectedPerformance100x << "× baseline\n\n";
    
    return ss.str();
}

bool ScalingBenchmark::saveResults(const std::string& dir, const std::vector<BenchmarkRun>& runs) {
    try {
        std::filesystem::create_directories(dir);
        
        // Save each run as JSON
        for (const auto& run : runs) {
            std::string filename = dir + "/run_" + std::to_string(run.randomSeed) + ".json";
            std::ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for writing: " + filename);
                return false;
            }
            
            file << run.toJSON();
            file.close();
        }
        
        // Create summary file
        std::string summaryFile = dir + "/summary.txt";
        std::ofstream summary(summaryFile);
        if (summary.is_open()) {
            summary << "NLM Scaling Benchmark Results\n";
            summary << "Generated: " << std::chrono::system_clock::now() << "\n\n";
            summary << "Runs saved: " << runs.size() << "\n";
            summary << "Neuron counts: ";
            for (size_t i = 0; i < runs.size(); ++i) {
                if (i > 0) summary << ", ";
                summary << runs[i].scale.neurons;
            }
            summary << "\n";
            summary.close();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save results: ") + e.what());
        return false;
    }
}

std::vector<BenchmarkRun> ScalingBenchmark::loadResults(const std::string& dir) {
    std::vector<BenchmarkRun> runs;
    
    try {
        if (!std::filesystem::exists(dir)) {
            return runs;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    continue;
                }
                
                // Parse JSON (simplified - would use proper JSON parser)
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                // For now, just create empty runs
                runs.push_back(BenchmarkRun());
                
                file.close();
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load results: ") + e.what());
    }
    
    return runs;
}

std::string ScalingBenchmark::getOutputPath(const std::string& dir, const std::string& filename) {
    std::filesystem::path path(dir);
    path /= filename;
    return path.string();
}

std::string ScalingBenchmark::getHardwareInfo() {
    std::stringstream ss;
    ss << "Hardware Information:\n";
    ss << "  CPU Cores: " << std::thread::hardware_concurrency() << "\n";
    
    // Try to get more detailed info
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        ss << "  Max RSS: " << usage.ru_maxrss / 1024 << " MB\n";
    }
    
    ss << "  PID: " << getpid() << "\n";
    
    return ss.str();
}

} // namespace nlm
