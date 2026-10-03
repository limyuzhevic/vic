#include "VisualizationController.hpp"

namespace nlm {

struct VisualizationController::Impl {
    VisualizationRenderer* renderer;
    VisualizationScene* scene;
    VisualizationCamera* camera;
    std::string activeView;
    bool interactionEnabled;
    bool cameraControlEnabled;
    bool objectPickingEnabled;
    bool selectionEnabled;
    bool highlightingEnabled;
    bool annotationsEnabled;
    std::array<float, 4> selectionColor;
    std::array<float, 4> highlightColor;
    std::array<float, 4> annotationColor;
    double cameraDistance;
    double cameraAzimuth;
    double cameraElevation;
    double cameraRoll;
    double fieldOfView;
    double nearClippingPlane;
    double farClippingPlane;
    bool gridEnabled;
    int gridSize;
    std::array<float, 4> gridColor;
    bool crosshairEnabled;
    std::array<float, 4> crosshairColor;
    bool fpsDisplayEnabled;
    bool debugInfoEnabled;
    double fps;
    std::array<double, 2> cursorPosition;
    std::string uiElementUnderCursor;
    bool rayCastingEnabled;
    std::vector<std::string> selectedObjects;
    bool widgetInteractionEnabled;
    bool menuEnabled;
    std::array<double, 2> menuPosition;
    std::string inputMode;
    bool inputSimulationEnabled;
    VisualizationEventHandler* eventHandler;
    
    Impl() : renderer(nullptr), scene(nullptr), camera(nullptr), activeView("default"),
             interactionEnabled(true), cameraControlEnabled(true), objectPickingEnabled(true),
             selectionEnabled(true), highlightingEnabled(true), annotationsEnabled(true),
             selectionColor({0.0f, 1.0f, 0.0f, 1.0f}), highlightColor({1.0f, 0.0f, 0.0f, 1.0f}),
             annotationColor({1.0f, 1.0f, 0.0f, 1.0f}), cameraDistance(10.0), cameraAzimuth(0.0),
             cameraElevation(0.0), cameraRoll(0.0), fieldOfView(45.0), nearClippingPlane(0.1),
             farClippingPlane(1000.0), gridEnabled(true), gridSize(10),
             gridColor({0.5f, 0.5f, 0.5f, 0.5f}), crosshairEnabled(true),
             crosshairColor({1.0f, 1.0f, 1.0f, 1.0f}), fpsDisplayEnabled(true), debugInfoEnabled(false),
             fps(0.0), cursorPosition({0.0, 0.0}), rayCastingEnabled(true),
             widgetInteractionEnabled(true), menuEnabled(false), menuPosition({0.0, 0.0}),
             inputMode("default"), inputSimulationEnabled(false), eventHandler(nullptr) {}
};

VisualizationController::VisualizationController() : pImpl(std::make_unique<Impl>()) {}
VisualizationController::~VisualizationController() = default;

void VisualizationController::initialize(VisualizationRenderer* renderer, VisualizationScene* scene) {
    pImpl->renderer = renderer;
    pImpl->scene = scene;
}

void VisualizationController::update(double deltaTime) {
    // Update FPS
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime);
    pImpl->fps = 1000000.0 / duration.count();
    lastTime = currentTime;
}

void VisualizationController::handleMousePress(double x, double y, int button) {
    pImpl->cursorPosition = {x, y};
    
    if (pImpl->objectPickingEnabled && pImpl->rayCastingEnabled) {
        selectObjectAt(x, y);
    }
    
    if (pImpl->eventHandler) {
        pImpl->eventHandler->handleMousePress(x, y, button);
    }
}

void VisualizationController::handleMouseRelease(double x, double y, int button) {
    if (pImpl->eventHandler) {
        pImpl->eventHandler->handleMouseRelease(x, y, button);
    }
}

void VisualizationController::handleMouseMove(double x, double y) {
    pImpl->cursorPosition = {x, y};
    
    if (pImpl->eventHandler) {
        pImpl->eventHandler->handleMouseMove(x, y);
    }
}

void VisualizationController::handleMouseScroll(double delta) {
    // Zoom based on scroll
    double zoomFactor = 1.0 + delta * 0.01;
    zoomIn(zoomFactor);
    
    if (pImpl->eventHandler) {
        pImpl->eventHandler->handleMouseScroll(delta);
    }
}

