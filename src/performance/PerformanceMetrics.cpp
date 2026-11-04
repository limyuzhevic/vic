#include "PerformanceMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>

namespace nlm {

PerformanceMetrics::PerformanceMetrics()
    : startTime(0.0), endTime(0.0), currentTime(0.0)
    , samplingInterval(0.1), maxDataPoints(10000), realTimeMonitoring(false) {}

PerformanceMetrics::~PerformanceMetrics() = default;

void PerformanceMetrics::recordMetric(const std::string& name, MetricCategory category, double value,
                                     double timestamp, const std::unordered_map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    MetricPoint point;
    point.name = name;
    point.category = category;
    point.value = value;
    point.timestamp = timestamp;
    point.tags = tags;
    
    addMetric(point);
}

void PerformanceMetrics::recordNeuronMetrics(size_t neuronCount, size_t activeNeurons, size_t firingNeurons) {
    double timestamp = currentTime;
    
    recordMetric("total_neurons", MetricCategory::NeuronActivity, static_cast<double>(neuronCount), timestamp);
    recordMetric("active_neurons", MetricCategory::NeuronActivity, static_cast<double>(activeNeurons), timestamp);
    recordMetric("firing_neurons", MetricCategory::NeuronActivity, static_cast<double>(firingNeurons), timestamp);
    
    if (neuronCount > 0) {
        double firingRate = static_cast<double>(firingNeurons) / static_cast<double>(neuronCount) * 100.0;
        recordMetric("firing_percentage", MetricCategory::NeuronActivity, firingRate, timestamp);
    }
}

void PerformanceMetrics::recordSynapticMetrics(size_t synapseCount, float excitationInhibitionRatio) {
    double timestamp = currentTime;
    
    recordMetric("total_synapses", MetricCategory::SynapticActivity, static_cast<double>(synapseCount), timestamp);
    recordMetric("excitation_inhibition_ratio", MetricCategory::SynapticActivity, 
                 static_cast<double>(excitationInhibitionRatio), timestamp);
}

void PerformanceMetrics::recordMemoryMetrics(size_t workingMemorySize, size_t episodicMemorySize) {
    double timestamp = currentTime;
    
    recordMetric("working_memory_size", MetricCategory::MemoryUsage, static_cast<double>(workingMemorySize), timestamp);
    recordMetric("episodic_memory_size", MetricCategory::MemoryUsage, static_cast<double>(episodicMemorySize), timestamp);
}

void PerformanceMetrics::recordProcessingMetrics(double processingTime, size_t stepsProcessed) {
    double timestamp = currentTime;
    
    recordMetric("processing_time", MetricCategory::ProcessingSpeed, processingTime, timestamp);
    if (stepsProcessed > 0) {
        double stepsPerSecond = static_cast<double>(stepsProcessed) / processingTime;
        recordMetric("steps_per_second", MetricCategory::ProcessingSpeed, stepsPerSecond, timestamp);
    }
}

void PerformanceMetrics::recordPlasticityMetrics(float stdpWeightChange, float hebbianWeightChange) {
    double timestamp = currentTime;
    
    recordMetric("stdp_weight_change", MetricCategory::Plasticity, static_cast<double>(stdpWeightChange), timestamp);
    recordMetric("hebbian_weight_change", MetricCategory::Plasticity, static_cast<double>(hebbianWeightChange), timestamp);
}

void PerformanceMetrics::recordDevelopmentMetrics(float developmentStage) {
    double timestamp = currentTime;
    
    recordMetric("development_stage", MetricCategory::Development, static_cast<double>(developmentStage), timestamp);
}

void PerformanceMetrics::recordNeuromodulationMetrics(float dopamineLevel, float curiosityLevel) {
    double timestamp = currentTime;
    
    recordMetric("dopamine_level", MetricCategory::Neuromodulation, static_cast<double>(dopamineLevel), timestamp);
    recordMetric("curiosity_level", MetricCategory::Neuromodulation, static_cast<double>(curiosityLevel), timestamp);
}

std::vector<PerformanceMetrics::MetricPoint> PerformanceMetrics::getMetrics(MetricCategory category) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    std::vector<MetricPoint> result;
    for (const auto& metric : metrics) {
        if (metric.category == category) {
            result.push_back(metric);
        }
    }
    
    return result;
}

std::vector<PerformanceMetrics::MetricPoint> PerformanceMetrics::getMetrics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    std::vector<MetricPoint> result;
    for (const auto& metric : metrics) {
        if (metric.name == name) {
            result.push_back(metric);
        }
    }
    
    return result;
}

