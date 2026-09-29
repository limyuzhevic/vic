#pragma once

#include "../environment/Environment.hpp"
#include "../core/Monitoring.hpp"

namespace nlm {

class EnvironmentPerformanceAdapter {
private:
    Environment* environment_ = nullptr;
    PerformanceMonitor& monitor_ = PerformanceMonitor::getInstance();
    std::string componentName_ = "environment";
    int lastStepCount_ = 0;

    // Interaction metrics
    void recordInteractionMetrics() {
        if (!environment_) return;
        
        // Count environment interactions (steps taken)
        // Note: This is a simplified placeholder - actual implementation would track
        // specific interactions, events, state changes, etc.
        
        int currentStep = environment_->getState().getStep(); // Placeholder - EnvironmentState may not have this
        int stepsSinceLastUpdate = currentStep - lastStepCount_;
        
        monitor_.recordMetric("environment_steps", static_cast<double>(currentStep), MetricType::Counter, componentName_);
        monitor_.recordMetric("environment_steps_since_last_update", static_cast<double>(stepsSinceLastUpdate), MetricType::Gauge, componentName_);
        
        // Record reward metrics
        float reward = environment_->getLastReward(); // Assuming this method exists
        monitor_.recordMetric("environment_reward_last", reward, MetricType::Gauge, componentName_);
        
        // Record termination state
        bool isDone = environment_->isDone(); // Assuming this method exists
        monitor_.recordMetric("environment_terminal_state", isDone ? 1.0 : 0.0, MetricType::Gauge, componentName_);
        
        lastStepCount_ = currentStep;
    }

    // World state metrics
    void recordWorldStateMetrics() {
        if (!environment_) return;
        
        // Track state changes and complexity
        monitor_.recordMetric("environment_state_changes", 0.0, MetricType::Gauge, componentName_); // Placeholder
        monitor_.recordMetric("environment_state_complexity", 0.0, MetricType::Gauge, componentName_); // Placeholder
        
        // Record exploration metrics
        monitor_.recordMetric("environment_exploration_rate", 0.0, MetricType::Gauge, componentName_); // Placeholder
        monitor_.recordMetric("environment_visited_states", 0.0, MetricType::Gauge, componentName_); // Placeholder
    }

public:
    EnvironmentPerformanceAdapter(Environment* environment) : environment_(environment) {
        if (environment_) {
            monitor_.registerComponent(componentName_, ComponentType::Environment);
        }
    }

    ~EnvironmentPerformanceAdapter() {
        if (environment_) {
            monitor_.unregisterComponent(componentName_);
        }
    }

    void updateMetrics() {
        if (!environment_) return;
        
        auto profileScope = monitor_.profile("environment_metrics_update", componentName_);
        
        recordInteractionMetrics();
        recordWorldStateMetrics();
        
        profileScope.complete();
    }

    void setComponentName(const std::string& name) {
        componentName_ = name;
    }

    std::string getComponentName() const {
        return componentName_;
    }

    // Performance metrics for step execution
    void recordStepExecutionMetrics(std::chrono::duration<double> duration) {
        if (!environment_) return;
        
        monitor_.recordTiming("environment_step_execution_time", duration, componentName_);
        monitor_.recordMetric("environment_steps_per_second", 1.0 / duration.count(), MetricType::Gauge, componentName_);
    }

    // Resource utilization metrics
    void recordResourceMetrics() {
        // Placeholder for environment resource metrics
        // CPU, memory, I/O operations in the environment
        monitor_.recordMetric("environment_cpu_usage", 0.0, MetricType::Gauge, componentName_);
        monitor_.recordMetric("environment_memory_usage", 0.0, MetricType::Gauge, componentName_);
        monitor_.recordMetric("environment_io_operations", 0.0, MetricType::Counter, componentName_);
    }

    // Integration with PerformanceProfiler
    template<typename Func>
    auto profileEnvironmentOperation(const std::string& operation, Func&& func) {
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

    // Environment state monitoring
    void monitorEnvironmentState() {
        if (!environment_) return;
        
        // Track environment state changes for performance analysis
        // This would integrate with the actual environment state changes
        monitor_.recordMetric("environment_state_stability", 0.0, MetricType::Gauge, componentName_); // Placeholder
        monitor_.recordMetric("environment_state_entropy", 0.0, MetricType::Gauge, componentName_); // Placeholder
    }

private:
    // Helper methods
    double calculateStateComplexity() const {
        // Placeholder for calculating state complexity
        return 0.0;
    }

    double calculateExplorationRate() const {
        // Placeholder for calculating exploration rate
        return 0.0;
    }
};

} // namespace nlm
