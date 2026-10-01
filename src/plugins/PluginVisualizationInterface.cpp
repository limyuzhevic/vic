#include "PluginVisualizationInterface.hpp"
#include "PluginCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

VisualizationPluginBase::VisualizationPluginBase(const std::string& name, const std::string& version)
    : name(name), version(version), visualizationInitialized(false), loadStatus(PluginLoadStatus::NotLoaded) {
    metadata.name = name;
    metadata.version = version;
    metadata.creationDate = std::chrono::system_clock::now();
    metadata.lastUpdated = metadata.creationDate;
    metadata.securityLevel = PluginSecurityLevel::Medium;
    
    settings.theme = "default";
    settings.interactive = true;
    settings.animated = false;
    settings.updateInterval = 1.0;
    
    visualizationInitialized = false;
}

std::string VisualizationPluginBase::getName() const {
    return name;
}

std::string VisualizationPluginBase::getVersion() const {
    return version;
}

std::string VisualizationPluginBase::getDescription() const {
    return metadata.description;
}

std::string VisualizationPluginBase::getAuthor() const {
    return metadata.author;
}

const PluginMetadata& VisualizationPluginBase::getMetadata() const {
    return metadata;
}

VisualizationData VisualizationPluginBase::getVisualizationData() const {
    VisualizationData data;
    data.name = name;
    data.type = VisualizationType::Custom;
    
    for (const auto& pair : visualizationData) {
        data.data[pair.first] = pair.second.data;
        if (!pair.second.metadata.empty()) {
            data.metadata[pair.first] = {pair.second.metadata.begin(), pair.second.metadata.end()};
        }
    }
    
    data.format = "json";
    data.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    
    return data;
}

bool VisualizationPluginBase::setVisualizationData(const VisualizationData& data) {
    visualizationData[data.name] = data;
    return true;
}

bool VisualizationPluginBase::updateVisualizationData(const std::string& key, const std::vector<float>& data) {
    auto it = visualizationData.find(key);
    if (it != visualizationData.end()) {
        it->second.data = data;
        return true;
    }
    
    return false;
}

VisualizationPluginSettings VisualizationPluginBase::getSettings() const {
    return settings;
}

bool VisualizationPluginBase::setSettings(const VisualizationPluginSettings& newSettings) {
    settings = newSettings;
    return true;
}

bool VisualizationPluginBase::updateSetting(const std::string& key, const std::string& value) {
    settings.style[key] = value;
    return true;
}

bool VisualizationPluginBase::updateParameter(const std::string& key, float value) {
    settings.parameters[key] = value;
    return true;
}

bool VisualizationPluginBase::createVisualization() {
    visualizationInitialized = true;
    return true;
}

bool VisualizationPluginBase::updateVisualization(double deltaTime) {
    if (!visualizationInitialized) {
        return false;
    }
    
    // Update visualization based on parameters
    for (const auto& pair : visualizationParameters) {
        // TODO: Update visualization based on parameters
    }
    
    return true;
}

bool VisualizationPluginBase::destroyVisualization() {
    visualizationInitialized = false;
    return true;
}

bool VisualizationPluginBase::refreshVisualization() {
    if (!visualizationInitialized) {
        return false;
    }
    
    // Refresh visualization data
    return true;
}

bool VisualizationPluginBase::handleInteraction(const std::string& event, const std::map<std::string, std::string>& data) {
    // Default implementation: handle common events
    if (event == "mouse_click") {
        // Handle mouse click event
        return true;
    } else if (event == "mouse_move") {
        // Handle mouse move event
        return true;
    } else if (event == "key_press") {
        // Handle key press event
        return true;
    }
    
    return false;
}

bool VisualizationPluginBase::setInteractionMode(const std::string& mode) {
    interactionMode = mode;
    return true;
}

std::string VisualizationPluginBase::getInteractionMode() const {
    return interactionMode;
}

bool VisualizationPluginBase::exportVisualization(const std::string& filepath, const std::string& format) {
    // Default implementation: export to file
    return true;
}

bool VisualizationPluginBase::exportVisualizationData(const std::string& filepath, const std::string& format) {
    // Default implementation: export data to file
    return true;
}

std::vector<std::string> VisualizationPluginBase::getExportFormats() const {
    return {"png", "jpg", "json", "csv", "pdf"};
}

bool VisualizationPluginBase::addFilter(const std::string& filter) {
    filters.push_back(filter);
    return true;
}

bool VisualizationPluginBase::removeFilter(const std::string& filter) {
    auto it = std::find(filters.begin(), filters.end(), filter);
    if (it != filters.end()) {
        filters.erase(it);
        return true;
    }
    return false;
}

bool VisualizationPluginBase::clearFilters() {
    filters.clear();
    return true;
}

std::vector<std::string> VisualizationPluginBase::getFilters() const {
    return filters;
}

bool VisualizationPluginBase::applyFilters() {
    // Default implementation: apply filters
    return true;
}

bool VisualizationPluginBase::setTheme(const std::string& theme) {
    settings.theme = theme;
    return true;
}

