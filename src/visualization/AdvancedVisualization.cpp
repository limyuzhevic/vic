#include "AdvancedVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <mutex>

using json = nlohmann::json;

namespace nlm {

// Neural Network Renderer Implementation
struct NeuralNetworkRenderer::Impl {
    GLFWwindow* window = nullptr;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint shaderProgram = 0;
    GLuint vao = 0, vbo = 0, ebo = 0;
    
    std::string visualizationMode = "default";
    bool showNeurons = true;
    bool showConnections = true;
    bool showSpikes = true;
    bool weightedConnections = true;
    std::string colorScale = "fire"; // fire, cool, viridis, gray
    
    std::string layoutAlgorithm = "force_directed";
    double cameraZoom = 1.0;
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    
    // Neuron and connection data
    struct NeuronData {
        NeuronId id;
        glm::vec3 position;
        glm::vec3 color;
        float radius;
        float activity;
        NeuronType type;
    };
    
    std::vector<NeuronData> neurons;
    std::vector<std::pair<NeuronId, NeuronId>> connections;
    std::vector<float> connectionWeights;
    std::vector<glm::vec3> connectionColors;
    
    // Interaction state
    bool mousePressed = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    std::mutex dataMutex;
    
    // Statistics
    size_t totalSpikes = 0;
    double lastUpdateTime = 0.0;
    int frameCount = 0;
    
    Impl() = default;
    
    ~Impl() {
        cleanupShaders();
        cleanupBuffers();
    }
    
    void cleanupShaders() {
        if (shaderProgram) glDeleteProgram(shaderProgram);
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        shaderProgram = vertexShader = fragmentShader = 0;
    }
    
    void cleanupBuffers() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ebo) glDeleteBuffers(1, &ebo);
        vao = vbo = ebo = 0;
    }
    
    void setupShaders() {
        const char* vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            out vec3 fragmentColor;
            
            void main() {
                gl_Position = projection * view * model * vec4(aPos, 1.0);
                fragmentColor = aColor;
            }
        )";
        
        const char* fragmentShaderSource = R"(
            #version 330 core
            in vec3 fragmentColor;
            out vec4 FragColor;
            
            void main() {
                FragColor = vec4(fragmentColor, 1.0);
            }
        )";
        
        // Compile vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        
        // Compile fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        
        // Link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
    }
    
    void updateBuffers() {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        cleanupBuffers();
        
        // Calculate total vertices needed
        size_t neuronVertices = neurons.size() * 36; // 36 vertices per sphere
        size_t connectionVertices = connections.size() * 6; // 6 vertices per line
        size_t totalVertices = neuronVertices + connectionVertices;
        
        if (totalVertices == 0) return;
        
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;
        std::vector<unsigned int> indices;
        
        // Add neuron spheres
        size_t offset = 0;
        for (const auto& neuron : neurons) {
            // Add sphere vertices (simplified - for now just points)
            positions.push_back(neuron.position);
            colors.push_back(neuron.color);
            indices.push_back(offset++);
        }
        
        // Add connection lines
        for (size_t i = 0; i < connections.size(); ++i) {
            const auto& conn = connections[i];
            // Find neuron positions
            glm::vec3 pos1 = glm::vec3(0.0f), pos2 = glm::vec3(0.0f);
            for (const auto& neuron : neurons) {
                if (neuron.id == conn.first) pos1 = neuron.position;
                if (neuron.id == conn.second) pos2 = neuron.position;
            }
            
            positions.push_back(pos1);
            positions.push_back(pos2);
            colors.push_back(connectionColors[i]);
            colors.push_back(connectionColors[i]);
            
            size_t base = neurons.size() + i * 2;
            indices.push_back(base);
            indices.push_back(base + 1);
        }
        
        // Create VAO, VBO, EBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 
                    (positions.size() * sizeof(glm::vec3) + colors.size() * sizeof(glm::vec3)),
                    nullptr, GL_STATIC_DRAW);
        
        // Upload position and color data interleaved
        std::vector<glm::vec3> interleaved;
        interleaved.reserve(positions.size() + colors.size());
        for (size_t i = 0; i < positions.size(); ++i) {
            interleaved.push_back(positions[i]);
            if (i < colors.size()) interleaved.push_back(colors[i]);
        }
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved.size() * sizeof(glm::vec3), interleaved.data());
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                    indices.data(), GL_STATIC_DRAW);
        
        // Set vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(glm::vec3), (void*)(3 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);
    }
    
    void computeNeuronPositions(const Brain& brain) {
        std::lock_guard<std::mutex> lock(dataMutex);
        neurons.clear();
        
        // Simple 3D layout based on region and type
        size_t neuronCount = brain.getTotalNeuronCount();
        if (neuronCount == 0) return;
        
        size_t idx = 0;
        for (const auto& region : brain.getRegions()) {
            auto neuronsInRegion = region->getAllNeurons();
            for (auto* neuron : neuronsInRegion) {
                // Calculate 3D position
                float x = static_cast<float>((idx % 20) * 0.5 - 5.0); // Spread across x-axis
                float y = static_cast<float>((idx / 20) * 0.3 - 1.5); // Spread across y-axis
                float z = static_cast<float>(region->getId().index() * 2); // Different z for regions
                
                // Determine color based on neuron type
                glm::vec3 color;
                switch (neuron->getType()) {
                    case NeuronType::Sensory: color = glm::vec3(0.2f, 0.8f, 0.2f); break; // Green
                    case NeuronType::Internal: color = glm::vec3(0.8f, 0.2f, 0.8f); break; // Purple
                    case NeuronType::Motor: color = glm::vec3(0.2f, 0.2f, 0.8f); break; // Blue
                    default: color = glm::vec3(0.5f, 0.5f, 0.5f); break; // Gray
                }
                
                NeuronData data;
                data.id = neuron->getId();
                data.position = glm::vec3(x, y, z);
                data.color = color;
                data.radius = 0.1f;
                data.activity = neuron->isFiring() ? 1.0f : 0.0f;
                data.type = neuron->getType();
                
                neurons.push_back(data);
                idx++;
            }
        }
        
        // Populate connections
        connections.clear();
        connectionWeights.clear();
        connectionColors.clear();
        
        for (const auto& region : brain.getRegions()) {
            auto syn = region->getSynapses();
            for (auto* s : syn) {
                connections.push_back({s->getSourceNeuron(), s->getDestinationNeuron()});
                connectionWeights.push_back(s->getWeight());
                
                // Color based on weight
                float weight = s->getWeight();
                glm::vec3 connColor;
                if (colorScale == "fire") {
                    connColor = glm::vec3(weight, 0.0f, 0.0f); // Red scale
                } else if (colorScale == "cool") {
                    connColor = glm::vec3(0.0f, 0.0f, weight); // Blue scale
                } else if (colorScale == "viridis") {
                    float v = std::clamp(weight * 2.0f, 0.0f, 1.0f);
                    connColor = glm::vec3(v * 0.14f, v * 0.05f, v * 0.5f);
                } else {
                    connColor = glm::vec3(weight, weight, weight); // Gray scale
                }
                
                connectionColors.push_back(connColor);
            }
        }
        
        updateBuffers();
    }
};

