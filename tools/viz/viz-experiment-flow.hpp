// Experiment flow visualization tool
//
// This file provides the implementation of experiment flow visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizExperimentFlowTool : public VisualizationTool {
public:
    VizExperimentFlowTool() = default;
    ~VizExperimentFlowTool() = default;
    
    std::string getName() const override {
        return "viz-experiment-flow";
    }
    
    std::string getDescription() const override {
        return "Visualize experiment execution flow";
    }
    
    std::string getUsage() const override {
        return "viz-experiment-flow --experiment <name> [--timeline] [--output <file>] [--format png|pdf|svg|html] [--include-metrics true|false]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeExperimentFlowImpl(const std::string& experimentName, bool timeline,
                                    const std::string& outputFile, const std::string& format,
                                    bool includeMetrics) const;
};

} // namespace nlm
