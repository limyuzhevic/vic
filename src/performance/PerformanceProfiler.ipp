// PerformanceProfiler implementation

inline PerformanceStatisticsAnalyzer::PerformanceStatisticsAnalyzer()
    : maxMeasurements_(100000) {}

inline PerformanceStatisticsAnalyzer::~PerformanceStatisticsAnalyzer() = default;

inline void PerformanceStatisticsAnalyzer::addMeasurement(const PerformanceMeasurement& measurement) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    measurements_.push_back(measurement);
    
    // Keep measurements size bounded
    if (measurements_.size() > maxMeasurements_) {
        measurements_.erase(measurements_.begin());
    }
}

inline PerformanceProfile PerformanceStatisticsAnalyzer::calculateProfile(
    PerformanceCategory category, const std::string& operation) const {
    std::vector<PerformanceMeasurement> filteredMeasurements;
    
    for (const auto& measurement : measurements_) {
        if (operation.empty() || measurement.operation == operation) {
            filteredMeasurements.push_back(measurement);
        }
    }
    
    if (filteredMeasurements.empty()) {
        return PerformanceProfile();
    }
    
    PerformanceProfile profile;
    profile.totalSamples = filteredMeasurements.size();
    
    double totalTime = 0.0;
    size_t totalObjects = 0;
    size_t peakMemory = 0;
    float peakMemoryUtil = 0.0f;
    
    for (const auto& measurement : filteredMeasurements) {
        totalTime += measurement.executionTime;
        totalObjects += measurement.objectsProcessed;
        if (measurement.memoryUsage > peakMemory) {
            peakMemory = measurement.memoryUsage;
        }
        if (measurement.memoryUtilization > peakMemoryUtil) {
            peakMemoryUtil = measurement.memoryUtilization;
        }
    }
    
    profile.totalTime = totalTime;
    profile.totalOperations = filteredMeasurements.size();
    profile.averageExecutionTime = totalTime / filteredMeasurements.size();
    profile.medianExecutionTime = calculateMedianExecutionTime(filteredMeasurements);
    profile.p95ExecutionTime = calculatePercentileExecutionTime(filteredMeasurements, 95.0);
    profile.p99ExecutionTime = calculatePercentileExecutionTime(filteredMeasurements, 99.0);
    profile.totalObjectsProcessed = totalObjects;
    profile.peakMemoryUsage = peakMemory;
    profile.peakMemoryUtilization = peakMemoryUtil;
    
    return profile;
}

inline std::vector<PerformanceMeasurement> PerformanceStatisticsAnalyzer::getMeasurements(
    PerformanceCategory category, const std::string& operation) const {
    return filterByCategoryAndOperation(measurements_, category, operation);
}

inline double PerformanceStatisticsAnalyzer::calculateMedianExecutionTime(
    const std::vector<PerformanceMeasurement>& measurements) {
    if (measurements.empty()) return 0.0;
    
    std::vector<double> times;
    for (const auto& measurement : measurements) {
        times.push_back(measurement.executionTime);
    }
    
    std::sort(times.begin(), times.end());
    size_t middle = times.size() / 2;
    
    if (times.size() % 2 == 0) {
        return (times[middle - 1] + times[middle]) / 2.0;
    } else {
        return times[middle];
    }
}

inline double PerformanceStatisticsAnalyzer::calculatePercentileExecutionTime(
    const std::vector<PerformanceMeasurement>& measurements, double percentile) {
    if (measurements.empty()) return 0.0;
    
    std::vector<double> times;
    for (const auto& measurement : measurements) {
        times.push_back(measurement.executionTime);
    }
    
    std::sort(times.begin(), times.end());
    size_t index = static_cast<size_t>((percentile / 100.0) * times.size());
    
    if (index >= times.size()) {
        index = times.size() - 1;
    }
    
    return times[index];
}

