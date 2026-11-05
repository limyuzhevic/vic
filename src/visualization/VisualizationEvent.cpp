// Forward declarations to avoid circular includes
namespace nlm {
    class VisualizationManager;
}

// VisualizationEvent implementation
namespace nlm {

VisualizationEvent::VisualizationEvent() {
    NLM_LOG_INFO("VisualizationEvent system initialized", "VisualizationEvent::Constructor");
}

VisualizationEvent::~VisualizationEvent() {
    NLM_LOG_INFO("VisualizationEvent system destroyed", "VisualizationEvent::Destructor");
}

void VisualizationEvent::emitEvent(const EventData& event) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    events_.push_back(event);
    
    // Keep only recent events (limit to 1000)
    if (events_.size() > 1000) {
        events_.erase(events_.begin(), events_.begin() + 100);
    }
    
    // Notify listeners
    for (const auto& listener : listeners_) {
        try {
            listener(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error notifying event listener: ") + e.what(), 
                         "VisualizationEvent::emitEvent");
        }
    }
    
    NLM_LOG_INFO(std::string("Event emitted: ") + getEventTypeString(event.type), 
                "VisualizationEvent::emitEvent");
}

void VisualizationEvent::addListener(std::function<void(const EventData&)> listener) {
    if (listener) {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        listeners_.push_back(listener);
        
        NLM_LOG_INFO("Event listener added", "VisualizationEvent::addListener");
    }
}

void VisualizationEvent::removeListener(std::function<void(const EventData&)> listener) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    auto it = std::find(listeners_.begin(), listeners_.end(), listener);
    if (it != listeners_.end()) {
        listeners_.erase(it);
        
        NLM_LOG_INFO("Event listener removed", "VisualizationEvent::removeListener");
    }
}

void VisualizationEvent::clearListeners() {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    listeners_.clear();
    
    NLM_LOG_INFO("All event listeners removed", "VisualizationEvent::clearListeners");
}

std::vector<VisualizationEvent::EventData> VisualizationEvent::getRecentEvents(size_t count) const {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    if (count >= events_.size()) {
        return events_;
    }
    
    size_t startIndex = events_.size() - count;
    return std::vector<EventData>(events_.begin() + startIndex, events_.end());
}

void VisualizationEvent::clearEvents() {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    events_.clear();
    
    NLM_LOG_INFO("All events cleared", "VisualizationEvent::clearEvents");
}

std::string VisualizationEvent::getEventTypeString(EventType type) {
    switch (type) {
        case EventType::DATA_UPDATED: return "DATA_UPDATED";
        case EventType::INTERACTION: return "INTERACTION";
        case EventType::CONFIGURATION_CHANGED: return "CONFIGURATION_CHANGED";
        case EventType::RENDER_COMPLETE: return "RENDER_COMPLETE";
        case EventType::ERROR_OCCURRED: return "ERROR_OCCURRED";
        default: return "UNKNOWN";
    }
}

} // namespace nlm
