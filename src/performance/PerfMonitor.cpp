#include "PerfMonitor.hpp"

namespace nlm {

PerfMonitor::PerfMonitor(bool enableProfiling,
                        bool enableBottleneckDetection,
                        bool enableTrendAnalysis,
                        bool enablePrediction,
                        uint64_t monitoringInterval)
    : initialized_(false)
    , profilingEnabled_(enableProfiling)
    , bottleneckDetectionEnabled_(enableBottleneckDetection)
    , trendAnalysisEnabled_(enableTrendAnalysis)
    , predictionEnabled_(enablePrediction)
    , monitoringInterval_(monitoringInterval)
    , monitoringActive_(false) {
}

PerfMonitor::~PerfMonitor() {
    if (initialized_) {
        finalize();
    }
}

bool PerfMonitor::initialize() {
    if (initialized_) {
        return true;
    }
    
    try {
        // Reset monitoring data
        monitoringData_ = MonitoringData();
        monitoringData_.lastUpdate = std::chrono::steady_clock::now();
        
        // Start monitoring thread
        startMonitoring();
        
        initialized_ = true;
        return true;
    } catch (...) {
        initialized_ = false;
        return false;
    }
}

void PerfMonitor::finalize() {
    if (!initialized_) {
        return;
    }
    
    // Stop monitoring
    stopMonitoring();
    
    // Clear all monitoring data
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        monitoringData_.profiles.clear();
        monitoringData_.bottlenecks.clear();
        monitoringData_.trends.clear();
        monitoringData_.predictions.clear();
        monitoringData_.recommendations.clear();
    }
    
    // Clear history
    performanceHistory_.clear();
    
    // Clear active bottlenecks
    activeBottlenecks_.clear();
    
    initialized_ = false;
}

void PerfMonitor::recordProfile(const PerformanceProfile& profile) {
    if (!initialized_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        monitoringData_.profiles.push_back(profile);
        monitoringData_.lastUpdate = std::chrono::steady_clock::now();
        
        // Add to history
        performanceHistory_.push_back(profile);
        
        // Keep only recent profiles
        while (performanceHistory_.size() > 1000) {
            performanceHistory_.pop_front();
        }
    }
}

void PerfMonitor::startProfiling() {
    profilingEnabled_ = true;
    
    if (monitoringActive_) {
        monitoringThread_ = std::thread(&PerfMonitor::monitoringThreadFunc, this);
    }
}

void PerfMonitor::stopProfiling() {
    profilingEnabled_ = false;
}

PerformanceProfile PerfMonitor::getCurrentProfile() const {
    if (!initialized_) {
        return PerformanceProfile();
    }
    
    ProfileMetrics metrics = collectProfileMetrics();
    return createProfileFromMetrics(metrics);
}

std::vector<PerformanceProfile> PerfMonitor::getPerformanceHistory(size_t limit) const {
    std::vector<PerformanceProfile> history;
    
    if (!initialized_) {
        return history;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        history.reserve(performanceHistory_.size());
        
        auto start = performanceHistory_.rbegin();
        auto end = performanceHistory_.rend();
        
        size_t count = 0;
        for (auto it = start; it != end && count < limit; ++it, ++count) {
            history.push_back(*it);
        }
    }
    
    return history;
}

size_t PerfMonitor::runBottleneckDetection(std::vector<BottleneckInfo>& bottlenecks) {
    bottlenecks.clear();
    
    if (!bottleneckDetectionEnabled_ || !initialized_) {
        return 0;
    }
    
    // Run all bottleneck detection algorithms
    detectMemoryBottlenecks(bottlenecks);
    detectProcessingBottlenecks(bottlenecks);
    detectIOBottlenecks(bottlenecks);
    detectConfigBottlenecks(bottlenecks);
    
    // Resolve detected bottlenecks
    resolveBottlenecks();
    
    return bottlenecks.size();
}

void PerfMonitor::getBottleneckAnalysis(std::vector<BottleneckInfo>& bottlenecks) {
    bottlenecks.clear();
    
    if (!initialized_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        bottlenecks = monitoringData_.bottlenecks;
    }
}

