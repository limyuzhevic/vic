#include "monitor.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

class NLMMonitor::Impl {
public:
    Impl() : isMonitoring_(false) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() = default;
    
    void configure(const MonitoringConfig& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
    }
    
    MonitoringConfig getConfig() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_;
    }
    
    PerfMonitor& getPerformanceMonitor() {
        if (!performanceMonitor_) {
            performanceMonitor_ = std::make_unique<PerfMonitor>();
        }
        return *performanceMonitor_;
    }
    
    MemMonitor& getMemoryMonitor() {
        if (!memoryMonitor_) {
            memoryMonitor_ = std::make_unique<MemMonitor>();
        }
        return *memoryMonitor_;
    }
    
    NeuralMonitorClass& getNeuralMonitor() {
        if (!neuralMonitor_) {
            neuralMonitor_ = std::make_unique<NeuralMonitorClass>();
        }
        return *neuralMonitor_;
    }
    
    NeuroModMonitor& getNeuromodulationMonitor() {
        if (!neuromodulationMonitor_) {
            neuromodulationMonitor_ = std::make_unique<NeuroModMonitor>();
        }
        return *neuromodulationMonitor_;
    }
    
    MemoryLeakDetector& getMemoryLeakDetector() {
        if (!memoryLeakDetector_) {
            memoryLeakDetector_ = std::make_unique<MemoryLeakDetector>();
        }
        return *memoryLeakDetector_;
    }
    
    MemoryPoolManager& getMemoryPoolManager() {
        if (!memoryPoolManager_) {
            memoryPoolManager_ = std::make_unique<MemoryPoolManager>();
        }
        return *memoryPoolManager_;
    }
    
    PerformanceProfiler& getPerformanceProfiler() {
        if (!performanceProfiler_) {
            performanceProfiler_ = std::make_unique<PerformanceProfiler>();
        }
        return *performanceProfiler_;
    }
    
    BottleneckAnalyzer& getBottleneckAnalyzer() {
        if (!bottleneckAnalyzer_) {
            bottleneckAnalyzer_ = std::make_unique<BottleneckAnalyzer>();
        }
        return *bottleneckAnalyzer_;
    }
    
    void startAllMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        isMonitoring_ = true;
    }
    
    void stopAllMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        isMonitoring_ = false;
    }
    
    bool isAllMonitoring() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isMonitoring_;
    }
    
    MonitoringMetrics collectGlobalMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        MonitoringMetrics metrics;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        metrics.timestamp = duration;
        
        if (performanceMonitor_) {
            auto currentMetrics = performanceMonitor_->getCurrentMetrics();
            metrics.cpuUsage = currentMetrics["cpu_usage"];
            metrics.firingRate = currentMetrics["firing_rate"];
            metrics.predictionError = currentMetrics["prediction_error"];
        }
        
        if (neuromodulationMonitor_) {
            metrics.noveltyLevel = neuromodulationMonitor_->getNoveltyLevel();
            metrics.curiosityLevel = neuromodulationMonitor_->getCuriosityLevel();
            metrics.dopamineLevel = neuromodulationMonitor_->getDopamineLevel();
        }
        
        if (memoryMonitor_) {
            metrics.memoryUsage = static_cast<double>(memoryMonitor_->getCurrentMemoryUsage()) / (1024 * 1024);
        }
        
        return metrics;
    }
    
    std::vector<MonitoringMetrics> getMetricsHistory(Timestamp startTime, Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<MonitoringMetrics> result;
        
        if (performanceMonitor_) {
            auto metrics = performanceMonitor_->getAllMetrics(startTime, endTime);
            for (const auto& metric : metrics) {
                MonitoringMetrics m;
                m.timestamp = metric.timestamp;
                m.cpuUsage = metric.value;
                m.predictionError = metric.value;
                result.push_back(m);
            }
        }
        
        return result;
    }
    
    std::vector<std::string> checkForAlerts() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> alerts;
        
        if (performanceMonitor_) {
            auto currentMetrics = performanceMonitor_->getCurrentMetrics();
            if (currentMetrics.count("cpu_usage") && currentMetrics["cpu_usage"] > config_.alertThreshold) {
                alerts.push_back("High CPU usage detected: " + std::to_string(currentMetrics["cpu_usage"] * 100) + "%");
            }
            
            if (currentMetrics.count("memory_usage") && currentMetrics["memory_usage"] > config_.alertThreshold) {
                alerts.push_back("High memory usage detected: " + std::to_string(currentMetrics["memory_usage"] * 100) + "%");
            }
        }
        
        if (memoryMonitor_ && memoryMonitor_->hasMemoryLeaks()) {
            alerts.push_back("Memory leaks detected");
        }
        
        return alerts;
    }
    
    std::string generateComprehensiveReport() const {
        std::stringstream report;
        
        report << "=== Comprehensive NLM Monitoring Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        report << "System Status:\n";
        report << "  Real-time monitoring: " << (config_.enableRealTimeMonitoring ? "ENABLED" : "DISABLED") << "\n";
        report << "  Memory tracking: " << (config_.enableMemoryTracking ? "ENABLED" : "DISABLED") << "\n";
        report << "  Neural monitoring: " << (config_.enableNeuralMonitoring ? "ENABLED" : "DISABLED") << "\n";
        report << "  Neuromodulation monitoring: " << (config_.enableNeuromodulationMonitoring ? "ENABLED" : "DISABLED") << "\n";
        report << "  Leak detection: " << (config_.enableLeakDetection ? "ENABLED" : "DISABLED") << "\n";
        report << "  Performance profiling: " << (config_.enablePerformanceProfiling ? "ENABLED" : "DISABLED") << "\n";
        report << "  Bottleneck analysis: " << (config_.enableBottleneckAnalysis ? "ENABLED" : "DISABLED") << "\n\n";
        
        auto metrics = collectGlobalMetrics();
        
        report << "Current Metrics:\n";
        report << "  CPU Usage: " << (metrics.cpuUsage * 100) << "%\n";
        report << "  Memory Usage: " << metrics.memoryUsage << " MB\n";
        report << "  Firing Rate: " << metrics.firingRate << " Hz\n";
        report << "  Prediction Error: " << metrics.predictionError << "\n";
        report << "  Novelty Level: " << metrics.noveltyLevel << "\n";
        report << "  Curiosity Level: " << metrics.curiosityLevel << "\n";
        report << "  Dopamine Level: " << metrics.dopamineLevel << "\n\n";
        
        auto alerts = checkForAlerts();
        if (!alerts.empty()) {
            report << "Alerts:\n";
            for (const auto& alert : alerts) {
                report << "  ⚠️  " << alert << "\n";
            }
            report << "\n";
        }
        
        if (performanceProfiler_) {
            auto bottlenecks = performanceProfiler_->identifyBottlenecks();;
            if (!bottlenecks.empty()) {
                report << "Performance Issues:\n";
                for (const auto& bottleneck : bottlenecks) {
                    report << "  - " << bottleneck << "\n";
                }
                report << "\n";
            }
        }
        
        report << "=== End of Report ===\n";
        
        return report.str();
    }
    
    bool exportAllData(const std::string& filepath, const std::string& format) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "json") {
            file << "{\n";
            file << "  \"monitoringConfig\": {\n";
            file << "    \"enableRealTimeMonitoring\": " << (config_.enableRealTimeMonitoring ? "true" : "false") << ",\n";
            file << "    \"enableMemoryTracking\": " << (config_.enableMemoryTracking ? "true" : "false") << ",\n";
            file << "    \"enableNeuralMonitoring\": " << (config_.enableNeuralMonitoring ? "true" : "false") << ",\n";
            file << "    \"enableNeuromodulationMonitoring\": " << (config_.enableNeuromodulationMonitoring ? "true" : "false") << ",\n";
            file << "    \"enableLeakDetection\": " << (config_.enableLeakDetection ? "true" : "false") << ",\n";
            file << "    \"enablePerformanceProfiling\": " << (config_.enablePerformanceProfiling ? "true" : "false") << ",\n";
            file << "    \"enableBottleneckAnalysis\": " << (config_.enableBottleneckAnalysis ? "true" : "false") << "\n";
            file << "  },\n";
            
            auto metrics = collectGlobalMetrics();
            file << "  \"currentMetrics\": {\n";
            file << "    \"cpuUsage\": " << metrics.cpuUsage << ",\n";
            file << "    \"memoryUsage\": " << metrics.memoryUsage << ",\n";
            file << "    \"firingRate\": " << metrics.firingRate << ",\n";
            file << "    \"predictionError\": " << metrics.predictionError << ",\n";
            file << "    \"noveltyLevel\": " << metrics.noveltyLevel << ",\n";
            file << "    \"curiosityLevel\": " << metrics.curiosityLevel << ",\n";
            file << "    \"dopamineLevel\": " << metrics.dopamineLevel << "\n";
            file << "  }\n";
            file << "}\n";
        }
        
        file.close();
        return true;
    }
    
    bool importAllData(const std::string& filepath, const std::string& format) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file.close();
        return true;
    }
    
    std::string analyzeOverallPerformance() const {
        std::stringstream analysis;
        
        analysis << "=== Overall Performance Analysis ===\n";
        
        if (performanceProfiler_) {
            auto hotspots = performanceProfiler_->identifyHotspots();;
            analysis << "Hotspots identified: " << hotspots.size() << "\n";
            if (!hotspots.empty()) {
                analysis << "Top hotspots:\n";
                for (size_t i = 0; i < std::min(3ULL, hotspots.size()); ++i) {
                    analysis << "  " << (i + 1) << ". " << hotspots[i] << "\n";
                }
            }
        }
        
        if (bottleneckAnalyzer_) {
            auto bottlenecks = bottleneckAnalyzer_->analyzeBottlenecks();;
            analysis << "Bottlenecks identified: " << bottlenecks.size() << "\n";
            if (!bottlenecks.empty()) {
                analysis << "Top bottlenecks:\n";
                for (size_t i = 0; i < std::min(3ULL, bottlenecks.size()); ++i) {
                    analysis << "  " << (i + 1) << ". " << bottlenecks[i].bottleneck_type 
                             << " (Severity: " << (bottlenecks[i].severity * 100) << "%)\n";
                }
            }
        }
        
        if (memoryMonitor_ && memoryMonitor_->hasMemoryLeaks()) {
            analysis << "⚠️  Memory leaks detected\n";
        }
        
        double overallScore = 0.0;
        if (performanceProfiler_) {
            auto scores = performanceProfiler_->getPerformanceScore();;
            overallScore = scores["overall_score"];
        }
        
        analysis << "\nOverall Performance Score: " << (overallScore * 100) << "%\n";
        
        if (overallScore > 0.8) {
            analysis << "Performance is good.\n";
        } else if (overallScore > 0.6) {
            analysis << "Performance is acceptable.\n";
        } else {
            analysis << "Performance needs improvement.\n";
        }
        
        return analysis.str();
    }
    
    std::string generateOptimizationRecommendations() const {
        std::stringstream recommendations;
        
        recommendations << "=== Optimization Recommendations ===\n\n";
        
        if (performanceProfiler_) {
            auto bottlenecks = performanceProfiler_->identifyBottlenecks();;
            if (!bottlenecks.empty()) {
                recommendations << "Performance Optimization:\n";
                recommendations << "  1. Optimize hotspots:\n";
                for (const auto& bottleneck : bottlenecks) {
                    if (bottleneck == "cpu") {
                        recommendations << "     - Optimize CPU-intensive functions\n";
                        recommendations << "     - Consider parallel processing\n";
                    } else if (bottleneck == "memory") {
                        recommendations << "     - Optimize memory usage\n";
                        recommendations << "     - Consider memory pooling\n";
                    } else if (bottleneck == "io") {
                        recommendations << "     - Optimize I/O operations\n";
                        recommendations << "     - Implement caching\n";
                    }
                }
                recommendations << "\n";
            }
        }
        
        if (memoryMonitor_ && memoryMonitor_->hasMemoryLeaks()) {
            recommendations << "Memory Optimization:\n";
            recommendations << "  1. Fix memory leaks\n";
            recommendations << "  2. Implement better memory management\n";
            recommendations << "  3. Use smart pointers and RAII\n\n";
        }
        
        if (bottleneckAnalyzer_) {
            auto bottlenecks = bottleneckAnalyzer_->analyzeBottlenecks();;
            if (!bottlenecks.empty()) {
                recommendations << "System Optimization:\n";
                recommendations << "  1. Address identified bottlenecks\n";
                recommendations << "  2. Improve resource utilization\n";
                recommendations << "  3. Monitor system load\n\n";
            }
        }
        
        recommendations << "General Recommendations:\n";
        recommendations << "  1. Regularly profile and analyze performance\n";
        recommendations << "  2. Implement comprehensive monitoring\n";
        recommendations << "  3. Use optimization tools and techniques\n";
        recommendations << "  4. Monitor system metrics continuously\n";
        
        return recommendations.str();
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        return duration;
    }
    
    std::string getCurrentTimestampString() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    mutable std::mutex mutex_;
    bool isMonitoring_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    MonitoringConfig config_;
    
    std::unique_ptr<PerfMonitor> performanceMonitor_;
    std::unique_ptr<MemMonitor> memoryMonitor_;
    std::unique_ptr<NeuralMonitorClass> neuralMonitor_;
    std::unique_ptr<NeuroModMonitor> neuromodulationMonitor_;
    std::unique_ptr<MemoryLeakDetector> memoryLeakDetector_;
    std::unique_ptr<MemoryPoolManager> memoryPoolManager_;
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::unique_ptr<BottleneckAnalyzer> bottleneckAnalyzer_;
};

