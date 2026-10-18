// Brain connectivity visualization tool
//
// This file provides the implementation of brain connectivity visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizBrainConnectivityTool : public VisualizationTool {
public:
    VizBrainConnectivityTool() = default;
    ~VizBrainConnectivityTool() = default;
    
    std::string getName() const override {
        return "viz-brain-connectivity";
    }
    
    std::string getDescription() const override {
        return "Visualize brain connectivity matrix";
    }
    
    std::string getUsage() const override {
        return "viz-brain-connectivity --brain brain_state.json [--threshold <float>] [--output <file>] [--format png|pdf|svg|html] [--matrix-type adjacency|correlation|efficiency]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeBrainConnectivityImpl(const std::string& brainFile, double threshold,
                                       const std::string& outputFile, const std::string& format,
                                       const std::string& matrixType) const;
};

} // namespace nlm