void VisualizationController::handleKeyPress(int key, int mods) {
    if (pImpl->eventHandler) {
        pImpl->eventHandler->handleKeyPress(key, mods);
    }
}

void VisualizationController::handleKeyRelease(int key, int mods) {
    if (pImpl->eventHandler) {
        pImpl->eventHandler->handleKeyRelease(key, mods);
    }
}

void VisualizationController::handleTouchBegin(double x, double y) {
    // Handle touch for touch devices
}

void VisualizationController::handleTouchEnd(double x, double y) {
    // Handle touch for touch devices
}

void VisualizationController::handleTouchMove(double x, double y) {
    // Handle touch for touch devices
}

void VisualizationController::setCamera(VisualizationCamera* camera) {
    pImpl->camera = camera;
    if (pImpl->renderer) {
        pImpl->renderer->setCamera(camera);
    }
}

VisualizationCamera* VisualizationController::getCamera() const {
    return pImpl->camera;
}

void VisualizationController::setActiveView(const std::string& viewName) {
    pImpl->activeView = viewName;
}

const std::string& VisualizationController::getActiveView() const {
    return pImpl->activeView;
}

void VisualizationController::enableInteraction(bool enabled) {
    pImpl->interactionEnabled = enabled;
}

bool VisualizationController::isInteractionEnabled() const {
    return pImpl->interactionEnabled;
}

void VisualizationController::enableCameraControl(bool enabled) {
    pImpl->cameraControlEnabled = enabled;
}

bool VisualizationController::isCameraControlEnabled() const {
    return pImpl->cameraControlEnabled;
}

void VisualizationController::enableObjectPicking(bool enabled) {
    pImpl->objectPickingEnabled = enabled;
}

bool VisualizationController::isObjectPickingEnabled() const {
    return pImpl->objectPickingEnabled;
}

void VisualizationController::enableSelection(bool enabled) {
    pImpl->selectionEnabled = enabled;
}

bool VisualizationController::isSelectionEnabled() const {
    return pImpl->selectionEnabled;
}

void VisualizationController::enableHighlighting(bool enabled) {
    pImpl->highlightingEnabled = enabled;
}

bool VisualizationController::isHighlightingEnabled() const {
    return pImpl->highlightingEnabled;
}

void VisualizationController::enableAnnotations(bool enabled) {
    pImpl->annotationsEnabled = enabled;
}

bool VisualizationController::areAnnotationsEnabled() const {
    return pImpl->annotationsEnabled;
}

void VisualizationController::setSelectionColor(float r, float g, float b, float a = 1.0f) {
    pImpl->selectionColor = {r, g, b, a};
}

std::array<float, 4> VisualizationController::getSelectionColor() const {
    return pImpl->selectionColor;
}

void VisualizationController::setHighlightColor(float r, float g, float b, float a = 1.0f) {
    pImpl->highlightColor = {r, g, b, a};
}

std::array<float, 4> VisualizationController::getHighlightColor() const {
    return pImpl->highlightColor;
}

void VisualizationController::setAnnotationColor(float r, float g, float b, float a = 1.0f) {
    pImpl->annotationColor = {r, g, b, a};
}

std::array<float, 4> VisualizationController::getAnnotationColor() const {
    return pImpl->annotationColor;
}

void VisualizationController::addAnnotation(const std::string& text, double x, double y, const std::string& font = "default") {
    if (pImpl->annotationsEnabled) {
        // Implementation would add annotation to the scene
    }
}

void VisualizationController::removeAnnotation(const std::string& id) {
    // Implementation would remove annotation from the scene
}

void VisualizationController::clearAnnotations() {
    // Implementation would clear all annotations from the scene
}

const std::map<std::string, std::string>& VisualizationController::getAnnotations() const {
    static std::map<std::string, std::string> emptyAnnotations;
    return emptyAnnotations;
}

void VisualizationController::setCameraDistance(double distance) {
    pImpl->cameraDistance = distance;
    if (pImpl->camera) {
        pImpl->camera->setDistance(distance);
    }
}

double VisualizationController::getCameraDistance() const {
    return pImpl->cameraDistance;
}

void VisualizationController::setCameraAzimuth(double azimuth) {
    pImpl->cameraAzimuth = azimuth;
    if (pImpl->camera) {
        pImpl->camera->setAzimuth(azimuth);
    }
}

