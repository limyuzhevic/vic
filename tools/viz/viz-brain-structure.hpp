// Brain structure visualization tool
//
// This file provides the implementation of brain structure visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizBrainStructureTool : public VisualizationTool {
public:
    VizBrainStructureTool() = default;
    ~VizBrainStructureTool() = default;
    
    std::string getName() const override {
        return "viz-brain-structure";
    }
    
    std::string getDescription() const override {
        return "Visualize brain structure and regions";
    }
    
    std::string getUsage() const override {
        return "viz-brain-structure --brain brain_state.json [--region-type all|cortex|subcortex|brainstem] [--output <file>] [--format png|pdf|svg|html] [--detailed true|false]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeBrainStructureImpl(const std::string& brainFile, const std::string& regionType,
                                    const std::string& outputFile, const std::string& format,
                                    bool detailed) const;
};

} // namespace nlm
