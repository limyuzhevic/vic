#pragma once

#include "Brain.hpp"
#include "Monitoring.hpp"

namespace nlm {

class BrainPerformanceAdapter {
private:
    Brain* brain_ = nullptr;
    PerformanceMonitor& monitor_ = PerformanceMonitor::getInstance();
    std::string componentName_ = "brain";

    // Neuron firing metrics
    void recordNeuronFiringMetrics() {
        if (!brain_) return;
        
        int activeNeurons = 0;
        double totalFiringRate = 0.0;
        double maxFiringRate = 0.0;
        
        // Iterate through neural regions to collect metrics
        for (const auto& region : brain_->getRegions()) {
            for (const auto& population : region->getPopulations()) {
                activeNeurons += population->getActiveCount();
                double avgFiringRate = population->getAverageFiringRate();
                totalFiringRate += avgFiringRate * population->getSize();
                maxFiringRate = std::max(maxFiringRate, avgFiringRate);
            }
        }
        
        int totalNeurons = brain_->getTotalNeuronCount();
        double averageNetworkFiringRate = totalNeurons > 0 ? totalFiringRate / totalNeurons : 0.0;
        
        // Record metrics
        monitor_.recordMetric("neuron_active_count", activeNeurons, MetricType::Gauge, componentName_);
        monitor_.recordMetric("neuron_firing_rate_avg", averageNetworkFiringRate, MetricType::Gauge, componentName_);
        monitor_.recordMetric("neuron_firing_rate_max", maxFiringRate, MetricType::Gauge, componentName_);
        monitor_.recordMetric("neuron_utilization", 
                            static_cast<double>(activeNeurons) / std::max(1, totalNeurons), 
                            MetricType::Percentage, componentName_);
    }

    // Synapse activity metrics
    void recordSynapseActivityMetrics() {
        if (!brain_) return;
        
        double totalSynapticActivity = 0.0;
        int activeConnections = 0;
        double averageConnectionStrength = 0.0;
        int totalConnections = 0;
        
        // Collect synapse metrics from neural regions
        for (const auto& region : brain_->getRegions()) {
            for (const auto& population : region->getPopulations()) {
                for (const auto& synapse : population->getConnectedSynapses()) {
                    totalConnections++;
                    double weight = synapse->getWeight();
                    averageConnectionStrength += weight;
                    
                    if (synapse->isActive()) {
                        activeConnections++;
                    }
                    
                    totalSynapticActivity += weight * (synapse->isActive() ? 1.0 : 0.0);
                }
            }
        }
        
        monitor_.recordMetric("synapse_active_count", activeConnections, MetricType::Gauge, componentName_);
        monitor_.recordMetric("synapse_total_connections", totalConnections, MetricType::Gauge, componentName_);
        monitor_.recordMetric("synapse_connection_strength_avg", 
                            totalConnections > 0 ? averageConnectionStrength / totalConnections : 0.0, 
                            MetricType::Gauge, componentName_);
        monitor_.recordMetric("synapse_activity_level", 
                            totalConnections > 0 ? totalSynapticActivity / totalConnections : 0.0, 
                            MetricType::Gauge, componentName_);
    }

    // Plasticity metrics
    void recordPlasticityMetrics() {
        if (!brain_) return;
        
        double totalPlasticityChange = 0.0;
        int plasticityEvents = 0;
        
        // Collect plasticity metrics from all regions
        for (const auto& region : brain_->getRegions()) {
            // Access plasticity rules through the region or brain
            // For now, record placeholder metrics
            monitor_.recordMetric("plasticity_events", plasticityEvents, MetricType::Counter, componentName_);
            monitor_.recordMetric("plasticity_total_change", totalPlasticityChange, MetricType::Gauge, componentName_);
        }
    }

    // Development system metrics (using available Brain metrics)
    void recordDevelopmentMetrics() {
        if (!brain_) return;
        
        // Use available Brain methods for development metrics
        double averageFiringRate = brain_->getAverageFiringRate();
        size_t totalNeurons = brain_->getTotalNeuronCount();
        
        monitor_.recordMetric("brain_firing_rate", averageFiringRate, MetricType::Gauge, componentName_);
        monitor_.recordMetric("brain_neuron_count", static_cast<double>(totalNeurons), MetricType::Gauge, componentName_);
    }

    // Neuromodulation metrics (placeholder - would integrate with actual neuromodulation system)
    void recordNeuromodulationMetrics() {
        // Placeholder for neuromodulation metrics
        // This would integrate with the actual neuromodulation system when available
        monitor_.recordMetric("neuromodulation_level", 0.5, MetricType::Gauge, componentName_);
    }

public:
    BrainPerformanceAdapter(Brain* brain) : brain_(brain) {
        if (brain_) {
            monitor_.registerComponent(componentName_, ComponentType::Brain);
        }
    }

    ~BrainPerformanceAdapter() {
        if (brain_) {
            monitor_.unregisterComponent(componentName_);
        }
    }

    void updateMetrics() {
        if (!brain_) return;
        
        auto profileScope = monitor_.profile("brain_metrics_update", componentName_);
        
        recordNeuronFiringMetrics();
        recordSynapseActivityMetrics();
        recordPlasticityMetrics();
        recordDevelopmentMetrics();
        recordNeuromodulationMetrics();
        
        profileScope.complete();
    }

    void setComponentName(const std::string& name) {
        componentName_ = name;
    }

    std::string getComponentName() const {
        return componentName_;
    }

    void enableRealTimeMonitoring(bool enable) {
        // Implementation for enabling/disabling real-time monitoring
    }

    std::string generateReport() {
        return monitor_.generateReport(ReportFormat::Text);
    }

    void exportMetrics(const std::string& filepath) {
        monitor_.exportMetrics(filepath);
    }

    // Performance profile access
    std::vector<ProfileData> getPerformanceProfiles(const std::string& functionName = "") {
        return PerformanceProfiler::getInstance().getProfileData(functionName);
    }

    // Metrics access
    double getMetric(const std::string& name) {
        return monitor_.getCurrentMetric(name);
    }

    std::vector<MetricSample> getMetricHistory(const std::string& name, 
                                              std::chrono::duration<double> range) {
        return monitor_.getMetrics(name, range);
    }

    // Integration with PerformanceProfiler
    template<typename Func>
    auto profileBrainOperation(const std::string& operation, Func&& func) {
        return PerformanceProfiler::getInstance().profile(operation, func);
    }

    // Statistical analysis
    std::string generateStatisticalAnalysis(const std::string& metricName) {
        auto metrics = monitor_.getMetrics(metricName, std::chrono::hours(24));
        return StatsCalculator::analyzeTimeSeries(
            extractValues(metrics), std::chrono::hours(24)
        ).toString();
    }

private:
    std::vector<double> extractValues(const std::vector<MetricSample>& samples) {
        std::vector<double> values;
        values.reserve(samples.size());
        for (const auto& sample : samples) {
            values.push_back(sample.value);
        }
        return values;
    }
};

} // namespace nlm
