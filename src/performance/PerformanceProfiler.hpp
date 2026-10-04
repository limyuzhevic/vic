// Performance profiler for NLM
#include "PerformanceProfiler.hpp"
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_map>
#include <filesystem>

namespace nlm {

PerformanceProfiler::PerformanceProfiler()
    : cpuProfiler_(new CPUProfiler())
    , memoryProfiler_(new MemoryProfiler())
    , ioProfiler_(new IOProfiler())
    , neuralProfiler_(new NeuralProfiler())
    , configProfiler_(new ConfigProfiler())
    , active_(false)
    , currentPhase_(ProfilePhase::Idle)
    , currentScenario_(ProfileScenario::None)
    , totalProfiles_(0)
    , memoryBaselineMB_(0)
{
    // Initialize hardware detection
    detectHardware();
    
    // Get initial memory baseline
    memoryBaselineMB_ = memoryProfiler_->getCurrentMemoryMB();
}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::startProfiling(ProfilePhase phase, ProfileScenario scenario) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (active_) {
        return; // Already profiling
    }
    
    active_ = true;
    currentPhase_ = phase;
    currentScenario_ = scenario;
    
    // Clear previous results
    profileResults_.clear();
    startTime_ = std::chrono::high_resolution_clock::now();
    
    // Start all profilers
    cpuProfiler_->start();
    memoryProfiler_->startRecording();
    ioProfiler_->start();
    neuralProfiler_->start();
    configProfiler_->start();
    
    // Record configuration parsing performance
    configProfiler_->record("start", "Profiling started");
    
    NLM_LOG_INFO("Performance profiler started: phase=" + phaseToString(phase) + 
                 ", scenario=" + scenarioToString(scenario));
}

void PerformanceProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_) {
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime_).count();
    
    // Stop all profilers
    configProfiler_->record("end", "Profiling stopped");
    configProfiler_->recordTiming("total", totalTime);
    
    cpuProfiler_->stop();
    memoryProfiler_->stopRecording();
    ioProfiler_->stop();
    neuralProfiler_->stop();
    configProfiler_->stop();
    
    active_ = false;
    currentPhase_ = ProfilePhase::Idle;
    currentScenario_ = ProfileScenario::None;
    
    // Collect final results
    collectResults();
    
    totalProfiles_++;
    
    NLM_LOG_INFO("Performance profiler stopped. Total profiles: " + std::to_string(totalProfiles_));
}

void PerformanceProfiler::recordCPUUsage(double usagePercent) {
    std::lock_guard<std::mutex> lock(mutex_);
    cpuProfiler_->recordUsage(usagePercent);
}

void PerformanceProfiler::recordMemoryUsage(size_t memoryMB) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryProfiler_->record(memoryMB);
}

void PerformanceProfiler::recordIOStats(const IOStats& stats) {
    std::lock_guard<std::mutex> lock(mutex_);
    ioProfiler_->recordStats(stats);
}

void PerformanceProfiler::recordNeuralActivity(const NeuralActivity& activity) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuralProfiler_->recordActivity(activity);
}

void PerformanceProfiler::recordConfigParsing(const std::string& operation, double timeMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    configProfiler_->record(operation, timeMs);
}

void PerformanceProfiler::recordStepPerformance(const StepPerfData& stepData) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuralProfiler_->recordStepPerformance(stepData);
}

void PerformanceProfiler::recordMemoryAllocation(size_t size, bool isAllocation) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryProfiler_->recordAllocation(size, isAllocation);
}

