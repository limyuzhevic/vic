#include "VisualizationSystem.hpp"
#include "VisualizationEngine.hpp"
#include "../brain/Brain.hpp"
#include "NetworkVisualizer.hpp"
#include "ActivityVisualizer.hpp"
#include "RasterVisualizer.hpp"
#include "WeightVisualizer.hpp"
#include "DevelopmentVisualizer.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

struct VisualizationSystem::Impl {
    bool active;
    double updateRate;
    double updateInterval;  // seconds
    double lastUpdateTime;
    
    // Performance tracking
    std::vector<double> frameTimes;
    size_t maxFrameHistory;
    
    // Visualizers
    std::unique_ptr<NetworkVisualizer> networkVisualizer;
    std::unique_ptr<ActivityVisualizer> activityVisualizer;
    std::unique_ptr<RasterVisualizer> rasterVisualizer;
    std::unique_ptr<WeightVisualizer> weightVisualizer;
    std::unique_ptr<DevelopmentVisualizer> developmentVisualizer;
    
    // Configuration
    VisualizationMode currentMode;
    bool showNeurons;
    bool showConnections;
    int colorScheme;
    bool neuronLabels;
    float bgColor[4];
    float zoomFactor;
    float rotation[3];
    int projectionMode;
    
    // Performance metrics
    PerformanceMetrics metrics;
    
    Impl()
        : active(false)
        , updateRate(30.0)
        , updateInterval(1.0 / 30.0)
        , lastUpdateTime(0.0)
        , maxFrameHistory(100)
        , currentMode(CombinedAll)
        , showNeurons(true)
        , showConnections(true)
        , colorScheme(0)
        , neuronLabels(true)
        , bgColor{0.0f, 0.0f, 0.0f, 1.0f}
        , zoomFactor(1.0f)
        , rotation{0.0f, 0.0f, 0.0f}
        , projectionMode(1)  // perspective
    {
        frameTimes.reserve(maxFrameHistory);
    }
};

VisualizationSystem::VisualizationSystem() : pImpl(new Impl) {}
VisualizationSystem::~VisualizationSystem() = default;

bool VisualizationSystem::initialize() {
    // Initialize all visualizers
    // TODO: Implement specific backend initialization
    pImpl->active = true;
    resetPerformanceMetrics();
    return true;
}

void VisualizationSystem::update(const Brain& brain, double timestep) {
    if (!pImpl->active) return;
    
    double currentTime = 0.0;  // TODO: Get from clock
    double elapsed = currentTime - pImpl->lastUpdateTime;
    
    // Update performance metrics
    pImpl->frameTimes.push_back(elapsed * 1000.0);  // Convert to ms
    if (pImpl->frameTimes.size() > pImpl->maxFrameHistory) {
        pImpl->frameTimes.erase(pImpl->frameTimes.begin());
    }
    
    pImpl->metrics.currentFrameTime = elapsed * 1000.0;
    if (!pImpl->frameTimes.empty()) {
        double sum = 0.0;
        for (double t : pImpl->frameTimes) sum += t;
        pImpl->metrics.averageFrameTime = sum / pImpl->frameTimes.size();
        pImpl->metrics.frameRate = 1000.0 / pImpl->metrics.averageFrameTime;
    }
    pImpl->metrics.activeVisualizers = 
        (pImpl->networkVisualizer ? 1 : 0) +
        (pImpl->activityVisualizer ? 1 : 0) +
        (pImpl->rasterVisualizer ? 1 : 0) +
        (pImpl->weightVisualizer ? 1 : 0) +
        (pImpl->developmentVisualizer ? 1 : 0);
    
    pImpl->lastUpdateTime = currentTime;
    
    // Update active visualizers based on mode
    switch (pImpl->currentMode) {
        case NetworkOnly:
            if (pImpl->networkVisualizer) {
                pImpl->networkVisualizer->update(brain, timestep);
            }
            break;
        case ActivityOnly:
            if (pImpl->activityVisualizer) {
                pImpl->activityVisualizer->update(brain, timestep);
            }
            break;
        case RasterOnly:
            if (pImpl->rasterVisualizer) {
                pImpl->rasterVisualizer->update(brain, timestep);
            }
            break;
        case WeightOnly:
            if (pImpl->weightVisualizer) {
                pImpl->weightVisualizer->update(brain, timestep);
            }
            break;
        case DevelopmentOnly:
            if (pImpl->developmentVisualizer) {
                pImpl->developmentVisualizer->update(brain, timestep);
            }
            break;
        case CombinedNetworkActivity:
            if (pImpl->networkVisualizer) {
                pImpl->networkVisualizer->update(brain, timestep);
            }
            if (pImpl->activityVisualizer) {
                pImpl->activityVisualizer->update(brain, timestep);
            }
            break;
        case CombinedAll:
            if (pImpl->networkVisualizer) {
                pImpl->networkVisualizer->update(brain, timestep);
            }
            if (pImpl->activityVisualizer) {
                pImpl->activityVisualizer->update(brain, timestep);
            }
            if (pImpl->rasterVisualizer) {
                pImpl->rasterVisualizer->update(brain, timestep);
            }
            if (pImpl->weightVisualizer) {
                pImpl->weightVisualizer->update(brain, timestep);
            }
            if (pImpl->developmentVisualizer) {
                pImpl->developmentVisualizer->update(brain, timestep);
            }
            break;
    }
}

