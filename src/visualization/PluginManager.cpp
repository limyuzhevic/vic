// Implementation of plugin manager
#include "PluginManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace nlm {

PluginManager::PluginManager()
    : autoLoad_(true), totalPlugins_(0), loadedPluginsCount_(0),
      enabledPluginsCount_(0), runningPluginsCount_(0) {
}

PluginManager::~PluginManager() {
    shutdown();
}

bool PluginManager::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PluginManager: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    
    // Create default plugin directory
    if (!pluginDirectory_.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(pluginDirectory_, ec);
    }
    
    // Scan for plugins if auto-load is enabled
    if (autoLoad_) {
        scanPluginDirectory();
    }
    
    NLM_LOG_INFO("PluginManager initialized successfully");
    return true;
}

void PluginManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Shutdown all plugins
    for (const auto& pair : plugins_) {
        if (pair.second) {
            pair.second->shutdown();
        }
    }
    
    // Clear all plugin containers
    plugins_.clear();
    pluginInfos_.clear();
    pluginEnabled_.clear();
    visualizationPlugins_.clear();
    analysisPlugins_.clear();
    debugPlugins_.clear();
    exportPlugins_.clear();
    eventHandlers_.clear();
    
    resetStatistics();
    
    NLM_LOG_INFO("PluginManager shutdown complete");
}

void PluginManager::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Update all enabled plugins
    for (const auto& pair : plugins_) {
        if (pair.second && pluginEnabled_[pair.first]) {
            pair.second->update(data);
        }
    }
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Validate plugin path
    if (pluginPath.empty()) {
        NLM_LOG_ERROR("Cannot load plugin: empty plugin path");
        return false;
    }
    
    std::error_code ec;
    bool exists = std::filesystem::exists(pluginPath, ec);
    
    if (!exists) {
        NLM_LOG_ERROR("Plugin not found: " + pluginPath);
        return false;
    }
    
    // Check if plugin is already loaded
    if (plugins_.find(pluginPath) != plugins_.end()) {
        NLM_LOG_INFO("Plugin already loaded: " + pluginPath);
        return true;
    }
    
    // Load plugin
    bool loaded = loadPluginFromFile(pluginPath);
    
    if (loaded) {
        invokePluginLoadCallback(pluginPath);
    } else {
        NLM_LOG_ERROR("Failed to load plugin: " + pluginPath);
    }
    
    return loaded;
}

bool PluginManager::unloadPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end()) {
        NLM_LOG_ERROR("Cannot unload plugin: not loaded - " + pluginId);
        return false;
    }
    
    // Shutdown plugin
    if (it->second) {
        it->second->shutdown();
    }
    
    // Remove from all containers
    plugins_.erase(it);
    pluginInfos_.erase(pluginId);
    pluginEnabled_.erase(pluginId);
    
    // Remove from typed containers
    visualizationPlugins_.erase(std::remove_if(visualizationPlugins_.begin(), visualizationPlugins_.end(),
                                               [pluginId](const std::shared_ptr<VisualizationPlugin>& plugin) {
                                                   return plugin && plugin->getPluginId() == pluginId;
                                               }),
                                visualizationPlugins_.end());
    
    analysisPlugins_.erase(std::remove_if(analysisPlugins_.begin(), analysisPlugins_.end(),
                                         [pluginId](const std::shared_ptr<AnalysisPlugin>& plugin) {
                                             return plugin && plugin->getPluginId() == pluginId;
                                         }),
                           analysisPlugins_.end());
    
    debugPlugins_.erase(std::remove_if(debugPlugins_.begin(), debugPlugins_.end(),
                                       [pluginId](const std::shared_ptr<DebugPlugin>& plugin) {
                                           return plugin && plugin->getPluginId() == pluginId;
                                       }),
                       debugPlugins_.end());
    
    exportPlugins_.erase(std::remove_if(exportPlugins_.begin(), exportPlugins_.end(),
                                        [pluginId](const std::shared_ptr<ExportPlugin>& plugin) {
                                            return plugin && plugin->getPluginId() == pluginId;
                                        }),
                         exportPlugins_.end());
    
    invokePluginUnloadCallback(pluginId);
    
    NLM_LOG_INFO("Plugin unloaded: " + pluginId);
    return true;
}