PerformanceMetrics PerformanceProfiler::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PerformanceMetrics metrics;
    
    // CPU metrics
    CPUMetrics cpuMetrics = cpuProfiler_->getMetrics();
    metrics.cpuUsagePercent = cpuMetrics.currentUsage;
    metrics.cpuInstructionsPerSecond = cpuMetrics.instructionsPerSecond;
    metrics.cpuCyclesPerSecond = cpuMetrics.cyclesPerSecond;
    
    // Memory metrics
    MemoryMetrics memoryMetrics = memoryProfiler_->getMetrics();
    metrics.memoryUsageMB = memoryMetrics.currentMB;
    metrics.memoryPeakMB = memoryMetrics.peakMB;
    metrics.memoryAllocatedMB = memoryMetrics.allocatedMB;
    metrics.memoryFragments = memoryMetrics.fragments;
    
    // I/O metrics
    IOMetrics ioMetrics = ioProfiler_->getMetrics();
    metrics.readOperationsPerSecond = ioMetrics.readOpsPerSec;
    metrics.writeOperationsPerSecond = ioMetrics.writeOpsPerSec;
    metrics.readBytesPerSecond = ioMetrics.readBytesPerSec;
    metrics.writeBytesPerSecond = ioMetrics.writeBytesPerSec;
    
    // Neural metrics
    NeuralMetrics neuralMetrics = neuralProfiler_->getMetrics();
    metrics.spikesPerSecond = neuralMetrics.spikesPerSecond;
    metrics.spikesPerNeuron = neuralMetrics.spikesPerNeuron;
    metrics.synapsesPerSecond = neuralMetrics.synapsesPerSecond;
    metrics.activeNeurons = neuralMetrics.activeNeurons;
    metrics.firingRateHz = neuralMetrics.firingRateHz;
    
    // Configuration metrics
    ConfigMetrics configMetrics = configProfiler_->getMetrics();
    metrics.configParsesPerSecond = configMetrics.parsesPerSecond;
    metrics.configParseTimeAvg = configMetrics.parseTimeAvg;
    
    // Aggregate metrics
    metrics.totalOperationsPerSecond = metrics.spikesPerSecond + 
                                      metrics.readOperationsPerSecond + 
                                      metrics.writeOperationsPerSecond;
    
    double totalMemory = metrics.memoryPeakMB + metrics.memoryAllocatedMB;
    metrics.memoryEfficiency = (totalMemory > 0) ? 
        (1.0 - (metrics.memoryFragments / std::max(1.0, totalMemory))) : 0.0;
    
    return metrics;
}

ProfileResult PerformanceProfiler::getProfileResult() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileResult result;
    result.phase = currentPhase_;
    result.scenario = currentScenario_;
    result.startTime = startTime_;
    
    if (active_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        result.endTime = currentTime;
        result.duration = std::chrono::duration<double>(currentTime - startTime_).count();
    } else {
        result.endTime = profileEndTime_;
        result.duration = profileDuration_;
    }
    
    result.metrics = getCurrentMetrics();
    result.hardwareInfo = hardwareInfo_;
    
    return result;
}

std::vector<ProfileResult> PerformanceProfiler::getProfileHistory(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ProfileResult> results;
    results.reserve(std::min(profileResults_.size(), maxCount));
    
    size_t startIdx = profileResults_.size() > maxCount ? 
        profileResults_.size() - maxCount : 0;
    
    for (size_t i = startIdx; i < profileResults_.size(); ++i) {
        results.push_back(profileResults_[i]);
    }
    
    return results;
}

void PerformanceProfiler::exportResults(const std::string& filename, ExportFormat format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (format) {
        case ExportFormat::CSV:
            exportToCSV(filename);
            break;
        case ExportFormat::JSON:
            exportToJSON(filename);
            break;
        case ExportFormat::HTML:
            exportToHTML(filename);
            break;
        case ExportFormat::Binary:
            exportToBinary(filename);
            break;
        default:
            NLM_LOG_WARNING("Unsupported export format");
    }
}

void PerformanceProfiler::exportProfile(const ProfileResult& result, const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    profileResults_.push_back(result);
    
    // Save to history
    exportResults(filename, ExportFormat::JSON);
}

void PerformanceProfiler::saveToDatabase(const std::string& connectionString, const std::string& tableName) {
    // Placeholder for database export
    NLM_LOG_INFO("Database export not yet implemented");
}

void PerformanceProfiler::generateReport(const std::string& filename, ReportType reportType) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (reportType) {
        case ReportType::Executive:
            generateExecutiveReport(filename);
            break;
        case ReportType::Detailed:
            generateDetailedReport(filename);
            break;
        case ReportType::Comparison:
            generateComparisonReport(filename);
            break;
        case ReportType::Optimization:
            generateOptimizationReport(filename);
            break;
        default:
            generateExecutiveReport(filename);
    }
}

