#include "VisualizationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <cmath>

namespace nlm {

// MonitorDataPoint implementation
std::string MonitorDataPoint::toJSON() const {
    std::ostringstream oss;
    oss << "{\"timestamp\": \"" << std::chrono::system_clock::to_time_t(timestamp) << "\", \"metric\": \"";
    
    // Convert metric to string
    const char* metricStr = "";
    switch (metric) {
        case MetricType::NeuralActivity: metricStr = "neural_activity"; break;
        case MetricType::MemoryUsage: metricStr = "memory_usage"; break;
        case MetricType::CognitivePerformance: metricStr = "cognitive_performance"; break;
        case MetricType::Neuromodulation: metricStr = "neuromodulation"; break;
        case MetricType::Plasticity: metricStr = "plasticity"; break;
        case MetricType::Development: metricStr = "development"; break;
        case MetricType::PredictionAccuracy: metricStr = "prediction_accuracy"; break;
        case MetricType::SimulationPerformance: metricStr = "simulation_performance"; break;
    }
    
    oss << metricStr << "\", \"label\": \"" << label << "\", \"value\": " << value;
    oss << ", \"additionalValues\": [";
    
    for (size_t i = 0; i < additionalValues.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << additionalValues[i];
    }
    oss << "], \"metadata\": {";
    
    bool first = true;
    for (const auto& pair : metadata) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    oss << "}}";
    
    return oss.str();
}

std::string MonitorDataPoint::toCSV() const {
    std::ostringstream oss;
    oss << std::chrono::system_clock::to_time_t(timestamp) << ",\" << label << "\"," << value;
    
    for (double val : additionalValues) {
        oss << "," << val;
    }
    
    oss << ",\"";
    for (const auto& pair : metadata) {
        oss << pair.first << "=" << pair.second << ";";
    }
    oss << "\"";
    
    return oss.str();
}

// TextVisualization implementation
TextVisualization::TextVisualization(const std::string& filename, bool includeTimestamps)
    : filename_(filename), includeTimestamps_(includeTimestamps), maxPoints_(1000), precision_(3) {
    std::ofstream file(filename, std::ios::trunc);
    file.close();
}

TextVisualization::~TextVisualization() {
    if (file_.is_open()) {
        file_.close();
    }
}

void TextVisualization::write(const std::vector<MonitorDataPoint>& data) {
    if (!file_.is_open() && !filename_.empty()) {
        file_.open(filename_, std::ios::app);
    }
    
    if (!file_.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open visualization file: ") + filename_);
        return;
    }
    
    // Write header if file is empty
    if (file_.tellp() == 0) {
        file_ << "# NLM Visualization Data" << std::endl;
        file_ << "# Format: timestamp, label, value, additional_values, metadata" << std::endl;
        if (includeTimestamps_) {
            file_ << "# Timestamp is Unix time in seconds" << std::endl;
        }
        file_ << std::endl;
    }
    
    // Write data points (limited to maxPoints_)
    size_t startIndex = std::max<size_t>(0, static_cast<size_t>(data.size()) - maxPoints_);
    
    for (size_t i = startIndex; i < data.size(); ++i) {
        const auto& point = data[i];
        
        file_ << point.timestamp.time_since_epoch().count();
        if (includeTimestamps_) file_ << ", ";
        
        file_ << point.label << ", " << std::fixed << std::setprecision(precision_) << point.value;
        
        for (double val : point.additionalValues) {
            file_ << ", " << std::setprecision(precision_) << val;
        }
        
        file_ << ", {";
        bool first = true;
        for (const auto& pair : point.metadata) {
            if (!first) file_ << ", ";
            file_ << pair.first << "=" << pair.second;
            first = false;
        }
        file_ << "}" << std::endl;
    }
    
    file_.flush();
}

void TextVisualization::close() {
    if (file_.is_open()) {
        file_.close();
    }
}

// JSONVisualization implementation
JSONVisualization::JSONVisualization(const std::string& filename)
    : filename_(filename), indent_(2), includeMetadata_(true) {
    std::ofstream file(filename, std::ios::trunc);
    file.close();
}

JSONVisualization::~JSONVisualization() {
    if (file_.is_open()) {
        file_.close();
    }
}

void JSONVisualization::write(const std::vector<MonitorDataPoint>& data) {
    if (!file_.is_open() && !filename_.empty()) {
        file_.open(filename_, std::ios::app);
    }
    
    if (!file_.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open JSON visualization file: ") + filename_);
        return;
    }
    
    // Write JSON array start if file is empty
    if (file_.tellp() == 0) {
        file_ << "[" << std::endl;
    }
    
    // Write data points
    for (size_t i = 0; i < data.size(); ++i) {
        const auto& point = data[i];
        
        if (i > 0) file_ << "," << std::endl;
        file_ << std::string(indent_, ' ');
        file_ << point.toJSON();
    }
    
    file_ << std::endl << std::string(indent_, ' ') << "]";
    file_.flush();
}

void JSONVisualization::close() {
    if (file_.is_open()) {
        file_.close();
    }
}

// CSVVisualization implementation
CSVVisualization::CSVVisualization(const std::string& filename)
    : filename_(filename), includeHeaders_(true) {
    std::ofstream file(filename, std::ios::trunc);
    file.close();
}

CSVVisualization::~CSVVisualization() {
    if (file_.is_open()) {
        file_.close();
    }
}

void CSVVisualization::write(const std::vector<MonitorDataPoint>& data) {
    if (!file_.is_open() && !filename_.empty()) {
        file_.open(filename_, std::ios::app);
    }
    
    if (!file_.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open CSV visualization file: ") + filename_);
        return;
    }
    
    // Write headers if file is empty
    if (file_.tellp() == 0 && includeHeaders_) {
        file_ << "timestamp,label,value,additional_values,metadata" << std::endl;
    }
    
    // Write data points
    for (const auto& point : data) {
        file_ << point.toCSV() << std::endl;
    }
    
    file_.flush();
}

void CSVVisualization::close() {
    if (file_.is_open()) {
        file_.close();
    }
}

// RealTimeVisualizer implementation
RealTimeVisualizer::RealTimeVisualizer(std::shared_ptr<Brain> brain)
    : brain_(brain), running_(false), dataFilter_(nullptr),
      visualizeNeurons_(true), visualizeMemory_(true),
      visualizeCognitive_(true), maxDataPoints_(10000) {
    
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
}

RealTimeVisualizer::~RealTimeVisualizer() {
    stop();
}

bool RealTimeVisualizer::start(size_t updateIntervalMs) {
    if (running_) {
        return false;
    }
    
    running_ = true;
    visualizationThread_ = std::thread(&RealTimeVisualizer::visualizationLoop, this);
    
    NLM_LOG_INFO(std::string("Real-time visualization started with update interval: ") + 
        std::to_string(updateIntervalMs) + " ms");
    
    return true;
}

void RealTimeVisualizer::stop() {
    if (running_) {
        running_ = false;
        
        if (visualizationThread_.joinable()) {
            visualizationThread_.join();
        }
        
        NLM_LOG_INFO("Real-time visualization stopped");
    }
}

void RealTimeVisualizer::addOutput(std::unique_ptr<VisualizationOutput> output) {
    if (output) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        outputs_.push_back(std::move(output));
    }
}

