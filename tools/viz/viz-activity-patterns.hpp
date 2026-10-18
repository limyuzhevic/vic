// Activity patterns visualization tool
//
// This file provides the implementation of activity patterns visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizActivityPatternsTool : public VisualizationTool {
public:
    VizActivityPatternsTool() = default;
    ~VizActivityPatternsTool() = default;
    
    std::string getName() const override {
        return "viz-activity-patterns";
    }
    
    std::string getDescription() const override {
        return "Visualize activity patterns over time";
    }
    
    std::string getUsage() const override {
        return "viz-activity-patterns --simulation output.dat [--neuron-type excitatory|inhibitory|all] [--time-window <start> <end>] [--output <file>] [--format png|pdf|svg|html] [--pattern-type periodic|bursting|stationary|mixed]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeActivityPatternsImpl(const std::string& simulationFile, const std::string& neuronType,
                                      const std::string& outputFile, const std::string& format,
                                      double startTime, double endTime, const std::string& patternType) const;
};

} // namespace nlm