bool PluginManager::enablePlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end()) {
        NLM_LOG_ERROR("Cannot enable plugin: not loaded - " + pluginId);
        return false;
    }
    
    pluginEnabled_[pluginId] = true;
    enablePluginInternal(pluginId);
    
    invokePluginStatusChangeCallback(pluginId, true);
    
    NLM_LOG_INFO("Plugin enabled: " + pluginId);
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end()) {
        NLM_LOG_ERROR("Cannot disable plugin: not loaded - " + pluginId);
        return false;
    }
    
    disablePluginInternal(pluginId);
    pluginEnabled_[pluginId] = false;
    
    invokePluginStatusChangeCallback(pluginId, false);
    
    NLM_LOG_INFO("Plugin disabled: " + pluginId);
    return true;
}

bool PluginManager::isPluginLoaded(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.find(pluginId) != plugins_.end();
}

bool PluginManager::isPluginEnabled(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginEnabled_.find(pluginId);
    if (it == pluginEnabled_.end()) {
        return false;
    }
    
    return it->second;
}

PluginInfo PluginManager::getPluginInfo(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return PluginInfo{}; // Return empty info
    }
    
    return it->second;
}

std::vector<PluginInfo> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PluginInfo> infos;
    for (const auto& pair : pluginInfos_) {
        infos.push_back(pair.second);
    }
    
    return infos;
}

std::vector<PluginInfo> PluginManager::getEnabledPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PluginInfo> enabledInfos;
    
    for (const auto& pair : pluginInfos_) {
        if (pluginEnabled_.find(pair.first) != pluginEnabled_.end() && 
            pluginEnabled_.at(pair.first)) {
            enabledInfos.push_back(pair.second);
        }
    }
    
    return enabledInfos;
}

std::vector<std::string> PluginManager::getPluginIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> ids;
    for (const auto& pair : plugins_) {
        ids.push_back(pair.first);
    }
    
    return ids;
}

std::vector<std::string> PluginManager::getEnabledPluginIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> enabledIds;
    
    for (const auto& pair : plugins_) {
        if (pluginEnabled_.find(pair.first) != pluginEnabled_.end() && 
            pluginEnabled_.at(pair.first)) {
            enabledIds.push_back(pair.first);
        }
    }
    
    return enabledIds;
}

std::string PluginManager::getService(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : plugins_) {
        if (pair.second && pluginEnabled_[pair.first]) {
            std::string service = pair.second->getService(serviceName);
            if (!service.empty()) {
                return service;
            }
        }
    }
    
    return "";
}

