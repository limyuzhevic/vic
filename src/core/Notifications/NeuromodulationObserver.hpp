#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <iostream>

namespace nlm {

// Neuromodulation observer for monitoring neuromodulatory signals
class NeuromodulationObserver : public Observer {
public:
    explicit NeuromodulationObserver(const std::string& systemName = "Neuromodulation")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        if (notification.sourceSystem == "Neuromodulation") {
            std::cout << "[NeuromodulationObserver] " << notification.data << std::endl;
        }
    }
    
    void onInitialize() override {
        std::cout << "[NeuromodulationObserver] Neuromodulation system initialized" << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[NeuromodulationObserver] Neuromodulation system shutting down" << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::Neuromodulation ||
               type == NotificationType::RewardReceived ||
               type == NotificationType::CuriosityActivation ||
               type == NotificationType::NoveltyDetected;
    }
    
    void onModulationSignal(const std::string& signal) {
        std::cout << "[NeuromodulationObserver] Modulation signal: " << signal << std::endl;
    }
    
private:
    std::string systemName_;
};

} // namespace nlm