void RealTimeVisualizer::setVisualizationOptions(const std::string& options) {
    // Parse visualization options (simplified)
    // In a real implementation, would parse JSON or key=value pairs
}

void RealTimeVisualizer::setDataFilter(const std::function<bool(const MonitorDataPoint&)>& filter) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    dataFilter_ = filter;
}

std::vector<std::string> RealTimeVisualizer::getActiveOutputs() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::vector<std::string> outputNames;
    for (const auto& output : outputs_) {
        outputNames.push_back(output->getFormatName());
    }
    return outputNames;
}

void RealTimeVisualizer::visualizationLoop() {
    while (running_) {
        try {
            collectData();
            writeData();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Visualization loop error: ") + e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void RealTimeVisualizer::collectData() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    dataBuffer_.clear();
    
    // Collect neural data
    if (visualizeNeurons_ && brain_) {
        // Add neural activity metrics
        MonitorDataPoint activity(MetricType::NeuralActivity, "neuron_activity", 0.0);
        activity.additionalValues.push_back(brain_->getTotalNeuronCount());
        activity.additionalValues.push_back(brain_->getActiveNeuronCount());
        activity.additionalValues.push_back(brain_->getFiringNeuronCount());
        if (!dataFilter_ || dataFilter_(activity)) {
            dataBuffer_.push_back(activity);
        }
    }
    
    // Collect memory data
    if (visualizeMemory_ && brain_) {
        // Add memory metrics
        MonitorDataPoint memory(MetricType::MemoryUsage, "working_memory_size", 0.0);
        auto workingMem = brain_->getWorkingMemory();n        if (workingMem) {
            memory.value = workingMem->getMemoryActivity();
        }
        if (!dataFilter_ || dataFilter_(memory)) {
            dataBuffer_.push_back(memory);
        }
    }
    
    // Collect cognitive data
    if (visualizeCognitive_ && brain_) {
        // Add cognitive metrics
        MonitorDataPoint cognitive(MetricType::CognitivePerformance, "prediction_accuracy", 0.0);
        auto predSystem = brain_->getPredictionSystem();
        if (predSystem) {
            cognitive.value = predSystem->getConfidence();
        }
        if (!dataFilter_ || dataFilter_(cognitive)) {
            dataBuffer_.push_back(cognitive);
        }
    }
    
    // Trim data buffer if too large
    if (dataBuffer_.size() > maxDataPoints_) {
        dataBuffer_.erase(dataBuffer_.begin(),
                         dataBuffer_.begin() + (dataBuffer_.size() - maxDataPoints_));
    }
}

void RealTimeVisualizer::writeData() {
    if (dataBuffer_.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    for (auto& output : outputs_) {
        try {
            output->write(dataBuffer_);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to write to visualization output: ") + e.what());
        }
    }
}

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMonitor(std::shared_ptr<Brain> brain)
    : brain_(brain), running_(false), sampleIntervalMs_(1000) {
    
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

bool PerformanceMonitor::start(size_t sampleIntervalMs) {
    if (running_) {
        return false;
    }
    
    running_ = true;
    monitoringThread_ = std::thread(&PerformanceMonitor::monitoringLoop, this);
    sampleIntervalMs_ = sampleIntervalMs;
    
    NLM_LOG_INFO(std::string("Performance monitoring started with sample interval: ") + 
        std::to_string(sampleIntervalMs_) + " ms");
    
    return true;
}

void PerformanceMonitor::stop() {
    if (running_) {
        running_ = false;
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        NLM_LOG_INFO("Performance monitoring stopped");
    }
}

void PerformanceMonitor::addMetricCollector(const std::string& name,
                                           const std::function<double()>& collector) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    metricCollectors_[name] = collector;
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::getStats() const {
    PerformanceStats stats;
    
    if (!brain_) {
        return stats;
    }
    
    stats.totalSteps = 0;  // Would need to track this
    stats.totalSpikes = brain_->getTotalSpikeCount();
    stats.activeNeurons = brain_->getActiveNeuronCount();
    stats.averageFiringRate = brain_->getAverageFiringRate();
    stats.activeNeurons = static_cast<size_t>(stats.averageFiringRate * 100);  // Rough estimate
    stats.averageFiringRate = brain_->getAverageFiringRate();
    stats.activeNeurons = brain_->getActiveNeuronCount();
    stats.averageFiringRate = brain_->getAverageFiringRate();
    
    return stats;
}

void PerformanceMonitor::monitoringLoop() {
    while (running_) {
        try {
            std::lock_guard<std::mutex> lock(dataMutex_);
            
            MonitorDataPoint point(MetricType::SimulationPerformance, "performance", 0.0);
            
            // Collect metric collectors
            double totalMetric = 0.0;
            size_t validCollectors = 0;
            
            for (const auto& pair : metricCollectors_) {
                try {
                    double value = pair.second();
                    point.additionalValues.push_back(value);
                    totalMetric += value;
                    validCollectors++;
                } catch (...) {
                    // Skip collectors that throw
                }
            }
            
            if (validCollectors > 0) {
                point.value = totalMetric / validCollectors;
            }
            
            performanceData_.push_back(point);
            
            // Trim old data
            auto cutoff = std::chrono::system_clock::now() - 
                std::chrono::milliseconds(sampleIntervalMs_ * 60);  // Keep last hour
            
            performanceData_.erase(std::remove_if(performanceData_.begin(), performanceData_.end(),
                [cutoff](const MonitorDataPoint& p) {
                    return p.timestamp < cutoff;
                }), performanceData_.end());
            
            std::this_thread::sleep_for(std::chrono::milliseconds(sampleIntervalMs_));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Monitoring loop error: ") + e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

// MemoryVisualizer implementation
MemoryVisualizer::MemoryVisualizer(std::shared_ptr<Brain> brain) : brain_(brain) {}

std::vector<MonitorDataPoint> MemoryVisualizer::getWorkingMemoryData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        MonitorDataPoint point(MetricType::MemoryUsage, "working_memory_activity", workingMem->getMemoryActivity());
        point.additionalValues.push_back(workingMem->getCurrentSize());
        point.additionalValues.push_back(workingMem->getCapacity());
        data.push_back(point);
    }
    
    return data;
}

std::vector<MonitorDataPoint> MemoryVisualizer::getEpisodicMemoryData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        MonitorDataPoint point(MetricType::MemoryUsage, "episodic_memory_episodes", episodicMem->getEpisodeCount());
        point.additionalValues.push_back(episodicMem->getAverageReward());
        data.push_back(point);
    }
    
    return data;
}

std::vector<MonitorDataPoint> MemoryVisualizer::getAssociativeMemoryData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto assocMem = brain_->getAssociativeMemory();
    if (assocMem) {
        MonitorDataPoint point(MetricType::MemoryUsage, "associative_memory_associations", assocMem->getAssociationCount());
        point.additionalValues.push_back(assocMem->getPatternCount());
        data.push_back(point);
    }
    
    return data;
}

MemoryVisualizer::MemoryStats MemoryVisualizer::getMemoryStats() const {
    MemoryStats stats;
    
    if (!brain_) {
        return stats;
    }
    
    auto workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        stats.workingMemorySize = workingMem->getCurrentSize();
        stats.workingMemoryCapacity = workingMem->getCapacity();
        stats.workingMemoryUtilization = stats.workingMemoryCapacity > 0 ? 
            static_cast<float>(stats.workingMemorySize) / stats.workingMemoryCapacity : 0.0f;
    }
    
    auto episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        stats.episodicMemoryCount = episodicMem->getEpisodeCount();
    }
    
    auto assocMem = brain_->getAssociativeMemory();
    if (assocMem) {
        stats.associativeMemoryAssociations = assocMem->getAssociationCount();
        stats.associativeMemoryPatterns = assocMem->getPatternCount();
    }
    
    return stats;
}

