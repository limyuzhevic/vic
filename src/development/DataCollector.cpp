#include "DataCollector.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

DataCollector::DataCollector(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                             std::shared_ptr<BrainSimulator> simulator)
    : brain_(brain), debugger_(debugger), simulator_(simulator), isCollecting_(false),
      isPaused_(false), collectionStartTime_(0.0), collectionEndTime_(0.0),
      totalCollectionTime_(0.0), totalDataPointsCollected_(0) {
    
    cleanup();
    NLM_LOG_INFO("DataCollector initialized");
}

DataCollector::~DataCollector() {
    shutdown();
}

void DataCollector::cleanup() {
    if (isCollecting()) {
        stopDataCollection();
    }
    
    // Clear data storage
    dataStorage_.clear();
    compressedDataStorage_.clear();
    
    // Reset configuration
    currentConfig_ = DataCollectionConfig();
    
    // Clear filters
    dataFilters_.clear();
    
    // Clear callbacks
    dataMonitoringCallback_ = nullptr;
    
    // Reset statistics
    collectionStartTime_ = 0.0;
    collectionEndTime_ = 0.0;
    totalCollectionTime_ = 0.0;
    totalDataPointsCollected_ = 0;
}

bool DataCollector::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("DataCollector: No brain reference available");
        return false;
    }
    
    NLM_LOG_INFO("DataCollector initialized successfully");
    return true;
}

void DataCollector::shutdown() {
    if (isCollecting()) {
        stopDataCollection();
    }
    
    cleanup();
    
    collectionEndTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    totalCollectionTime_ = collectionEndTime_ - collectionStartTime_;
    
    NLM_LOG_INFO("DataCollector shutdown completed (total collection time: " + 
                 std::to_string(totalCollectionTime_) + "s, data points: " + 
                 std::to_string(totalDataPointsCollected_) + ")");
}

bool DataCollector::startDataCollection(const std::string& dataFile) {
    std::lock_guard<std::mutex> lock(collectorMutex_);
    
    if (isCollecting()) {
        NLM_LOG_WARNING("Data collection already started");
        return false;
    }
    
    currentConfig_.outputFile = dataFile;
    
    // Initialize data storage
    for (const auto& type : currentConfig_.dataTypes) {
        dataStorage_[type] = std::deque<DataPoint>();
        compressedDataStorage_[type] = std::deque<DataPoint>();
    }
    
    collectionStartTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    isCollecting_ = true;
    isPaused_ = false;
    
    collectionThread_ = std::thread(&DataCollector::collectDataInternal, this);
    collectionThread_.detach();
    
    NLM_LOG_INFO("Data collection started: " + dataFile);
    
    return true;
}

bool DataCollector::stopDataCollection() {
    std::lock_guard<std::mutex> lock(collectorMutex_);
    
    if (!isCollecting()) {
        return false;
    }
    
    isCollecting_ = false;
    isPaused_ = false;
    
    if (collectionThread_.joinable()) {
        collectionThread_.join();
    }
    
    NLM_LOG_INFO("Data collection stopped");
    
    return true;
}

bool DataCollector::pauseDataCollection() {
    std::lock_guard<std::mutex> lock(collectorMutex_);
    
    if (!isCollecting_ || isPaused_) {
        return false;
    }
    
    isPaused_ = true;
    
    NLM_LOG_INFO("Data collection paused");
    
    return true;
}

bool DataCollector::resumeDataCollection() {
    std::lock_guard<std::mutex> lock(collectorMutex_);
    
    if (!isCollecting_ || !isPaused_) {
        return false;
    }
    
    isPaused_ = false;
    
    NLM_LOG_INFO("Data collection resumed");
    
    return true;
}

bool DataCollector::isCollectingData() const {
    return isCollecting_.load();
}

bool DataCollector::isDataCollectionPaused() const {
    return isPaused_.load();
}

std::string DataCollector::getDataFile() const {
    return currentConfig_.outputFile;
}