NeuralNetworkRenderer::NeuralNetworkRenderer() : pImpl(std::make_unique<Impl>()) {}
NeuralNetworkRenderer::~NeuralNetworkRenderer() = default;

bool NeuralNetworkRenderer::initialize(GLFWwindow* window) {
    pImpl->window = window;
    
    // Initialize OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        NLM_LOG_ERROR("Failed to initialize OpenGL loader");
        return false;
    }
    
    // Setup shaders
    pImpl->setupShaders();
    
    NLM_LOG_INFO("3D Neural Network Renderer initialized successfully");
    return true;
}

void NeuralNetworkRenderer::update(const Brain& brain, double time) {
    pImpl->computeNeuronPositions(brain);
    pImpl->totalSpikes = brain.getTotalSpikeCount();
    pImpl->lastUpdateTime = time;
}

void NeuralNetworkRenderer::render() {
    if (!pImpl->vao) return;
    
    // Set up model matrix
    glm::mat4 model = glm::mat4(1.0f);
    
    // Set up view and projection matrices
    glm::mat4 view = glm::lookAt(pImpl->cameraPosition, pImpl->cameraTarget, glm::vec3(0, 1, 0));
    float aspectRatio = 16.0f / 9.0f;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    
    // Use shader program
    glUseProgram(pImpl->shaderProgram);
    
    // Set uniforms
    GLint modelLoc = glGetUniformLocation(pImpl->shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(pImpl->shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(pImpl->shaderProgram, "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Render neurons
    if (pImpl->showNeurons) {
        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pImpl->neurons.size()));
    }
    
    // Render connections
    if (pImpl->showConnections && !pImpl->connections.empty()) {
        glLineWidth(2.0f);
        glDrawElements(GL_LINES, static_cast<GLsizei>(pImpl->connections.size() * 2), 
                      GL_UNSIGNED_INT, nullptr);
    }
}

void NeuralNetworkRenderer::setVisualizationMode(const std::string& mode) {
    pImpl->visualizationMode = mode;
}

void NeuralNetworkRenderer::setShowNeurons(bool show) {
    pImpl->showNeurons = show;
}

void NeuralNetworkRenderer::setShowConnections(bool show) {
    pImpl->showConnections = show;
}

void NeuralNetworkRenderer::setShowSpikes(bool show) {
    pImpl->showSpikes = show;
}

void NeuralNetworkRenderer::setConnectionStrength(bool weighted) {
    pImpl->weightedConnections = weighted;
}

void NeuralNetworkRenderer::setColorScale(const std::string& scale) {
    pImpl->colorScale = scale;
}

void NeuralNetworkRenderer::setLayoutAlgorithm(const std::string& algorithm) {
    pImpl->layoutAlgorithm = algorithm;
    // TODO: Implement layout algorithms
}

void NeuralNetworkRenderer::forceDirectedLayout(const Brain& brain) {
    pImpl->layoutAlgorithm = "force_directed";
    // TODO: Implement force-directed layout
}

void NeuralNetworkRenderer::radialLayout(const Brain& brain) {
    pImpl->layoutAlgorithm = "radial";
    // TODO: Implement radial layout
}

void NeuralNetworkRenderer::hierarchicalLayout(const Brain& brain) {
    pImpl->layoutAlgorithm = "hierarchical";
    // TODO: Implement hierarchical layout
}

void NeuralNetworkRenderer::onMouseClick(double x, double y) {
    pImpl->mousePressed = true;
    pImpl->lastMouseX = x;
    pImpl->lastMouseY = y;
}

void NeuralNetworkRenderer::onMouseScroll(double xoffset, double yoffset) {
    pImpl->cameraZoom += yoffset * 0.1f;
    pImpl->cameraZoom = std::max(0.1f, std::min(pImpl->cameraZoom, 5.0f));
}

void NeuralNetworkRenderer::onKeyPress(int key, int scancode, int action) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_W: pImpl->cameraPosition.z += 0.1f; break;
            case GLFW_KEY_S: pImpl->cameraPosition.z -= 0.1f; break;
            case GLFW_KEY_A: pImpl->cameraPosition.x -= 0.1f; break;
            case GLFW_KEY_D: pImpl->cameraPosition.x += 0.1f; break;
            case GLFW_KEY_R: pImpl->setLayoutAlgorithm("force_directed"); break;
            case GLFW_KEY_T: pImpl->setLayoutAlgorithm("radial"); break;
            case GLFW_KEY_Y: pImpl->setLayoutAlgorithm("hierarchical"); break;
        }
    }
}