void VisualizationSystem::render() {
    if (!pImpl->active) return;
    
    // Configure OpenGL/viewport based on settings
    // TODO: Implement actual rendering pipeline
    
    // Render each visualizer based on mode
    switch (pImpl->currentMode) {
        case NetworkOnly:
            if (pImpl->networkVisualizer) {
                pImpl->networkVisualizer->render();
            }
            break;
        case ActivityOnly:
            if (pImpl->activityVisualizer) {
                pImpl->activityVisualizer->render();
            }
            break;
        case RasterOnly:
            if (pImpl->rasterVisualizer) {
                pImpl->rasterVisualizer->render();
            }
            break;
        case WeightOnly:
            if (pImpl->weightVisualizer) {
                pImpl->weightVisualizer->render();
            }
            break;
        case DevelopmentOnly:
            if (pImpl->developmentVisualizer) {
                pImpl->developmentVisualizer->render();
            }
            break;
        case CombinedNetworkActivity:
            if (pImpl->networkVisualizer) {
                pImpl->networkVisualizer->render();
            }
            if (pImpl->activityVisualizer) {
                pImpl->activityVisualizer->render();
            }
            break;
        case CombinedAll:
            if (pImpl->networkVisualizer) {
                pImpl->networkVisualizer->render();
            }
            if (pImpl->activityVisualizer) {
                pImpl->activityVisualizer->render();
            }
            if (pImpl->rasterVisualizer) {
                pImpl->rasterVisualizer->render();
            }
            if (pImpl->weightVisualizer) {
                pImpl->weightVisualizer->render();
            }
            if (pImpl->developmentVisualizer) {
                pImpl->developmentVisualizer->render();
            }
            break;
    }
}

void VisualizationSystem::close() {
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->close();
    }
    if (pImpl->activityVisualizer) {
        pImpl->activityVisualizer->close();
    }
    if (pImpl->rasterVisualizer) {
        pImpl->rasterVisualizer->close();
    }
    if (pImpl->weightVisualizer) {
        pImpl->weightVisualizer->close();
    }
    if (pImpl->developmentVisualizer) {
        pImpl->developmentVisualizer->close();
    }
    
    pImpl->active = false;
}

bool VisualizationSystem::isActive() const {
    return pImpl->active;
}

void VisualizationSystem::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    pImpl->updateInterval = 1.0 / hz;
}

double VisualizationSystem::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationSystem::registerNetworkVisualizer(std::unique_ptr<NetworkVisualizer> visualizer) {
    pImpl->networkVisualizer = std::move(visualizer);
}

void VisualizationSystem::registerActivityVisualizer(std::unique_ptr<ActivityVisualizer> visualizer) {
    pImpl->activityVisualizer = std::move(visualizer);
}

void VisualizationSystem::registerRasterVisualizer(std::unique_ptr<RasterVisualizer> visualizer) {
    pImpl->rasterVisualizer = std::move(visualizer);
}

void VisualizationSystem::registerWeightVisualizer(std::unique_ptr<WeightVisualizer> visualizer) {
    pImpl->weightVisualizer = std::move(visualizer);
}

void VisualizationSystem::registerDevelopmentVisualizer(std::unique_ptr<DevelopmentVisualizer> visualizer) {
    pImpl->developmentVisualizer = std::move(visualizer);
}

void VisualizationSystem::setShowNeurons(bool show) {
    pImpl->showNeurons = show;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setShowNeurons(show);
    }
}

void VisualizationSystem::setShowConnections(bool show) {
    pImpl->showConnections = show;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setShowConnections(show);
    }
}

void VisualizationSystem::setColorScheme(int scheme) {
    pImpl->colorScheme = scheme;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setColorScheme(scheme);
    }
}

void VisualizationSystem::setNeuronLabels(bool show) {
    pImpl->neuronLabels = show;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setNeuronLabels(show);
    }
}

void VisualizationSystem::setBackgroundColor(float r, float g, float b, float a) {
    pImpl->bgColor[0] = r;
    pImpl->bgColor[1] = g;
    pImpl->bgColor[2] = b;
    pImpl->bgColor[3] = a;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setBackgroundColor(r, g, b, a);
    }
}

void VisualizationSystem::setZoom(float factor) {
    pImpl->zoomFactor = factor;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setZoom(factor);
    }
}

void VisualizationSystem::setRotation(float x, float y, float z) {
    pImpl->rotation[0] = x;
    pImpl->rotation[1] = y;
    pImpl->rotation[2] = z;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setRotation(x, y, z);
    }
}

void VisualizationSystem::setProjectionMode(int mode) {
    pImpl->projectionMode = mode;
    if (pImpl->networkVisualizer) {
        pImpl->networkVisualizer->setProjectionMode(mode);
    }
}

bool VisualizationSystem::exportData(const std::string& filepath, ExportFormat format) {
    // Delegate to appropriate visualizer based on data type
    // TODO: Implement based on format
    return false;
}

NetworkVisualizer* VisualizationSystem::getNetworkVisualizer() {
    return pImpl->networkVisualizer.get();
}

ActivityVisualizer* VisualizationSystem::getActivityVisualizer() {
    return pImpl->activityVisualizer.get();
}

RasterVisualizer* VisualizationSystem::getRasterVisualizer() {
    return pImpl->rasterVisualizer.get();
}

WeightVisualizer* VisualizationSystem::getWeightVisualizer() {
    return pImpl->weightVisualizer.get();
}

DevelopmentVisualizer* VisualizationSystem::getDevelopmentVisualizer() {
    return pImpl->developmentVisualizer.get();
}

VisualizationSystem::PerformanceMetrics VisualizationSystem::getPerformanceMetrics() const {
    return pImpl->metrics;
}

void VisualizationSystem::resetPerformanceMetrics() {
    pImpl->metrics = PerformanceMetrics();
    pImpl->frameTimes.clear();
}

void VisualizationSystem::setVisualizationMode(VisualizationMode mode) {
    pImpl->currentMode = mode;
}

VisualizationSystem::VisualizationMode VisualizationSystem::getVisualizationMode() const {
    return pImpl->currentMode;
}

} // namespace nlm