PerformanceAnalysis PerformanceProfiler::analyzePerformance(const std::vector<ProfileResult>& profiles) {
    PerformanceAnalysis analysis;
    
    if (profiles.empty()) return analysis;
    
    // Calculate performance trends
    analysis.averageCPUUsage = 0.0;
    analysis.averageMemoryUsage = 0.0;
    analysis.averageSpikesPerSecond = 0.0;
    analysis.averageOperationsPerSecond = 0.0;
    
    double totalCPUSum = 0.0, totalMemSum = 0.0, totalSpikesSum = 0.0, totalOpsSum = 0.0;
    size_t validProfiles = 0;
    
    for (const auto& profile : profiles) {
        if (!std::isnan(profile.metrics.cpuUsagePercent)) {
            totalCPUSum += profile.metrics.cpuUsagePercent;
            totalMemSum += profile.metrics.memoryUsageMB;
            totalSpikesSum += profile.metrics.spikesPerSecond;
            totalOpsSum += profile.metrics.totalOperationsPerSecond;
            validProfiles++;
        }
    }
    
    if (validProfiles > 0) {
        analysis.averageCPUUsage = totalCPUSum / validProfiles;
        analysis.averageMemoryUsage = totalMemSum / validProfiles;
        analysis.averageSpikesPerSecond = totalSpikesSum / validProfiles;
        analysis.averageOperationsPerSecond = totalOpsSum / validProfiles;
    }
    
    // Calculate bottlenecks and recommendations
    analysis.bottleneck = identifyBottleneck(profiles);
    analysis.recommendations = generateRecommendations(profiles);
    
    // Performance projections
    analysis.performanceProjection = projectScaling(profiles);
    
    return analysis;
}

void PerformanceProfiler::collectResults() {
    // Collect final metrics from all profilers
    ProfileResult result;
    result = getProfileResult();
    result.endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(result.endTime - startTime_).count();
    
    profileResults_.push_back(result);
    profileEndTime_ = result.endTime;
    profileDuration_ = result.duration;
}

void PerformanceProfiler::detectHardware() {
    hardwareInfo_.cpuModel = getCPUModel();
    hardwareInfo_.cpuCores = std::thread::hardware_concurrency();
    hardwareInfo_.ramGB = getSystemMemoryGB();
    hardwareInfo_.supportsAVX = checkSIMDSupport("AVX");
    hardwareInfo_.supportsAVX2 = checkSIMDSupport("AVX2");
    hardwareInfo_.supportsAVX512 = checkSIMDSupport("AVX512");
}

void PerformanceProfiler::exportToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for CSV export: " + filename);
        return;
    }
    
    file << "Phase,Scenario,StartTime,Duration,CPU%,";
    file << "Memory(MB),Spikes/sec,Ops/sec,ReadOps/sec,WriteOps/sec,";
    file << "ReadBytes/sec,WriteBytes/sec,MemoryFragments\n";
    
    for (const auto& result : profileResults_) {
        file << scenarioToString(result.scenario) << ","
             << std::put_time(std::gmtime(&result.startTime), "%Y-%m-%d %H:%M:%S") << ","
             << std::fixed << std::setprecision(3) << result.duration << ","
             << std::fixed << std::setprecision(1) << result.metrics.cpuUsagePercent << ","
             << std::fixed << std::setprecision(1) << result.metrics.memoryUsageMB << ","
             << std::fixed << std::setprecision(1) << result.metrics.spikesPerSecond << ","
             << std::fixed << std::setprecision(0) << result.metrics.totalOperationsPerSecond << ","
             << std::fixed << std::setprecision(0) << result.metrics.readOperationsPerSecond << ","
             << std::fixed << std::setprecision(0) << result.metrics.writeOperationsPerSecond << ","
             << std::fixed << std::setprecision(0) << result.metrics.readBytesPerSecond << ","
             << std::fixed << std::setprecision(0) << result.metrics.writeBytesPerSecond << ","
             << std::fixed << std::setprecision(0) << result.metrics.memoryFragments << "\n";
    }
    
    file.close();
}

void PerformanceProfiler::exportToJSON(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for JSON export: " + filename);
        return;
    }
    
    file << "{\n  \"profiles\": [\n";
    
    for (size_t i = 0; i < profileResults_.size(); ++i) {
        const auto& result = profileResults_[i];
        
        file << "    {\n";
        file << "      \"phase\": \"" << phaseToString(result.phase) << "\",\n";
        file << "      \"scenario\": \"" << scenarioToString(result.scenario) << "\",\n";
        file << "      \"start_time\": " << result.startTime << ",\n";
        file << "      \"duration\": " << std::fixed << std::setprecision(3) << result.duration << ",\n";
        file << "      \"metrics\": {\n";
        file << "        \"cpu_usage_percent\": " << std::fixed << std::setprecision(1) << result.metrics.cpuUsagePercent << ",\n";
        file << "        \"memory_usage_mb\": " << std::fixed << std::setprecision(1) << result.metrics.memoryUsageMB << ",\n";
        file << "        \"spikes_per_second\": " << std::fixed << std::setprecision(1) << result.metrics.spikesPerSecond << ",\n";
        file << "        \"total_operations_per_second\": " << std::fixed << std::setprecision(0) << result.metrics.totalOperationsPerSecond << "\n";
        file << "      }\n";
        
        if (i < profileResults_.size() - 1) file << "    },\n";
        else file << "    }\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
}

