#include "VisualizationInterface.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    // Core systems
    std::shared_ptr<VisualizationSystem> visualizationSystem;
    std::shared_ptr<RealTimeMonitor> monitor;
    std::shared_ptr<Brain> brain;
    
    // Configuration
    bool active;
    double updateRate;
    
    // Threading
    std::mutex mutex;
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {
    pImpl->active = false;
    pImpl->updateRate = 30.0;
}

VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize(std::shared_ptr<Config> config, 
                                        VisualizationSystem::BackendType backend) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->visualizationSystem = std::make_shared<VisualizationSystem>();
    pImpl->active = pImpl->visualizationSystem->initialize(config, backend);
    
    return pImpl->active;
}

bool VisualizationInterface::initialize(std::shared_ptr<VisualizationSystem> visualizer) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->visualizationSystem = visualizer;
    pImpl->active = visualizer ? true : false;
    
    return pImpl->active;
}

bool VisualizationInterface::initialize(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->monitor = monitor;
    pImpl->active = monitor ? true : false;
    
    return pImpl->active;
}

bool VisualizationInterface::initialize(std::shared_ptr<Brain> brain,
                                        std::shared_ptr<RealTimeMonitor> monitor,
                                        VisualizationSystem::BackendType backend) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->brain = brain;
    pImpl->monitor = monitor;
    
    auto config = std::make_shared<Config>();
    pImpl->visualizationSystem = std::make_shared<VisualizationSystem>();
    
    pImpl->active = pImpl->visualizationSystem->initialize(config, backend);
    
    if (pImpl->active && pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setBrainData(brain);
        pImpl->visualizationSystem->setMonitor(monitor);
    }
    
    return pImpl->active;
}

void VisualizationInterface::update(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem && brain) {
        pImpl->visualizationSystem->setBrainData(brain);
        pImpl->visualizationSystem->updateVisualization(
            VisualizationSystem::VisualizationType::NETWORK);
        pImpl->visualizationSystem->updateVisualization(
            VisualizationSystem::VisualizationType::ACTIVITY);
    }
}

void VisualizationInterface::update(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem && monitor) {
        pImpl->visualizationSystem->setMonitor(monitor);
        pImpl->visualizationSystem->updateVisualization(
            VisualizationSystem::VisualizationType::PERFORMANCE);
    }
}

void VisualizationInterface::render() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem && pImpl->visualizationSystem->isRunning()) {
        pImpl->visualizationSystem->updateVisualization(
            VisualizationSystem::VisualizationType::NETWORK);
        pImpl->visualizationSystem->updateVisualization(
            VisualizationSystem::VisualizationType::ACTIVITY);
    }
}

void VisualizationInterface::render(VisualizationSystem::VisualizationType type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem && pImpl->visualizationSystem->isRunning()) {
        pImpl->visualizationSystem->updateVisualization(type);
    }
}

void VisualizationInterface::close() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->stopVisualization();
        pImpl->visualizationSystem.reset();
    }
    
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->updateRate = hz;
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setUpdateFrequency(hz);
    }
}

double VisualizationInterface::getUpdateRate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->updateRate;
}

void VisualizationInterface::startVisualization(double fps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->startVisualization(fps);
    }
}

void VisualizationInterface::stopVisualization() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->stopVisualization();
    }
}

bool VisualizationInterface::isRunning() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->visualizationSystem && pImpl->visualizationSystem->isRunning();
}

void VisualizationInterface::setBrainData(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->brain = brain;
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setBrainData(brain);
    }
}

void VisualizationInterface::setMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->monitor = monitor;
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setMonitor(monitor);
    }
}

void VisualizationInterface::setBackend(VisualizationSystem::BackendType backend) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setBackend(backend);
    }
}

VisualizationSystem::BackendType VisualizationInterface::getBackendType() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getBackendType();
    }
    return VisualizationSystem::BackendType::SOFTWARE_RENDERER;
}

void VisualizationInterface::setCameraPosition(float x, float y, float z) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setCameraPosition(x, y, z);
    }
}

void VisualizationInterface::setCameraTarget(float x, float y, float z) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setCameraTarget(x, y, z);
    }
}

void VisualizationInterface::setZoom(float zoom) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setZoom(zoom);
    }
}

void VisualizationInterface::rotateCamera(float yaw, float pitch) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->rotateCamera(yaw, pitch);
    }
}

bool VisualizationInterface::exportVisualization(const std::string& filepath,
                                                 const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->exportVisualization(filepath, format);
    }
    return false;
}

bool VisualizationInterface::exportAnimation(const std::string& filepath,
                                             double duration, double fps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->exportAnimation(filepath, duration, fps);
    }
    return false;
}

size_t VisualizationInterface::getTotalNeuronCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getTotalNeuronCount();
    }
    return 0;
}

float VisualizationInterface::getRenderingFPS() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getRenderingFPS();
    }
    return 0.0f;
}

size_t VisualizationInterface::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getMemoryUsage();
    }
    return 0;
}

std::shared_ptr<VisualizationSystem> VisualizationInterface::getVisualizationSystem() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->visualizationSystem;
}

std::shared_ptr<RealTimeMonitor> VisualizationInterface::getMonitor() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->monitor;
}

} // namespace nlm
