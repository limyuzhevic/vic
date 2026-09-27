#ifndef BATCH_BRAIN_PROCESSOR_HPP
#define BATCH_BRAIN_PROCESSOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <mutex>

namespace nlm {
namespace cli {

struct BatchProcessResult {
    std::string inputPath;
    std::string outputPath;
    bool success;
    std::string errorMessage;
    double processingTime;
    std::map<std::string, std::string> metadata;
};

class BatchBrainProcessor {
public:
    BatchBrainProcessor();
    ~BatchBrainProcessor();

    bool ProcessBatch(const std::vector<std::string>& inputPaths, const std::string& outputDirectory);
    bool ProcessBatchSequentially(const std::vector<std::string>& inputPaths, const std::string& outputDirectory);
    bool ProcessBatchParallel(const std::vector<std::string>& inputPaths, const std::string& outputDirectory, size_t threadCount = 4);

    bool SetProcessingOptions(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> GetProcessingOptions() const { return processingOptions; }

    std::vector<BatchProcessResult> GetLastResults() const { return lastResults; }
    bool GenerateBatchReport(const std::vector<BatchProcessResult>& results, const std::string& reportPath) const;
    bool ExportBatchData(const std::string& outputPath) const;
    bool ImportBatchData(const std::string& inputPath);

protected:
    std::map<std::string, std::string> processingOptions;
    std::vector<BatchProcessResult> lastResults;
    std::mutex processingMutex;
    size_t threadCount;
};

} // namespace cli
} // namespace nlm

#endif // BATCH_BRAIN_PROCESSOR_HPP