double PerformanceMetrics::getAverageValue(const std::string& name, MetricCategory category,
                                          double startTime, double endTime) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    std::vector<double> values;
    for (const auto& metric : metrics) {
        if (metric.name == name && metric.category == category &&
            metric.timestamp >= startTime && metric.timestamp <= endTime) {
            values.push_back(metric.value);
        }
    }
    
    return values.empty() ? 0.0 : calculateMean(values);
}

double PerformanceMetrics::getMaximumValue(const std::string& name, MetricCategory category,
                                          double startTime, double endTime) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    std::vector<double> values;
    for (const auto& metric : metrics) {
        if (metric.name == name && metric.category == category &&
            metric.timestamp >= startTime && metric.timestamp <= endTime) {
            values.push_back(metric.value);
        }
    }
    
    return values.empty() ? 0.0 : *std::max_element(values.begin(), values.end());
}

double PerformanceMetrics::getMinimumValue(const std::string& name, MetricCategory category,
                                          double startTime, double endTime) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    std::vector<double> values;
    for (const auto& metric : metrics) {
        if (metric.name == name && metric.category == category &&
            metric.timestamp >= startTime && metric.timestamp <= endTime) {
            values.push_back(metric.value);
        }
    }
    
    return values.empty() ? 0.0 : *std::min_element(values.begin(), values.end());
}

PerformanceMetrics::PerformanceReport PerformanceMetrics::generateReport(double startTime, double endTime) const {
    PerformanceReport report;
    report.duration = endTime - startTime;
    
    // Calculate statistics
    size_t totalSteps = 0;
    float totalFiringRate = 0.0f;
    size_t firingRateCount = 0;
    float totalEIRatio = 0.0f;
    size_t eiratioCount = 0;
    size_t peakNeuronActivity = 0;
    double peakProcessingTime = 0.0;
    
    for (const auto& metric : metrics) {
        if (metric.timestamp >= startTime && metric.timestamp <= endTime) {
            if (metric.name == "firing_percentage") {
                totalFiringRate += metric.value;
                firingRateCount++;
                if (metric.value > peakNeuronActivity) {
                    peakNeuronActivity = static_cast<size_t>(metric.value);
                }
            } else if (metric.name == "steps_per_second") {
                totalProcessingTime += metric.value;
                if (metric.value > peakProcessingTime) {
                    peakProcessingTime = metric.value;
                }
            } else if (metric.name == "excitation_inhibition_ratio") {
                totalEIRatio += metric.value;
                eiratioCount++;
            }
        }
    }
    
    report.totalSteps = totalSteps;
    report.averageFiringRate = firingRateCount > 0 ? totalFiringRate / firingRateCount : 0.0f;
    report.excitationInhibitionRatio = eiratioCount > 0 ? totalEIRatio / eiratioCount : 0.0f;
    report.peakNeuronActivity = peakNeuronActivity;
    report.peakProcessingTime = peakProcessingTime;
    
    return report;
}

size_t PerformanceMetrics::getMetricCount(MetricCategory category) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    size_t count = 0;
    for (const auto& metric : metrics) {
        if (metric.category == category) {
            count++;
        }
    }
    return count;
}

void PerformanceMetrics::clear() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    metrics.clear();
    metricsByName.clear();
    metricsByCategory.clear();
}

void PerformanceMetrics::resetTime(double timestamp) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    startTime = timestamp;
    endTime = timestamp;
    currentTime = timestamp;
    metrics.clear();
}

void PerformanceMetrics::addMetric(const MetricPoint& point) {
    // Add to main metrics list
    metrics.push_back(point);
    
    // Add to categorized metrics
    metricsByCategory[point.category].push_back(point);
    
    // Add to named metrics
    metricsByName[point.name].push_back(point);
    
    // Update end time
    if (point.timestamp > endTime) {
        endTime = point.timestamp;
    }
    
    // Clean up old metrics if we're at the limit
    if (metrics.size() > maxDataPoints) {
        cleanupOldMetrics(startTime);
    }
}