std::vector<std::pair<std::string, double>> MemoryVisualizer::getActivityPatterns() const {
    std::vector<std::pair<std::string, double>> patterns;
    
    if (!brain_) {
        return patterns;
    }
    
    auto workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        patterns.emplace_back("working_memory_activity", workingMem->getMemoryActivity());
    }
    
    auto episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        patterns.emplace_back("episodic_memory_relevance", episodicMem->getAverageReward());
    }
    
    return patterns;
}

// NeuralVisualizer implementation
NeuralVisualizer::NeuralVisualizer(std::shared_ptr<Brain> brain) : brain_(brain) {}

std::vector<MonitorDataPoint> NeuralVisualizer::getNeuralActivityData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    MonitorDataPoint point(MetricType::NeuralActivity, "total_neurons", brain_->getTotalNeuronCount());
    point.additionalValues.push_back(brain_->getActiveNeuronCount());
    point.additionalValues.push_back(brain_->getFiringNeuronCount());
    point.additionalValues.push_back(brain_->getAverageFiringRate());
    data.push_back(point);
    
    return data;
}

std::vector<MonitorDataPoint> NeuralVisualizer::getRegionActivityData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    size_t regionCount = brain_->getRegionCount();
    MonitorDataPoint point(MetricType::NeuralActivity, "region_count", regionCount);
    point.additionalValues.push_back(regionCount > 0 ? brain_->getTotalNeuronCount() / regionCount : 0);
    data.push_back(point);
    
    return data;
}

