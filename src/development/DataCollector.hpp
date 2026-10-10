#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <deque>
#include <algorithm>

namespace nlm {

class DataCollector {
public:
    DataCollector(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                  std::shared_ptr<BrainSimulator> simulator);
    ~DataCollector();
    
    // Disable copying, enable moving
    DataCollector(const DataCollector&) = delete;
    DataCollector& operator=(const DataCollector&) = delete;
    DataCollector(DataCollector&&) noexcept = default;
    DataCollector& operator=(DataCollector&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Data collection API
    bool startDataCollection(const std::string& dataFile);
    bool stopDataCollection();
    bool pauseDataCollection();
    bool resumeDataCollection();
    
    // Data collection control
    bool isCollectingData() const;
    bool isDataCollectionPaused() const;
    std::string getDataFile() const;
    
    // Data collection types
    enum class DataType {
        NeuralData,
        SynapseData,
        MemoryData,
        PerformanceData,
        LearningData,
        ConnectivityData,
        AllData
    };
    
    // Data collection configuration
    struct DataCollectionConfig {
        std::string name;
        std::string description;
        std::vector<DataType> dataTypes;
        double collectionInterval;
        size_t maxDataPoints;
        std::string outputFormat;
        std::string outputFile;
        bool compressData;
        bool anonymizeData;
        
        DataCollectionConfig()
            : name("Data Collection"), description(""), collectionInterval(0.1),
              maxDataPoints(10000), outputFormat("csv"), outputFile("data.csv"),
              compressData(false), anonymizeData(false) {}
    };
    
    // Data collection management
    bool configureDataCollection(const DataCollectionConfig& config);
    DataCollectionConfig getCurrentDataCollectionConfig() const;
    
    // Data collection by type
    bool collectNeuralData();
    bool collectSynapseData();
    bool collectMemoryData();
    bool collectPerformanceData();
    bool collectLearningData();
    bool collectConnectivityData();
    bool collectAllData();
    
    // Data storage and management
    bool saveData(const std::string& filename);
    bool loadData(const std::string& filename);
    std::vector<DataPoint> getData(DataType type) const;
    std::vector<DataPoint> getData(const std::vector<DataType>& types) const;
    bool clearData();
    
    // Data analysis
    DataStatistics calculateStatistics(DataType type) const;
    std::vector<DataStatistics> calculateAllStatistics() const;
    DataTrends analyzeTrends(DataType type, double timeWindow) const;
    DataPatterns detectPatterns(DataType type) const;
    
    // Data filtering
    bool addDataFilter(const DataFilter& filter);
    bool removeDataFilter(const DataFilter& filter);
    bool clearDataFilters();
    std::vector<DataFilter> getDataFilters() const;
    
    // Data validation
    bool validateData(DataType type) const;
    bool validateDataCollection() const;
    std::vector<std::string> getDataValidationErrors() const;
    
    // Data export
    bool exportData(const std::string& filename, const std::string& format = "csv");
    bool exportDataAsJSON(const std::string& filename);
    bool exportDataAsBinary(const std::string& filename);
    bool exportDataAsXML(const std::string& filename);
    
    // Data compression
    bool compressData();
    bool decompressData();
    bool setCompressionLevel(int level);
    int getCompressionLevel() const;
    
    // Data anonymization
    bool anonymizeData();
    bool setAnonymizationThreshold(float threshold);
    float getAnonymizationThreshold() const;
    
    // Data monitoring
    void enableRealTimeDataMonitoring(bool enable);
    bool isRealTimeDataMonitoringEnabled() const;
    void setDataMonitoringCallback(std::function<void(const DataPoint&)> callback);
    
    // Thread safety
    void lockDataCollector() const;
    void unlockDataCollector() const;
    bool tryLockDataCollector() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex collectorMutex_;
    std::atomic<bool> isCollecting_;
    std::atomic<bool> isPaused_;
    std::thread collectionThread_;
    
    // Data collection configuration
    DataCollectionConfig currentConfig_;
    
    // Data storage
    std::unordered_map<DataType, std::deque<DataPoint>> dataStorage_;
    std::unordered_map<DataType, std::deque<DataPoint>> compressedDataStorage_;
    
    // Data filters
    std::vector<DataFilter> dataFilters_;
    
    // Data callbacks
    std::function<void(const DataPoint&)> dataMonitoringCallback_;
    
    // Performance monitoring
    double collectionStartTime_;
    double collectionEndTime_;
    double totalCollectionTime_;
    size_t totalDataPointsCollected_;
    
    // Internal helper methods
    void cleanup();
    void startDataCollectionInternal();
    void stopDataCollectionInternal();
    void pauseDataCollectionInternal();
    void resumeDataCollectionInternal();
    void collectDataInternal();
    void collectNeuralDataInternal();
    void collectSynapseDataInternal();
    void collectMemoryDataInternal();
    void collectPerformanceDataInternal();
    void collectLearningDataInternal();
    void collectConnectivityDataInternal();
    void collectAllDataInternal();
    DataPoint captureNeuralData() const;
    DataPoint captureSynapseData() const;
    DataPoint captureMemoryData() const;
    DataPoint capturePerformanceData() const;
    DataPoint captureLearningData() const;
    DataPoint captureConnectivityData() const;
    void processDataFilters(std::vector<DataPoint>& dataPoints) const;
    bool matchesFilter(const DataPoint& dataPoint, const DataFilter& filter) const;
    void compressDataInternal();
    void decompressDataInternal();
    void anonymizeDataInternal();
    DataStatistics calculateStatisticsInternal(const std::deque<DataPoint>& data) const;
    DataTrends analyzeTrendsInternal(const std::deque<DataPoint>& data, double timeWindow) const;
    DataPatterns detectPatternsInternal(const std::deque<DataPoint>& data) const;
    void logDataCollection(const std::string& message);
    void exportDataInternal() const;
    void validateDataCollectionInternal() const;
    void setupDataCollectionEnvironment();
    void cleanupDataCollectionEnvironment();
    void handleDataCollectionError(const std::string& error);
    void syncWithDebugger();
};

} // namespace nlm