void PerfMonitor::runTrendAnalysis(std::vector<PerformanceTrend>& trends) {
    trends.clear();
    
    if (!trendAnalysisEnabled_ || !initialized_) {
        return;
    }
    
    calculateTrendScores(trends);
}

void PerfMonitor::getPerformanceTrends(std::vector<PerformanceTrend>& trends) {
    trends.clear();
    
    if (!initialized_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        trends = monitoringData_.trends;
    }
}

void PerfMonitor::runPerformancePrediction(size_t steps, std::vector<PerformancePrediction>& predictions) {
    predictions.clear();
    
    if (!predictionEnabled_ || !initialized_) {
        return;
    }
    
    // Get current step
    SimulationStep currentStep = monitoringData_.currentStep;
    
    // Run prediction algorithms
    predictSpikeProcessing(predictions);
    predictNeuralComputation(predictions);
    predictMemoryUsage(predictions);
    predictCPUUsage(predictions);
    
    // Update step
    monitoringData_.currentStep = currentStep + steps;
}

void PerfMonitor::getPerformancePredictions(std::vector<PerformancePrediction>& predictions) {
    predictions.clear();
    
    if (!initialized_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        predictions = monitoringData_.predictions;
    }
}

void PerfMonitor::generateOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations) {
    recommendations.clear();
    
    if (!initialized_) {
        return;
    }
    
    // Generate recommendations from each component
    generateMemoryOptimizationRecommendations(recommendations);
    generateProcessingOptimizationRecommendations(recommendations);
    generateConfigOptimizationRecommendations(recommendations);
    generateSystemOptimizationRecommendations(recommendations);
}

void PerfMonitor::getOptimizationRecommendations(std::vector<OptimizationRecommendation>& recommendations) {
    recommendations.clear();
    
    if (!initialized_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        recommendations = monitoringData_.recommendations;
    }
}

bool PerfMonitor::applyOptimizationRecommendation(const OptimizationRecommendation& recommendation) {
    if (!initialized_) {
        return false;
    }
    
    // Mark recommendation as applied
    auto it = monitoringData_.recommendations.begin();
    for (; it != monitoringData_.recommendations.end(); ++it) {
        if (it->component == recommendation.component &&
            it->recommendationType == recommendation.recommendationType) {
            it->applied = true;
            it->result = "Applied successfully";
            break;
        }
    }
    
    return it != monitoringData_.recommendations.end();
}

 double PerfMonitor::getOverallPerformanceScore() const {
    if (!initialized_ || monitoringData_.profiles.empty()) {
        return 0.0;
    }
    
    double totalScore = 0.0;
    size_t count = 0;
    
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        for (const auto& profile : monitoringData_.profiles) {
            double score = calculatePerformanceScore(profile);
            totalScore += score;
            count++;
        }
    }
    
    if (count == 0) {
        return 0.0;
    }
    
    return totalScore / count;
}

bool PerfMonitor::isPerformanceDegrading(size_t window) const {
    if (!initialized_ || performanceHistory_.size() < 2) {
        return false;
    }
    
    // Get recent profiles
    std::vector<PerformanceProfile> recentProfiles;
    recentProfiles.reserve(performanceHistory_.size());
    
    auto start = performanceHistory_.rbegin();
    auto end = performanceHistory_.rend();
    
    size_t count = 0;
    for (auto it = start; it != end && count < window; ++it, ++count) {
        recentProfiles.push_back(*it);
    }
    
    if (recentProfiles.size() < 2) {
        return false;
    }
    
    // Calculate trend in performance score
    double earlierScore = 0.0;
    double laterScore = 0.0;
    
    for (size_t i = 0; i < recentProfiles.size() / 2; ++i) {
        earlierScore += calculatePerformanceScore(recentProfiles[i]);
    }
    
    for (size_t i = recentProfiles.size() / 2; i < recentProfiles.size(); ++i) {
        laterScore += calculatePerformanceScore(recentProfiles[i]);
    }
    
    earlierScore /= recentProfiles.size() / 2;
    laterScore /= recentProfiles.size() / 2;
    
    return laterScore < earlierScore * 0.9; // 10% degradation threshold
}