void PerformanceMetrics::cleanupOldMetrics(double cutoffTime) {
    auto newEnd = metrics.end();
    auto newBegin = std::remove_if(metrics.begin(), metrics.end(),
                                  [cutoffTime](const MetricPoint& point) {
                                      return point.timestamp < cutoffTime;
                                  });
    metrics.erase(newBegin, newEnd);
    
    // Clean up categorized metrics
    for (auto& categoryMetrics : metricsByCategory) {
        auto newEnd = categoryMetrics.second.end();
        auto newBegin = std::remove_if(categoryMetrics.second.begin(), categoryMetrics.second.end(),
                                      [cutoffTime](const MetricPoint& point) {
                                          return point.timestamp < cutoffTime;
                                      });
        categoryMetrics.second.erase(newBegin, newEnd);
    }
    
    // Clean up named metrics
    for (auto& nameMetrics : metricsByName) {
        auto newEnd = nameMetrics.second.end();
        auto newBegin = std::remove_if(nameMetrics.second.begin(), nameMetrics.second.end(),
                                      [cutoffTime](const MetricPoint& point) {
                                          return point.timestamp < cutoffTime;
                                      });
        nameMetrics.second.erase(newBegin, newEnd);
    }
}

void PerformanceMetrics::updateTime(double timestamp) {
    currentTime = timestamp;
}

double PerformanceMetrics::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceMetrics::calculateVariance(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    double sumSquares = 0.0;
    for (double val : values) {
        double diff = val - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / values.size();
}

double PerformanceMetrics::calculateStdDev(const std::vector<double>& values, double mean) {
    return std::sqrt(calculateVariance(values, mean));
}

PerformanceProfiler::PerformanceProfiler()
    : currentProfileStartTime(0.0), currentProfileOperation(""), profileEnabled(true), detailedProfiling(false) {}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::startProfile(const std::string& operation, const std::unordered_map<std::string, std::string>& context) {
    if (!profileEnabled) return;
    
    std::lock_guard<std::mutex> lock(profileMutex);
    
    currentProfileOperation = operation;
    currentProfileContext = context;
    currentProfileStartTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
}

PerformanceProfiler::ProfilePoint PerformanceProfiler::stopProfile(const std::string& operation) {
    if (!profileEnabled) return ProfilePoint{};
    
    std::lock_guard<std::mutex> lock(profileMutex);
    
    if (currentProfileOperation != operation) return ProfilePoint{};
    
    double endTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    double duration = endTime - currentProfileStartTime;
    
    ProfilePoint point;
    point.operation = currentProfileOperation;
    point.duration = duration;
    point.timestamp = endTime;
    point.context = currentProfileContext;
    
    addProfilePoint(point);
    
    // Reset current profile
    currentProfileOperation = "";
    currentProfileContext.clear();
    
    return point;
}

void PerformanceProfiler::addProfilePoint(const ProfilePoint& point) {
    std::lock_guard<std::mutex> lock(profileMutex);
    
    profileData.push_back(point);
    profileDataByOperation[point.operation].push_back(point);
}

std::vector<PerformanceProfiler::ProfilePoint> PerformanceProfiler::getProfile(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(profileMutex);
    
    return profileDataByOperation[operation];
}

double PerformanceProfiler::getTotalProfileTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(profileMutex);
    
    auto it = profileDataByOperation.find(operation);
    if (it == profileDataByOperation.end()) return 0.0;
    
    double total = 0.0;
    for (const auto& point : it->second) {
        total += point.duration;
    }
    
    return total;
}

double PerformanceProfiler::getAverageProfileTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(profileMutex);
    
    auto it = profileDataByOperation.find(operation);
    if (it == profileDataByOperation.end()) return 0.0;
    
    const auto& points = it->second;
    if (points.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& point : points) {
        total += point.duration;
    }
    
    return total / points.size();
}

