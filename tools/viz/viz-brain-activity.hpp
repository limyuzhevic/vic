// Brain activity visualization tool
//
// This file provides the implementation of brain activity visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizBrainActivityTool : public VisualizationTool {
public:
    VizBrainActivityTool() = default;
    ~VizBrainActivityTool() = default;
    
    std::string getName() const override {
        return "viz-brain-activity";
    }
    
    std::string getDescription() const override {
        return "Visualize brain-wide activity patterns";
    }
    
    std::string getUsage() const override {
        return "viz-brain-activity --brain brain_state.json [--neuron-type all|excitatory|inhibitory] [--output <file>] [--format png|pdf|svg|html] [--time-window <start> <end>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeBrainActivityImpl(const std::string& brainFile, const std::string& neuronType,
                                   const std::string& outputFile, const std::string& format,
                                   double startTime, double endTime) const;
};

} // namespace nlm
