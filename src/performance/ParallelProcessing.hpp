// Parallel processing system for NLM
// Enables multi-threaded neural simulation for improved performance

#pragma once

#include <thread>
#include <vector>

namespace nlm {

/**
 * Parallel processing system for neural simulation
 * Manages multiple threads for neural computation
 */
class ParallelProcessing {
public:
    ParallelProcessing();
    ~ParallelProcessing();
    
    void setThreadCount(size_t count);
    size_t getThreadCount() const;
    
    template<typename Func>
    void parallelFor(size_t begin, size_t end, Func func);
    
    template<typename Func>
    void parallelMap(const std::vector<float>& input, std::vector<float>& output, Func func);
    
    bool isEnabled() const;
    void enable(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
