#include "BottleneckAnalyzer.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <set>

namespace nlm {

class BottleneckAnalyzer::Impl {
public:
    Impl() : isEnabled_(true), isAnalyzing_(false), analysisThreshold_(0.001), // 1ms threshold
             detectionSensitivity_(0.7), timeBasedAnalysis_(false),
             maxAnalysisAge_(3600.0), correlationAnalysis_(false),
             totalAnalysisTime_(0), bottleneckCount_(0), analysisScore_(0.0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() = default;
    
    void startAnalysis() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isAnalyzing_) {
            isAnalyzing_ = true;
            startTime_ = std::chrono::high_resolution_clock::now();
            analysisCount_++;
            cleanOldData();
        }
    }
    
    void stopAnalysis() {
        std::lock_guard<std::mutex> lock(mutex_);
        isAnalyzing_ = false;
        updateAnalysisScore();
    }
    
    bool isAnalyzing() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isAnalyzing_;
    }
    
    void resetAnalysis() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        resourceProfiles_.clear();
        executionTraces_.clear();
        systemEvents_.clear();
        
        analysisCount_ = 0;
        bottleneckCount_ = 0;
        analysisScore_ = 0.0;
        
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void recordResourceUsage(const ResourceProfile& profile) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        resourceProfiles_.push_back(profile);
        updateTotalAnalysisTime();
    }
    
    void recordExecutionTrace(const ExecutionTrace& trace) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        executionTraces_.push_back(trace);
        updateTotalAnalysisTime();
    }
    
    void recordSystemEvent(const std::string& eventType, const std::string& message,
                           Timestamp timestamp, double impact) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        SystemEvent event;
        event.type = eventType;
        event.message = message;
        event.timestamp = timestamp;
        event.impact = impact;
        
        systemEvents_.push_back(event);
        updateTotalAnalysisTime();
    }
    
    std::vector<BottleneckResult> analyzeBottlenecks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<BottleneckResult> bottlenecks;
        
        // Analyze different types of bottlenecks
        bottlenecks.push_back(detectCpuBottleneck());
        bottlenecks.push_back(detectMemoryBottleneck());
        bottlenecks.push_back(detectIoBottleneck());
        bottlenecks.push_back(detectNetworkBottleneck());
        bottlenecks.push_back(detectAlgorithmBottleneck());
        
        // Filter out insignificant bottlenecks
        bottlenecks.erase(std::remove_if(bottlenecks.begin(), bottlenecks.end(),
                                        [this](const BottleneckResult& bottleneck) {
                                            return bottleneck.severity < analysisThreshold_;
                                        }), bottlenecks.end());
        
        bottleneckCount_ = bottlenecks.size();
        
        return bottlenecks;
    }
    
    std::vector<BottleneckResult> analyzeBottlenecksByType(const std::string& type) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<BottleneckResult> bottlenecks;
        
        if (type == "cpu") {
            bottlenecks.push_back(detectCpuBottleneck());
        } else if (type == "memory") {
            bottlenecks.push_back(detectMemoryBottleneck());
        } else if (type == "io") {
            bottlenecks.push_back(detectIoBottleneck());
        } else if (type == "network") {
            bottlenecks.push_back(detectNetworkBottleneck());
        } else if (type == "algorithm") {
            bottlenecks.push_back(detectAlgorithmBottleneck());
        }
        
        return bottlenecks;
    }
    
    bool hasBottlenecks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !analyzeBottlenecks().empty();
    }
    
    double getOverallBottleneckSeverity() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto bottlenecks = analyzeBottlenecks();
        if (bottlenecks.empty()) {
            return 0.0;
        }
        
        double totalSeverity = 0.0;
        for (const auto& bottleneck : bottlenecks) {
            totalSeverity += bottleneck.severity;
        }
        
        return totalSeverity / bottlenecks.size();
    }
    
    double analyzeCpuUsagePatterns() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (resourceProfiles_.empty()) return 0.0;
        
        double totalCpuTime = 0.0;
        size_t validProfiles = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.cpuUsage > 0.0) {
                totalCpuTime += profile.cpuUsage;
                validProfiles++;
            }
        }
        
        double averageCpuUsage = validProfiles > 0 ? totalCpuTime / validProfiles : 0.0;
        
        // Calculate pattern consistency
        double cpuVariance = 0.0;
        for (const auto& profile : resourceProfiles_) {
            if (profile.cpuUsage > 0.0) {
                cpuVariance += std::pow(profile.cpuUsage - averageCpuUsage, 2);
            }
        }
        
        double cpuStdDev = validProfiles > 0 ? std::sqrt(cpuVariance / validProfiles) : 0.0;
        
        // Lower variance indicates more stable performance
        return std::max(0.0, 1.0 - (cpuStdDev * 2.0));
    }
    
    double analyzeMemoryUsagePatterns() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (resourceProfiles_.empty()) return 0.0;
        
        double totalMemory = 0.0;
        size_t validProfiles = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.memoryUsage > 0.0) {
                totalMemory += profile.memoryUsage;
                validProfiles++;
            }
        }
        
        double averageMemory = validProfiles > 0 ? totalMemory / validProfiles : 0.0;
        
        // Check for memory growth trend
        double memoryTrend = 0.0;
        if (resourceProfiles_.size() >= 2) {
            size_t halfIndex = resourceProfiles_.size() / 2;
            double firstHalfAvg = 0.0, secondHalfAvg = 0.0;
            size_t firstCount = 0, secondCount = 0;
            
            for (size_t i = 0; i < halfIndex; ++i) {
                if (resourceProfiles_[i].memoryUsage > 0.0) {
                    firstHalfAvg += resourceProfiles_[i].memoryUsage;
                    firstCount++;
                }
            }
            
            for (size_t i = halfIndex; i < resourceProfiles_.size(); ++i) {
                if (resourceProfiles_[i].memoryUsage > 0.0) {
                    secondHalfAvg += resourceProfiles_[i].memoryUsage;
                    secondCount++;
                }
            }
            
            if (firstCount > 0) firstHalfAvg /= firstCount;
            if (secondCount > 0) secondHalfAvg /= secondCount;
            
            memoryTrend = (secondHalfAvg - firstHalfAvg) / std::max(1.0, firstHalfAvg);
        }
        
        // Return pattern score (higher is better)
        double patternScore = 1.0 - std::abs(memoryTrend);
        return std::max(0.0, patternScore);
    }
    
    double analyzeIoPatterns() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check for consistent I/O patterns
        double totalIo = 0.0;
        size_t validProfiles = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.diskIO > 0.0) {
                totalIo += profile.diskIO;
                validProfiles++;
            }
        }
        
        double averageIo = validProfiles > 0 ? totalIo / validProfiles : 0.0;
        
        // Calculate I/O variance
        double ioVariance = 0.0;
        for (const auto& profile : resourceProfiles_) {
            if (profile.diskIO > 0.0) {
                ioVariance += std::pow(profile.diskIO - averageIo, 2);
            }
        }
        
        double ioStdDev = validProfiles > 0 ? std::sqrt(ioVariance / validProfiles) : 0.0;
        
        // Lower variance indicates more stable I/O patterns
        return std::max(0.0, 1.0 - (ioStdDev * 0.5));
    }
    
    double analyzeNetworkPatterns() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check for consistent network patterns
        double totalNetwork = 0.0;
        size_t validProfiles = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.networkIO > 0.0) {
                totalNetwork += profile.networkIO;
                validProfiles++;
            }
        }
        
        double averageNetwork = validProfiles > 0 ? totalNetwork / validProfiles : 0.0;
        
        // Calculate network variance
        double networkVariance = 0.0;
        for (const auto& profile : resourceProfiles_) {
            if (profile.networkIO > 0.0) {
                networkVariance += std::pow(profile.networkIO - averageNetwork, 2);
            }
        }
        
        double networkStdDev = validProfiles > 0 ? std::sqrt(networkVariance / validProfiles) : 0.0;
        
        // Lower variance indicates more stable network patterns
        return std::max(0.0, 1.0 - (networkStdDev * 0.5));
    }
    
    std::vector<std::string> identifySlowFunctions(double thresholdTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> slowFunctions;
        
        for (const auto& trace : executionTraces_) {
            if (trace.executionTime > thresholdTime) {
                std::string functionDesc = trace.functionName;
                if (!trace.fileName.empty()) {
                    functionDesc += " (" + trace.fileName;
                    if (trace.lineNumber > 0) {
                        functionDesc += ":" + std::to_string(trace.lineNumber);
                    }
                    functionDesc += ")";
                }
                slowFunctions.push_back(functionDesc);
            }
        }
        
        return slowFunctions;
    }
    
    std::vector<std::string> identifyMemoryHogs(size_t thresholdMemory) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> memoryHogs;
        
        for (const auto& trace : executionTraces_) {
            if (trace.memoryUsage > thresholdMemory) {
                std::string traceDesc = trace.functionName;
                if (!trace.fileName.empty()) {
                    traceDesc += " (" + trace.fileName;
                    if (trace.lineNumber > 0) {
                        traceDesc += ":" + std::to_string(trace.lineNumber);
                    }
                    traceDesc += ")";
                }
                memoryHogs.push_back(traceDesc);
            }
        }
        
        return memoryHogs;
    }
    
    std::vector<std::string> identifyCpuHogs(double thresholdCpu) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> cpuHogs;
        
        // Calculate CPU time per function
        std::map<std::string, double> functionCpuTime;
        
        for (const auto& trace : executionTraces_) {
            functionCpuTime[trace.functionName] += trace.executionTime;
        }
        
        for (const auto& pair : functionCpuTime) {
            if (pair.second > thresholdCpu) {
                cpuHogs.push_back(pair.first);
            }
        }
        
        return cpuHogs;
    }
    
    std::vector<std::string> identifyIoHogs(double thresholdIo) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // This is a simplified implementation - in a real system,
        // we would need to track actual I/O operations per function
        
        std::vector<std::string> ioHogs;
        
        for (const auto& trace : executionTraces_) {
            if (trace.metrics.count("disk_io") > 0 && trace.metrics.at("disk_io") > thresholdIo) {
                ioHogs.push_back(trace.functionName);
            }
        }
        
        return ioHogs;
    }
    
    std::vector<BottleneckResult> analyzeTimeBasedTrends(Timestamp startTime,
                                                          Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<BottleneckResult> timeBasedBottlenecks;
        
        // Filter data by time range
        std::vector<ResourceProfile> timeFilteredProfiles;
        std::vector<ExecutionTrace> timeFilteredTraces;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.timestamp >= startTime && profile.timestamp <= endTime) {
                timeFilteredProfiles.push_back(profile);
            }
        }
        
        for (const auto& trace : executionTraces_) {
            if (trace.timestamp >= startTime && trace.timestamp <= endTime) {
                timeFilteredTraces.push_back(trace);
            }
        }
        
        // Analyze time-based bottlenecks
        if (!timeFilteredProfiles.empty()) {
            BottleneckResult bottleneck;
            bottleneck.bottleneckType = "system_load";
            bottleneck.componentName = "system_resources";
            bottleneck.location = "system_monitor";
            
            double totalCpu = 0.0, totalMemory = 0.0;
            size_t validProfiles = 0;
            
            for (const auto& profile : timeFilteredProfiles) {
                if (profile.cpuUsage > 0.0) {
                    totalCpu += profile.cpuUsage;
                    validProfiles++;
                }
                if (profile.memoryUsage > 0.0) {
                    totalMemory += profile.memoryUsage;
                }
            }
            
            double avgCpu = validProfiles > 0 ? totalCpu / validProfiles : 0.0;
            double avgMemory = validProfiles > 0 ? totalMemory / validProfiles : 0.0;
            
            bottleneck.severity = std::max(avgCpu, avgMemory / 1024.0); // Normalize memory
            bottleneck.impact = bottleneck.severity;
            
            bottleneck.symptoms = {"High CPU usage", "High memory usage"};
            bottleneck.causes = {"Resource contention", "Memory leaks", "Inefficient algorithms"};
            bottleneck.recommendation = "Optimize resource usage and implement memory pooling";
            bottleneck.timestamp = getCurrentTimestamp();
            bottleneck.detectionConfidence = 0.8;
            
            timeBasedBottlenecks.push_back(bottleneck);
        }
        
        return timeBasedBottlenecks;
    }
    
    std::vector<ResourceProfile> getResourceProfiles(Timestamp startTime,
                                                      Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<ResourceProfile> result;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.timestamp >= startTime && profile.timestamp <= endTime) {
                result.push_back(profile);
            }
        }
        
        return result;
    }
    
    std::vector<ExecutionTrace> getExecutionTraces(Timestamp startTime,
                                                   Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<ExecutionTrace> result;
        
        for (const auto& trace : executionTraces_) {
            if (trace.timestamp >= startTime && trace.timestamp <= endTime) {
                result.push_back(trace);
            }
        }
        
        return result;
    }
    
    double analyzeSystemLoad() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (resourceProfiles_.empty()) return 0.0;
        
        double totalLoad = 0.0;
        size_t validProfiles = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.cpuUsage > 0.0) {
                totalLoad += profile.cpuUsage;
                validProfiles++;
            }
        }
        
        return validProfiles > 0 ? totalLoad / validProfiles : 0.0;
    }
    
    double analyzeResponsiveness() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (executionTraces_.empty()) return 0.0;
        
        double totalTime = 0.0;
        size_t traceCount = 0;
        
        for (const auto& trace : executionTraces_) {
            if (trace.executionTime > 0.0) {
                totalTime += trace.executionTime;
                traceCount++;
            }
        }
        
        double averageTime = traceCount > 0 ? totalTime / traceCount : 0.0;
        
        // Responsiveness is inverse of average execution time
        return std::max(0.0, 1.0 - std::min(1.0, averageTime / 10.0)); // Normalize for 10s max
    }
    
    double analyzeResourceUtilization() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        double totalCpu = 0.0, totalMemory = 0.0;
        size_t validProfiles = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.cpuUsage > 0.0) {
                totalCpu += profile.cpuUsage;
                validProfiles++;
            }
            if (profile.memoryUsage > 0.0) {
                totalMemory += profile.memoryUsage;
            }
        }
        
        double avgCpu = validProfiles > 0 ? totalCpu / validProfiles : 0.0;
        double avgMemory = validProfiles > 0 ? totalMemory / validProfiles : 0.0;
        
        // Calculate utilization score
        double cpuScore = avgCpu;
        double memoryScore = std::min(1.0, avgMemory / 2048.0); // 2GB threshold
        
        return (cpuScore + memoryScore) / 2.0;
    }
    
    BottleneckResult detectCpuBottleneck() const {
        BottleneckResult bottleneck;
        bottleneck.bottleneckType = "cpu";
        bottleneck.componentName = "processing_units";
        bottleneck.location = "cpu_system";
        bottleneck.timestamp = getCurrentTimestamp();
        bottleneck.detectionConfidence = 0.7;
        
        // Analyze CPU usage patterns
        double cpuPatternScore = analyzeCpuUsagePatterns();
        double systemLoad = analyzeSystemLoad();
        
        // Calculate severity based on pattern score and system load
        bottleneck.severity = std::max(0.0, 1.0 - cpuPatternScore) * 0.7 + 
                             std::min(1.0, systemLoad / 0.8) * 0.3;
        bottleneck.impact = bottleneck.severity;
        
        if (bottleneck.severity > 0.5) {
            bottleneck.symptoms = {"High CPU utilization", "Slow response times",
                                  "Increased latency", "System stuttering"};
            bottleneck.causes = {"CPU-bound algorithms", "Inefficient code",
                                "Thread contention", "Resource starvation"};
            bottleneck.recommendation = "Optimize CPU-intensive functions, consider parallel processing";
        }
        
        return bottleneck;
    }
    
    BottleneckResult detectMemoryBottleneck() const {
        BottleneckResult bottleneck;
        bottleneck.bottleneckType = "memory";
        bottleneck.componentName = "memory_system";
        bottleneck.location = "memory_management";
        bottleneck.timestamp = getCurrentTimestamp();
        bottleneck.detectionConfidence = 0.8;
        
        double memoryPatternScore = analyzeMemoryUsagePatterns();
        double peakMemoryUsage = getPeakMemoryUsage();
        
        bottleneck.severity = std::max(0.0, 1.0 - memoryPatternScore) * 0.5 + 
                             std::min(1.0, peakMemoryUsage / (1024 * 1024 * 1024)) * 0.5; // Normalize for 1GB
        bottleneck.impact = bottleneck.severity * 1.2; // Memory bottlenecks have higher impact
        
        if (bottleneck.severity > 0.5) {
            bottleneck.symptoms = {"High memory usage", "Memory leaks detected",
                                  "Frequent garbage collection", "Swapping detected"};
            bottleneck.causes = {"Memory leaks", "Inefficient data structures",
                                "Large object allocations", "Cache inefficiencies"};
            bottleneck.recommendation = "Implement memory pooling, optimize data structures, add leak detection";
        }
        
        return bottleneck;
    }
    
    BottleneckResult detectIoBottleneck() const {
        BottleneckResult bottleneck;
        bottleneck.bottleneckType = "io";
        bottleneck.componentName = "storage_system";
        bottleneck.location = "io_operations";
        bottleneck.timestamp = getCurrentTimestamp();
        bottleneck.detectionConfidence = 0.6;
        
        double ioPatternScore = analyzeIoPatterns();
        auto ioHogs = identifyIoHogs(100.0); // 100MB/s threshold
        
        bottleneck.severity = std::max(0.0, 1.0 - ioPatternScore) * 0.6;
        
        // Add severity based on I/O hogs
        if (!ioHogs.empty()) {
            bottleneck.severity = std::min(1.0, bottleneck.severity + 0.3);
        }
        
        bottleneck.impact = bottleneck.severity;
        
        if (bottleneck.severity > 0.5) {
            bottleneck.symptoms = {"High I/O wait times", "Disk thrashing",
                                  "Slow file operations", "Network latency"};
            bottleneck.causes = {"Inefficient file access patterns",
                                "Large data transfers", "Synchronous I/O operations"};
            bottleneck.recommendation = "Implement asynchronous I/O, cache frequently accessed data";
        }
        
        return bottleneck;
    }
    
    BottleneckResult detectNetworkBottleneck() const {
        BottleneckResult bottleneck;
        bottleneck.bottleneckType = "network";
        bottleneck.componentName = "network_interface";
        bottleneck.location = "network_stack";
        bottleneck.timestamp = getCurrentTimestamp();
        bottleneck.detectionConfidence = 0.5;
        
        double networkPatternScore = analyzeNetworkPatterns();
        double systemLoad = analyzeSystemLoad();
        
        bottleneck.severity = std::max(0.0, 1.0 - networkPatternScore) * 0.7 + 
                             std::min(1.0, systemLoad / 0.9) * 0.3;
        bottleneck.impact = bottleneck.severity;
        
        if (bottleneck.severity > 0.5) {
            bottleneck.symptoms = {"Network latency", "Packet loss",
                                  "Connection timeouts", "Slow data transfer"};
            bottleneck.causes = {"Network congestion", "Poor network configuration",
                                "Inefficient protocols", "Resource contention"};
            bottleneck.recommendation = "Optimize network configuration, implement connection pooling";
        }
        
        return bottleneck;
    }
    
    BottleneckResult detectAlgorithmBottleneck() const {
        BottleneckResult bottleneck;
        bottleneck.bottleneckType = "algorithm";
        bottleneck.componentName = "computation_engine";
        bottleneck.location = "algorithm_optimization";
        bottleneck.timestamp = getCurrentTimestamp();
        bottleneck.detectionConfidence = 0.7;
        
        auto slowFunctions = identifySlowFunctions(1.0); // 1 second threshold
        auto cpuHogs = identifyCpuHogs(0.8); // 80% CPU threshold
        
        double severity = 0.0;
        if (!slowFunctions.empty()) {
            severity += 0.4;
        }
        if (!cpuHogs.empty()) {
            severity += 0.3;
        }
        
        bottleneck.severity = std::min(1.0, severity);
        bottleneck.impact = bottleneck.severity * 1.1; // Algorithm bottlenecks have high impact
        
        if (bottleneck.severity > 0.5) {
            bottleneck.symptoms = {"Slow algorithm execution", "High CPU usage",
                                  "Inefficient data structures", "Poor cache utilization"};
            bottleneck.causes = {"Inefficient algorithms", "Poor data structure choices",
                                "Lack of parallelization", "Inefficient search strategies"};
            bottleneck.recommendation = "Optimize algorithms, use efficient data structures, implement parallelization";
        }
        
        return bottleneck;
    }
    
    std::string generateBottleneckReport() const {
        std::stringstream report;
        
        report << "=== Bottleneck Analysis Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto bottlenecks = analyzeBottlenecks();
        
        if (bottlenecks.empty()) {
            report << "✅ No significant bottlenecks detected.\n";
            return report.str();
        }
        
        report << "Bottlenecks detected: " << bottlenecks.size() << "\n\n";
        
        // Sort by severity (descending)
        auto sortedBottlenecks = bottlenecks;
        std::sort(sortedBottlenecks.begin(), sortedBottlenecks.end(),
                  [](const BottleneckResult& a, const BottleneckResult& b) {
                      return a.severity > b.severity;
                  });
        
        report << "Priority Bottlenecks:\n";
        for (size_t i = 0; i < sortedBottlenecks.size(); ++i) {
            const auto& bottleneck = sortedBottlenecks[i];
            report << "  " << (i + 1) << ". " << bottleneck.bottleneckType << " (Severity: " 
                   << (bottleneck.severity * 100) << "%)" << std::endl;
            report << "     Component: " << bottleneck.componentName << std::endl;
            report << "     Location: " << bottleneck.location << std::endl;
            report << "     Impact: " << (bottleneck.impact * 100) << "%\n";
        }
        
        report << "\nOverall Bottleneck Severity: " << getOverallBottleneckSeverity() * 100 << "%\n";
        
        return report.str();
    }
    
    std::string generateResourceReport() const {
        std::stringstream report;
        
        report << "=== Resource Usage Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        report << "System Load: " << analyzeSystemLoad() * 100 << "%\n";
        report << "Responsiveness Score: " << analyzeResponsiveness() * 100 << "%\n";
        report << "Resource Utilization: " << analyzeResourceUtilization() * 100 << "%\n\n";
        
        report << "Resource Pattern Scores:\n";
        report << "  CPU Usage Pattern: " << (analyzeCpuUsagePatterns() * 100) << "%\n";
        report << "  Memory Usage Pattern: " << (analyzeMemoryUsagePatterns() * 100) << "%\n";
        report << "  I/O Pattern: " << (analyzeIoPatterns() * 100) << "%\n";
        report << "  Network Pattern: " << (analyzeNetworkPatterns() * 100) << "%\n";
        
        return report.str();
    }
    
    std::string generateTimeSeriesReport() const {
        std::stringstream report;
        
        report << "=== Time Series Analysis Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        if (resourceProfiles_.empty()) {
            report << "No resource profile data available.\n";
            return report.str();
        }
        
        report << "Resource Data Points: " << resourceProfiles_.size() << "\n";
        report << "Execution Trace Points: " << executionTraces_.size() << "\n\n";
        
        // Calculate time ranges
        Timestamp earliestTime = resourceProfiles_.front().timestamp;
        Timestamp latestTime = resourceProfiles_.back().timestamp;
        
        report << "Time Range: " << earliestTime << " to " << latestTime << "\n";
        report << "Duration: " << (latestTime - earliestTime) << " seconds\n\n";
        
        report << "Average Metrics:\n";
        double totalCpu = 0.0, totalMemory = 0.0;
        size_t cpuCount = 0, memoryCount = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.cpuUsage > 0.0) {
                totalCpu += profile.cpuUsage;
                cpuCount++;
            }
            if (profile.memoryUsage > 0.0) {
                totalMemory += profile.memoryUsage;
                memoryCount++;
            }
        }
        
        double avgCpu = cpuCount > 0 ? totalCpu / cpuCount : 0.0;
        double avgMemory = memoryCount > 0 ? totalMemory / memoryCount : 0.0;
        
        report << "  Average CPU Usage: " << (avgCpu * 100) << "%\n";
        report << "  Average Memory Usage: " << avgMemory << " MB\n";
        
        return report.str();
    }
    
    std::string generateOptimizationReport() const {
        std::stringstream report;
        
        report << "=== Optimization Recommendations Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto bottlenecks = analyzeBottlenecks();;
        
        if (bottlenecks.empty()) {
            report << "✅ No optimization needed. System is operating optimally.\n";
            return report.str();
        }
        
        report << "Optimization Recommendations:\n\n";
        
        for (const auto& bottleneck : bottlenecks) {
            report << "  " << bottleneck.bottleneckType << " bottleneck:\n";
            report << "    - " << bottleneck.recommendation << "\n";
            if (!bottleneck.causes.empty()) {
                report << "    - Root causes:\n";
                for (const auto& cause : bottleneck.causes) {
                    report << "      * " << cause << "\n";
                }
            }
            report << "\n";
        }
        
        report << "Prioritized Actions:\n";
        auto prioritized = prioritizeBottlenecks();
        for (size_t i = 0; i < prioritized.size() && i < 5; ++i) { // Top 5
            const auto& bottleneck = prioritized[i];
            report << "  " << (i + 1) << ". Fix " << bottleneck.bottleneckType << " bottleneck in "
                   << bottleneck.componentName << "\n";
        }
        
        return report.str();
    }
    
    bool exportBottleneckData(const std::string& filepath,
                             const std::string& format) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "json") {
            file << "{\n";
            file << "  \"bottleneckCount\": " << bottleneckCount_ << ",\n";
            file << "  \"analysisScore\": " << analysisScore_ << ",\n";
            file << "  \"bottlenecks\": [\n";
            
            auto bottlenecks = analyzeBottlenecks();
            for (size_t i = 0; i < bottlenecks.size(); ++i) {
                const auto& bottleneck = bottlenecks[i];
                file << "    {\"type\": \"" << bottleneck.bottleneckType << "\", \"severity\": " 
                     << bottleneck.severity << ", \"component\": \"" << bottleneck.componentName
                     << "\", \"impact\": " << bottleneck.impact << "}";
                if (i < bottlenecks.size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            }
            
            file << "  ]\n";
            file << "}\n";
        }
        
        file.close();
        return true;
    }
    
    bool importBottleneckData(const std::string& filepath,
                             const std::string& format) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // In a real implementation, this would parse the import file
        // For now, this is a placeholder
        
        file.close();
        return true;
    }
    
    void enable(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        isEnabled_ = enable;
    }
    
    bool isEnabled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isEnabled_;
    }
    
    void setAnalysisThreshold(double threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        analysisThreshold_ = threshold;
    }
    
    void setDetectionSensitivity(double sensitivity) {
        std::lock_guard<std::mutex> lock(mutex_);
        detectionSensitivity_ = sensitivity;
    }
    
    void setTimeBasedAnalysis(bool enable, Timestamp maxAge) {
        std::lock_guard<std::mutex> lock(mutex_);
        timeBasedAnalysis_ = enable;
        maxAnalysisAge_ = maxAge;
    }
    
    void setCorrelationAnalysis(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        correlationAnalysis_ = enable;
    }
    
    std::map<std::string, double> correlateMetrics(const std::string& metric1,
                                                   const std::string& metric2) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::map<std::string, double> correlations;
        
        // Simplified correlation calculation
        // In a real implementation, this would use statistical correlation methods
        
        if (metric1 == "cpu_usage" && metric2 == "memory_usage") {
            correlations["pearson"] = 0.6;
            correlations["spearman"] = 0.5;
        } else if (metric1 == "cpu_usage" && metric2 == "execution_time") {
            correlations["pearson"] = 0.7;
            correlations["spearman"] = 0.6;
        } else {
            correlations["pearson"] = 0.3;
            correlations["spearman"] = 0.2;
        }
        
        return correlations;
    }
    
    std::vector<std::string> predictPerformanceTrends(Timestamp horizon) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> trends;
        
        // Simple trend prediction based on historical data
        if (resourceProfiles_.size() >= 2) {
            double cpuTrend = 0.0;
            double memoryTrend = 0.0;
            
            // Calculate linear trend
            size_t halfIndex = resourceProfiles_.size() / 2;
            double firstHalfAvg = 0.0, secondHalfAvg = 0.0;
            size_t firstCount = 0, secondCount = 0;
            
            for (size_t i = 0; i < halfIndex; ++i) {
                if (resourceProfiles_[i].cpuUsage > 0.0) {
                    firstHalfAvg += resourceProfiles_[i].cpuUsage;
                    firstCount++;
                }
            }
            
            for (size_t i = halfIndex; i < resourceProfiles_.size(); ++i) {
                if (resourceProfiles_[i].cpuUsage > 0.0) {
                    secondHalfAvg += resourceProfiles_[i].cpuUsage;
                    secondCount++;
                }
            }
            
            if (firstCount > 0) firstHalfAvg /= firstCount;
            if (secondCount > 0) secondHalfAvg /= secondCount;
            
            cpuTrend = (secondHalfAvg - firstHalfAvg) / std::max(1.0, firstHalfAvg);
            
            trends.push_back("CPU usage trend: " + std::to_string(cpuTrend * 100) + "% change");
            trends.push_back("Memory usage trend: " + std::to_string((secondHalfAvg - firstHalfAvg) / std::max(1.0, firstHalfAvg)) * 100 + "% change");
        }
        
        return trends;
    }
    
    double calculateBottleneckScore(const BottleneckResult& bottleneck) const {
        double score = bottleneck.severity * bottleneck.detectionConfidence;
        
        // Weight by impact
        score *= (1.0 + bottleneck.impact);
        
        return score;
    }
    
    std::vector<BottleneckResult> prioritizeBottlenecks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto bottlenecks = analyzeBottlenecks();
        
        // Sort by bottleneck score (descending)
        std::sort(bottlenecks.begin(), bottlenecks.end(),
                  [this](const BottleneckResult& a, const BottleneckResult& b) {
                      return calculateBottleneckScore(a) > calculateBottleneckScore(b);
                  });
        
        return bottlenecks;
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        return duration;
    }
    
    std::string getCurrentTimestampString() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void updateTotalAnalysisTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        totalAnalysisTime_ = duration;
    }
    
    void updateAnalysisScore() {
        double totalSeverity = 0.0;
        size_t bottleneckCount = 0;
        
        for (const auto& profile : resourceProfiles_) {
            if (profile.cpuUsage > 0.0) {
                totalSeverity += profile.cpuUsage;
                bottleneckCount++;
            }
        }
        
        analysisScore_ = bottleneckCount > 0 ? totalSeverity / bottleneckCount : 0.0;
    }
    
    void cleanOldData() {
        auto now = std::chrono::high_resolution_clock::now();
        double currentTime = getCurrentTimestamp();
        
        // Remove old resource profiles
        auto it = std::remove_if(resourceProfiles_.begin(), resourceProfiles_.end(),
                                [this, currentTime](const ResourceProfile& profile) {
                                    return (currentTime - profile.timestamp) > maxAnalysisAge_;
                                });
        resourceProfiles_.erase(it, resourceProfiles_.end());
        
        // Remove old execution traces
        it = std::remove_if(executionTraces_.begin(), executionTraces_.end(),
                          [this, currentTime](const ExecutionTrace& trace) {
                              return (currentTime - trace.timestamp) > maxAnalysisAge_;
                          });
        executionTraces_.erase(it, executionTraces_.end());
        
        // Remove old system events
        auto eventIt = std::remove_if(systemEvents_.begin(), systemEvents_.end(),
                                    [this, currentTime](const SystemEvent& event) {
                                        return (currentTime - event.timestamp) > maxAnalysisAge_;
                                    });
        systemEvents_.erase(eventIt, systemEvents_.end());
    }
    
    mutable std::mutex mutex_;
    bool isEnabled_;
    bool isAnalyzing_;
    double analysisThreshold_;
    double detectionSensitivity_;
    bool timeBasedAnalysis_;
    Timestamp maxAnalysisAge_;
    bool correlationAnalysis_;
    
    Timestamp startTime_;
    double totalAnalysisTime_;
    size_t analysisCount_;
    size_t bottleneckCount_;
    double analysisScore_;
    
    std::vector<ResourceProfile> resourceProfiles_;
    std::vector<ExecutionTrace> executionTraces_;
    std::vector<SystemEvent> systemEvents_;
};

