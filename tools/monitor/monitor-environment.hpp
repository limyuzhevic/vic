// Environment monitoring tool
//
// This file provides the implementation of environment monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorEnvironmentTool : public MonitoringTool {
public:
    MonitorEnvironmentTool() = default;
    ~MonitorEnvironmentTool() = default;
    
    std::string getName() const override {
        return "monitor-environment";
    }
    
    std::string getDescription() const override {
        return "Monitor environment performance";
    }
    
    std::string getUsage() const override {
        return "monitor-environment --output <file> [--interval <float>] [--env-type normal|harsh|extreme] [--duration <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorEnvironmentImpl(const std::string& outputFile, double interval, const std::string& envType, double duration) const;
};

} // namespace nlm
