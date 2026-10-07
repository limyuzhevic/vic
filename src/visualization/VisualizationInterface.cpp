#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <queue>
#include <map>
#include <vector>

namespace nlm {

struct ShaderProgram {
    unsigned int id;
    bool load(const std::string& vertexSource, const std::string& fragmentSource);
    void use();
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setVec3(const std::string& name, const glm::vec3& vec);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void setBool(const std::string& name, bool value);
    void setVec4(const std::string& name, const glm::vec4& vec);
    void setMat4Array(const std::string& name, const std::vector<glm::mat4>& matrices, int count);
    void setFloatArray(const std::string& name, const std::vector<float>& values, int count);
};

struct OpenGLRenderer {
    GLFWwindow* window;
    unsigned int VAO, VBO, EBO;
    ShaderProgram shader;
    unsigned int textureAtlas;
    int windowWidth, windowHeight;
    bool initialized;
    
    OpenGLRenderer();
    ~OpenGLRenderer();
    bool initialize(int width, int height);
    void renderFrame();
    void resize(int width, int height);
    void clear(float r, float g, float b, float a);
    void present();
    bool shouldClose();
    void setWindowTitle(const std::string& title);
    void setVSync(bool enable);
};

struct NetworkData {
    std::vector<glm::vec3> nodePositions;
    std::vector<glm::vec3> nodeColors;
    std::vector<glm::vec3> nodeLabels;
    std::vector<std::pair<size_t, size_t>> connections;
    std::vector<glm::vec4> connectionColors;
    std::vector<float> connectionWeights;
    size_t nodeCount;
    size_t connectionCount;
};

struct ActivityData {
    std::vector<glm::vec3> neuronPositions;
    std::vector<float> firingRates;
    std::vector<float> membranePotentials;
    std::vector<glm::vec4> activityColors;
    std::vector<std::vector<glm::vec3>> spikeTrains;
    size_t neuronCount;
    double time;
};

struct SpikeRasterData {
    std::vector<double> spikeTimes;
    std::vector<int> neuronIndices;
    std::vector<glm::vec4> spikeColors;
    double duration;
    int maxSpikes;
};

struct WeightMatrixData {
    std::vector<std::vector<float>> weights;
    std::vector<glm::vec3> neuronPositions;
    float minWeight;
    float maxWeight;
    int matrixSize;
};

struct TimeSeriesData {
    std::string name;
    std::deque<double> values;
    double duration;
    double time;
    glm::vec3 color;
    bool active;
};

struct VisualizationInterface::Impl {
    OpenGLRenderer renderer;
    NetworkData networkData;
    ActivityData activityData;
    SpikeRasterData spikeRasterData;
    WeightMatrixData weightMatrixData;
    std::map<std::string, TimeSeriesData> timeSeries;
    std::queue<std::string> statusMessages;
    
    double updateRate;
    double lastUpdateTime;
    bool active;
    bool debugMode;
    int debugLevel;
    double currentProgress;
    std::string currentStatus;
    bool showGrid;
    bool showLabels;
    bool showConnections;
    
    VisualizationStats stats;
    std::chrono::high_resolution_clock::time_point frameStart;
    uint64_t totalFrames;
    
    Impl() : updateRate(30.0), lastUpdateTime(0.0), active(false), 
             debugMode(false), debugLevel(0), currentProgress(0.0),
             currentStatus("Ready"), showGrid(true), showLabels(true),
             showConnections(true), totalFrames(0) {
        frameStart = std::chrono::high_resolution_clock::now();
        stats.fps = 0.0;
        stats.lastFrameTime = 0.0;
        stats.trianglesRendered = 0;
        stats.verticesRendered = 0;
        stats.memoryUsage = 0;
        stats.cpuUsage = 0.0;
        stats.gpuUsage = 0.0;
    }
};

// Shader implementation
bool ShaderProgram::load(const std::string& vertexSource, const std::string& fragmentSource) {
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSourceCStr = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);
    glCompileShader(vertexShader);
    
    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSourceCStr = fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSourceCStr, nullptr);
    glCompileShader(fragmentShader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Create shader program
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    
    // Check linking
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void ShaderProgram::use() {
    glUseProgram(id);
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& vec) {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
}

void ShaderProgram::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& vec) {
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
}

void ShaderProgram::setMat4Array(const std::string& name, const std::vector<glm::mat4>& matrices, int count) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), count, GL_FALSE, &matrices[0][0][0]);
}