double VisualizationController::getCameraAzimuth() const {
    return pImpl->cameraAzimuth;
}

void VisualizationController::setCameraElevation(double elevation) {
    pImpl->cameraElevation = elevation;
    if (pImpl->camera) {
        pImpl->camera->setElevation(elevation);
    }
}

double VisualizationController::getCameraElevation() const {
    return pImpl->cameraElevation;
}

void VisualizationController::setCameraRoll(double roll) {
    pImpl->cameraRoll = roll;
    if (pImpl->camera) {
        pImpl->camera->setRoll(roll);
    }
}

double VisualizationController::getCameraRoll() const {
    return pImpl->cameraRoll;
}

void VisualizationController::setFieldOfView(double fov) {
    pImpl->fieldOfView = fov;
    if (pImpl->camera) {
        pImpl->camera->setFieldOfView(fov);
    }
}

double VisualizationController::getFieldOfView() const {
    return pImpl->fieldOfView;
}

void VisualizationController::setNearClippingPlane(double near) {
    pImpl->nearClippingPlane = near;
    if (pImpl->camera) {
        pImpl->camera->setNearClippingPlane(near);
    }
}

double VisualizationController::getNearClippingPlane() const {
    return pImpl->nearClippingPlane;
}

void VisualizationController::setFarClippingPlane(double far) {
    pImpl->farClippingPlane = far;
    if (pImpl->camera) {
        pImpl->camera->setFarClippingPlane(far);
    }
}

double VisualizationController::getFarClippingPlane() const {
    return pImpl->farClippingPlane;
}

void VisualizationController::resetCamera() {
    if (pImpl->camera) {
        pImpl->camera->reset();
    }
}

void VisualizationController::zoomIn(double factor) {
    if (pImpl->camera) {
        pImpl->camera->zoom(factor);
    }
}

void VisualizationController::zoomOut(double factor) {
    if (pImpl->camera) {
        pImpl->camera->zoom(1.0 / factor);
    }
}

void VisualizationController::pan(double dx, double dy) {
    if (pImpl->camera) {
        pImpl->camera->pan(dx, dy);
    }
}

void VisualizationController::rotate(double azimuth, double elevation, double roll) {
    if (pImpl->camera) {
        pImpl->camera->rotate(azimuth, elevation, roll);
    }
}

void VisualizationController::setViewport(int x, int y, int width, int height) {
    if (pImpl->renderer) {
        pImpl->renderer->setViewport(x, y, width, height);
    }
}

std::array<int, 4> VisualizationController::getViewport() const {
    if (pImpl->renderer) {
        return pImpl->renderer->getViewport();
    }
    return {0, 0, 0, 0};
}

void VisualizationController::enableGrid(bool enabled) {
    pImpl->gridEnabled = enabled;
}

bool VisualizationController::isGridEnabled() const {
    return pImpl->gridEnabled;
}

void VisualizationController::setGridSize(int size) {
    pImpl->gridSize = size;
}

int VisualizationController::getGridSize() const {
    return pImpl->gridSize;
}

void VisualizationController::setGridColor(float r, float g, float b, float a = 1.0f) {
    pImpl->gridColor = {r, g, b, a};
}

std::array<float, 4> VisualizationController::getGridColor() const {
    return pImpl->gridColor;
}

void VisualizationController::enableCrosshair(bool enabled) {
    pImpl->crosshairEnabled = enabled;
}

bool VisualizationController::isCrosshairEnabled() const {
    return pImpl->crosshairEnabled;
}

void VisualizationController::setCrosshairColor(float r, float g, float b, float a = 1.0f) {
    pImpl->crosshairColor = {r, g, b, a};
}

std::array<float, 4> VisualizationController::getCrosshairColor() const {
    return pImpl->crosshairColor;
}

void VisualizationController::enableFPSDisplay(bool enabled) {
    pImpl->fpsDisplayEnabled = enabled;
}

bool VisualizationController::isFPSDisplayEnabled() const {
    return pImpl->fpsDisplayEnabled;
}

void VisualizationController::enableDebugInfo(bool enabled) {
    pImpl->debugInfoEnabled = enabled;
}

bool VisualizationController::isDebugInfoEnabled() const {
    return pImpl->debugInfoEnabled;
}

double VisualizationController::getFPS() const {
    return pImpl->fps;
}

