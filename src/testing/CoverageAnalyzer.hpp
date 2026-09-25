// CoverageAnalyzer.hpp
// Code coverage analysis for NLM

#pragma once

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

namespace nlm {

class CoverageAnalyzer {
public:
    struct SourceFile {
        std::string filename;
        std::vector<bool> coverage;
        size_t coveredLines;
        size_t totalLines;
    };
    
    CoverageAnalyzer();
    ~CoverageAnalyzer();
    
    void addSourceFile(const std::string& filename, const std::vector<bool>& coverage);
    double getOverallCoverage() const;
    void generateReport(const std::string& filename);
    
private:
    std::vector<SourceFile*> files_;
    size_t totalLines_;
    size_t coveredLines_;
    
    static std::string getCurrentTimestamp();
};

}} // namespace nlm