std::string PerfMonitor::generateReport() const {
    std::ostringstream oss;
    
    oss << "=== Performance Monitoring Report ===\n";
    oss << "Profiling Enabled: " << (profilingEnabled_ ? "Yes" : "No") << "\n";
    oss << "Bottleneck Detection Enabled: " << (bottleneckDetectionEnabled_ ? "Yes" : "No") << "\n";
    oss << "Trend Analysis Enabled: " << (trendAnalysisEnabled_ ? "Yes" : "No") << "\n";
    oss << "Prediction Enabled: " << (predictionEnabled_ ? "Yes" : "No") << "\n";
    oss << "Monitoring Interval: " << monitoringInterval_ << " ms\n";
    
    oss << "\n=== Performance Metrics ===\n";
    PerformanceProfile currentProfile = getCurrentProfile();
    oss << "Current Step: " << currentProfile.step << "\n";
    oss << "CPU Usage: " << std::fixed << std::setprecision(2) << currentProfile.cpuUsage << "%\n";
    oss << "Memory Usage: " << std::fixed << std::setprecision(2) << currentProfile.memoryUsage << "%\n";
    oss << "Disk I/O: " << std::fixed << std::setprecision(3) << currentProfile.diskIO << "\n";
    oss << "Network I/O: " << std::fixed << std::setprecision(3) << currentProfile.networkIO << "\n";
    oss << "Spike Processing Time: " << std::fixed << std::setprecision(3)
        << currentProfile.spikeProcessingTime << " ms\n";
    oss << "Neural Computation Time: " << std::fixed << std::setprecision(3)
        << currentProfile.neuralComputationTime << " ms\n";
    oss << "Config Loading Time: " << std::fixed << std::setprecision(3)
        << currentProfile.configLoadingTime << " ms\n";
    
    oss << "\n=== Performance Score ===\n";
    double overallScore = getOverallPerformanceScore();
    oss << "Overall Performance Score: " << std::fixed << std::setprecision(2) << overallScore << " / 100\n";
    oss << "Performance Status: " << (isPerformanceDegrading() ? "Degrading" : "Stable") << "\n";
    
    oss << "\n=== Bottleneck Analysis ===\n";
    std::vector<BottleneckInfo> bottlenecks;
    getBottleneckAnalysis(bottlenecks);
    oss << "Active Bottlenecks: " << bottlenecks.size() << "\n";
    for (const auto& bottleneck : bottlenecks) {
        oss << "  - " << bottleneck.component << " (" << bottleneck.bottleneckType << ")\n";
        oss << "    Impact: " << std::fixed << std::setprecision(2) << (bottleneck.impactScore * 100) << "%\n";
        oss << "    Recommendation: " << bottleneck.recommendation << "\n";
    }
    
    oss << "\n=== Performance Trends ===\n";
    std::vector<PerformanceTrend> trends;
    getPerformanceTrends(trends);
    if (!trends.empty()) {
        oss << "Trend Average Spike Processing Time: " << std::fixed << std::setprecision(3)
            << trends.back().averageSpikeProcessingTime << " ms\n";
        oss << "Trend Average Neural Computation Time: " << std::fixed << std::setprecision(3)
            << trends.back().averageNeuralComputationTime << " ms\n";
        oss << "Trend Average Memory Usage: " << std::fixed << std::setprecision(2)
            << trends.back().memoryUsageMB << " MB\n";
        oss << "Trend Average CPU Utilization: " << std::fixed << std::setprecision(2)
            << trends.back().cpuUtilizationPercent << "%\n";
        oss << "Trend Performance Score: " << std::fixed << std::setprecision(2)
            << trends.back().overallPerformanceScore << " / 100\n";
    }
    
    oss << "\n=== Optimization Recommendations ===\n";
    std::vector<OptimizationRecommendation> recommendations;
    getOptimizationRecommendations(recommendations);
    oss << "Pending Recommendations: " << recommendations.size() << "\n";
    for (const auto& recommendation : recommendations) {
        if (!recommendation.applied) {
            oss << "  - " << recommendation.component << ": " << recommendation.description << "\n";
            oss << "    Expected Improvement: " << std::fixed << std::setprecision(2)
                << (recommendation.expectedImprovement * 100) << "%\n";
        }
    }
    
    return oss.str();
}

