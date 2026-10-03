#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

class VisualizationRenderer;
class VisualizationScene;
class VisualizationCamera;

// Visualization controller for user interaction
class VisualizationController {
public:
    VisualizationController();
    ~VisualizationController();

    // Initialize controller with renderer and scene
    void initialize(VisualizationRenderer* renderer, VisualizationScene* scene);

    // Update controller
    void update(double deltaTime);

    // Handle mouse events
    void handleMousePress(double x, double y, int button);
    void handleMouseRelease(double x, double y, int button);
    void handleMouseMove(double x, double y);
    void handleMouseScroll(double delta);

    // Handle keyboard events
    void handleKeyPress(int key, int mods);
    void handleKeyRelease(int key, int mods);

    // Handle touch events
    void handleTouchBegin(double x, double y);
    void handleTouchEnd(double x, double y);
    void handleTouchMove(double x, double y);

    // Set camera
    void setCamera(VisualizationCamera* camera);
    VisualizationCamera* getCamera() const;

    // Set active view
    void setActiveView(const std::string& viewName);
    const std::string& getActiveView() const;

    // Enable/disable interaction
    void enableInteraction(bool enabled);
    bool isInteractionEnabled() const;

    // Enable/disable camera control
    void enableCameraControl(bool enabled);
    bool isCameraControlEnabled() const;

    // Enable/disable object picking
    void enableObjectPicking(bool enabled);
    bool isObjectPickingEnabled() const;

    // Enable/disable selection
    void enableSelection(bool enabled);
    bool isSelectionEnabled() const;

    // Enable/disable highlighting
    void enableHighlighting(bool enabled);
    bool isHighlightingEnabled() const;

    // Enable/disable annotations
    void enableAnnotations(bool enabled);
    bool areAnnotationsEnabled() const;

    // Set selection color
    void setSelectionColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getSelectionColor() const;

    // Set highlight color
    void setHighlightColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getHighlightColor() const;

    // Set annotation color
    void setAnnotationColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getAnnotationColor() const;

    // Add annotation
    void addAnnotation(const std::string& text, double x, double y, const std::string& font = "default");

    // Remove annotation
    void removeAnnotation(const std::string& id);

    // Clear all annotations
    void clearAnnotations();

    // Get all annotations
    const std::map<std::string, std::string>& getAnnotations() const;

    // Set camera distance
    void setCameraDistance(double distance);
    double getCameraDistance() const;

    // Set camera azimuth
    void setCameraAzimuth(double azimuth);
    double getCameraAzimuth() const;

    // Set camera elevation
    void setCameraElevation(double elevation);
    double getCameraElevation() const;

    // Set camera roll
    void setCameraRoll(double roll);
    double getCameraRoll() const;

    // Set field of view
    void setFieldOfView(double fov);
    double getFieldOfView() const;

    // Set near clipping plane
    void setNearClippingPlane(double near);
    double getNearClippingPlane() const;

    // Set far clipping plane
    void setFarClippingPlane(double far);
    double getFarClippingPlane() const;

    // Reset camera to default position
    void resetCamera();

    // Zoom in
    void zoomIn(double factor = 1.1);

    // Zoom out
    void zoomOut(double factor = 0.9);

    // Pan camera
    void pan(double dx, double dy);

    // Rotate camera
    void rotate(double azimuth, double elevation, double roll);

    // Set viewport
    void setViewport(int x, int y, int width, int height);

    // Get viewport
    std::array<int, 4> getViewport() const;

    // Enable/disable grid
    void enableGrid(bool enabled);
    bool isGridEnabled() const;

    // Set grid size
    void setGridSize(int size);
    int getGridSize() const;

    // Set grid color
    void setGridColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getGridColor() const;

    // Enable/disable crosshair
    void enableCrosshair(bool enabled);
    bool isCrosshairEnabled() const;

    // Set crosshair color
    void setCrosshairColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getCrosshairColor() const;

    // Enable/disable fps display
    void enableFPSDisplay(bool enabled);
    bool isFPSDisplayEnabled() const;

    // Enable/disable debug info
    void enableDebugInfo(bool enabled);
    bool isDebugInfoEnabled() const;

    // Get FPS
    double getFPS() const;

    // Get debug information
    std::string getDebugInfo() const;

    // Set cursor position
    void setCursorPosition(double x, double y);

    // Get cursor position
    std::array<double, 2> getCursorPosition() const;

    // Check if cursor is over UI element
    bool isCursorOverUIElement() const;

    // Get UI element under cursor
    std::string getUIElementUnderCursor() const;

    // Enable/disable ray casting
    void enableRayCasting(bool enabled);
    bool isRayCastingEnabled() const;

    // Cast ray from screen coordinates
    bool castRay(double x, double y, std::array<double, 3>& outDirection, std::array<double, 3>& outOrigin);

    // Select object at screen coordinates
    void selectObjectAt(double x, double y);

    // Get selected objects
    const std::vector<std::string>& getSelectedObjects() const;

    // Clear selection
    void clearSelection();

    // Add to selection
    void addToSelection(const std::string& objectId);

    // Remove from selection
    void removeFromSelection(const std::string& objectId);

    // Check if object is selected
    bool isObjectSelected(const std::string& objectId) const;

    // Enable/disable widget interaction
    void enableWidgetInteraction(bool enabled);
    bool isWidgetInteractionEnabled() const;

    // Enable/disable menu
    void enableMenu(bool enabled);
    bool isMenuEnabled() const;

    // Show menu at position
    void showMenu(double x, double y);

    // Hide menu
    void hideMenu();

    // Is menu visible?
    bool isMenuVisible() const;

    // Get menu position
    std::array<double, 2> getMenuPosition() const;

    // Set input mode
    void setInputMode(const std::string& mode);
    const std::string& getInputMode() const;

    // Enable/disable input simulation
    void enableInputSimulation(bool enabled);
    bool isInputSimulationEnabled() const;

    // Simulate input events
    void simulateKeyPress(int key);
    void simulateMouseClick(int button);
    void simulateMouseMove(double x, double y);

    // Set event handler
    void setEventHandler(class VisualizationEventHandler* handler);

    // Get event handler
    class VisualizationEventHandler* getEventHandler() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm