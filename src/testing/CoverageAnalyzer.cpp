// CoverageAnalyzer.cpp
// Implementation of code coverage analysis for NLM

#include "CoverageAnalyzer.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace nlm {

CoverageAnalyzer::CoverageAnalyzer() : totalLines_(0), coveredLines_(0) {}

CoverageAnalyzer::~CoverageAnalyzer() {
    for (auto* file : files_) {
        delete file;
    }
    files_.clear();
}

void CoverageAnalyzer::addSourceFile(const std::string& filename, const std::vector<bool>& coverage) {
    SourceFile* file = new SourceFile();
    file->filename = filename;
    file->coverage = coverage;
    file->coveredLines = std::count(coverage.begin(), coverage.end(), true);
    file->totalLines = coverage.size();
    
    files_.push_back(file);
    totalLines_ += file->totalLines;
    coveredLines_ += file->coveredLines;
}

double CoverageAnalyzer::getOverallCoverage() const {
    return totalLines_ > 0 ? 
        static_cast<double>(coveredLines_) / totalLines_ * 100.0 : 0.0;
}

void CoverageAnalyzer::generateReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "=== NLM Code Coverage Report ===\n";
    file << "Generated on: " << getCurrentTimestamp() << "\n\n";
    
    file << "Summary:\n";
    file << "  Total lines: " << totalLines_ << "\n";
    file << "  Covered lines: " << coveredLines_ << "\n";
    file << "  Coverage: " << std::fixed << std::setprecision(2) 
         << getOverallCoverage() << "%\n\n";
    
    file << std::left << std::setw(60) << "File"
         << std::setw(15) << "Covered"
         << std::setw(15) << "Total"
         << std::setw(15) << "Coverage %" << "\n";
    
    file << std::string(105, '-') << "\n";
    
    for (const auto* file : files_) {
        double coveragePercent = file->totalLines > 0 ? 
            static_cast<double>(file->coveredLines) / file->totalLines * 100.0 : 0.0;
        
        file << std::left << std::setw(60) << file->filename
             << std::setw(15) << file->coveredLines
             << std::setw(15) << file->totalLines
             << std::setw(15) << std::fixed << std::setprecision(2) << coveragePercent << "\n";
    }
    
    file.close();
}

std::string CoverageAnalyzer::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

}} // namespace nlm