bool PluginManager::provideService(const std::string& serviceName, const std::string& serviceData) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : plugins_) {
        if (pair.second && pluginEnabled_[pair.first]) {
            if (pair.second->provideService(serviceName, serviceData)) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::string> PluginManager::getAvailableServices() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> services;
    
    for (const auto& pair : plugins_) {
        if (pair.second && pluginEnabled_[pair.first]) {
            // This would normally iterate over all services provided by the plugin
            // For simplicity, we'll just add some sample services
            services.push_back("analysis_" + pair.first);
            services.push_back("visualization_" + pair.first);
        }
    }
    
    return services;
}

bool PluginManager::supportsFormat(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& plugin : visualizationPlugins_) {
        if (plugin && std::find(plugin->getSupportedFormats().begin(), 
                               plugin->getSupportedFormats().end(), format) != 
            plugin->getSupportedFormats().end()) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> PluginManager::getSupportedFormats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> formats;
    
    for (const auto& plugin : visualizationPlugins_) {
        if (plugin) {
            for (const auto& format : plugin->getSupportedFormats()) {
                if (std::find(formats.begin(), formats.end(), format) == formats.end()) {
                    formats.push_back(format);
                }
            }
        }
    }
    
    return formats;
}

void PluginManager::registerPlugin(const std::shared_ptr<Plugin>& plugin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!plugin) return;
    
    std::string pluginId = plugin->getPluginId();
    
    // Check if plugin is already registered
    if (plugins_.find(pluginId) != plugins_.end()) {
        NLM_LOG_WARNING("Plugin already registered: " + pluginId);
        return;
    }
    
    plugins_[pluginId] = plugin;
    pluginEnabled_[pluginId] = true; // Enabled by default
    
    // Update plugin info
    PluginInfo info;
    info.pluginId = plugin->getPluginId();
    info.pluginName = plugin->getPluginName();
    info.pluginVersion = plugin->getPluginVersion();
    info.isEnabled = true;
    info.loadedTime = std::chrono::steady_clock::now();
    
    // Add to typed containers based on plugin type
    if (auto vizPlugin = std::dynamic_pointer_cast<VisualizationPlugin>(plugin)) {
        visualizationPlugins_.push_back(vizPlugin);
        info.supportedFormats = vizPlugin->getSupportedFormats();
        info.providedServices = vizPlugin->getProvidedServices();
    } else if (auto analysisPlugin = std::dynamic_pointer_cast<AnalysisPlugin>(plugin)) {
        analysisPlugins_.push_back(analysisPlugin);
        info.supportedFormats = analysisPlugin->getSupportedFormats();
        info.providedServices = analysisPlugin->getProvidedServices();
    } else if (auto debugPlugin = std::dynamic_pointer_cast<DebugPlugin>(plugin)) {
        debugPlugins_.push_back(debugPlugin);
        info.supportedFormats = debugPlugin->getSupportedFormats();
        info.providedServices = debugPlugin->getProvidedServices();
    } else if (auto exportPlugin = std::dynamic_pointer_cast<ExportPlugin>(plugin)) {
        exportPlugins_.push_back(exportPlugin);
        info.supportedFormats = exportPlugin->getSupportedFormats();
        info.providedServices = exportPlugin->getProvidedServices();
    } else {
        info.supportedFormats = plugin->getSupportedFormats();
        info.providedServices = plugin->getProvidedServices();
    }
    
    pluginInfos_[pluginId] = info;
    
    totalPlugins_++;
    loadedPluginsCount_++;
    enabledPluginsCount_++;
    
    NLM_LOG_INFO("Plugin registered: " + pluginId);
}

void PluginManager::unregisterPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Remove plugin from all containers
    plugins_.erase(pluginId);
    pluginInfos_.erase(pluginId);
    pluginEnabled_.erase(pluginId);
    
    // Remove from typed containers
    visualizationPlugins_.erase(std::remove_if(visualizationPlugins_.begin(), visualizationPlugins_.end(),
                                               [pluginId](const std::shared_ptr<VisualizationPlugin>& plugin) {
                                                   return plugin && plugin->getPluginId() == pluginId;
                                               }),
                                visualizationPlugins_.end());
    
    analysisPlugins_.erase(std::remove_if(analysisPlugins_.begin(), analysisPlugins_.end(),
                                         [pluginId](const std::shared_ptr<AnalysisPlugin>& plugin) {
                                             return plugin && plugin->getPluginId() == pluginId;
                                         }),
                           analysisPlugins_.end());
    
    debugPlugins_.erase(std::remove_if(debugPlugins_.begin(), debugPlugins_.end(),
                                       [pluginId](const std::shared_ptr<DebugPlugin>& plugin) {
                                           return plugin && plugin->getPluginId() == pluginId;
                                       }),
                       debugPlugins_.end());
    
    exportPlugins_.erase(std::remove_if(exportPlugins_.begin(), exportPlugins_.end(),
                                        [pluginId](const std::shared_ptr<ExportPlugin>& plugin) {
                                            return plugin && plugin->getPluginId() == pluginId;
                                        }),
                         exportPlugins_.end());
    
    // Update statistics
    loadedPluginsCount_--;
    if (pluginEnabled_.find(pluginId) != pluginEnabled_.end() && pluginEnabled_[pluginId]) {
        enabledPluginsCount_--;
    }
    
    NLM_LOG_INFO("Plugin unregistered: " + pluginId);
}

std::shared_ptr<Plugin> PluginManager::getPlugin(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end()) {
        return nullptr;
    }
    
    return it->second;
}