std::string NeuralNetworkRenderer::getStatistics() const {
    std::stringstream ss;
    ss << "Neural Network Renderer Statistics:" << std::endl;
    ss << "  Neurons: " << pImpl->neurons.size() << std::endl;
    ss << "  Connections: " << pImpl->connections.size() << std::endl;
    ss << "  Total Spikes: " << pImpl->totalSpikes << std::endl;
    ss << "  Visualization Mode: " << pImpl->visualizationMode << std::endl;
    ss << "  Layout Algorithm: " << pImpl->layoutAlgorithm << std::endl;
    ss << "  Camera Zoom: " << pImpl->cameraZoom << std::endl;
    return ss.str();
}

bool NeuralNetworkRenderer::exportToFile(const std::string& filepath, const std::string& format) {
    // Export data as JSON or other format
    json data;
    
    data["neurons"] = json::array();
    for (const auto& neuron : pImpl->neurons) {
        json n;
        n["id"] = neuron.id.index();
        n["position"] = {neuron.position.x, neuron.position.y, neuron.position.z};
        n["color"] = {neuron.color.r, neuron.color.g, neuron.color.b};
        n["radius"] = neuron.radius;
        n["activity"] = neuron.activity;
        data["neurons"].push_back(n);
    }
    
    data["connections"] = json::array();
    for (size_t i = 0; i < pImpl->connections.size(); ++i) {
        json c;
        c["source"] = pImpl->connections[i].first.index();
        c["target"] = pImpl->connections[i].second.index();
        c["weight"] = pImpl->connectionWeights[i];
        c["color"] = {pImpl->connectionColors[i].r, pImpl->connectionColors[i].g, 
                      pImpl->connectionColors[i].b};
        data["connections"].push_back(c);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filepath);
        return false;
    }
    
    if (format == "json") {
        file << data.dump(2);
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + format);
        return false;
    }
    
    file.close();
    NLM_LOG_INFO("Visualization exported to: " + filepath);
    return true;
}

std::string NeuralNetworkRenderer::getDataJSON() const {
    json data;
    
    data["type"] = "neural_network";
    data["neurons"] = neurons.size();
    data["connections"] = connections.size();
    data["spikeCount"] = totalSpikes;
    data["layoutAlgorithm"] = layoutAlgorithm;
    data["visualizationMode"] = visualizationMode;
    
    return data.dump();
}

