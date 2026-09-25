// Common utility functions and classes for neural simulation framework

#pragma once

namespace nlm {

// Utility class for managing spike history with automatic truncation
class SpikeHistoryManager {
public:
    // Generic spike history management - records a timestamp and enforces max size
    template<typename Container>
    static void recordAndTruncate(Container& history, size_t maxSize, const typename Container::value_type& timestamp) {
        history.push_back(timestamp);
        if (history.size() > maxSize) {
            history.erase(history.begin());
        }
    }
    
    // Overload specifically for std::vector for clarity
    template<typename T, typename Allocator>
    static void recordAndTruncate(std::vector<T, Allocator>& history, size_t maxSize, const T& timestamp) {
        history.push_back(timestamp);
        if (history.size() > maxSize) {
            history.erase(history.begin());
        }
    }
};

// Utility class for weight clamping
class WeightClamper {
public:
    // Generic clamp function for any type
    template<typename T>
    static T clamp(T value, T min, T max) {
        return std::clamp(value, min, max);
    }
};

// Helper for clearing spike history
template<typename Container>
void clearHistory(Container& history) {
    history.clear();
}

// Common initialization helper
struct InitHelper {
    template<typename Container>
    static void initializeContainer(Container& container, size_t expectedSize) {
        container.reserve(expectedSize);
    }
};

} // namespace nlm
