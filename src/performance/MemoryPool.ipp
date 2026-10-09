// MemoryPoolStatisticsCollector implementation

inline MemoryPoolStatisticsCollector::MemoryPoolStatisticsCollector()
    : maxHistorySamples_(1000), peakUtilization_(0.0f), averageUtilization_(0.0f),
      currentAverageUtil_(0.0f), currentPressureLevel_(MemoryPressureLevel::Normal),
      pressureChanged_(false), pressureCallback_(nullptr) {}

inline MemoryPoolStatisticsCollector::~MemoryPoolStatisticsCollector() = default;

inline void MemoryPoolStatisticsCollector::update(const MemoryStats& stats) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Add to history
    usageHistory_.push_back({currentTime, stats.utilizationRatio, stats.allocatedCount});
    
    // Keep history size bounded
    if (usageHistory_.size() > maxHistorySamples_) {
        usageHistory_.erase(usageHistory_.begin());
    }
    
    // Update peak and average utilization
    if (stats.utilizationRatio > peakUtilization_) {
        peakUtilization_ = stats.utilizationRatio;
    }
    
    // Calculate running average
    float totalUtil = 0.0f;
    size_t count = 0;
    for (const auto& sample : usageHistory_) {
        totalUtil += sample.utilization;
        count++;
    }
    averageUtilization_ = (count > 0) ? (totalUtil / count) : 0.0f;
    
    // Check for memory pressure level changes
    MemoryPressureLevel newPressureLevel = getMemoryPressure(stats.utilizationRatio);
    if (newPressureLevel != currentPressureLevel_) {
        currentPressureLevel_ = newPressureLevel;
        pressureChanged_ = true;
        
        // Notify callback
        if (pressureCallback_) {
            pressureCallback_(currentPressureLevel_, stats.utilizationRatio);
        }
    }
}

inline MemoryPressureLevel MemoryPoolStatisticsCollector::getMemoryPressure(float utilizationRatio) const {
    if (utilizationRatio > 0.95f) {
        return MemoryPressureLevel::Exhausted;
    } else if (utilizationRatio > 0.90f) {
        return MemoryPressureLevel::Critical;
    } else if (utilizationRatio > 0.75f) {
        return MemoryPressureLevel::High;
    } else if (utilizationRatio > 0.50f) {
        return MemoryPressureLevel::Moderate;
    } else {
        return MemoryPressureLevel::Normal;
    }
}

inline float MemoryPoolStatisticsCollector::getAverageUtilization(uint32_t timeWindowMs) const {
    if (usageHistory_.size() < 2) return 0.0f;
    
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    float totalUtil = 0.0f;
    size_t count = 0;
    
    for (const auto& sample : usageHistory_) {
        if (currentTime - sample.timestamp <= timeWindowMs) {
            totalUtil += sample.utilization;
            count++;
        }
    }
    
    return (count > 0) ? (totalUtil / count) : 0.0f;
}

inline bool MemoryPoolStatisticsCollector::shouldResize(uint64_t currentAllocationCount,
                                                         uint32_t checkInterval) const {
    if (usageHistory_.size() < 2) return false;
    
    // Check if we've allocated enough objects to make a decision
    if (currentAllocationCount < checkInterval) return false;
    
    // Check if utilization is trending away from target
    float currentUtil = usageHistory_.back().utilization;
    
    // If utilization is significantly different from average, consider resize
    if (std::abs(currentUtil - averageUtilization_) > 0.1f) {
        return true;
    }
    
    return false;
}

inline size_t MemoryPoolStatisticsCollector::calculateOptimalBlockSize(size_t currentBlockSize,
                                                                        float currentUtilization) const {
    // Adaptive block size calculation based on utilization
    if (currentUtilization < 0.5f) {
        // Underutilized - increase block size
        return std::min(currentBlockSize * 2, size_t(8192));
    } else if (currentUtilization > 0.8f) {
        // Overutilized - decrease block size
        return std::max(currentBlockSize / 2, size_t(256));
    } else {
        // Good utilization - maintain current size
        return currentBlockSize;
    }
}

inline void MemoryPoolStatisticsCollector::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    usageHistory_.clear();
    peakUtilization_ = 0.0f;
    averageUtilization_ = 0.0f;
    currentAverageUtil_ = 0.0f;
    currentPressureLevel_ = MemoryPressureLevel::Normal;
    pressureChanged_ = false;
}