std::vector<MonitorDataPoint> NeuralVisualizer::getSynapseActivityData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    MonitorDataPoint point(MetricType::NeuralActivity, "total_synapses", brain_->getTotalSynapseCount());
    point.additionalValues.push_back(brain_->getExcitationInhibitionRatio());
    data.push_back(point);
    
    return data;
}

NeuralVisualizer::NeuralStats NeuralVisualizer::getNeuralStats() const {
    NeuralStats stats;
    
    if (!brain_) {
        return stats;
    }
    
    stats.totalNeurons = brain_->getTotalNeuronCount();
    stats.activeNeurons = brain_->getActiveNeuronCount();
    stats.firingNeurons = brain_->getFiringNeuronCount();
    stats.averageFiringRate = brain_->getAverageFiringRate();
    stats.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
    stats.totalSynapses = brain_->getTotalSynapseCount();
    stats.averageSynapticWeight = stats.totalSynapses > 0 ? 0.5f : 0.0f;  // Would need to calculate
    stats.regionCount = brain_->getRegionCount();
    stats.populationCount = stats.regionCount * 3;  // Rough estimate
    
    return stats;
}

// CognitiveVisualizer implementation
CognitiveVisualizer::CognitiveVisualizer(std::shared_ptr<Brain> brain) : brain_(brain) {}