// NeuralTimeSeries Implementation
struct NeuralTimeSeries::Impl {
    double timeWindow = 10.0; // seconds
    double binSize = 0.01;    // seconds
    std::string colorScale = "heat";
    bool showConfidenceIntervals = false;
    
    struct TimeSeriesData {
        double time;
        float firingRate;
        float spikeCount;
        float activity;
        std::vector<float> neuronActivations;
    };
    
    std::vector<TimeSeriesData> timeSeries;
    std::mutex dataMutex;
    
    // Time-series metrics
    struct Metrics {
        float meanFiringRate = 0.0f;
        float stdDevFiringRate = 0.0f;
        float peakFiringRate = 0.0f;
        float learningProgress = 0.0f;
        float predictionAccuracy = 0.0f;
    } metrics;
    
    Impl() = default;
};

NeuralTimeSeries::NeuralTimeSeries() : pImpl(std::make_unique<Impl>()) {}
NeuralTimeSeries::~NeuralTimeSeries() = default;

bool NeuralTimeSeries::initialize() {
    timeSeries.reserve(10000); // Pre-allocate for performance
    NLM_LOG_INFO("Neural Time-Series Visualization initialized successfully");
    return true;
}

void NeuralTimeSeries::update(const std::vector<std::pair<NeuronId, Timestamp>>& spikes, double time) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Calculate firing rate over time window
    size_t spikesInWindow = 0;
    double windowStart = time - pImpl->timeWindow;
    
    for (const auto& spike : spikes) {
        if (spike.second >= windowStart) {
            spikesInWindow++;
        }
    }
    
    float firingRate = static_cast<float>(spikesInWindow / pImpl->timeWindow);
    
    TimeSeriesData data;
    data.time = time;
    data.firingRate = firingRate;
    data.spikeCount = static_cast<float>(spikesInWindow);
    data.activity = firingRate / 50.0f; // Normalize
    data.neuronActivations = std::vector<float>(100, 0.0f); // Placeholder
    
    timeSeries.push_back(data);
    
    // Keep only recent data
    if (timeSeries.size() > 10000) {
        timeSeries.erase(timeSeries.begin(), timeSeries.begin() + 1000);
    }
    
    // Update metrics
    if (!timeSeries.empty()) {
        float sum = 0.0f, sumSq = 0.0f, max = 0.0f;
        for (const auto& d : timeSeries) {
            sum += d.firingRate;
            sumSq += d.firingRate * d.firingRate;
            max = std::max(max, d.firingRate);
        }
        
        pImpl->metrics.meanFiringRate = sum / timeSeries.size();
        pImpl->metrics.stdDevFiringRate = std::sqrt(sumSq / timeSeries.size() - 
                                                   pImpl->metrics.meanFiringRate * 
                                                   pImpl->metrics.meanFiringRate);
        pImpl->metrics.peakFiringRate = max;
    }
}

void NeuralTimeSeries::renderSpikingActivity() {
    // Render spike activity as time-series plot
    // This would typically use a plotting library like ImPlot or custom OpenGL rendering
    NLM_LOG_INFO("Rendering spiking activity time-series");
}

void NeuralTimeSeries::renderHeatMap(const std::string& metric) {
    // Render heat map of neural activity
    // This would show activity intensity across time
    NLM_LOG_INFO("Rendering heat map for metric: " + metric);
}

void NeuralTimeSeries::renderLearningProgress() {
    // Render learning progress with confidence intervals
    NLM_LOG_INFO("Rendering learning progress visualization");
}

void NeuralTimeSeries::renderPredictionAccuracy() {
    // Render prediction accuracy with error bars
    NLM_LOG_INFO("Rendering prediction accuracy visualization");
}

void NeuralTimeSeries::renderNeuromodulatorConcentration() {
    // Render neuromodulator concentration over time
    NLM_LOG_INFO("Rendering neuromodulator concentration visualization");
}

void NeuralTimeSeries::setTimeWindow(double window) {
    pImpl->timeWindow = window;
}

void NeuralTimeSeries::setBinSize(double binSize) {
    pImpl->binSize = binSize;
}

void NeuralTimeSeries::setColorScale(const std::string& scale) {
    pImpl->colorScale = scale;
}

void NeuralTimeSeries::enableConfidenceIntervals(bool enable) {
    pImpl->showConfidenceIntervals = enable;
}

std::string NeuralTimeSeries::getSpikeStatistics() const {
    std::stringstream ss;
    ss << "Spike Statistics:" << std::endl;
    ss << "  Total Time Series Points: " << pImpl->timeSeries.size() << std::endl;
    ss << "  Mean Firing Rate: " << pImpl->metrics.meanFiringRate << " Hz" << std::endl;
    ss << "  Std Dev Firing Rate: " << pImpl->metrics.stdDevFiringRate << " Hz" << std::endl;
    ss << "  Peak Firing Rate: " << pImpl->metrics.peakFiringRate << " Hz" << std::endl;
    ss << "  Time Window: " << pImpl->timeWindow << " seconds" << std::endl;
    return ss.str();
}

