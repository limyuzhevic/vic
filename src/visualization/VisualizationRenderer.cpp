#include "VisualizationRenderer.hpp"

namespace nlm {

class DefaultVisualizationRenderer : public VisualizationRenderer {
public:
    DefaultVisualizationRenderer() = default;
    ~DefaultVisualizationRenderer() override = default;
    
    bool initialize(VisualizationEngine* engine) override {
        m_engine = engine;
        return m_engine != nullptr;
    }
    
    void shutdown() override {
        m_engine = nullptr;
        m_postProcessingEffects.clear();
    }
    
    void update(double deltaTime) override {
        // Update post-processing effects
        for (auto& effect : m_postProcessingEffects) {
            effect.second->update(deltaTime);
        }
    }
    
    void render(VisualizationScene* scene) override {
        if (!m_engine || !m_renderingEnabled) return;
        
        // Set viewport
        m_engine->getBackend()->setViewport(m_viewport[0], m_viewport[1], 
                                             m_viewport[2], m_viewport[3]);
        
        // Set clear color
        m_engine->getBackend()->setClearColor(m_clearColor[0], m_clearColor[1], 
                                              m_clearColor[2], m_clearColor[3]);
        
        // Clear buffers
        m_engine->getBackend()->clearBuffers(m_depthBufferEnabled, m_stencilBufferEnabled);
        
        // Set scissor test
        if (m_scissorTestEnabled) {
            m_engine->getBackend()->enableScissorTest(true);
            m_engine->getBackend()->setScissor(m_scissorRect);
        }
        
        // Render scene
        if (scene) {
            scene->render();
        }
        
        // Apply post-processing
        if (!m_postProcessingEffects.empty()) {
            applyPostProcessing();
        }
    }
    
    VisualizationEngine* getEngine() const override {
        return m_engine;
    }
    
    void setEngine(VisualizationEngine* engine) override {
        m_engine = engine;
    }
    
    void enableRendering(bool enabled) override {
        m_renderingEnabled = enabled;
    }
    
    bool isRenderingEnabled() const override {
        return m_renderingEnabled;
    }
    
    void setRenderMode(const std::string& mode) override {
        m_renderMode = mode;
    }
    
    std::string getRenderMode() const override {
        return m_renderMode;
    }
    
    void enablePostProcessing(bool enabled) override {
        m_postProcessingEnabled = enabled;
    }
    
    bool isPostProcessingEnabled() const override {
        return m_postProcessingEnabled;
    }
    
    void addPostProcessingEffect(const std::string& effect) override {
        if (m_postProcessingEnabled) {
            m_postProcessingEffects[effect] = createPostProcessingEffect(effect);
        }
    }
    
    void removePostProcessingEffect(const std::string& effect) override {
        auto it = m_postProcessingEffects.find(effect);
        if (it != m_postProcessingEffects.end()) {
            m_postProcessingEffects.erase(it);
        }
    }
    
    const std::vector<std::string>& getPostProcessingEffects() const override {
        static std::vector<std::string> effects;
        effects.clear();
        for (const auto& pair : m_postProcessingEffects) {
            effects.push_back(pair.first);
        }
        return effects;
    }
    
    void setRenderQuality(float quality) override {
        m_renderQuality = quality;
        if (m_engine) {
            m_engine->getBackend()->setRenderQuality(quality);
        }
    }
    
    float getRenderQuality() const override {
        return m_renderQuality;
    }
    
    void enableAntiAliasing(bool enabled) override {
        m_antiAliasingEnabled = enabled;
        if (m_engine) {
            m_engine->getBackend()->enableAntiAliasing(enabled);
        }
    }
    
    bool isAntiAliasingEnabled() const override {
        return m_antiAliasingEnabled;
    }
    
    void setAntiAliasingQuality(int quality) override {
        m_antiAliasingQuality = quality;
        if (m_engine) {
            m_engine->getBackend()->setAntiAliasingQuality(quality);
        }
    }
    
    int getAntiAliasingQuality() const override {
        return m_antiAliasingQuality;
    }
    
    bool captureFrame(const std::string& filepath) override {
        if (m_engine) {
            return m_engine->getBackend()->captureFrame(filepath);
        }
        return false;
    }
    
    void setFrameRate(double fps) override {
        m_frameRate = fps;
        if (m_engine) {
            m_engine->getBackend()->setFrameRate(fps);
        }
    }
    
    double getFrameRate() const override {
        return m_frameRate;
    }
    
    void enableVerticalSync(bool enabled) override {
        m_verticalSyncEnabled = enabled;
        if (m_engine) {
            m_engine->getBackend()->enableVerticalSync(enabled);
        }
    }
    
    bool isVerticalSyncEnabled() const override {
        return m_verticalSyncEnabled;
    }
    
    void setViewport(int x, int y, int width, int height) override {
        m_viewport = {x, y, width, height};
    }
    
    std::array<int, 4> getViewport() const override {
        return m_viewport;
    }
    
    void setCamera(VisualizationCamera* camera) override {
        m_camera = camera;
    }
    
    VisualizationCamera* getCamera() const override {
        return m_camera;
    }
    
    void setLight(VisualizationLight* light) override {
        m_light = light;
    }
    
    VisualizationLight* getLight() const override {
        return m_light;
    }
    
    void setClearColor(float r, float g, float b, float a = 1.0f) override {
        m_clearColor = {r, g, b, a};
    }
    
    std::array<float, 4> getClearColor() const override {
        return m_clearColor;
    }
    
    void enableDepthBuffer(bool enabled) override {
        m_depthBufferEnabled = enabled;
    }
    
    bool isDepthBufferEnabled() const override {
        return m_depthBufferEnabled;
    }
    
    void enableStencilBuffer(bool enabled) override {
        m_stencilBufferEnabled = enabled;
    }
    
    bool isStencilBufferEnabled() const override {
        return m_stencilBufferEnabled;
    }
    
    void enableScissorTest(bool enabled) override {
        m_scissorTestEnabled = enabled;
    }
    
    bool isScissorTestEnabled() const override {
        return m_scissorTestEnabled;
    }
    
    void setScissorRect(int x, int y, int width, int height) {
        m_scissorRect = {x, y, width, height};
    }
    
    std::array<int, 4> getScissorRect() const {
        return m_scissorRect;
    }
    
    std::unique_ptr<class VisualizationPostProcessor> createPostProcessingEffect(const std::string& effect) {
        // Implementation would create different types of post-processing effects
        return std::make_unique<VisualizationPostProcessor>();
    }
    
    void applyPostProcessing() {
        // Implementation would apply all post-processing effects
    }

private:
    VisualizationEngine* m_engine;
    bool m_renderingEnabled;
    std::string m_renderMode;
    bool m_postProcessingEnabled;
    std::map<std::string, std::unique_ptr<class VisualizationPostProcessor>> m_postProcessingEffects;
    float m_renderQuality;
    bool m_antiAliasingEnabled;
    int m_antiAliasingQuality;
    double m_frameRate;
    bool m_verticalSyncEnabled;
    std::array<int, 4> m_viewport;
    VisualizationCamera* m_camera;
    VisualizationLight* m_light;
    std::array<float, 4> m_clearColor;
    bool m_depthBufferEnabled;
    bool m_stencilBufferEnabled;
    bool m_scissorTestEnabled;
    std::array<int, 4> m_scissorRect;
};

// Factory function to create a visualization renderer
inline VisualizationRenderer* createVisualizationRenderer() {
    return new DefaultVisualizationRenderer();
}

} // namespace nlm