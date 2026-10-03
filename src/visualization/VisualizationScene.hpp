#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

class VisualizationActor;
class VisualizationLight;
class VisualizationCamera;

// Scene for organizing visualization elements
class VisualizationScene {
public:
    VisualizationScene();
    ~VisualizationScene();

    // Add an actor to the scene
    void addActor(std::unique_ptr<VisualizationActor> actor);
    
    // Remove an actor from the scene
    void removeActor(VisualizationActor* actor);
    
    // Get all actors in the scene
    const std::vector<VisualizationActor*>& getActors() const;
    
    // Add a light to the scene
    void addLight(std::unique_ptr<VisualizationLight> light);
    
    // Remove a light from the scene
    void removeLight(VisualizationLight* light);
    
    // Get all lights in the scene
    const std::vector<VisualizationLight*>& getLights() const;
    
    // Add a camera to the scene
    void addCamera(std::unique_ptr<VisualizationCamera> camera);
    
    // Remove a camera from the scene
    void removeCamera(VisualizationCamera* camera);
    
    // Get all cameras in the scene
    const std::vector<VisualizationCamera*>& getCameras() const;
    
    // Set the active camera
    void setActiveCamera(VisualizationCamera* camera);
    
    // Get the active camera
    VisualizationCamera* getActiveCamera() const;
    
    // Update the scene
    void update(double deltaTime);
    
    // Render the scene
    void render();
    
    // Clear all actors, lights, and cameras
    void clear();
    
    // Get scene name
    const std::string& getName() const;
    
    // Set scene name
    void setName(const std::string& name);
    
    // Check if scene is active
    bool isActive() const;
    
    // Activate/deactivate scene
    void setActive(bool active);
    
    // Enable/disable scene culling
    void enableCulling(bool enabled);
    
    // Check if culling is enabled
    bool isCullingEnabled() const;
    
    // Enable/disable scene lighting
    void enableLighting(bool enabled);
    
    // Check if lighting is enabled
    bool isLightingEnabled() const;
    
    // Enable/disable shadows
    void enableShadows(bool enabled);
    
    // Check if shadows are enabled
    bool areShadowsEnabled() const;
    
    // Set ambient light color
    void setAmbientLightColor(float r, float g, float b);
    
    // Get ambient light color
    std::array<float, 3> getAmbientLightColor() const;
    
    // Set fog parameters
    void setFog(bool enabled, float density, float r, float g, float b);
    
    // Get fog parameters
    std::array<float, 4> getFogParameters() const;
    
    // Enable/disable depth testing
    void enableDepthTesting(bool enabled);
    
    // Check if depth testing is enabled
    bool isDepthTestingEnabled() const;
    
    // Enable/disable face culling
    void enableFaceCulling(bool enabled);
    
    // Check if face culling is enabled
    bool isFaceCullingEnabled() const;
    
    // Enable/disable texture rendering
    void enableTextureRendering(bool enabled);
    
    // Check if texture rendering is enabled
    bool isTextureRenderingEnabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm