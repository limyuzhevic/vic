#pragma once

#include "Types.hpp"
#include <chrono>
#include <format>
#include <optional>
#include <thread>
#include <ctime>
#include <ratio>

namespace nlm {

// Enhanced simulation clock with C++20 chrono features
// Provides deterministic time tracking separate from wall-clock time with improved precision

class SimulationClock {
public:
    // Create with specific timestep using C++20 time units
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    ~SimulationClock();
    
    // Disable copying, enable moving with noexcept
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    // Time operations with C++20 chrono integration
    void reset();
    void advance();
    
    // Step counter with validation
    SimulationStep getStep() const;
    void setStep(SimulationStep step);
    
    // Current simulation time using std::chrono for better precision
    Timestamp getTime() const;
    void setTime(Timestamp time);
    
    // Timestep with better type safety
    TimestepDuration getTimestep() const;
    void setTimestep(TimestepDuration dt);
    
    // Convenience queries using modern C++20 algorithms
    bool isFirstStep() const;
    bool isLastStep(SimulationStep totalSteps) const;
    
    // Step to time conversion using chrono for accuracy
    Timestamp stepToTime(SimulationStep step) const;
    SimulationStep timeToStep(Timestamp time) const;
    
    // Advance by multiple steps using range algorithms
    void advanceBy(SimulationStep steps);
    
    // Get time at a future step from now with C++20 calculations
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock comparison using std::chrono::system_clock (C++20 improved)
    double getRealTimeElapsed() const;
    void setRealTimeStart();
    
    // Enhanced time management with C++20 features
    
    // Get current wall-clock time as Timestamp
    static Timestamp getWallClockTime();
    
    // Convert between different time representations
    static Timestamp to_timestamp(std::chrono::system_clock::time_point tp);
    static std::chrono::system_clock::time_point from_timestamp(Timestamp ts);
    
    // Get simulation time with high precision using std::chrono::high_resolution_clock
    static Timestamp get_high_precision_time();
    
    // Time utilities using C++20 chrono literals
    static constexpr TimestepDuration operator" _ms(unsigned long long ms) {
        return TimestepDuration(ms / 1000.0);
    }
    
    static constexpr TimestepDuration operator" _s(unsigned long long s) {
        return TimestepDuration(s);
    }
    
    static constexpr TimestepDuration operator" _us(unsigned long long us) {
        return TimestepDuration(us / 1000000.0);
    }
    
    // Enhanced time validation using chrono comparisons
    static bool is_time_valid(Timestamp time) {
        return time >= 0.0 && std::isfinite(time);
    }
    
    static bool is_step_valid(SimulationStep step) {
        return step <= std::numeric_limits<SimulationStep>::max() / 2;
    }
    
    // Time formatting using std::format (C++20)
    static std::string format_time(Timestamp time, const char* format = "%H:%M:%S") {
        auto tp = from_timestamp(time);
        auto in_time_t = std::chrono::system_clock::to_time_t(tp);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), format);
        return ss.str();
    }
    
    // Sleep function using std::this_thread::sleep_for (C++20)
    static void sleep_for(std::chrono::duration<double> duration) {
        std::this_thread::sleep_for(duration);
    }
    
    static void sleep_until(std::chrono::system_clock::time_point time_point) {
        std::this_thread::sleep_until(time_point);
    }
    
    // Enhanced time calculations using C++20 chrono features
    static double calculate_elapsed(std::chrono::system_clock::time_point start,
                                   std::chrono::system_clock::time_point end) {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        return duration.count();
    }
    
    static Timestamp calculate_future_time(Timestamp current_time, TimestepDuration timestep, SimulationStep steps) {
        return current_time + timestep * static_cast<Timestamp>(steps);
    }
    
    // Time conversion utilities
    static constexpr Timestamp operator*(SimulationStep steps, TimestepDuration timestep) {
        return static_cast<Timestamp>(steps) * timestep;
    }
    
    static constexpr SimulationStep operator/(Timestamp time, TimestepDuration timestep) {
        return static_cast<SimulationStep>(time / timestep);
    }
    
    // Enhanced C++20 iterators for time sequences
    class time_iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Timestamp;
        using difference_type = std::ptrdiff_t;
        using pointer = const Timestamp*;
        using reference = const Timestamp&;
        
        time_iterator(Timestamp start, TimestepDuration timestep, SimulationStep step_count)
            : current_time(start), timestep(timestep), remaining_steps(step_count), current_step(0) {}
        
        reference operator*() const { return current_time; }
        pointer operator->() const { return &current_time; }
        
        time_iterator& operator++() {
            if (current_step < remaining_steps) {
                current_time += timestep;
                current_step++;
            }
            return *this;
        }
        
        time_iterator operator++(int) {
            time_iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const time_iterator& other) const {
            return current_step == other.current_step;
        }
        
        bool operator!=(const time_iterator& other) const {
            return !(*this == other);
        }
    
    private:
        Timestamp current_time;
        TimestepDuration timestep;
        SimulationStep remaining_steps;
        SimulationStep current_step;
    };
    
    // Create time sequence using C++20 ranges and iterators
    static auto create_time_sequence(Timestamp start, TimestepDuration timestep, SimulationStep count) {
        return std::views::iota(0ULL, count) |
               std::views::transform([start, timestep](SimulationStep step) {
                   return start + timestep * static_cast<Timestamp>(step);
               });
    }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced chrono type aliases for better C++20 compatibility
