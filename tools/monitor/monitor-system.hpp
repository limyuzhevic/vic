// System monitoring tool
//
// This file provides the implementation of system monitoring CLI tool

#pragma once

#include "tools/monitor/MonitoringTool.hpp"
#include <string>
#include <vector>

namespace nlm {

class MonitorSystemTool : public MonitoringTool {
public:
    MonitorSystemTool() = default;
    ~MonitorSystemTool() = default;
    
    std::string getName() const override {
        return "monitor-system";
    }
    
    std::string getDescription() const override {
        return "Monitor system-wide performance";
    }
    
    std::string getUsage() const override {
        return "monitor-system --output <file> [--interval <float>] [--format json|csv|txt] [--duration <float>]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool monitorSystemImpl(const std::string& outputFile, double interval, const std::string& format, double duration) const;
};

} // namespace nlm
