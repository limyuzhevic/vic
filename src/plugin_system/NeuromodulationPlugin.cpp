#pragma once

#include "NeuromodulationPlugin.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <unordered_map>

namespace nlm {

class NeuromodulationPlugin::Impl {
public:
    NeuromodulationPlugin::Impl() : initialized_(false), dopamineLevel_(0.0f), curiosityLevel_(0.0f), noveltyLevel_(0.0f) {
    }
    
    ~Impl() {
    }
    
    bool initialize() {
        initialized_ = true;
        NLM_LOG_INFO("NeuromodulationPluginImpl initialized");
        return true;
    }
    
    void shutdown() {
        initialized_ = false;
        NLM_LOG_INFO("NeuromodulationPluginImpl shutdown");
    }
    
    bool isInitialized() const {
        return initialized_;
    }
    
    std::vector<ServiceInfo> getProvidedServices() const {
        return providedServices_;
    }
    
    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) {
        if (!initialized_) {
            response.success = false;
            response.errorMessage = "Plugin not initialized";
            return false;
        }
        
        if (request.operation == "get_dopamine") {
            response.success = true;
            response.data["dopamine_level"] = dopamineLevel_;
        } else if (request.operation == "get_curiosity") {
            response.success = true;
            response.data["curiosity_level"] = curiosityLevel_;
        } else if (request.operation == "get_novelty") {
            response.success = true;
            response.data["novelty_level"] = noveltyLevel_;
        } else {
            response.success = false;
            response.errorMessage = "Unknown operation: " + request.operation;
        }
        
        return response.success;
    }
    
    bool onPluginEvent(const PluginEvent& event) {
        if (event.type == PluginEventType::BrainUpdate) {
            auto it = event.data.find("dopamine_level");
            if (it != event.data.end()) {
                dopamineLevel_ = std::any_cast<float>(it->second);
            }
            
            it = event.data.find("curiosity_level");
            if (it != event.data.end()) {
                curiosityLevel_ = std::any_cast<float>(it->second);
            }
            
            it = event.data.find("novelty_level");
            if (it != event.data.end()) {
                noveltyLevel_ = std::any_cast<float>(it->second);
            }
        }
        
        return true;
    }
    
    bool execute(const ExecutionContext& context) {
        if (!initialized_) {
            return false;
        }
        
        if (context.operation == "regulate_neural_excitability") {
            float targetDopamine = 0.0f;
            auto it = context.typedArguments.find("target_dopamine");
            if (it != context.typedArguments.end()) {
                targetDopamine = std::any_cast<float>(it->second);
            }
            
            // Simple regulation logic
            float diff = targetDopamine - dopamineLevel_;
            dopamineLevel_ += diff * 0.1f; // Smooth adjustment
            
            NLM_LOG_INFO(std::string("Regulating neural excitability, dopamine: ") + 
                        std::to_string(dopamineLevel_));
            
            return true;
        } else if (context.operation == "process_novelty") {
            float noveltyInput = 0.0f;
            auto it = context.typedArguments.find("novelty_input");
            if (it != context.typedArguments.end()) {
                noveltyInput = std::any_cast<float>(it->second);
            }
            
            // Simple novelty processing
            noveltyLevel_ = std::min(1.0f, noveltyLevel_ + noveltyInput * 0.05f);
            
            NLM_LOG_INFO(std::string("Processing novelty, level: ") + std::to_string(noveltyLevel_));
            
            return true;
        }
        
        return false;
    }
    
    void onBrainUpdate(const BrainUpdateEvent& event) {
        if (event.type == BrainUpdateEvent::Type::NeuromodulationUpdate) {
            PluginEvent pluginEvent;
            pluginEvent.type = PluginEventType::BrainUpdate;
            pluginEvent.pluginName = "NeuromodulationPlugin";
            
            for (const auto& pair : event.data) {
                pluginEvent.data[pair.first] = pair.second;
            }
            
            onPluginEvent(pluginEvent);
        }
    }
    
    std::string getName() const {
        return "NeuromodulationPlugin";
    }
    
    std::string getVersion() const {
        return "1.0.0";
    }
    
    void registerServices() {
        // Register dopamine service
        ServiceInfo dopamineService;
        dopamineService.name = "dopamine_service";
        dopamineService.description = "Dopamine neuromodulation service";
        dopamineService.type = ServiceType::Neuromodulation;
        dopamineService.interfaceType = typeid(std::function<float()>());
        dopamineService.isSingleton = true;
        
        providedServices_.push_back(dopamineService);
        
        // Register curiosity service
        ServiceInfo curiosityService;
        curiosityService.name = "curiosity_service";
        curiosityService.description = "Curiosity neuromodulation service";
        curiosityService.type = ServiceType::Motivation;
        curiosityService.interfaceType = typeid(std::function<float()>());
        curiosityService.isSingleton = true;
        
        providedServices_.push_back(curiosityService);
        
        // Register novelty service
        ServiceInfo noveltyService;
        noveltyService.name = "novelty_service";
        noveltyService.description = "Novelty detection service";
        noveltyService.type = ServiceType::Perception;
        noveltyService.interfaceType = typeid(std::function<float()>());
        noveltyService.isSingleton = true;
        
        providedServices_.push_back(noveltyService);
    }
    
private:
    bool initialized_;
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    std::vector<ServiceInfo> providedServices_;
};

NeuromodulationPlugin::NeuromodulationPlugin() : pImpl(std::make_unique<Impl>()) {
    pImpl->registerServices();
}

NeuromodulationPlugin::~NeuromodulationPlugin() = default;

const PluginMetadata& NeuromodulationPlugin::getMetadata() const {
    static PluginMetadata metadata;
    metadata.name = "NeuromodulationPlugin";
    metadata.version = "1.0.0";
    metadata.description = "Neuromodulation plugin for dopamine, curiosity, and novelty";
    metadata.author = "NLM Team";
    metadata.requiresBrain = true;
    metadata.autoEnable = false;
    metadata.type = PluginType::Neuromodulation;
    return metadata;
}

bool NeuromodulationPlugin::initialize() {
    return pImpl->initialize();
}

void NeuromodulationPlugin::shutdown() {
    pImpl->shutdown();
}

bool NeuromodulationPlugin::isInitialized() const {
    return pImpl->isInitialized();
}

std::vector<ServiceInfo> NeuromodulationPlugin::getProvidedServices() const {
    return pImpl->getProvidedServices();
}

bool NeuromodulationPlugin::onServiceRequest(const ServiceRequest& request, ServiceResponse& response) {
    return pImpl->onServiceRequest(request, response);
}

bool NeuromodulationPlugin::onPluginEvent(const PluginEvent& event) {
    return pImpl->onPluginEvent(event);
}

bool NeuromodulationPlugin::execute(const ExecutionContext& context) {
    return pImpl->execute(context);
}

void NeuromodulationPlugin::onBrainUpdate(const BrainUpdateEvent& event) {
    pImpl->onBrainUpdate(event);
}

std::string NeuromodulationPlugin::getName() const {
    return pImpl->getName();
}

std::string NeuromodulationPlugin::getVersion() const {
    return pImpl->getVersion();
}

} // namespace nlm
