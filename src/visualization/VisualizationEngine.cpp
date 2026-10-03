#include "VisualizationEngine.hpp"

namespace nlm {

class DefaultVisualizationEngine : public VisualizationEngine {
public:
    DefaultVisualizationEngine() = default;
    ~DefaultVisualizationEngine() override = default;
    
    bool initialize(std::unique_ptr<VisualizationBackend> backend) override {
        m_backend = std::move(backend);
        return m_backend != nullptr;
    }
    
    void shutdown() override {
        m_backend.reset();
        m_actors.clear();
        m_lights.clear();
        m_cameras.clear();
    }
    
    void update(double deltaTime) override {
        // Update all actors
        for (auto* actor : m_actors) {
            actor->update(deltaTime);
        }
    }
    
    void render() override {
        if (m_backend) {
            m_backend->beginFrame();
            for (auto* actor : m_actors) {
                actor->render();
            }
            m_backend->endFrame();
        }
    }
    
    VisualizationBackend* getBackend() const override {
        return m_backend.get();
    }
    
    void setBackend(std::unique_ptr<VisualizationBackend> backend) override {
        m_backend = std::move(backend);
    }
    
    VisualizationActor* createActor(const std::string& type) override {
        // Implementation would create different types of actors
        VisualizationActor* actor = new VisualizationActor();
        m_actors.push_back(actor);
        return actor;
    }
    
    void removeActor(VisualizationActor* actor) override {
        auto it = std::find(m_actors.begin(), m_actors.end(), actor);
        if (it != m_actors.end()) {
            m_actors.erase(it);
            delete actor;
        }
    }
    
    const std::vector<VisualizationActor*>& getActors() const override {
        return m_actors;
    }
    
    VisualizationLight* createLight() override {
        VisualizationLight* light = new VisualizationLight();
        m_lights.push_back(light);
        return light;
    }
    
    void removeLight(VisualizationLight* light) override {
        auto it = std::find(m_lights.begin(), m_lights.end(), light);
        if (it != m_lights.end()) {
            m_lights.erase(it);
            delete light;
        }
    }
    
    const std::vector<VisualizationLight*>& getLights() const override {
        return m_lights;
    }
    
    VisualizationCamera* createCamera() override {
        VisualizationCamera* camera = new VisualizationCamera();
        m_cameras.push_back(camera);
        return camera;
    }
    
    void removeCamera(VisualizationCamera* camera) override {
        auto it = std::find(m_cameras.begin(), m_cameras.end(), camera);
        if (it != m_cameras.end()) {
            m_cameras.erase(it);
            delete camera;
        }
    }
    
    const std::vector<VisualizationCamera*>& getCameras() const override {
        return m_cameras;
    }
    
    void enableDebugRendering(bool enabled) override {
        m_debugRenderingEnabled = enabled;
    }
    
    bool isDebugRenderingEnabled() const override {
        return m_debugRenderingEnabled;
    }
    
    void enableWireframeMode(bool enabled) override {
        m_wireframeModeEnabled = enabled;
    }
    
    bool isWireframeModeEnabled() const override {
        return m_wireframeModeEnabled;
    }
    
    void setClearColor(float r, float g, float b, float a = 1.0f) override {
        m_clearColor = {r, g, b, a};
    }
    
    std::array<float, 4> getClearColor() const override {
        return m_clearColor;
    }
    
    void enableVerticalSync(bool enabled) override {
        m_verticalSyncEnabled = enabled;
    }
    
    bool isVerticalSyncEnabled() const override {
        return m_verticalSyncEnabled;
    }
    
    void setTargetFrameRate(double fps) override {
        m_targetFrameRate = fps;
    }
    
    double getTargetFrameRate() const override {
        return m_targetFrameRate;
    }
    
    bool captureScreenshot(const std::string& filepath) override {
        // Implementation would capture screenshot from backend
        return false;
    }
    
    void startRecording(const std::string& filepath) override {
        // Implementation would start recording
    }
    
    void stopRecording() override {
        // Implementation would stop recording
    }
    
    bool isRecording() const override {
        return m_recording;
    }

private:
    std::unique_ptr<VisualizationBackend> m_backend;
    std::vector<VisualizationActor*> m_actors;
    std::vector<VisualizationLight*> m_lights;
    std::vector<VisualizationCamera*> m_cameras;
    bool m_debugRenderingEnabled = false;
    bool m_wireframeModeEnabled = false;
    std::array<float, 4> m_clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    bool m_verticalSyncEnabled = true;
    double m_targetFrameRate = 60.0;
    bool m_recording = false;
    std::string m_recordingPath;
};

// Factory function to create a visualization engine
inline VisualizationEngine* createVisualizationEngine() {
    return new DefaultVisualizationEngine();
}

} // namespace nlm