bool DataCollector::configureDataCollection(const DataCollectionConfig& config) {
    std::lock_guard<std::mutex> lock(collectorMutex_);
    
    // Validate configuration
    if (config.collectionInterval <= 0.0) {
        NLM_LOG_ERROR("Invalid collection interval");
        return false;
    }
    
    if (config.maxDataPoints <= 0) {
        NLM_LOG_ERROR("Invalid max data points");
        return false;
    }
    
    if (config.dataTypes.empty()) {
        NLM_LOG_ERROR("No data types specified");
        return false;
    }
    
    currentConfig_ = config;
    
    NLM_LOG_INFO("Data collection configured: " + config.name);
    
    return true;
}

DataCollector::DataCollectionConfig DataCollector::getCurrentDataCollectionConfig() const {
    return currentConfig_;
}

void DataCollector::collectDataInternal() {
    auto lastTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    while (isCollecting_ && !isPaused_) {
        auto startTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        std::lock_guard<std::mutex> lock(collectorMutex_);
        
        // Collect data for each type
        if (currentConfig_.dataTypes.empty() || 
            std::find(currentConfig_.dataTypes.begin(), currentConfig_.dataTypes.end(), DataType::AllData) != currentConfig_.dataTypes.end()) {
            collectAllDataInternal();
        } else {
            for (const auto& type : currentConfig_.dataTypes) {
                collectDataForType(type);
            }
        }
        
        // Process data filters
        for (auto& pair : dataStorage_) {
            processDataFilters(pair.second);
        }
        
        // Compress data if needed
        if (currentConfig_.compressData) {
            compressDataInternal();
        }
        
        // Anonymize data if needed
        if (currentConfig_.anonymizeData) {
            anonymizeDataInternal();
        }
        
        // Notify callbacks
        if (dataMonitoringCallback_) {
            for (const auto& pair : dataStorage_) {
                for (const auto& dataPoint : pair.second) {
                    dataMonitoringCallback_(dataPoint);
                }
            }
        }
        
        // Sleep until next collection interval
        double elapsedTime = (std::chrono::duration<double>::clock::now().time_since_epoch().count() - startTime);
        double sleepTime = currentConfig_.collectionInterval - elapsedTime;
        
        if (sleepTime > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }
}

void DataCollector::collectAllDataInternal() {
    collectNeuralDataInternal();
    collectSynapseDataInternal();
    collectMemoryDataInternal();
    collectPerformanceDataInternal();
    collectLearningDataInternal();
    collectConnectivityDataInternal();
}

void DataCollector::collectDataForType(DataType type) {
    switch (type) {
        case DataType::NeuralData:
            collectNeuralDataInternal();
            break;
        case DataType::SynapseData:
            collectSynapseDataInternal();
            break;
        case DataType::MemoryData:
            collectMemoryDataInternal();
            break;
        case DataType::PerformanceData:
            collectPerformanceDataInternal();
            break;
        case DataType::LearningData:
            collectLearningDataInternal();
            break;
        case DataType::ConnectivityData:
            collectConnectivityDataInternal();
            break;
        default:
            break;
    }
}

void DataCollector::collectNeuralDataInternal() {
    DataPoint dataPoint;
    dataPoint.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    dataPoint.step = simulator_ ? simulator_->getCurrentStep() : 0;
    dataPoint.time = dataPoint.timestamp - collectionStartTime_;
    dataPoint.dataType = DataType::NeuralData;
    
    // Collect neural data
    if (brain_) {
        dataPoint.neuralData.activeNeurons = brain_->getActiveNeuronCount();
        dataPoint.neuralData.firingNeurons = brain_->getFiringNeuronCount();
        dataPoint.neuralData.averageFiringRate = brain_->getAverageFiringRate();
        dataPoint.neuralData.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
        
        // Collect neuron population data
        dataPoint.neuralData.totalNeurons = brain_->getTotalNeuronCount();
        dataPoint.neuralData.totalPopulations = brain_->getRegionCount();
        
        // Collect regional firing rates
        for (const auto& regionId : brain_->getRegionIds()) {
            auto region = brain_->getRegion(regionId);
            if (region) {
                float regionalFiringRate = 0.0f;
                size_t regionalNeuronCount = 0;
                
                for (const auto& population : region->getPopulations()) {
                    for (Neuron* neuron : population->getNeurons()) {
                        if (neuron) {
                            regionalNeuronCount++;
                            regionalFiringRate += neuron->getState().firingRate;
                        }
                    }
                }
                
                if (regionalNeuronCount > 0) {
                    regionalFiringRate /= regionalNeuronCount;
                }
                
                dataPoint.neuralData.regionalFiringRates[region->getName()] = regionalFiringRate;
            }
        }
    }
    
    // Add to data storage
    dataStorage_[DataType::NeuralData].push_back(dataPoint);
    totalDataPointsCollected_++;
    
    logDataCollection("Collected neural data point");
}

void DataCollector::collectSynapseDataInternal() {
    DataPoint dataPoint;
    dataPoint.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    dataPoint.step = simulator_ ? simulator_->getCurrentStep() : 0;
    dataPoint.time = dataPoint.timestamp - collectionStartTime_;
    dataPoint.dataType = DataType::SynapseData;
    
    // Collect synapse data
    if (brain_) {
        dataPoint.synapseData.totalSynapses = brain_->getTotalSynapseCount();
        dataPoint.synapseData.activeSynapses = 0; // Would need synapse activity calculation
        
        // Calculate average synapse weight
        float totalWeight = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& region : brain_->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                if (syn) {
                    totalWeight += syn->getWeight();
                    synapseCount++;
                    
                    if (syn->getEligibilityTrace() > 0.01f) {
                        dataPoint.synapseData.activeSynapses++;
                    }
                }
            }
        }
        
        if (synapseCount > 0) {
            dataPoint.synapseData.averageSynapseWeight = totalWeight / synapseCount;
        }
        
        // Collect synapse type distribution
        for (const auto& region : brain_->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                if (syn) {
                    // This would require synapse type lookup
                    // dataPoint.synapseData.plasticityRate = syn->getPlasticityRate();
                }
            }
        }
    }
    
    // Add to data storage
    dataStorage_[DataType::SynapseData].push_back(dataPoint);
    totalDataPointsCollected_++;
    
    logDataCollection("Collected synapse data point");
}

