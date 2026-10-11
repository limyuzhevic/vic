// InteractiveVisualizer.cpp - Interactive visualization implementation

#include "InteractiveVisualizer.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>

namespace nlm {

struct InteractiveVisualizer::Impl {
    InteractiveConfig config;
    bool active;
    int width;
    int height;
    std::string windowTitle;
    double lastFrameTime;
    double frameTime;
    double zoomLevel;
    double panX;
    double panY;
    std::string selectedElementId;
    std::vector<std::string> highlightedElements;
    bool recording;
    std::string recordingFile;
    std::chrono::steady_clock::time_point recordingStart;
    InteractionHandler eventHandler;
    SelectionCallback selectionCallback;
    std::vector<double> mousePosition;
    std::vector<double> keyboardState;
    bool keys[256];
    bool mouseButtons[8];
    double mouseScroll;
    
    Impl() : active(false), width(1920), height(1080), zoomLevel(1.0), panX(0.0), panY(0.0),
             recording(false), mouseScroll(0.0) {
        std::fill(std::begin(keys), std::end(keys), false);
        std::fill(std::begin(mouseButtons), std::end(mouseButtons), false);
        mousePosition = {0.0, 0.0};
        keyboardState.resize(256, 0.0);
    }
};

InteractiveVisualizer::InteractiveVisualizer() : pImpl(std::make_unique<Impl>()) {
}

InteractiveVisualizer::InteractiveVisualizer(const InteractiveConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->zoomLevel = config.zoomLevel;
    pImpl->panX = config.panX;
    pImpl->panY = config.panY;
}

InteractiveVisualizer::~InteractiveVisualizer() {
    stop();
}

void InteractiveVisualizer::configure(const InteractiveConfig& config) {
    pImpl->config = config;
    pImpl->zoomLevel = config.zoomLevel;
    pImpl->panX = config.panX;
    pImpl->panY = config.panY;
}

bool InteractiveVisualizer::initialize(int width, int height, const std::string& title) {
    pImpl->width = width;
    pImpl->height = height;
    pImpl->windowTitle = title;
    pImpl->active = true;
    
    NLM_LOG_INFO("InteractiveVisualizer initialized: " + std::to_string(width) + "x" + std::to_string(height) + ", " + title);
    return true;
}

void InteractiveVisualizer::update(const Brain& brain, double timeStep, bool forceUpdate) {
    if (!pImpl->active) return;
    
    // Update internal state
    auto currentTime = std::chrono::steady_clock::now();
    pImpl->lastFrameTime = std::chrono::duration<double>(currentTime - pImpl->recordingStart).count();
    
    if (pImpl->config.enableRecording && pImpl->recording) {
        // Record frame for later playback
    }
    
    // Process input
    processInput(timeStep);
    
    // Update visualization based on current configuration
    // This would call underlying visualization libraries in a real implementation
}

bool InteractiveVisualizer::handleEvent(const InteractionEvent& event, const std::vector<double>& parameters) {
    if (!pImpl->active) return false;
    
    // Convert to internal representation
    switch (event) {
        case InteractionEvent::MOUSE_CLICK:
            if (parameters.size() >= 2) {
                pImpl->mousePosition = {parameters[0], parameters[1]};
                if (parameters.size() > 2) {
                    int button = static_cast<int>(parameters[2]);
                    if (button >= 0 && button < 8) {
                        pImpl->mouseButtons[button] = true;
                    }
                }
            }
            break;
            
        case InteractionEvent::MOUSE_MOVE:
            if (parameters.size() >= 2) {
                pImpl->mousePosition = {parameters[0], parameters[1]};
            }
            break;
            
        case InteractionEvent::KEY_PRESS:
            if (parameters.size() >= 1) {
                int key = static_cast<int>(parameters[0]);
                if (key >= 0 && key < 256) {
                    pImpl->keys[key] = true;
                    pImpl->keyboardState[key] = 1.0;
                }
            }
            break;
            
        case InteractionEvent::KEY_RELEASE:
            if (parameters.size() >= 1) {
                int key = static_cast<int>(parameters[0]);
                if (key >= 0 && key < 256) {
                    pImpl->keys[key] = false;
                    pImpl->keyboardState[key] = 0.0;
                }
            }
            break;
            
        case InteractionEvent::MOUSE_WHEEL:
            if (!parameters.empty()) {
                pImpl->mouseScroll = parameters[0];
            }
            break;
            
        case InteractionEvent::WINDOW_RESIZE:
            if (parameters.size() >= 2) {
                pImpl->width = static_cast<int>(parameters[0]);
                pImpl->height = static_cast<int>(parameters[1]);
            }
            break;
            
        case InteractionEvent::WINDOW_CLOSE:
            pImpl->active = false;
            break;
            
        case InteractionEvent::DRAG_DROP:
            // Handle file drag and drop
            break;
    }
    
    // Call custom event handler if set
    if (pImpl->eventHandler && !pImpl->eventHandler(event, parameters)) {
        NLM_LOG_WARNING("Event handler returned false for event: " + std::to_string(static_cast<int>(event)));
    }
    
    // Process specific events that trigger selection
    if (event == InteractionEvent::MOUSE_CLICK && pImpl->config.enableSelection) {
        handleMouseClick();
    }
    
    return true;
}

void InteractiveVisualizer::processInput(double deltaTime) {
    // Process keyboard input for controls
    if (pImpl->keys[static_cast<int>('W')]) { // Move up
        pImpl->panY += 10.0 * deltaTime * pImpl->zoomLevel;
    }
    if (pImpl->keys[static_cast<int>('S')]) { // Move down
        pImpl->panY -= 10.0 * deltaTime * pImpl->zoomLevel;
    }
    if (pImpl->keys[static_cast<int>('A')]) { // Move left
        pImpl->panX -= 10.0 * deltaTime * pImpl->zoomLevel;
    }
    if (pImpl->keys[static_cast<int>('D')]) { // Move right
        pImpl->panX += 10.0 * deltaTime * pImpl->zoomLevel;
    }
    
    // Zoom controls
    if (pImpl->mouseScroll != 0.0) {
        double zoomChange = pImpl->mouseScroll * 0.1 * pImpl->config.interactionSensitivity;
        setZoom(pImpl->zoomLevel + zoomChange);
        pImpl->mouseScroll = 0.0;
    }
    
    // Selection mode toggle
    if (pImpl->keys[static_cast<int>('E')]) { // Enable selection mode
        pImpl->config.enableSelection = !pImpl->config.enableSelection;
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Debounce
    }
}

bool InteractiveVisualizer::render() {
    if (!pImpl->active) return false;
    
    // Clear screen
    // In a real implementation, this would use OpenGL, Vulkan, or similar
    
    // Render visualization with current state
    // This would call the underlying visualization system
    
    // Render UI elements
    renderUI();
    
    return true;
}

bool InteractiveVisualizer::exportState(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Export current configuration and state
    file << "InteractiveVisualizer State Export\n";
    file << "===============================\n\n";
    file << "Timestamp: " << getCurrentTime() << "\n";
    file << "Zoom Level: " << pImpl->zoomLevel << "\n";
    file << "Pan: (" << pImpl->panX << ", " << pImpl->panY << ")\n";
    file << "Selected Element: " << pImpl->selectedElementId << "\n";
    file << "Highlighted Elements: " << pImpl->highlightedElements.size() << "\n";
    file << "Mouse Position: (" << pImpl->mousePosition[0] << ", " << pImpl->mousePosition[1] << ")\n";
    
    for (const auto& element : pImpl->highlightedElements) {
        file << "  - " << element << "\n";
    }
    
    file.close();
    
    NLM_LOG_INFO("InteractiveVisualizer state exported: " + filename);
    return true;
}

void InteractiveVisualizer::start() {
    pImpl->active = true;
    pImpl->recording = pImpl->config.enableRecording;
    if (pImpl->recording) {
        pImpl->recordingStart = std::chrono::steady_clock::now();
        pImpl->recordingFile = pImpl->config.recordingFile;
    }
    
    NLM_LOG_INFO("InteractiveVisualizer started");
}

void InteractiveVisualizer::stop() {
    pImpl->active = false;
    pImpl->recording = false;
    NLM_LOG_INFO("InteractiveVisualizer stopped");
}

bool InteractiveVisualizer::isActive() const {
    return pImpl->active;
}

bool InteractiveVisualizer::isRecording() const {
    return pImpl->recording;
}

void InteractiveVisualizer::setInteractionHandler(const InteractionHandler& handler) {
    pImpl->eventHandler = handler;
}

void InteractiveVisualizer::setSelectionCallback(const SelectionCallback& callback) {
    pImpl->selectionCallback = callback;
}

InteractiveVisualizer::InteractiveConfig InteractiveVisualizer::getConfig() const {
    return pImpl->config;
}

void InteractiveVisualizer::setZoom(double zoomLevel) {
    if (pImpl->config.enableZoom) {
        pImpl->zoomLevel = std::max(0.1, std::min(10.0, zoomLevel)); // Clamp to reasonable range
        pImpl->config.zoomLevel = pImpl->zoomLevel;
    }
}

double InteractiveVisualizer::getZoom() const {
    return pImpl->zoomLevel;
}

void InteractiveVisualizer::setPan(double x, double y) {
    if (pImpl->config.enablePan) {
        pImpl->panX = x;
        pImpl->panY = y;
        pImpl->config.panX = x;
        pImpl->config.panY = y;
    }
}

std::pair<double, double> InteractiveVisualizer::getPan() const {
    return {pImpl->panX, pImpl->panY};
}

void InteractiveVisualizer::handleMouseClick() {
    // Handle mouse click for element selection
    if (pImpl->config.enableSelection && pImpl->config.selectedElementId.empty()) {
        // Simple implementation: find element under mouse cursor
        std::string elementId = findElementUnderMouse();
        if (!elementId.empty()) {
            pImpl->selectedElementId = elementId;
            pImpl->config.selectedElementId = elementId;
            
            if (pImpl->selectionCallback) {
                pImpl->selectionCallback(elementId, getElementProperties(elementId));
            }
            
            NLM_LOG_INFO("Selected element: " + elementId);
        }
    }
}

void InteractiveVisualizer::renderUI() {
    // Render UI elements
    // In a real implementation, this would render menus, buttons, and information panels
}

std::string InteractiveVisualizer::findElementUnderMouse() const {
    // Find element under current mouse position
    // This would use the underlying visualization system to pick elements
    return "";
}

std::vector<double> InteractiveVisualizer::getElementProperties(const std::string& elementId) const {
    // Get properties of element for display
    return {};
}

std::string InteractiveVisualizer::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace nlm
