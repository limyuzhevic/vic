// SubcommandHandlers.hpp - Handlers for each CLI subcommand
// Implements config, run, validate, export, and benchmark subcommands

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include "core/Config/ConfigPresets.hpp"

namespace nlm {
namespace CLI {

class ConfigHandler {
public:
    static bool handleConfig(const std::string& action, const std::string& preset, 
                           const std::string& format, const std::string& outputFile,
                           const std::unordered_map<std::string, std::string>& envVars);
private:
    static bool exportConfig(const std::string& preset, const std::string& format,
                           const std::string& outputFile);
    static bool listPresets();
    static bool showPresetInfo(const std::string& preset);
};

class RunHandler {
public:
    static int handleRun(const std::shared_ptr<Config>& config, bool benchmark,
                        bool dryRun, const std::string& outputFile,
                        const std::string& format);
private:
    static int runSimulation(const std::shared_ptr<Config>& config);
    static int runBenchmark(const std::shared_ptr<Config>& config);
    static int runDryRun(const std::shared_ptr<Config>& config);
};

class ValidateHandler {
public:
    static bool handleValidate(const std::shared_ptr<Config>& config,
                             const std::string& format, const std::string& outputFile);
private:
    static void printValidationResults(const ConfigValidator::ValidationResult& result,
                                     const std::string& format, const std::string& outputFile);
};

class ExportHandler {
public:
    static bool handleExport(const std::shared_ptr<Config>& config,
                           const std::string& format, const std::string& outputFile,
                           const std::vector<std::string>& keys);
private:
    static bool exportToText(const std::shared_ptr<Config>& config, 
                          const std::string& outputFile, const std::vector<std::string>& keys);
    static bool exportToJSON(const std::shared_ptr<Config>& config,
                          const std::string& outputFile, const std::vector<std::string>& keys);
    static bool exportToYAML(const std::shared_ptr<Config>& config,
                          const std::string& outputFile, const std::vector<std::string>& keys);
    static bool exportToCSV(const std::shared_ptr<Config>& config,
                          const std::string& outputFile, const std::vector<std::string>& keys);
    static bool exportToXML(const std::shared_ptr<Config>& config,
                          const std::string& outputFile, const std::vector<std::string>& keys);
};

class BenchmarkHandler {
public:
    static bool handleBenchmark(const std::shared_ptr<Config>& config,
                              const std::string& outputFile, const std::string& format,
                              int iterations, bool verbose);
private:
    static double measureExecutionTime(std::function<void()> task);
    static bool runPerformanceTest(const std::shared_ptr<Config>& config, int iterations);
    static bool compareBenchmarks(const std::string& baselineFile, 
                                 const std::string& currentFile);
};

} // namespace CLI
} // namespace nlm