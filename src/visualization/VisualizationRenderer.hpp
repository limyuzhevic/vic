// ============================================================================
// Visualization Rendering Engine
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"
#include "VisualizationBase.hpp"
#include "VisualizationConfig.hpp"

namespace nlm {

/**
 * @enum RendererType
 * @brief Rendering engine type enumeration
 */
enum class RendererType {
    NullRenderer,       // No rendering (for testing)
    TerminalRenderer,   // Text-based rendering
    OpenGLRenderer,     // Hardware-accelerated OpenGL
    VulkanRenderer,     // Modern GPU Vulkan
    WebGLRenderer,      // Browser-based WebGL
    HDRIrrer,           // High-dynamic-range image
    VideoRenderer       // Video sequence rendering
};

/**
 * @class VisualizationRenderer
 * @brief Rendering engine for different output formats
 * 
 * This class provides the core rendering infrastructure that supports
 * multiple rendering backends including OpenGL, Vulkan, WebGL, and
 * terminal-based output. It handles 3D transformations, shader programs,
 * texture management, and frame rendering pipelines.
 */
class VisualizationRenderer {
public:
    VisualizationRenderer();
    ~VisualizationRenderer();
    
    // Rendering configuration
    bool initialize(const VisualizationConfig& config);
    void update(const VisualizationConfig& config);
    void render();
    void close();
    
    // Renderer control
    bool setRendererType(RendererType type);
    RendererType getRendererType() const { return rendererType_; }
    
    // Configuration
    bool configure(const VisualizationConfig& config);
    VisualizationConfig getConfiguration() const { return config_; }
    
    // Frame management
    void startFrame();
    void endFrame();
    bool isFrameComplete() const { return frameComplete_; }
    
    // Viewport and camera
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void setCamera(const std::string& name, float x, float y, float z, float angleX = 0.0f, float angleY = 0.0f);
    void setProjection(const std::string& name, float fov, float aspectRatio, float near, float far);
    
    // 3D transformations
    void translate(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);
    void scale(float x, float y, float z);
    void resetTransformations();
    
    // Shader management
    bool loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    bool setShaderUniform(const std::string& shaderName, const std::string& uniformName, float value);
    bool setShaderUniform(const std::string& shaderName, const std::string& uniformName, const std::string& value);
    bool setShaderUniform(const std::string& shaderName, const std::string& uniformName, const std::vector<float>& values);
    
    // Texture management
    bool createTexture(const std::string& name, const std::vector<uint8_t>& data, uint32_t width, uint32_t height);
    bool bindTexture(const std::string& name, uint32_t slot = 0);
    
    // Model rendering
    bool renderModel(const std::string& modelName, const std::string& shaderName, const std::map<std::string, float>& uniforms);
    bool renderPointCloud(const std::vector<float>& points, const std::string& shaderName, float pointSize = 1.0f);
    bool renderLines(const std::vector<float>& lines, const std::string& shaderName, float lineWidth = 1.0f);
    
    // Visualization primitives
    bool renderSphere(float radius, uint32_t segments = 16);
    bool renderCylinder(float radius, float height, uint32_t segments = 8);
    bool renderCone(float radius, float height, uint32_t segments = 8);
    bool renderBox(float width, float height, float depth);
    bool renderText(const std::string& text, float x, float y, float scale = 1.0f);
    
    // Performance monitoring
    double getFrameTime() const { return frameTime_; }
    double getFPS() const { return fps_; }
    uint32_t getTriangleCount() const { return triangleCount_; }
    uint32_t getDrawCallCount() const { return drawCallCount_; }
    
    // Statistics
    uint64_t getTotalFramesRendered() const { return totalFramesRendered_; }
    uint64_t getTotalPrimitivesRendered() const { return totalPrimitivesRendered_; }
    std::map<std::string, uint32_t> getShaderStats() const { return shaderStats_; }
    
    // Health and status
    bool isHealthy() const;
    std::string getHealthReport() const;
    bool isInitialized() const { return initialized_; }
    
    // Resource management
    void clearResources();
    void optimizeForPerformance();
    void dumpPerformanceStats(std::ostream& out);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initializeImpl(const VisualizationConfig& config);
    void updateImpl(const VisualizationConfig& config);
    void renderImpl();
    void closeImpl();
    
    // Configuration
    VisualizationConfig config_;
    RendererType rendererType_;
    
    // Rendering state
    bool initialized_;
    bool frameComplete_;
    uint32_t currentViewportX_;
    uint32_t currentViewportY_;
    uint32_t currentViewportWidth_;
    uint32_t currentViewportHeight_;
    
    // Performance metrics
    double frameTime_;
    double fps_;
    uint32_t triangleCount_;
    uint32_t drawCallCount_;
    uint64_t totalFramesRendered_;
    uint64_t totalPrimitivesRendered_;
    
    // Statistics
    std::map<std::string, uint32_t> shaderStats_;
    std::map<std::string, uint32_t> textureStats_;
    
    // Timing
    std::chrono::steady_clock::time_point lastFrameTime_;
    uint64_t lastFrameCount_;
};

} // namespace nlm
