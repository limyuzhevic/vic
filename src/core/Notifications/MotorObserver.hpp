#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <iostream>

namespace nlm {

// Motor observer for monitoring motor system activity
class MotorObserver : public Observer {
public:
    explicit MotorObserver(const std::string& systemName = "Motor")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        if (notification.sourceSystem == "Motor") {
            std::cout << "[MotorObserver] " << notification.data << std::endl;
        }
    }
    
    void onInitialize() override {
        std::cout << "[MotorObserver] Motor system initialized" << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[MotorObserver] Motor system shutting down" << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::MotorOutput ||
               type == NotificationType::Spike ||
               type == NotificationType::Neuromodulation;
    }
    
    void onActionProduced(const std::string& action) {
        std::cout << "[MotorObserver] Action produced: " << action << std::endl;
    }
    
private:
    std::string systemName_;
};

// Sensory observer for monitoring sensory processing
class SensoryObserver : public Observer {
public:
    explicit SensoryObserver(const std::string& systemName = "Sensory")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        if (notification.sourceSystem == "Sensory") {
            std::cout << "[SensoryObserver] " << notification.data << std::endl;
        }
    }
    
    void onInitialize() override {
        std::cout << "[SensoryObserver] Sensory system initialized" << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[SensoryObserver] Sensory system shutting down" << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::SensoryInput ||
               type == NotificationType::NoveltyDetected ||
               type == NotificationType::CuriosityActivation;
    }
    
    void onInputProcessed(const std::string& modality) {
        std::cout << "[SensoryObserver] Input processed: " << modality << std::endl;
    }
    
private:
    std::string systemName_;
};

} // namespace nlm
