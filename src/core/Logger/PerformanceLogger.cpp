// Implementation of performance logging utilities
namespace nlm {

PerformanceLogger::PerformanceLogger(const std::string& operation, const std::string& category)
    : operation_(operation), category_(category), threshold_(5.0) {
    startTime_ = std::chrono::high_resolution_clock::now();
}

PerformanceLogger::~PerformanceLogger() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime_).count();
    
    // Log performance metrics
    Logger::getGlobal().info("Performance: " + operation_ + " took " + std::to_string(duration) + "s",
                            category, {{ "operation", operation_ }, { "duration_ms", std::to_string(duration * 1000) }});
    
    if (duration > threshold_) {
        Logger::getGlobal().warning("Performance: " + operation_ + " exceeded threshold of " + std::to_string(threshold_) + "s",
                                   category, {{ "operation", operation_ }, { "duration_ms", std::to_string(duration * 1000) }});
    }
}

void PerformanceLogger::setThreshold(double threshold) {
    threshold_ = threshold;
}

double PerformanceLogger::getElapsedTime() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime_).count();
}

bool PerformanceLogger::exceededThreshold() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime_).count();
    return duration > threshold_;
}

} // namespace nlm