void PerformanceProfiler::exportToHTML(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for HTML export: " + filename);
        return;
    }
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n";
    file << "<title>NLM Performance Report</title>\n";
    file << "<style>\n";
    file << "  body { font-family: Arial, sans-serif; margin: 20px; }\n";
    file << "  table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n";
    file << "  th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    file << "  th { background-color: #f2f2f2; }\n";
    file << "  .metric { font-weight: bold; color: #2c3e50; }\n";
    file << "</style>\n";
    file << "</head>\n<body>\n";
    
    file << "<h1>NLM Performance Benchmark Report</h1>\n";
    file << "<p>Generated: " << std::put_time(std::gmtime(&profileResults_[0].startTime), "%Y-%m-%d %H:%M:%S") << "</p>\n";
    
    file << "<table>\n";
    file << "<tr><th>Metric</th><th>Value</th><th>Unit</th></tr>\n";
    
    if (!profileResults_.empty()) {
        const auto& metrics = profileResults_[0].metrics;
        
        file << "<tr><td>CPU Usage</td><td class='metric'>" 
             << std::fixed << std::setprecision(1) << metrics.cpuUsagePercent << "</td><td>%</td></tr>\n";
        file << "<tr><td>Memory Usage</td><td class='metric'>" 
             << std::fixed << std::setprecision(1) << metrics.memoryUsageMB << "</td><td>MB</td></tr>\n";
        file << "<tr><td>Spikes/sec</td><td class='metric'>" 
             << std::fixed << std::setprecision(1) << metrics.spikesPerSecond << "</td><td>Hz</td></tr>\n";
        file << "<tr><td>Operations/sec</td><td class='metric'>" 
             << std::fixed << std::setprecision(0) << metrics.totalOperationsPerSecond << "</td><td>ops</td></tr>\n";
        file << "<tr><td>Read Ops/sec</td><td>" 
             << std::fixed << std::setprecision(0) << metrics.readOperationsPerSecond << "</td><td>ops</td></tr>\n";
        file << "<tr><td>Write Ops/sec</td><td>" 
             << std::fixed << std::setprecision(0) << metrics.writeOperationsPerSecond << "</td><td>ops</td></tr>\n";
        file << "<tr><td>Read Speed</td><td>" 
             << std::fixed << std::setprecision(0) << metrics.readBytesPerSecond << "</td><td>bytes/sec</td></tr>\n";
        file << "<tr><td>Write Speed</td><td>" 
             << std::fixed << std::setprecision(0) << metrics.writeBytesPerSecond << "</td><td>bytes/sec</td></tr>\n";
        file << "<tr><td>Memory Fragments</td><td>" 
             << std::fixed << std::setprecision(0) << metrics.memoryFragments << "</td><td>count</td></tr>\n";
    }
    
    file << "</table>\n</body>\n</html>\n";
    
    file.close();
}

void PerformanceProfiler::exportToBinary(const std::string& filename) {
    // Placeholder for binary export
    NLM_LOG_INFO("Binary export not yet implemented");
}

void PerformanceProfiler::generateExecutiveReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for report: " + filename);
        return;
    }
    
    file << "NLM Executive Performance Summary\n";
    file << "================================\n\n";
    
    if (!profileResults_.empty()) {
        const auto& metrics = profileResults_[0].metrics;
        
        file << "Performance Overview:\n";
        file << "--------------------\n";
        file << "CPU Utilization: " << std::fixed << std::setprecision(1) << metrics.cpuUsagePercent << "%\n";
        file << "Memory Usage: " << std::fixed << std::setprecision(1) << metrics.memoryUsageMB << " MB\n";
        file << "Spike Rate: " << std::fixed << std::setprecision(1) << metrics.spikesPerSecond << " Hz\n";
        file << "Total Throughput: " << std::fixed << std::setprecision(0) << metrics.totalOperationsPerSecond << " operations/sec\n\n";
        
        file << "Key Findings:\n";
        file << "-------------\n";
        
        // Generate key findings
        if (metrics.cpuUsagePercent > 80.0) {
            file << "* CPU usage is high (>80%), potential for optimization\n";
        }
        
        if (metrics.memoryUsageMB > 1000) {
            file << "* Memory usage is high (>1GB), consider memory optimization\n";
        }
        
        if (metrics.spikesPerSecond < 100.0) {
            file << "* Spike rate is low (<100 Hz), may indicate underutilization\n";
        }
        
        if (metrics.totalOperationsPerSecond < 1000) {
            file << "* Operation throughput is low (<1000 ops/sec), consider performance improvements\n";
        }
    }
    
    file.close();
}

