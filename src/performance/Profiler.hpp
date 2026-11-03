#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * CPU profiler for detailed performance analysis
 * 
 * Tracks function call times, stack traces, and identifies performance
 * bottlenecks through comprehensive CPU profiling.
 */

class Profiler {
public:
    struct ProfileData {
        std::string functionName;
        std::string fileName;
        int lineNumber;
        std::chrono::duration<double> totalTime;
        std::chrono::duration<double> selfTime;
        uint64_t callCount;
        double averageTime;
        double percentage;
        
        ProfileData() 
            : lineNumber(0), callCount(0), averageTime(0.0), percentage(0.0) {}
        
        ProfileData(const std::string& name, const std::string& file, int line)
            : functionName(name), fileName(file), lineNumber(line),
              totalTime(0.0), selfTime(0.0), callCount(0), averageTime(0.0), percentage(0.0) {}
    };

    enum class ProfileMode {
        CALL_GRAPH,
        TIMELINE,
        HEAP,
        THREAD,
        MEMORY
    };

    static Profiler& getInstance() {
        static Profiler instance;
        return instance;
    }

    void startProfile(const std::string& functionName, const std::string& fileName = "", int lineNumber = 0);
    void endProfile(const std::string& functionName);
    void startThreadProfile(const std::string& threadName);
    void endThreadProfile(const std::string& threadName);
    
    void setProfileMode(ProfileMode mode) { profileMode_ = mode; }
    ProfileMode getProfileMode() const { return profileMode_; }
    
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool isEnabled() const { return enabled_; }
    
    void reset();
    
    const std::vector<ProfileData>& getProfileData() const { return profileData_; }
    
    double getTotalProfileTime() const { return totalProfileTime_; }
    
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    
    void exportProfile(const std::string& filename);
    void exportCallGraph(const std::string& filename);
    void exportTimeline(const std::string& filename);
    
    std::string generateReport() const;
    
    void profileFunction(const std::string& functionName, std::function<void()> func);

private:
    Profiler()
        : enabled_(false)
        , profileMode_(ProfileMode::CALL_GRAPH)
        , samplingInterval_(0.001)  // 1ms
        , totalProfileTime_(0.0)
        , currentDepth_(0)
    {
    }

    ~Profiler() {
        // Ensure all profiles are closed
        for (auto& entry : activeProfiles_) {
            // Log that profile was left unclosed
        }
    }

    struct ProfileEntry {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::duration<double> accumulatedTime;
        uint64_t callCount;
        std::string functionName;
        std::string fileName;
        int lineNumber;
        std::vector<ProfileEntry> children;
        
        ProfileEntry(const std::string& name, const std::string& file, int line)
            : startTime(std::chrono::high_resolution_clock::now())
            , accumulatedTime(0.0)
            , callCount(0)
            , functionName(name)
            , fileName(file)
            , lineNumber(line) {}
    };

    void pushProfile(const ProfileEntry& entry);
    void popProfile();
    ProfileEntry& getCurrentProfile();
    
    std::chrono::duration<double> calculateProfileTime(const ProfileEntry& entry) const;
    
    void aggregateProfileData();
    void calculatePercentages();

private:
    bool enabled_;
    ProfileMode profileMode_;
    double samplingInterval_;
    double totalProfileTime_;
    int currentDepth_;
    
    std::vector<ProfileData> profileData_;
    std::vector<ProfileEntry> profileStack_;
    std::unordered_map<std::string, ProfileEntry> threadProfiles_;
    mutable std::mutex profileMutex_;
};

} // namespace nlm