MonitoringData PerfMonitor::getMonitoringData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return monitoringData_;
}

// PerfMonitor implementation details
PerfMonitor::ProfileMetrics PerfMonitor::collectProfileMetrics() const {
    ProfileMetrics metrics;
    
    // In real implementation, would collect actual system metrics
    // This is a placeholder implementation
    
    metrics.cpuUsage = 50.0; // Placeholder
    metrics.memoryUsage = 60.0; // Placeholder
    metrics.diskIO = 0.2; // Placeholder
    metrics.networkIO = 0.1; // Placeholder
    metrics.spikeProcessingTime = 5.0; // ms placeholder
    metrics.neuralComputationTime = 15.0; // ms placeholder
    metrics.configLoadingTime = 1.0; // ms placeholder
    metrics.iowait = 0.1; // Placeholder
    metrics.activeSpikes = 100; // Placeholder
    metrics.totalSpikesProcessed = 1000; // Placeholder
    metrics.memoryAllocations = 500; // Placeholder
    metrics.configLoads = 10; // Placeholder
    metrics.networkPackets = 50; // Placeholder
    metrics.timestamp = std::chrono::steady_clock::now();
    
    return metrics;
}

PerformanceProfile PerfMonitor::createProfileFromMetrics(const ProfileMetrics& metrics) const {
    static SimulationStep currentStep = 0;
    
    PerformanceProfile profile;
    profile.step = currentStep++;
    profile.timestamp = metrics.timestamp;
    profile.cpuUsage = metrics.cpuUsage;
    profile.memoryUsage = metrics.memoryUsage;
    profile.diskIO = metrics.diskIO;
    profile.networkIO = metrics.networkIO;
    profile.spikeProcessingTime = metrics.spikeProcessingTime;
    profile.neuralComputationTime = metrics.neuralComputationTime;
    profile.configLoadingTime = metrics.configLoadingTime;
    profile.iowait = metrics.iowait;
    profile.activeSpikes = metrics.activeSpikes;
    profile.totalSpikesProcessed = metrics.totalSpikesProcessed;
    profile.memoryAllocations = metrics.memoryAllocations;
    profile.configLoads = metrics.configLoads;
    profile.networkPackets = metrics.networkPackets;
    
    return profile;
}

void PerfMonitor::detectMemoryBottlenecks(std::vector<BottleneckInfo>& bottlenecks) {
    // Check memory usage against threshold
    double memoryUsage = getCurrentProfile().memoryUsage / 100.0; // Convert percentage
    
    if (memoryUsage > MEMORY_THRESHOLD) {
        BottleneckInfo bottleneck;
        bottleneck.component = "MemoryOptimizer";
        bottleneck.bottleneckType = "Memory Usage";
        bottleneck.impactScore = (memoryUsage - MEMORY_THRESHOLD) / (1.0 - MEMORY_THRESHOLD);
        bottleneck.detectedTime = std::chrono::steady_clock::now();
        bottleneck.recommendation = "Increase memory pool size or enable compression";
        bottleneck.nextCheckTime = bottleneck.detectedTime + std::chrono::milliseconds(5000);
        bottleneck.resolved = false;
        
        bottlenecks.push_back(bottleneck);
        activeBottlenecks_["memory"] = bottleneck;
    }
}

