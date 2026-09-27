#ifndef BRAIN_VISUALIZATION_TOOL_HPP
#define BRAIN_VISUALIZATION_TOOL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {
namespace cli {

class BrainVisualizationTool {
public:
    BrainVisualizationTool();
    ~BrainVisualizationTool();

    bool CreateBrainVisualization(const std::string& brainPath, const std::string& outputPath);
    bool CreatePerformanceVisualization(const std::string& dataPath, const std::string& outputPath);
    bool CreateStructureVisualization(const std::string& structurePath, const std::string& outputPath);
    bool CreateActivityVisualization(const std::string& activityPath, const std::string& outputPath);
    bool CreateDynamicsVisualization(const std::string& dynamicsPath, const std::string& outputPath);

    bool Generate3DView(const std::string& inputPath, const std::string& outputPath) const;
    bool Generate2DView(const std::string& inputPath, const std::string& outputPath) const;
    bool GenerateHeatmap(const std::vector<double>& data, const std::string& outputPath) const;
    bool GenerateTimeSeriesPlot(const std::vector<double>& data, const std::string& outputPath) const;

    bool ExportVisualization(const std::string& outputPath, const std::string& format = "png") const;
    bool ImportVisualization(const std::string& inputPath);

    bool SetVisualizationOptions(const std::string& option, const std::string& value);
    std::string GetVisualizationOption(const std::string& option) const;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_VISUALIZATION_TOOL_HPP