NLMMonitor::NLMMonitor() : pImpl(new Impl()) {}

NLMMonitor::~NLMMonitor() {
    delete pImpl;
}

void NLMMonitor::configure(const MonitoringConfig& config) {
    pImpl->configure(config);
}

MonitoringConfig NLMMonitor::getConfig() const {
    return pImpl->getConfig();
}

PerfMonitor& NLMMonitor::getPerformanceMonitor() {
    return pImpl->getPerformanceMonitor();
}

MemMonitor& NLMMonitor::getMemoryMonitor() {
    return pImpl->getMemoryMonitor();
}

NeuralMonitorClass& NLMMonitor::getNeuralMonitor() {
    return pImpl->getNeuralMonitor();
}

NeuroModMonitor& NLMMonitor::getNeuromodulationMonitor() {
    return pImpl->getNeuromodulationMonitor();
}

MemoryLeakDetector& NLMMonitor::getMemoryLeakDetector() {
    return pImpl->getMemoryLeakDetector();
}

MemoryPoolManager& NLMMonitor::getMemoryPoolManager() {
    return pImpl->getMemoryPoolManager();
}

PerformanceProfiler& NLMMonitor::getPerformanceProfiler() {
    return pImpl->getPerformanceProfiler();
}

BottleneckAnalyzer& NLMMonitor::getBottleneckAnalyzer() {
    return pImpl->getBottleneckAnalyzer();
}

