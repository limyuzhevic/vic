// Neuron visualization tool
//
// This file provides the implementation of neuron visualization CLI tool

#pragma once

#include "tools/viz/VisualizationTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class VizNeuronTool : public VisualizationTool {
public:
    VizNeuronTool() = default;
    ~VizNeuronTool() = default;
    
    std::string getName() const override {
        return "viz-neuron";
    }
    
    std::string getDescription() const override {
        return "Visualize individual neuron properties and activity";
    }
    
    std::string getUsage() const override {
        return "viz-neuron --brain brain_state.json --neuron <id> --output <file> [--format png|pdf|svg|html] [--type membrane_potential|spike_raster|connectivity|plasticity]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool visualizeNeuronImpl(const std::string& brainFile, int neuronId,
                           const std::string& outputFile, const std::string& format,
                           const std::string& vizType) const;
};

} // namespace nlm
