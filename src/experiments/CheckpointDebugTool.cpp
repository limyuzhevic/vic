// Checkpoint Debugging and Profiling Tool
// Advanced checkpoint validation, analysis, and recovery tools

#include "performance/CheckpointSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

using namespace nlm;

namespace {

// Forward declarations for checkpoint debugging tools
void generateDiagnosticReport(const std::string& filepath, const std::string& outputFile);
void printUsage(const std::string& programName);

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string checkpointFile;
    std::string outputFile;
    bool analyze = false;
    bool profile = false;
    bool recover = false;
    bool generateReport = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--analyze") {
            analyze = true;
        } else if (arg == "--profile") {
            profile = true;
        } else if (arg == "--recover") {
            recover = true;
        } else if (arg == "--generate-report") {
            generateReport = true;
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (checkpointFile.empty()) {
            checkpointFile = arg;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (checkpointFile.empty()) {
        std::cerr << "Error: Checkpoint file not specified" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    std::cout << "=== NLM Checkpoint Debugging Tool ===" << std::endl;
    std::cout << "Checkpoint File: " << checkpointFile << std::endl;
    std::cout << std::endl;
    
    if (analyze) {
        std::cout << "=== Analyzing Checkpoint ===" << std::endl;
        auto analysis = CheckpointAnalyzer::analyzeCheckpoint(checkpointFile);
        
        std::cout << "Health Status: " << analysis.healthStatus << std::endl;
        std::cout << "Integrity Score: " << std::fixed << std::setprecision(1) << analysis.integrityScore << "%" << std::endl;
        std::cout << "Corruption Risk: " << std::fixed << std::setprecision(1) << analysis.fileCorruptionRisk * 100 << "%" << std::endl;
        std::cout << "Can Load: " << (analysis.canLoad ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        
        if (!analysis.errors.empty()) {
            std::cout << "ERRORS:" << std::endl;
            for (const auto& error : analysis.errors) {
                std::cout << "  - " << error << std::endl;
            }
            std::cout << std::endl;
        }
        
        if (!analysis.warnings.empty()) {
            std::cout << "WARNINGS:" << std::endl;
            for (const auto& warning : analysis.warnings) {
                std::cout << "  - " << warning << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    if (profile) {
        std::cout << "=== Profiling Checkpoint ===" << std::endl;
        auto profile = CheckpointProfiler::profileCheckpoint(checkpointFile, "load");
        
        std::cout << "File Size: " << profile.fileSize << " bytes" << std::endl;
        std::cout << "Read Speed: " << profile.readSpeed << " bytes/second" << std::endl;
        std::cout << "Read Time: " << std::fixed << std::setprecision(3) << profile.readTime << "s" << std::endl;
        
        if (profile.bottleneck != "None detected") {
            std::cout << "Bottleneck Detected: " << profile.bottleneck << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    if (recover) {
        std::cout << "=== Recovering Checkpoint ===" << std::endl;
        auto recovery = CheckpointRecovery::recoverCheckpoint(checkpointFile);
        
        std::cout << "Success: " << (recovery.success ? "Yes" : "No") << std::endl;
        std::cout << "Message: " << recovery.message << std::endl;
        std::cout << "Recovered Bytes: " << recovery.recoveredBytes << std::endl;
        
        if (!recovery.recoverySteps.empty()) {
            std::cout << "Recovery Steps:" << std::endl;
            for (const auto& step : recovery.recoverySteps) {
                std::cout << "  - " << step << std::endl;
            }
        }
        
        std::cout << std::endl;
    }
    
    if (generateReport) {
        std::cout << "=== Generating Diagnostic Report ===" << std::endl;
        std::string reportFile = outputFile.empty() ? checkpointFile + ".diagnostic.txt" : outputFile;
        generateDiagnosticReport(checkpointFile, reportFile);
        
        std::cout << "Report generated: " << reportFile << std::endl;
    }
    
    // If no specific action requested, perform basic analysis
    if (!analyze && !profile && !recover && !generateReport) {
        std::cout << "=== Basic Checkpoint Analysis ===" << std::endl;
        auto analysis = CheckpointAnalyzer::analyzeCheckpoint(checkpointFile);
        
        std::cout << "Health Status: " << analysis.healthStatus << std::endl;
        std::cout << "Integrity Score: " << std::fixed << std::setprecision(1) << analysis.integrityScore << "%" << std::endl;
        std::cout << "Can Load: " << (analysis.canLoad ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        
        if (!analysis.errors.empty()) {
            std::cout << "CRITICAL ERRORS:" << std::endl;
            for (const auto& error : analysis.errors) {
                std::cout << "  - " << error << std::endl;
            }
            std::cout << std::endl;
            return 1;
        }
        
        if (!analysis.warnings.empty()) {
            std::cout << "WARNINGS:" << std::endl;
            for (const auto& warning : analysis.warnings) {
                std::cout << "  - " << warning << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] <checkpoint_file>" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --analyze              Analyze checkpoint integrity and health" << std::endl;
    std::cout << "  --profile              Profile checkpoint read/write performance" << std::endl;
    std::cout << "  --recover              Attempt to recover corrupted checkpoint" << std::endl;
    std::cout << "  --generate-report      Generate comprehensive diagnostic report" << std::endl;
    std::cout << "  --output FILE          Output file for diagnostic report" << std::endl;
    std::cout << "  --help                 Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --analyze checkpoint.ckpt" << std::endl;
    std::cout << "  " << programName << " --profile --generate-report checkpoint.ckpt --output report.html" << std::endl;
    std::cout << "  " << programName << " --recover corrupted.ckpt" << std::endl;
}