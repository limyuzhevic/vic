// Monitoring Tool Base Class
//
// This file provides the base class for monitoring CLI tools.
// It provides common functionality and a framework for monitoring tool development.

#pragma once

#include "tools/CLITool.hpp"
#include <string>

namespace nlm {

class MonitoringTool : public CLITool {
public:
    MonitoringTool() = default;
    ~MonitoringTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::MONITORING;
    }
    virtual std::string getCategory() const override {
        return "Monitoring";
    }
    
    // Monitoring-specific helper methods
    bool monitorSystem(const std::string& outputFile, double interval) const;
    bool monitorBrain(const std::string& brainFile, const std::string& outputFile) const;
    bool monitorPerformance(const std::string& outputFile, double duration) const;
    bool monitorPlasticity(const std::string& brainFile, const std::string& outputFile) const;
    bool monitorMemory(const std::string& brainFile, const std::string& outputFile) const;
    bool monitorDevelopment(const std::string& brainFile, const std::string& outputFile) const;
    bool monitorNeuromodulation(const std::string& brainFile, const std::string& outputFile) const;
    
protected:
    // Monitoring-specific implementations
    virtual bool monitorSystemImpl(const std::string& outputFile, double interval) const;
    virtual bool monitorBrainImpl(const std::string& brainFile, const std::string& outputFile) const;
    virtual bool monitorPerformanceImpl(const std::string& outputFile, double duration) const;
    virtual bool monitorPlasticityImpl(const std::string& brainFile, const std::string& outputFile) const;
    virtual bool monitorMemoryImpl(const std::string& brainFile, const std::string& outputFile) const;
    virtual bool monitorDevelopmentImpl(const std::string& brainFile, const std::string& outputFile) const;
    virtual bool monitorNeuromodulationImpl(const std::string& brainFile, const std::string& outputFile) const;
};

} // namespace nlm
