// Data export for visualization tool
//
// This file provides the implementation of data export for visualization CLI tool

#pragma once

#include "tools/data/VizDataExport.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

class VizDataExportTool : public VizDataExport {
public:
    VizDataExportTool() = default;
    ~VizDataExportTool() = default;
    
    std::string getName() const override {
        return "viz-data-export";
    }
    
    std::string getDescription() const override {
        return "Export data for external visualization";
    }
    
    std::string getUsage() const override {
        return "viz-data-export --simulation <file> --format <format> --output <file> [--options]";
    }
    
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override;
    
protected:
    bool exportDataImpl(const std::string& simulationFile, const std::string& outputFile,
                       const std::string& format, const std::map<std::string, std::string>& options) const;
};

} // namespace nlm