void NLMMonitor::startAllMonitoring() {
    pImpl->startAllMonitoring();
}

void NLMMonitor::stopAllMonitoring() {
    pImpl->stopAllMonitoring();
}

bool NLMMonitor::isAllMonitoring() const {
    return pImpl->isAllMonitoring();
}

MonitoringMetrics NLMMonitor::collectGlobalMetrics() const {
    return pImpl->collectGlobalMetrics();
}

std::vector<MonitoringMetrics> NLMMonitor::getMetricsHistory(Timestamp startTime, Timestamp endTime) const {
    return pImpl->getMetricsHistory(startTime, endTime);
}

std::vector<std::string> NLMMonitor::checkForAlerts() const {
    return pImpl->checkForAlerts();
}

std::string NLMMonitor::generateComprehensiveReport() const {
    return pImpl->generateComprehensiveReport();
}

bool NLMMonitor::exportAllData(const std::string& filepath, const std::string& format) const {
    return pImpl->exportAllData(filepath, format);
}

bool NLMMonitor::importAllData(const std::string& filepath, const std::string& format) {
    return pImpl->importAllData(filepath, format);
}

std::string NLMMonitor::analyzeOverallPerformance() const {
    return pImpl->analyzeOverallPerformance();
}

std::string NLMMonitor::generateOptimizationRecommendations() const {
    return pImpl->generateOptimizationRecommendations();
}

} // namespace nlm