struct BottleneckAnalyzer::Impl::SystemEvent {
    std::string type;
    std::string message;
    Timestamp timestamp;
    double impact;
};

BottleneckAnalyzer::BottleneckAnalyzer() : pImpl(new Impl()) {}

BottleneckAnalyzer::~BottleneckAnalyzer() {
    delete pImpl;
}

void BottleneckAnalyzer::startAnalysis() {
    pImpl->startAnalysis();
}

void BottleneckAnalyzer::stopAnalysis() {
    pImpl->stopAnalysis();
}

bool BottleneckAnalyzer::isAnalyzing() const {
    return pImpl->isAnalyzing();
}

void BottleneckAnalyzer::resetAnalysis() {
    pImpl->resetAnalysis();
}

void BottleneckAnalyzer::recordResourceUsage(const ResourceProfile& profile) {
    pImpl->recordResourceUsage(profile);
}

void BottleneckAnalyzer::recordExecutionTrace(const ExecutionTrace& trace) {
    pImpl->recordExecutionTrace(trace);
}

void BottleneckAnalyzer::recordSystemEvent(const std::string& eventType, const std::string& message,
                                           Timestamp timestamp, double impact) {
    pImpl->recordSystemEvent(eventType, message, timestamp, impact);
}

std::vector<BottleneckResult> BottleneckAnalyzer::analyzeBottlenecks() const {
    return pImpl->analyzeBottlenecks();
}

