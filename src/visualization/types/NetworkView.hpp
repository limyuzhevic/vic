#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class NetworkView {
public:
    NetworkView();
    virtual ~NetworkView();

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

    // Set opacity
    void setOpacity(float opacity);
    float getOpacity() const;

    // Set color scheme
    void setColorScheme(const std::string& scheme);
    const std::string& getColorScheme() const;

    // Set edge width
    void setEdgeWidth(float width);
    float getEdgeWidth() const;

    // Set node size
    void setNodeSize(float size);
    float getNodeSize() const;

    // Enable/disable node labels
    void enableNodeLabels(bool enabled);
    bool areNodeLabelsEnabled() const;

    // Enable/disable edge labels
    void enableEdgeLabels(bool enabled);
    bool areEdgeLabelsEnabled() const;

    // Set label font size
    void setLabelFontSize(float size);
    float getLabelFontSize() const;

    // Enable/disable tooltips
    void enableTooltips(bool enabled);
    bool areTooltipsEnabled() const;

    // Set animation speed
    void setAnimationSpeed(float speed);
    float getAnimationSpeed() const;

    // Enable/disable animations
    void enableAnimations(bool enabled);
    bool areAnimationsEnabled() const;

    // Set view position
    void setPosition(float x, float y);
    std::array<float, 2> getPosition() const;

    // Set view scale
    void setScale(float scale);
    float getScale() const;

    // Set view rotation
    void setRotation(float rotation);
    float getRotation() const;

    // Enable/disable 3D
    void enable3D(bool enabled);
    bool is3DEnabled() const;

    // Set depth
    void setDepth(float depth);
    float getDepth() const;

    // Set camera distance
    void setCameraDistance(float distance);
    float getCameraDistance() const;

    // Set field of view
    void setFieldOfView(float fov);
    float getFieldOfView() const;

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