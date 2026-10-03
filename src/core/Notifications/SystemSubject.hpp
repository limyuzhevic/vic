#pragma once

#include "core/Notifications/Observer.hpp"
#include "core/Notifications/Subject.hpp"
#include "core/Notifications/NotificationManager.hpp"
#include <memory>
#include <string>

namespace nlm {

// Wrapper subject for systems that don't natively implement notifications
class SystemSubject : public Subject {
public:
    SystemSubject(const std::string& systemName)
        : systemName_(systemName) {}
    
    std::string getSystemName() const override {
        return systemName_;
    }
    
    // Notify about a system event
    void notifyEvent(const std::string& eventType, const std::string& eventData) {
        Notification notification(
            static_cast<NotificationType>(0), // Temporary - need eventType mapping
            systemName_,
            eventType + ": " + eventData
        );
        notify(notification);
    }
    
private:
    std::string systemName_;
};

} // namespace nlm
