#include "PerformanceReport.hpp"
#include "PerformanceMonitor.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct PerformanceReporter::Impl {
    std::string reportFormat = "text";
    bool includeHistoricalData = true;
    size_t maxHistoricalEntries = 100;
    PerformanceMonitor& monitor = PerformanceMonitor::getInstance();
};

PerformanceReporter::PerformanceReporter() {
    pImpl = std::make_unique<Impl>();
}

PerformanceReporter::~PerformanceReporter() = default;

PerformanceReport PerformanceReporter::generateReport(const std::string& title,
                                                    const std::string& format) const {
    PerformanceReport report;
    report.title = title;
    report.timestamp = getCurrentTimestamp();
    
    // Generate analysis from performance monitor
    report.systemAnalysis = pImpl->monitor.analyzeSystemPerformance();
    report.brainAnalysis = pImpl->monitor.analyzeBrainPerformance();
    report.integrationAnalysis = pImpl->monitor.analyzeIntegrationPerformance();
    report.plasticityAnalysis = pImpl->monitor.analyzePlasticityPerformance();
    
    // Generate recommendations
    report.recommendations = generateOptimizationRecommendations(report);
    
    // Generate summary
    report.summary = generateSummary(report);
    
    return report;
}

std::string PerformanceReporter::generateDashboard(const std::string& format) const {
    if (format == "json") {
        return generateJSONDashboard();
    } else {
        return generateTextDashboard();
    }
}

bool PerformanceReporter::saveReport(const PerformanceReport& report,
                                    const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pImpl->reportFormat == "json") {
        json j = report.toJSON();
        file << j.dump(2);
    } else {
        file << report.generateTextReport();
    }
    
    file.close();
    return true;
}

void PerformanceReporter::setReportFormat(const std::string& format) {
    pImpl->reportFormat = format;
}

void PerformanceReporter::setIncludeHistoricalData(bool include) {
    pImpl->includeHistoricalData = include;
}

void PerformanceReporter::setMaxHistoricalEntries(size_t maxEntries) {
    pImpl->maxHistoricalEntries = maxEntries;
}

std::vector<std::string> PerformanceReporter::analyzeBottlenecks(const PerformanceReport& report) const {
    std::vector<std::string> bottlenecks;
    
    // CPU bottleneck detection
    if (report.systemAnalysis.averageLatencyMs > 50.0) {
        bottlenecks.push_back("High CPU latency (>50ms)");
    }
    
    // Memory bottleneck detection
    if (report.systemAnalysis.efficiencyScore < 0.7) {
        bottlenecks.push_back("Memory efficiency below optimal threshold");
    }
    
    // Brain performance bottlenecks
    if (report.brainAnalysis.p95LatencyMs > 100.0) {
        bottlenecks.push_back("Brain operation P95 latency >100ms");
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceReporter::generateOptimizationRecommendations(
    const PerformanceReport& report) const {
    std::vector<std::string> recommendations;
    
    // CPU optimization
    if (report.systemAnalysis.averageLatencyMs > 30.0) {
        recommendations.push_back("Consider enabling parallel processing for CPU-intensive operations");
    }
    
    // Memory optimization
    if (report.systemAnalysis.efficiencyScore < 0.8) {
        recommendations.push_back("Implement memory pooling to reduce allocation overhead");
    }
    
    // Synaptic optimization
    if (report.brainAnalysis.p95LatencyMs > 80.0) {
        recommendations.push_back("Optimize synaptic connectivity patterns for better locality");
    }
    
    // Plasticity optimization
    if (report.plasticityAnalysis.averageLatencyMs > 20.0) {
        recommendations.push_back("Reduce plasticity update frequency during stable learning phases");
    }
    
    return recommendations;
}

std::string PerformanceReporter::generateTextDashboard() const {
    auto report = generateReport("Performance Dashboard");
    std::stringstream ss;
    
    ss << "=== NLM Performance Dashboard ===\n\n";
    ss << "System Status:" << std::endl;
    ss << "  CPU Usage: " << report.systemAnalysis.averageLatencyMs << "%\n";
    ss << "  Memory Usage: " << report.systemAnalysis.efficiencyScore << "%\n";
    ss << "  Operations/sec: " << report.systemAnalysis.throughputOpsPerSecond << "\n\n";
    
    ss << "Brain Status:" << std::endl;
    ss << "  Neurons Active: " << "N/A" << "\n";
    ss << "  Firing Rate: " << "N/A" << " Hz\n";
    ss << "  Spike Rate: " << "N/A" << " spikes/sec\n\n";
    
    if (!report.recommendations.empty()) {
        ss << "Recommendations:" << std::endl;
        for (const auto& rec : report.recommendations) {
            ss << "  * " << rec << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceReporter::generateJSONDashboard() const {
    auto report = generateReport("Performance Dashboard", "json");
    return report.generateJSONReport();
}

std::string PerformanceReport::generateTextReport() const {
    std::stringstream ss;
    ss << "=== " << title << " ===\n\n";
    ss << "Generated: " << timestamp << "\n";
    ss << "Duration: " << durationSeconds << " seconds\n\n";
    
    // Add detailed analysis sections...
    ss << "System Performance:" << std::endl;
    ss << "  Average Latency: " << systemAnalysis.averageLatencyMs << " ms\n";
    ss << "  P95 Latency: " << systemAnalysis.p95LatencyMs << " ms\n";
    ss << "  Throughput: " << systemAnalysis.throughputOpsPerSecond << " ops/sec\n\n";
    
    ss << "Summary:" << std::endl;
    ss << "  " << summary << std::endl;
    
    return ss.str();
}

std::string PerformanceReport::generateJSONReport() const {
    json j;
    j["title"] = title;
    j["timestamp"] = timestamp;
    j["duration_seconds"] = durationSeconds;
    
    // Add analysis data...
    
    return j.dump(2);
}

std::string PerformanceReporter::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

} // namespace nlm