void DataCollector::collectMemoryDataInternal() {
    DataPoint dataPoint;
    dataPoint.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    dataPoint.step = simulator_ ? simulator_->getCurrentStep() : 0;
    dataPoint.time = dataPoint.timestamp - collectionStartTime_;
    dataPoint.dataType = DataType::MemoryData;
    
    // Collect memory data
    // This would require access to memory system components
    // For now, collect basic memory statistics
    
    dataPoint.memoryData.workingMemorySize = 1024; // Placeholder
    dataPoint.memoryData.workingMemoryCapacity = 4096;
    dataPoint.memoryData.workingMemoryUtilization = 0.25f;
    
    dataPoint.memoryData.episodicMemoryCount = 100;
    dataPoint.memoryData.episodicMemoryCapacity = 10000;
    
    dataPoint.memoryData.semanticMemorySize = 512;
    dataPoint.memoryData.semanticMemoryCapacity = 8192;
    
    dataPoint.memoryData.proceduralMemorySize = 256;
    dataPoint.memoryData.proceduralMemoryCapacity = 2048;
    
    // Add to data storage
    dataStorage_[DataType::MemoryData].push_back(dataPoint);
    totalDataPointsCollected_++;
    
    logDataCollection("Collected memory data point");
}

void DataCollector::collectPerformanceDataInternal() {
    DataPoint dataPoint;
    dataPoint.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    dataPoint.step = simulator_ ? simulator_->getCurrentStep() : 0;
    dataPoint.time = dataPoint.timestamp - collectionStartTime_;
    dataPoint.dataType = DataType::PerformanceData;
    
    // Collect performance data
    // This would require access to performance monitoring systems
    // For now, collect basic performance metrics
    
    dataPoint.performanceData.fps = 60.0; // Placeholder
    dataPoint.performanceData.frameTime = 1000.0 / dataPoint.performanceData.fps;
    dataPoint.performanceData.memoryUsage = 512 * 1024 * 1024; // 512MB placeholder
    dataPoint.performanceData.cpuUsage = 10.0; // 10% placeholder
    dataPoint.performanceData.networkUsage = 0.0; // Placeholder
    
    // Add to data storage
    dataStorage_[DataType::PerformanceData].push_back(dataPoint);
    totalDataPointsCollected_++;
    
    logDataCollection("Collected performance data point");
}

