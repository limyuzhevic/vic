#include "3DNeuralVisualizer.hpp"
#include <iostream>

namespace nlm {

struct Neural3DVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    float cameraX, cameraY, cameraZ;
    float lookAtX, lookAtY, lookAtZ;
    float fov;
    float lightX, lightY, lightZ;
    bool showNeuronSpheres;
    bool showSynapseTubes;
    bool showBackgroundGrid;
    bool enableLighting;
    std::string renderingMode;
    float rotationX, rotationY;
    
    Impl() : visible(false), width(800), height(600), cameraX(0.0f), cameraY(0.0f), cameraZ(20.0f),
             lookAtX(0.0f), lookAtY(0.0f), lookAtZ(0.0f), fov(45.0f), lightX(10.0f), lightY(10.0f), lightZ(10.0f),
             showNeuronSpheres(true), showSynapseTubes(true), showBackgroundGrid(true), enableLighting(true),
             renderingMode("solid"), rotationX(0.0f), rotationY(0.0f) {}
};

Neural3DVisualizer::Neural3DVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool Neural3DVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void Neural3DVisualizer::update() {
    // Update 3D scene transformations
}

void Neural3DVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering 3D Neural Network Visualization:" << std::endl;
    std::cout << "  Camera position: (" << pImpl->cameraX << ", " << pImpl->cameraY << ", " << pImpl->cameraZ << ")" << std::endl;
    std::cout << "  Look at: (" << pImpl->lookAtX << ", " << pImpl->lookAtY << ", " << pImpl->lookAtZ << ")" << std::endl;
    std::cout << "  FOV: " << pImpl->fov << " degrees" << std::endl;
    std::cout << "  Light position: (" << pImpl->lightX << ", " << pImpl->lightY << ", " << pImpl->lightZ << ")" << std::endl;
    std::cout << "  Neuron spheres shown: " << (pImpl->showNeuronSpheres ? "yes" : "no") << std::endl;
    std::cout << "  Synapse tubes shown: " << (pImpl->showSynapseTubes ? "yes" : "no") << std::endl;
    std::cout << "  Background grid shown: " << (pImpl->showBackgroundGrid ? "yes" : "no") << std::endl;
    std::cout << "  Lighting enabled: " << (pImpl->enableLighting ? "yes" : "no") << std::endl;
    std::cout << "  Rendering mode: " << pImpl->renderingMode << std::endl;
}

void Neural3DVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool Neural3DVisualizer::isVisible() const {
    return pImpl->visible;
}

void Neural3DVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void Neural3DVisualizer::setCameraPosition(float x, float y, float z) {
    pImpl->cameraX = x;
    pImpl->cameraY = y;
    pImpl->cameraZ = z;
}

void Neural3DVisualizer::setCameraLookAt(float x, float y, float z) {
    pImpl->lookAtX = x;
    pImpl->lookAtY = y;
    pImpl->lookAtZ = z;
}

void Neural3DVisualizer::setCameraFOV(float fov) {
    pImpl->fov = fov;
}

void Neural3DVisualizer::setLightPosition(float x, float y, float z) {
    pImpl->lightX = x;
    pImpl->lightY = y;
    pImpl->lightZ = z;
}

void Neural3DVisualizer::setShowNeuronSpheres(bool show) {
    pImpl->showNeuronSpheres = show;
}

void Neural3DVisualizer::setShowSynapseTubes(bool show) {
    pImpl->showSynapseTubes = show;
}

void Neural3DVisualizer::setShowBackgroundGrid(bool show) {
    pImpl->showBackgroundGrid = show;
}

void Neural3DVisualizer::setEnableLighting(bool enable) {
    pImpl->enableLighting = enable;
}

void Neural3DVisualizer::setRenderingMode(const std::string& mode) {
    pImpl->renderingMode = mode;
}

void Neural3DVisualizer::rotate(float angleX, float angleY) {
    pImpl->rotationX += angleX;
    pImpl->rotationY += angleY;
}

void Neural3DVisualizer::zoom(float factor) {
    pImpl->cameraZ *= (1.0f + factor * 0.1f);
}

void Neural3DVisualizer::pan(float deltaX, float deltaY) {
    pImpl->cameraX += deltaX * 0.1f;
    pImpl->cameraY += deltaY * 0.1f;
}

} // namespace nlm