std::string NeuralTimeSeries::getLearningStatistics() const {
    std::stringstream ss;
    ss << "Learning Statistics:" << std::endl;
    ss << "  Learning Progress: " << pImpl->metrics.learningProgress << std::endl;
    ss << "  Prediction Accuracy: " << pImpl->metrics.predictionAccuracy << std::endl;
    return ss.str();
}

bool NeuralTimeSeries::exportRasterPlot(const std::string& filepath, int width, int height) {
    // Export raster plot as PNG or other image format
    NLM_LOG_INFO("Exporting raster plot to: " + filepath);
    // TODO: Implement actual raster plot export
    return false;
}

std::string NeuralTimeSeries::getDataCSV() const {
    std::stringstream csv;
    csv << "Time,FiringRate,SpikeCount,Activity\n";
    
    for (const auto& data : pImpl->timeSeries) {
        csv << data.time << "," << data.firingRate << "," 
            << data.spikeCount << "," << data.activity << "\n";
    }
    
    return csv.str();
}

// VisualizationWebSocketServer Implementation
struct VisualizationWebSocketServer::Impl {
    std::thread serverThread;
    std::atomic<bool> running = false;
    int port = 8080;
    
    // Client connections
    struct Client {
        int id;
        std::string address;
        // TODO: Add WebSocket handle
    };
    
    std::vector<Client> clients;
    std::mutex clientsMutex;
    
    // Message queue
    std::queue<std::string> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    
    Impl() = default;
};

VisualizationWebSocketServer::VisualizationWebSocketServer() : pImpl(std::make_unique<Impl>()) {}
VisualizationWebSocketServer::~VisualizationWebSocketServer() {
    stop();
}

bool VisualizationWebSocketServer::start(int port) {
    pImpl->port = port;
    pImpl->running = true;
    
    // Start server thread
    pImpl->serverThread = std::thread(&VisualizationWebSocketServer::runServer, this);
    
    NLM_LOG_INFO("WebSocket server started on port " + std::to_string(port));
    return true;
}

void VisualizationWebSocketServer::stop() {
    if (pImpl->running) {
        pImpl->running = false;
        pImpl->queueCV.notify_all();
        
        if (pImpl->serverThread.joinable()) {
            pImpl->serverThread.join();
        }
        
        NLM_LOG_INFO("WebSocket server stopped");
    }
}

void VisualizationWebSocketServer::broadcastBrainState(const Brain& brain, double time) {
    // TODO: Create JSON representation of brain state
    std::string data = "{\"type\": \"brain_state\", \"time\": " + 
                      std::to_string(time) + ", \"spikeCount\": " + 
                      std::to_string(brain.getTotalSpikeCount()) + "}";
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->messageQueue.push(data);
    pImpl->queueCV.notify_all();
}

void VisualizationWebSocketServer::sendVisualizationData(const std::string& dataType, 
                                                         const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->messageQueue.push("{\"type\": \"" + dataType + "\", \"data\": " + data + "}");
    pImpl->queueCV.notify_all();
}

void VisualizationWebSocketServer::onClientConnect(int clientId) {
    NLM_LOG_INFO("Client connected: ID=" + std::to_string(clientId));
}

void VisualizationWebSocketServer::onClientDisconnect(int clientId) {
    NLM_LOG_INFO("Client disconnected: ID=" + std::to_string(clientId));
}

void VisualizationWebSocketServer::onMessageReceived(int clientId, const std::string& message) {
    // Handle incoming WebSocket messages
    NLM_LOG_INFO("Message from client " + std::to_string(clientId) + ": " + message);
}

bool VisualizationWebSocketServer::isRunning() const {
    return pImpl->running;
}

std::string VisualizationWebSocketServer::getServerStatus() const {
    std::stringstream ss;
    ss << "WebSocket Server Status:" << std::endl;
    ss << "  Running: " << (pImpl->running ? "Yes" : "No") << std::endl;
    ss << "  Port: " << pImpl->port << std::endl;
    ss << "  Connected Clients: " << pImpl->clients.size() << std::endl;
    ss << "  Message Queue Size: " << pImpl->messageQueue.size() << std::endl;
    return ss.str();
}