void PerfMonitor::detectProcessingBottlenecks(std::vector<BottleneckInfo>& bottlenecks) {
    PerformanceProfile profile = getCurrentProfile();
    
    // Check spike processing time
    if (profile.spikeProcessingTime > SPIKE_PROCESSING_THRESHOLD) {
        BottleneckInfo bottleneck;
        bottleneck.component = "ProcessingOptimizer";
        bottleneck.bottleneckType = "Spike Processing";
        bottleneck.impactScore = std::min(1.0, (profile.spikeProcessingTime - SPIKE_PROCESSING_THRESHOLD) / 10.0);
        bottleneck.detectedTime = std::chrono::steady_clock::now();
        bottleneck.recommendation = "Optimize spike processing or increase SIMD usage";
        bottleneck.nextCheckTime = bottleneck.detectedTime + std::chrono::milliseconds(5000);
        bottleneck.resolved = false;
        
        bottlenecks.push_back(bottleneck);
        activeBottlenecks_["spike_processing"] = bottleneck;
    }
    
    // Check neural computation time
    if (profile.neuralComputationTime > NEURAL_COMPUTATION_THRESHOLD) {
        BottleneckInfo bottleneck;
        bottleneck.component = "ProcessingOptimizer";
        bottleneck.bottleneckType = "Neural Computation";
        bottleneck.impactScore = std::min(1.0, (profile.neuralComputationTime - NEURAL_COMPUTATION_THRESHOLD) / 20.0);
        bottleneck.detectedTime = std::chrono::steady_clock::now();
        bottleneck.recommendation = "Optimize neural computation or enable parallel processing";
        bottleneck.nextCheckTime = bottleneck.detectedTime + std::chrono::milliseconds(5000);
        bottleneck.resolved = false;
        
        bottlenecks.push_back(bottleneck);
        activeBottlenecks_["neural_computation"] = bottleneck;
    }
}

void PerfMonitor::detectIOBottlenecks(std::vector<BottleneckInfo>& bottlenecks) {
    PerformanceProfile profile = getCurrentProfile();
    
    // Check I/O utilization
    double ioUsage = profile.diskIO + profile.networkIO;
    
    if (ioUsage > IO_THRESHOLD) {
        BottleneckInfo bottleneck;
        bottleneck.component = "SystemOptimizer";
        bottleneck.bottleneckType = "I/O Utilization";
        bottleneck.impactScore = (ioUsage - IO_THRESHOLD) / (1.0 - IO_THRESHOLD);
        bottleneck.detectedTime = std::chrono::steady_clock::now();
        bottleneck.recommendation = "Optimize I/O operations or increase parallelism";
        bottleneck.nextCheckTime = bottleneck.detectedTime + std::chrono::milliseconds(5000);
        bottleneck.resolved = false;
        
        bottlenecks.push_back(bottleneck);
        activeBottlenecks_["io"] = bottleneck;
    }
}

void PerfMonitor::detectConfigBottlenecks(std::vector<BottleneckInfo>& bottlenecks) {
    PerformanceProfile profile = getCurrentProfile();
    
    // Check config loading time
    if (profile.configLoadingTime > 5.0) { // 5ms threshold
        BottleneckInfo bottleneck;
        bottleneck.component = "ConfigOptimizer";
        bottleneck.bottleneckType = "Configuration Loading";
        bottleneck.impactScore = std::min(1.0, (profile.configLoadingTime - 5.0) / 10.0);
        bottleneck.detectedTime = std::chrono::steady_clock::now();
        bottleneck.recommendation = "Enable configuration caching or compression";
        bottleneck.nextCheckTime = bottleneck.detectedTime + std::chrono::milliseconds(5000);
        bottleneck.resolved = false;
        
        bottlenecks.push_back(bottleneck);
        activeBottlenecks_["config"] = bottleneck;
    }
}

