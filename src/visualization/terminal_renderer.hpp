// NLM Advanced CLI - Terminal Renderer Implementation
#pragma once

#include "visualization/terminal_renderer.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

class terminal_renderer::Impl {
public:
    int width = 80;
    int height = 40;
    bool initialized = false;
    std::vector<std::vector<char>> buffer;
    std::vector<std::string> colors;
    
    Impl() {
        colors = {
            "\033[30m", // Black
            "\033[31m", // Red
            "\033[32m", // Green
            "\033[33m", // Yellow
            "\033[34m", // Blue
            "\033[35m", // Magenta
            "\033[36m", // Cyan
            "\033[37m", // White
            "\033[90m", // Bright Black
            "\033[91m", // Bright Red
            "\033[92m", // Bright Green
            "\033[93m", // Bright Yellow
            "\033[94m", // Bright Blue
            "\033[95m", // Bright Magenta
            "\033[96m", // Bright Cyan
            "\033[97m"  // Bright White
        };
    }
    
    void clearBuffer() {
        buffer.clear();
        for (int y = 0; y < height; ++y) {
            std::vector<char> row(width, ' ');
            buffer.push_back(row);
        }
    }
    
    void setCell(int x, int y, char c, const std::string& color = "\033[37m") {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            buffer[y][x] = c;
            colors[y * width + x] = color;
        }
    }
    
    void render() {
        if (!initialized) return;
        
        // Clear screen
        std::cout << "\033[2J\033[H";
        
        // Render buffer
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (colors[y * width + x].empty()) {
                    std::cout << buffer[y][x];
                } else {
                    std::cout << colors[y * width + x] << buffer[y][x];
                }
            }
            std::cout << "\033[0m\n";
        }
    }
};

terminal_renderer::terminal_renderer() : pImpl(std::make_unique<Impl>()) {
}

terminal_renderer::~terminal_renderer() = default;

bool terminal_renderer::initialize() {
    pImpl->initialized = true;
    pImpl->clearBuffer();
    
    std::cout << "Terminal renderer initialized (" << pImpl->width << "x" << pImpl->height << ")" << std::endl;
    return true;
}

void terminal_renderer::close() {
    if (pImpl->initialized) {
        std::cout << "Terminal renderer closed." << std::endl;
    }
    pImpl->initialized = false;
}

void terminal_renderer::setWidth(int width) {
    pImpl->width = std::max(20, width);
    pImpl->clearBuffer();
}

void terminal_renderer::setHeight(int height) {
    pImpl->height = std::max(10, height);
    pImpl->clearBuffer();
}

void terminal_renderer::visualizeNetwork(const Brain& brain) {
    if (!pImpl->initialized) return;
    
    pImpl->clearBuffer();
    
    // Draw network diagram
    int neuronCount = brain.getTotalNeuronCount();
    int regionCount = brain.getRegionCount();
    
    // Calculate positions
    int neuronsPerRow = std::min(10, neuronCount);
    float neuronSpacing = static_cast<float>(pImpl->width) / std::max(neuronsPerRow + 2, 1);
    
    // Draw neurons as circles
    for (int r = 0; r < regionCount; ++r) {
        int neuronsInRegion = neuronCount / regionCount;
        int regionNeurons = std::min(neuronsInRegion, neuronCount - r * neuronsInRegion);
        
        int regionY = (r + 1) * pImpl->height / (regionCount + 1);
        
        for (int n = 0; n < regionNeurons; ++n) {
            int neuronX = (n % neuronsPerRow) * neuronSpacing + neuronSpacing / 2;
            int neuronY = regionY;
            
            // Draw neuron (simplified as filled circle)
            int radius = 2;
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    int x = neuronX + dx;
                    int y = neuronY + dy;
                    if (x >= 0 && x < pImpl->width && y >= 0 && y < pImpl->height) {
                        float dist = std::sqrt(dx * dx + dy * dy);
                        if (dist <= radius) {
                            bool isFiring = (n < brain.getFiringNeuronCount());
                            if (isFiring) {
                                pImpl->setCell(x, y, 'O', "\033[91m"); // Red if firing
                            } else {
                                pImpl->setCell(x, y, 'o', "\033[34m"); // Blue if resting
                            }
                        }
                    }
                }
            }
        }
    }
}

void terminal_renderer::visualizeActivity(const Brain& brain) {
    if (!pImpl->initialized) return;
    
    pImpl->clearBuffer();
    
    // Create activity histogram
    int histogramHeight = pImpl->height / 3;
    int histogramWidth = pImpl->width / 2;
    
    int firingNeurons = brain.getFiringNeuronCount();
    int totalNeurons = brain.getTotalNeuronCount();
    
    if (totalNeurons > 0) {
        float firingRatio = static_cast<float>(firingNeurons) / totalNeurons;
        int barHeight = static_cast<int>(firingRatio * histogramHeight);
        
        // Draw histogram
        std::string color = "\033[32m"; // Green
        if (firingRatio > 0.5f) {
            color = "\033[33m"; // Yellow
        } else if (firingRatio > 0.8f) {
            color = "\033[31m"; // Red
        }
        
        for (int y = histogramHeight - 1; y >= 0; --y) {
            for (int x = 0; x < histogramWidth; ++x) {
                if (y < barHeight) {
                    pImpl->setCell(x, y, '|', color);
                } else {
                    pImpl->setCell(x, y, '|', "\033[37m"); // Gray
                }
            }
        }
        
        // Draw labels
        pImpl->setCell(0, histogramHeight, 'F', "\033[37m");
        pImpl->setCell(1, histogramHeight, 'i', "\033[37m");
        pImpl->setCell(2, histogramHeight, 'r', "\033[37m");
        pImpl->setCell(3, histogramHeight, 'i', "\033[37m");
        pImpl->setCell(4, histogramHeight, 'n', "\033[37m");
        pImpl->setCell(5, histogramHeight, 'g', "\033[37m");
        
        // Draw statistics
        pImpl->setCell(0, histogramHeight + 2, 'N', "\033[37m");
        pImpl->setCell(1, histogramHeight + 2, ':', "\033[37m");
        pImpl->setCell(2, histogramHeight + 2, totalNeurons / 10 + '0', "\033[37m");
        pImpl->setCell(3, histogramHeight + 2, totalNeurons % 10 + '0', "\033[37m");
        
        int digit = firingNeurons % 10;
        pImpl->setCell(histogramWidth - 2, histogramHeight + 2, digit + '0', "\033[37m");
    }
}

void terminal_renderer::update() {
    if (pImpl->initialized) {
        pImpl->render();
    }
}

void terminal_renderer::close() {
    if (pImpl->initialized) {
        std::cout << "Terminal renderer closed." << std::endl;
    }
    pImpl->initialized = false;
}

} // namespace nlm