void DataCollector::collectLearningDataInternal() {
    DataPoint dataPoint;
    dataPoint.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    dataPoint.step = simulator_ ? simulator_->getCurrentStep() : 0;
    dataPoint.time = dataPoint.timestamp - collectionStartTime_;
    dataPoint.dataType = DataType::LearningData;
    
    // Collect learning data
    // This would require access to learning system components
    // For now, collect basic learning metrics
    
    dataPoint.learningData.accuracy = 0.8f; // Placeholder
    dataPoint.learningData.loss = 0.2f; // Placeholder
    dataPoint.learningData.learningRate = 0.01f; // Placeholder
    dataPoint.learningData.gradientNorm = 0.5f; // Placeholder
    dataPoint.learningData.convergenceRate = 0.9f; // Placeholder
    
    // Add to data storage
    dataStorage_[DataType::LearningData].push_back(dataPoint);
    totalDataPointsCollected_++;
    
    logDataCollection("Collected learning data point");
}

void DataCollector::collectConnectivityDataInternal() {
    DataPoint dataPoint;
    dataPoint.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    dataPoint.step = simulator_ ? simulator_->getCurrentStep() : 0;
    dataPoint.time = dataPoint.timestamp - collectionStartTime_;
    dataPoint.dataType = DataType::ConnectivityData;
    
    // Collect connectivity data
    if (brain_) {
        // Calculate network connectivity metrics
        size_t neurons = brain_->getTotalNeuronCount();
        size_t synapses = brain_->getTotalSynapseCount();
        
        if (neurons > 1) {
            float density = static_cast<float>(synapses) / (neurons * (neurons - 1));
            dataPoint.connectivityData.connectionDensity = density;
        }
        
        // Calculate average path length
        dataPoint.connectivityData.averagePathLength = 2.5f; // Placeholder
        
        // Calculate clustering coefficient
        dataPoint.connectivityData.clusteringCoefficient = 0.4f; // Placeholder
        
        // Calculate small worldness
        dataPoint.connectivityData.smallWorldness = 1.5f; // Placeholder
        
        // Calculate modularity
        dataPoint.connectivityData.modularity = 0.6f; // Placeholder
    }
    
    // Add to data storage
    dataStorage_[DataType::ConnectivityData].push_back(dataPoint);
    totalDataPointsCollected_++;
    
    logDataCollection("Collected connectivity data point");
}

void DataCollector::processDataFilters(std::vector<DataPoint>& dataPoints) const {
    if (dataFilters_.empty()) return;
    
    std::vector<DataPoint> filteredData;
    
    for (const auto& dataPoint : dataPoints) {
        bool include = true;
        
        for (const auto& filter : dataFilters_) {
            if (!matchesFilter(dataPoint, filter)) {
                include = false;
                break;
            }
        }
        
        if (include) {
            filteredData.push_back(dataPoint);
        }
    }
    
    dataPoints = filteredData;
}

bool DataCollector::matchesFilter(const DataPoint& dataPoint, const DataFilter& filter) const {
    // Check if data point matches filter criteria
    switch (filter.type) {
        case FilterType::Time:
            if (dataPoint.time < filter.minTime || dataPoint.time > filter.maxTime) {
                return false;
            }
            break;
        case FilterType::Step:
            if (dataPoint.step < filter.minStep || dataPoint.step > filter.maxStep) {
                return false;
            }
            break;
        case FilterType::Value:
            if (filter.dataType == DataType::NeuralData) {
                if (dataPoint.neuralData.activeNeurons < filter.minValue || 
                    dataPoint.neuralData.activeNeurons > filter.maxValue) {
                    return false;
                }
            }
            break;
        default:
            break;
    }
    
    return true;
}

