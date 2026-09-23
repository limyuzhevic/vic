#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <algorithm>

namespace nlm {

struct HierarchicalLogger::Impl {
    std::shared_ptr<ComponentLogger> rootLogger;
    std::unordered_map<std::string, std::shared_ptr<ComponentLogger>> componentLoggers;
    std::mutex mutex;
    std::function<bool(const std::string&, const LogEntry&)> globalFilter;
    LogLevel globalLevel = LogLevel::Info;
    std::unordered_map<std::string, bool> componentEnabled;
};

HierarchicalLogger::HierarchicalLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->rootLogger = std::make_shared<ComponentLogger>("root");
}

HierarchicalLogger::~HierarchicalLogger() = default;

void HierarchicalLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if logging is enabled for this component
    if (pImpl->componentEnabled.find(entry.component) == pImpl->componentEnabled.end() || 
        !pImpl->componentEnabled[entry.component]) {
        return;
    }
    
    // Apply global filter
    if (pImpl->globalFilter && !pImpl->globalFilter(entry.component, entry)) {
        return;
    }
    
    // Check global level
    if (entry.level < pImpl->globalLevel) {
        return;
    }
    
    // Find the appropriate component logger
    std::shared_ptr<ComponentLogger> targetLogger;
    
    // First try to find exact match
    auto it = pImpl->componentLoggers.find(entry.component);
    if (it != pImpl->componentLoggers.end()) {
        targetLogger = it->second;
    } else {
        // Find parent component
        targetLogger = pImpl->rootLogger;
        
        // Walk down the hierarchy to find the appropriate logger
        std::vector<std::string> componentPath;
        std::string current = entry.component;
        while (!current.empty()) {
            componentPath.insert(componentPath.begin(), current);
            auto parentIt = pImpl->componentLoggers.find(current);
            if (parentIt != pImpl->componentLoggers.end()) {
                // Found a registered component logger, use it
                targetLogger = parentIt->second;
                break;
            }
            // Move to parent (if any)
            size_t lastDot = current.find_last_of('.');
            if (lastDot != std::string::npos) {
                current = current.substr(0, lastDot);
            } else {
                current = "";
            }
        }
    }
    
    if (targetLogger) {
        targetLogger->log(entry);
    }
}

void HierarchicalLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalLevel = level;
    for (auto& [name, logger] : pImpl->componentLoggers) {
        logger->setLevel(level);
    }
}

LogLevel HierarchicalLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalLevel;
}

void HierarchicalLogger::registerComponentLogger(const std::string& componentName, 
                                                 const std::string& parentComponent,
                                                 LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create component logger
    auto componentLogger = std::make_shared<ComponentLogger>(componentName, parentComponent, level);
    
    // Set enabled status if configured
    if (pImpl->componentEnabled.find(componentName) != pImpl->componentEnabled.end()) {
        componentLogger->setEnabled(pImpl->componentEnabled[componentName]);
    }
    
    // Register the logger
    pImpl->componentLoggers[componentName] = componentLogger;
    
    // Add to parent if specified
    if (!parentComponent.empty()) {
        auto parentIt = pImpl->componentLoggers.find(parentComponent);
        if (parentIt != pImpl->componentLoggers.end()) {
            parentIt->second->addChildLogger(componentLogger);
        }
    }
}

std::shared_ptr<ComponentLogger> HierarchicalLogger::getComponentLogger(const std::string& componentName,
                                                                         const std::string& parentComponent) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->componentLoggers.find(componentName);
    if (it != pImpl->componentLoggers.end()) {
        return it->second;
    }
    
    // Create and register new component logger if not found
    auto componentLogger = std::make_shared<ComponentLogger>(componentName, parentComponent);
    pImpl->componentLoggers[componentName] = componentLogger;
    
    // Add to parent if specified
    if (!parentComponent.empty()) {
        auto parentIt = pImpl->componentLoggers.find(parentComponent);
        if (parentIt != pImpl->componentLoggers.end()) {
            parentIt->second->addChildLogger(componentLogger);
        }
    }
    
    return componentLogger;
}

std::shared_ptr<ComponentLogger> HierarchicalLogger::getRootComponentLogger() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->rootLogger;
}

void HierarchicalLogger::unregisterComponentLogger(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->componentLoggers.find(componentName);
    if (it != pImpl->componentLoggers.end()) {
        // Remove from parent's child list
        std::string parentComponent = it->second->getParentComponent();
        if (!parentComponent.empty()) {
            auto parentIt = pImpl->componentLoggers.find(parentComponent);
            if (parentIt != pImpl->componentLoggers.end()) {
                parentIt->second->clearChildLoggers();
                // Re-add remaining children
                for (const auto& childLogger : it->second->getChildLoggers()) {
                    parentIt->second->addChildLogger(childLogger);
                }
            }
        }
        
        pImpl->componentLoggers.erase(it);
    }
}

void HierarchicalLogger::setGlobalFilter(const std::function<bool(const std::string&, const LogEntry&)>& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
}

void HierarchicalLogger::setGlobalLevel(LogLevel level) {
    setLevel(level);
}

void HierarchicalLogger::setComponentEnabled(const std::string& componentName, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->componentEnabled[componentName] = enabled;
    
    auto it = pImpl->componentLoggers.find(componentName);
    if (it != pImpl->componentLoggers.end()) {
        it->second->setEnabled(enabled);
    }
}

bool HierarchicalLogger::isComponentEnabled(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->componentEnabled.find(componentName);
    if (it != pImpl->componentEnabled.end()) {
        return it->second;
    }
    
    // Default to enabled
    return true;
}

bool HierarchicalLogger::shouldLog(const std::string& componentName, const LogEntry& entry) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if component is enabled
    if (!isComponentEnabled(componentName)) {
        return false;
    }
    
    // Apply global filter
    if (pImpl->globalFilter && !pImpl->globalFilter(componentName, entry)) {
        return false;
    }
    
    // Check global level
    if (entry.level < pImpl->globalLevel) {
        return false;
    }
    
    return true;
}

} // namespace nlm