std::string PerformanceProfiler::generateProfileReport() const {
    std::stringstream ss;
    ss << "Performance Profile Report" << std::endl;
    ss << "==========================" << std::endl << std::endl;
    
    ss << "Total operations: " << profileData.size() << std::endl;
    ss << "Unique operations: " << profileDataByOperation.size() << std::endl << std::endl;
    
    for (const auto& operation : profileDataByOperation) {
        const auto& points = operation.second;
        if (points.empty()) continue;
        
        ss << "Operation: " << operation.first << std::endl;
        ss << "  Count: " << points.size() << std::endl;
        ss << "  Total time: " << getTotalProfileTime(operation.first) << " seconds" << std::endl;
        ss << "  Average time: " << getAverageProfileTime(operation.first) << " seconds" << std::endl;
        ss << "  Min time: " << (points.empty() ? 0.0 : 
            *std::min_element(points.begin(), points.end(),
                             [](const ProfilePoint& a, const ProfilePoint& b) {
                                 return a.duration < b.duration;
                             })->duration) << " seconds" << std::endl;
        ss << "  Max time: " << (points.empty() ? 0.0 : 
            *std::max_element(points.begin(), points.end(),
                             [](const ProfilePoint& a, const ProfilePoint& b) {
                                 return a.duration < b.duration;
                             })->duration) << " seconds" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

void PerformanceProfiler::clearProfileData() {
    std::lock_guard<std::mutex> lock(profileMutex);
    profileData.clear();
    profileDataByOperation.clear();
}

BrainMetricsCollector::BrainMetricsCollector()
    : core(nullptr), lastCollectTime(0.0), stepsSinceLastCollect(0) {
}

BrainMetricsCollector::~BrainMetricsCollector() = default;

bool BrainMetricsCollector::initialize(BrainCore* core) {
    this->core = core;
    
    if (!core) {
        NLM_LOG_ERROR("Cannot initialize metrics collector without core brain");
        return false;
    }
    
    metrics = std::make_shared<PerformanceMetrics>();
    profiler = std::make_shared<PerformanceProfiler>();
    
    NLM_LOG_INFO("Brain metrics collector initialized");
    return true;
}

void BrainMetricsCollector::collectMetrics(double timestamp) {
    if (!core) return;
    
    metrics->updateTime(timestamp);
    
    // Collect neuron metrics
    collectNeuronMetrics(core);
    
    // Collect synaptic metrics
    collectSynapticMetrics(core);
    
    // Collect memory metrics
    collectMemoryMetrics(core);
    
    // Collect processing metrics
    collectProcessingMetrics(core, 0.1); // Assume 0.1 second delta time
    
    // Collect development metrics
    float developmentStage = core->getDevelopmentalStage();
    metrics->recordDevelopmentMetrics(developmentStage);
    
    // Collect neuromodulation metrics
    float dopamineLevel = core->getDopamine() ? core->getDopamine()->getLevel() : 0.0f;
    float curiosityLevel = core->getCuriosity() ? core->getCuriosity()->getLevel() : 0.0f;
    metrics->recordNeuromodulationMetrics(dopamineLevel, curiosityLevel);
    
    stepsSinceLastCollect++;
}

void BrainMetricsCollector::collectNeuronMetrics(BrainCore* core) {
    if (!core) return;
    
    size_t neuronCount = core->getTotalNeuronCount();
    size_t activeNeurons = core->getActiveNeuronCount();
    size_t firingNeurons = core->getFiringNeuronCount();
    
    metrics->recordNeuronMetrics(neuronCount, activeNeurons, firingNeurons);
}

void BrainMetricsCollector::collectSynapticMetrics(BrainCore* core) {
    if (!core) return;
    
    size_t synapseCount = core->getTotalSynapseCount();
    float eiRatio = core->getExcitationInhibitionRatio();
    
    metrics->recordSynapticMetrics(synapseCount, eiRatio);
}

void BrainMetricsCollector::collectMemoryMetrics(BrainCore* core) {
    if (!core) return;
    
    size_t workingMemorySize = 0;
    size_t episodicMemorySize = 0;
    
    if (auto* workingMemory = integration->getWorkingMemory()) {
        workingMemorySize = workingMemory->getActiveTraces();
    }
    
    if (auto* episodicMemory = integration->getEpisodicMemory()) {
        episodicMemorySize = episodicMemory->getEpisodeCount();
    }
    
    metrics->recordMemoryMetrics(workingMemorySize, episodicMemorySize);
}

void BrainMetricsCollector::collectProcessingMetrics(BrainCore* core, double deltaTime) {
    if (!core) return;
    
    double processingTime = deltaTime; // Simplified - would use actual profiling
    size_t stepsProcessed = 1; // Simplified - would use actual step count
    
    metrics->recordProcessingMetrics(processingTime, stepsProcessed);
}

void BrainMetricsCollector::startProfiling(const std::string& operation) {
    if (profiler) {
        profiler->startProfile(operation);
    }
}

void BrainMetricsCollector::stopProfiling(const std::string& operation) {
    if (profiler) {
        profiler->stopProfile(operation);
    }
}

std::string BrainMetricsCollector::generateFullReport() const {
    std::stringstream ss;
    
    if (metrics) {
        ss << "=== Performance Metrics ===" << std::endl;
        ss << metrics->generateReport() << std::endl;
    }
    
    if (profiler) {
        ss << "=== Performance Profile ===" << std::endl;
        ss << profiler->generateProfileReport() << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
