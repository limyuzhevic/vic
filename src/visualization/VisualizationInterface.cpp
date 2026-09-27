#include "VisualizationInterface.hpp"
#include "Types.hpp"
#include "Brain.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    // OpenGL/GLFW context
    GLFWwindow* window;
    int width;
    int height;
    bool initialized;
    
    // Visualization configuration
    bool showNetwork;
    bool showActivity;
    bool showSpikes;
    bool showWeights;
    int currentView;
    float nodeSize;
    float connectionWidth;
    
    // Visualization data
    std::vector<SpikeEvent> spikeBuffer;
    size_t spikeBufferSize;
    double lastSpikeTime;
    
    // OpenGL shaders and resources (simplified for now)
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;
    
    // Colors for different neuron types
    glm::vec3 excitatoryColor;
    glm::vec3 inhibitoryColor;
    glm::vec3 sensoryColor;
    glm::vec3 motorColor;
    
    Impl() : active(false), updateRate(30.0), 
             window(nullptr), width(1280), height(720),
             initialized(false), showNetwork(true), showActivity(true),
             showSpikes(true), showWeights(true), currentView(0),
             nodeSize(5.0f), connectionWidth(1.0f),
             spikeBufferSize(10000), lastSpikeTime(0.0),
             VAO(0), VBO(0), EBO(0), shaderProgram(0) {
        
        excitatoryColor = glm::vec3(0.2f, 0.8f, 0.2f);
        inhibitoryColor = glm::vec3(0.8f, 0.2f, 0.2f);
        sensoryColor = glm::vec3(0.2f, 0.2f, 0.8f);
        motorColor = glm::vec3(0.8f, 0.8f, 0.2f);
    }
};

// Simple shader implementation
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec3 vertexColor;
    
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vertexColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(vertexColor, 1.0f);
    }
)";