void VisualizationWebSocketServer::runServer() {
    // TODO: Implement actual WebSocket server logic
    while (pImpl->running) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        pImpl->queueCV.wait_for(lock, std::chrono::milliseconds(100));
        
        // Process messages from queue
        while (!pImpl->messageQueue.empty()) {
            std::string message = pImpl->messageQueue.front();
            pImpl->messageQueue.pop();
            
            // TODO: Send to all connected clients
            NLM_LOG_INFO("Broadcasting message: " + message);
        }
    }
}

// BrainStructureVisualizer Implementation
struct BrainStructureVisualizer::Impl {
    bool showCortex = true;
    bool showSubcortical = true;
    bool showPathways = true;
    double sliceThickness = 0.1;
    bool transparentLayers = false;
    bool show3DProjection = true;
    
    std::vector<glm::vec3> corticalLayerPoints;
    std::vector<glm::vec3> subcorticalPoints;
    std::vector<std::pair<glm::vec3, glm::vec3>> pathways;
    
    Impl() = default;
};

BrainStructureVisualizer::BrainStructureVisualizer() : pImpl(std::make_unique<Impl>()) {}
BrainStructureVisualizer::~BrainStructureVisualizer() = default;

bool BrainStructureVisualizer::initialize() {
    NLM_LOG_INFO("Brain Structure Visualizer initialized successfully");
    return true;
}

void BrainStructureVisualizer::update(const Brain& brain) {
    // Generate 3D points representing brain structure
    // This is a simplified implementation - would need real anatomical data
    
    corticalLayerPoints.clear();
    subcorticalPoints.clear();
    pathways.clear();
    
    // Create cortical layers (simplified)
    for (int layer = 0; layer < 6; ++layer) {
        float y = layer * 0.5f - 1.5f; // Six layers
        for (float x = -3.0f; x <= 3.0f; x += 0.5f) {
            for (float z = -3.0f; z <= 3.0f; z += 0.5f) {
                if (layer % 2 == 0) { // Only show some points for visualization
                    corticalLayerPoints.push_back(glm::vec3(x, y, z));
                }
            }
        }
    }
    
    // Create subcortical structures
    subcorticalPoints.push_back(glm::vec3(0.0f, -1.0f, 0.0f)); // Placeholder for hippocampus
    subcorticalPoints.push_back(glm::vec3(2.0f, -1.0f, 0.0f)); // Placeholder for amygdala
    subcorticalPoints.push_back(glm::vec3(-2.0f, -1.0f, 0.0f)); // Placeholder for thalamus
    
    // Create pathways (simplified)
    pathways.push_back(std::make_pair(glm::vec3(-2.0f, 0.0f, 0.0f), 
                                 glm::vec3(2.0f, 0.0f, 0.0f)));
}

void BrainStructureVisualizer::renderCorticalLayer(int layer, const Brain& brain) {
    // Render specific cortical layer
    // In a real implementation, would render the specific layer geometry
    NLM_LOG_INFO("Rendering cortical layer " + std::to_string(layer));
}

void BrainStructureVisualizer::renderSubcorticalStructures(const Brain& brain) {
    // Render subcortical brain structures
    NLM_LOG_INFO("Rendering subcortical structures");
}

void BrainStructureVisualizer::renderConnectivePathways(const Brain& brain) {
    // Render neural pathways and connections
    NLM_LOG_INFO("Rendering connective pathways");
}

void BrainStructureVisualizer::highlightRegion(const std::string& regionName, 
                                               const std::string& color) {
    // Highlight specific brain region
    NLM_LOG_INFO("Highlighting region " + regionName + " with color " + color);
}

void BrainStructureVisualizer::highlightConnections(
    const std::vector<std::pair<NeuronId, NeuronId>>& connections) {
    // Highlight specific connections
    NLM_LOG_INFO("Highlighting " + std::to_string(connections.size()) + " connections");
}

void BrainStructureVisualizer::setSliceThickness(double thickness) {
    pImpl->sliceThickness = thickness;
}

void BrainStructureVisualizer::setTransparentLayer(bool transparent) {
    pImpl->transparentLayers = transparent;
}

void BrainStructureVisualizer::set3DProjection(bool enabled) {
    pImpl->show3DProjection = enabled;
}

std::string BrainStructureVisualizer::getStructuralStatistics() const {
    std::stringstream ss;
    ss << "Brain Structure Statistics:" << std::endl;
    ss << "  Cortical Layers Points: " << pImpl->corticalLayerPoints.size() << std::endl;
    ss << "  Subcortical Points: " << pImpl->subcorticalPoints.size() << std::endl;
    ss << "  Pathways: " << pImpl->pathways.size() << std::endl;
    ss << "  Show Cortex: " << (pImpl->showCortex ? "Yes" : "No") << std::endl;
    ss << "  Show Subcortical: " << (pImpl->showSubcortical ? "Yes" : "No") << std::endl;
    ss << "  Show Pathways: " << (pImpl->showPathways ? "Yes" : "No") << std::endl;
    return ss.str();
}

