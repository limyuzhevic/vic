// Space-time visualization tool
//
// This file provides the implementation of space-time visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizSpaceTimeTool : public VisualizationTool {
public:
    VizSpaceTimeTool() = default;
    ~VizSpaceTimeTool() = default;
    
    std::string getName() const override {
        return "viz-space-time";
    }
    
    std::string getDescription() const override {
        return "Visualize space-time activity patterns";
    }
    
    std::string getUsage() const override {
        return "viz-space-time --simulation output.dat [--space-scale <float>] [--time-scale <float>] [--output <file>] [--format png|pdf|svg|html] [--visualization 2d|3d|heatmap|isosurface]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeSpaceTimeImpl(const std::string& simulationFile, double spaceScale, double timeScale,
                               const std::string& outputFile, const std::string& format,
                               const std::string& visualizationType) const;
};

} // namespace nlm
