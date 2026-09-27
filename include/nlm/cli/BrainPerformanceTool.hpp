#ifndef BRAIN_PERFORMANCE_TOOL_HPP
#define BRAIN_PERFORMANCE_TOOL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>

namespace nlm {
namespace cli {

struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    double diskUsage;
    double networkUsage;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> customMetrics;
};

class BrainPerformanceTool {
public:
    BrainPerformanceTool();
    ~BrainPerformanceTool();

    bool BenchmarkBrainPerformance(const std::string& brainPath, PerformanceMetrics& metrics);
    bool MonitorPerformance(const std::string& brainPath, double duration);
    bool ProfileBrainActivity(const std::string& brainPath, double duration);
    bool OptimizeBrainPerformance(const std::string& brainPath);
    bool CollectPerformanceData(const std::string& brainPath, std::vector<PerformanceMetrics>& metrics);
    bool AnalyzePerformanceTrends(const std::vector<PerformanceMetrics>& metrics);

    bool GeneratePerformanceReport(const std::vector<PerformanceMetrics>& metrics, const std::string& reportPath) const;
    bool CreatePerformanceBenchmark(const std::string& benchmarkPath, const PerformanceMetrics& metrics);
    bool ComparePerformance(const std::string& brainPath1, const std::string& brainPath2);

    bool SetPerformanceThresholds(const std::map<std::string, double>& thresholds);
    std::map<std::string, double> GetPerformanceThresholds() const { return performanceThresholds; }

    bool ExportPerformanceData(const std::string& outputPath) const;
    bool ImportPerformanceData(const std::string& inputPath);

protected:
    std::map<std::string, double> performanceThresholds;
    std::vector<PerformanceMetrics> performanceHistory;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_PERFORMANCE_TOOL_HPP