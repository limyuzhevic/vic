#include "VisualizationWindow.hpp"
#include <iostream>

namespace nlm {

struct VisualizationWindow::Impl {
    std::string title;
    int width;
    int height;
    int x;
    int y;
    bool visible;
    bool fullscreen;
    std::shared_ptr<ControlPanel> content;
    std::vector<std::shared_ptr<ControlPanel>> views;
    std::function<void(const std::string&)> eventHandler;
    bool mouseCursor;
    bool keyboardInput;
    std::string theme;
    int fontSize;
    std::vector<std::string> colors;
    
    Impl() : width(800), height(600), x(100), y(100), visible(false), fullscreen(false),
             mouseCursor(true), keyboardInput(true), theme("default"), fontSize(12) {}
};

VisualizationWindow::VisualizationWindow(const std::string& title) : pImpl(std::make_unique<Impl>()) {
    pImpl->title = title;
}

VisualizationWindow::~VisualizationWindow() = default;

void VisualizationWindow::setTitle(const std::string& title) {
    pImpl->title = title;
}

std::string VisualizationWindow::getTitle() const {
    return pImpl->title;
}

void VisualizationWindow::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void VisualizationWindow::getSize(int& width, int& height) const {
    width = pImpl->width;
    height = pImpl->height;
}

void VisualizationWindow::setPosition(int x, int y) {
    pImpl->x = x;
    pImpl->y = y;
}

void VisualizationWindow::getPosition(int& x, int& y) const {
    x = pImpl->x;
    y = pImpl->y;
}

void VisualizationWindow::show() {
    pImpl->visible = true;
    std::cout << "Window shown: " << pImpl->title << std::endl;
}

void VisualizationWindow::hide() {
    pImpl->visible = false;
    std::cout << "Window hidden: " << pImpl->title << std::endl;
}

void VisualizationWindow::close() {
    pImpl->visible = false;
    std::cout << "Window closed: " << pImpl->title << std::endl;
}

bool VisualizationWindow::isVisible() const {
    return pImpl->visible;
}

void VisualizationWindow::setContent(std::shared_ptr<ControlPanel> panel) {
    pImpl->content = panel;
}

void VisualizationWindow::addView(std::shared_ptr<ControlPanel> view) {
    if (view) {
        pImpl->views.push_back(view);
    }
}

void VisualizationWindow::removeView(std::shared_ptr<ControlPanel> view) {
    for (size_t i = 0; i < pImpl->views.size(); ++i) {
        if (pImpl->views[i] == view) {
            pImpl->views.erase(pImpl->views.begin() + i);
            break;
        }
    }
}

void VisualizationWindow::processEvents() {
    if (pImpl->eventHandler) {
        pImpl->eventHandler("events_processed");
    }
}

void VisualizationWindow::setEventHandler(std::function<void(const std::string&)> handler) {
    pImpl->eventHandler = handler;
}

void VisualizationWindow::render() {
    std::cout << "Rendering window: " << pImpl->title << " (" << pImpl->width << "x" << pImpl->height << ")" << std::endl;
    
    if (pImpl->content) {
        pImpl->content->render();
    }
    
    for (const auto& view : pImpl->views) {
        if (view) {
            view->render();
        }
    }
}

void VisualizationWindow::update() {
    if (pImpl->content) {
        pImpl->content->update();
    }
    
    for (const auto& view : pImpl->views) {
        if (view) {
            view->update();
        }
    }
}

void VisualizationWindow::saveState(const std::string& filename) const {
    std::cout << "Saving window state to: " << filename << std::endl;
    // In a real implementation, would serialize window state
}

void VisualizationWindow::loadState(const std::string& filename) {
    std::cout << "Loading window state from: " << filename << std::endl;
    // In a real implementation, would deserialize window state
}

void VisualizationWindow::setFullscreen(bool fullscreen) {
    pImpl->fullscreen = fullscreen;
    std::cout << "Fullscreen " << (fullscreen ? "enabled" : "disabled") << std::endl;
}

bool VisualizationWindow::isFullscreen() const {
    return pImpl->fullscreen;
}

void VisualizationWindow::minimize() {
    std::cout << "Window minimized: " << pImpl->title << std::endl;
}

void VisualizationWindow::maximize() {
    std::cout << "Window maximized: " << pImpl->title << std::endl;
}

void VisualizationWindow::restore() {
    std::cout << "Window restored: " << pImpl->title << std::endl;
}

void VisualizationWindow::setMouseCursor(bool show) {
    pImpl->mouseCursor = show;
}

void VisualizationWindow::setKeyboardInput(bool enable) {
    pImpl->keyboardInput = enable;
}

void VisualizationWindow::setTheme(const std::string& theme) {
    pImpl->theme = theme;
}

void VisualizationWindow::setFontSize(int size) {
    pImpl->fontSize = size;
}

void VisualizationWindow::setColors(const std::vector<std::string>& colors) {
    pImpl->colors = colors;
}

} // namespace nlm