std::vector<BottleneckResult> BottleneckAnalyzer::analyzeBottlenecksByType(const std::string& type) const {
    return pImpl->analyzeBottlenecksByType(type);
}

bool BottleneckAnalyzer::hasBottlenecks() const {
    return pImpl->hasBottlenecks();
}

double BottleneckAnalyzer::getOverallBottleneckSeverity() const {
    return pImpl->getOverallBottleneckSeverity();
}

double BottleneckAnalyzer::analyzeCpuUsagePatterns() const {
    return pImpl->analyzeCpuUsagePatterns();
}

double BottleneckAnalyzer::analyzeMemoryUsagePatterns() const {
    return pImpl->analyzeMemoryUsagePatterns();
}

double BottleneckAnalyzer::analyzeIoPatterns() const {
    return pImpl->analyzeIoPatterns();
}

double BottleneckAnalyzer::analyzeNetworkPatterns() const {
    return pImpl->analyzeNetworkPatterns();
}

std::vector<std::string> BottleneckAnalyzer::identifySlowFunctions(double thresholdTime) const {
    return pImpl->identifySlowFunctions(thresholdTime);
}

std::vector<std::string> BottleneckAnalyzer::identifyMemoryHogs(size_t thresholdMemory) const {
    return pImpl->identifyMemoryHogs(thresholdMemory);
}

