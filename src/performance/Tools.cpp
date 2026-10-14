// Implementation of Performance Tools and Utilities

#include "Tools.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// BenchmarkRunner implementation
void BenchmarkRunner::registerBenchmark(const std::string& name, std::function<double()> benchmarkFunc,
                                        const std::string& category,
                                        const std::unordered_map<std::string, std::string>& params) {
    BenchmarkResult result;
    result.name = name;
    result.category = category;
    result.parameters = params;
    
    // Warmup
    for (size_t i = 0; i < warmupIterations_; ++i) {
        benchmarkFunc();
    }
    
    // Actual benchmark
    std::vector<double> times;
    for (size_t i = 0; i < defaultIterations_; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        benchmarkFunc();
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        times.push_back(duration);
    }
    
    result.times = times;
    result.iterations = times.size();
    
    if (!times.empty()) {
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        result.meanTime = sum / times.size();
        
        double sumSq = 0.0;
        result.minTime = times[0];
        result.maxTime = times[0];
        
        for (double time : times) {
            sumSq += time * time;
            result.minTime = std::min(result.minTime, time);
            result.maxTime = std::max(result.maxTime, time);
        }
        
        result.stdDevTime = std::sqrt(sumSq / times.size() - result.meanTime * result.meanTime);
        
        // Memory and CPU measurements (simplified)
        result.memoryUsageMB = 0.0;  // Would use memory tracking
        result.cpuUsagePercent = 0.0;  // Would use CPU monitoring
    }
    
    benchmarkRegistry_[name].push_back(result);
    currentResults_[name] = result;
}

double BenchmarkRunner::runBenchmark(const std::string& name, const std::string& paramSet) {
    auto it = benchmarkRegistry_.find(name);
    if (it == benchmarkRegistry_.end()) {
        throw std::runtime_error("Benchmark not found: " + name);
    }
    
    // Run the benchmark with default configuration
    if (it->second.empty()) {
        throw std::runtime_error("Benchmark has no registered implementations");
    }
    
    // For simplicity, we run the first implementation
    auto& results = it->second;
    auto& result = results.back();
    
    double totalTime = 0.0;
    for (size_t i = 0; i < defaultIterations_; ++i) {
        // Would run the actual benchmark function
        totalTime += result.times[i % result.times.size()];
    }
    
    return totalTime / defaultIterations_;
}

std::vector<std::pair<std::string, double>> BenchmarkRunner::runBenchmarks(const std::vector<std::string>& names,
                                                                         const std::string& paramSet) {
    std::vector<std::pair<std::string, double>> results;
    for (const auto& name : names) {
        try {
            double time = runBenchmark(name, paramSet);
            results.emplace_back(name, time);
        } catch (const std::exception& e) {
            std::cerr << "Error running benchmark " << name << ": " << e.what() << std::endl;
        }
    }
    return results;
}

void BenchmarkRunner::exportResults(const std::string& filename, const std::string& format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    if (format == "csv") {
        file << "Benchmark,Category,Parameters,Iterations,MeanTime,StdDev,MinTime,MaxTime,MemoryMB,CPUPercent\n";
        for (const auto& [name, results] : benchmarkRegistry_) {
            for (const auto& result : results) {
                file << result.name << ",";
                file << result.category << ",";
                // Export parameters
                file << result.iterations << ",";
                file << std::fixed << std::setprecision(6) << result.meanTime << ",";
                file << result.stdDevTime << ",";
                file << result.minTime << ",";
                file << result.maxTime << ",";
                file << result.memoryUsageMB << ",";
                file << result.cpuUsagePercent << "\n";
            }
        }
    }
    else if (format == "json") {
        json array = json::array();
        for (const auto& [name, results] : benchmarkRegistry_) {
            for (const auto& result : results) {
                json jsonResult;
                jsonResult["name"] = result.name;
                jsonResult["category"] = result.category;
                jsonResult["iterations"] = result.iterations;
                jsonResult["meanTime"] = std::fixed << std::setprecision(6) << result.meanTime;
                jsonResult["stdDevTime"] = result.stdDevTime;
                jsonResult["minTime"] = result.minTime;
                jsonResult["maxTime"] = result.maxTime;
                jsonResult["memoryUsageMB"] = result.memoryUsageMB;
                jsonResult["cpuUsagePercent"] = result.cpuUsagePercent;
                array.push_back(jsonResult);
            }
        }
        file << array.dump(2);
    }
}