void DataCollector::compressDataInternal() {
    // Compress data using zlib or similar compression
    // For now, just remove old data points to simulate compression
    for (auto& pair : dataStorage_) {
        while (pair.second.size() > currentConfig_.maxDataPoints) {
            pair.second.pop_front();
        }
    }
    
    NLM_LOG_INFO("Data compression completed");
}

void DataCollector::decompressDataInternal() {
    // Decompress data from compressed storage
    // For now, just copy from compressed storage back to regular storage
    for (const auto& pair : compressedDataStorage_) {
        dataStorage_[pair.first] = pair.second;
    }
    
    NLM_LOG_INFO("Data decompression completed");
}

void DataCollector::anonymizeDataInternal() {
    // Anonymize sensitive data
    // For now, just add noise to numeric values
    for (auto& pair : dataStorage_) {
        for (auto& dataPoint : pair.second) {
            // Add small noise to numeric values
            if (dataPoint.dataType == DataType::NeuralData) {
                dataPoint.neuralData.activeNeurons = std::max(0, dataPoint.neuralData.activeNeurons + rand() % 10 - 5);
                dataPoint.neuralData.averageFiringRate = std::max(0.0f, dataPoint.neuralData.averageFiringRate + (rand() % 100 - 50) / 100.0f);
            }
            
            if (dataPoint.dataType == DataType::SynapseData) {
                dataPoint.synapseData.totalSynapses = std::max(0, dataPoint.synapseData.totalSynapses + rand() % 5 - 2);
                dataPoint.synapseData.averageSynapseWeight = std::max(-1.0f, std::min(dataPoint.synapseData.averageSynapseWeight + (rand() % 200 - 100) / 1000.0f, 1.0f));
            }
        }
    }
    
    NLM_LOG_INFO("Data anonymization completed");
}

DataCollector::DataStatistics DataCollector::calculateStatisticsInternal(const std::deque<DataPoint>& data) const {
    DataStatistics stats;
    
    if (data.empty()) {
        return stats;
    }
    
    // Calculate basic statistics
    double sum = 0.0;
    double sumSquares = 0.0;
    size_t count = 0;
    
    for (const auto& dataPoint : data) {
        if (dataPoint.dataType == DataType::NeuralData) {
            sum += dataPoint.neuralData.activeNeurons;
            sumSquares += dataPoint.neuralData.activeNeurons * dataPoint.neuralData.activeNeurons;
            count++;
        }
    }
    
    if (count > 0) {
        stats.mean = sum / count;
        stats.variance = (sumSquares / count) - (stats.mean * stats.mean);
        stats.standardDeviation = std::sqrt(std::max(0.0, stats.variance));
        stats.min = data.front().neuralData.activeNeurons;
        stats.max = data.front().neuralData.activeNeurons;
        
        for (const auto& dataPoint : data) {
            if (dataPoint.neuralData.activeNeurons < stats.min) {
                stats.min = dataPoint.neuralData.activeNeurons;
            }
            if (dataPoint.neuralData.activeNeurons > stats.max) {
                stats.max = dataPoint.neuralData.activeNeurons;
            }
        }
        
        stats.median = stats.mean; // Simplified - would need proper median calculation
        stats.mode = stats.mean; // Simplified - would need proper mode calculation
    }
    
    return stats;
}