std::vector<MonitorDataPoint> CognitiveVisualizer::getPredictionData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto predSystem = brain_->getPredictionSystem();
    if (predSystem) {
        MonitorDataPoint point(MetricType::PredictionAccuracy, "prediction_error", predSystem->getPredictionError());
        point.additionalValues.push_back(predSystem->getConfidence());
        data.push_back(point);
    }
    
    return data;
}

std::vector<MonitorDataPoint> CognitiveVisualizer::getPlanningData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto planner = brain_->getPlanner();
    if (planner) {
        MonitorDataPoint point(MetricType::CognitivePerformance, "planning_depth", 5);  // Would get from planner
        point.additionalValues.push_back(0.5f);  // Exploration rate
        data.push_back(point);
    }
    
    return data;
}

std::vector<MonitorDataPoint> CognitiveVisualizer::getAttentionData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto attention = brain_->getAttention();
    if (attention) {
        MonitorDataPoint point(MetricType::CognitivePerformance, "attention_focus", 0.5f);  // Would get from attention
        point.additionalValues.push_back(0.3f);  // Inhibition strength
        data.push_back(point);
    }
    
    return data;
}

std::vector<MonitorDataPoint> CognitiveVisualizer::getConceptData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto conceptFormation = brain_->getConceptFormation();
    if (conceptFormation) {
        MonitorDataPoint point(MetricType::CognitivePerformance, "concept_count", 10);  // Would get from concept formation
        point.additionalValues.push_back(0.7f);  // Similarity threshold
        data.push_back(point);
    }
    
    return data;
}

CognitiveVisualizer::CognitiveStats CognitiveVisualizer::getCognitiveStats() const {
    CognitiveStats stats;
    
    if (!brain_) {
        return stats;
    }
    
    auto predSystem = brain_->getPredictionSystem();
    if (predSystem) {
        stats.predictionAccuracy = 1.0f - predSystem->getPredictionError();
        stats.predictionError = predSystem->getPredictionError();
        stats.predictionConfidence = predSystem->getConfidence();
    }
    
    return stats;
}

