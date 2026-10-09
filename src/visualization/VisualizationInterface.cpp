#include "VisualizationInterface.hpp"
#include "AdvancedVisualization.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::shared_ptr<AdvancedVisualizationSystem> advancedViz;
    GLFWwindow* window;
    
    Impl() : active(false), updateRate(30.0), window(nullptr) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() {
    if (pImpl->window) {
        glfwDestroyWindow(pImpl->window);
        glfwTerminate();
    }
}

bool VisualizationInterface::initialize() {
    // Initialize GLFW for OpenGL context
    if (!glfwInit()) {
        NLM_LOG_ERROR("Failed to initialize GLFW");
        return false;
    }
    
    // Create window with OpenGL support
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    pImpl->window = glfwCreateWindow(1280, 720, "NLM Advanced Visualization", nullptr, nullptr);
    if (!pImpl->window) {
        NLM_LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(pImpl->window);
    glfwSwapInterval(1); // Enable vsync
    
    // Initialize advanced visualization system
    pImpl->advancedViz = std::make_shared<AdvancedVisualizationSystem>();
    if (!pImpl->advancedViz->initialize(pImpl->window)) {
        NLM_LOG_ERROR("Failed to initialize advanced visualization system");
        glfwDestroyWindow(pImpl->window);
        glfwTerminate();
        return false;
    }
    
    pImpl->active = true;
    NLM_LOG_INFO("Advanced Visualization Interface initialized successfully");
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active || !pImpl->advancedViz) return;
    
    // Update visualization with current brain state (if available)
    // This would be called from the main simulation loop
    // For now, we just update the visualization system
    pImpl->advancedViz->update(nullptr, 0.0); // TODO: Pass actual brain reference
}

void VisualizationInterface::render() {
    if (!pImpl->active || !pImpl->window) return;
    
    // Render the visualization
    pImpl->advancedViz->render();
    
    // Swap buffers
    glfwSwapBuffers(pImpl->window);
    
    // Poll for events
    glfwPollEvents();
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        pImpl->active = false;
        
        if (pImpl->advancedViz) {
            pImpl->advancedViz->shutdown();
        }
        
        if (pImpl->window) {
            glfwDestroyWindow(pImpl->window);
            pImpl->window = nullptr;
        }
        
        glfwTerminate();
        NLM_LOG_INFO("Visualization Interface closed");
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    
    if (pImpl->advancedViz) {
        // TODO: Configure update rate in visualization system
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (pImpl->advancedViz) {
        pImpl->advancedViz->setVisualizationMode("3d_network");
        // TODO: Actually update the visualization with the brain data
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (pImpl->advancedViz) {
        pImpl->advancedViz->setVisualizationMode("time_series");
        // TODO: Actually update the visualization with the brain data
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (pImpl->advancedViz) {
        // TODO: Update time-series visualization with spike data
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (pImpl->advancedViz) {
        pImpl->advancedViz->setVisualizationMode("weight_matrix");
        // TODO: Update weight visualization with brain data
    }
}

} // namespace nlm
