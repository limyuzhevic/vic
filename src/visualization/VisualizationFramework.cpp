#include "VisualizationFramework.hpp"
#include <chrono>

namespace nlm {

struct VisualizationFramework::Impl {
    std::map<std::string, std::unique_ptr<VisualizationInterface>> visualizers;
    std::map<std::string, VisualizationConfig> configs;
    std::vector<std::string> availableThemes;
    std::vector<std::string> availableStyles;
    std::string currentTheme;
    std::string currentStyle;
    bool running;
    double updateRate;
    double lastFrameTime;
    std::chrono::steady_clock::time_point startTime;
    
    Impl() : running(false), updateRate(60.0), lastFrameTime(0.0) {
        startTime = std::chrono::steady_clock::now();
        availableThemes = {"default", "dark", "light", "neural"};
        availableStyles = {"minimal", "detailed", "scientific", "abstract"};
        currentTheme = "default";
        currentStyle = "detailed";
    }
};

VisualizationFramework::VisualizationFramework() : pImpl(std::make_unique<Impl>()) {}

VisualizationFramework::~VisualizationFramework() = default;

bool VisualizationFramework::initialize() {
    return initialize(VisualizationConfig{ "default", VisualizationType::NETWORK, VisualizationBackend::CONSOLE, "png", "", 60.0 });
}

bool VisualizationFramework::initialize(const VisualizationConfig& config) {
    pImpl->running = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    
    // Create default visualizer based on backend
    if (config.backend == VisualizationBackend::CONSOLE) {
        pImpl->visualizers[config.name] = std::make_unique<ConsoleVisualizer>();
    } else if (config.backend == VisualizationBackend::GUI) {
        pImpl->visualizers[config.name] = std::make_unique<GUIVisualizer>();
    } else if (config.backend == VisualizationBackend::WEB) {
        pImpl->visualizers[config.name] = std::make_unique<WebVisualizer>();
    }
    
    if (pImpl->visualizers.find(config.name) != pImpl->visualizers.end()) {
        pImpl->configs[config.name] = config;
        pImpl->visualizers[config.name]->initialize();
        return true;
    }
    
    return false;
}

void VisualizationFramework::update() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->startTime).count();
    pImpl->lastFrameTime = elapsed;
    
    for (auto& pair : pImpl->visualizers) {
        pair.second->update();
    }
}

void VisualizationFramework::render() {
    for (auto& pair : pImpl->visualizers) {
        pair.second->render();
    }
}

void VisualizationFramework::close() {
    pImpl->running = false;
    for (auto& pair : pImpl->visualizers) {
        pair.second->close();
    }
}

bool VisualizationFramework::addVisualizer(std::unique_ptr<VisualizationInterface> visualizer) {
    std::string name = "visualizer_" + std::to_string(pImpl->visualizers.size());
    pImpl->visualizers[name] = std::move(visualizer);
    return true;
}

bool VisualizationFramework::removeVisualizer(const std::string& name) {
    auto it = pImpl->visualizers.find(name);
    if (it != pImpl->visualizers.end()) {
        it->second->close();
        pImpl->visualizers.erase(it);
        pImpl->configs.erase(name);
        return true;
    }
    return false;
}

VisualizationInterface* VisualizationFramework::getVisualizer(const std::string& name) const {
    auto it = pImpl->visualizers.find(name);
    if (it != pImpl->visualizers.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool VisualizationFramework::createVisualization(const VisualizationConfig& config) {
    if (pImpl->visualizers.find(config.name) != pImpl->visualizers.end()) {
        return false;
    }
    
    if (config.backend == VisualizationBackend::CONSOLE) {
        pImpl->visualizers[config.name] = std::make_unique<ConsoleVisualizer>();
    } else if (config.backend == VisualizationBackend::GUI) {
        pImpl->visualizers[config.name] = std::make_unique<GUIVisualizer>();
    } else if (config.backend == VisualizationBackend::WEB) {
        pImpl->visualizers[config.name] = std::make_unique<WebVisualizer>();
    }
    
    if (pImpl->visualizers.find(config.name) != pImpl->visualizers.end()) {
        pImpl->configs[config.name] = config;
        pImpl->visualizers[config.name]->initialize();
        return true;
    }
    
    return false;
}

bool VisualizationFramework::updateVisualization(const std::string& name, const VisualizationConfig& config) {
    auto vizIt = pImpl->visualizers.find(name);
    if (vizIt == pImpl->visualizers.end()) {
        return false;
    }
    
    pImpl->configs[name] = config;
    return true;
}

bool VisualizationFramework::destroyVisualization(const std::string& name) {
    return removeVisualizer(name);
}

bool VisualizationFramework::renderVisualization(const std::string& name) {
    auto it = pImpl->visualizers.find(name);
    if (it != pImpl->visualizers.end()) {
        it->second->render();
        return true;
    }
    return false;
}

bool VisualizationFramework::renderAllVisualizations() {
    for (auto& pair : pImpl->visualizers) {
        pair.second->render();
    }
    return true;
}

bool VisualizationFramework::exportData(const std::string& visualizationName, const std::string& filePath, const std::string& format) const {
    auto it = pImpl->visualizers.find(visualizationName);
    if (it != pImpl->visualizers.end()) {
        return it->second->exportData(filePath, format);
    }
    return false;
}

bool VisualizationFramework::importData(const std::string& visualizationName, const std::string& filePath) {
    auto it = pImpl->visualizers.find(visualizationName);
    if (it != pImpl->visualizers.end()) {
        return it->second->importData(filePath);
    }
    return false;
}

void VisualizationFramework::setTheme(const std::string& theme) {
    auto it = std::find(pImpl->availableThemes.begin(), pImpl->availableThemes.end(), theme);
    if (it != pImpl->availableThemes.end()) {
        pImpl->currentTheme = theme;
    }
}

void VisualizationFramework::setStyle(const std::string& style) {
    auto it = std::find(pImpl->availableStyles.begin(), pImpl->availableStyles.end(), style);
    if (it != pImpl->availableStyles.end()) {
        pImpl->currentStyle = style;
    }
}

std::vector<std::string> VisualizationFramework::getAvailableThemes() const {
    return pImpl->availableThemes;
}

std::vector<std::string> VisualizationFramework::getAvailableStyles() const {
    return pImpl->availableStyles;
}

std::vector<std::string> VisualizationFramework::getActiveVisualizations() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->visualizers) {
        names.push_back(pair.first);
    }
    return names;
}

bool VisualizationFramework::isRunning() const {
    return pImpl->running;
}

void VisualizationFramework::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    for (auto& pair : pImpl->visualizers) {
        pair.second->setUpdateRate(hz);
    }
}

double VisualizationFramework::getUpdateRate() const {
    return pImpl->updateRate;
}

double VisualizationFramework::getMemoryUsage() const {
    double totalMemory = 0.0;
    for (const auto& pair : pImpl->visualizers) {
        totalMemory += pair.second->getMemoryUsage();
    }
    return totalMemory;
}

double VisualizationFramework::getFrameTime() const {
    return pImpl->lastFrameTime;
}

int VisualizationFramework::getActiveVisualizationsCount() const {
    return static_cast<int>(pImpl->visualizers.size());
}

} // namespace nlm
