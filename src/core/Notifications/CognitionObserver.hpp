#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <iostream>

namespace nlm {

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
