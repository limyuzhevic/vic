// Neuromodulation monitoring tool
//
// This file provides the implementation of neuromodulation monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorNeuromodulationTool : public MonitoringTool {
public:
    MonitorNeuromodulationTool() = default;
    ~MonitorNeuromodulationTool() = default;
    
    std::string getName() const override {
        return "monitor-neuromodulation";
    }
    
    std::string getDescription() const override {
        return "Monitor neuromodulation systems";
    }
    
    std::string getUsage() const override {
        return "monitor-neuromodulation --brain <file> --output <file> [--interval <float>] [--neuromod-type reward|curiosity|novelty|prediction_error] [--duration <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorNeuromodulationImpl(const std::string& brainFile, const std::string& outputFile,
                                    double interval, const std::string& neuromodType, double duration) const;
};

} // namespace nlm
