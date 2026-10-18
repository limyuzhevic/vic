// Synapse visualization tool
//
// This file provides the implementation of synapse visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizSynapsesTool : public VisualizationTool {
public:
    VizSynapsesTool() = default;
    ~VizSynapsesTool() = default;
    
    std::string getName() const override {
        return "viz-synapses";
    }
    
    std::string getDescription() const override {
        return "Visualize synaptic connections and plasticity";
    }
    
    std::string getUsage() const override {
        return "viz-synapses --brain brain_state.json [--plasticity true|false] [--output <file>] [--format png|pdf|svg|html] [--type structural|functional|temporal]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeSynapsesImpl(const std::string& brainFile, bool plasticity,
                              const std::string& outputFile, const std::string& format,
                              const std::string& vizType) const;
};

} // namespace nlm