void PerfMonitor::calculateTrendScores(std::vector<PerformanceTrend>& trends) {
    if (performanceHistory_.size() < 2) {
        return;
    }
    
    // Calculate average values over a window
    size_t windowSize = std::min<size_t>(performanceHistory_.size(), 10);
    
    PerformanceTrend trend;
    trend.step = monitoringData_.currentStep;
    trend.timestamp = std::chrono::steady_clock::now();
    
    // Calculate averages
    double sumSpikeTime = 0.0;
    double sumNeurComputeTime = 0.0;
    double sumMemoryUsage = 0.0;
    double sumCPUUsage = 0.0;
    double sumConfigTime = 0.0;
    double sumPerfScore = 0.0;
    size_t count = 0;
    
    auto it = performanceHistory_.rbegin();
    for (size_t i = 0; i < windowSize && it != performanceHistory_.rend(); ++it, ++i, ++count) {
        const PerformanceProfile& profile = *it;
        
        sumSpikeTime += profile.spikeProcessingTime;
        sumNeurComputeTime += profile.neuralComputationTime;
        sumMemoryUsage += profile.memoryUsage;
        sumCPUUsage += profile.cpuUsage;
        sumConfigTime += profile.configLoadingTime;
        sumPerfScore += calculatePerformanceScore(profile);
    }
    
    if (count > 0) {
        trend.averageSpikeProcessingTime = sumSpikeTime / count;
        trend.averageNeuralComputationTime = sumNeurComputeTime / count;
        trend.memoryUsageMB = sumMemoryUsage / count;
        trend.cpuUtilizationPercent = sumCPUUsage / count;
        trend.configLoadingTimeMs = sumConfigTime / count;
        trend.overallPerformanceScore = sumPerfScore / count;
    }
    
    monitoringData_.trends.push_back(trend);
}

double PerfMonitor::calculatePerformanceScore(const PerformanceProfile& profile) const {
    double score = 100.0;
    
    // Deduct points for slow spike processing
    if (profile.spikeProcessingTime > 0) {
        score -= std::min(50.0, profile.spikeProcessingTime / 2.0);
    }
    
    // Deduct points for slow neural computation
    if (profile.neuralComputationTime > 0) {
        score -= std::min(30.0, profile.neuralComputationTime / 3.0);
    }
    
    // Deduct points for high memory usage
    if (profile.memoryUsage > 0) {
        score -= std::min(20.0, profile.memoryUsage);
    }
    
    // Deduct points for high CPU usage
    if (profile.cpuUsage > 0) {
        score -= std::min(10.0, profile.cpuUsage);
    }
    
    // Deduct points for slow config loading
    if (profile.configLoadingTime > 0) {
        score -= std::min(10.0, profile.configLoadingTime);
    }
    
    return std::max(0.0, score);
}

void PerfMonitor::predictSpikeProcessing(std::vector<PerformancePrediction>& predictions) {
    PerformanceProfile current = getCurrentProfile();
    
    for (size_t i = 1; i <= 5; ++i) { // Predict next 5 steps
        PerformancePrediction pred;
        pred.step = monitoringData_.currentStep + i;
        pred.predictedSpikeProcessingTime = current.spikeProcessingTime * (1.0 + i * 0.1); // Simple linear growth
        pred.predictedNeuralComputationTime = current.neuralComputationTime * (1.0 + i * 0.05);
        pred.predictedMemoryUsage = current.memoryUsage * (1.0 + i * 0.02);
        pred.predictedCPUUsage = current.cpuUsage * (1.0 + i * 0.03);
        pred.confidenceInterval = 0.8; // 80% confidence
        pred.predictionTime = std::chrono::steady_clock::now();
        
        predictions.push_back(pred);
    }
}

void PerfMonitor::predictNeuralComputation(std::vector<PerformancePrediction>& predictions) {
    // Similar to spike processing prediction but for neural computation
    PerformanceProfile current = getCurrentProfile();
    
    for (size_t i = 1; i <= 5; ++i) {
        PerformancePrediction pred;
        pred.step = monitoringData_.currentStep + i;
        pred.predictedNeuralComputationTime = current.neuralComputationTime * (1.0 + i * 0.05);
        pred.predictedSpikeProcessingTime = current.spikeProcessingTime * (1.0 + i * 0.1);
        pred.predictedMemoryUsage = current.memoryUsage * (1.0 + i * 0.02);
        pred.predictedCPUUsage = current.cpuUsage * (1.0 + i * 0.03);
        pred.confidenceInterval = 0.8;
        pred.predictionTime = std::chrono::steady_clock::now();
        
        predictions.push_back(pred);
    }
}

