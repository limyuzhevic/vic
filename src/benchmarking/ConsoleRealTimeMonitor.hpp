// NLM Real-time Monitor - Header
// ConsoleRealTimeMonitor.hpp

#pragma once

#include "EnhancedBenchmarkManager.hpp"
#include <string>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>
#include <deque>

namespace nlm {
namespace benchmarking {

struct ConsoleRealTimeMonitor::Impl {
    std::thread updateThread;
    std::atomic<bool> running;
    std::deque<std::pair<double, BenchmarkResult>> history;
    size_t maxHistorySize;
    std::chrono::high_resolution_clock::time_point lastUpdate;
    
    Impl() : running(false), maxHistorySize(50) {}
};

class ConsoleRealTimeMonitor : public RealTimeMonitor {
public:
    ConsoleRealTimeMonitor(std::shared_ptr<Logger> logger);
    ~ConsoleRealTimeMonitor();
    
    void onBenchmarkStart(const BenchmarkConfig& config) override;
    void onBenchmarkProgress(double progress, const BenchmarkResult& current) override;
    void onBenchmarkComplete(const BenchmarkResult& result) override;
    void onBenchmarkError(const std::string& error) override;
    void onPause() override;
    void onResume() override;
    void onInterrupt() override;
    
    BenchmarkStatus getStatus() const override { return status_; }
    double getProgress() const override { return progress_; }
    std::string getCurrentOperation() const override { return currentOperation_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BenchmarkStatus status_;
    double progress_;
    std::string currentOperation_;
    std::shared_ptr<Logger> logger_;
    
    void updateLoop();
    std::string getStatusString(BenchmarkStatus status) const;
};

}}} // namespace nlm::benchmarking
