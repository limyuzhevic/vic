#ifndef BRAIN_VALIDATION_TOOL_HPP
#define BRAIN_VALIDATION_TOOL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {
namespace cli {

struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationScore;
    std::string details;
};

class BrainValidationTool {
public:
    BrainValidationTool();
    ~BrainValidationTool();

    bool ValidateBrainIntegrity(const std::string& brainPath, ValidationResult& result);
    bool ValidateBrainConsistency(const std::string& brainPath, ValidationResult& result);
    bool ValidateBrainFormat(const std::string& brainPath, ValidationResult& result);
    bool ValidateBrainPerformance(const std::string& brainPath, ValidationResult& result);
    bool ValidateBrainMemory(const std::string& brainPath, ValidationResult& result);
    bool ValidateBrainDynamics(const std::string& brainPath, ValidationResult& result);

    bool CreateValidationTestSuite(const std::string& testSuitePath);
    bool RunValidationTests(const std::string& testSuitePath, std::vector<ValidationResult>& results);
    bool GenerateValidationReport(const std::vector<ValidationResult>& results, const std::string& reportPath) const;
    bool CompareValidationResults(const std::string& resultPath1, const std::string& resultPath2) const;

    bool ExportValidationData(const std::string& outputPath) const;
    bool ImportValidationData(const std::string& inputPath);

    bool SetValidationThresholds(const std::map<std::string, double>& thresholds);
    std::map<std::string, double> GetValidationThresholds() const { return validationThresholds; }

protected:
    std::map<std::string, double> validationThresholds;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_VALIDATION_TOOL_HPP