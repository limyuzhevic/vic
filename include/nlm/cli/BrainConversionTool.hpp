#ifndef BRAIN_CONVERSION_TOOL_HPP
#define BRAIN_CONVERSION_TOOL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {
namespace cli {

class BrainConversionTool {
public:
    BrainConversionTool();
    ~BrainConversionTool();

    bool ConvertBrainFormat(const std::string& inputPath, const std::string& outputPath, const std::string& targetFormat);
    bool ConvertActivityData(const std::string& inputPath, const std::string& outputPath);
    bool ConvertStructureData(const std::string& inputPath, const std::string& outputPath);
    bool ConvertPerformanceData(const std::string& inputPath, const std::string& outputPath);

    bool ProcessBatchConversion(const std::vector<std::string>& inputPaths, const std::string& outputDirectory);
    bool LoadConversionProfile(const std::string& profilePath);
    bool SaveConversionProfile(const std::string& profilePath) const;

    bool ConvertToJSON(const std::string& inputPath, const std::string& outputPath) const;
    bool ConvertToXML(const std::string& inputPath, const std::string& outputPath) const;
    bool ConvertToCSV(const std::string& inputPath, const std::string& outputPath) const;
    bool ConvertToBinary(const std::string& inputPath, const std::string& outputPath) const;

    bool ValidateConversion(const std::string& inputPath, const std::string& outputPath) const;
    bool MergeConvertedData(const std::vector<std::string>& inputPaths, const std::string& outputPath);

    bool SetConversionOptions(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> GetConversionOptions() const { return conversionOptions; }

protected:
    std::map<std::string, std::string> conversionOptions;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_CONVERSION_TOOL_HPP