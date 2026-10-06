#include "MetricRegistry.hpp"
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace nlm {

MetricRegistry::MetricRegistry() : customMetricCounter_(0) {}

bool MetricRegistry::registerBuiltInMetric(const std::string& name,
                                          MetricCategory category,
                                          const std::string& description,
                                          MetricCollector collector) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (metrics_.find(name) != metrics_.end()) {
        return false; // Metric already exists
    }
    
    MetricEntry entry(name, category, description, std::move(collector), true);
    metrics_[name] = entry;
    builtInMetrics_[name] = entry;
    
    return true;
}

bool MetricRegistry::registerCustomMetric(const std::string& name,
                                         const std::string& categoryStr,
                                         const std::string& description,
                                         MetricCollector collector,
                                         const std::unordered_map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (metrics_.find(name) != metrics_.end()) {
        return false; // Metric already exists
    }
    
    // Determine category from string
    MetricCategory category = MetricCategory::CUSTOM;
    if (categoryStr == "NEURAL_PERFORMANCE") category = MetricCategory::NEURAL_PERFORMANCE;
    else if (categoryStr == "MEMORY_PERFORMANCE") category = MetricCategory::MEMORY_PERFORMANCE;
    else if (categoryStr == "COGNITIVE_PERFORMANCE") category = MetricCategory::COGNITIVE_PERFORMANCE;
    else if (categoryStr == "SYSTEM_PERFORMANCE") category = MetricCategory::SYSTEM_PERFORMANCE;
    else if (categoryStr == "BEHAVIOR_PERFORMANCE") category = MetricCategory::BEHAVIOR_PERFORMANCE;
    
    MetricEntry entry(name, category, description, std::move(collector), false);
    entry.tags = tags;
    metrics_[name] = entry;
    customMetrics_[name] = entry;
    
    return true;
}

MetricCollector* MetricRegistry::getCollector(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = metrics_.find(name);
    if (it == metrics_.end()) {
        return nullptr;
    }
    
    return &it->second.collector;
}

const MetricData* MetricRegistry::getMetricInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = metrics_.find(name);
    if (it == metrics_.end()) {
        return nullptr;
    }
    
    // Create temporary MetricData for return
    // In a real implementation, we might want to cache this
    static thread_local MetricData tempData("", "", "", 0.0);
    const auto& entry = it->second;
    tempData.name = entry.name;
    tempData.category = metricCategoryToString(entry.category);
    tempData.description = entry.description;
    tempData.tags = entry.tags;
    
    return &tempData;
}

std::vector<MetricData> MetricRegistry::collectAllMetrics() const {
    std::vector<MetricData> results;
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : metrics_) {
        const auto& entry = pair.second;
        if (entry.collector) {
            try {
                double value = entry.collector();
                MetricData data(entry.name, metricCategoryToString(entry.category), 
                              entry.description, value);
                data.tags = entry.tags;
                results.push_back(data);
            } catch (const std::exception& e) {
                // Skip metrics that throw exceptions
                continue;
            }
        }
    }
    
    return results;
}

std::vector<const MetricData*> MetricRegistry::getMetricsByCategory(MetricCategory category) const {
    std::vector<const MetricData*> results;
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : metrics_) {
        const auto& entry = pair.second;
        if (entry.category == category) {
            // Create temporary MetricData for return
            static thread_local MetricData tempData("", "", "", 0.0);
            tempData.name = entry.name;
            tempData.category = metricCategoryToString(entry.category);
            tempData.description = entry.description;
            tempData.tags = entry.tags;
            results.push_back(&tempData);
        }
    }
    
    return results;
}

std::vector<std::string> MetricRegistry::getMetricNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(metrics_.size());
    
    for (const auto& pair : metrics_) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool MetricRegistry::unregisterMetric(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = metrics_.find(name);
    if (it == metrics_.end()) {
        return false;
    }
    
    const auto& entry = it->second;
    metrics_.erase(it);
    
    if (entry.isBuiltIn) {
        builtInMetrics_.erase(name);
    } else {
        customMetrics_.erase(name);
    }
    
    return true;
}

bool MetricRegistry::hasMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_.find(name) != metrics_.end();
}

void MetricRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.clear();
    builtInMetrics_.clear();
    customMetrics_.clear();
    customMetricCounter_ = 0;
}

std::string MetricRegistry::exportToJSON() const {
    std::lock_guard<std::mutex> lock(mutex_);
    json j;
    
    json metricsArray = json::array();
    for (const auto& pair : metrics_) {
        const auto& entry = pair.second;
        json metricJson;
        metricJson["name"] = entry.name;
        metricJson["category"] = metricCategoryToString(entry.category);
        metricJson["description"] = entry.description;
        metricJson["isBuiltIn"] = entry.isBuiltIn;
        
        if (!entry.tags.empty()) {
            metricJson["tags"] = entry.tags;
        }
        
        metricsArray.push_back(metricJson);
    }
    
    j["metrics"] = metricsArray;
    j["count"] = metrics_.size();
    j["builtInCount"] = builtInMetrics_.size();
    j["customCount"] = customMetrics_.size();
    
    return j.dump(2);
}

bool MetricRegistry::importFromJSON(const std::string& jsonData) {
    try {
        json j = json::parse(jsonData);
        
        clear();
        
        if (j.contains("metrics") && j["metrics"].is_array()) {
            for (const auto& metricJson : j["metrics"]) {
                std::string name = metricJson["name"].get<std::string>();
                std::string categoryStr = metricJson["category"].get<std::string>();
                std::string description = metricJson["description"].get<std::string>();
                bool isBuiltIn = metricJson.value("isBuiltIn", false);
                
                std::unordered_map<std::string, std::string> tags;
                if (metricJson.contains("tags") && metricJson["tags"].is_object()) {
                    tags = metricJson["tags"].get<std::unordered_map<std::string, std::string>>();
                }
                
                // For imported metrics, we can't create the collector without a function
                // So we'll just register the metadata
                MetricEntry entry(name, MetricCategory::CUSTOM, description,
                                nullptr, isBuiltIn);
                entry.tags = tags;
                metrics_[name] = entry;
                
                if (isBuiltIn) {
                    builtInMetrics_[name] = entry;
                } else {
                    customMetrics_[name] = entry;
                }
            }
        }
        
        return true;
    } catch (const json::exception& e) {
        return false;
    }
}

std::string MetricRegistry::metricCategoryToString(MetricCategory category) const {
    switch (category) {
        case MetricCategory::NEURAL_PERFORMANCE: return "NEURAL_PERFORMANCE";
        case MetricCategory::MEMORY_PERFORMANCE: return "MEMORY_PERFORMANCE";
        case MetricCategory::COGNITIVE_PERFORMANCE: return "COGNITIVE_PERFORMANCE";
        case MetricCategory::SYSTEM_PERFORMANCE: return "SYSTEM_PERFORMANCE";
        case MetricCategory::BEHAVIOR_PERFORMANCE: return "BEHAVIOR_PERFORMANCE";
        case MetricCategory::CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

} // namespace nlm
