// Network dynamics visualization tool
//
// This file provides the implementation of network dynamics visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizNetworkDynamicsTool : public VisualizationTool {
public:
    VizNetworkDynamicsTool() = default;
    ~VizNetworkDynamicsTool() = default;
    
    std::string getName() const override {
        return "viz-network-dynamics";
    }
    
    std::string getDescription() const override {
        return "Visualize network-level dynamics";
    }
    
    std::string getUsage() const override {
        return "viz-network-dynamics --simulation output.dat [--frequency <float>] [--mode oscillation|propagation|diffusion] [--output <file>] [--format png|pdf|svg|html]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeNetworkDynamicsImpl(const std::string& simulationFile, const std::string& outputFile,
                                     const std::string& frequency, const std::string& mode,
                                     const std::string& format) const;
};

} // namespace nlm