void PluginManager::setPluginDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex_);
    pluginDirectory_ = directory;
}

std::string PluginManager::getPluginDirectory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pluginDirectory_;
}

std::vector<PluginInfo> PluginManager::discoverPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PluginInfo> discoveredPlugins;
    
    scanPluginDirectory();
    
    for (const auto& pair : pluginInfos_) {
        discoveredPlugins.push_back(pair.second);
    }
    
    return discoveredPlugins;
}

bool PluginManager::validatePlugin(const std::string& pluginPath) const {
    // Validate plugin file
    std::error_code ec;
    if (!std::filesystem::exists(pluginPath, ec)) {
        return false;
    }
    
    // Check if file is a valid plugin (placeholder)
    // In real implementation, this would check file format and metadata
    return true;
}

ExportReport PluginManager::exportUsingPlugin(const std::string& pluginId, const ExportOptions& options) const {
    ExportReport report;
    report.exportType = "plugin_export";
    report.outputPath = options.outputPath;
    report.success = false;
    
    // Find export plugin
    std::shared_ptr<ExportPlugin> exportPlugin;
    for (const auto& plugin : exportPlugins_) {
        if (plugin && plugin->getPluginId() == pluginId) {
            exportPlugin = plugin;
            break;
        }
    }
    
    if (!exportPlugin) {
        report.errorMessage = "Export plugin not found: " + pluginId;
        return report;
    }
    
    // Check if plugin supports the format
    if (!exportPlugin->supportsFormat(options.filename.substr(options.filename.find_last_of('.') + 1))) {
        report.errorMessage = "Export plugin does not support format: " + options.filename;
        return report;
    }
    
    // Prepare data for export
    std::vector<float> exportData;
    
    // Export using plugin
    report = exportPlugin->exportData(options, exportData);
    
    return report;
}

bool PluginManager::canExportFormat(const std::string& pluginId, const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& plugin : exportPlugins_) {
        if (plugin && plugin->getPluginId() == pluginId) {
            return plugin->supportsFormat(format);
        }
    }
    
    return false;
}

void PluginManager::broadcastEvent(const std::string& eventType, const std::map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Call event handlers
    auto it = eventHandlers_.find(eventType);
    if (it != eventHandlers_.end()) {
        for (const auto& handler : it->second) {
            try {
                handler(parameters);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in event handler for " + eventType + ": " + std::string(e.what()));
            }
        }
    }
    
    // Notify all enabled plugins
    for (const auto& pair : plugins_) {
        if (pair.second && pluginEnabled_[pair.first]) {
            pair.second->handleEvent(eventType, parameters);
        }
    }
}

void PluginManager::registerEventHandler(const std::string& eventType, 
                                        std::function<bool(const std::map<std::string, std::string>&)> handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    eventHandlers_[eventType].push_back(handler);
}

void PluginManager::addVisualizationPlugin(const std::shared_ptr<VisualizationPlugin>& plugin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (plugin) {
        visualizationPlugins_.push_back(plugin);
    }
}

std::vector<std::shared_ptr<VisualizationPlugin>> PluginManager::getVisualizationPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return visualizationPlugins_;
}

void PluginManager::executeVisualizationPlugins(const VisualizationData& data, const ExportOptions& options) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& plugin : visualizationPlugins_) {
        if (plugin) {
            try {
                plugin->visualize(data, options);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in visualization plugin " + plugin->getPluginId() + ": " + std::string(e.what()));
            }
        }
    }
}

void PluginManager::addAnalysisPlugin(const std::shared_ptr<AnalysisPlugin>& plugin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (plugin) {
        analysisPlugins_.push_back(plugin);
    }
}

std::vector<std::shared_ptr<AnalysisPlugin>> PluginManager::getAnalysisPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return analysisPlugins_;
}

std::vector<std::string> PluginManager::executeAnalysisPlugins(const VisualizationData& data) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> results;
    
    for (const auto& plugin : analysisPlugins_) {
        if (plugin) {
            try {
                std::vector<std::string> analysis = plugin->analyze(data);
                results.insert(results.end(), analysis.begin(), analysis.end());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in analysis plugin " + plugin->getPluginId() + ": " + std::string(e.what()));
            }
        }
    }
    
    return results;
}