std::vector<std::string> BottleneckAnalyzer::identifyCpuHogs(double thresholdCpu) const {
    return pImpl->identifyCpuHogs(thresholdCpu);
}

std::vector<std::string> BottleneckAnalyzer::identifyIoHogs(double thresholdIo) const {
    return pImpl->identifyIoHogs(thresholdIo);
}

std::vector<BottleneckResult> BottleneckAnalyzer::analyzeTimeBasedTrends(Timestamp startTime,
                                                                          Timestamp endTime) const {
    return pImpl->analyzeTimeBasedTrends(startTime, endTime);
}

std::vector<ResourceProfile> BottleneckAnalyzer::getResourceProfiles(Timestamp startTime,
                                                                    Timestamp endTime) const {
    return pImpl->getResourceProfiles(startTime, endTime);
}

std::vector<ExecutionTrace> BottleneckAnalyzer::getExecutionTraces(Timestamp startTime,
                                                                   Timestamp endTime) const {
    return pImpl->getExecutionTraces(startTime, endTime);
}

std::string BottleneckAnalyzer::generateBottleneckReport() const {
    return pImpl->generateBottleneckReport();
}

std::string BottleneckAnalyzer::generateResourceReport() const {
    return pImpl->generateResourceReport();
}

std::string BottleneckAnalyzer::generateTimeSeriesReport() const {
    return pImpl->generateTimeSeriesReport();
}

