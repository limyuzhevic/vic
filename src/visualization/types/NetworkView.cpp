#include "NetworkView.hpp"

namespace nlm {

struct NetworkView::Impl {
    std::string name;
    bool enabled;
    float opacity;
    std::string colorScheme;
    float edgeWidth;
    float nodeSize;
    bool nodeLabelsEnabled;
    bool edgeLabelsEnabled;
    float labelFontSize;
    bool tooltipsEnabled;
    float animationSpeed;
    bool animationsEnabled;
    std::array<float, 2> position;
    float scale;
    float rotation;
    bool enabled3D;
    float depth;
    float cameraDistance;
    float fieldOfView;
    
    Impl() : name("NetworkView"), enabled(true), opacity(1.0f), colorScheme("default"),
             edgeWidth(1.0f), nodeSize(5.0f), nodeLabelsEnabled(true), 
             edgeLabelsEnabled(true), labelFontSize(12.0f), tooltipsEnabled(true),
             animationSpeed(1.0f), animationsEnabled(true), position({0.0f, 0.0f}),
             scale(1.0f), rotation(0.0f), enabled3D(false), depth(1.0f), cameraDistance(10.0f),
             fieldOfView(45.0f) {}
};

NetworkView::NetworkView() : pImpl(std::make_unique<Impl>()) {}
NetworkView::~NetworkView() = default;

const std::string& NetworkView::getName() const {
    return pImpl->name;
}

void NetworkView::setName(const std::string& name) {
    pImpl->name = name;
}

void NetworkView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool NetworkView::isEnabled() const {
    return pImpl->enabled;
}

void NetworkView::setOpacity(float opacity) {
    pImpl->opacity = opacity;
}

float NetworkView::getOpacity() const {
    return pImpl->opacity;
}

void NetworkView::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

const std::string& NetworkView::getColorScheme() const {
    return pImpl->colorScheme;
}

void NetworkView::setEdgeWidth(float width) {
    pImpl->edgeWidth = width;
}

float NetworkView::getEdgeWidth() const {
    return pImpl->edgeWidth;
}

void NetworkView::setNodeSize(float size) {
    pImpl->nodeSize = size;
}

float NetworkView::getNodeSize() const {
    return pImpl->nodeSize;
}

void NetworkView::enableNodeLabels(bool enabled) {
    pImpl->nodeLabelsEnabled = enabled;
}

bool NetworkView::areNodeLabelsEnabled() const {
    return pImpl->nodeLabelsEnabled;
}

void NetworkView::enableEdgeLabels(bool enabled) {
    pImpl->edgeLabelsEnabled = enabled;
}

bool NetworkView::areEdgeLabelsEnabled() const {
    return pImpl->edgeLabelsEnabled;
}

void NetworkView::setLabelFontSize(float size) {
    pImpl->labelFontSize = size;
}

float NetworkView::getLabelFontSize() const {
    return pImpl->labelFontSize;
}

void NetworkView::enableTooltips(bool enabled) {
    pImpl->tooltipsEnabled = enabled;
}

bool NetworkView::areTooltipsEnabled() const {
    return pImpl->tooltipsEnabled;
}

void NetworkView::setAnimationSpeed(float speed) {
    pImpl->animationSpeed = speed;
}

float NetworkView::getAnimationSpeed() const {
    return pImpl->animationSpeed;
}

void NetworkView::enableAnimations(bool enabled) {
    pImpl->animationsEnabled = enabled;
}

bool NetworkView::areAnimationsEnabled() const {
    return pImpl->animationsEnabled;
}

void NetworkView::setPosition(float x, float y) {
    pImpl->position = {x, y};
}

std::array<float, 2> NetworkView::getPosition() const {
    return pImpl->position;
}

void NetworkView::setScale(float scale) {
    pImpl->scale = scale;
}

float NetworkView::getScale() const {
    return pImpl->scale;
}

void NetworkView::setRotation(float rotation) {
    pImpl->rotation = rotation;
}

float NetworkView::getRotation() const {
    return pImpl->rotation;
}

void NetworkView::enable3D(bool enabled) {
    pImpl->enabled3D = enabled;
}

bool NetworkView::is3DEnabled() const {
    return pImpl->enabled3D;
}

void NetworkView::setDepth(float depth) {
    pImpl->depth = depth;
}

float NetworkView::getDepth() const {
    return pImpl->depth;
}

void NetworkView::setCameraDistance(float distance) {
    pImpl->cameraDistance = distance;
}

float NetworkView::getCameraDistance() const {
    return pImpl->cameraDistance;
}

void NetworkView::setFieldOfView(float fov) {
    pImpl->fieldOfView = fov;
}

float NetworkView::getFieldOfView() const {
    return pImpl->fieldOfView;
}

void NetworkView::reset() {
    setName("NetworkView");
    enable(true);
    setOpacity(1.0f);
    setColorScheme("default");
    setEdgeWidth(1.0f);
    setNodeSize(5.0f);
    enableNodeLabels(true);
    enableEdgeLabels(true);
    setLabelFontSize(12.0f);
    enableTooltips(true);
    setAnimationSpeed(1.0f);
    enableAnimations(true);
    setPosition(0.0f, 0.0f);
    setScale(1.0f);
    setRotation(0.0f);
    enable3D(false);
    setDepth(1.0f);
    setCameraDistance(10.0f);
    setFieldOfView(45.0f);
}

} // namespace nlm