std::map<std::string, double> PluginManager::computeMetricsUsingPlugins(const VisualizationData& data) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, double> metrics;
    
    for (const auto& plugin : analysisPlugins_) {
        if (plugin) {
            try {
                std::map<std::string, double> pluginMetrics = plugin->computeMetrics(data);
                metrics.insert(pluginMetrics.begin(), pluginMetrics.end());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error computing metrics in plugin " + plugin->getPluginId() + ": " + std::string(e.what()));
            }
        }
    }
    
    return metrics;
}

void PluginManager::addDebugPlugin(const std::shared_ptr<DebugPlugin>& plugin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (plugin) {
        debugPlugins_.push_back(plugin);
    }
}

std::vector<std::shared_ptr<DebugPlugin>> PluginManager::getDebugPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return debugPlugins_;
}

std::string PluginManager::executeDebugPlugins(const std::string& target) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream result;
    
    for (const auto& plugin : debugPlugins_) {
        if (plugin) {
            try {
                std::string debugResult = plugin->debug(target);
                if (!debugResult.empty()) {
                    result << "[" << plugin->getPluginId() << "] " << debugResult << "\n\n";
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in debug plugin " + plugin->getPluginId() + ": " + std::string(e.what()));
            }
        }
    }
    
    return result.str();
}

std::vector<std::string> PluginManager::getDebugInfoUsingPlugins(const std::string& target) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> debugInfo;
    
    for (const auto& plugin : debugPlugins_) {
        if (plugin) {
            try {
                std::vector<std::string> info = plugin->getDebugInfo(target);
                debugInfo.insert(debugInfo.end(), info.begin(), info.end());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error getting debug info from plugin " + plugin->getPluginId() + ": " + std::string(e.what()));
            }
        }
    }
    
    return debugInfo;
}

void PluginManager::addExportPlugin(const std::shared_ptr<ExportPlugin>& plugin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (plugin) {
        exportPlugins_.push_back(plugin);
    }
}

std::vector<std::shared_ptr<ExportPlugin>> PluginManager::getExportPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return exportPlugins_;
}

ExportReport PluginManager::executeExportPlugins(const ExportOptions& options, const std::vector<float>& data) const {
    ExportReport report;
    report.exportType = "plugin_export";
    report.outputPath = options.outputPath;
    report.success = false;
    
    // Use the export plugin with highest priority
    std::vector<std::string> pluginOrder = getExportPluginOrder();
    
    for (const auto& pluginId : pluginOrder) {
        std::shared_ptr<ExportPlugin> exportPlugin;
        for (const auto& plugin : exportPlugins_) {
            if (plugin && plugin->getPluginId() == pluginId) {
                exportPlugin = plugin;
                break;
            }
        }
        
        if (exportPlugin && exportPlugin->supportsFormat(options.filename.substr(options.filename.find_last_of('.') + 1))) {
            report = exportPlugin->exportData(options, data);
            if (report.success) {
                return report;
            }
        }
    }
    
    report.errorMessage = "No suitable export plugin found";
    return report;
}

bool PluginManager::isPluginRunning(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end() || !it->second) {
        return false;
    }
    
    // Check if plugin provides a service
    return true;
}

std::chrono::steady_clock::time_point PluginManager::getPluginLoadTime(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return std::chrono::steady_clock::time_point();
    }
    
    return it->second.loadedTime;
}

double PluginManager::getPluginUptime(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return 0.0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.loadedTime);
    
    return static_cast<double>(duration.count());
}

void PluginManager::updateStatistics() {
    // Update plugin statistics
    size_t runningCount = 0;
    
    for (const auto& pair : plugins_) {
        if (pair.second && pluginEnabled_[pair.first]) {
            runningCount++;
        }
    }
    
    runningPluginsCount_ = runningCount;
    
    // Update enabled count
    enabledPluginsCount_ = 0;
    for (const auto& pair : pluginEnabled_) {
        if (pair.second) {
            enabledPluginsCount_++;
        }
    }
}

