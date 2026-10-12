#include "CoverageReport.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

CoverageInfo::CoverageInfo() : totalFunctions(0), coveredFunctions(0), functionPercentage(0.0),
                               totalLines(0), coveredLines(0), linePercentage(0.0) {}

CoverageInfo::~CoverageInfo() {}

CoverageReporter::CoverageReporter() {}

CoverageReporter::~CoverageReporter() {}

CoverageReporter& CoverageReporter::getInstance() {
    static CoverageReporter instance;
    return instance;
}

CoverageInfo& CoverageReporter::getCoverage() {
    return coverage;
}

void CoverageReporter::generateReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open coverage report file: " << filename << std::endl;
        return;
    }
    
    // Write report to file
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

void CoverageReporter::printReport() {
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "NLM CODE COVERAGE REPORT" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    std::cout << "OVERALL COVERAGE STATISTICS:" << std::endl;
    std::cout << "  Total Functions: " << coverage.totalFunctions << std::endl;
    std::cout << "  Covered Functions: " << coverage.coveredFunctions << std::endl;
    std::cout << "  Function Coverage: " << std::fixed << std::setprecision(2) << coverage.functionPercentage << "%" << std::endl;
    std::cout << "  Total Lines: " << coverage.totalLines << std::endl;
    std::cout << "  Covered Lines: " << coverage.coveredLines << std::endl;
    std::cout << "  Line Coverage: " << std::fixed << std::setprecision(2) << coverage.linePercentage << "%" << std::endl;
    std::cout << std::endl;
    
    std::cout << "COVERED FUNCTIONS:" << std::endl;
    for (const auto& func : coverage.coveredFunctions) {
        std::cout << "  ✓ " << func << std::endl;
    }
    
    std::cout << std::endl << "UNCOVERED FUNCTIONS:" << std::endl;
    for (const auto& func : coverage.uncoveredFunctions) {
        std::cout << "  ✗ " << func << std::endl;
    }
    
    std::cout << std::endl << "FILE BREAKDOWN:" << std::endl;
    std::cout << std::left << std::setw(50) << "File" << std::setw(10) << "Total" 
              << std::setw(10) << "Cover" << std::setw(10) << "Skip" << std::setw(15) << "Percent" << std::endl;
    std::cout << std::string(95, '-') << std::endl;
    
    for (const auto& file : coverage.filesCoverage) {
        std::cout << std::left << std::setw(50) << file.filePath 
                  << std::setw(10) << file.totalLines
                  << std::setw(10) << file.coveredLines
                  << std::setw(10) << file.skippedLines
                  << std::setw(15) << std::fixed << std::setprecision(2) << file.percentage << "%" << std::endl;
    }
    
    std::cout << std::endl << "=" << std::string(60, '=') << std::endl;
}