void PerformanceProfiler::generateDetailedReport(const std::string& filename) {
    // Similar to executive but with more detailed information
    generateExecutiveReport(filename);
}

void PerformanceProfiler::generateComparisonReport(const std::string& filename) {
    // Generate report comparing different scenarios
    generateExecutiveReport(filename);
}

void PerformanceProfiler::generateOptimizationReport(const std::string& filename) {
    // Generate report with optimization recommendations
    generateExecutiveReport(filename);
}

std::string PerformanceProfiler::identifyBottleneck(const std::vector<ProfileResult>& profiles) {
    if (profiles.empty()) return "No bottlenecks detected";
    
    double avgCPU = 0.0, avgMemory = 0.0, avgSpikes = 0.0;
    size_t validProfiles = 0;
    
    for (const auto& profile : profiles) {
        if (!std::isnan(profile.metrics.cpuUsagePercent)) {
            avgCPU += profile.metrics.cpuUsagePercent;
            avgMemory += profile.metrics.memoryUsageMB;
            avgSpikes += profile.metrics.spikesPerSecond;
            validProfiles++;
        }
    }
    
    if (validProfiles == 0) return "Unknown";
    
    avgCPU /= validProfiles;
    avgMemory /= validProfiles;
    avgSpikes /= validProfiles;
    
    // Determine bottleneck
    if (avgCPU > 90.0) {
        return "CPU bottleneck - consider hardware acceleration or algorithm optimization";
    } else if (avgMemory > 2000) {
        return "Memory bottleneck - consider memory pool optimization or data structure improvements";
    } else if (avgSpikes < 50.0) {
        return "Neural activity bottleneck - consider increasing input or adjusting network parameters";
    } else {
        return "I/O bound - consider optimizing data access patterns";
    }
}

std::vector<std::string> PerformanceProfiler::generateRecommendations(const std::vector<ProfileResult>& profiles) {
    std::vector<std::string> recommendations;
    
    if (profiles.empty()) return recommendations;
    
    double avgCPU = 0.0, avgMemory = 0.0;
    size_t validProfiles = 0;
    
    for (const auto& profile : profiles) {
        if (!std::isnan(profile.metrics.cpuUsagePercent)) {
            avgCPU += profile.metrics.cpuUsagePercent;
            avgMemory += profile.metrics.memoryUsageMB;
            validProfiles++;
        }
    }
    
    if (validProfiles == 0) return recommendations;
    
    avgCPU /= validProfiles;
    avgMemory /= validProfiles;
    
    // Generate recommendations based on metrics
    if (avgCPU > 80.0) {
        recommendations.push_back("Optimize CPU-intensive operations - consider SIMD vectorization");
        recommendations.push_back("Profile hot code paths to identify performance bottlenecks");
    }
    
    if (avgMemory > 1000) {
        recommendations.push_back("Implement memory pools for neuron/synapse allocation");
        recommendations.push_back("Consider structure-of-arrays (SoA) data layout for better cache utilization");
    }
    
    if (avgMemory > 2000) {
        recommendations.push_back("Use sparse connectivity representation for large networks");
        recommendations.push_back("Implement checkpointing to reduce memory footprint");
    }
    
    if (!profiles.empty()) {
        double totalSpikes = 0.0, totalOps = 0.0;
        size_t spikeCount = 0;
        
        for (const auto& profile : profiles) {
            if (!std::isnan(profile.metrics.spikesPerSecond)) {
                totalSpikes += profile.metrics.spikesPerSecond;
                totalOps += profile.metrics.totalOperationsPerSecond;
                spikeCount++;
            }
        }
        
        if (spikeCount > 0) {
            double avgSpikesPerOp = totalSpikes / totalOps;
            if (avgSpikesPerOp < 0.01) {
                recommendations.push_back("Improve neural computation efficiency - low spike-to-operation ratio");
            }
        }
    }
    
    if (recommendations.empty()) {
        recommendations.push_back("Performance is within acceptable ranges");
        recommendations.push_back("Consider periodic monitoring for regression detection");
    }
    
    return recommendations;
}

