#include "VisualizationEngine.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace nlm {

struct VisualizationEngine::Impl {
    // Window and rendering
    SDL_Window* window;
    SDL_Renderer* renderer;
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    bool fullscreen;
    bool vsync;
    
    // Timing and performance
    double targetFPS;
    double currentFPS;
    std::chrono::microseconds lastRenderTime;
    std::chrono::microseconds lastUpdateTime;
    uint64_t frameCount;
    
    // Data management
    VisualizationData currentData;
    FrameBuffer frameBuffer;
    std::mutex dataMutex;
    
    // Visualizer management
    std::map<std::string, std::shared_ptr<VisualizationInterface>> visualizers;
    std::string activeVisualizerName;
    UpdatePriority currentUpdatePriority;
    
    // Rendering queue
    std::priority_queue<RenderCommand> renderQueue;
    std::mutex renderQueueMutex;
    
    // Callbacks
    std::vector<std::function<void(SimulationStep, Timestamp)>> updateCallbacks;
    std::vector<std::function<void()>> renderCallbacks;
    
    // Configuration
    std::string currentTheme;
    std::map<std::string, std::map<std::string, std::string>> styles;
    bool debugMode;
    bool highPerformanceMode;
    
    // Performance metrics
    struct InternalMetrics {
        uint64_t drawCalls;
        uint64_t vertexCount;
        size_t memoryUsage;
        std::chrono::microseconds totalRenderTime;
        uint32_t frameTimeSamples;
        std::vector<std::chrono::microseconds> frameTimes;
    } metrics;
    
    // Plugin system
    std::map<std::string, void*> loadedPlugins;
    std::map<std::string, std::string> pluginPaths;
    
    // State management
    bool initialized;
    bool running;
    
    Impl() : window(nullptr), renderer(nullptr), windowWidth(1280), windowHeight(720),
             targetFPS(60.0), currentFPS(0.0), frameCount(0), frameBuffer(60),
             currentUpdatePriority(UpdatePriority::Normal), debugMode(false),
             highPerformanceMode(false), initialized(false), running(false) {
        frameTimes.reserve(100);
    }
};

VisualizationEngine::VisualizationEngine() : pImpl(std::make_unique<Impl>()) {
}

VisualizationEngine::~VisualizationEngine() {
    shutdown();
}