void PluginManager::resetStatistics() {
    totalPlugins_ = 0;
    loadedPluginsCount_ = 0;
    enabledPluginsCount_ = 0;
    runningPluginsCount_ = 0;
}

void PluginManager::logPluginError(const std::string& pluginId, const std::string& error) {
    NLM_LOG_ERROR("Plugin \"" + pluginId + "\" error: " + error);
}

void PluginManager::updateExportPluginOrder() {
    // Update export plugin order based on priorities
    // This is a placeholder - real implementation would sort by priority
}

bool PluginManager::comparePluginPriorities(const std::string& pluginId1, const std::string& pluginId2) const {
    // Compare plugin priorities
    // This is a placeholder - real implementation would compare priorities
    return true;
}

void PluginManager::updatePluginServices() const {
    // Update plugin services
    // This is a placeholder - real implementation would update services
}

void PluginManager::invokePluginLoadCallback(const std::string& pluginId) const {
    for (const auto& callback : pluginLoadCallbacks_) {
        callback(pluginId);
    }
}

void PluginManager::invokePluginUnloadCallback(const std::string& pluginId) const {
    for (const auto& callback : pluginUnloadCallbacks_) {
        callback(pluginId);
    }
}

void PluginManager::invokePluginErrorCallback(const std::string& pluginId, const std::string& error) const {
    for (const auto& callback : pluginErrorCallbacks_) {
        callback(pluginId, error);
    }
}

void PluginManager::invokePluginStatusChangeCallback(const std::string& pluginId, bool enabled) const {
    for (const auto& callback : pluginStatusChangeCallbacks_) {
        callback(pluginId, enabled);
    }
}

std::vector<std::string> PluginManager::getPluginCommands(const std::string& pluginId) const {
    std::vector<std::string> commands;
    
    auto it = plugins_.find(pluginId);
    if (it != plugins_.end() && it->second) {
        // Get available commands from plugin
        // This is a placeholder - real implementation would get commands
        commands.push_back("status");
        commands.push_back("config");
        commands.push_back("help");
    }
    
    return commands;
}

std::string PluginManager::executePluginCommand(const std::string& pluginId, const std::string& command, 
                                               const std::map<std::string, std::string>& parameters) {
    std::stringstream result;
    
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end() || !it->second) {
        return "Plugin not found: " + pluginId;
    }
    
    // Execute command
    // This is a placeholder - real implementation would execute commands
    result << "Executed command \"" << command << "\" for plugin \"" << pluginId << "\"\n";
    
    return result.str();
}

void PluginManager::registerPluginLoadCallback(std::function<void(const std::string&)> callback) {
    pluginLoadCallbacks_.push_back(callback);
}

void PluginManager::registerPluginUnloadCallback(std::function<void(const std::string&)> callback) {
    pluginUnloadCallbacks_.push_back(callback);
}

void PluginManager::registerPluginErrorCallback(std::function<void(const std::string&, const std::string&)> callback) {
    pluginErrorCallbacks_.push_back(callback);
}

void PluginManager::registerPluginStatusChangeCallback(std::function<void(const std::string&, bool)> callback) {
    pluginStatusChangeCallbacks_.push_back(callback);
}

std::string PluginManager::getPluginName(const std::string& pluginId) const {
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return "";
    }
    
    return it->second.pluginName;
}

std::string PluginManager::getPluginVersion(const std::string& pluginId) const {
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return "";
    }
    
    return it->second.pluginVersion;
}

std::string PluginManager::getPluginDescription(const std::string& pluginId) const {
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return "";
    }
    
    return it->second.pluginDescription;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginId) const {
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return std::vector<std::string>();
    }
    
    return it->second.dependencies;
}

std::map<std::string, std::string> PluginManager::getPluginMetadata(const std::string& pluginId) const {
    auto it = pluginInfos_.find(pluginId);
    if (it == pluginInfos_.end()) {
        return std::map<std::string, std::string>();
    }
    
    return it->second.metadata;
}