void PerfMonitor::predictMemoryUsage(std::vector<PerformancePrediction>& predictions) {
    // Similar to other predictions but for memory usage
    PerformanceProfile current = getCurrentProfile();
    
    for (size_t i = 1; i <= 5; ++i) {
        PerformancePrediction pred;
        pred.step = monitoringData_.currentStep + i;
        pred.predictedMemoryUsage = current.memoryUsage * (1.0 + i * 0.02);
        pred.predictedSpikeProcessingTime = current.spikeProcessingTime * (1.0 + i * 0.1);
        pred.predictedNeuralComputationTime = current.neuralComputationTime * (1.0 + i * 0.05);
        pred.predictedCPUUsage = current.cpuUsage * (1.0 + i * 0.03);
        pred.confidenceInterval = 0.8;
        pred.predictionTime = std::chrono::steady_clock::now();
        
        predictions.push_back(pred);
    }
}

void PerfMonitor::predictCPUUsage(std::vector<PerformancePrediction>& predictions) {
    // Similar to other predictions but for CPU usage
    PerformanceProfile current = getCurrentProfile();
    
    for (size_t i = 1; i <= 5; ++i) {
        PerformancePrediction pred;
        pred.step = monitoringData_.currentStep + i;
        pred.predictedCPUUsage = current.cpuUsage * (1.0 + i * 0.03);
        pred.predictedSpikeProcessingTime = current.spikeProcessingTime * (1.0 + i * 0.1);
        pred.predictedNeuralComputationTime = current.neuralComputationTime * (1.0 + i * 0.05);
        pred.predictedMemoryUsage = current.memoryUsage * (1.0 + i * 0.02);
        pred.confidenceInterval = 0.8;
        pred.predictionTime = std::chrono::steady_clock::now();
        
        predictions.push_back(pred);
    }
}

void PerfMonitor::generateMemoryOptimizationRecommendations(
    std::vector<OptimizationRecommendation>& recommendations) {
    // Check memory usage against threshold
    double memoryUsage = getCurrentProfile().memoryUsage / 100.0;
    
    if (memoryUsage > MEMORY_THRESHOLD) {
        OptimizationRecommendation rec;
        rec.component = "MemoryOptimizer";
        rec.recommendationType = "Memory Pool Optimization";
        rec.description = "Increase memory pool sizes for better allocation efficiency";
        rec.expectedImprovement = 0.3; // 30% improvement expected
        rec.recommendedTime = std::chrono::steady_clock::now();
        rec.applied = false;
        rec.result = "";
        
        recommendations.push_back(rec);
    }
}

void PerfMonitor::generateProcessingOptimizationRecommendations(
    std::vector<OptimizationRecommendation>& recommendations) {
    PerformanceProfile profile = getCurrentProfile();
    
    // Check spike processing performance
    if (profile.spikeProcessingTime > SPIKE_PROCESSING_THRESHOLD) {
        OptimizationRecommendation rec;
        rec.component = "ProcessingOptimizer";
        rec.recommendationType = "Spike Processing Optimization";
        rec.description = "Enable SIMD optimizations for spike processing";
        rec.expectedImprovement = 0.25; // 25% improvement expected
        rec.recommendedTime = std::chrono::steady_clock::now();
        rec.applied = false;
        rec.result = "";
        
        recommendations.push_back(rec);
    }
    
    // Check neural computation performance
    if (profile.neuralComputationTime > NEURAL_COMPUTATION_THRESHOLD) {
        OptimizationRecommendation rec;
        rec.component = "ProcessingOptimizer";
        rec.recommendationType = "Neural Computation Optimization";
        rec.description = "Enable parallel processing for neural computation";
        rec.expectedImprovement = 0.2; // 20% improvement expected
        rec.recommendedTime = std::chrono::steady_clock::now();
        rec.applied = false;
        rec.result = "";
        
        recommendations.push_back(rec);
    }
}

