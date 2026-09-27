#ifndef BRAIN_ANALYSIS_TOOL_HPP
#define BRAIN_ANALYSIS_TOOL_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace nlm {
namespace cli {

struct BrainAnalysisReport {
    std::string timestamp;
    double performanceScore;
    double memoryUsage;
    double cpuUsage;
    std::vector<double> activityPatterns;
    std::map<std::string, double> metrics;
    std::string reportText;
};

class BrainAnalysisTool {
public:
    BrainAnalysisTool();
    ~BrainAnalysisTool();

    bool AnalyzeBrainStructure(const std::string& brainPath, BrainAnalysisReport& report);
    bool AnalyzeActivityPatterns(const std::string& brainPath, BrainAnalysisReport& report);
    bool AnalyzePerformance(const std::string& brainPath, BrainAnalysisReport& report);
    bool AnalyzeMemory(const std::string& brainPath, BrainAnalysisReport& report);
    bool AnalyzeDynamics(const std::string& brainPath, BrainAnalysisReport& report);
    bool GenerateVisualization(const BrainAnalysisReport& report, const std::string& outputPath);

    bool LoadBrainData(const std::string& brainPath);
    bool ProcessData(std::vector<double>& data);
    bool ExtractFeatures(const std::string& brainPath, std::vector<double>& features);
    bool ExportReport(const BrainAnalysisReport& report, const std::string& outputPath) const;
    bool ImportReport(const std::string& inputPath, BrainAnalysisReport& report);

    const BrainAnalysisReport& GetLastReport() const { return lastReport; }

protected:
    BrainAnalysisReport lastReport;
    std::vector<double> brainData;
    std::chrono::system_clock::time_point analysisTime;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_ANALYSIS_TOOL_HPP