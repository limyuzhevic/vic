#pragma once

#include "../memory/Memory.hpp"
#include "../core/Monitoring.hpp"

namespace nlm {

class MemoryPerformanceAdapter {
private:
    WorkingMemory* workingMemory_ = nullptr;
    EpisodicMemory* episodicMemory_ = nullptr;
    PerformanceMonitor& monitor_ = PerformanceMonitor::getInstance();
    std::string componentName_ = "memory";

    // Working memory metrics
    void recordWorkingMemoryMetrics() {
        if (!workingMemory_) return;
        
        size_t capacity = workingMemory_->getCapacity();
        size_t currentSize = workingMemory_->getCurrentSize();
        
        monitor_.recordMetric("working_memory_capacity", static_cast<double>(capacity), MetricType::Gauge, componentName_);
        monitor_.recordMetric("working_memory_usage", static_cast<double>(currentSize), MetricType::Gauge, componentName_);
        monitor_.recordMetric("working_memory_utilization", 
                            static_cast<double>(currentSize) / std::max(1.0, static_cast<double>(capacity)), 
                            MetricType::Percentage, componentName_);
    }

    // Episodic memory metrics
    void recordEpisodicMemoryMetrics() {
        if (!episodicMemory_) return;
        
        // Get episodic memory size if available
        size_t episodicSize = 0; // Placeholder - would need EpisodicMemory API
        monitor_.recordMetric("episodic_memory_size", static_cast<double>(episodicSize), MetricType::Gauge, componentName_);
        
        // Record growth rate if available
        monitor_.recordMetric("episodic_memory_growth_rate", 0.0, MetricType::Gauge, componentName_);
    }

public:
    MemoryPerformanceAdapter(WorkingMemory* workingMemory, EpisodicMemory* episodicMemory) 
        : workingMemory_(workingMemory), episodicMemory_(episodicMemory) {
        if (workingMemory_ || episodicMemory_) {
            monitor_.registerComponent(componentName_, ComponentType::Memory);
        }
    }

    ~MemoryPerformanceAdapter() {
        if (workingMemory_ || episodicMemory_) {
            monitor_.unregisterComponent(componentName_);
        }
    }

    void updateMetrics() {
        auto profileScope = monitor_.profile("memory_metrics_update", componentName_);
        
        recordWorkingMemoryMetrics();
        recordEpisodicMemoryMetrics();
        
        profileScope.complete();
    }

    void setComponentName(const std::string& name) {
        componentName_ = name;
    }

    std::string getComponentName() const {
        return componentName_;
    }

    // Capacity management metrics
    void recordCapacityMetrics() {
        if (!workingMemory_) return;
        
        size_t capacity = workingMemory_->getCapacity();
        size_t currentSize = workingMemory_->getCurrentSize();
        
        // Record utilization metrics
        double utilization = static_cast<double>(currentSize) / std::max(1.0, static_cast<double>(capacity));
        monitor_.recordMetric("memory_capacity_utilization", utilization, MetricType::Gauge, componentName_);
        
        // Record efficiency metrics
        if (currentSize > 0) {
            monitor_.recordMetric("memory_efficiency", 1.0 - (utilization * utilization), MetricType::Gauge, componentName_);
        }
    }

    // Access pattern metrics
    void recordAccessPatternMetrics() {
        // Placeholder for access pattern analysis
        // Would track read/write operations, cache hits, etc.
        monitor_.recordMetric("memory_access_frequency", 0.0, MetricType::Gauge, componentName_);
        monitor_.recordMetric("memory_cache_hit_rate", 0.0, MetricType::Percentage, componentName_);
    }

    // Integration with PerformanceProfiler
    template<typename Func>
    auto profileMemoryOperation(const std::string& operation, Func&& func) {
        return PerformanceProfiler::getInstance().profile(operation, func);
    }

    // Metrics access
    double getMetric(const std::string& name) {
        return monitor_.getCurrentMetric(name);
    }

    std::vector<MetricSample> getMetricHistory(const std::string& name, 
                                              std::chrono::duration<double> range) {
        return monitor_.getMetrics(name, range);
    }

    // Generate reports
    std::string generateReport() {
        return monitor_.generateReport(ReportFormat::Text);
    }

    void exportMetrics(const std::string& filepath) {
        monitor_.exportMetrics(filepath);
    }

private:
    // Placeholder for EpisodicMemory methods that don't exist yet
    size_t getEpisodicMemorySize() const {
        // Placeholder implementation
        return 0;
    }
};

} // namespace nlm
