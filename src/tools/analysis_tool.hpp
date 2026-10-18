// Analysis Tool Implementation
//
// This file provides the base class and common functionality for analysis CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class AnalysisTool : public CLITool {
public:
    AnalysisTool() = default;
    ~AnalysisTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::ANALYSIS;
    }
    virtual std::string getCategory() const override {
        return "Analysis";
    }
    
    // Analysis-specific helper methods
    bool analyzeMetrics(const std::string& inputFile, const std::string& outputFile) const;
    bool analyzePerformance(const std::string& benchmarkFile) const;
    bool analyzeResults(const std::string& inputFile) const;
    bool analyzeMemory(const std::string& threshold) const;
    bool analyzeNeural(const std::string& inputFile) const;
    
protected:
    // Analysis-specific implementations
    virtual bool analyzeMetricsImpl(const std::string& inputFile, const std::string& outputFile) const;
    virtual bool analyzePerformanceImpl(const std::string& benchmarkFile) const;
    virtual bool analyzeResultsImpl(const std::string& inputFile) const;
    virtual bool analyzeMemoryImpl(const std::string& threshold) const;
    virtual bool analyzeNeuralImpl(const std::string& inputFile) const;
};

} // namespace nlm