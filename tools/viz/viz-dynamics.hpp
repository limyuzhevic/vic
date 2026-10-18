// Dynamics visualization tool
//
// This file provides the implementation of dynamics visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizDynamicsTool : public VisualizationTool {
public:
    VizDynamicsTool() = default;
    ~VizDynamicsTool() = default;
    
    std::string getName() const override {
        return "viz-dynamics";
    }
    
    std::string getDescription() const override {
        return "Visualize neural dynamics and activity patterns";
    }
    
    std::string getUsage() const override {
        return "viz-dynamics --simulation output.dat [--variable membrane_potential|spike_count|population_activity] [--neuron-type excitatory|inhibitory|all] [--output <file>] [--format png|pdf|svg|html] [--time-range start end]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeDynamicsImpl(const std::string& simulationFile, const std::string& outputFile,
                              const std::string& variable, const std::string& neuronType,
                              const std::string& format, double startTime, double endTime) const;
};

} // namespace nlm