void BenchmarkRunner::generateReport(const std::string& filename, const std::string& format) {
    if (format == "html") {
        std::ofstream file(filename);
        file << "<!DOCTYPE html>\n<html>\n<head><title>Benchmark Report</title></head>\n<body>\n";
        file << "<h1>Benchmark Report</h1>\n";
        file << "<table border='1'>\n<tr><th>Benchmark</th><th>Category</th><th>Mean Time</th><th>Std Dev</th></tr>\n";
        
        for (const auto& [name, results] : benchmarkRegistry_) {
            for (const auto& result : results) {
                file << "<tr><td>" << result.name << "</td><td>" << result.category << "</td><td>";
                file << std::fixed << std::setprecision(6) << result.meanTime << "</td><td>";
                file << result.stdDevTime << "</td></tr>\n";
            }
        }
        
        file << "</table>\n</body>\n</html>";
    }
}

// ProfilerViewer implementation
void ProfilerViewer::loadProfile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open profile file: " + filename);
    }
    
    json data = json::parse(file);
    // Parse and load profile data
}

void ProfilerViewer::displayProfileSummary() const {
    std::cout << "Profile Summary:" << std::endl;
    std::cout << "================" << std::endl;
    for (const auto& profile : profiles_) {
        std::cout << "Session: " << profile.sessionName << std::endl;
        std::cout << "  Duration: " << (profile.endTime - profile.startTime) << "s" << std::endl;
        std::cout << "  Events: " << profile.timelineEvents.size() << std::endl;
        std::cout << "  Memory samples: " << profile.memorySamples.size() << std::endl;
        std::cout << "  CPU samples: " << profile.cpuSamples.size() << std::endl;
        std::cout << std::endl;
    }
}

void ProfilerViewer::displayTimeline() const {
    std::cout << "Timeline Visualization:" << std::endl;
    std::cout << "======================" << std::endl;
    // Implementation would display timeline
}

void ProfilerViewer::displayMemoryUsage() const {
    std::cout << "Memory Usage Analysis:" << std::endl;
    std::cout << "=====================" << std::endl;
    // Implementation would display memory analysis
}

void ProfilerViewer::displayCPUTimeline() const {
    std::cout << "CPU Timeline:" << std::endl;
    std::cout << "=============" << std::endl;
    // Implementation would display CPU timeline
}

void ProfilerViewer::exportProfile(const std::string& filename, const std::string& format) {
    if (format == "json") {
        json array = profiles_;
        std::ofstream file(filename);
        file << array.dump(2);
    }
}

// MemoryAnalyzer implementation
void MemoryAnalyzer::startTracking(const std::string& sessionName) {
    currentSession_ = sessionName;
    trackingEnabled_ = true;
    allocations_.clear();
    allocatedAddresses_.clear();
}

void MemoryAnalyzer::stopTracking() {
    trackingEnabled_ = false;
    currentSession_.clear();
}

void MemoryAnalyzer::recordAllocation(size_t address, size_t size, const std::string& type,
                                      const std::string& stack) {
    if (!trackingEnabled_) return;
    
    AllocationInfo info;
    info.address = address;
    info.size = size;
    info.type = type;
    info.timestamp = std::chrono::high_resolution_clock::now();
    info.allocationStack = stack;
    
    allocations_.push_back(info);
    allocatedAddresses_.insert(address);
}

void MemoryAnalyzer::recordDeallocation(size_t address) {
    if (!trackingEnabled_) return;
    
    auto it = std::find_if(allocations_.begin(), allocations_.end(),
                          [address](const AllocationInfo& info) { return info.address == address; });n    
    if (it != allocations_.end()) {
        allocatedAddresses_.erase(address);
        allocations_.erase(it);
    }
}

