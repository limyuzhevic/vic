#ifndef NLM_CLI_UTILS_COMMAND_HPP
#define NLM_CLI_UTILS_COMMAND_HPP

#include "CLI.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {
namespace cli {

class UtilsCommand : public Command {
public:
    UtilsCommand() = default;
    ~UtilsCommand() = default;
    
    std::string getName() const override { return "utils"; }
    std::string getDescription() const override { 
        return "Utility functions and development tools"; 
    }
    std::string getUsage() const override { 
        return "utils <UTILITY> [OPTIONS]"; 
    }
    bool isHidden() const override { return false; }
    
    Result execute(const std::vector<std::string>& args) override;
    Result executeWithConfig(const std::vector<std::string>& args, 
                           const std::string& configFile) override;
    
private:
    enum class UtilsSubcommand {
        Benchmark,
        Validate,
        Optimize,
        Profile,
        Monitor,
        Convert,
        Export,
        Import,
        Stats,
        Compare,
        ConvertFormat,
        Batch
    };
    
    UtilsSubcommand parseSubcommand(const std::string& name) const;
    
    Result executeBenchmark(const std::vector<std::string>& args, 
                           const std::string& configFile) const;
    Result executeValidate(const std::vector<std::string>& args, 
                          const std::string& configFile) const;
    Result executeOptimize(const std::vector<std::string>& args, 
                          const std::string& configFile) const;
    Result executeProfile(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result executeMonitor(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result executeConvert(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result executeExport(const std::vector<std::string>& args, 
                        const std::string& configFile) const;
    Result executeImport(const std::vector<std::string>& args, 
                        const std::string& configFile) const;
    Result executeStats(const std::vector<std::string>& args, 
                       const std::string& configFile) const;
    Result executeCompare(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result executeConvertFormat(const std::vector<std::string>& args, 
                               const std::string& configFile) const;
    Result executeBatch(const std::vector<std::string>& args, 
                       const std::string& configFile) const;
    
    Result runBenchmark(const std::map<std::string, std::string>& options) const;
    Result validateConfiguration(const std::string& configFile) const;
    Result optimizeParameters(const std::map<std::string, std::string>& options) const;
    Result runProfiler(const std::map<std::string, std::string>& options) const;
    Result startMonitor(const std::map<std::string, std::string>& options) const;
    Result convertConfiguration(const std::vector<std::string>& args, 
                               const std::string& configFile) const;
    Result exportResults(const std::vector<std::string>& args, 
                        const std::string& configFile) const;
    Result importResults(const std::vector<std::string>& args, 
                        const std::string& configFile) const;
    Result collectStatistics(const std::string& configFile) const;
    Result compareResults(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result convertFormat(const std::string& inputFile, 
                        const std::string& outputFile,
                        const std::string& format) const;
    Result runBatch(const std::vector<std::string>& args, 
                   const std::string& configFile) const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_UTILS_COMMAND_HPP