bool VisualizationEngine::initialize(int width, int height, const std::string& title) {
    if (pImpl->initialized) {
        shutdown();n    }
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_image and SDL_ttf
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    
    // Create window
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
    if (pImpl->highPerformanceMode) {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    
    pImpl->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width, height, windowFlags);
    if (!pImpl->window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    
    pImpl->windowWidth = width;
    pImpl->windowHeight = height;
    pImpl->windowTitle = title;
    
    // Create renderer with hardware acceleration
    int renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    if (!pImpl->vsync) {
        renderFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    
    pImpl->renderer = SDL_CreateRenderer(pImpl->window, -1, renderFlags);
    if (!pImpl->renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(pImpl->window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    
    pImpl->initialized = true;
    pImpl->running = true;
    
    // Load default theme and styles
    loadDefaultTheme();
    
    return true;
}

void VisualizationEngine::shutdown() {
    if (!pImpl->initialized) {
        return;
    }
    
    pImpl->running = false;
    
    // Unload plugins
    for (auto& plugin : pImpl->loadedPlugins) {
        if (plugin.second) {
            dlclose(plugin.second);
        }
    }
    pImpl->loadedPlugins.clear();
    
    // Cleanup SDL
    if (pImpl->renderer) {
        SDL_DestroyRenderer(pImpl->renderer);
        pImpl->renderer = nullptr;
    }
    
    if (pImpl->window) {
        SDL_DestroyWindow(pImpl->window);
        pImpl->window = nullptr;
    }
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    pImpl->initialized = false;
}

void VisualizationEngine::update(SimulationStep step, Timestamp timestamp) {
    if (!pImpl->initialized) {
        return;
    }
    
    auto updateStart = std::chrono::high_resolution_clock::now();
    
    // Update current data
    updateNeuronData(brain);  // Note: Need access to brain - will be fixed
    updateSpikeData(brain);
    updateConnectionData(brain);
    
    pImpl->currentData.step = step;
    pImpl->currentData.timestamp = timestamp;
    pImpl->currentData.isDirty = true;
    
    // Push to frame buffer
    pImpl->frameBuffer.push(pImpl->currentData);
    
    // Process callbacks
    for (const auto& callback : pImpl->updateCallbacks) {
        callback(step, timestamp);
    }
    
    pImpl->lastUpdateTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - updateStart);
}

void VisualizationEngine::render() {
    if (!pImpl->initialized || !pImpl->running) {
        return;
    }
    
    auto renderStart = std::chrono::high_resolution_clock::now();
    
    // Clear screen
    SDL_SetRenderDrawColor(pImpl->renderer, 0, 0, 0, 255);
    SDL_RenderClear(pImpl->renderer);
    
    // Process render commands
    updateRenderQueue();
    processRenderCommands();
    
    // Render active visualizer
    if (!pImpl->activeVisualizerName.empty() && pImpl->visualizers.count(pImpl->activeVisualizerName)) {
        // Note: Need to call visualizeNetwork/visualizeActivity - will be fixed
        // pImpl->visualizers[pImpl->activeVisualizerName]->visualizeNetwork(brain);
        // pImpl->visualizers[pImpl->activeVisualizerName]->visualizeActivity(brain);
    }
    
    // Process render callbacks
    for (const auto& callback : pImpl->renderCallbacks) {
        callback();
    }
    
    // Debug overlay
    if (pImpl->debugMode) {
        renderDebugInfo();
    }
    
    // Present frame
    SDL_RenderPresent(pImpl->renderer);
    
    auto renderEnd = std::chrono::high_resolution_clock::now();
    pImpl->lastRenderTime = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderStart);
    
    // Update metrics
    pImpl->frameCount++;
    pImpl->metrics.frameTimes.push_back(pImpl->lastRenderTime);
    if (pImpl->metrics.frameTimes.size() > 100) {
        pImpl->metrics.frameTimes.erase(pImpl->metrics.frameTimes.begin());
    }
    
    // Calculate FPS
    if (pImpl->frameCount % 60 == 0) {
        auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(pImpl->lastRenderTime * 60);
        if (totalTime.count() > 0) {
            pImpl->currentFPS = 60.0 / totalTime.count();
        }
    }
}

void VisualizationEngine::registerVisualizer(const std::string& name, std::shared_ptr<VisualizationInterface> visualizer) {
    if (visualizer) {
        pImpl->visualizers[name] = visualizer;
        visualizer->initialize();
    }
}

void VisualizationEngine::unregisterVisualizer(const std::string& name) {
    if (pImpl->visualizers.count(name)) {
        pImpl->visualizers[name]->close();
        pImpl->visualizers.erase(name);
    }
}

void VisualizationEngine::setActiveVisualizer(const std::string& name) {
    if (pImpl->visualizers.count(name)) {
        pImpl->activeVisualizerName = name;
    }
}

void VisualizationEngine::setTargetFPS(double fps) {
    pImpl->targetFPS = fps;
    if (fps > 0) {
        int vsync = (fps >= 30) ? 1 : 0;
        SDL_SetRenderVSync(pImpl->renderer, vsync);
    }
}

void VisualizationEngine::setPerformanceMode(bool highPerformance) {
    pImpl->highPerformanceMode = highPerformance;
    if (pImpl->initialized) {
        if (highPerformance) {
            SDL_SetWindowFullscreen(pImpl->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(pImpl->window, 0);
        }
    }
}

double VisualizationEngine::getCurrentFPS() const {
    return pImpl->currentFPS;
}

bool VisualizationEngine::isInitialized() const {
    return pImpl->initialized;
}

bool VisualizationEngine::isRunning() const {
    return pImpl->running;
}

std::vector<std::string> VisualizationEngine::getAvailableVisualizers() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->visualizers) {
        names.push_back(pair.first);
    }
    return names;
}

// Helper methods (implementation details)
void VisualizationEngine::updateRenderQueue() {
    std::lock_guard<std::mutex> lock(pImpl->renderQueueMutex);
    // Process high priority commands
    while (!pImpl->renderQueue.empty() && 
           pImpl->renderQueue.top().priority == UpdatePriority::Critical) {
        auto cmd = pImpl->renderQueue.top();
        pImpl->renderQueue.pop();
        cmd.command();
    }
}

void VisualizationEngine::processRenderCommands() {
    std::lock_guard<std::mutex> lock(pImpl->renderQueueMutex);
    while (!pImpl->renderQueue.empty()) {
        auto cmd = pImpl->renderQueue.top();
        pImpl->renderQueue.pop();
        cmd.command();
    }
}

void VisualizationEngine::renderDebugInfo() {
    // Simple debug overlay rendering
    if (!pImpl->pImpl->renderer) return;
    
    // Create debug text
    std::string debugText = "FPS: " + std::to_string(pImpl->currentFPS) +
                           " | Draw Calls: " + std::to_string(pImpl->metrics.drawCalls) +
                           " | Memory: " + std::to_string(pImpl->metrics.memoryUsage) + " KB";
    
    // Note: SDL_ttf integration needed for proper text rendering
    // For now, just placeholder for debug info
}

void VisualizationEngine::loadDefaultTheme() {
    pImpl->currentTheme = "default";
    
    // Default styles
    pImpl->styles["neuron"] = {
        {"color", "#4A90E2"},
        {"radius", "5"},
        {"outline", "#FFFFFF"}
    };
    
    pImpl->styles["connection"] = {
        {"width", "2"},
        {"color", "#888888"},
        {"opacity", "0.5"}
    };
    
    pImpl->styles["background"] = {
        {"color", "#000000"}
    };
}

VisualizationEngine::PerformanceMetrics VisualizationEngine::getMetrics() const {
    PerformanceMetrics metrics;
    
    if (!pImpl->frameTimes.empty()) {
        auto totalFrameTime = std::accumulate(pImpl->frameTimes.begin(), pImpl->frameTimes.end(), 
                                            std::chrono::microseconds(0));
        auto avgFrameTime = totalFrameTime / pImpl->frameTimes.size();
        metrics.fps = 1000000.0 / avgFrameTime.count();
    }
    
    metrics.updateTime = pImpl->lastUpdateTime;
    metrics.renderTime = pImpl->lastRenderTime;
    metrics.vertexCount = pImpl->metrics.vertexCount;
    metrics.drawCallCount = pImpl->metrics.drawCallCount;
    metrics.memoryUsage = pImpl->metrics.memoryUsage;
    metrics.activeVisualizers = pImpl->visualizers.size();
    
    return metrics;
}

} // namespace nlm