inline std::vector<PerformanceMeasurement> PerformanceStatisticsAnalyzer::filterByCategory(
    const std::vector<PerformanceMeasurement>& measurements, PerformanceCategory category) {
    std::vector<PerformanceMeasurement> filtered;
    
    for (const auto& measurement : measurements) {
        if (category == PerformanceCategory::All) {
            filtered.push_back(measurement);
        } else {
            // Simple string-based category matching (can be enhanced)
            if (category == PerformanceCategory::Neuron && 
                (measurement.category == "neuron" || measurement.category == "Neuron")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::Synapse &&
                      (measurement.category == "synapse" || measurement.category == "Synapse")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::Memory &&
                      (measurement.category == "memory" || measurement.category == "Memory")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::Plasticity &&
                      (measurement.category == "plasticity" || measurement.category == "Plasticity")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::Integration &&
                      (measurement.category == "integration" || measurement.category == "Integration")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::Connectivity &&
                      (measurement.category == "connectivity" || measurement.category == "Connectivity")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::Simulation &&
                      (measurement.category == "simulation" || measurement.category == "Simulation")) {
                filtered.push_back(measurement);
            } else if (category == PerformanceCategory::I_O &&
                      (measurement.category == "io" || measurement.category == "IO" || 
                       measurement.category == "input/output" || measurement.category == "Input/Output")) {
                filtered.push_back(measurement);
            }
        }
    }
    
    return filtered;
}

inline std::vector<PerformanceMeasurement> PerformanceStatisticsAnalyzer::filterByCategoryAndOperation(
    const std::vector<PerformanceMeasurement>& measurements,
    PerformanceCategory category, const std::string& operation) {
    std::vector<PerformanceMeasurement> filtered;
    
    for (const auto& measurement : measurements) {
        if (category == PerformanceCategory::All || 
            filterByCategory(std::vector<PerformanceMeasurement>{measurement}, category).size() > 0) {
            if (operation.empty() || measurement.operation == operation) {
                filtered.push_back(measurement);
            }
        }
    }
    
    return filtered;
}

// PerformanceProfiler implementation

inline PerformanceProfiler::PerformanceProfiler()
    : nextMeasurementId_(1), monitoringActive_(false), monitoringIntervalMs_(1000) {}

inline PerformanceProfiler::~PerformanceProfiler() {
    stopMonitoring();
}

inline uint64_t PerformanceProfiler::startMeasurement(PerformanceCategory category,
                                                        const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    
    uint64_t measurementId = nextMeasurementId_++;
    
    activeMeasurements_[measurementId] = MeasurementContext(measurementId, currentTime, category, operation);
    
    // Record measurement start
    if (performanceCallback_) {
        PerformanceMeasurement startMeasurement(
            currentTime, getCategoryString(category), operation, 0.0, 0, 0, 0.0f, 0);
        performanceCallback_(startMeasurement);
    }
    
    return measurementId;
}

inline void PerformanceProfiler::endMeasurement(uint64_t measurementId, size_t objectsProcessed,
                                                 size_t memoryUsage, float memoryUtilization) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeMeasurements_.find(measurementId);
    if (it == activeMeasurements_.end()) {
        return;
    }
    
    auto& context = it->second;
    
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t endTime = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    
    double executionTime = static_cast<double>(endTime - context.startTime);
    
    // Record measurement end
    if (performanceCallback_) {
        PerformanceMeasurement endMeasurement(
            endTime, getCategoryString(context.category), context.operation,
            executionTime, objectsProcessed, memoryUsage, memoryUtilization, 0);
        performanceCallback_(endMeasurement);
    }
    
    // Remove from active measurements
    activeMeasurements_.erase(it);
}

inline void PerformanceProfiler::recordMeasurement(PerformanceCategory category,
                                                    const std::string& operation,
                                                    double executionTime, size_t objectsProcessed,
                                                    size_t memoryUsage, float memoryUtilization) {
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    
    PerformanceMeasurement measurement(
        timestamp, getCategoryString(category), operation,
        executionTime, objectsProcessed, memoryUsage, memoryUtilization, 0);
    
    statsAnalyzer_.addMeasurement(measurement);
    
    if (performanceCallback_) {
        performanceCallback_(measurement);
    }
}

inline PerformanceProfile PerformanceProfiler::getProfile(PerformanceCategory category,
                                                            const std::string& operation) const {
    return statsAnalyzer_.calculateProfile(category, operation);
}

inline bool PerformanceProfiler::isSlowOperation(PerformanceCategory category,
                                                  const std::string& operation) const {
    PerformanceProfile profile = getProfile(category, operation);
    return profile.averageExecutionTime > thresholds_.slowOperationThreshold;
}

inline bool PerformanceProfiler::isHighMemoryUsage(PerformanceCategory category,
                                                    const std::string& operation) const {
    PerformanceProfile profile = getProfile(category, operation);
    return profile.peakMemoryUtilization > thresholds_.highMemoryThreshold;
}

