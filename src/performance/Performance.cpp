#include "Performance.hpp"

namespace nlm {

class Performance {
public:
    Performance();
    ~Performance();
    
    // Start performance monitoring
    bool startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Get current metrics
    double getAverageFPS() const;
    double getAverageMemoryUsage() const;
    double getPeakMemoryUsage() const;
    size_t getTotalOperations() const;
    
    // Get performance counters
    uint64_t getMemoryAllocations() const;
    uint64_t getMemoryDeallocations() const;
    uint64_t getCacheHits() const;
    uint64_t getCacheMisses() const;
    
    // Reset metrics
    void resetMetrics();
    
    // Generate performance report
    std::string generateReport() const;
    
    // Check if monitoring is active
    bool isMonitoring() const;
    
    // Update performance monitoring (should be called periodically)
    void update(double deltaTime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm