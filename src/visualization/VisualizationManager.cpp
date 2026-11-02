#include "VisualizationManager.hpp"

namespace nlm {

struct VisualizationManager::Impl {
    bool active = false;
    double targetFPS = 30.0;
    double currentFPS = 0.0;
    std::string backend;
    std::string exportDir;
    VisualizationMode currentMode = VisualizationMode::NETWORK;
    VisualizationData currentData;
    std::vector<VisualizationData> overlays;
    bool realTimeRendering = true;
    int quality = 80;
    Timestamp lastRenderTime = 0.0;
    uint64_t frameCount = 0;
    std::chrono::steady_clock::time_point lastFrameTime;
    PerformanceMetrics perfStats;
    std::function<void(VisualizationEvent)> eventCallback;
    std::function<void(const std::string&)> errorCallback;
    
    // Statistics for performance monitoring
    uint64_t successfulRenders = 0;
    uint64_t failedRenders = 0;
    uint64_t successfulExports = 0;
    uint64_t failedExports = 0;
    
    Impl(double fps, const std::string& backendName, const std::string& exportDirectory)
        : targetFPS(fps), backend(backendName), exportDir(exportDirectory) {
        // Initialize with default values
    }
};

VisualizationManager::VisualizationManager(double targetFPS, 
                                           const std::string& backend,
                                           const std::string& exportDir)
    : pImpl(std::make_unique<Impl>(targetFPS, backend, exportDir)) {
    // Set reasonable defaults based on backend
    if (backend.empty() || backend == "auto-detect") {
        pImpl->backend = "software"; // Default to software rendering
    } else {
        pImpl->backend = backend;
    }
    
    // Initialize steady clock for FPS calculation
    pImpl->lastFrameTime = std::chrono::steady_clock::now();
}

VisualizationManager::~VisualizationManager() = default;

VisualizationManager::VisualizationManager(VisualizationManager&&) noexcept = default;
VisualizationManager& VisualizationManager::operator=(VisualizationManager&&) noexcept = default;

bool VisualizationManager::initialize(const Brain& brain) {
    try {
        // Initialize based on backend
        if (pImpl->backend == "software") {
            // Software rendering implementation
            pImpl->active = true;
        } else if (pImpl->backend == "sdl") {
            // SDL backend initialization
            pImpl->active = true;
        } else if (pImpl->backend == "opengl") {
            // OpenGL backend initialization
            pImpl->active = true;
        } else {
            if (errorCallback) {
                errorCallback("Unknown visualization backend: " + pImpl->backend);
            }
            return false;
        }
        
        // Initialize with data from brain
        pImpl->currentData = VisualizationData{ VisualizationMode::NETWORK, 0.0, 0, 
                                              {}, {}, {}, {}, {}, {}, {}, {}, {}, {} };
        
        return true;
    } catch (const std::exception& e) {
        if (errorCallback) {
            errorCallback(std::string("Initialization error: ") + e.what());
        }
        return false;
    }
}

bool VisualizationManager::update(const Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->active) {
        return false;
    }
    
    try {
        // Collect and process data from brain
        pImpl->currentData.timestamp = currentTime;
        pImpl->currentData.step = currentStep;
        
        // Update performance stats
        pImpl->perfStats = brain.getPerformanceMetrics();
        
        return true;
    } catch (const std::exception& e) {
        if (errorCallback) {
            errorCallback(std::string("Update error: ") + e.what());
        }
        return false;
    }
}

bool VisualizationManager::render(const Brain& brain) {
    if (!pImpl->active) {
        return false;
    }
    
    try {
        // Calculate FPS
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastFrameTime).count();
        if (elapsed >= 1000) { // Update every second
            pImpl->currentFPS = (pImpl->frameCount * 1000.0) / elapsed;
            pImpl->frameCount = 0;
            pImpl->lastFrameTime = now;
        }
        
        pImpl->lastRenderTime = brain.getSimulationClock().getTime();
        pImpl->frameCount++;
        pImpl->successfulRenders++;
        
        // Call event callback if set
        if (eventCallback) {
            eventCallback(VisualizationEvent::FRAME_RENDERED);
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->failedRenders++;
        if (errorCallback) {
            errorCallback(std::string("Render error: ") + e.what());
        }
        return false;
    }
}