bool PluginManager::loadPluginFromFile(const std::string& pluginPath) {
    // Load plugin from file
    // This is a placeholder - real implementation would load plugin from file
    
    // Generate unique plugin ID
    std::string pluginId = pluginPath;
    
    // Create plugin info
    PluginInfo info;
    info.pluginId = pluginId;
    info.pluginName = "Unknown Plugin";
    info.pluginVersion = "1.0.0";
    info.pluginDescription = "Plugin loaded from " + pluginPath;
    info.isEnabled = true;
    info.loadedTime = std::chrono::steady_clock::now();
    
    // Register plugin
    registerPlugin(std::shared_ptr<Plugin>(new Plugin));
    
    // Store plugin info
    pluginInfos_[pluginId] = info;
    
    NLM_LOG_INFO("Loaded plugin from file: " + pluginPath);
    return true;
}

void PluginManager::scanPluginDirectory() const {
    // Scan plugin directory for plugins
    // This is a placeholder - real implementation would scan directory
    
    std::vector<std::string> pluginFiles;
    
    // For now, we'll just create a dummy plugin
    std::string pluginPath = pluginDirectory_ + "/dummy_plugin.so";
    if (validatePlugin(pluginPath)) {
        pluginFiles.push_back(pluginPath);
    }
    
    // Load discovered plugins
    for (const auto& path : pluginFiles) {
        loadPlugin(path);
    }
}

bool PluginManager::validatePluginCompatibility(const PluginInfo& info) const {
    // Validate plugin compatibility
    // This is a placeholder - real implementation would validate compatibility
    return true;
}

bool PluginManager::checkDependencies(const PluginInfo& info) const {
    // Check plugin dependencies
    // This is a placeholder - real implementation would check dependencies
    return true;
}

void PluginManager::initializePlugin(const std::string& pluginId) {
    // Initialize plugin
    // This is a placeholder - real implementation would initialize plugin
}

void PluginManager::shutdownPlugin(const std::string& pluginId) {
    // Shutdown plugin
    // This is a placeholder - real implementation would shutdown plugin
}

void PluginManager::enablePluginInternal(const std::string& pluginId) {
    // Enable plugin internally
    // This is a placeholder - real implementation would enable plugin
}

void PluginManager::disablePluginInternal(const std::string& pluginId) {
    // Disable plugin internally
    // This is a placeholder - real implementation would disable plugin
}

std::vector<std::string> PluginManager::getExportPluginOrder() const {
    std::vector<std::string> orderedPlugins;
    
    for (const auto& pair : plugins_) {
        if (pluginEnabled_.find(pair.first) != pluginEnabled_.end() && 
            pluginEnabled_.at(pair.first)) {
            orderedPlugins.push_back(pair.first);
        }
    }
    
    return orderedPlugins;
}

std::vector<std::string> PluginManager::getPluginErrors() const {
    std::vector<std::string> errors;
    
    // Get plugin errors
    // This is a placeholder - real implementation would get errors
    
    return errors;
}

void PluginManager::clearPluginErrors() {
    // Clear plugin errors
    // This is a placeholder - real implementation would clear errors
}

void PluginManager::setAutoLoad(bool autoLoad) {
    std::lock_guard<std::mutex> lock(mutex_);
    autoLoad_ = autoLoad;
}

void PluginManager::setPluginSearchPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    pluginSearchPath_ = path;
}

bool PluginManager::getAutoLoad() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return autoLoad_;
}

std::string PluginManager::getPluginSearchPath() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pluginSearchPath_;
}

void PluginManager::setExportPluginPriority(const std::string& pluginId, int priority) {
    // Set export plugin priority
    // This is a placeholder - real implementation would set priority
}

int PluginManager::getExportPluginPriority(const std::string& pluginId) const {
    // Get export plugin priority
    // This is a placeholder - real implementation would get priority
    return 0;
}

void Plugin::registerPlugin(std::shared_ptr<Plugin> plugin) {
    // Register plugin
    // This is a placeholder - real implementation would register plugin
}

Plugin::Plugin() {
    // Constructor
}

Plugin::~Plugin() {
    // Destructor
}

} // namespace nlm