std::string VisualizationController::getDebugInfo() const {
    std::string info = "FPS: " + std::to_string(pImpl->fps) + "\n";
    info += "Camera: Distance=" + std::to_string(pImpl->cameraDistance) + ", "
            + "Azimuth=" + std::to_string(pImpl->cameraAzimuth) + ", "
            + "Elevation=" + std::to_string(pImpl->cameraElevation) + ", "
            + "Roll=" + std::to_string(pImpl->cameraRoll) + "\n";
    return info;
}

void VisualizationController::setCursorPosition(double x, double y) {
    pImpl->cursorPosition = {x, y};
}

std::array<double, 2> VisualizationController::getCursorPosition() const {
    return pImpl->cursorPosition;
}

bool VisualizationController::isCursorOverUIElement() const {
    return !pImpl->uiElementUnderCursor.empty();
}

std::string VisualizationController::getUIElementUnderCursor() const {
    return pImpl->uiElementUnderCursor;
}

void VisualizationController::enableRayCasting(bool enabled) {
    pImpl->rayCastingEnabled = enabled;
}

bool VisualizationController::isRayCastingEnabled() const {
    return pImpl->rayCastingEnabled;
}

bool VisualizationController::castRay(double x, double y, std::array<double, 3>& outDirection, std::array<double, 3>& outOrigin) {
    // Implementation would cast ray from screen coordinates
    return false;
}

void VisualizationController::selectObjectAt(double x, double y) {
    if (pImpl->selectionEnabled) {
        // Implementation would select object at screen coordinates
    }
}

const std::vector<std::string>& VisualizationController::getSelectedObjects() const {
    return pImpl->selectedObjects;
}

void VisualizationController::clearSelection() {
    pImpl->selectedObjects.clear();
}

void VisualizationController::addToSelection(const std::string& objectId) {
    if (std::find(pImpl->selectedObjects.begin(), pImpl->selectedObjects.end(), objectId) == pImpl->selectedObjects.end()) {
        pImpl->selectedObjects.push_back(objectId);
    }
}

void VisualizationController::removeFromSelection(const std::string& objectId) {
    auto it = std::find(pImpl->selectedObjects.begin(), pImpl->selectedObjects.end(), objectId);
    if (it != pImpl->selectedObjects.end()) {
        pImpl->selectedObjects.erase(it);
    }
}

bool VisualizationController::isObjectSelected(const std::string& objectId) const {
    return std::find(pImpl->selectedObjects.begin(), pImpl->selectedObjects.end(), objectId) != pImpl->selectedObjects.end();
}

void VisualizationController::enableWidgetInteraction(bool enabled) {
    pImpl->widgetInteractionEnabled = enabled;
}

bool VisualizationController::isWidgetInteractionEnabled() const {
    return pImpl->widgetInteractionEnabled;
}

void VisualizationController::enableMenu(bool enabled) {
    pImpl->menuEnabled = enabled;
}

bool VisualizationController::isMenuEnabled() const {
    return pImpl->menuEnabled;
}

void VisualizationController::showMenu(double x, double y) {
    pImpl->menuEnabled = true;
    pImpl->menuPosition = {x, y};
}

void VisualizationController::hideMenu() {
    pImpl->menuEnabled = false;
}

bool VisualizationController::isMenuVisible() const {
    return pImpl->menuEnabled;
}

std::array<double, 2> VisualizationController::getMenuPosition() const {
    return pImpl->menuPosition;
}

void VisualizationController::setInputMode(const std::string& mode) {
    pImpl->inputMode = mode;
}

const std::string& VisualizationController::getInputMode() const {
    return pImpl->inputMode;
}

void VisualizationController::enableInputSimulation(bool enabled) {
    pImpl->inputSimulationEnabled = enabled;
}

bool VisualizationController::isInputSimulationEnabled() const {
    return pImpl->inputSimulationEnabled;
}

void VisualizationController::simulateKeyPress(int key) {
    // Implementation would simulate key press
}

void VisualizationController::simulateMouseClick(int button) {
    // Implementation would simulate mouse click
}

void VisualizationController::simulateMouseMove(double x, double y) {
    // Implementation would simulate mouse move
}

void VisualizationController::setEventHandler(VisualizationEventHandler* handler) {
    pImpl->eventHandler = handler;
}

VisualizationEventHandler* VisualizationController::getEventHandler() const {
    return pImpl->eventHandler;
}

} // namespace nlm