std::vector<MemoryAnalyzer::LeakInfo> MemoryAnalyzer::detectLeaks() const {
    std::vector<LeakInfo> leaks;
    
    for (const auto& alloc : allocations_) {
        LeakInfo leak;
        leak.address = alloc.address;
        leak.size = alloc.size;
        leak.type = alloc.type;
        leak.allocationStack = alloc.allocationStack;
        leaks.push_back(leak);
    }
    
    return leaks;
}

double MemoryAnalyzer::getCurrentMemoryUsageMB() const {
    // Implementation would query current memory usage
    return 0.0;
}

double MemoryAnalyzer::getPeakMemoryUsageMB() const {
    // Implementation would return peak memory usage
    return 0.0;
}

void MemoryAnalyzer::exportLeaks(const std::string& filename) const {
    std::ofstream file(filename);
    json array = detectLeaks();
    file << array.dump(2);
}

void MemoryAnalyzer::generateLeakReport(const std::string& filename) const {
    std::ofstream file(filename);
    file << "Memory Leak Analysis Report\n";
    file << "=============================\n\n";
    
    auto leaks = detectLeaks();
    file << "Total leaks detected: " << leaks.size() << "\n\n";
    
    for (const auto& leak : leaks) {
        file << "Address: 0x" << std::hex << leak.address << std::dec << "\n";
        file << "Size: " << leak.size << " bytes\n";
        file << "Type: " << leak.type << "\n";
        file << "Stack: " << leak.allocationStack << "\n\n";
    }
}

// CPUMemoryProfiler implementation
CPUMemoryProfiler::ProfilingResult CPUMemoryProfiler::profileOperation(const std::function<void()>& operation,
                                                                       const std::string& operationName,
                                                                       bool measureMemory) {
    ProfilingResult result;
    result.operationName = operationName;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (measureMemory) {
        // Record initial memory
    }
    
    operation();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (measureMemory) {
        // Record final memory and calculate delta
    }
    
    result.efficiencyScore = 1.0 / result.totalTime;  // Simple efficiency metric
    sessionResults_.push_back(result);
    return result;
}

void CPUMemoryProfiler::startProfilingSession(const std::string& sessionName) {
    currentSession_ = sessionName;
}

void CPUMemoryProfiler::endProfilingSession() {
    currentSession_.clear();
}

void CPUMemoryProfiler::exportResults(const std::string& filename, const std::string& format) const {
    if (format == "csv") {
        std::ofstream file(filename);
        file << "Session,Operation,CPUTime,MemoryTime,TotalTime,EfficiencyScore\n";
        for (const auto& result : sessionResults_) {
            file << currentSession_ << "," << result.operationName << ",";
            file << result.cpuTime << "," << result.memoryTime << ",";
            file << result.totalTime << "," << result.efficiencyScore << "\n";
        }
    }
}

void CPUMemoryProfiler::generatePerformanceReport(const std::string& filename) const {
    std::ofstream file(filename);
    file << "Performance Profiling Report\n";
    file << "=============================\n\n";
    
    for (const auto& result : sessionResults_) {
        file << "Operation: " << result.operationName << "\n";
        file << "  CPU Time: " << result.cpuTime << "s\n";
        file << "  Memory Time: " << result.memoryTime << "s\n";
        file << "  Total Time: " << result.totalTime << "s\n";
        file << "  Efficiency Score: " << result.efficiencyScore << "\n\n";
    }
}

// PerformanceComparator implementation
void PerformanceComparator::addBaseline(const std::string& name, const std::vector<BenchmarkRunner::BenchmarkResult>& results) {
    baselineSuites_[name] = results;
}

void PerformanceComparator::addCurrent(const std::string& name, const std::vector<BenchmarkRunner::BenchmarkResult>& results) {
    currentSuites_[name] = results;
}

std::vector<PerformanceComparator::ComparisonResult> PerformanceComparator::compareAll() const {
    std::vector<ComparisonResult> allComparisons;
    
    for (const auto& [baselineName, baselineResults] : baselineSuites_) {
        for (const auto& [currentName, currentResults] : currentSuites_) {
            if (baselineName != currentName) {
                auto comparisons = compare(baselineName, currentName);
                allComparisons.insert(allComparisons.end(), comparisons.begin(), comparisons.end());
            }
        }
    }
    
    return allComparisons;
}