std::string VisualizationPluginBase::getTheme() const {
    return settings.theme;
}

std::vector<std::string> VisualizationPluginBase::getAvailableThemes() const {
    return {"default", "dark", "light", "blue", "green", "red"};
}

bool VisualizationPluginBase::prepareVisualization() {
    // Default implementation: prepare visualization resources
    return createVisualization();
}

bool VisualizationPluginBase::finalizeVisualization() {
    // Default implementation: clean up visualization resources
    return destroyVisualization();
}

bool VisualizationPluginBase::cleanupVisualization() {
    // Default implementation: cleanup visualization
    clearFilters();
    return true;
}

bool VisualizationPluginBase::loadConfiguration(const std::string& filepath) {
    // TODO: Implement configuration loading
    return true;
}

bool VisualizationPluginBase::saveConfiguration(const std::string& filepath) const {
    // TODO: Implement configuration saving
    return true;
}

bool VisualizationPluginBase::resetConfiguration() {
    settings = VisualizationPluginSettings();
    filters.clear();
    return true;
}

void VisualizationPluginBase::addVisualizationEventCallback(VisualizationPluginEventCallback callback) {
    visualizationEventCallbacks.push_back(callback);
}

void VisualizationPluginBase::removeVisualizationEventCallback(VisualizationPluginEventCallback callback) {
    auto it = std::find(visualizationEventCallbacks.begin(), visualizationEventCallbacks.end(), callback);
    if (it != visualizationEventCallbacks.end()) {
        visualizationEventCallbacks.erase(it);
    }
}

void VisualizationPluginBase::broadcastVisualizationEvent(const VisualizationPluginEvent& event) {
    for (const auto& callback : visualizationEventCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("VisualizationPluginBase: Exception in visualization event callback: " + std::string(e.what()));
        }
    }
}

bool VisualizationPluginBase::initializeVisualization() {
    // Default implementation: initialize visualization
    return prepareVisualization();
}

bool VisualizationPluginBase::shutdownVisualization() {
    // Default implementation: shutdown visualization
    return cleanupVisualization();
}

bool VisualizationPluginBase::validateVisualizationConfiguration() {
    // Default implementation: validate configuration
    return true;
}

bool VisualizationPluginBase::updateVisualizationParameters(const std::map<std::string, float>& parameters) {
    visualizationParameters = parameters;
    return true;
}

std::map<std::string, float> VisualizationPluginBase::getVisualizationParameters() const {
    return visualizationParameters;
}

bool VisualizationPluginBase::onVisualizationEvent(const VisualizationPluginEvent& event) {
    // Default implementation: broadcast event
    broadcastVisualizationEvent(event);
    return true;
}

bool VisualizationPluginBase::onVisualizationUpdate(double deltaTime) {
    // Default implementation: update visualization
    return updateVisualization(deltaTime);
}

bool VisualizationPluginBase::onVisualizationError(const std::string& error) {
    // Default implementation: log error
    NLM_LOG_ERROR("VisualizationPluginBase: Visualization error: " + error);
    return true;
}

// Plugin lifecycle callbacks implementation
void VisualizationPluginBase::onPluginLoadStarted(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Loading;
}

void VisualizationPluginBase::onPluginLoadCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Loaded;
    } else {
        loadStatus = PluginLoadStatus::Failed;
    }
}

void VisualizationPluginBase::onPluginLoadFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void VisualizationPluginBase::onPluginStartRequested(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Starting;
}

void VisualizationPluginBase::onPluginStartCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Running;
    } else {
        loadStatus = PluginLoadStatus::Failed;
    }
}

void VisualizationPluginBase::onPluginStartFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void VisualizationPluginBase::onPluginStopRequested(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Stopping;
}

void VisualizationPluginBase::onPluginStopCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Stopped;
    } else {
        loadStatus = PluginLoadStatus::Errored;
    }
}

void VisualizationPluginBase::onPluginStopFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void VisualizationPluginBase::onPluginUnloadRequested(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Unloading;
}

void VisualizationPluginBase::onPluginUnloadCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Unloaded;
    } else {
        loadStatus = PluginLoadStatus::Errored;
    }
}

void VisualizationPluginBase::onPluginUnloadFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void VisualizationPluginBase::onPluginUpdated(const std::string& name, double deltaTime) {
    // Default implementation - can be overridden
    // Update plugin based on deltaTime
}

void VisualizationPluginBase::onPluginRecovered(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Loaded;
}

void VisualizationPluginBase::onSystemInitialized() {
    // Default implementation - can be overridden
}

void VisualizationPluginBase::onSystemShutdown() {
    // Default implementation - can be overridden
}

void VisualizationPluginBase::onSystemError(const std::string& error) {
    // Default implementation - can be overridden
    NLM_LOG_ERROR("VisualizationPluginBase: System error: " + error);
}

void VisualizationPluginBase::onSystemWarning(const std::string& warning) {
    // Default implementation - can be overridden
    NLM_LOG_WARNING("VisualizationPluginBase: System warning: " + warning);
}

} // namespace nlm