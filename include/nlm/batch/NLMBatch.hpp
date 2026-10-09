# NLM Batch Processing System
# Complete implementation for running multiple NLM experiments

#ifndef NLM_BATCH_HPP
#define NLM_BATCH_HPP

#include "cli/NLMCLI.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>

namespace NLMBatch {

    // Batch experiment result
    struct BatchResult {
        std::string experimentName;
        double executionTime;
        bool success;
        std::string errorMessage;
        Phase6IntegrationResult result;
        size_t neuronsUsed;
        size_t stepsExecuted;
        double rewardAchieved;
        std::chrono::time_point<std::chrono::system_clock> startTime;
        std::chrono::time_point<std::chrono::system_clock> endTime;
    };

    // Batch processing configuration
    struct BatchConfig {
        std::string batchFile;
        int numThreads = 1;
        bool dryRun = false;
        bool validateOnly = false;
        bool continueOnError = false;
        std::string outputFormat = "console"; // console, json, csv
        std::string outputFile;
        bool showProgress = true;
        std::chrono::seconds progressInterval = 30;
        bool saveIndividualCheckpoints = false;
        std::string checkpointPrefix = "batch_";
        bool collectMetrics = true;
        bool generateReport = true;
        std::string reportFormat = "text"; // text, html, json
        std::map<std::string, std::string> globalOverrides;
    };

    // Batch file parser
    class BatchFileParser {
    public:
        static std::vector<BatchResult> parse(const std::string& filename);
        static bool validateFile(const std::string& filename, std::vector<std::string>& errors);
        static std::vector<std::string> parseLine(const std::string& line);
        static std::map<std::string, std::string> parseParameters(const std::vector<std::string>& tokens);
    };

    // Batch experiment runner
    class BatchExperimentRunner {
    private:
        BatchConfig config;
        std::mutex mutex;
        size_t completedExperiments = 0;
        size_t totalExperiments = 0;
        double totalReward = 0.0;
        double totalExecutionTime = 0.0;

    public:
        BatchExperimentRunner(const BatchConfig& cfg) : config(cfg) {}

        std::vector<BatchResult> run(const std::vector<BatchResult>& experiments);
        BatchResult runSingleExperiment(const BatchResult& experiment);
        void updateProgress(const BatchResult& result);
        void saveResult(const BatchResult& result);
        void generateReport(const std::vector<BatchResult>& results);
        bool shouldContinue(const BatchResult& result);
    };

    // Parallel batch execution
    namespace Parallel {
        template<typename T, typename F>
        std::vector<T> executeParallel(std::vector<T>& items, F&& worker, size_t numThreads = std::thread::hardware_concurrency()) {
            std::vector<std::future<T>> futures;
            futures.reserve(items.size());

            for (auto& item : items) {
                futures.emplace_back(std::async(std::launch::async, worker, std::move(item)));
            }

            std::vector<T> results;
            results.reserve(items.size());

            for (auto& future : futures) {
                results.emplace_back(future.get());
            }

            return results;
        }
    }

    // Utility functions
    std::string generateTimestamp();
    std::string formatDuration(double seconds);
    std::string escapeCSV(const std::string& str);
    std::string generateJSONReport(const std::vector<BatchResult>& results);
    std::string generateHTMLReport(const std::vector<BatchResult>& results);
    std::string generateTextReport(const std::vector<BatchResult>& results);
}

#endif // NLM_BATCH_HPP
