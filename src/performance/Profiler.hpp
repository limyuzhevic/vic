#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

class Profiler;

/**
     * @brief High-resolution timer for performance measurement.
     * 
     * Timer class provides accurate timing measurements with microsecond precision
     * using std::chrono. It supports starting, stopping, and querying elapsed time
     * for performance analysis of specific operations within NLM.
     */
class Timer {
public:
    Timer();
    
    /**
     * @brief Starts the timer.
     * 
     * Records the current time point as the start of the measurement.
     */
    void start();
    
    /**
     * @brief Stops the timer.
     * 
     * Calculates and returns the elapsed time since the timer was started.
     * @return double Elapsed time in seconds since start()
     */
    double stop();
    
    /**
     * @brief Gets the elapsed time since the timer was started.
     * 
     * @return double Current elapsed time in seconds
     */
    double elapsed() const;
    
    /**
     * @brief Checks if the timer is currently running.
     * 
     * @return bool True if timer is started and not stopped
     */
    bool isRunning() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Implementation details for Timer class.
 * 
 * This private implementation class encapsulates the timer functionality,
 * hiding the details of std::chrono usage and providing a clean interface.
 */
class Timer::Impl {
private:
    Impl();
    friend class Timer;
    
    void start();
    double stop();
    double elapsed() const;
    bool isRunning() const;
    
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point stopTime;
    bool running;
};

/**
 * @brief Constructor for Timer.
 * 
 * Initializes a new timer in a stopped state.
 */
inline Timer::Timer() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Start the timer.
 * 
 * Records the current time point as the start of the measurement.
 */
inline void Timer::start() { pImpl->start(); }

/**
 * @brief Stop the timer.
 * 
 * Calculates and returns the elapsed time since the timer was started.
 * @return double Elapsed time in seconds since start()
 */
inline double Timer::stop() { return pImpl->stop(); }

/**
 * @brief Get the elapsed time since the timer was started.
 * 
 * @return double Current elapsed time in seconds
 */
inline double Timer::elapsed() const { return pImpl->elapsed(); }

/**
 * @brief Check if the timer is currently running.
 * 
 * @return bool True if timer is started and not stopped
 */
inline bool Timer::isRunning() const { return pImpl->isRunning(); }

/**
 * @brief Performance profiler for tracking operation timing.
 * 
 * Profiler class provides a comprehensive performance monitoring system that
 * tracks timing statistics for multiple operations simultaneously. It supports
 * concurrent profiling, statistical analysis, and formatted reporting of timing
 * data. The profiler is a singleton accessed via getInstance().
 * 
 * Key features:
 * - High-resolution timing with microsecond precision
 * - Thread-safe operations for concurrent profiling
 * - Statistical analysis (min, max, average, total)
 * - Multiple simultaneous profiles
 * - Formatted summary output
 * - Python bindings for integration
 */
class Profiler {
public:
    /**
     * @brief Profile data structure containing timing statistics.
     * 
     * Stores timing information for a specific operation including call count,
     * total time, and statistical measures (min, max, average).
     */
    struct ProfileData {
        std::string name;            ///< Operation name
        size_t callCount = 0;        ///< Number of calls to this operation
        double totalTime = 0.0;      ///< Total time spent in this operation
        double minTime = 0.0;        ///< Minimum time for a single call
        double maxTime = 0.0;        ///< Maximum time for a single call
        double avgTime = 0.0;        ///< Average time per call
        
        ProfileData(const std::string& operationName) : name(operationName) {}
        
        /**
         * @brief Add a timing sample to the profile statistics.
         * 
         * Updates the statistics based on a new timing measurement.
         * @param elapsed Time value for the sample in seconds
         */
        void addSample(double elapsed);
        
        /**
         * @brief Get formatted string representation of profile data.
         * 
         * @return std::string Formatted profile data string
         */
        std::string toString() const;
    };
    
    /**
     * @brief Get the singleton profiler instance.
     * 
     * @return Profiler& Reference to the global profiler instance
     */
    static Profiler& getInstance();
    
    /**
     * @brief Start timing for a named operation.
     * 
     * Begins timing for a specific operation. If an operation with the same
     * name is already being timed, it will be replaced.
     * @param name Name of the operation to profile
     */
    void start(const std::string& name);
    
    /**
     * @brief Stop timing for a named operation.
     * 
     * Ends timing for the specified operation and updates the profile statistics.
     * @param name Name of the operation to stop timing
     */
    void stop(const std::string& name);
    
    /**
     * @brief Get profile data for a specific operation.
     * 
     * @param name Name of the operation
     * @return const ProfileData& Profile data (empty if operation not found)
     */
    const ProfileData& getProfile(const std::string& name) const;
    
    /**
     * @brief Get all profile data as a vector.
     * 
     * @return std::vector<ProfileData> All profile data
     */
    std::vector<ProfileData> getAllProfiles() const;
    
    /**
     * @brief Get total accumulated time across all profiles.
     * 
     * @return double Total time in seconds
     */
    double getTotalTime() const;
    
    /**
     * @brief Clear all profile data.
     * 
     * Resets the profiler to its initial state, removing all recorded data.
     */
    void clear();
    
    /**
     * @brief Print formatted summary of all profiles.
     * 
     * Outputs a detailed table showing timing statistics for all profiled
     * operations with proper formatting and alignment.
     */
    void printSummary() const;
    
    /**
     * @brief Get formatted summary as a string.
     * 
     * @return std::string Formatted summary string
     */
    std::string getSummaryString() const;
    
private:
    Profiler() = default;
    ~Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    
    /**
     * @brief Timer objects for currently running operations.
     * 
     * Maps operation names to their corresponding Timer objects.
     */
    std::unordered_map<std::string, Timer> timers;
    
    /**
     * @brief Profile data for completed operations.
     * 
     * Maps operation names to their accumulated profile statistics.
     */
    std::unordered_map<std::string, ProfileData> profiles;
};

/**
 * @brief Convenience function for timing Python code blocks.
 * 
 * Utility function that simplifies performance timing of Python code blocks
 * in NLM integration scenarios. Starts timing, executes the lambda function,
 * stops timing, and returns the result along with the elapsed time.
 * 
 * @tparam F Type of callable (lambda or function)
 * @param func Callable to execute and time
 * @param name Name of the operation for profiling
 * @return std::pair<T, double> Pair of (result, elapsed_time)
 */
template<typename F>
auto timePython(F&& func, const std::string& name) {
    Profiler::getInstance().start(name);
    auto result = func();
    double elapsed = Profiler::getInstance().getProfile(name).totalTime;
    Profiler::getInstance().stop(name);
    return std::make_pair(result, elapsed);
}

} // namespace nlm
