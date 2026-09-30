// NLM Real-time Monitor - Implementation
// ConsoleRealTimeMonitor.cpp

#include "ConsoleRealTimeMonitor.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace benchmarking {

ConsoleRealTimeMonitor::ConsoleRealTimeMonitor(std::shared_ptr<Logger> logger)
    : logger_(logger) {
    pImpl = std::make_unique<Impl>();
    status_ = BenchmarkStatus::IDLE;
    progress_ = 0.0;
    currentOperation_ = "Idle";
}

ConsoleRealTimeMonitor::~ConsoleRealTimeMonitor() {
    if (pImpl->running) {
        pImpl->running = false;
        if (pImpl->updateThread.joinable()) {
            pImpl->updateThread.join();
        }
    }
}

void ConsoleRealTimeMonitor::onBenchmarkStart(const BenchmarkConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = BenchmarkStatus::RUNNING;
    progress_ = 0.0;
    currentOperation_ = "Starting benchmark: " + config.name;
    
    NLM_LOG_INFO("=== Benchmark Started ===");
    NLM_LOG_INFO("Benchmark: " + config.name);
    NLM_LOG_INFO("Type: " + config.getBenchmarkTypeString());
    NLM_LOG_INFO("Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("Steps: " + std::to_string(config.steps));
    NLM_LOG_INFO("Regions: " + std::to_string(config.regions));
    NLM_LOG_INFO("Memory Tracking: " + std::string(config.enableMemoryTracking ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("CPU Monitoring: " + std::string(config.enableCPUMonitoring ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Real-time Reporting: " + std::string(config.enableRealTimeReporting ? "Enabled" : "Disabled"));
    
    // Start update thread
    pImpl->running = true;
    pImpl->updateThread = std::thread(&ConsoleRealTimeMonitor::updateLoop, this);
}

void ConsoleRealTimeMonitor::onBenchmarkProgress(double progress, const BenchmarkResult& current) {
    std::lock_guard<std::mutex> lock(mutex_);
    progress_ = progress;
    
    // Add to history
    pImpl->history.emplace_back(progress, current);
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        pImpl->history.pop_front();
    }
    
    // Clear screen and display progress
    system("clear"); // Use "clear" for Unix, "cls" for Windows
    
    std::cout << "\n╔══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      NLM Enhanced Performance Benchmark                       ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Benchmark: " << std::setw(50) << std::left << current.benchmarkName << "║" << std::endl;
    std::cout << "║ Type: " << std::setw(50) << std::left << current.benchmarkName << "║" << std::endl; // TODO: Use actual type
    std::cout << "║ Progress: " << std::setw(50) << std::left << std::fixed << std::setprecision(2) 
              << (progress * 100.0) << "% " << "║" << std::endl;
    std::cout << "║ Status: " << std::setw(50) << std::left << getStatusString(status_) << " ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Display current metrics
    std::cout << "║ Current Metrics:" << std::endl;
    std::cout << "║   Steps/sec: " << std::setw(47) << std::left 
              << std::fixed << std::setprecision(2) << current.stepsPerSecond << "║" << std::endl;
    std::cout << "║   Total Time: " << std::setw(47) << std::left 
              << std::fixed << std::setprecision(2) << current.getTotalTimeSeconds() << "s " << "║" << std::endl;
    std::cout << "║   Memory: " << std::setw(47) << std::left 
              << current.memoryUsage.toString() << " " << "║" << std::endl;
    std::cout << "║   CPU: " << std::setw(47) << std::left 
              << current.cpuMetrics.toString() << " " << "║" << std::endl;
    std::cout << "║   Spikes: " << std::setw(47) << std::left 
              << current.neuralMetrics.spikeCount << " " << "║" << std::endl;
    std::cout << "║   Firing Neurons: " << std::setw(43) << std::left 
              << current.neuralMetrics.firingNeuronCount << " " << "║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Display recent history
    if (!pImpl->history.empty()) {
        std::cout << "║ Recent Performance:" << std::endl;
        size_t displayCount = std::min(static_cast<size_t>(5), pImpl->history.size());
        for (size_t i = pImpl->history.size() - displayCount; i < pImpl->history.size(); ++i) {
            double histProgress = pImpl->history[i].first * 100.0;
            double histThroughput = pImpl->history[i].second.stepsPerSecond;
            std::cout << "║   " << std::setw(3) << (i - pImpl->history.size() + displayCount + 1) << ": "
                      << std::setw(6) << std::fixed << std::setprecision(1) << histProgress << "% "
                      << "throughput: " << std::setw(8) << std::fixed << std::setprecision(1) 
                      << histThroughput << " steps/sec" << std::endl;
        }
    }
    
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // Show controls
    std::cout << "\nControls: [P]ause, [C]ancel, [R]esume, [Q]uit" << std::endl;
}

void ConsoleRealTimeMonitor::onBenchmarkComplete(const BenchmarkResult& result) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = BenchmarkStatus::COMPLETED;
    progress_ = 1.0;
    currentOperation_ = "Benchmark completed successfully";
    
    // Stop update thread
    pImpl->running = false;
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
    
    NLM_LOG_INFO("\n=== Benchmark Completed ===");
    NLM_LOG_INFO("Benchmark: " + result.benchmarkName);
    NLM_LOG_INFO("Total Time: " + std::to_string(result.getTotalTimeSeconds()) + "s");
    NLM_LOG_INFO("Steps per Second: " + std::to_string(result.stepsPerSecond));
    NLM_LOG_INFO("Memory Usage: " + result.memoryUsage.toString());
    NLM_LOG_INFO("CPU Usage: " + result.cpuMetrics.toString());
    NLM_LOG_INFO("Total Spikes: " + std::to_string(result.neuralMetrics.spikeCount));
    NLM_LOG_INFO("Firing Neurons: " + std::to_string(result.neuralMetrics.firingNeuronCount));
}

void ConsoleRealTimeMonitor::onBenchmarkError(const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = BenchmarkStatus::ERROR;
    currentOperation_ = "Benchmark error: " + error;
    
    // Stop update thread
    pImpl->running = false;
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
    
    NLM_LOG_ERROR("\n=== Benchmark Error ===");
    NLM_LOG_ERROR("Error: " + error);
}

void ConsoleRealTimeMonitor::onPause() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = BenchmarkStatus::PAUSED;
    currentOperation_ = "Benchmark paused";
    NLM_LOG_INFO("Benchmark paused");
}

void ConsoleRealTimeMonitor::onResume() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = BenchmarkStatus::RUNNING;
    currentOperation_ = "Benchmark resumed";
    NLM_LOG_INFO("Benchmark resumed");
}

void ConsoleRealTimeMonitor::onInterrupt() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = BenchmarkStatus::CANCELLED;
    currentOperation_ = "Benchmark cancelled";
    
    // Stop update thread
    pImpl->running = false;
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
    
    NLM_LOG_INFO("\n=== Benchmark Cancelled ===");
    NLM_LOG_INFO("Benchmark was cancelled by user");
}

void ConsoleRealTimeMonitor::updateLoop() {
    while (pImpl->running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastUpdate).count();
        
        if (elapsed >= 1) {
            pImpl->lastUpdate = now;
            
            // Refresh console display if status is running
            if (status_ == BenchmarkStatus::RUNNING) {
                // The console display is updated in onBenchmarkProgress
                // This thread just keeps the monitor running
            }
        }
    }
}

std::string ConsoleRealTimeMonitor::getStatusString(BenchmarkStatus status) const {
    switch (status) {
        case BenchmarkStatus::IDLE: return "IDLE";
        case BenchmarkStatus::RUNNING: return "RUNNING";
        case BenchmarkStatus::PAUSED: return "PAUSED";
        case BenchmarkStatus::COMPLETED: return "COMPLETED";
        case BenchmarkStatus::CANCELLED: return "CANCELLED";
        case BenchmarkStatus::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

}}} // namespace nlm::benchmarking
