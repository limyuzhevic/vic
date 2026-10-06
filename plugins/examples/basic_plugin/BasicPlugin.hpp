#pragma once

#include "Plugin.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>

namespace nlm {

class BasicPlugin : public Plugin {
public:
    BasicPlugin() : initialized_(false) {}
    ~BasicPlugin() override { shutdown(); }
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            .name = "BasicPlugin",
            .version = "1.0.0",
            .description = "A basic plugin template for NLM",
            .author = "NLM Team",
            .requiresBrain = false,
            .autoEnable = true,
            .type = PluginType::General
        };
        return metadata;
    }
    
    bool initialize() override {
        if (initialized_) return true;
        
        // Initialize plugin
        NLM_LOG_INFO("BasicPlugin initialized");
        
        // Register services
        ServiceInfo info;
        info.name = "dataProcessor";
        info.description = "Process input data";
        info.type = ServiceType::Data;
        info.isSingleton = true;
        registerService(info);
        
        info.name = "eventHandler";
        info.description = "Handle plugin events";
        info.type = ServiceType::Communication;
        info.isSingleton = false;
        registerService(info);
        
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        if (!initialized_) return;
        
        NLM_LOG_INFO("BasicPlugin shutdown");
        initialized_ = false;
    }
    
    bool isInitialized() const override {
        return initialized_;
    }
    
    std::vector<ServiceInfo> getProvidedServices() const override {
        // Return service info (simplified - in real implementation this would be stored)
        return {
            {"dataProcessor", "Process input data", ServiceType::Data, typeid(std::vector<float>), true},
            {"eventHandler", "Handle plugin events", ServiceType::Communication, typeid(std::string), false}
        };
    }
    
    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override {
        NLM_LOG_INFO(std::string("BasicPlugin received service request: ") + request.serviceName + 
                    " operation: " + request.operation);
        
        if (request.serviceName == "dataProcessor") {
            // Process data
            std::vector<float> result;
            if (request.parameters.count("input")) {
                auto& input = std::any_cast<std::vector<float>&>(request.parameters.at("input"));
                // Simple processing: double the values
                result.resize(input.size());
                for (size_t i = 0; i < input.size(); ++i) {
                    result[i] = input[i] * 2.0f;
                }
            }
            
            response.success = true;
            response.data["result"] = result;
            NLM_LOG_INFO("Data processing completed");
            
        } else if (request.serviceName == "eventHandler") {
            // Handle event
            response.success = true;
            response.data["handled"] = true;
            NLM_LOG_INFO("Event handled");
        }
        
        return response.success;
    }
    
    bool onPluginEvent(const PluginEvent& event) override {
        NLM_LOG_INFO(std::string("BasicPlugin received plugin event: ") + 
                    std::to_string(static_cast<int>(event.type)));
        
        // Handle different event types
        switch (event.type) {
            case PluginEventType::Load:
                NLM_LOG_INFO("Plugin loaded successfully");
                break;
            case PluginEventType::Enable:
                NLM_LOG_INFO("Plugin enabled");
                break;
            case PluginEventType::Disable:
                NLM_LOG_INFO("Plugin disabled");
                break;
            default:
                NLM_LOG_INFO("Plugin event received (type: " + std::to_string(static_cast<int>(event.type)) + ")");
                break;
        }
        
        return true;
    }
    
    bool execute(const ExecutionContext& context) override {
        NLM_LOG_INFO(std::string("BasicPlugin executing: ") + context.operation);
        
        if (context.operation == "processData") {
            // Process data based on arguments
            std::string input = "default";
            if (context.arguments.count("input")) {
                input = context.arguments.at("input");
            }
            
            // Simulate processing
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            
            // Store result
            context.result["processed"] = input + "_processed";
            context.result["status"] = "success";
            
            NLM_LOG_INFO(std::string("Data processing completed for: ") + input);
            
        } else if (context.operation == "calculate") {
            // Perform calculation
            float result = 0.0f;
            if (context.arguments.count("x") && context.arguments.count("y")) {
                try {
                    float x = std::stof(context.arguments.at("x"));
                    float y = std::stof(context.arguments.at("y"));
                    result = x * y + 10.0f;
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Calculation error: ") + e.what());
                    context.result["error"] = e.what();
                    return false;
                }
            }
            
            context.result["result"] = result;
            NLM_LOG_INFO(std::string("Calculation completed: ") + std::to_string(result));
            
        } else {
            NLM_LOG_WARNING(std::string("Unknown operation: ") + context.operation);
            context.result["error"] = "Unknown operation";
            return false;
        }
        
        return true;
    }
    
    void onBrainUpdate(const BrainUpdateEvent& event) override {
        NLM_LOG_INFO(std::string("BasicPlugin received brain update: ") + 
                    std::to_string(static_cast<int>(event.type)));
    }
    
    std::string getName() const override {
        return "BasicPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
private:
    bool initialized_;
};

// Plugin factory function
extern "C" std::unique_ptr<Plugin> createPlugin() {
    return std::make_unique<BasicPlugin>();
}

} // namespace nlm
