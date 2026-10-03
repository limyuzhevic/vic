#include "VisualizationScene.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

struct VisualizationScene::Impl {
    std::string name;
    bool active;
    bool cullingEnabled;
    bool lightingEnabled;
    bool shadowsEnabled;
    bool depthTestingEnabled;
    bool faceCullingEnabled;
    bool textureRenderingEnabled;
    std::array<float, 3> ambientLightColor;
    std::array<float, 4> fogParameters;
    std::vector<std::unique_ptr<VisualizationActor>> actors;
    std::vector<std::unique_ptr<VisualizationLight>> lights;
    std::vector<std::unique_ptr<VisualizationCamera>> cameras;
    VisualizationCamera* activeCamera;
    
    Impl() : active(true), cullingEnabled(true), lightingEnabled(true), 
             shadowsEnabled(true), depthTestingEnabled(true), 
             faceCullingEnabled(true), textureRenderingEnabled(true),
             ambientLightColor({0.2f, 0.2f, 0.2f}), fogParameters({0.0f, 0.0f, 0.0f, 0.0f}),
             activeCamera(nullptr) {}
};

VisualizationScene::VisualizationScene() : pImpl(std::make_unique<Impl>()) {
    pImpl->name = "DefaultScene";
}

VisualizationScene::~VisualizationScene() = default;

void VisualizationScene::addActor(std::unique_ptr<VisualizationActor> actor) {
    pImpl->actors.push_back(std::move(actor));
}

void VisualizationScene::removeActor(VisualizationActor* actor) {
    auto it = std::find_if(pImpl->actors.begin(), pImpl->actors.end(),
                           [actor](const std::unique_ptr<VisualizationActor>& ptr) {
                               return ptr.get() == actor;
                           });
    if (it != pImpl->actors.end()) {
        pImpl->actors.erase(it);
    }
}

const std::vector<VisualizationActor*>& VisualizationScene::getActors() const {
    static std::vector<VisualizationActor*> emptyActors;
    if (pImpl->actors.empty()) return emptyActors;
    
    // Convert unique_ptrs to raw pointers for const access
    // This is a temporary workaround - in production, you might want a different design
    static std::vector<VisualizationActor*> actorPointers;
    actorPointers.clear();
    for (const auto& actor : pImpl->actors) {
        actorPointers.push_back(actor.get());
    }
    return actorPointers;
}

void VisualizationScene::addLight(std::unique_ptr<VisualizationLight> light) {
    pImpl->lights.push_back(std::move(light));
}

void VisualizationScene::removeLight(VisualizationLight* light) {
    auto it = std::find_if(pImpl->lights.begin(), pImpl->lights.end(),
                           [light](const std::unique_ptr<VisualizationLight>& ptr) {
                               return ptr.get() == light;
                           });
    if (it != pImpl->lights.end()) {
        pImpl->lights.erase(it);
    }
}

const std::vector<VisualizationLight*>& VisualizationScene::getLights() const {
    static std::vector<VisualizationLight*> emptyLights;
    if (pImpl->lights.empty()) return emptyLights;
    
    static std::vector<VisualizationLight*> lightPointers;
    lightPointers.clear();
    for (const auto& light : pImpl->lights) {
        lightPointers.push_back(light.get());
    }
    return lightPointers;
}

void VisualizationScene::addCamera(std::unique_ptr<VisualizationCamera> camera) {
    pImpl->cameras.push_back(std::move(camera));
}

void VisualizationScene::removeCamera(VisualizationCamera* camera) {
    auto it = std::find_if(pImpl->cameras.begin(), pImpl->cameras.end(),
                           [camera](const std::unique_ptr<VisualizationCamera>& ptr) {
                               return ptr.get() == camera;
                           });
    if (it != pImpl->cameras.end()) {
        pImpl->cameras.erase(it);
    }
}

const std::vector<VisualizationCamera*>& VisualizationScene::getCameras() const {
    static std::vector<VisualizationCamera*> emptyCameras;
    if (pImpl->cameras.empty()) return emptyCameras;
    
    static std::vector<VisualizationCamera*> cameraPointers;
    cameraPointers.clear();
    for (const auto& camera : pImpl->cameras) {
        cameraPointers.push_back(camera.get());
    }
    return cameraPointers;
}

void VisualizationScene::setActiveCamera(VisualizationCamera* camera) {
    pImpl->activeCamera = camera;
}

VisualizationCamera* VisualizationScene::getActiveCamera() const {
    return pImpl->activeCamera;
}

void VisualizationScene::update(double deltaTime) {
    for (auto& actor : pImpl->actors) {
        actor->update(deltaTime);
    }
}

void VisualizationScene::render() {
    // Implementation would render scene with lighting, camera, etc.
}

void VisualizationScene::clear() {
    pImpl->actors.clear();
    pImpl->lights.clear();
    pImpl->cameras.clear();
    pImpl->activeCamera = nullptr;
}

const std::string& VisualizationScene::getName() const {
    return pImpl->name;
}

void VisualizationScene::setName(const std::string& name) {
    pImpl->name = name;
}

bool VisualizationScene::isActive() const {
    return pImpl->active;
}

void VisualizationScene::setActive(bool active) {
    pImpl->active = active;
}

void VisualizationScene::enableCulling(bool enabled) {
    pImpl->cullingEnabled = enabled;
}

bool VisualizationScene::isCullingEnabled() const {
    return pImpl->cullingEnabled;
}

void VisualizationScene::enableLighting(bool enabled) {
    pImpl->lightingEnabled = enabled;
}

bool VisualizationScene::isLightingEnabled() const {
    return pImpl->lightingEnabled;
}

void VisualizationScene::enableShadows(bool enabled) {
    pImpl->shadowsEnabled = enabled;
}

bool VisualizationScene::areShadowsEnabled() const {
    return pImpl->shadowsEnabled;
}

void VisualizationScene::setAmbientLightColor(float r, float g, float b) {
    pImpl->ambientLightColor = {r, g, b};
}

std::array<float, 3> VisualizationScene::getAmbientLightColor() const {
    return pImpl->ambientLightColor;
}

void VisualizationScene::setFog(bool enabled, float density, float r, float g, float b) {
    pImpl->fogParameters[0] = enabled ? density : 0.0f;
    pImpl->fogParameters[1] = r;
    pImpl->fogParameters[2] = g;
    pImpl->fogParameters[3] = b;
}

std::array<float, 4> VisualizationScene::getFogParameters() const {
    return pImpl->fogParameters;
}

void VisualizationScene::enableDepthTesting(bool enabled) {
    pImpl->depthTestingEnabled = enabled;
}

bool VisualizationScene::isDepthTestingEnabled() const {
    return pImpl->depthTestingEnabled;
}

void VisualizationScene::enableFaceCulling(bool enabled) {
    pImpl->faceCullingEnabled = enabled;
}

bool VisualizationScene::isFaceCullingEnabled() const {
    return pImpl->faceCullingEnabled;
}

void VisualizationScene::enableTextureRendering(bool enabled) {
    pImpl->textureRenderingEnabled = enabled;
}

bool VisualizationScene::isTextureRenderingEnabled() const {
    return pImpl->textureRenderingEnabled;
}

} // namespace nlm