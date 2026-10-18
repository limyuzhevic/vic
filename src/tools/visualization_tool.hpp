// Visualization Tool Implementation
//
// This file provides the base class and common functionality for visualization CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class VisualizationTool : public CLITool {
public:
    VisualizationTool() = default;
    ~VisualizationTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::VISUALIZATION;
    }
    virtual std::string getCategory() const override {
        return "Visualization";
    }
    
    // Visualization-specific helper methods
    bool visualizeSimulation(const std::string& inputFile) const;
    bool visualizeResults(const std::string& inputFile) const;
    bool visualizeBrain(const std::string& inputFile) const;
    bool visualizeMetrics(const std::string& inputFile) const;
    bool visualizeComparison(const std::string& file1, const std::string& file2) const;
    
protected:
    // Visualization-specific implementations
    virtual bool visualizeSimulationImpl(const std::string& inputFile) const;
    virtual bool visualizeResultsImpl(const std::string& inputFile) const;
    virtual bool visualizeBrainImpl(const std::string& inputFile) const;
    virtual bool visualizeMetricsImpl(const std::string& inputFile) const;
    virtual bool visualizeComparisonImpl(const std::string& file1, const std::string& file2) const;
};

} // namespace nlm