PerformanceProjection PerformanceProfiler::projectScaling(const std::vector<ProfileResult>& profiles) {
    PerformanceProjection projection;
    
    if (profiles.size() < 2) return projection;
    
    // Use last two profiles to estimate scaling
    const auto& last = profiles.back();
    const auto& secondLast = profiles[profiles.size() - 2];
    
    if (last.metrics.spikesPerSecond <= 0.0 || secondLast.metrics.spikesPerSecond <= 0.0) {
        return projection;
    }
    
    double scalingFactor = static_cast<double>(last.metrics.spikesPerSecond) / 
                          secondLast.metrics.spikesPerSecond;
    
    projection.linearScaling10x = secondLast.metrics.spikesPerSecond * 10.0 * scalingFactor;
    projection.linearScaling100x = secondLast.metrics.spikesPerSecond * 100.0 * scalingFactor;
    projection.hyperScaling10x = secondLast.metrics.spikesPerSecond * 10.0 * std::pow(scalingFactor, 2);
    projection.hyperScaling100x = secondLast.metrics.spikesPerSecond * 100.0 * std::pow(scalingFactor, 2);
    
    projection.confidence = (profiles.size() >= 3) ? 0.9 : 0.7;
    
    return projection;
}

// Helper function implementations
std::string PerformanceProfiler::phaseToString(ProfilePhase phase) {
    switch (phase) {
        case ProfilePhase::Idle: return "Idle";
        case ProfilePhase::BrainInit: return "Brain Initialization";
        case ProfilePhase::Simulation: return "Simulation";
        case ProfilePhase::AgentStep: return "Agent Step";
        case ProfilePhase::WorldStep: return "World Step";
        case ProfilePhase::MemoryTest: return "Memory Test";
        case ProfilePhase::IOTest: return "I/O Test";
        case ProfilePhase::NeuralActivity: return "Neural Activity";
        case ProfilePhase::ConfigTest: return "Configuration Test";
        case ProfilePhase::Scaling: return "Scaling Test";
        case ProfilePhase::Stability: return "Stability Test";
        default: return "Unknown";
    }
}

std::string PerformanceProfiler::scenarioToString(ProfileScenario scenario) {
    switch (scenario) {
        case ProfileScenario::None: return "None";
        case ProfileScenario::SmallBrain: return "Small Brain (<100 neurons)";
        case ProfileScenario::MediumBrain: return "Medium Brain (100-1000 neurons)";
        case ProfileScenario::LargeBrain: return "Large Brain (>1000 neurons)";
        case ProfileScenario::ComplexConnectivity: return "Complex Connectivity";
        case ProfileScenario::HighPlasticity: return "High Plasticity";
        case ProfileScenario::NoveltyDriven: return "Novelty-Driven Exploration";
        case ProfileScenario::MemoryIntensive: return "Memory Intensive";
        case ProfileScenario::CPUIntensive: return "CPU Intensive";
        case ProfileScenario::IOIntensive: return "I/O Intensive";
        default: return "Unknown";
    }
}

std::string PerformanceProfiler::getCPUModel() {
    // Platform-specific implementation
#ifdef __x86_64__
    return "x86-64"; // Would need to parse /proc/cpuinfo or use cpuid
#else
    return "Unknown";
#endif
}

size_t PerformanceProfiler::getSystemMemoryGB() {
    // Platform-specific implementation
#ifdef __linux__
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    size_t memTotal = 0;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            std::string key;
            size_t value;
            iss >> key >> value;
            memTotal = value / 1024 / 1024; // Convert to MB, then to GB
            break;
        }
    }
    return memTotal / 1024;
#else
    return 0; // Unknown platform
#endif
}

bool PerformanceProfiler::checkSIMDSupport(const std::string& target) {
    // Platform-specific SIMD detection
#ifdef __x86_64__
    if (target == "AVX") {
        // Check AVX support using cpuid or similar
        return true;
    } else if (target == "AVX2") {
        return true;
    } else if (target == "AVX512") {
        return true;
    }
#endif
    return false;
}

}} // namespace nlm
