// PerformanceError implementation
namespace nlm {

PerformanceError::PerformanceError(const std::string& message,
                                   PerformanceErrorType type,
                                   const std::string& metric,
                                   const std::string& suggestion)
    : NLMError("Performance Error: " + message, ErrorSeverity::Warning, ErrorCategory::Performance),
      performanceErrorType_(type),
      metric_(metric),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string PerformanceError::getRecoveryHint() const {
    switch (performanceErrorType_) {
        case PerformanceErrorType::MemoryUsageExceeded:
            return "Reduce memory usage or increase memory allocation. Use memory optimization techniques.";
        case PerformanceErrorType::CPUUsageExceeded:
            return "Reduce CPU usage or optimize algorithm. Check CPU-intensive operations.";
        case PerformanceErrorType::TimeLimitExceeded:
            return "Optimize performance or increase timeout value. Check performance bottlenecks.";
        case PerformanceErrorType::ThroughputBelowMinimum:
            return "Optimize performance or reduce workload. Check throughput bottlenecks.";
        case PerformanceErrorType::LatencyExceeded:
            return "Reduce latency or increase timeout value. Check network latency or algorithm performance.";
        case PerformanceErrorType::ContextSwitchRateHigh:
            return "Reduce context switching or optimize thread usage. Check concurrency overhead.";
        case PerformanceErrorType::CacheMissRateHigh:
            return "Optimize caching or improve data locality. Check cache performance.";
        case PerformanceErrorType::ResourceContention:
            return "Reduce resource contention or increase resource availability. Check resource usage.";
        case PerformanceErrorType::SlowdownDetected:
            return "Optimize performance or investigate slowdown. Check performance bottlenecks.";
        case PerformanceErrorType::DegradationDetected:
            return "Optimize performance or investigate degradation. Check performance degradation causes.";
        default:
            return "Optimize performance or check resource usage. Use performance monitoring tools.";
    }
}

} // namespace nlm