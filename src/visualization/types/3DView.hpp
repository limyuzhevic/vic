#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class View3D {
public:
    View3D();
    virtual ~View3D();

    // Initialize the view
    virtual bool initialize() = 0;

    // Update the view with new data
    virtual void update(const VisualizationData& data) = 0;

    // Render the view
    virtual void render() = 0;

    // Get view name
    const std::string& getName() const;

    // Set view name
    void setName(const std::string& name);

    // Enable/disable view
    void enable(bool enabled);
    bool isEnabled() const;

    // Set projection type
    void setProjectionType(const std::string& type);
    const std::string& getProjectionType() const;

    // Set camera position
    void setCameraPosition(float x, float y, float z);
    std::array<float, 3> getCameraPosition() const;

    // Set camera look-at
    void setCameraLookAt(float x, float y, float z);
    std::array<float, 3> getCameraLookAt() const;

    // Set field of view
    void setFieldOfView(float fov);
    float getFieldOfView() const;

    // Set near clipping plane
    void setNearClippingPlane(float near);
    float getNearClippingPlane() const;

    // Set far clipping plane
    void setFarClippingPlane(float far);
    float getFarClippingPlane() const;

    // Enable/disable lighting
    void enableLighting(bool enabled);
    bool isLightingEnabled() const;

    // Enable/disable shadows
    void enableShadows(bool enabled);
    bool areShadowsEnabled() const;

    // Enable/disable wireframe
    void enableWireframe(bool enabled);
    bool isWireframeEnabled() const;

    // Enable/disable backface culling
    void enableBackfaceCulling(bool enabled);
    bool isBackfaceCullingEnabled() const;

    // Set point size
    void setPointSize(float size);
    float getPointSize() const;

    // Set line width
    void setLineWidth(float width);
    float getLineWidth() const;

    // Set triangle size
    void setTriangleSize(float size);
    float getTriangleSize() const;

    // Enable/disable animations
    void enableAnimations(bool enabled);
    bool areAnimationsEnabled() const;

    // Set animation speed
    void setAnimationSpeed(float speed);
    float getAnimationSpeed() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Reset view to default
    virtual void reset();

    // Export view data
    virtual bool exportView(const std::string& filepath) = 0;

    // Get view statistics
    virtual std::map<std::string, double> getStatistics() const = 0;

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm