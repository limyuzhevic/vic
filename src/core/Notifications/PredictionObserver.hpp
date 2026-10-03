#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <iostream>

namespace nlm {

// Prediction observer for monitoring prediction errors and prediction system activity
class PredictionObserver : public Observer {
public:
    explicit PredictionObserver(const std::string& systemName = "Prediction")
        : systemName_(systemName) {}
    
    void onNotify(const Notification& notification) override {
        if (notification.sourceSystem == "Prediction") {
            std::cout << "[PredictionObserver] " << notification.data << std::endl;
        }
    }
    
    void onInitialize() override {
        std::cout << "[PredictionObserver] Prediction system initialized" << std::endl;
    }
    
    void onShutdown() override {
        std::cout << "[PredictionObserver] Prediction system shutting down" << std::endl;
    }
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    bool isInterested(const NotificationType& type) const override {
        return type == NotificationType::PredictionError ||
               type == NotificationType::ConceptFormation ||
               type == NotificationType::PlanningUpdate;
    }
    
    void onPredictionErrorDetected(const std::string& error) {
        std::cout << "[PredictionObserver] Prediction error detected: " << error << std::endl;
    }
    
private:
    std::string systemName_;
};

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