// MLModelVisualizer Implementation
struct MLModelVisualizer::Impl {
    std::string metricToVisualize = "firing_rate";
    bool showErrorBars = false;
    std::string clusterAlgorithm = "kmeans";
    
    // ML model data
    std::vector<glm::vec3> decisionBoundaries;
    std::vector<glm::vec3> featureSpacePoints;
    std::vector<std::vector<glm::vec3>> clusters;
    std::vector<float> networkMetrics;
    
    Impl() = default;
};

MLModelVisualizer::MLModelVisualizer() : pImpl(std::make_unique<Impl>()) {}
MLModelVisualizer::~MLModelVisualizer() = default;

bool MLModelVisualizer::initialize() {
    NLM_LOG_INFO("ML Model Visualizer initialized successfully");
    return true;
}

void MLModelVisualizer::update(const PredictionSystem& predictionSystem, double time) {
    // Update with prediction system data
    // This would extract features and predictions from the prediction system
    NLM_LOG_INFO("Updating ML model visualization with time " + std::to_string(time));
}

void MLModelVisualizer::renderDecisionBoundaries(const Brain& brain) {
    // Render decision boundaries in feature space
    NLM_LOG_INFO("Rendering decision boundaries");
}

void MLModelVisualizer::renderFeatureSpace(const Brain& brain) {
    // Render neural feature space
    NLM_LOG_INFO("Rendering feature space visualization");
}

void MLModelVisualizer::renderClusterVisualization(const Brain& brain) {
    // Render clustering results
    NLM_LOG_INFO("Rendering cluster visualization");
}

void MLModelVisualizer::renderNetworkMetrics(const Brain& brain) {
    // Render network metrics
    NLM_LOG_INFO("Rendering network metrics");
}

void MLModelVisualizer::setMetricToVisualize(const std::string& metric) {
    pImpl->metricToVisualize = metric;
}

void MLModelVisualizer::enableErrorBars(bool enable) {
    pImpl->showErrorBars = enable;
}

void MLModelVisualizer::setClusterAlgorithm(const std::string& algorithm) {
    pImpl->clusterAlgorithm = algorithm;
}

std::string MLModelVisualizer::getNetworkMetrics() const {
    std::stringstream ss;
    ss << "Network Metrics:" << std::endl;
    ss << "  Metric to Visualize: " << pImpl->metricToVisualize << std::endl;
    ss << "  Show Error Bars: " << (pImpl->showErrorBars ? "Yes" : "No") << std::endl;
    ss << "  Cluster Algorithm: " << pImpl->clusterAlgorithm << std::endl;
    return ss.str();
}

std::string MLModelVisualizer::getClusterData() const {
    std::stringstream ss;
    ss << "Cluster Data:" << std::endl;
    ss << "  Number of Clusters: " << pImpl->clusters.size() << std::endl;
    return ss.str();
}

// AdvancedVisualizationSystem Implementation
struct AdvancedVisualizationSystem::Impl {
    std::shared_ptr<NeuralNetworkRenderer> neuralRenderer;
    std::shared_ptr<NeuralTimeSeries> timeSeries;
    std::shared_ptr<VisualizationWebSocketServer> webSocketServer;
    std::shared_ptr<BrainStructureVisualizer> structureVisualizer;
    std::shared_ptr<MLModelVisualizer> mlVisualizer;
    
    std::string currentMode = "3d_network";
    bool performanceMonitoring = false;
    double lastFrameTime = 0.0;
    
    std::mutex updateMutex;
    
    Impl() {
        neuralRenderer = std::make_shared<NeuralNetworkRenderer>();
        timeSeries = std::make_shared<NeuralTimeSeries>();
        structureVisualizer = std::make_shared<BrainStructureVisualizer>();
        mlVisualizer = std::make_shared<MLModelVisualizer>();
        webSocketServer = std::make_shared<VisualizationWebSocketServer>();
    }
};

AdvancedVisualizationSystem::AdvancedVisualizationSystem() : pImpl(std::make_unique<Impl>()) {}
AdvancedVisualizationSystem::~AdvancedVisualizationSystem() {
    shutdown();
}

bool AdvancedVisualizationSystem::initialize(GLFWwindow* window) {
    NLM_LOG_INFO("Initializing Advanced Visualization System...");
    
    // Initialize components
    if (!pImpl->neuralRenderer->initialize(window)) {
        NLM_LOG_ERROR("Failed to initialize neural renderer");
        return false;
    }
    
    if (!pImpl->timeSeries->initialize()) {
        NLM_LOG_ERROR("Failed to initialize time series visualizer");
        return false;
    }
    
    if (!pImpl->structureVisualizer->initialize()) {
        NLM_LOG_ERROR("Failed to initialize structure visualizer");
        return false;
    }
    
    if (!pImpl->mlVisualizer->initialize()) {
        NLM_LOG_ERROR("Failed to initialize ML model visualizer");
        return false;
    }
    
    // Start WebSocket server
    if (!pImpl->webSocketServer->start()) {
        NLM_LOG_ERROR("Failed to start WebSocket server");
        return false;
    }
    
    NLM_LOG_INFO("Advanced Visualization System initialized successfully");
    return true;
}