typedef std::chrono::system_clock system_clock;
typedef std::chrono::high_resolution_clock high_resolution_clock;
typedef std::chrono::steady_clock steady_clock;

// C++20 time unit conversions
typedef std::chrono::duration<Timestamp, std::ratio<1, 1>> seconds_t;
typedef std::chrono::duration<Timestamp, std::ratio<1, 1000>> milliseconds_t;
typedef std::chrono::duration<Timestamp, std::ratio<1, 1000000>> microseconds_t;
typedef std::chrono::duration<Timestamp, std::ratio<1, 60>> minutes_t;
typedef std::chrono::duration<Timestamp, std::ratio<1, 3600>> hours_t;

// Time point type aliases
typedef std::chrono::time_point<system_clock, seconds_t> system_time_point;
typedef std::chrono::time_point<high_resolution_clock, microseconds_t> high_resolution_time_point;
typedef std::chrono::time_point<steady_clock, milliseconds_t> steady_time_point;

// Utility functions for enhanced C++20 time management
namespace chrono {
    // High-precision time utilities
    inline Timestamp to_timestamp(high_resolution_time_point tp) {
        auto duration = tp.time_since_epoch();
        return std::chrono::duration_cast<seconds_t>(duration).count();
    }
    
    inline high_resolution_time_point from_timestamp(Timestamp ts) {
        return high_resolution_time_point(std::chrono::duration_cast<microseconds_t>(seconds_t(ts)));
    }
    
    // Time formatting with C++20 format
    inline std::string format_time_point(high_resolution_time_point tp) {
        auto time = std::chrono::system_clock::to_time_t(system_clock::now());
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch() % std::chrono::seconds(1)).count();
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S") 
           << '.' << std::setfill('0') << std::setw(3) << ms;
        return ss.str();
    }
    
    // Time measurement utilities
    class timer {
    public:
        timer() : start_(high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto end = high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
                end - start_);
            return duration.count();
        }
        
        void reset() {
            start_ = high_resolution_clock::now();
        }
    
    private:
        high_resolution_clock::time_point start_;
    };
    
    // Sleep utilities with C++20
    inline void sleep_for(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }
    
    inline void sleep_until(high_resolution_time_point time_point) {
        std::this_thread::sleep_until(time_point);
    }
    
    // Time comparison utilities
    inline bool is_time_point_valid(high_resolution_time_point tp) {
        return tp.time_since_epoch().count() > 0;
    }
    
    // Time conversion utilities
    inline high_resolution_time_point add_time(high_resolution_time_point tp, double seconds) {
        return tp + std::chrono::duration<double>(seconds);
    }
    
    inline double time_difference(high_resolution_time_point t1, high_resolution_time_point t2) {
        auto duration = t2 - t1;
        return std::chrono::duration<double>(duration).count();
    }
}

} // namespace nlm
