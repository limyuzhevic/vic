// Visualization Tool Base Class
//
// This file provides the base class for visualization CLI tools.
// It provides common functionality and a framework for visualization tool development.

#pragma once

#include "tools/CLITool.hpp"
#include <string>

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
    
    // Visualization framework integration
    bool renderWithFramework(const std::string& name, const std::string& configFile) const;
    bool createVisualization(const std::string& name, const std::string& type,
                            const std::string& backend, const std::map<std::string, std::string>& params) const;
    
protected:
    // Visualization-specific implementations
    virtual bool visualizeSimulationImpl(const std::string& inputFile) const;
    virtual bool visualizeResultsImpl(const std::string& inputFile) const;
    virtual bool visualizeBrainImpl(const std::string& inputFile) const;
    virtual bool visualizeMetricsImpl(const std::string& inputFile) const;
    virtual bool visualizeComparisonImpl(const std::string& file1, const std::string& file2) const;
    
    virtual bool renderWithFrameworkImpl(const std::string& name, const std::string& configFile) const;
    virtual bool createVisualizationImpl(const std::string& name, const std::string& type,
                                        const std::string& backend, const std::map<std::string, std::string>& params) const;
};

} // namespace nlm
