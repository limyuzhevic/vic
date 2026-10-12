// NLM (熙然) - Neural Learning Machine CLI Progress Monitor
//
// This file implements the progress monitoring system for the CLI,
// providing real-time feedback for long-running operations.

#ifndef NLM_PROGRESS_MONITOR_HPP
#define NLM_PROGRESS_MONITOR_HPP

#include <string>
#include <chrono>
#include <thread>

namespace nlm {
namespace cli {

// Progress Monitor for long-running operations
class ProgressMonitor {
public:
    ProgressMonitor();
    ~ProgressMonitor() = default;
    
    // Start monitoring progress
    void start(const std::string& operation, int totalSteps);
    
    // Update progress
    void update(int currentStep, const std::string& message = "");
    
    // Stop monitoring
    void stop();
    
    // Set update interval
    void setUpdateInterval(int interval);
    
    // Enable or disable progress display
    void enable(bool enable);
    
    // Check if monitoring is active
    bool isActive() const { return active; }
    
    // Get remaining time estimate
    std::chrono::seconds getEstimatedTimeRemaining() const;

private:
    bool enabled;
    bool active;
    int updateInterval;
    int totalSteps;
    int currentStep;
    std::string currentOperation;
    std::chrono::steady_clock::time_point startTime;
    
    // Display progress bar
    void displayProgressBar() const;
    
    // Clear progress line
    void clearProgressLine() const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_PROGRESS_MONITOR_HPP