inline PerformanceTrend PerformanceProfiler::getPerformanceTrend(PerformanceCategory category,
                                                                   const std::string& operation) const {
    PerformanceTrend trend;
    
    std::vector<PerformanceMeasurement> measurements = statsAnalyzer_.getMeasurements(category, operation);
    if (measurements.size() < 2) return trend;
    
    // Calculate time trend
    std::vector<double> times;
    for (const auto& measurement : measurements) {
        times.push_back(measurement.executionTime);
    }
    
    std::sort(times.begin(), times.end());
    size_t middle = times.size() / 2;
    double median = times[middle];
    
    // Compare with previous measurements
    size_t quarterSize = measurements.size() / 4;
    if (quarterSize >= 2) {
        double firstQuarterMedian = calculateMedianExecutionTime(
            std::vector<PerformanceMeasurement>(measurements.begin(), measurements.begin() + quarterSize));
        double lastQuarterMedian = calculateMedianExecutionTime(
            std::vector<PerformanceMeasurement>(measurements.end() - quarterSize, measurements.end()));
        
        trend.timeTrend = (lastQuarterMedian - firstQuarterMedian) / firstQuarterMedian;
        trend.isSlowing = trend.timeTrend > thresholds_.increasingTrendThreshold;
    }
    
    // Calculate memory trend
    float firstQuarterMemory = 0.0f, lastQuarterMemory = 0.0f;
    size_t firstQuarterCount = 0, lastQuarterCount = 0;
    
    for (size_t i = 0; i < quarterSize; ++i) {
        firstQuarterMemory += measurements[i].memoryUtilization;
        firstQuarterCount++;
    }
    
    for (size_t i = measurements.size() - quarterSize; i < measurements.size(); ++i) {
        lastQuarterMemory += measurements[i].memoryUtilization;
        lastQuarterCount++;
    }
    
    if (firstQuarterCount > 0 && lastQuarterCount > 0) {
        firstQuarterMemory /= firstQuarterCount;
        lastQuarterMemory /= lastQuarterCount;
        
        trend.memoryTrend = lastQuarterMemory - firstQuarterMemory;
        trend.isMemoryIncreasing = trend.memoryTrend > 0.01f;
    }
    
    // Predict slow time if trend is negative (improving)
    if (trend.timeTrend < -0.05f) {
        trend.predictedSlowTime = 0.0f; // Performance improving, no slowdown predicted
    } else if (trend.timeTrend > thresholds_.increasingTrendThreshold) {
        trend.predictedSlowTime = static_cast<float>(measurements.size() * 10); // Rough prediction
    }
    
    return trend;
}

inline bool PerformanceProfiler::exportToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Performance Measurements Export" << std::endl;
    file << "===============================" << std::endl << std::endl;
    
    const auto& measurements = statsAnalyzer_.getAllMeasurements();
    
    file << "Total Measurements: " << measurements.size() << std::endl;
    file << "Categories: " << getCategoriesSummary() << std::endl << std::endl;
    
    for (const auto& category : {PerformanceCategory::Neuron, PerformanceCategory::Synapse,
                                  PerformanceCategory::Memory, PerformanceCategory::Plasticity,
                                  PerformanceCategory::Integration, PerformanceCategory::Connectivity,
                                  PerformanceCategory::Simulation, PerformanceCategory::I_O}) {
        file << "\n" << getCategoryString(category) << " category:" << std::endl;
        file << "  Operations: " << getOperationsForCategory(category) << std::endl;
        
        for (const auto& operation : getOperationsForCategory(category)) {
            PerformanceProfile profile = statsAnalyzer_.calculateProfile(category, operation);
            file << "    " << operation << ": " << profile.totalOperations << " measurements, "
                 << std::fixed << std::setprecision(2) << profile.averageExecutionTime << " us avg, "
                 << profile.p95ExecutionTime << " us p95" << std::endl;
        }
    }
    
    file.close();
    return true;
}

inline bool PerformanceProfiler::importFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse line (simplified parsing for now)
        if (line.find("Measurement:") != std::string::npos) {
            // Parse measurement data
            // This is a simplified implementation - real implementation would parse CSV/JSON
        }
    }
    
    file.close();
    return true;
}