// Helper function to compile and link shaders
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    
    return shader;
}

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    // Create window
    pImpl->window = glfwCreateWindow(
        pImpl->width, pImpl->height, 
        "NLM Brain Visualization", nullptr, nullptr
    );
    
    if (!pImpl->window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(pImpl->window);
    glfwSetFramebufferSizeCallback(pImpl->window, [](GLFWwindow* win, int w, int h) {
        auto impl = reinterpret_cast<VisualizationInterface::Impl*>(glfwGetWindowUserPointer(win));
        if (impl) {
            impl->width = w;
            impl->height = h;
            glViewport(0, 0, w, h);
        }
    });
    
    // Set user pointer
    glfwSetWindowUserPointer(pImpl->window, pImpl.get());
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(pImpl->window);
        glfwTerminate();
        return false;
    }
    
    // Setup OpenGL state
    glViewport(0, 0, pImpl->width, pImpl->height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // Create shaders
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    pImpl->shaderProgram = glCreateProgram();
    glAttachShader(pImpl->shaderProgram, vertexShader);
    glAttachShader(pImpl->shaderProgram, fragmentShader);
    glLinkProgram(pImpl->shaderProgram);
    
    int success;
    char infoLog[512];
    glGetProgramiv(pImpl->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(pImpl->shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Initialize VAO/VBO for rendering
    glGenVertexArrays(1, &pImpl->VAO);
    glGenBuffers(1, &pImpl->VBO);
    glGenBuffers(1, &pImpl->EBO);
    
    pImpl->active = true;
    pImpl->initialized = true;
    
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active || !pImpl->initialized) return;
    
    // Update spike buffer (remove old spikes)
    double currentTime = glfwGetTime();
    if (currentTime - pImpl->lastSpikeTime > 0.1) { // Update every 100ms
        // Simple circular buffer management
        if (pImpl->spikeBuffer.size() > pImpl->spikeBufferSize) {
            pImpl->spikeBuffer.erase(pImpl->spikeBuffer.begin(), 
                                   pImpl->spikeBuffer.begin() + 100);
        }
        pImpl->lastSpikeTime = currentTime;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active || !pImpl->initialized || !pImpl->window) return;
    
    // Clear screen
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(pImpl->shaderProgram);
    
    // Setup view and projection matrices
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        (float)pImpl->width / (float)pImpl->height, 
        0.1f, 100.0f
    );
    
    glUniformMatrix4fv(glGetUniformLocation(pImpl->shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(pImpl->shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Render different views based on currentView
    switch (pImpl->currentView) {
        case 0: // Network view
            visualizeNetworkGL();
            break;
        case 1: // Activity view
            visualizeActivityGL();
            break;
        case 2: // Spikes view
            visualizeSpikesGL();
            break;
        case 3: // Weights view
            visualizeWeightsGL();
            break;
    }
    
    glfwSwapBuffers(pImpl->window);
    glfwPollEvents();
}

void VisualizationInterface::close() {
    if (pImpl->initialized) {
        if (pImpl->VAO) glDeleteVertexArrays(1, &pImpl->VAO);
        if (pImpl->VBO) glDeleteBuffers(1, &pImpl->VBO);
        if (pImpl->EBO) glDeleteBuffers(1, &pImpl->EBO);
        if (pImpl->shaderProgram) glDeleteProgram(pImpl->shaderProgram);
        if (pImpl->window) glfwDestroyWindow(pImpl->window);
        glfwTerminate();
        
        pImpl->initialized = false;
        pImpl->active = false;
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active && pImpl->initialized;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

// Visualization implementation methods
void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Store spike data for rendering
    auto spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        const auto& spikeHistory = spikeSystem->getSpikeHistory();
        pImpl->spikeBuffer.clear();
        
        // Convert DetailedSpikeEvent to SpikeEvent for simplicity
        for (const auto& detailedSpike : spikeHistory) {
            SpikeEvent spike;
            spike.source_neuron = detailedSpike.sourceNeuron;
            spike.timestamp = detailedSpike.timestamp;
            spike.step = detailedSpike.step;
            pImpl->spikeBuffer.push_back(spike);
        }
    }
    
    // In real implementation, this would prepare OpenGL buffers for network rendering
    // For now, we just store the data
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Get neuron activity data
    auto regions = brain.getRegions();
    
    // In real implementation, this would prepare OpenGL buffers for activity visualization
    // Would render neurons with colors based on firing rates
    
    // For now, just store some basic activity data
    pImpl->active = true;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    pImpl->spikeBuffer = spikes;
    
    // In real implementation, this would prepare OpenGL buffers for spike raster plot
    // Would create points colored by time and neuron type
    
    // For now, just store the spike data
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // In real implementation, this would prepare OpenGL buffers for weight matrix visualization
    // Would create a 2D representation of synaptic weights
    
    // For now, just mark that weights should be visualized
    // In a real implementation, this would read the weight matrices from brain regions
}

// OpenGL rendering implementations
void VisualizationInterface::Impl::prepareNetworkBuffers(const Brain& brain) {
    // This would prepare OpenGL buffers for network rendering
    // Create vertices for neurons and edges between them
}

void VisualizationInterface::visualizeNetworkGL() {
    if (!pImpl->showNetwork) return;
    
    // Render neurons and connections
    // This would use OpenGL to draw spheres for neurons and lines for connections
    // For simplicity, we'll just demonstrate the structure
}

void VisualizationInterface::visualizeActivityGL() {
    if (!pImpl->showActivity) return;
    
    // Render neuron activity as colored spheres
    // Color intensity based on firing rate
}

void VisualizationInterface::visualizeSpikesGL() {
    if (!pImpl->showSpikes) return;
    
    // Render spike raster as points
    // Points arranged in time/rank space
}

void VisualizationInterface::visualizeWeightsGL() {
    if (!pImpl->showWeights) return;
    
    // Render weight matrix as heatmap
    // Color intensity based on synaptic weight values
}

} // namespace nlm
