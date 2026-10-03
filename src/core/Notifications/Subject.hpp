#pragma once

#include "core/Notifications/Observer.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iostream>

namespace nlm {

class Subject {
public:
    virtual ~Subject() = default;
    
    void attach(std::shared_ptr<Observer> observer) {
        if (!observer) return;
        
        // Check if observer is already attached
        auto it = std::find_if(observers_.begin(), observers_.end(),
            [&observer](const std::shared_ptr<Observer>& obs) {
                return obs.get() == observer.get();
            });
        
        if (it == observers_.end()) {
            observers_.push_back(observer);
            std::cout << "[Subject] Observer " << observer->getSystemName() 
                      << " attached to " << getSystemName() << std::endl;
        }
    }
    
    void detach(const std::shared_ptr<Observer>& observer) {
        if (!observer) return;
        
        auto it = std::find_if(observers_.begin(), observers_.end(),
            [&observer](const std::shared_ptr<Observer>& obs) {
                return obs.get() == observer.get();
            });
        
        if (it != observers_.end()) {
            std::cout << "[Subject] Observer " << (*it)->getSystemName() 
                      << " detached from " << getSystemName() << std::endl;
            observers_.erase(it);
        }
    }
    
    void notify(const Notification& notification) {
        // Find interested observers
        std::vector<std::shared_ptr<Observer>> interestedObservers;
        
        for (const auto& observer : observers_) {
            if (observer && observer->isInterested(notification.type)) {
                interestedObservers.push_back(observer);
            }
        }
        
        // Notify all interested observers
        for (const auto& observer : interestedObservers) {
            try {
                observer->onNotify(notification);
            } catch (const std::exception& e) {
                std::cerr << "[Subject] Error notifying observer " 
                          << observer->getSystemName() << ": " << e.what() << std::endl;
            }
        }
        
        if (!interestedObservers.empty()) {
            std::cout << "[Subject] Notified " << interestedObservers.size() 
                      << " observers about " << static_cast<int>(notification.type)
                      << " from " << notification.sourceSystem << std::endl;
        }
    }
    
    std::vector<std::shared_ptr<Observer>> getObservers() const {
        return observers_;
    }
    
    void initializeAll() {
        std::cout << "[Subject] Initializing all observers..." << std::endl;
        for (const auto& observer : observers_) {
            if (observer) {
                observer->onInitialize();
            }
        }
    }
    
    void shutdownAll() {
        std::cout << "[Subject] Shutting down all observers..." << std::endl;
        for (const auto& observer : observers_) {
            if (observer) {
                observer->onShutdown();
            }
        }
        observers_.clear();
    }
    
protected:
    Subject() = default;
    
    std::vector<std::shared_ptr<Observer>> observers_;
};

// Template Subject implementation for convenience
class ConcreteSubject : public Subject {
public:
    explicit ConcreteSubject(const std::string& systemName)
        : systemName_(systemName) {}
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
private:
    std::string systemName_;
};

} // namespace nlm