void AdvancedVisualizationSystem::update(std::shared_ptr<Brain> brain, double time) {
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    
    if (!brain) {
        return;
    }
    
    // Get spike events for time-series visualization
    std::vector<std::pair<NeuronId, Timestamp>> spikeEvents;
    // TODO: Get actual spike events from brain
    
    // Update all visualization components
    pImpl->neuralRenderer->update(*brain, time);
    pImpl->timeSeries->update(spikeEvents, time);
    pImpl->structureVisualizer->update(*brain);
    
    // Broadcast state via WebSocket
    pImpl->webSocketServer->broadcastBrainState(*brain, time);
    
    // Track performance
    pImpl->lastFrameTime = time;
}

void AdvancedVisualizationSystem::render() {
    // Render all visualization components
    pImpl->neuralRenderer->render();
    pImpl->timeSeries->renderSpikingActivity();
    pImpl->timeSeries->renderHeatMap("firing_rate");
}

void AdvancedVisualizationSystem::onWindowResize(int width, int height) {
    // Handle window resize events
    NLM_LOG_INFO("Window resized to " + std::to_string(width) + "x" + std::to_string(height));
    
    // TODO: Update projection matrices
}

void AdvancedVisualizationSystem::onMouseButton(int button, int action, double x, double y) {
    if (action == GLFW_PRESS) {
        pImpl->neuralRenderer->onMouseClick(x, y);
    }
}

void AdvancedVisualizationSystem::onCursorPosition(double x, double y) {
    // Handle mouse movement
    // TODO: Implement camera controls
}

void AdvancedVisualizationSystem::setVisualizationMode(const std::string& mode) {
    pImpl->currentMode = mode;
    
    if (mode == "3d_network") {
        // Enable 3D network renderer
    } else if (mode == "time_series") {
        // Focus on time-series
    } else if (mode == "structure") {
        // Focus on brain structure
    }
}

void AdvancedVisualizationSystem::toggleComponent(const std::string& component) {
    if (component == "neural_renderer") {
        // TODO: Toggle neural renderer visibility
    } else if (component == "time_series") {
        // TODO: Toggle time-series visibility
    }
}

void AdvancedVisualizationSystem::exportAllData(const std::string& basePath) {
    // Export all visualization data
    std::string timestamp = "export_" + std::to_string(std::time(nullptr));
    
    // Export neural network data
    pImpl->neuralRenderer->exportToFile(basePath + "/" + timestamp + "_neural_network.json", "json");
    
    // Export time-series data
    std::string csvData = pImpl->timeSeries->getDataCSV();
    std::ofstream csvFile(basePath + "/" + timestamp + "_time_series.csv");
    if (csvFile.is_open()) {
        csvFile << csvData;
        csvFile.close();
    }
    
    NLM_LOG_INFO("Exported all visualization data to: " + basePath);
}

void AdvancedVisualizationSystem::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoring = enable;
    
    if (enable) {
        NLM_LOG_INFO("Performance monitoring enabled");
    } else {
        NLM_LOG_INFO("Performance monitoring disabled");
    }
}

std::string AdvancedVisualizationSystem::getPerformanceStats() const {
    std::stringstream ss;
    ss << "Performance Statistics:" << std::endl;
    ss << "  Current Mode: " << pImpl->currentMode << std::endl;
    ss << "  Performance Monitoring: " << (pImpl->performanceMonitoring ? "Enabled" : "Disabled") << std::endl;
    ss << "  Last Frame Time: " << pImpl->lastFrameTime << "s" << std::endl;
    ss << "  Neural Renderer Status: " << pImpl->neuralRenderer->getStatistics() << std::endl;
    ss << "  Time Series Status: " << pImpl->timeSeries->getSpikeStatistics() << std::endl;
    ss << "  WebSocket Server Status: " << pImpl->webSocketServer->getServerStatus() << std::endl;
    return ss.str();
}

void AdvancedVisualizationSystem::shutdown() {
    NLM_LOG_INFO("Shutting down Advanced Visualization System...");
    
    // Stop WebSocket server
    pImpl->webSocketServer->stop();
    
    NLM_LOG_INFO("Advanced Visualization System shutdown complete");
}

} // namespace nlm
