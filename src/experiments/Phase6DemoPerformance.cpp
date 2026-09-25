#include "experiments/Phase6DemoPerformance.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/Memory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <filesystem>

namespace nlm {

// Phase6DemoMetrics implementation
void Phase6DemoMetrics::recordStep(uint64_t step, double visualizationTimeUs, 
                                  double exportTimeUs, size_t memoryUsage) {
    totalSteps = step;
    totalVisualizationTime += visualizationTimeUs;
    totalExportTime += exportTimeUs;
    
    if (memoryUsage > peakMemoryUsage) {
        peakMemoryUsage = memoryUsage;
    }
    
    stepTimes.push_back(visualizationTimeUs + exportTimeUs);
}

double Phase6DemoMetrics::getAverageFPS() const {
    if (totalSteps == 0) return 0.0;
    double totalTimeS = (totalVisualizationTime + totalExportTime) / 1000000.0;
    if (totalTimeS <= 0.0) return 0.0;
    return totalSteps / totalTimeS;
}

double Phase6DemoMetrics::getAverageStepTime() const {
    if (stepTimes.empty()) return 0.0;
    double sum = 0.0;
    for (double time : stepTimes) sum += time;
    return sum / stepTimes.size();
}

double Phase6DemoMetrics::getPeakMemoryUsage() const {
    return static_cast<double>(peakMemoryUsage);
}

double Phase6DemoMetrics::getTotalRuntime() const {
    return (totalVisualizationTime + totalExportTime) / 1000000.0;
}

void Phase6DemoMetrics::recordSpikeEvent() {
    spikeCount++;
}

void Phase6DemoMetrics::recordPlasticityEvent() {
    plasticityCount++;
}

void Phase6DemoMetrics::recordDevelopmentEvent() {
    developmentCount++;
}

void Phase6DemoMetrics::recordPredictionEvent() {
    predictionCount++;
}

void Phase6DemoMetrics::recordMemoryEvent() {
    memoryCount++;
}

// Phase6DemoPerformanceAnalyzer implementation
std::string Phase6DemoPerformanceAnalyzer::analyzePerformance(const Phase6DemoMetrics& metrics) const {
    std::stringstream ss;
    
    ss << "=== Phase 6 Demo Performance Analysis ===" << std::endl;
    ss << "Total Steps: " << metrics.getTotalSteps() << std::endl;
    ss << "Total Runtime: " << PerformanceUtils::formatTime(metrics.getTotalRuntime() * 1000000.0) << std::endl;
    ss << "Average FPS: " << metrics.getAverageFPS() << std::endl;
    ss << "Average Step Time: " << PerformanceUtils::formatTime(metrics.getAverageStepTime() * 1000000.0) << std::endl;
    ss << "Peak Memory Usage: " << PerformanceUtils::formatBytes(static_cast<size_t>(metrics.getPeakMemoryUsage())) << std::endl;
    ss << std::endl;
    
    // Performance grade
    double performanceScore = 0.0;
    if (metrics.getAverageFPS() > 10.0) performanceScore += 3.0;
    else if (metrics.getAverageFPS() > 5.0) performanceScore += 2.0;
    else if (metrics.getAverageFPS() > 1.0) performanceScore += 1.0;
    
    if (metrics.getPeakMemoryUsage() < 100 * 1024 * 1024) performanceScore += 2.0; // < 100MB
    else if (metrics.getPeakMemoryUsage() < 500 * 1024 * 1024) performanceScore += 1.0; // < 500MB
    
    if (metrics.getAverageStepTime() * 1000000.0 < 10000.0) performanceScore += 2.0; // < 10ms per step
    else if (metrics.getAverageStepTime() * 1000000.0 < 50000.0) performanceScore += 1.0; // < 50ms per step
    
    ss << "Performance Grade: " << getPerformanceGrade(performanceScore) << std::endl;
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::generatePerformanceReport(const Phase6DemoMetrics& metrics,
                                                                     const std::string& demoName) const {
    std::stringstream ss;
    
    ss << "=== " << demoName << " Performance Report ===" << std::endl;
    ss << "Generated: " << PerformanceUtils::getSystemInfo() << std::endl;
    ss << std::endl;
    ss << analyzePerformance(metrics);
    ss << std::endl;
    
    // Detailed metrics
    ss << "=== Detailed Metrics ===" << std::endl;
    ss << formatMetric("Total Steps", metrics.getTotalSteps(), "steps") << std::endl;
    ss << formatMetric("Total Runtime", metrics.getTotalRuntime(), "seconds") << std::endl;
    ss << formatMetric("Average FPS", metrics.getAverageFPS(), "frames/second") << std::endl;
    ss << formatMetric("Average Step Time", metrics.getAverageStepTime(), "microseconds") << std::endl;
    ss << formatMetric("Peak Memory Usage", metrics.getPeakMemoryUsage(), "bytes") << std::endl;
    ss << std::endl;
    
    // Event counts
    ss << "=== Event Counts ===" << std::endl;
    ss << formatMetric("Spike Events", static_cast<double>(metrics.spikeCount), "events") << std::endl;
    ss << formatMetric("Plasticity Events", static_cast<double>(metrics.plasticityCount), "events") << std::endl;
    ss << formatMetric("Development Events", static_cast<double>(metrics.developmentCount), "events") << std::endl;
    ss << formatMetric("Prediction Events", static_cast<double>(metrics.predictionCount), "events") << std::endl;
    ss << formatMetric("Memory Events", static_cast<double>(metrics.memoryCount), "events") << std::endl;
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::generateBottleneckAnalysis(const Phase6DemoMetrics& metrics) const {
    std::stringstream ss;
    
    ss << "=== Performance Bottleneck Analysis ===" << std::endl;
    ss << std::endl;
    
    // Memory bottleneck
    if (metrics.getPeakMemoryUsage() > 500 * 1024 * 1024) {
        ss << "MEM - MEMORY BOTTLENECK DETECTED" << std::endl;
        ss << "  Peak memory usage: " << PerformanceUtils::formatBytes(static_cast<size_t>(metrics.getPeakMemoryUsage())) << std::endl;
        ss << "  Suggestion: Consider optimizing memory usage or increasing available memory." << std::endl;
        ss << std::endl;
    }
    
    // Performance bottleneck
    if (metrics.getAverageFPS() < 1.0) {
        ss << "PERF - PERFORMANCE BOTTLENECK DETECTED" << std::endl;
        ss << "  Average FPS: " << metrics.getAverageFPS() << std::endl;
        ss << "  Average step time: " << PerformanceUtils::formatTime(metrics.getAverageStepTime() * 1000000.0) << std::endl;
        ss << "  Suggestion: Optimize brain step computation or reduce simulation complexity." << std::endl;
        ss << std::endl;
    }
    
    // Step time bottleneck
    if (metrics.getAverageStepTime() * 1000000.0 > 50000.0) {
        ss << "STEP - STEP TIME BOTTLENECK DETECTED" << std::endl;
        ss << "  Average step time: " << PerformanceUtils::formatTime(metrics.getAverageStepTime() * 1000000.0) << std::endl;
        ss << "  Suggestion: Optimize neural computations or reduce network size." << std::endl;
        ss << std::endl;
    }
    
    if (ss.str().find("=== Performance Bottleneck Analysis ===") != std::string::npos && 
        ss.str().find("MEM - MEMORY BOTTLENECK") == std::string::npos &&
        ss.str().find("PERF - PERFORMANCE BOTTLENECK") == std::string::npos &&
        ss.str().find("STEP - STEP TIME BOTTLENECK") == std::string::npos) {
        ss << "No significant bottlenecks detected." << std::endl;
        ss << "System performance appears optimal within current constraints." << std::endl;
    }
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::analyzeMemoryPerformance(const Phase6DemoMetrics& metrics) const {
    std::stringstream ss;
    
    ss << "=== Memory Performance Analysis ===" << std::endl;
    ss << formatMetric("Peak Memory Usage", metrics.getPeakMemoryUsage(), "bytes") << std::endl;
    ss << formatMetric("Memory Efficiency", metrics.getPeakMemoryUsage() / metrics.getTotalSteps(), "bytes/step") << std::endl;
    ss << std::endl;
    
    if (metrics.getPeakMemoryUsage() > 1024 * 1024 * 1024) {
        ss << "WARNING: High memory usage detected." << std::endl;
        ss << "Consider using memory pool allocators or sparse connectivity." << std::endl;
    } else {
        ss << "Memory usage is within acceptable limits." << std::endl;
    }
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::analyzeCPUPerformance(const Phase6DemoMetrics& metrics) const {
    std::stringstream ss;
    
    ss << "=== CPU Performance Analysis ===" << std::endl;
    ss << formatMetric("Average FPS", metrics.getAverageFPS(), "frames/second") << std::endl;
    ss << formatMetric("Average Step Time", metrics.getAverageStepTime(), "microseconds") << std::endl;
    ss << std::endl;
    
    if (metrics.getAverageFPS() < 5.0) {
        ss << "WARNING: Low CPU performance detected." << std::endl;
        ss << "Consider enabling SIMD optimizations or multithreading." << std::endl;
    } else {
        ss << "CPU performance is optimal." << std::endl;
    }
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::analyzeVisualizationPerformance(const Phase6DemoMetrics& metrics) const {
    std::stringstream ss;
    
    ss << "=== Visualization Performance Analysis ===" << std::endl;
    ss << formatMetric("Average FPS", metrics.getAverageFPS(), "frames/second") << std::endl;
    ss << formatMetric("Average Step Time", metrics.getAverageStepTime(), "microseconds") << std::endl;
    ss << std::endl;
    
    if (metrics.getAverageFPS() < 30.0) {
        ss << "WARNING: Visualization may appear choppy." << std::endl;
        ss << "Consider reducing visualization complexity or increasing update rate." << std::endl;
    } else {
        ss << "Visualization performance is smooth." << std::endl;
    }
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::analyzePerformanceTrends(const std::vector<Phase6DemoMetrics>& history) const {
    std::stringstream ss;
    
    ss << "=== Performance Trends Analysis ===" << std::endl;
    if (history.size() < 2) {
        ss << "Insufficient data for trend analysis." << std::endl;
        return ss.str();
    }
    
    // Simple trend analysis
    double prevFPS = history[history.size() - 2].getAverageFPS();
    double currFPS = history[history.size() - 1].getAverageFPS();
    double fpsChange = ((currFPS - prevFPS) / prevFPS) * 100.0;
    
    ss << formatMetric("FPS Trend", fpsChange, "%") << std::endl;
    
    if (fpsChange > 10.0) {
        ss << "Trend: Improving performance" << std::endl;
    } else if (fpsChange < -10.0) {
        ss << "Trend: Declining performance" << std::endl;
    } else {
        ss << "Trend: Stable performance" << std::endl;
    }
    
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::formatMetric(const std::string& name, double value, const std::string& unit) const {
    std::stringstream ss;
    ss << name << ": " << std::fixed << std::setprecision(2) << value << " " << unit;
    return ss.str();
}

std::string Phase6DemoPerformanceAnalyzer::getPerformanceGrade(double performanceScore) const {
    if (performanceScore >= 8.0) return "EXCELLENT";
    if (performanceScore >= 6.0) return "GOOD";
    if (performanceScore >= 4.0) return "FAIR";
    if (performanceScore >= 2.0) return "POOR";
    return "CRITICAL";
}

// Phase6DemoProfiling namespace implementation
bool Phase6DemoProfiling::parseCommandLine(int argc, char* argv[], Phase6DemoConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--phase6-profile") {
            config.enableProfiling = true;
            if (i + 1 < argc) {
                config.profileOutputDir = argv[++i];
            }
        } else if (arg == "--phase6-profile-format") {
            if (i + 1 < argc) {
                config.profileFormat = argv[++i];
            }
        } else if (arg == "--phase6-profile-console") {
            if (i + 1 < argc) {
                config.profileConsoleOutput = (argv[++i][0] == '1');
            }
        } else if (arg == "--phase6-profile-interval") {
            if (i + 1 < argc) {
                config.profileIntervalMs = std::stod(argv[++i]);
            }
        } else if (arg == "--phase6-profile-memory") {
            // Already enabled by default
        } else if (arg == "--phase6-profile-cpu") {
            // Already enabled by default
        } else if (arg == "--phase6-profile-system") {
            // Already enabled by default
        } else if (arg == "--help") {
            std::cout << getPhase6DemoProfilingHelp() << std::endl;
            return false;
        }
    }
    
    return true;
}

void Phase6DemoProfiling::applyProfileConfig(Phase6DemoConfig& config, PerformanceProfiler& profiler) {
    if (!config.enableProfiling) return;
    
    profiler.setEnabled(true);
    profiler.setOutputDirectory(config.profileOutputDir);
    profiler.setOutputFormat(config.profileFormat);
    profiler.setConsoleOutput(config.profileConsoleOutput);
    profiler.setSamplingInterval(config.profileIntervalMs * 1000.0);
}

void Phase6DemoProfiling::setupProfilingForExperiment(PerformanceProfiler& profiler, 
                                                     Phase6DemoConfig& config) {
    if (!config.enableProfiling) return;
    
    profiler.start();
}

void Phase6DemoProfiling::integrateProfilingWithVisualization(PerformanceProfiler& profiler,
                                                            Phase6DemoConfig& config) {
    if (!config.enableProfiling) return;
    
    // Update profiler configuration based on visualization needs
    if (config.profileConsoleOutput) {
        std::cout << "[Phase6DemoProfiling] Integrated with visualization" << std::endl;
    }
}

Phase6DemoMetrics Phase6DemoProfiling::collectDemoMetrics(const Brain& brain, uint64_t step,
                                                        double visualizationTimeUs, double exportTimeUs) {
    Phase6DemoMetrics metrics;
    metrics.recordStep(step, visualizationTimeUs, exportTimeUs, 0);
    
    // Collect brain system metrics
    if (auto workingMem = brain.getWorkingMemory()) {
        metrics.recordMemoryEvent();
    }
    if (auto episodicMem = brain.getEpisodicMemory()) {
        metrics.recordMemoryEvent();
    }
    if (auto assocMem = brain.getAssociativeMemory()) {
        metrics.recordMemoryEvent();
    }
    if (auto prediction = brain.getPredictionSystem()) {
        metrics.recordPredictionEvent();
    }
    if (auto planner = brain.getPlanner()) {
        metrics.recordMemoryEvent();
    }
    if (auto development = brain.getDevelopmentSystem()) {
        metrics.recordDevelopmentEvent();
    }
    
    // Note: Spike events would need to be tracked separately
    
    return metrics;
}

void Phase6DemoProfiling::exportPhase6DemoMetrics(const Phase6DemoMetrics& metrics, 
                                                 const std::string& baseFilename) {
    std::string filename = baseFilename + ".txt";
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "[Phase6DemoProfiling] Failed to open metrics file: " << filename << std::endl;
        return;
    }
    
    Phase6DemoPerformanceAnalyzer analyzer;
    file << analyzer.generatePerformanceReport(metrics, "Phase 6 Demo");
    file.close();
    
    std::cout << "[Phase6DemoProfiling] Metrics exported to: " << filename << std::endl;
}

void Phase6DemoProfiling::generatePerformanceAnalysis(const Phase6DemoMetrics& metrics,
                                                     const std::string& analysisFilename) {
    std::ofstream file(analysisFilename);
    
    if (!file.is_open()) {
        std::cerr << "[Phase6DemoProfiling] Failed to open analysis file: " << analysisFilename << std::endl;
        return;
    }
    
    Phase6DemoPerformanceAnalyzer analyzer;
    
    file << "=== Comprehensive Performance Analysis ===" << std::endl;
    file << "Analysis generated at: " << PerformanceUtils::getSystemInfo() << std::endl;
    file << std::endl;
    
    file << analyzer.analyzePerformance(metrics);
    file << std::endl;
    
    file << analyzer.generateBottleneckAnalysis(metrics);
    file << std::endl;
    
    file << analyzer.analyzeMemoryPerformance(metrics);
    file << std::endl;
    
    file << analyzer.analyzeCPUPerformance(metrics);
    file << std::endl;
    
    file << analyzer.analyzeVisualizationPerformance(metrics);
    file << std::endl;
    
    file.close();
    
    std::cout << "[Phase6DemoProfiling] Performance analysis exported to: " << analysisFilename << std::endl;
}

std::string Phase6DemoProfiling::getPhase6DemoProfilingHelp() {
    return "\n=== Phase 6 Demo Performance Profiling Options ===\n"
           "  --phase6-profile [dir]         Enable profiling with output directory\n"
           "  --phase6-profile-format [fmt]   Specify output format (text, json, csv)\n"
           "  --phase6-profile-console [0|1]  Enable/disable console output\n"
           "  --phase6-profile-interval [ms]   Sampling interval in milliseconds\n"
           "  --phase6-profile-memory          Enable memory profiling\n"
           "  --phase6-profile-cpu            Enable CPU profiling\n"
           "  --phase6-profile-system         Enable system-wide profiling\n"";
}

} // namespace nlm