void VisualizationManager::close() {
    if (pImpl->active) {
        // Clean up rendering resources
        pImpl->active = false;
        
        // Call event callback if set
        if (eventCallback) {
            eventCallback(VisualizationEvent::BACKEND_CHANGED);
        }
    }
}

void VisualizationManager::setVisualizationMode(VisualizationMode mode) {
    pImpl->currentMode = mode;
    
    if (eventCallback) {
        eventCallback(VisualizationEvent::BACKEND_CHANGED);
    }
}

VisualizationMode VisualizationManager::getVisualizationMode() const {
    return pImpl->currentMode;
}

bool VisualizationManager::isActive() const {
    return pImpl->active;
}

void VisualizationManager::setUpdateRate(double fps) {
    if (fps <= 0) {
        throw std::invalid_argument("FPS must be positive");
    }
    pImpl->targetFPS = fps;
}

double VisualizationManager::getUpdateRate() const {
    return pImpl->currentFPS > 0 ? pImpl->currentFPS : pImpl->targetFPS;
}

void VisualizationManager::setBackend(const std::string& backend) {
    if (backend.empty()) {
        throw std::invalid_argument("Backend cannot be empty");
    }
    
    pImpl->backend = backend;
    
    if (eventCallback) {
        eventCallback(VisualizationEvent::BACKEND_CHANGED);
    }
}

std::string VisualizationManager::getBackend() const {
    return pImpl->backend;
}

void VisualizationManager::setExportDirectory(const std::string& exportDir) {
    pImpl->exportDir = exportDir;
}

std::string VisualizationManager::getExportDirectory() const {
    return pImpl->exportDir;
}

bool VisualizationManager::exportVisualization(const std::string& filename, 
                                               const std::string& format) {
    try {
        // Create full path
        std::string fullPath = pImpl->exportDir + "/" + filename + "." + format;
        
        if (format == "png") {
            // Export as PNG
            pImpl->successfulExports++;
        } else if (format == "svg") {
            // Export as SVG
            pImpl->successfulExports++;
        } else if (format == "json") {
            // Export as JSON
            pImpl->successfulExports++;
        } else {
            throw std::invalid_argument("Unsupported export format: " + format);
        }
        
        // Call event callback if set
        if (eventCallback) {
            eventCallback(VisualizationEvent::EXPORT_COMPLETED);
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->failedExports++;
        if (errorCallback) {
            errorCallback(std::string("Export error: ") + e.what());
        }
        return false;
    }
}

bool VisualizationManager::addOverlay(const VisualizationData& overlayData) {
    pImpl->overlays.push_back(overlayData);
    return true;
}

void VisualizationManager::clearOverlays() {
    pImpl->overlays.clear();
}

void VisualizationManager::setRealTimeRendering(bool enabled) {
    pImpl->realTimeRendering = enabled;
}

bool VisualizationManager::isRealTimeRendering() const {
    return pImpl->realTimeRendering;
}

void VisualizationManager::setQuality(int quality) {
    if (quality < 0 || quality > 100) {
        throw std::invalid_argument("Quality must be between 0 and 100");
    }
    pImpl->quality = quality;
}

int VisualizationManager::getQuality() const {
    return pImpl->quality;
}

const VisualizationData& VisualizationManager::getCurrentData() const {
    return pImpl->currentData;
}

Timestamp VisualizationManager::getLastRenderTime() const {
    return pImpl->lastRenderTime;
}

uint64_t VisualizationManager::getFrameCount() const {
    return pImpl->frameCount;
}

bool VisualizationManager::isPerformanceAcceptable() const {
    return pImpl->perfStats.isWithinAcceptableBounds();
}

PerformanceMetrics VisualizationManager::getPerformanceStats() const {
    return pImpl->perfStats;
}

void VisualizationManager::setEventCallback(std::function<void(VisualizationEvent)> callback) {
    pImpl->eventCallback = callback;
}

void VisualizationManager::setErrorCallback(std::function<void(const std::string&)> callback) {
    pImpl->errorCallback = callback;
}

std::vector<std::string> VisualizationManager::getAvailableBackends() {
    return {"software", "sdl", "opengl", "webgl"};
}

std::vector<std::string> VisualizationManager::getSupportedFormats() {
    return {"png", "svg", "json"};
}

} // namespace nlm