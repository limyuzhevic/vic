#include "InteractionManager.hpp"
#include <iostream>

namespace nlm {

struct InteractionManager::Impl {
    std::function<void(int, int, int)> mouseCallback;
    std::function<void(int, int)> mouseMotionCallback;
    std::function<void(int, int)> mouseWheelCallback;
    std::function<void(int, int, int)> keyCallback;
    std::function<void(unsigned int)> charCallback;
    std::function<void(int, int)> touchBeginCallback;
    std::function<void(int, int)> touchEndCallback;
    std::function<void(int, int, int, int)> touchMotionCallback;
    std::function<void()> windowCloseCallback;
    std::function<void(bool)> windowFocusCallback;
    std::function<void(int, int)> windowResizeCallback;
    std::function<void(const std::string&)> dragDropCallback;
    std::function<void(const std::string&)> selectionCallback;
    bool mouseCaptured;
    bool keyboardCaptured;
    float inputScale;
    int repeatDelay;
    int repeatRate;
    bool debugMode;
    
    Impl() : mouseCaptured(false), keyboardCaptured(false), inputScale(1.0f), repeatDelay(500), repeatRate(50), debugMode(false) {}
};

InteractionManager::InteractionManager() : pImpl(std::make_unique<Impl>()) {}

InteractionManager::~InteractionManager() = default;

void InteractionManager::setMouseCallback(std::function<void(int, int, int)> callback) {
    pImpl->mouseCallback = callback;
    std::cout << "Mouse callback set" << std::endl;
}

void InteractionManager::setMouseMotionCallback(std::function<void(int, int)> callback) {
    pImpl->mouseMotionCallback = callback;
    std::cout << "Mouse motion callback set" << std::endl;
}

void InteractionManager::setMouseWheelCallback(std::function<void(int, int)> callback) {
    pImpl->mouseWheelCallback = callback;
    std::cout << "Mouse wheel callback set" << std::endl;
}

void InteractionManager::setKeyCallback(std::function<void(int, int, int)> callback) {
    pImpl->keyCallback = callback;
    std::cout << "Key callback set" << std::endl;
}

void InteractionManager::setCharCallback(std::function<void(unsigned int)> callback) {
    pImpl->charCallback = callback;
    std::cout << "Char callback set" << std::endl;
}

void InteractionManager::setTouchBeginCallback(std::function<void(int, int)> callback) {
    pImpl->touchBeginCallback = callback;
    std::cout << "Touch begin callback set" << std::endl;
}

void InteractionManager::setTouchEndCallback(std::function<void(int, int)> callback) {
    pImpl->touchEndCallback = callback;
    std::cout << "Touch end callback set" << std::endl;
}

void InteractionManager::setTouchMotionCallback(std::function<void(int, int, int, int)> callback) {
    pImpl->touchMotionCallback = callback;
    std::cout << "Touch motion callback set" << std::endl;
}

void InteractionManager::setWindowCloseCallback(std::function<void()> callback) {
    pImpl->windowCloseCallback = callback;
    std::cout << "Window close callback set" << std::endl;
}

void InteractionManager::setWindowFocusCallback(std::function<void(bool)> callback) {
    pImpl->windowFocusCallback = callback;
    std::cout << "Window focus callback set" << std::endl;
}

void InteractionManager::setWindowResizeCallback(std::function<void(int, int)> callback) {
    pImpl->windowResizeCallback = callback;
    std::cout << "Window resize callback set" << std::endl;
}

void InteractionManager::setDragDropCallback(std::function<void(const std::string&)> callback) {
    pImpl->dragDropCallback = callback;
    std::cout << "Drag drop callback set" << std::endl;
}

void InteractionManager::setSelectionCallback(std::function<void(const std::string&)> callback) {
    pImpl->selectionCallback = callback;
    std::cout << "Selection callback set" << std::endl;
}

void InteractionManager::captureMouse(bool capture) {
    pImpl->mouseCaptured = capture;
    std::cout << "Mouse capture " << (capture ? "enabled" : "disabled") << std::endl;
}

void InteractionManager::captureKeyboard(bool capture) {
    pImpl->keyboardCaptured = capture;
    std::cout << "Keyboard capture " << (capture ? "enabled" : "disabled") << std::endl;
}

bool InteractionManager::isMouseCaptured() const {
    return pImpl->mouseCaptured;
}

bool InteractionManager::isKeyboardCaptured() const {
    return pImpl->keyboardCaptured;
}

void InteractionManager::setInputScale(float scale) {
    pImpl->inputScale = scale;
    std::cout << "Input scale set to: " << scale << std::endl;
}

void InteractionManager::setRepeatDelay(int delay) {
    pImpl->repeatDelay = delay;
    std::cout << "Repeat delay set to: " << delay << " ms" << std::endl;
}

void InteractionManager::setRepeatRate(int rate) {
    pImpl->repeatRate = rate;
    std::cout << "Repeat rate set to: " << rate << " Hz" << std::endl;
}

void InteractionManager::enableDebugMode(bool enable) {
    pImpl->debugMode = enable;
    std::cout << "Debug mode " << (enable ? "enabled" : "disabled") << std::endl;
}

bool InteractionManager::isDebugMode() const {
    return pImpl->debugMode;
}

void InteractionManager::calibrateTouchScreen() {
    std::cout << "Touch screen calibration started" << std::endl;
    // In a real implementation, would calibrate touch screen
}

void InteractionManager::resetInputState() {
    captureMouse(false);
    captureKeyboard(false);
    std::cout << "Input state reset" << std::endl;
}

} // namespace nlm