#include "VisualizationInterface.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string name;
    std::string version;
    bool threeDSupported;
    
    Impl() : active(false), updateRate(30.0), name("NLM Visualization"), 
             version("1.0.0"), threeDSupported(false) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Base implementation - subclasses should override
    pImpl->active = false;
    return false;
}

void VisualizationInterface::update() {
    // Base implementation - subclasses should override
}

void VisualizationInterface::render() {
    // Base implementation - subclasses should override
}

void VisualizationInterface::close() {
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

bool VisualizationInterface::supports3D() const {
    return pImpl->threeDSupported;
}

std::string VisualizationInterface::getName() const {
    return pImpl->name;
}

std::string VisualizationInterface::getVersion() const {
    return pImpl->version;
}

void VisualizationInterface::onResize(int width, int height) {
    // Base implementation - subclasses should override
}

void VisualizationInterface::setParameter(const std::string& name, const std::string& value) {
    // Base implementation - subclasses should override
}

std::string VisualizationInterface::getParameter(const std::string& name) const {
    return "";
}

void VisualizationInterface::setComponentEnabled(const std::string& component, bool enabled) {
    // Base implementation - subclasses should override
}

bool VisualizationInterface::isComponentEnabled(const std::string& component) const {
    return false;
}

void VisualizationInterface::addEventCallback(const std::string& eventType, std::function<void(const std::string&)> callback) {
    // Base implementation - subclasses should override
}

bool VisualizationInterface::exportState(const std::string& filename, const std::string& format) {
    // Base implementation - subclasses should override
    return false;
}

bool VisualizationInterface::importState(const std::string& filename, const std::string& format) {
    // Base implementation - subclasses should override
    return false;
}

} // namespace nlm