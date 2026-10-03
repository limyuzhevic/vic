#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace nlm {

// Forward declaration
class SpikeSystem;

class MemoryObserver : public Observer {
public:
    explicit MemoryObserver(const std::string& systemName = "Memory")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[MemoryObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[MemoryObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[MemoryObserver] Shutting down..." << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::MemoryUpdate ||
               type == NotificationType::WorkingMemoryUpdate ||
               type == NotificationType::EpisodicMemoryUpdate;
    }
    
    void onSpike(const Notification& notification) {
        std::cout << "[MemoryObserver] Spike detected - updating working memory" << std::endl;
    }
    
private:
    std::string systemName_;
};

class PredictionObserver : public Observer {
public:
    explicit PredictionObserver(const std::string& systemName = "Prediction")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[PredictionObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[PredictionObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[PredictionObserver] Shutting down..." << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::PredictionError ||
               type == NotificationType::ConceptFormation ||
               type == NotificationType::PlanningUpdate;
    }
    
    void onPredictionError(const Notification& notification) {
        std::cout << "[PredictionObserver] Prediction error detected - triggering learning" << std::endl;
    }
    
private:
    std::string systemName_;
};

class MotorObserver : public Observer {
public:
    explicit MotorObserver(const std::string& systemName = "Motor")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[MotorObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[MotorObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[MotorObserver] Shutting down..." << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::MotorOutput ||
               type == NotificationType::Spike ||
               type == NotificationType::Neuromodulation;
    }
    
    void onActionProduced(const Notification& notification) {
        std::cout << "[MotorObserver] Action produced - monitoring execution" << std::endl;
    }
    
private:
    std::string systemName_;
};

class SensoryObserver : public Observer {
public:
    explicit SensoryObserver(const std::string& systemName = "Sensory")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[SensoryObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[SensoryObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[SensoryObserver] Shutting down..." << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::SensoryInput ||
               type == NotificationType::NoveltyDetected ||
               type == NotificationType::CuriosityActivation;
    }
    
    void onInputProcessed(const Notification& notification) {
        std::cout << "[SensoryObserver] Sensory input processed - updating perception" << std::endl;
    }
    
private:
    std::string systemName_;
};

class NeuromodulationObserver : public Observer {
public:
    explicit NeuromodulationObserver(const std::string& systemName = "Neuromodulation")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[NeuromodulationObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[NeuromodulationObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[NeuromodulationObserver] Shutting down..." << std::endl;
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
    
    void onModulationSignal(const Notification& notification) {
        std::cout << "[NeuromodulationObserver] Neuromodulation signal - affecting behavior" << std::endl;
    }
    
private:
    std::string systemName_;
};

class DevelopmentObserver : public Observer {
public:
    explicit DevelopmentObserver(const std::string& systemName = "Development")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[DevelopmentObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[DevelopmentObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[DevelopmentObserver] Shutting down..." << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::DevelopmentUpdate ||
               type == NotificationType::PlasticityUpdate ||
               type == NotificationType::IntegrationComplete;
    }
    
    void onDevelopmentChange(const Notification& notification) {
        std::cout << "[DevelopmentObserver] Development stage changed - adapting architecture" << std::endl;
    }
    
private:
    std::string systemName_;
};

class CognitionObserver : public Observer {
public:
    explicit CognitionObserver(const std::string& systemName = "Cognition")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        std::cout << "[CognitionObserver] Received " << static_cast<int>(notification.type) 
                  << " from " << notification.sourceSystem << std::endl;
    }
    
    void onInitialize() override {
        std::cout << "[CognitionObserver] Initializing..." << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[CognitionObserver] Shutting down..." << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::ConceptFormation ||
               type == NotificationType::PlanningUpdate ||
               type == NotificationType::AttentionShift;
    }
    
    void onConceptFormed(const Notification& notification) {
        std::cout << "[CognitionObserver] New concept formed - updating knowledge" << std::endl;
    }
    
private:
    std::string systemName_;
};

} // namespace nlm