void PerfMonitor::generateConfigOptimizationRecommendations(
    std::vector<OptimizationRecommendation>& recommendations) {
    PerformanceProfile profile = getCurrentProfile();
    
    // Check config loading performance
    if (profile.configLoadingTime > 5.0) { // 5ms threshold
        OptimizationRecommendation rec;
        rec.component = "ConfigOptimizer";
        rec.recommendationType = "Configuration Caching";
        rec.description = "Enable configuration caching to reduce load times";
        rec.expectedImprovement = 0.4; // 40% improvement expected
        rec.recommendedTime = std::chrono::steady_clock::now();
        rec.applied = false;
        rec.result = "";
        
        recommendations.push_back(rec);
    }
}

void PerfMonitor::generateSystemOptimizationRecommendations(
    std::vector<OptimizationRecommendation>& recommendations) {
    PerformanceProfile profile = getCurrentProfile();
    
    // Check I/O performance
    double ioUsage = profile.diskIO + profile.networkIO;
    
    if (ioUsage > IO_THRESHOLD) {
        OptimizationRecommendation rec;
        rec.component = "SystemOptimizer";
        rec.recommendationType = "I/O Optimization";
        rec.description = "Enable asynchronous I/O operations";
        rec.expectedImprovement = 0.35; // 35% improvement expected
        rec.recommendedTime = std::chrono::steady_clock::now();
        rec.applied = false;
        rec.result = "";
        
        recommendations.push_back(rec);
    }
}

void PerfMonitor::monitoringThreadFunc() {
    while (monitoringActive_) {
        // Record a new performance profile
        PerformanceProfile profile = getCurrentProfile();
        recordProfile(profile);
        
        // Run bottleneck detection
        if (bottleneckDetectionEnabled_) {
            std::vector<BottleneckInfo> bottlenecks;
            runBottleneckDetection(bottlenecks);
            
            std::lock_guard<std::mutex> lock(dataMutex_);
            monitoringData_.bottlenecks = bottlenecks;
        }
        
        // Run trend analysis
        if (trendAnalysisEnabled_) {
            std::vector<PerformanceTrend> trends;
            runTrendAnalysis(trends);
            
            std::lock_guard<std::mutex> lock(dataMutex_);
            monitoringData_.trends = trends;
        }
        
        // Run performance prediction
        if (predictionEnabled_) {
            std::vector<PerformancePrediction> predictions;
            runPerformancePrediction(5, predictions); // Predict 5 steps ahead
            
            std::lock_guard<std::mutex> lock(dataMutex_);
            monitoringData_.predictions = predictions;
        }
        
        // Generate optimization recommendations
        {
            std::vector<OptimizationRecommendation> recommendations;
            generateOptimizationRecommendations(recommendations);
            
            std::lock_guard<std::mutex> lock(dataMutex_);
            monitoringData_.recommendations = recommendations;
        }
        
        // Sleep for monitoring interval
        std::this_thread::sleep_for(std::chrono::milliseconds(monitoringInterval_));
    }
}

void PerfMonitor::resolveBottlenecks() {
    // Check if any bottlenecks have been resolved
    auto it = activeBottlenecks_.begin();
    while (it != activeBottlenecks_.end()) {
        auto& bottleneck = it->second;
        
        if (bottleneck.nextCheckTime <= std::chrono::steady_clock::now()) {
            // Check if bottleneck is still present
            bool stillPresent = false;
            
            // Simple check - in real implementation would do more sophisticated checks
            PerformanceProfile profile = getCurrentProfile();
            
            if (bottleneck.component == "MemoryOptimizer" &&
                profile.memoryUsage / 100.0 <= MEMORY_THRESHOLD) {
                stillPresent = false;
            } else if (bottleneck.component == "ProcessingOptimizer" &&
                      profile.spikeProcessingTime <= SPIKE_PROCESSING_THRESHOLD) {
                stillPresent = false;
            } else if (bottleneck.component == "SystemOptimizer" &&
                      (profile.diskIO + profile.networkIO) <= IO_THRESHOLD) {
                stillPresent = false;
            } else if (bottleneck.component == "ConfigOptimizer" &&
                      profile.configLoadingTime <= 5.0) {
                stillPresent = false;
            } else {
                stillPresent = true;
            }
            
            if (!stillPresent) {
                bottleneck.resolved = true;
                activeBottlenecks_.erase(it++);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

} // namespace nlm