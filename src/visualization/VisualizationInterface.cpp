#include "VisualizationInterface.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../experiments/Experiment.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    // OpenGL/GLFW resources
    GLFWwindow* window;
    unsigned int shaderProgram;
    unsigned int vertexArrayObject;
    unsigned int vertexBufferObject;
    
    // Visualization state
    std::vector<NeuronId> spikeTimes;  // For raster plot
    std::vector<std::pair<NeuronId, NeuronId>> connections;  // For network visualization
    std::vector<float> weights;  // For weight matrix
    std::vector<std::pair<NeuronId, float>> neuralActivity;  // For activity visualization
    
    Impl() : active(false), updateRate(30.0), window(nullptr), 
             shaderProgram(0), vertexArrayObject(0), vertexBufferObject(0) {}
    
    ~Impl() {
        if (window) glfwDestroyWindow(window);
        glfwTerminate();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

// OpenGL shaders for rendering
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main() {
        FragColor = vec4(color, 1.0f);
    }
)";

// Helper function to compile shaders
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        // Log compilation error
    }
    return shader;
}

// Helper function to create shader program
unsigned int createShaderProgram() {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

// Helper function to generate network visualization data
void generateNetworkData(const Brain& brain, std::vector<NeuronId>& spikeTimesOut,
                         std::vector<std::pair<NeuronId, NeuronId>>& connectionsOut,
                         std::vector<float>& weightsOut,
                         std::vector<std::pair<NeuronId, float>>& neuralActivityOut) {
    spikeTimesOut.clear();
    connectionsOut.clear();
    weightsOut.clear();
    neuralActivityOut.clear();
    
    // Collect spike times from brain's spike history
    // This would require access to brain's internal spike history
    // For now, we'll generate synthetic data
    spikeTimesOut.push_back(NeuronId(1)); spikeTimesOut.push_back(NeuronId(2));
    
    // Collect neural connections and weights
    connectionsOut.push_back(std::make_pair(NeuronId(1), NeuronId(2)));
    connectionsOut.push_back(std::make_pair(NeuronId(2), NeuronId(3)));
    connectionsOut.push_back(std::make_pair(NeuronId(3), NeuronId(1)));
    
    weightsOut.push_back(0.8f);
    weightsOut.push_back(0.5f);
    weightsOut.push_back(0.3f);
    
    // Collect neural activity levels
    for (size_t i = 0; i < 10; ++i) {
        neuralActivityOut.push_back(std::make_pair(NeuronId(i + 1), 0.5f + 0.5f * std::sin(i * 0.5)));
    }
}

bool VisualizationInterface::initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window = glfwCreateWindow(800, 600, "Neural Network Visualization", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    // Initialize OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    
    // Create shader program
    pImpl->shaderProgram = createShaderProgram();
    
    // Generate vertex data for basic rendering
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
    glGenVertexArrays(1, &pImpl->vertexArrayObject);
    glGenBuffers(1, &pImpl->vertexBufferObject);
    
    glBindVertexArray(pImpl->vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, pImpl->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::render() {
    if (!pImpl->active || !pImpl->window) return;
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Update visualization data
    static_cast<void>(currentTime);
    static_cast<void>(currentTime);
    
    // Draw a simple triangle (placeholder for actual visualizations)
    glUseProgram(pImpl->shaderProgram);
    
    mat4 model = mat4(1.0f);
    model = translate(model, vec3(0.0f, 0.0f, 0.0f));
    model = scale(model, vec3(0.5f, 0.5f, 0.5f));
    
    GLint modelLoc = glGetUniformLocation(pImpl->shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(pImpl->shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(pImpl->shaderProgram, "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    
    mat4 view = mat4(1.0f);
    mat4 projection = mat4(1.0f);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    
    GLint colorLoc = glGetUniformLocation(pImpl->shaderProgram, "color");
    glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // Green color
    
    glBindVertexArray(pImpl->vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glfwSwapBuffers(pImpl->window);
    glfwPollEvents();
}

void VisualizationInterface::close() {
    pImpl->active = false;
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Generate network visualization data
    generateNetworkData(brain, pImpl->spikeTimes, pImpl->connections, 
                       pImpl->weights, pImpl->neuralActivity);
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Generate activity visualization data
    pImpl->neuralActivity.clear();
    
    // Collect neural activity from brain's neurons
    // This would require accessing brain's neuron data
    // For now, we'll generate sample activity data
    for (size_t i = 0; i < 20; ++i) {
        float activity = 0.3f + 0.7f * std::sin(static_cast<float>(i) * 0.3f);
        pImpl->neuralActivity.push_back(std::make_pair(NeuronId(i + 1), activity));
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Generate spike raster plot data
    pImpl->spikeTimes.clear();
    
    // Extract neuron IDs from spikes
    for (const auto& spike : spikes) {
        pImpl->spikeTimes.push_back(spike.source_neuron);
    }
    
    // If no spikes provided, generate synthetic data for demonstration
    if (spikes.empty()) {
        for (int i = 0; i < 50; ++i) {
            pImpl->spikeTimes.push_back(NeuronId(i % 10 + 1));
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Generate weight matrix visualization data
    pImpl->weights.clear();
    pImpl->connections.clear();
    
    // Extract synaptic weights from brain's connections
    // This would require accessing brain's synaptic data
    // For now, we'll generate sample weight data
    std::vector<std::pair<NeuronId, NeuronId>> sampleConnections = {
        {NeuronId(1), NeuronId(2)}, {NeuronId(2), NeuronId(3)}, 
        {NeuronId(3), NeuronId(4)}, {NeuronId(4), NeuronId(5)},
        {NeuronId(5), NeuronId(1)}, {NeuronId(2), NeuronId(4)},
        {NeuronId(3), NeuronId(5)}, {NeuronId(4), NeuronId(1)},
        {NeuronId(5), NeuronId(2)}
    };
    
    for (const auto& conn : sampleConnections) {
        pImpl->connections.push_back(conn);
        float weight = 0.1f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.9f;
        pImpl->weights.push_back(weight);
    }
}

} // namespace nlm
