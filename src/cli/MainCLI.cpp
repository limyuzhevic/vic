// NLM (熙然) - Neural Learning Machine
// Main CLI Implementation
// Implementation of the main CLI interface with enhanced functionality

#include "cli/MainCLI.hpp"
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;

namespace nlm {

// Helper functions
namespace {

    // Parse output format from string
    OutputFormat parseOutputFormat(const std::string& formatStr) {
        std::string lower = formatStr;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "json") return OutputFormat::JSON;
        if (lower == "csv") return OutputFormat::CSV;
        if (lower == "text") return OutputFormat::Text;
        
        return OutputFormat::Text;
    }

    // Parse execution mode from string
    ExecutionMode parseExecutionMode(const std::string& modeStr) {
        std::string lower = modeStr;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "brain-only" || lower == "brain_only") return ExecutionMode::BrainOnly;
        if (lower == "agent-world" || lower == "agent_world") return ExecutionMode::AgentWorldSimulation;
        if (lower == "development" || lower == "development-learning") return ExecutionMode::DevelopmentLearning;
        if (lower == "analysis" || lower == "debugging") return ExecutionMode::AnalysisDebugging;
        if (lower == "batch") return ExecutionMode::BatchProcessing;
        
        return ExecutionMode::AgentWorldSimulation;
    }

    // Generate unique output filename
    std::string generateOutputFilename(const std::string& baseDir, const std::string& prefix, 
                                       const std::string& extension) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << baseDir << "/" << prefix << "_" << time << "_" << rand() % 10000 << extension;
        return ss.str();
    }

    // Create output directory if it doesn't exist
    void ensureOutputDirectory(const std::string& filename) {
        size_t pos = filename.find_last_of('/');
        if (pos != std::string::npos) {
            std::string dir = filename.substr(0, pos);
            // In a real implementation, we would create the directory
            // For now, we'll just check if it exists
        }
    }

} // namespace

// Main CLI implementation
namespace {

    // Custom log function for CLI
    void cliLog(const std::string& message, LogLevel level) {
        std::string levelStr;
        switch (level) {
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Warning: levelStr = "WARN"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Quiet: levelStr = ""; break;
            default: levelStr = "INFO"; break;
        }
        
        if (!levelStr.empty()) {
            std::cout << "[" << levelStr << "]";
            if (level != LogLevel::Quiet) {
                std::cout << " [" << MainCLI::getInstance().getTimestamp() << "] " << message;
            }
        } else {
            std::cout << "[" << MainCLI::getInstance().getTimestamp() << "] " << message;
        }
    }

} // namespace

// Singleton implementation for MainCLI
namespace {
    std::unique_ptr<MainCLI> instance_;
    std::mutex instanceMutex_;
}

MainCLI& MainCLI::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = std::make_unique<MainCLI>();
    }
    return *instance_.get();
}

MainCLI::MainCLI() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set up signal handlers for graceful shutdown
    // (In a real implementation, we would set up signal handlers)
}

MainCLI::~MainCLI() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    instance_.reset();
}

// Global function to get timestamp
std::string MainCLI::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace nlm
