// Brain monitoring tool
//
// This file provides the implementation of brain monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorBrainTool : public MonitoringTool {
public:
    MonitorBrainTool() = default;
    ~MonitorBrainTool() = default;
    
    std::string getName() const override {
        return "monitor-brain";
    }
    
    std::string getDescription() const override {
        return "Monitor brain-specific performance";
    }
    
    std::string getUsage() const override {
        return "monitor-brain --brain <file> --output <file> [--interval <float>] [--metrics all|firing_rate|plasticity|connectivity]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorBrainImpl(const std::string& brainFile, const std::string& outputFile,
                         double interval, const std::string& metrics) const;
};

} // namespace nlm
