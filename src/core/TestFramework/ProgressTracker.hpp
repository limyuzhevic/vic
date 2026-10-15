#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

class ProgressTracker {
public:
    struct ProgressEntry {
        std::string testName;
        std::string category;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        float progress; // 0.0 to 1.0
        bool completed;
        bool success;
        std::string statusMessage;
        std::string errorMessage;
        size_t outputLines;
        
        // Helper methods
        std::chrono::milliseconds getDuration() const {
            if (endTime.time_since_epoch().count() == 0) {
                return std::chrono::milliseconds(0);
            }
            return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        }
        
        std::string getFormattedDuration() const {
            auto duration = getDuration();
            auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
            duration -= hours;
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
            duration -= minutes;
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
            std::stringstream ss;
            if (hours.count() > 0) ss << hours.count() << "h ";
            if (minutes.count() > 0 || hours.count() > 0) ss << minutes.count() % 60 << "m ";
            ss << seconds.count() % 60 << "s";
            return ss.str();
        }
    };
    
    ProgressTracker();
    
    // Start tracking a test
    void startTest(const std::string& testName, const std::string& category = "");
    
    // Update progress
    void updateProgress(const std::string& testName, float progress, const std::string& message = "");
    
    // End tracking a test
    void endTest(const std::string& testName, bool success, const std::string& message = "");
    void endTest(const std::string& testName, bool success, const std::string& message, 
                 const std::string& error);
    
    // Get current progress
    const ProgressEntry* getCurrentTest() const;
    const std::vector<ProgressEntry>& getAllProgress() const;
    
    // Statistics and reporting
    size_t getTotalTests() const;
    size_t getCompletedTests() const;
    size_t getFailedTests() const;
    size_t getSkippedTests() const;
    float getOverallProgress() const;
    
    // Display methods
    void displayProgress(bool forceUpdate = false);
    void displayFinalSummary();
    std::string getProgressSummary() const;
    
    // Configuration
    void setUpdateInterval(int interval); // in milliseconds
    void setShowDetails(bool show);
    void setColorOutput(bool enable);
    
    // Clearing
    void clear();
    void reset();
    
private:
    std::vector<ProgressEntry> progress_;
    mutable std::mutex mutex_;
    std::chrono::steady_clock::time_point lastUpdate_;
    int updateInterval_;
    bool showDetails_;
    bool colorOutput_;
    
    // Display helpers
    void displayProgressInternal() const;
    std::string formatTestName(const std::string& name, bool active = false) const;
    std::string formatProgress(float progress) const;
    std::string getStatusColor(bool success, bool active) const;
};

} // namespace nlm