inline std::string PerformanceProfiler::generateReport() const {
    std::stringstream report;
    
    report << "Performance Analysis Report" << std::endl;
    report << "===========================" << std::endl << std::endl;
    
    report << "Total Measurements: " << statsAnalyzer_.getMeasurementCount() << std::endl;
    report << "Time Window: " << getTimeWindowString() << std::endl << std::endl;
    
    for (const auto& category : {PerformanceCategory::Neuron, PerformanceCategory::Synapse,
                                  PerformanceCategory::Memory, PerformanceCategory::Plasticity,
                                  PerformanceCategory::Integration, PerformanceCategory::Connectivity,
                                  PerformanceCategory::Simulation, PerformanceCategory::I_O}) {
        report << getCategoryString(category) << ":" << std::endl;
        
        for (const auto& operation : getOperationsForCategory(category)) {
            PerformanceProfile profile = statsAnalyzer_.calculateProfile(category, operation);
            if (profile.totalSamples > 0) {
                report << "  " << operation << ": " << profile.totalSamples << " samples, "
                       << std::fixed << std::setprecision(2) << profile.averageExecutionTime << " us avg, "
                       << std::fixed << std::setprecision(1) << profile.peakMemoryUtilization * 100 << "% mem, "
                       << profile.p95ExecutionTime << " us p95" << std::endl;
            }
        }
        report << std::endl;
    }
    
    return report.str();
}

inline void PerformanceProfiler::monitoringLoop() {
    while (monitoringActive_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(monitoringIntervalMs_));
        updateProfileStatistics();
    }
}

inline void PerformanceProfiler::updateProfileStatistics() {
    // Update any ongoing statistics or generate summary reports
    // This can be extended with more sophisticated analysis
}

inline std::string PerformanceProfiler::formatReport(const PerformanceProfile& profile,
                                                      PerformanceCategory category,
                                                      const std::string& operation) const {
    std::stringstream ss;
    
    ss << std::left << std::setw(20) << operation << ": "
       << std::setw(6) << profile.totalSamples << " samples, "
       << std::setw(8) << std::fixed << std::setprecision(2) << profile.averageExecutionTime << " us avg, "
       << std::setw(8) << std::fixed << std::setprecision(1) << profile.peakMemoryUtilization * 100 << "% mem, "
       << std::setw(8) << std::fixed << std::setprecision(2) << profile.p95ExecutionTime << " us p95";
    
    return ss.str();
}

inline const char* PerformanceProfiler::getCategoryString(PerformanceCategory category) {
    switch (category) {
        case PerformanceCategory::Neuron: return "neuron";
        case PerformanceCategory::Synapse: return "synapse";
        case PerformanceCategory::Memory: return "memory";
        case PerformanceCategory::Plasticity: return "plasticity";
        case PerformanceCategory::Integration: return "integration";
        case PerformanceCategory::Connectivity: return "connectivity";
        case PerformanceCategory::Simulation: return "simulation";
        case PerformanceCategory::I_O: return "io";
        default: return "unknown";
    }
}

inline std::vector<std::string> PerformanceProfiler::getOperationsForCategory(PerformanceCategory category) {
    // Return typical operations for each category
    switch (category) {
        case PerformanceCategory::Neuron:
            return {"create", "delete", "update_state", "step", "fire", "reset"};
        case PerformanceCategory::Synapse:
            return {"create", "delete", "update_weight", "record_spike", "step"};
        case PerformanceCategory::Memory:
            return {"allocate", "deallocate", "copy", "access", "evict"};
        case PerformanceCategory::Plasticity:
            return {"hebbian", "stdp", "eligibility_trace", "update_weight"};
        case PerformanceCategory::Integration:
            return {"compute_current", "update_potential", "check_threshold", "reset"};
        case PerformanceCategory::Connectivity:
            return {"find_connections", "traverse", "update_links"};
        case PerformanceCategory::Simulation:
            return {"advance_step", "synchronize", "checkpoint", "restore"};
        case PerformanceCategory::I_O:
            return {"read_input", "write_output", "process_signal"};
        default:
            return {"unknown"};
    }
}

inline std::string PerformanceProfiler::getTimeWindowString() const {
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    if (statsAnalyzer_.getMeasurementCount() == 0) {
        return "No measurements";
    }
    
    // Find earliest and latest measurement times
    const auto& measurements = statsAnalyzer_.getAllMeasurements();
    if (measurements.empty()) {
        return "No measurements";
    }
    
    uint64_t earliest = measurements.front().timestamp;
    uint64_t latest = measurements.back().timestamp;
    uint64_t duration = latest - earliest;
    
    std::stringstream ss;
    ss << duration << " ms";
    return ss.str();
}

inline void PerformanceProfiler::startMonitoring(uint32_t sampleIntervalMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!monitoringActive_) {
        monitoringActive_ = true;
        monitoringIntervalMs_ = sampleIntervalMs;
        monitoringThread_ = std::thread(&PerformanceProfiler::monitoringLoop, this);
    }
}

inline void PerformanceProfiler::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (monitoringActive_) {
        monitoringActive_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
}