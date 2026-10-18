// Network visualization tool
//
// This file provides the implementation of network visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizNetworkTool : public VisualizationTool {
public:
    VizNetworkTool() = default;
    ~VizNetworkTool() = default;
    
    std::string getName() const override {
        return "viz-network";
    }
    
    std::string getDescription() const override {
        return "Visualize network connectivity and structure";
    }
    
    std::string getUsage() const override {
        return "viz-network --brain brain_state.json [--type adjacency|region|population] [--output <file>] [--format png|pdf|svg|html] [--threshold <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeNetworkImpl(const std::string& brainFile, const std::string& outputFile,
                            const std::string& vizType, const std::string& format,
                            double threshold) const;
};

} // namespace nlm
