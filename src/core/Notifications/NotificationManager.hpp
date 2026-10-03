#pragma once

#include "core/Notifications/Observer.hpp"
#include "core/Notifications/Subject.hpp"
#include "core/Types/Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <vector>
#include <iostream>

namespace nlm {

// Forward declarations for key systems
class Memory;
class PredictionSystem;
class MotorSystem;
class SensoryInput;
class Neuromodulator;
class DevelopmentSystem;
class Brain;

// NotificationManager - central hub for system notifications
class NotificationManager {
public:
    static NotificationManager& getInstance() {
        static NotificationManager instance;
        return instance;
    }
    
    // Register a subject for notification broadcasting
    void registerSubject(const std::string& systemName, std::shared_ptr<Subject> subject) {
        subjects_[systemName] = subject;
        std::cout << "[NotificationManager] Registered subject: " << systemName << std::endl;
    }
    
    // Unregister a subject
    void unregisterSubject(const std::string& systemName) {
        if (subjects_.find(systemName) != subjects_.end()) {
            subjects_.erase(systemName);
            std::cout << "[NotificationManager] Unregistered subject: " << systemName << std::endl;
        }
    }
    
    // Broadcast notification from a specific system
    void notify(const Notification& notification) {
        std::cout << "[NotificationManager] Broadcasting: " << static_cast<int>(notification.type)
                  << " from " << notification.sourceSystem << std::endl;
        
        // Notify the specific source system if it exists
        if (subjects_.find(notification.sourceSystem) != subjects_.end()) {
            subjects_[notification.sourceSystem]->notify(notification);
        }
        
        // Also broadcast to all subjects (optional - can be disabled)
        broadcastToAllSubjects(notification);
    }
    
    // Broadcast to all registered subjects
    void broadcastToAllSubjects(const Notification& notification) {
        for (auto& pair : subjects_) {
            try {
                pair.second->notify(notification);
            } catch (const std::exception& e) {
                std::cerr << "[NotificationManager] Error broadcasting to " 
                          << pair.first << ": " << e.what() << std::endl;
            }
        }
    }
    
    // Get a subject by system name
    std::shared_ptr<Subject> getSubject(const std::string& systemName) {
        if (subjects_.find(systemName) != subjects_.end()) {
            return subjects_[systemName];
        }
        return nullptr;
    }
    
    // Initialize all subjects
    void initializeAll() {
        std::cout << "[NotificationManager] Initializing all subjects..." << std::endl;
        for (auto& pair : subjects_) {
            pair.second->initializeAll();
        }
    }
    
    // Shutdown all subjects
    void shutdownAll() {
        std::cout << "[NotificationManager] Shutting down all subjects..." << std::endl;
        for (auto& pair : subjects_) {
            pair.second->shutdownAll();
        }
        subjects_.clear();
    }
    
    // Create and send specific event notifications
    void notifySpike(NeuronId neuron, const std::string& sourceSystem = "Brain") {
        Notification notification(
            NotificationType::Spike,
            sourceSystem,
            "Neuron " + std::to_string(neuron.value) + " fired"
        );
        notify(notification);
    }
    
    void notifyMemoryUpdate(const std::string& memoryType, const std::string& sourceSystem = "Memory") {
        Notification notification(
            NotificationType::MemoryUpdate,
            sourceSystem,
            "Memory update: " + memoryType
        );
        notify(notification);
    }
    
    void notifyPredictionError(const std::string& sourceSystem = "Prediction") {
        Notification notification(
            NotificationType::PredictionError,
            sourceSystem,
            "Prediction error detected"
        );
        notify(notification);
    }
    
    void notifyNeuromodulation(const std::string& modulatorType, const std::string& sourceSystem = "Neuromodulation") {
        Notification notification(
            NotificationType::Neuromodulation,
            sourceSystem,
            "Neuromodulation: " + modulatorType
        );
        notify(notification);
    }
    
    void notifySensoryInput(const std::string& modality, const std::string& sourceSystem = "Sensory") {
        Notification notification(
            NotificationType::SensoryInput,
            sourceSystem,
            "Sensory input: " + modality
        );
        notify(notification);
    }
    
    void notifyMotorOutput(const std::string& action, const std::string& sourceSystem = "Motor") {
        Notification notification(
            NotificationType::MotorOutput,
            sourceSystem,
            "Motor output: " + action
        );
        notify(notification);
    }
    
    void notifyDevelopmentUpdate(const std::string& stage, const std::string& sourceSystem = "Development") {
        Notification notification(
            NotificationType::DevelopmentUpdate,
            sourceSystem,
            "Development update: " + stage
        );
        notify(notification);
    }
    
    void notifyConceptFormation(const std::string& concept, const std::string& sourceSystem = "Cognition") {
        Notification notification(
            NotificationType::ConceptFormation,
            sourceSystem,
            "Concept formed: " + concept
        );
        notify(notification);
    }
    
    void notifyAttentionShift(const std::string& target, const std::string& sourceSystem = "Attention") {
        Notification notification(
            NotificationType::AttentionShift,
            sourceSystem,
            "Attention shifted to: " + target
        );
        notify(notification);
    }
    
    void notifyCuriosityActivation(const std::string& trigger, const std::string& sourceSystem = "Curiosity") {
        Notification notification(
            NotificationType::CuriosityActivation,
            sourceSystem,
            "Curiosity activated: " + trigger
        );
        notify(notification);
    }
    
    void notifyRewardReceived(const std::string& reward, const std::string& sourceSystem = "Reward") {
        Notification notification(
            NotificationType::RewardReceived,
            sourceSystem,
            "Reward received: " + reward
        );
        notify(notification);
    }
    
    void notifyNoveltyDetected(const std::string& novelty, const std::string& sourceSystem = "Novelty") {
        Notification notification(
            NotificationType::NoveltyDetected,
            sourceSystem,
            "Novelty detected: " + novelty
        );
        notify(notification);
    }
    
    void notifyIntegrationComplete(const std::string& stage, const std::string& sourceSystem = "Integration") {
        Notification notification(
            NotificationType::IntegrationComplete,
            sourceSystem,
            "Integration complete: " + stage
        );
        notify(notification);
    }
    
    void notifySystemReady(const std::string& system, const std::string& sourceSystem = "System") {
        Notification notification(
            NotificationType::SystemReady,
            sourceSystem,
            "System ready: " + system
        );
        notify(notification);
    }
    
    void notifyPlasticityUpdate(const std::string& ruleType, const std::string& sourceSystem = "Plasticity") {
        Notification notification(
            NotificationType::PlasticityUpdate,
            sourceSystem,
            "Plasticity update: " + ruleType
        );
        notify(notification);
    }
    
    // Special observation hooks for brain step integration
    void observeBrainStep(SimulationStep step, Timestamp time, const std::string& sourceSystem = "Brain") {
        Notification notification(
            NotificationType::IntegrationComplete,
            sourceSystem,
            "Brain step: " + std::to_string(step) + " at time " + std::to_string(time)
        );
        notify(notification);
    }
    
private:
    NotificationManager() = default;
    ~NotificationManager() = default;
    
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<Subject>> subjects_;
};

} // namespace nlm
