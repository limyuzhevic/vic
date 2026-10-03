#include "3DView.hpp"

namespace nlm {

struct View3D::Impl {
    std::string name;
    bool enabled;
    std::string projectionType;
    std::array<float, 3> cameraPosition;
    std::array<float, 3> cameraLookAt;
    float fieldOfView;
    float nearClippingPlane;
    float farClippingPlane;
    bool lightingEnabled;
    bool shadowsEnabled;
    bool wireframeEnabled;
    bool backfaceCullingEnabled;
    float pointSize;
    float lineWidth;
    float triangleSize;
    bool animationsEnabled;
    float animationSpeed;
    std::pair<Timestamp, Timestamp> timeWindow;
    
    Impl() : name("3DView"), enabled(true), projectionType("perspective"),
             cameraPosition({0.0f, 0.0f, 10.0f}), cameraLookAt({0.0f, 0.0f, 0.0f}),
             fieldOfView(45.0f), nearClippingPlane(0.1f), farClippingPlane(1000.0f),
             lightingEnabled(true), shadowsEnabled(true), wireframeEnabled(false),
             backfaceCullingEnabled(true), pointSize(1.0f), lineWidth(1.0f),
             triangleSize(1.0f), animationsEnabled(true), animationSpeed(1.0f),
             timeWindow({0.0, 100.0}) {}
};

View3D::View3D() : pImpl(std::make_unique<Impl>()) {}
View3D::~View3D() = default;

const std::string& View3D::getName() const {
    return pImpl->name;
}

void View3D::setName(const std::string& name) {
    pImpl->name = name;
}

void View3D::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool View3D::isEnabled() const {
    return pImpl->enabled;
}

void View3D::setProjectionType(const std::string& type) {
    pImpl->projectionType = type;
}

const std::string& View3D::getProjectionType() const {
    return pImpl->projectionType;
}

void View3D::setCameraPosition(float x, float y, float z) {
    pImpl->cameraPosition = {x, y, z};
}

std::array<float, 3> View3D::getCameraPosition() const {
    return pImpl->cameraPosition;
}

void View3D::setCameraLookAt(float x, float y, float z) {
    pImpl->cameraLookAt = {x, y, z};
}

std::array<float, 3> View3D::getCameraLookAt() const {
    return pImpl->cameraLookAt;
}

void View3D::setFieldOfView(float fov) {
    pImpl->fieldOfView = fov;
}

float View3D::getFieldOfView() const {
    return pImpl->fieldOfView;
}

void View3D::setNearClippingPlane(float near) {
    pImpl->nearClippingPlane = near;
}

float View3D::getNearClippingPlane() const {
    return pImpl->nearClippingPlane;
}

void View3D::setFarClippingPlane(float far) {
    pImpl->farClippingPlane = far;
}

float View3D::getFarClippingPlane() const {
    return pImpl->farClippingPlane;
}

void View3D::enableLighting(bool enabled) {
    pImpl->lightingEnabled = enabled;
}

bool View3D::isLightingEnabled() const {
    return pImpl->lightingEnabled;
}

void View3D::enableShadows(bool enabled) {
    pImpl->shadowsEnabled = enabled;
}

bool View3D::areShadowsEnabled() const {
    return pImpl->shadowsEnabled;
}

void View3D::enableWireframe(bool enabled) {
    pImpl->wireframeEnabled = enabled;
}

bool View3D::isWireframeEnabled() const {
    return pImpl->wireframeEnabled;
}

void View3D::enableBackfaceCulling(bool enabled) {
    pImpl->backfaceCullingEnabled = enabled;
}

bool View3D::isBackfaceCullingEnabled() const {
    return pImpl->backfaceCullingEnabled;
}

void View3D::setPointSize(float size) {
    pImpl->pointSize = size;
}

float View3D::getPointSize() const {
    return pImpl->pointSize;
}

void View3D::setLineWidth(float width) {
    pImpl->lineWidth = width;
}

float View3D::getLineWidth() const {
    return pImpl->lineWidth;
}

void View3D::setTriangleSize(float size) {
    pImpl->triangleSize = size;
}

float View3D::getTriangleSize() const {
    return pImpl->triangleSize;
}

void View3D::enableAnimations(bool enabled) {
    pImpl->animationsEnabled = enabled;
}

bool View3D::areAnimationsEnabled() const {
    return pImpl->animationsEnabled;
}

void View3D::setAnimationSpeed(float speed) {
    pImpl->animationSpeed = speed;
}

float View3D::getAnimationSpeed() const {
    return pImpl->animationSpeed;
}

void View3D::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> View3D::getTimeWindow() const {
    return pImpl->timeWindow;
}

void View3D::reset() {
    setName("3DView");
    enable(true);
    setProjectionType("perspective");
    setCameraPosition(0.0f, 0.0f, 10.0f);
    setCameraLookAt(0.0f, 0.0f, 0.0f);
    setFieldOfView(45.0f);
    setNearClippingPlane(0.1f);
    setFarClippingPlane(1000.0f);
    enableLighting(true);
    enableShadows(true);
    enableWireframe(false);
    enableBackfaceCulling(true);
    setPointSize(1.0f);
    setLineWidth(1.0f);
    setTriangleSize(1.0f);
    enableAnimations(true);
    setAnimationSpeed(1.0f);
    setTimeWindow(0.0, 100.0);
}

} // namespace nlm