// Plasticity monitoring tool
//
// This file provides the implementation of plasticity monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorPlasticityTool : public MonitoringTool {
public:
    MonitorPlasticityTool() = default;
    ~MonitorPlasticityTool() = default;
    
    std::string getName() const override {
        return "monitor-plasticity";
    }
    
    std::string getDescription() const override {
        return "Monitor plasticity mechanisms";
    }
    
    std::string getUsage() const override {
        return "monitor-plasticity --brain <file> --output <file> [--interval <float>] [--plasticity-type stdp|hebbian|structural] [--duration <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorPlasticityImpl(const std::string& brainFile, const std::string& outputFile,
                              double interval, const std::string& plasticityType, double duration) const;
};

} // namespace nlm
