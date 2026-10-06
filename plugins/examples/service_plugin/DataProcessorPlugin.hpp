#pragma once

#include "Service.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

class DataProcessorService : public Service {
public:
    DataProcessorService() : initialized_(false) {}
    ~DataProcessorService() override { shutdown(); }
    
    const ServiceInfo& getInfo() const override {
        static ServiceInfo info = {
            .name = "DataProcessor",
            .description = "Processes and analyzes data",
            .type = ServiceType::Data,
            .interfaceType = typeid(std::vector<float>),
            .isSingleton = true
        };
        return info;
    }
    
    bool initialize() override {
        if (initialized_) return true;
        
        NLM_LOG_INFO("DataProcessorService initialized");
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        if (!initialized_) return;
        
        NLM_LOG_INFO("DataProcessorService shutdown");
        initialized_ = false;
    }
    
    bool isInitialized() const override {
        return initialized_;
    }
    
    bool handleRequest(const ServiceRequest& request, ServiceResponse& response) override {
        NLM_LOG_INFO(std::string("DataProcessorService handling request: ") + request.operation);
        
        if (request.serviceName != "DataProcessor") {
            response.success = false;
            response.errorMessage = "Invalid service name";
            return false;
        }
        
        if (request.operation == "analyze") {
            // Analyze input data
            std::vector<float> result;
            if (request.parameters.count("data")) {
                auto& data = std::any_cast<std::vector<float>&>(request.parameters.at("data"));
                
                // Simple analysis: calculate mean
                if (!data.empty()) {
                    float sum = 0.0f;
                    for (float value : data) {
                        sum += value;
                    }
                    float mean = sum / data.size();
                    result.push_back(mean);
                    
                    // Add basic statistics
                    float min = *std::min_element(data.begin(), data.end());
                    float max = *std::max_element(data.begin(), data.end());
                    result.push_back(max - min); // range
                    result.push_back(mean * 2.0f); // scaled mean
                }
            }
            
            response.success = true;
            response.data["analysis"] = result;
            response.data["count"] = static_cast<size_t>(result.size());
            
        } else if (request.operation == "filter") {
            // Filter data
            std::vector<float> result;
            if (request.parameters.count("data") && request.parameters.count("threshold")) {
                auto& data = std::any_cast<std::vector<float>&>(request.parameters.at("data"));
                float threshold = std::any_cast<float>(request.parameters.at("threshold"));
                
                // Filter values above threshold
                for (float value : data) {
                    if (value > threshold) {
                        result.push_back(value);
                    }
                }
            }
            
            response.success = true;
            response.data["filtered"] = result;
            
        } else {
            response.success = false;
            response.errorMessage = "Unknown operation: " + request.operation;
        }
        
        return response.success;
    }
    
    std::string getName() const override {
        return "DataProcessor";
    }
    
    ServiceType getType() const override {
        return ServiceType::Data;
    }
    
private:
    bool initialized_;
};

// Plugin factory function that creates both plugin and service
extern "C" std::unique_ptr<Plugin> createPlugin() {
    auto plugin = std::make_unique<DataProcessorPlugin>();
    
    // Register the service with the plugin
    ServiceInfo info;
    info.name = "DataProcessor";
    info.description = "Processes and analyzes data";
    info.type = ServiceType::Data;
    info.interfaceType = typeid(std::vector<float>);
    info.isSingleton = true;
    
    plugin->registerService(info);
    
    // Create service instance (plugin needs to manage it)
    plugin->setService("DataProcessor", std::make_unique<DataProcessorService>());
    
    return plugin;
}

} // namespace nlm
