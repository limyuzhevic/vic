#include "LayoutManager.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

struct LayoutManager::Impl {
    std::string layoutType;
    size_t gridColumns;
    size_t gridRows;
    int gridSpacing;
    int gridMargin;
    std::map<std::string, std::pair<int, int>> windowPositions;
    std::map<std::string, std::pair<int, int>> windowSizes;
    std::map<std::string, std::string> docks;
    std::string maximizedWindow;
    
    Impl() : gridColumns(2), gridRows(2), gridSpacing(10), gridMargin(5) {}
};

LayoutManager::LayoutManager() : pImpl(std::make_unique<Impl>()) {}

LayoutManager::~LayoutManager() = default;

void LayoutManager::setLayoutType(const std::string& type) {
    pImpl->layoutType = type;
    std::cout << "Layout type set to: " << type << std::endl;
}

void LayoutManager::setGridColumns(size_t columns) {
    pImpl->gridColumns = columns;
}

void LayoutManager::setGridRows(size_t rows) {
    pImpl->gridRows = rows;
}

void LayoutManager::setGridSpacing(int spacing) {
    pImpl->gridSpacing = spacing;
}

void LayoutManager::setGridMargin(int margin) {
    pImpl->gridMargin = margin;
}

void LayoutManager::addWindow(const std::string& windowId, int x, int y, int width, int height) {
    pImpl->windowPositions[windowId] = std::make_pair(x, y);
    pImpl->windowSizes[windowId] = std::make_pair(width, height);
    std::cout << "Window " << windowId << " added at position (" << x << ", " << y << ") size (" << width << "x" << height << ")" << std::endl;
}

void LayoutManager::removeWindow(const std::string& windowId) {
    pImpl->windowPositions.erase(windowId);
    pImpl->windowSizes.erase(windowId);
    pImpl->docks.erase(windowId);
    if (pImpl->maximizedWindow == windowId) {
        pImpl->maximizedWindow.clear();
    }
    std::cout << "Window " << windowId << " removed" << std::endl;
}

void LayoutManager::updateWindowPosition(const std::string& windowId, int x, int y) {
    if (pImpl->windowPositions.find(windowId) != pImpl->windowPositions.end()) {
        pImpl->windowPositions[windowId] = std::make_pair(x, y);
        std::cout << "Window " << windowId << " position updated to (" << x << ", " << y << ")" << std::endl;
    }
}

void LayoutManager::updateWindowSize(const std::string& windowId, int width, int height) {
    if (pImpl->windowSizes.find(windowId) != pImpl->windowSizes.end()) {
        pImpl->windowSizes[windowId] = std::make_pair(width, height);
        std::cout << "Window " << windowId << " size updated to (" << width << "x" << height << ")" << std::endl;
    }
}

void LayoutManager::calculateLayout() {
    std::cout << "Calculating layout with " << pImpl->layoutType << " layout" << std::endl;
    
    if (pImpl->layoutType == "grid" && !pImpl->windowPositions.empty()) {
        // Calculate grid positions for windows
        size_t windowCount = pImpl->windowPositions.size();
        size_t totalCells = pImpl->gridColumns * pImpl->gridRows;
        
        if (windowCount <= totalCells) {
            size_t index = 0;
            for (auto& window : pImpl->windowPositions) {
                int col = index % pImpl->gridColumns;
                int row = index / pImpl->gridColumns;
                
                int cellWidth = (1920 - 2 * pImpl->gridMargin - (pImpl->gridColumns - 1) * pImpl->gridSpacing) / pImpl->gridColumns;
                int cellHeight = (1080 - 2 * pImpl->gridMargin - (pImpl->gridRows - 1) * pImpl->gridSpacing) / pImpl->gridRows;
                
                window.second.first = pImpl->gridMargin + col * (cellWidth + pImpl->gridSpacing);
                window.second.second = pImpl->gridMargin + row * (cellHeight + pImpl->gridSpacing);
                
                // Update window size if available
                if (pImpl->windowSizes.find(window.first) != pImpl->windowSizes.end()) {
                    pImpl->windowSizes[window.first].first = cellWidth;
                    pImpl->windowSizes[window.first].second = cellHeight;
                }
                
                index++;
            }
        }
    }
}

void LayoutManager::resizeWindows() {
    std::cout << "Resizing windows according to layout" << std::endl;
}

void LayoutManager::addDock(const std::string& dockId, const std::string& position) {
    pImpl->docks[dockId] = position;
    std::cout << "Dock " << dockId << " added at position: " << position << std::endl;
}

void LayoutManager::removeDock(const std::string& dockId) {
    pImpl->docks.erase(dockId);
    std::cout << "Dock " << dockId << " removed" << std::endl;
}

void LayoutManager::splitDock(const std::string& dockId, const std::string& direction) {
    std::cout << "Splitting dock " << dockId << " in direction: " << direction << std::endl;
}

void LayoutManager::saveLayout(const std::string& filename) const {
    std::cout << "Saving layout to: " << filename << std::endl;
    // In a real implementation, would serialize layout
}

void LayoutManager::loadLayout(const std::string& filename) {
    std::cout << "Loading layout from: " << filename << std::endl;
    // In a real implementation, would deserialize layout
}

void LayoutManager::resetLayout() {
    pImpl->windowPositions.clear();
    pImpl->windowSizes.clear();
    pImpl->docks.clear();
    pImpl->maximizedWindow.clear();
    std::cout << "Layout reset to default" << std::endl;
}

void LayoutManager::setMaximizedWindow(const std::string& windowId, bool maximized) {
    if (maximized) {
        pImpl->maximizedWindow = windowId;
    } else {
        pImpl->maximizedWindow.clear();
    }
    std::cout << "Window " << windowId << " maximized: " << (maximized ? "yes" : "no") << std::endl;
}

std::string LayoutManager::getMaximizedWindow() const {
    return pImpl->maximizedWindow;
}

void LayoutManager::render() {
    std::cout << "Rendering layout manager" << std::endl;
    
    // Render dock areas
    for (const auto& dock : pImpl->docks) {
        std::cout << "  Dock: " << dock.first << " at position: " << dock.second << std::endl;
    }
    
    // Render windows
    for (const auto& window : pImpl->windowPositions) {
        std::cout << "  Window: " << window.first << " at position (" << window.second.first << ", " << window.second.second << ")" << std::endl;
        if (pImpl->windowSizes.find(window.first) != pImpl->windowSizes.end()) {
            std::cout << "    Size: (" << pImpl->windowSizes[window.first].first << "x" << pImpl->windowSizes[window.first].second << ")" << std::endl;
        }
    }
}

void LayoutManager::update() {
    // Update layout animations if any
}

} // namespace nlm