std::vector<PerformanceComparator::ComparisonResult> PerformanceComparator::compare(const std::string& baselineName,
                                                                                   const std::string& currentName) const {
    std::vector<ComparisonResult> comparisons;
    
    auto baselineIt = baselineSuites_.find(baselineName);
    auto currentIt = currentSuites_.find(currentName);
    
    if (baselineIt == baselineSuites_.end() || currentIt == currentSuites_.end()) {
        return comparisons;
    }
    
    for (const auto& baselineResult : baselineIt->second) {
        for (const auto& currentResult : currentIt->second) {
            if (baselineResult.name == currentResult.name) {
                ComparisonResult comparison;
                comparison.testName = baselineResult.name;
                comparison.baselineName = baselineName;
                comparison.currentName = currentName;
                
                comparison.baselineTime = baselineResult.meanTime;
                comparison.currentTime = currentResult.meanTime;
                comparison.timeRatio = (comparison.currentTime / comparison.baselineTime) * 100.0;
                comparison.isRegression = comparison.timeRatio > (100.0 + timeThresholdPercent_);
                comparison.confidence = 0.95;  // Would calculate statistical confidence
                
                comparison.baselineMemory = baselineResult.memoryUsageMB;
                comparison.currentMemory = currentResult.memoryUsageMB;
                comparison.memoryRatio = (comparison.currentMemory / comparison.baselineMemory) * 100.0;
                
                comparison.baselineCPU = baselineResult.cpuUsagePercent;
                comparison.currentCPU = currentResult.cpuUsagePercent;
                comparison.cpuRatio = (comparison.currentCPU / comparison.baselineCPU) * 100.0;
                
                comparison.regressionThreshold = timeThresholdPercent_;
                
                comparisons.push_back(comparison);
                break;
            }
        }
    }
    
    return comparisons;
}

void PerformanceComparator::exportComparison(const std::string& filename, const std::string& format) const {
    if (format == "csv") {
        std::ofstream file(filename);
        file << "Test,Baseline,Current,BaselineTime,CurrentTime,TimeRatio,IsRegression,Confidence\n";
        
        auto comparisons = compareAll();
        for (const auto& comp : comparisons) {
            file << comp.testName << ",";
            file << comp.baselineName << ",";
            file << comp.currentName << ",";
            file << comp.baselineTime << ",";
            file << comp.currentTime << ",";
            file << comp.timeRatio << ",";
            file << (comp.isRegression ? "true" : "false") << ",";
            file << comp.confidence << "\n";
        }
    }
}

void PerformanceComparator::generateRegressionReport(const std::string& filename) const {
    std::ofstream file(filename);
    file << "Performance Regression Analysis Report\n";
    file << "====================================\n\n";
    
    auto comparisons = compareAll();
    
    file << "Total comparisons: " << comparisons.size() << "\n";
    size_t regressions = 0;
    for (const auto& comp : comparisons) {
        if (comp.isRegression) regressions++;
    }
    
    file << "Regressions detected: " << regressions << "\n\n";
    
    file << "Detailed results:\n";
    for (const auto& comp : comparisons) {
        file << "- " << comp.testName << " (" << comp.baselineName << " -> " << comp.currentName << ")\n";
        file << "  Time: " << comp.baselineTime << " -> " << comp.currentTime << "s (ratio: " << comp.timeRatio << "%)\n";
        file << "  Memory: " << comp.baselineMemory << " -> " << comp.currentMemory << "MB\n";
        file << "  CPU: " << comp.baselineCPU << " -> " << comp.currentCPU << "%\n";
        file << "  Regression: " << (comp.isRegression ? "YES" : "NO") << "\n\n";
    }
}

// OutputFormatter implementation
std::string OutputFormatter::formatBenchmarkResult(const BenchmarkRunner::BenchmarkResult& result,
                                                    const std::string& format) {
    if (format == "table") {
        std::stringstream ss;
        ss << std::left << std::setw(30) << result.name;
        ss << std::right << std::setw(10) << result.iterations;
        ss << std::fixed << std::setprecision(6) << std::setw(12) << result.meanTime;
        ss << std::setw(12) << result.stdDevTime;
        ss << std::setw(12) << result.minTime;
        ss << std::setw(12) << result.maxTime;
        ss << std::setw(12) << result.memoryUsageMB << "MB";
        ss << std::setw(12) << result.cpuUsagePercent << "%";
        return ss.str();
    }
    return "";
}