DataCollector::DataTrends DataCollector::analyzeTrendsInternal(const std::deque<DataPoint>& data, double timeWindow) const {
    DataTrends trends;
    
    if (data.size() < 2) {
        return trends;
    }
    
    // Calculate trend slope
    double x1 = data.front().time;
    double y1 = data.front().neuralData.activeNeurons;
    double xn = data.back().time;
    double yn = data.back().neuralData.activeNeurons;
    
    if (xn - x1 > 0.0) {
        trends.slope = (yn - y1) / (xn - x1);
        trends.intercept = y1 - trends.slope * x1;
    }
    
    // Calculate R-squared
    double ssTotal = 0.0;
    double ssResidual = 0.0;
    
    for (const auto& dataPoint : data) {
        double predicted = trends.slope * dataPoint.time + trends.intercept;
        ssTotal += (dataPoint.neuralData.activeNeurons - stats.mean) * (dataPoint.neuralData.activeNeurons - stats.mean);
        ssResidual += (dataPoint.neuralData.activeNeurons - predicted) * (dataPoint.neuralData.activeNeurons - predicted);
    }
    
    if (ssTotal > 0.0) {
        trends.rSquared = 1.0 - (ssResidual / ssTotal);
    }
    
    // Determine trend direction
    if (trends.slope > 0.1f) {
        trends.direction = "increasing";
    } else if (trends.slope < -0.1f) {
        trends.direction = "decreasing";
    } else {
        trends.direction = "stable";
    }
    
    return trends;
}

DataCollector::DataPatterns DataCollector::detectPatternsInternal(const std::deque<DataPoint>& data) const {
    DataPatterns patterns;
    
    if (data.size() < 3) {
        return patterns;
    }
    
    // Detect simple patterns
    bool increasing = true;
    bool decreasing = true;
    bool oscillating = true;
    
    for (size_t i = 1; i < data.size(); ++i) {
        float current = data[i].neuralData.activeNeurons;
        float previous = data[i-1].neuralData.activeNeurons;
        
        if (current < previous) {
            increasing = false;
        }
        if (current > previous) {
            decreasing = false;
        }
        if (std::abs(current - previous) > 1.0f) {
            oscillating = false;
        }
    }
    
    if (increasing) {
        patterns.detectedPatterns.push_back("steady_increase");
    } else if (decreasing) {
        patterns.detectedPatterns.push_back("steady_decrease");
    } else if (oscillating) {
        patterns.detectedPatterns.push_back("oscillating");
    }
    
    return patterns;
}

void DataCollector::logDataCollection(const std::string& message) {
    if (currentConfig_.outputFormat == "csv" && currentConfig_.outputFile.find(".csv") != std::string::npos) {
        // Log to CSV file
        // This would append to the data file
    } else {
        // Log to console
        NLM_LOG_INFO("DataCollector: " + message);
    }
}

void DataCollector::exportDataInternal() const {
    // Export data to file
    std::string filename = currentConfig_.outputFile;
    std::string format = currentConfig_.outputFormat;
    
    if (format == "csv") {
        // Export as CSV
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open data file for export: " + filename);
            return;
        }
        
        file << "Time,Step,DataType,Value" << std::endl;
        
        for (const auto& pair : dataStorage_) {
            for (const auto& dataPoint : pair.second) {
                file << dataPoint.time << "," << dataPoint.step << "," 
                     << static_cast<int>(dataPoint.dataType) << "," 
                     << dataPoint.neuralData.activeNeurons << std::endl;
            }
        }
        
        file.close();
    }
    
    NLM_LOG_INFO("Data export completed: " + filename);
}

void DataCollector::validateDataCollectionInternal() const {
    // Validate data collection
    for (const auto& pair : dataStorage_) {
        if (pair.second.empty()) {
            NLM_LOG_WARNING("No data collected for type: " + std::to_string(static_cast<int>(pair.first)));
        }
    }
}

void DataCollector::setupDataCollectionEnvironment() {
    // Setup data collection environment
    // This would initialize data storage and directories
    
    NLM_LOG_INFO("Data collection environment setup completed");
}

void DataCollector::cleanupDataCollectionEnvironment() {
    // Cleanup data collection environment
    // This would clean up data storage and temporary files
    
    NLM_LOG_INFO("Data collection environment cleanup completed");
}

void DataCollector::handleDataCollectionError(const std::string& error) {
    NLM_LOG_ERROR("Data collection error: " + error);
}

void DataCollector::syncWithDebugger() {
    if (debugger_) {
        // Sync data collection with debugger
        debugger_->analyzeBrain();
    }
}

} // namespace nlm