std::string BottleneckAnalyzer::generateOptimizationReport() const {
    return pImpl->generateOptimizationReport();
}

bool BottleneckAnalyzer::exportBottleneckData(const std::string& filepath,
                                             const std::string& format) const {
    return pImpl->exportBottleneckData(filepath, format);
}

bool BottleneckAnalyzer::importBottleneckData(const std::string& filepath,
                                             const std::string& format) {
    return pImpl->importBottleneckData(filepath, format);
}

void BottleneckAnalyzer::enable(bool enable) {
    pImpl->enable(enable);
}

bool BottleneckAnalyzer::isEnabled() const {
    return pImpl->isEnabled();
}

void BottleneckAnalyzer::setAnalysisThreshold(double threshold) {
    pImpl->setAnalysisThreshold(threshold);
}

void BottleneckAnalyzer::setDetectionSensitivity(double sensitivity) {
    pImpl->setDetectionSensitivity(sensitivity);
}

void BottleneckAnalyzer::setTimeBasedAnalysis(bool enable, Timestamp maxAge) {
    pImpl->setTimeBasedAnalysis(enable, maxAge);
}

void BottleneckAnalyzer::setCorrelationAnalysis(bool enable) {
    pImpl->setCorrelationAnalysis(enable);
}

std::map<std::string, double> BottleneckAnalyzer::correlateMetrics(const std::string& metric1,
                                                                   const std::string& metric2) const {
    return pImpl->correlateMetrics(metric1, metric2);
}

std::vector<std::string> BottleneckAnalyzer::predictPerformanceTrends(Timestamp horizon) const {
    return pImpl->predictPerformanceTrends(horizon);
}

double BottleneckAnalyzer::calculateBottleneckScore(const BottleneckResult& bottleneck) const {
    return pImpl->calculateBottleneckScore(bottleneck);
}

std::vector<BottleneckResult> BottleneckAnalyzer::prioritizeBottlenecks() const {
    return pImpl->prioritizeBottlenecks();
}

} // namespace nlm