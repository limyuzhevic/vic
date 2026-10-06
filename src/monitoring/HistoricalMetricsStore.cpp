#include "HistoricalMetricsStore.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <zlib.h>  // For compression
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

HistoricalMetricsStore::HistoricalMetricsStore(const StorageConfig& config)
    : config_(config), cacheSize_(10000) // Cache up to 10,000 samples
{
    ensureDirectoryExists();
    if (config_.enableArchival) {
        ensureDirectoryExists(); // Create archive directory if needed
    }
}

HistoricalMetricsStore::~HistoricalMetricsStore() = default;

bool HistoricalMetricsStore::ensureDirectoryExists() const {
    try {
        if (!std::filesystem::exists(config_.directory)) {
            std::filesystem::create_directories(config_.directory);
        }
        
        if (config_.enableArchival && !std::filesystem::exists(config_.archivePath)) {
            std::filesystem::create_directories(config_.archivePath);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool HistoricalMetricsStore::storeMetrics(const std::vector<MetricData>& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Add to memory cache
        for (const auto& metric : metrics) {
            MetricSample sample(metric.name, metric.value, metric.tags);
            memoryCache_.push_back(sample);
        }
        
        // Keep cache size reasonable
        if (memoryCache_.size() > cacheSize_) {
            size_t removeCount = memoryCache_.size() - cacheSize_ / 2;
            memoryCache_.erase(memoryCache_.begin(), memoryCache_.begin() + removeCount);
        }
        
        // Write to file if configured
        if (config_.fileFormat != "memory_only") {
            std::string filename = generateFilename();
            std::string jsonData;
            
            if (config_.fileFormat == "json" || config_.fileFormat == "mixed") {
                jsonData = exportToJSON();
                if (!jsonData.empty() && !writeToFile(filename, jsonData)) {
                    return false;
                }
            }
            
            if (config_.enableCompression && !jsonData.empty()) {
                compressFile(filename);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool HistoricalMetricsStore::storeSample(const MetricSample& sample) {
    return storeMetrics(std::vector<MetricData>(1, MetricData(sample.metricName, 0.0, "", sample.value)));
}

std::vector<MetricData> HistoricalMetricsStore::retrieveMetrics(
    const std::string& metricName,
    std::chrono::system_clock::time_point start,
    std::chrono::system_clock::time_point end) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MetricData> result;
    
    // Search in memory cache
    for (const auto& sample : memoryCache_) {
        if ((metricName.empty() || sample.metricName == metricName) &&
            sample.timestamp >= start && sample.timestamp <= end) {
            result.emplace_back(sample.metricName, sample.value, "", 0.0);
            result.back().timestamp = sample.timestamp;
        }
    }
    
    // TODO: Search in files when implemented
    
    return result;
}

std::vector<MetricSample> HistoricalMetricsStore::getAllSamples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryCache_;
}

std::vector<MetricData> HistoricalMetricsStore::getMetricsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MetricData> result;
    
    for (const auto& sample : memoryCache_) {
        // Note: MetricData doesn't store category, so this is a simplified implementation
        // In a real implementation, you'd need to store category in MetricData
        result.emplace_back(sample.metricName, sample.value, "", 0.0);
        result.back().timestamp = sample.timestamp;
    }
    
    return result;
}

std::string HistoricalMetricsStore::exportToJSON(
    const std::string& metricName,
    std::chrono::system_clock::time_point start,
    std::chrono::system_clock::time_point end) const {
    
    auto metrics = retrieveMetrics(metricName, start, end);
    
    json j = json::array();
    for (const auto& metric : metrics) {
        json metricJson;
        metricJson["name"] = metric.name;
        metricJson["value"] = metric.value;
        metricJson["timestamp"] = std::chrono::system_clock::to_time_t(metric.timestamp);
        
        // Add tags if present
        if (!metric.tags.empty()) {
            metricJson["tags"] = metric.tags;
        }
        
        j.push_back(metricJson);
    }
    
    return j.dump(2);
}

std::string HistoricalMetricsStore::exportToCSV(
    const std::string& metricName,
    std::chrono::system_clock::time_point start,
    std::chrono::system_clock::time_point end) const {
    
    auto metrics = retrieveMetrics(metricName, start, end);
    
    std::stringstream csv;
    csv << "timestamp,metric_name,value,tags\n";
    
    for (const auto& metric : metrics) {
        csv << std::chrono::system_clock::to_time_t(metric.timestamp) << ","
            << metric.name << ","
            << metric.value << ",";
        
        // Add tags
        if (!metric.tags.empty()) {
            bool first = true;
            for (const auto& tag : metric.tags) {
                if (!first) csv << ";";
                csv << tag.first << "=" << tag.second;
                first = false;
            }
        }
        
        csv << "\n";
    }
    
    return csv.str();
}

bool HistoricalMetricsStore::importFromJSON(const std::string& data) {
    try {
        json j = json::parse(data);
        
        for (const auto& item : j) {
            MetricSample sample(
                item["name"].get<std::string>(),
                item["value"].get<double>()
            );
            
            // Parse timestamp
            if (item.contains("timestamp")) {
                auto timestamp = std::chrono::system_clock::from_time_t(
                    item["timestamp"].get<std::time_t>()
                );
                // Note: This is a simplified timestamp parsing
                // In a real implementation, you'd need to handle the full timestamp format
            }
            
            // Parse tags
            if (item.contains("tags") && item["tags"].is_object()) {
                sample.tags = item["tags"].get<std::unordered_map<std::string, std::string>>();
            }
            
            storeSample(sample);
        }
        
        return true;
    } catch (const json::exception& e) {
        return false;
    }
}

bool HistoricalMetricsStore::importFromCSV(const std::string& data) {
    // TODO: Implement CSV import
    return false;
}

void HistoricalMetricsStore::cleanupOldData() {
    if (config_.maxHistoryDays <= 0) {
        return; // No cleanup needed
    }
    
    auto now = std::chrono::system_clock::now();
    auto cutoffTime = now - std::chrono::hours(24 * config_.maxHistoryDays);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Remove old samples from memory cache
    auto newEnd = std::remove_if(memoryCache_.begin(), memoryCache_.end(),
                                 [cutoffTime](const MetricSample& sample) {
                                     return sample.timestamp < cutoffTime;
                                 });
    memoryCache_.erase(newEnd, memoryCache_.end());
    
    // TODO: Clean old files from disk storage
}

HistoricalMetricsStore::StorageStats HistoricalMetricsStore::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    StorageStats stats;
    stats.totalSamples = memoryCache_.size();
    
    // Calculate disk usage
    try {
        for (const auto& entry : fileTimestamps_) {
            if (std::filesystem::exists(entry.first)) {
                stats.diskUsage += std::filesystem::file_size(entry.first);
                stats.totalFiles++;
            }
        }
    } catch (const std::exception& e) {
        // If we can't get file sizes, disk usage will be zero
    }
    
    // Find oldest and newest data
    if (!memoryCache_.empty()) {
        auto oldest = std::min_element(memoryCache_.begin(), memoryCache_.end(),
                                       [](const MetricSample& a, const MetricSample& b) {
                                           return a.timestamp < b.timestamp;
                                       });
        auto newest = std::max_element(memoryCache_.begin(), memoryCache_.end(),
                                       [](const MetricSample& a, const MetricSample& b) {
                                           return a.timestamp < b.timestamp;
                                       });
        
        stats.oldestData = oldest->timestamp;
        stats.newestData = newest->timestamp;
    }
    
    return stats;
}

std::string HistoricalMetricsStore::generateFilename(const std::string& prefix,
                                                     const std::string& extension) const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream filename;
    filename << config_.directory << "/" << prefix << "_";
    filename << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S");
    filename << "." << extension;
    
    return filename.str();
}

bool HistoricalMetricsStore::writeToFile(const std::string& filename, const std::string& data) const {
    try {
        std::ofstream file(filename, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        
        file << data;
        file.close();
        
        fileTimestamps_[filename] = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string HistoricalMetricsStore::readFromFile(const std::string& filename) const {
    try {
        std::ifstream file(filename, std::ios::in);
        if (!file.is_open()) {
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return buffer.str();
    } catch (const std::exception& e) {
        return "";
    }
}

bool HistoricalMetricsStore::compressFile(const std::string& filename) const {
    std::string uncompressed = readFromFile(filename);
    if (uncompressed.empty()) {
        return false;
    }
    
    std::string compressedFilename = filename + ".gz";
    
    try {
        std::ofstream outFile(compressedFilename, std::ios::binary | std::ios::trunc);
        if (!outFile.is_open()) {
            return false;
        }
        
        // Simple gzip compression (would use zlib in a real implementation)
        outFile.write(uncompressed.c_str(), uncompressed.size());
        outFile.close();
        
        // Remove uncompressed file
        std::remove(filename.c_str());
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool HistoricalMetricsStore::archiveOldFiles() {
    if (!config_.enableArchival) {
        return true;
    }
    
    auto now = std::chrono::system_clock::now();
    auto cutoffTime = now - std::chrono::hours(24 * config_.maxHistoryDays);
    
    try {
        for (const auto& entry : fileTimestamps_) {
            if (entry.second < cutoffTime && std::filesystem::exists(entry.first)) {
                std::string archivePath = config_.archivePath + "/" + 
                                         std::filesystem::path(entry.first).filename().string();
                std::filesystem::copy_file(entry.first, archivePath);
                std::filesystem::remove(entry.first.c_str());
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace nlm
