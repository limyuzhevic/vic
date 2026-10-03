#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <iostream>

namespace nlm {

// Development observer for monitoring developmental changes
class DevelopmentObserver : public Observer {
public:
    explicit DevelopmentObserver(const std::string& systemName = "Development")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        if (notification.sourceSystem == "Development") {
            std::cout << "[DevelopmentObserver] " << notification.data << std::endl;
        }
    }
    
    void onInitialize() override {
        std::cout << "[DevelopmentObserver] Development system initialized" << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[DevelopmentObserver] Development system shutting down" << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::DevelopmentUpdate ||
               type == NotificationType::PlasticityUpdate ||
               type == NotificationType::IntegrationComplete;
    }
    
    void onDevelopmentStageChanged(const std::string& stage) {
        std::cout << "[DevelopmentObserver] Development stage changed: " << stage << std::endl;
    }
    
private:
    std::string systemName_;
};

// Cognition observer for monitoring cognitive processes
class CognitionObserver : public Observer {
public:
    explicit CognitionObserver(const std::string& systemName = "Cognition")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        if (notification.sourceSystem == "Cognition") {
            std::cout << "[CognitionObserver] " << notification.data << std::endl;
        }
    }
    
    void onInitialize() override {
        std::cout << "[CognitionObserver] Cognition system initialized" << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[CognitionObserver] Cognition system shutting down" << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::ConceptFormation ||
               type == NotificationType::PlanningUpdate ||
               type == NotificationType::AttentionShift;
    }
    
    void onConceptFormed(const std::string& concept) {
        std::cout << "[CognitionObserver] Concept formed: " << concept << std::endl;
    }
    
private:
    std::string systemName_;
};

} // namespace nlm