std::vector<std::pair<std::string, double>> CognitiveVisualizer::getCognitivePerformance() const {
    std::vector<std::pair<std::string, double>> performance;
    
    if (!brain_) {
        return performance;
    }
    
    performance.emplace_back("prediction_accuracy", 0.85f);  // Would get from prediction system
    performance.emplace_back("planning_efficiency", 0.75f);
    performance.emplace_back("attention_focus", 0.80f);
    performance.emplace_back("concept_cohesion", 0.70f);
    
    return performance;
}

// DevelopmentVisualizer implementation
DevelopmentVisualizer::DevelopmentVisualizer(std::shared_ptr<Brain> brain) : brain_(brain) {}

std::vector<MonitorDataPoint> DevelopmentVisualizer::getDevelopmentData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    MonitorDataPoint point(MetricType::Development, "developmental_stage", 2);  // Would get from development system
    point.additionalValues.push_back(0.5f);  // Plasticity level
    data.push_back(point);
    
    return data;
}

std::vector<MonitorDataPoint> DevelopmentVisualizer::getPlasticityData() const {
    std::vector<MonitorDataPoint> data;
    
    if (!brain_) {
        return data;
    }
    
    auto stdp = brain_->getSTDP();
    if (stdp) {
        MonitorDataPoint point(MetricType::Plasticity, "stdp_strength", 0.01f);  // Would get from STDP system
        point.additionalValues.push_back(0.012f);  // LTD weight
        data.push_back(point);
    }
    
    return data;
}

DevelopmentVisualizer::DevelopmentStats DevelopmentVisualizer::getDevelopmentStats() const {
    DevelopmentStats stats;
    
    if (!brain_) {
        return stats;
    }
    
    stats.developmentalStage = brain_->getDevelopmentalStage();  // Would need conversion
    stats.plasticityLevel = 0.5f;  // Would get from plasticity system
    stats.synaptogenesisRate = 0.0001f;
    stats.pruningRate = 0.00001f;
    stats.criticalPeriodActive = false;  // Would check from development system
    stats.developmentProgress = 50;  // Percentage
    
    return stats;
}

// VisualizationManager implementation
VisualizationManager::VisualizationManager(std::shared_ptr<Brain> brain)
    : brain_(brain), running_(false) {
    
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
}

VisualizationManager::~VisualizationManager() {
    stopAll();
}

void VisualizationManager::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize visualization manager: brain is null");
        return;
    }
    
    memoryVisualizer_ = std::make_unique<MemoryVisualizer>(brain_);
    neuralVisualizer_ = std::make_unique<NeuralVisualizer>(brain_);
    cognitiveVisualizer_ = std::make_unique<CognitiveVisualizer>(brain_);
    neuromodulationVisualizer_ = std::make_unique<NeuromodulationVisualizer>(brain_);
    developmentVisualizer_ = std::make_unique<DevelopmentVisualizer>(brain_);
    
    NLM_LOG_INFO("Visualization manager initialized");
}

bool VisualizationManager::startAll(size_t updateIntervalMs) {
    if (running_) {
        return false;
    }
    
    // Start real-time visualizer if enabled
    if (realTimeVisualizer_) {
        realTimeVisualizer_->start(updateIntervalMs);
    }
    
    // Start performance monitor if enabled
    if (performanceMonitor_) {
        performanceMonitor_->start(updateIntervalMs * 10);  // Less frequent monitoring
    }
    
    running_ = true;
    
    NLM_LOG_INFO("Visualization manager started");
    return true;
}

void VisualizationManager::stopAll() {
    if (running_) {
        running_ = false;
        
        if (realTimeVisualizer_) {
            realTimeVisualizer_->stop();
        }
        
        if (performanceMonitor_) {
            performanceMonitor_->stop();
        }
        
        NLM_LOG_INFO("Visualization manager stopped");
    }
}

