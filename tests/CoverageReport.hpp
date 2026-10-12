#include "framework/test_base.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>

class CoverageInfo {
public:
    struct FileCoverage {
        std::string filePath;
        size_t totalLines;
        size_t coveredLines;
        size_t skippedLines;
        double percentage;
        
        FileCoverage(const std::string& path, size_t total, size_t covered, size_t skipped)
            : filePath(path), totalLines(total), coveredLines(covered), skippedLines(skipped) {
            percentage = totalLines > 0 ? (static_cast<double>(coveredLines) / totalLines) * 100.0 : 0.0;
        }
    };
    
    void addFunctionCoverage(const std::string& functionName, bool covered) {
        if (covered) {
            coveredFunctions.insert(functionName);
        } else {
            uncoveredFunctions.insert(functionName);
        }
    }
    
    void addFileCoverage(const std::string& filePath, size_t totalLines, size_t coveredLines, size_t skippedLines) {
        filesCoverage.push_back(FileCoverage(filePath, totalLines, coveredLines, skippedLines));
    }
    
    void setOverallStats(size_t totalFunctions, size_t coveredFunctions, size_t totalLines, size_t coveredLines) {
        this->totalFunctions = totalFunctions;
        this->coveredFunctions = coveredFunctions;
        this->totalLines = totalLines;
        this->coveredLines = coveredLines;
        functionPercentage = totalFunctions > 0 ? (static_cast<double>(coveredFunctions) / totalFunctions) * 100.0 : 0.0;
        linePercentage = totalLines > 0 ? (static_cast<double>(coveredLines) / totalLines) * 100.0 : 0.0;
    }
    
    void printReport() const {
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM CODE COVERAGE REPORT" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        std::cout << "OVERALL COVERAGE STATISTICS:" << std::endl;
        std::cout << "  Total Functions: " << totalFunctions << std::endl;
        std::cout << "  Covered Functions: " << coveredFunctions << std::endl;
        std::cout << "  Function Coverage: " << std::fixed << std::setprecision(2) << functionPercentage << "%" << std::endl;
        std::cout << "  Total Lines: " << totalLines << std::endl;
        std::cout << "  Covered Lines: " << coveredLines << std::endl;
        std::cout << "  Line Coverage: " << std::fixed << std::setprecision(2) << linePercentage << "%" << std::endl;
        std::cout << std::endl;
        
        std::cout << "COVERED FUNCTIONS:" << std::endl;
        for (const auto& func : coveredFunctions) {
            std::cout << "  ✓ " << func << std::endl;
        }
        
        std::cout << std::endl << "UNCovered FUNCTIONS:" << std::endl;
        for (const auto& func : uncoveredFunctions) {
            std::cout << "  ✗ " << func << std::endl;
        }
        
        std::cout << std::endl << "FILE BREAKDOWN:" << std::endl;
        std::cout << std::left << std::setw(50) << "File" << std::setw(10) << "Total" 
                  << std::setw(10) << "Cover" << std::setw(10) << "Skip" << std::setw(15) << "Percent" << std::endl;
        std::cout << std::string(95, '-') << std::endl;
        
        for (const auto& file : filesCoverage) {
            std::cout << std::left << std::setw(50) << file.filePath 
                      << std::setw(10) << file.totalLines
                      << std::setw(10) << file.coveredLines
                      << std::setw(10) << file.skippedLines
                      << std::setw(15) << std::fixed << std::setprecision(2) << file.percentage << "%" << std::endl;
        }
        
        std::cout << std::endl << "=" << std::string(60, '=') << std::endl;
    }
    
    bool meetsThreshold(double lineThreshold = 80.0, double functionThreshold = 80.0) const {
        return linePercentage >= lineThreshold && functionPercentage >= functionThreshold;
    }
    
private:
    size_t totalFunctions = 0;
    size_t coveredFunctions = 0;
    double functionPercentage = 0.0;
    size_t totalLines = 0;
    size_t coveredLines = 0;
    double linePercentage = 0.0;
    std::unordered_set<std::string> coveredFunctions;
    std::unordered_set<std::string> uncoveredFunctions;
    std::vector<FileCoverage> filesCoverage;
};

class CoverageReporter {
public:
    static CoverageReporter& getInstance() {
        static CoverageReporter instance;
        return instance;
    }
    
    CoverageInfo& getCoverage() { return coverage; }
    
    void generateReport(const std::string& filename = "coverage_report.txt") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open coverage report file: " << filename << std::endl;
            return;
        }
        
        file << "NLM CODE COVERAGE REPORT" << std::endl;
        file << std::string(60, '=') << std::endl;
        file << std::endl;
        file << "OVERALL COVERAGE STATISTICS:" << std::endl;
        file << "  Total Functions: " << coverage.totalFunctions << std::endl;
        file << "  Covered Functions: " << coverage.coveredFunctions << std::endl;
        file << "  Function Coverage: " << std::fixed << std::setprecision(2) << coverage.functionPercentage << "%" << std::endl;
        file << "  Total Lines: " << coverage.totalLines << std::endl;
        file << "  Covered Lines: " << coverage.coveredLines << std::endl;
        file << "  Line Coverage: " << std::fixed << std::setprecision(2) << coverage.linePercentage << "%" << std::endl;
        file << std::endl;
        
        file << "COVERED FUNCTIONS:" << std::endl;
        for (const auto& func : coverage.coveredFunctions) {
            file << "  ✓ " << func << std::endl;
        }
        
        file << std::endl << "UNCOVERED FUNCTIONS:" << std::endl;
        for (const auto& func : coverage.uncoveredFunctions) {
            file << "  ✗ " << func << std::endl;
        }
        
        file << std::endl << "FILE BREAKDOWN:" << std::endl;
        file << std::left << std::setw(50) << "File" << std::setw(10) << "Total" 
              << std::setw(10) << "Cover" << std::setw(10) << "Skip" << std::setw(15) << "Percent" << std::endl;
        file << std::string(95, '-') << std::endl;
        
        for (const auto& fileInfo : coverage.filesCoverage) {
            file << std::left << std::setw(50) << fileInfo.filePath 
                  << std::setw(10) << fileInfo.totalLines
                  << std::setw(10) << fileInfo.coveredLines
                  << std::setw(10) << fileInfo.skippedLines
                  << std::setw(15) << std::fixed << std::setprecision(2) << fileInfo.percentage << "%" << std::endl;
        }
        
        file << std::endl << "=" << std::string(60, '=') << std::endl;
        file.close();
        
        printReport();
    }
    
private:
    CoverageReporter() {}
    CoverageInfo coverage;
};