void ShaderProgram::setFloatArray(const std::string& name, const std::vector<float>& values, int count) {
    glUniform1fv(glGetUniformLocation(id, name.c_str()), count, values.data());
}

// OpenGLRenderer implementation
OpenGLRenderer::OpenGLRenderer() : window(nullptr), VAO(0), VBO(0), EBO(0), 
                                  textureAtlas(0), windowWidth(1920), windowHeight(1080),
                                  initialized(false) {}

OpenGLRenderer::~OpenGLRenderer() {
    if (initialized) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool OpenGLRenderer::initialize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    // Create window
    window = glfwCreateWindow(width, height, "NLM Neural Visualization", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // Make context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Create vertex array
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Initialize shader
    const char* vertexShaderSource = R"(
        #version 410 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 ourColor;
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            ourColor = aColor;
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 410 core
        in vec3 ourColor;
        in vec2 TexCoord;
        
        out vec4 FragColor;
        
        uniform sampler2D textureAtlas;
        uniform bool useTexture;
        
        void main() {
            if (useTexture) {
                FragColor = texture(textureAtlas, TexCoord);
            } else {
                FragColor = vec4(ourColor, 1.0);
            }
        }
    )";
    
    if (!shader.load(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to load shader program" << std::endl;
        return false;
    }
    
    initialized = true;
    return true;
}

void OpenGLRenderer::renderFrame() {
    if (!initialized) return;
    
    glfwPollEvents();
    
    clear(0.1f, 0.1f, 0.1f, 1.0f);
    
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
    projection = glm::perspective(glm::radians(45.0f), 
                                  (float)windowWidth / (float)windowHeight, 
                                  0.1f, 100.0f);
    
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setBool("useTexture", false);
    
    present();
}

void OpenGLRenderer::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::present() {
    glfwSwapBuffers(window);
}

bool OpenGLRenderer::shouldClose() {
    return glfwWindowShouldClose(window);
}

void OpenGLRenderer::setWindowTitle(const std::string& title) {
    glfwSetWindowTitle(window, title.c_str());
}

void OpenGLRenderer::setVSync(bool enable) {
    glfwSwapInterval(enable ? 1 : 0);
}

// VisualizationInterface implementation
VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() {
    if (pImpl->active) {
        pImpl->renderer.clear(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

bool VisualizationInterface::initialize(bool useVulkan, bool useOpenGL, 
                                       int width, int height) {
    if (pImpl->active) return true;
    
    // Initialize OpenGL renderer (Vulkan would be implemented similarly)
    if (!pImpl->renderer.initialize(width, height)) {
        std::cerr << "Failed to initialize OpenGL renderer" << std::endl;
        return false;
    }
    
    pImpl->renderer.setWindowTitle("NLM Neural Visualization");
    pImpl->renderer.setVSync(true);
    
    pImpl->active = true;
    pImpl->currentStatus = "Visualization initialized";
    
    NLM_LOG_INFO("VisualizationInterface initialized successfully");
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = currentTime - pImpl->frameStart;
    pImpl->lastUpdateTime = elapsed.count();
    
    // Update time series
    for (auto& pair : pImpl->timeSeries) {
        auto& ts = pair.second;
        ts.time += elapsed.count();
        if (ts.values.size() > static_cast<size_t>(pImpl->renderer.windowHeight)) {
            ts.values.pop_front();
        }
    }
    
    // Update statistics
    pImpl->stats.lastFrameTime = pImpl->lastUpdateTime;
    pImpl->stats.fps = 1.0 / (pImpl->lastUpdateTime + 0.0001);
    pImpl->stats.memoryUsage = 0; // Would be obtained from system monitoring
    pImpl->stats.cpuUsage = 0.0;  // Would be obtained from system monitoring
    pImpl->stats.gpuUsage = 0.0;  // Would be obtained from GPU monitoring
    
    pImpl->totalFrames++;
    pImpl->frameStart = currentTime;
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    pImpl->renderer.renderFrame();
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        pImpl->renderer.clear(0.0f, 0.0f, 0.0f, 1.0f);
        pImpl->active = false;
        pImpl->currentStatus = "Visualization closed";
        NLM_LOG_INFO("VisualizationInterface closed");
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setWindowTitle(const std::string& title) {
    pImpl->renderer.setWindowTitle(title);
}

void VisualizationInterface::setBackgroundColor(float r, float g, float b, float a) {
    // Would update the clear color in renderer
}

void VisualizationInterface::setShowGrid(bool show) {
    pImpl->showGrid = show;
}

void VisualizationInterface::setShowLabels(bool show) {
    pImpl->showLabels = show;
}

void VisualizationInterface::setShowConnections(bool show) {
    pImpl->showConnections = show;
}

void VisualizationInterface::visualizeNetwork(const class Brain& brain, 
                                            NetworkStyle style,
                                            float nodeScale) {
    if (!pImpl->active) return;
    
    // Simulate network data extraction
    pImpl->networkData.nodeCount = brain.getTotalNeuronCount();
    pImpl->networkData.connectionCount = brain.getTotalSynapseCount();
    
    // Generate node positions based on style
    switch (style) {
        case NetworkStyle::Graph:
            generateGraphLayout(brain);
            break;
        case NetworkStyle::Circular:
            generateCircularLayout(brain);
            break;
        case NetworkStyle::Hierarchical:
            generateHierarchicalLayout(brain);
            break;
        case NetworkStyle::Planar:
            generatePlanarLayout(brain);
            break;
        case NetworkStyle::ForceDirected:
            generateForceDirectedLayout(brain);
            break;
    }
    
    pImpl->networkData.nodeColors.resize(pImpl->networkData.nodeCount);
    for (size_t i = 0; i < pImpl->networkData.nodePositions.size(); ++i) {
        // Use brain statistics to determine node color
        float firingRate = 0.1f;  // Would be obtained from brain
        pImpl->networkData.nodeColors[i] = calculateNodeColor(firingRate, nodeScale);
    }
    
    NLM_LOG_INFO("Network visualization updated with " + 
                 std::to_string(pImpl->networkData.nodeCount) + " nodes and " +
                 std::to_string(pImpl->networkData.connectionCount) + " connections");
}

void VisualizationInterface::visualizeActivity(const class Brain& brain,
                                              ActivityMode mode,
                                              float intensityScale) {
    if (!pImpl->active) return;
    
    pImpl->activityData.neuronCount = brain.getTotalNeuronCount();
    pImpl->activityData.neuronPositions.resize(pImpl->activityData.neuronCount);
    pImpl->activityData.firingRates.resize(pImpl->activityData.neuronCount);
    pImpl->activityData.activityColors.resize(pImpl->activityData.neuronCount);
    
    // Simulate activity data extraction
    for (size_t i = 0; i < pImpl->activityData.neuronCount; ++i) {
        pImpl->activityData.neuronPositions[i] = calculateNeuronPosition(i, brain);
        pImpl->activityData.firingRates[i] = simulateFiringRate(i, brain);
        
        switch (mode) {
            case ActivityMode::FiringRate:
                pImpl->activityData.activityColors[i] = 
                    glm::vec4(pImpl->activityData.firingRates[i] * intensityScale, 
                             0.0f, 0.0f, 1.0f);
                break;
            case ActivityMode::MembranePotential:
                pImpl->activityData.activityColors[i] = 
                    glm::vec4(simulateMembranePotential(i, brain) * 0.01f,
                             0.5f, 0.5f, 1.0f);
                break;
            case ActivityMode::Calcium:
                pImpl->activityData.activityColors[i] = 
                    glm::vec4(simulateCalciumLevel(i, brain) * intensityScale,
                             0.0f, 0.5f, 1.0f);
                break;
            case ActivityMode::SpikeTiming:
                pImpl->activityData.activityColors[i] = 
                    glm::vec4(0.5f, simulateSpikeTiming(i, brain), 0.0f, 1.0f);
                break;
            case ActivityMode::PopulationActivity:
                pImpl->activityData.activityColors[i] = 
                    calculatePopulationActivityColor(i, brain, intensityScale);
                break;
        }
    }
    
    NLM_LOG_INFO("Activity visualization updated for " + 
                 std::to_string(pImpl->activityData.neuronCount) + " neurons");
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes,
                                            double duration,
                                            int maxSpikes) {
    if (!pImpl->active) return;
    
    pImpl->spikeRasterData.spikeTimes.clear();
    pImpl->spikeRasterData.neuronIndices.clear();
    pImpl->spikeRasterData.spikeColors.clear();
    
    pImpl->spikeRasterData.duration = duration;
    pImpl->spikeRasterData.maxSpikes = maxSpikes;
    
    // Process spikes
    for (const auto& spike : spikes) {
        if (pImpl->spikeRasterData.spikeTimes.size() >= static_cast<size_t>(maxSpikes)) {
            break;
        }
        
        double normalizedTime = spike.timestamp / duration;
        int neuronIndex = spike.source_neuron.index() % 1000; // Normalize for display
        
        pImpl->spikeRasterData.spikeTimes.push_back(normalizedTime);
        pImpl->spikeRasterData.neuronIndices.push_back(neuronIndex);
        
        // Color based on spike properties
        glm::vec4 color(1.0f, normalizedTime, 0.5f, 1.0f);
        pImpl->spikeRasterData.spikeColors.push_back(color);
    }
    
    NLM_LOG_INFO("Spike raster plot updated with " + 
                 std::to_string(pImpl->spikeRasterData.spikeTimes.size()) + 
                 " spikes over " + std::to_string(duration) + " seconds");
}

void VisualizationInterface::visualizeWeights(const class Brain& brain,
                                            WeightMode mode,
                                            float minWeight,
                                            float maxWeight) {
    if (!pImpl->active) return;
    
    // Simulate weight matrix extraction
    pImpl->weightMatrixData.matrixSize = 100; // Square matrix for visualization
    pImpl->weightMatrixData.weights.resize(pImpl->weightMatrixData.matrixSize,
                                          std::vector<float>(pImpl->weightMatrixData.matrixSize));
    pImpl->weightMatrixData.neuronPositions.resize(pImpl->weightMatrixData.matrixSize);
    pImpl->weightMatrixData.minWeight = minWeight;
    pImpl->weightMatrixData.maxWeight = maxWeight;
    
    // Generate weight data
    for (int i = 0; i < pImpl->weightMatrixData.matrixSize; ++i) {
        pImpl->weightMatrixData.neuronPositions[i] = calculateNeuronPosition(i, brain);
        for (int j = 0; j < pImpl->weightMatrixData.matrixSize; ++j) {
            // Simulate weight based on distance and neuron types
            float distance = glm::length(pImpl->weightMatrixData.neuronPositions[i] - 
                                       pImpl->weightMatrixData.neuronPositions[j]);
            float baseWeight = 0.5f * exp(-distance * 0.1f);
            
            switch (mode) {
                case WeightMode::SignedWeights:
                    pImpl->weightMatrixData.weights[i][j] = baseWeight * (1.0f - 2.0f * (i % 2));
                    break;
                case WeightMode::AbsoluteWeights:
                    pImpl->weightMatrixData.weights[i][j] = std::abs(baseWeight);
                    break;
                case WeightMode::NormalizedWeights:
                    pImpl->weightMatrixData.weights[i][j] = baseWeight / maxWeight;
                    break;
                case WeightMode::LogTransformed:
                    pImpl->weightMatrixData.weights[i][j] = std::log(1.0f + std::abs(baseWeight));
                    break;
            }
        }
    }
    
    NLM_LOG_INFO("Weight matrix visualization updated with " +
                 std::to_string(pImpl->weightMatrixData.matrixSize) + "x" +
                 std::to_string(pImpl->weightMatrixData.matrixSize) + " matrix");
}

bool VisualizationInterface::exportVisualization(const std::string& filename, 
                                                 OutputFormat format,
                                                 int quality) {
    if (!pImpl->active) return false;
    
    // Capture current frame
    auto screenshot = captureScreenshot();
    if (screenshot.empty()) {
        std::cerr << "Failed to capture screenshot" << std::endl;
        return false;
    }
    
    // Save based on format
    bool success = false;
    switch (format) {
        case OutputFormat::PNG:
            success = savePNG(filename, screenshot);
            break;
        case OutputFormat::SVG:
            success = saveSVG(filename);
            break;
        case OutputFormat::PDF:
            success = savePDF(filename);
            break;
        case OutputFormat::Video:
            success = saveVideo(filename, quality);
            break;
        case OutputFormat::WebGL:
            success = saveWebGL(filename);
            break;
        case OutputFormat::JSON:
            success = saveJSON(filename);
            break;
    }
    
    if (success) {
        NLM_LOG_INFO("Visualization exported to " + filename);
    } else {
        NLM_LOG_ERROR("Failed to export visualization to " + filename);
    }
    
    return success;
}

std::vector<unsigned char> VisualizationInterface::captureScreenshot() {
    std::vector<unsigned char> screenshot;
    
    if (!pImpl->active) return screenshot;
    
    // In a real implementation, this would capture the current OpenGL framebuffer
    // For now, we return a placeholder
    screenshot.resize(1920 * 1080 * 4); // RGBA for 1920x1080
    std::fill(screenshot.begin(), screenshot.end(), 255);
    
    return screenshot;
}

VisualizationInterface::VisualizationStats VisualizationInterface::getStats() const {
    return pImpl->stats;
}

void VisualizationInterface::enableDebugMode(bool enable) {
    pImpl->debugMode = enable;
    if (enable) {
        pImpl->debugLevel = 1;
    }
}

void VisualizationInterface::setDebugLevel(int level) {
    pImpl->debugLevel = level;
}

void VisualizationInterface::setProgress(double progress) {
    pImpl->currentProgress = std::max(0.0, std::min(1.0, progress));
}

void VisualizationInterface::setStatus(const std::string& status) {
    pImpl->currentStatus = status;
    std::cout << "Status: " << status << std::endl;
}

void VisualizationInterface::addTimeSeriesData(const std::string& name,
                                            const std::vector<double>& data,
                                            double duration) {
    if (pImpl->timeSeries.find(name) == pImpl->timeSeries.end()) {
        TimeSeriesData ts;
        ts.name = name;
        ts.duration = duration;
        ts.color = glm::vec3(1.0f, 0.0f, 0.0f); // Default red
        ts.active = true;
        pImpl->timeSeries[name] = ts;
    }
    
    auto& ts = pImpl->timeSeries[name];
    ts.values.clear();
    for (double value : data) {
        ts.values.push_back(value);
    }
    ts.time = 0.0;
}

void VisualizationInterface::clearTimeSeries() {
    pImpl->timeSeries.clear();
}

// Helper functions
void VisualizationInterface::Impl::generateGraphLayout(const class Brain& brain) {
    networkData.nodePositions.resize(networkData.nodeCount);
    networkData.connections.resize(networkData.connectionCount);
    networkData.connectionColors.resize(networkData.connectionCount);
    networkData.connectionWeights.resize(networkData.connectionCount);
    
    // Simple spring layout
    for (size_t i = 0; i < networkData.nodePositions.size(); ++i) {
        float angle = static_cast<float>(i) / static_cast<float>(networkData.nodeCount) * 2.0f * 3.14159f;
        networkData.nodePositions[i] = glm::vec3(
            std::cos(angle) * 2.0f,
            std::sin(angle) * 2.0f,
            0.0f
        );
    }
    
    // Generate connections
    size_t connIndex = 0;
    for (size_t i = 0; i < networkData.nodeCount && connIndex < networkData.connectionCount; ++i) {
        for (size_t j = i + 1; j < networkData.nodeCount && connIndex < networkData.connectionCount; ++j) {
            networkData.connections[connIndex] = {i, j};
            networkData.connectionWeights[connIndex] = 0.5f;
            networkData.connectionColors[connIndex] = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
            connIndex++;
        }
    }
}

void VisualizationInterface::Impl::generateCircularLayout(const class Brain& brain) {
    networkData.nodePositions.resize(networkData.nodeCount);
    
    for (size_t i = 0; i < networkData.nodeCount; ++i) {
        float angle = static_cast<float>(i) / static_cast<float>(networkData.nodeCount) * 2.0f * 3.14159f;
        networkData.nodePositions[i] = glm::vec3(
            std::cos(angle) * 3.0f,
            std::sin(angle) * 3.0f,
            0.0f
        );
    }
}

void VisualizationInterface::Impl::generateHierarchicalLayout(const class Brain& brain) {
    networkData.nodePositions.resize(networkData.nodeCount);
    
    size_t layerSize = std::sqrt(networkData.nodeCount);
    for (size_t i = 0; i < networkData.nodeCount; ++i) {
        size_t layer = i / layerSize;
        size_t posInLayer = i % layerSize;
        
        float angle = static_cast<float>(posInLayer) / static_cast<float>(layerSize) * 2.0f * 3.14159f;
        networkData.nodePositions[i] = glm::vec3(
            angle * std::cos(layer * 0.5f),
            angle * std::sin(layer * 0.5f),
            layer * 1.0f
        );
    }
}

void VisualizationInterface::Impl::generatePlanarLayout(const class Brain& brain) {
    generateGraphLayout(brain);
}

void VisualizationInterface::Impl::generateForceDirectedLayout(const class Brain& brain) {
    generateGraphLayout(brain);
}

glm::vec3 VisualizationInterface::Impl::calculateNodeColor(float firingRate, float scale) {
    float intensity = std::min(1.0f, firingRate * scale);
    return glm::vec3(intensity, 1.0f - intensity, 0.5f);
}

glm::vec3 VisualizationInterface::Impl::calculateNeuronPosition(size_t index, const class Brain& brain) {
    float angle = static_cast<float>(index) / 100.0f * 2.0f * 3.14159f;
    float radius = 2.0f + (brain.getAverageFiringRate() * 5.0f);
    return glm::vec3(
        std::cos(angle) * radius,
        std::sin(angle) * radius,
        0.0f
    );
}

float VisualizationInterface::Impl::simulateFiringRate(size_t neuronIndex, const class Brain& brain) {
    return 0.1f + 0.4f * std::sin(static_cast<float>(neuronIndex) * 0.1f + 
                                 static_cast<float>(brain.getTotalSpikeCount()) * 0.01f);
}

float VisualizationInterface::Impl::simulateMembranePotential(size_t neuronIndex, const class Brain& brain) {
    return 0.5f + 0.3f * std::sin(static_cast<float>(neuronIndex) * 0.05f);
}

float VisualizationInterface::Impl::simulateCalciumLevel(size_t neuronIndex, const class Brain& brain) {
    return simulateFiringRate(neuronIndex, brain) * 0.8f;
}

float VisualizationInterface::Impl::simulateSpikeTiming(size_t neuronIndex, const class Brain& brain) {
    return static_cast<float>(neuronIndex) / static_cast<float>(brain.getTotalNeuronCount());
}

glm::vec3 VisualizationInterface::Impl::calculatePopulationActivityColor(size_t neuronIndex, 
                                                                          const class Brain& brain,
                                                                          float intensityScale) {
    float activity = simulateFiringRate(neuronIndex, brain) * intensityScale;
    float hue = activity * 240.0f / 360.0f; // Red to blue gradient
    return glm::vec3(hue / 360.0f, 0.8f, 0.9f);
}

bool VisualizationInterface::savePNG(const std::string& filename, 
                                    const std::vector<unsigned char>& data) {
    // Simple PNG save implementation (would use a proper library like stb_image_write)
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    
    // Write minimal PNG header
    file << "\x89PNG\r\n\x1a\n";
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

bool VisualizationInterface::saveSVG(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return false;
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1920\" height=\"1080\">\n";
    file << "  <!-- SVG visualization would go here -->\n";
    file << "</svg>\n";
    
    return true;
}

bool VisualizationInterface::savePDF(const std::string& filename) {
    // Would use a PDF library
    std::cerr << "PDF export not yet implemented" << std::endl;
    return false;
}

bool VisualizationInterface::saveVideo(const std::string& filename, int quality) {
    // Would use a video encoding library
    std::cerr << "Video export not yet implemented" << std::endl;
    return false;
}

bool VisualizationInterface::saveWebGL(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return false;
    
    file << "// WebGL visualization code would go here\n";
    file << "const visualizationData = {
        // Neural network data
        nodes: [],
        connections: [],
        // Activity data
        firingRates: [],
        membranePotentials: [],
        // Spike data
        spikes: [],
        // Weight matrix
        weights: []
    };\n";
    
    return true;
}

bool VisualizationInterface::saveJSON(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return false;
    
    file << "{\n";
    file << "  \"metadata\": {
    \"timestamp\": \"" << std::chrono::system_clock::now().time_since_epoch().count() << "\",
    \"version\": \"1.0.0\"
  },\n";
    file << "  \"networkData\": " << std::endl;
    file << "    // Network data would be serialized here\n";
    file << "  ,\"activityData\": " << std::endl;
    file << "    // Activity data would be serialized here\n";
    file << "  ,\"spikeRasterData\": " << std::endl;
    file << "    // Spike raster data would be serialized here\n";
    file << "  ,\"weightMatrixData\": " << std::endl;
    file << "    // Weight matrix data would be serialized here\n";
    file << "}\n";
    
    return true;
}

} // namespace nlm
