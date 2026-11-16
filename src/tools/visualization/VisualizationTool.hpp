# VisualizationTool.hpp

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Tools.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class VisualizationToolImpl;

struct VisualizationConfig {
    std::string inputFile;
    std::string outputFile;
    std::string outputType;
    std::string chartType;
    std::string title;
    bool showLegend;
    bool darkMode;
    std::map<std::string, std::string> options;
};

class VisualizationTool {
public:
    VisualizationTool();
    ~VisualizationTool();
    
    int run(int argc, char** argv);
    
    int visualizeCommand(int argc, char** argv);

private:
    ToolConfig toolConfig_;
    std::unique_ptr<VisualizationToolImpl> toolImpl_;
    
    void printHelp();
};

} // namespace nlm