std::string OutputFormatter::formatProfilerOutput(const std::string& sessionName,
                                                   const ProfilerViewer::ProfileData& profile,
                                                   const std::string& format) {
    if (format == "text") {
        std::stringstream ss;
        ss << "Session: " << sessionName << "\n";
        ss << "Duration: " << (profile.endTime - profile.startTime) << "s\n";
        ss << "Timeline events: " << profile.timelineEvents.size() << "\n";
        return ss.str();
    }
    return "";
}

std::string OutputFormatter::formatComparisonResult(const PerformanceComparator::ComparisonResult& result,
                                                     const std::string& format) {
    if (format == "text") {
        std::stringstream ss;
        ss << result.testName << ": " << result.baselineTime << " -> " << result.currentTime << "s\n";
        ss << "  Ratio: " << result.timeRatio << "% (regression: " << (result.isRegression ? "yes" : "no") << ")\n";
        return ss.str();
    }
    return "";
}

void OutputFormatter::writeToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::trunc);
    if (file.is_open()) {
        file << content;
    }
}

void OutputFormatter::appendToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << content;
    }
}

std::string OutputFormatter::escapeHTML(const std::string& text) const {
    std::string escaped = text;
    // Replace & with &amp;, < with &lt;, etc.
    size_t pos = 0;
    while ((pos = escaped.find('&', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = escaped.find('<', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    return escaped;
}

std::string OutputFormatter::escapeCSV(const std::string& text) const {
    std::string escaped = text;
    if (escaped.find(',') != std::string::npos || escaped.find('"') != std::string::npos ||
        escaped.find('\n') != std::string::npos) {
        escaped = "\"" + escaped + "\"";
        // Escape quotes by doubling them
        size_t pos = 0;
        while ((pos = escaped.find('""', pos)) != std::string::npos) {
            escaped.replace(pos, 2, "\"\"\"\"");
            pos += 4;
        }
    }
    return escaped;
}

// BenchmarkSuite implementation
void BenchmarkSuite::addBenchmark(const std::string& name, std::function<double()> benchmark,
                                 const std::string& category,
                                 const std::unordered_map<std::string, std::string>& parameters) {
    benchmarkRunner_.registerBenchmark(name, benchmark, category, parameters);
}

void BenchmarkSuite::runParameterizedBenchmarks(const std::unordered_map<std::string, std::vector<std::string>>& paramGrid) {
    // Implementation would run parameterized benchmarks
}

std::vector<BenchmarkRunner::BenchmarkResult> BenchmarkSuite::runAllBenchmarks() {
    return benchmarkRunner_.getAllResults();
}

std::vector<BenchmarkRunner::BenchmarkResult> BenchmarkSuite::runBenchmarksByCategory(const std::string& category) {
    std::vector<BenchmarkRunner::BenchmarkResult> results;
    // Implementation would filter by category
    return results;
}

void BenchmarkSuite::saveBaseline(const std::string& filename) {
    benchmarkRunner_.exportResults(filename + "_baseline", "json");
}

void BenchmarkSuite::loadBaseline(const std::string& filename) {
    // Implementation would load baseline results
}

void BenchmarkSuite::detectRegressions(const std::string& baselineFile,
                                       const std::string& outputFile) {
    // Implementation would detect regressions
}

std::vector<PerformanceComparator::ComparisonResult> BenchmarkSuite::getRegressionReport() const {
    return comparator_.compareAll();
}

void BenchmarkSuite::generateComprehensiveReport(const std::string& filename,
                                                const std::string& format) {
    if (format == "html") {
        OutputFormatter formatter;
        formatter.writeToFile(filename, "<!DOCTYPE html>\n<html>\n<head><title>NLM Performance Report</title></head>\n<body>\n<h1>NLM Performance Report</h1>\n</body>\n</html>");
    }
}

} // namespace nlm