std::string VisualizationManager::generateReport() const {
    std::ostringstream oss;
    oss << "NLM System Report" << std::endl;
    oss << "================" << std::endl << std::endl;
    
    if (!brain_) {
        oss << "No brain instance available" << std::endl;
        return oss.str();
    }
    
    // Generate comprehensive report
    oss << "Neural Statistics:" << std::endl;
    auto neuralStats = neuralVisualizer_->getNeuralStats();
    oss << "  Total Neurons: " << neuralStats.totalNeurons << std::endl;
    oss << "  Active Neurons: " << neuralStats.activeNeurons << std::endl;
    oss << "  Firing Neurons: " << neuralStats.firingNeurons << std::endl;
    oss << "  Average Firing Rate: " << neuralStats.averageFiringRate << " Hz" << std::endl;
    oss << "  E/I Ratio: " << neuralStats.excitationInhibitionRatio << std::endl;
    oss << "  Total Synapses: " << neuralStats.totalSynapses << std::endl;
    oss << "  Regions: " << neuralStats.regionCount << std::endl;
    oss << std::endl;
    
    oss << "Memory Statistics:" << std::endl;
    auto memoryStats = memoryVisualizer_->getMemoryStats();
    oss << "  Working Memory: " << memoryStats.workingMemorySize << "/" 
        << memoryStats.workingMemoryCapacity << " (" 
        << (memoryStats.workingMemoryCapacity > 0 ? (memoryStats.workingMemoryUtilization * 100) : 0)
        << "% utilized)" << std::endl;
    oss << "  Episodic Episodes: " << memoryStats.episodicMemoryCount << std::endl;
    oss << "  Associative Associations: " << memoryStats.associativeMemoryAssociations << std::endl;
    oss << std::endl;
    
    oss << "Performance:" << std::endl;
    // Add performance metrics
    oss << "  Status: Running" << std::endl;
    oss << "  Data Points: " << (realTimeVisualizer_ ? realTimeVisualizer_->getActiveOutputs().size() : 0) << std::endl;
    
    return oss.str();
}

bool VisualizationManager::exportData(const std::string& format, const std::string& filepath) {
    // Convert format string to enum
    VisualizationFormat visFormat;
    if (format == "text") visFormat = VisualizationFormat::TEXT;
    else if (format == "json") visFormat = VisualizationFormat::JSON;
    else if (format == "csv") visFormat = VisualizationFormat::CSV;
    else {
        NLM_LOG_ERROR(std::string("Unsupported visualization format: ") + format);
        return false;
    }
    
    // In a real implementation, would write data in the specified format
    // For now, just log the request
    NLM_LOG_INFO(std::string("Would export data in format: ") + format + 
        " to file: " + filepath);
    
    return true;
}

void VisualizationManager::setOption(const std::string& key, const std::string& value) {
    options_[key] = value;
    
    // Update visualizers based on options
    if (key == "neurons_enabled") {
        if (realTimeVisualizer_) {
            realTimeVisualizer_->enableNeuronVisualization(value == "true" || value == "1");
        }
    } else if (key == "memory_enabled") {
        if (realTimeVisualizer_) {
            realTimeVisualizer_->enableMemoryVisualization(value == "true" || value == "1");
        }
    } else if (key == "cognitive_enabled") {
        if (realTimeVisualizer_) {
            realTimeVisualizer_->enableCognitiveVisualization(value == "true" || value == "1");
        }
    }
}

bool VisualizationManager::isRunning() const {
    return running_;
}

std::vector<std::string> VisualizationManager::getActiveVisualizers() const {
    std::vector<std::string> visualizers;
    
    if (realTimeVisualizer_ && realTimeVisualizer_->isRunning()) {
        visualizers.push_back("real_time");
    }
    
    if (performanceMonitor_ && performanceMonitor_->isRunning()) {
        visualizers.push_back("performance_monitoring");
    }
    
    return visualizers;
}

void VisualizationManager::updateVisualizers() {
    // Update all visualizers with current brain state
    // Implementation details would include collecting data from each visualizer
    // and updating their internal state
}

} // namespace nlm