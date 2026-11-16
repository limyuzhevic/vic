# AnalysisTool.hpp

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "Tools.hpp"
#include "ToolsCommon.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class AnalysisToolImpl;

struct AnalysisResult {
    std::string scenarioName;
    std::string inputDirectory;
    std::string outputFile;
    bool success;
    std::string errorMessage;
    std::chrono::duration<double> executionTime;
    std::map<std::string, double> metrics;
    std::map<std::string, std::string> metadata;
};

class AnalysisTool {
public:
    AnalysisTool();
    ~AnalysisTool();
    
    int run(int argc, char** argv);
    
    int analyzeCommand(int argc, char** argv);
    int compareCommand(int argc, char** argv);
    int statisticalCommand(int argc, char** argv);
    int trendCommand(int argc, char** argv);

private:
    ToolConfig toolConfig_;
    std::unique_ptr<AnalysisToolImpl> toolImpl_;
    
    void printHelp();
};

} // namespace nlm