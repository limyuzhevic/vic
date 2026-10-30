// Terminal-based visualization methods
void VisualizationInterface::visualizeNetworkTerminal() {
    if (!pImpl->active) return;
    
    // Draw network header
    drawRectangle(5, 8, 140, 30);
    
    // Draw network title
    drawText(10, 10, "NEURAL NETWORK CONNECTIVITY");
    
    // Draw neuron nodes (simplified representation)
    int startX = 15;
    int startY = 20;
    int nodeSpacing = 25;
    
    // Draw sample neurons (maximum 10 for readability)
    int neuronCount = std::min(10, (int)pImpl->neuronActivities.size());
    std::vector<NeuronId> neuronIds;
    
    for (const auto& pair : pImpl->neuronActivities) {
        neuronIds.push_back(pair.first);
        if (neuronIds.size() >= (size_t)neuronCount) break;
    }
    
    // Draw neurons
    for (int i = 0; i < neuronCount; ++i) {
        float activity = pImpl->neuronActivities[neuronIds[i]];
        std::string symbol = getNeuronSymbol(activity);
        std::string color = getColorCode(activity, pImpl->neuronColorScheme);
        
        drawText(startX + i * nodeSpacing, startY, color + symbol + "\033[0m");
    }
    
    // Draw sample connections
    if (neuronCount >= 2) {
        for (int i = 0; i < neuronCount - 1; ++i) {
            for (int j = i + 1; j < neuronCount; ++j) {
                if (rand() % 100 < 30) { // 30% connection probability for display
                    drawLine(startX + i * nodeSpacing + pImpl->neuronSize, startY + pImpl->neuronSize,
                             startX + j * nodeSpacing - pImpl->neuronSize, startY + pImpl->neuronSize);
                }
            }
        }
    }
    
    // Draw legend
    drawText(150, 10, "NEURON TYPES:");
    drawText(150, 12, "  " + getColorCode(1.0, pImpl->neuronColorScheme) + "● EXCITATORY" + "\033[0m");
    drawText(150, 14, "  " + getColorCode(0.0, pImpl->neuronColorScheme) + "○ INHIBITORY" + "\033[0m");
    drawText(150, 16, "  " + getColorCode(0.5, pImpl->neuronColorScheme) + "▫ MODULATORY" + "\033[0m");
}

void VisualizationInterface::visualizeActivityTerminal() {
    if (!pImpl->active) return;
    
    // Draw activity heatmap
    drawRectangle(150, 8, 70, 25);
    
    // Draw title
    drawText(155, 10, "NEURAL ACTIVITY HEATMAP");
    
    // Draw activity bars for neurons
    int barWidth = 30;
    int barSpacing = 5;
    int startX = 155;
    int startY = 30;
    
    int neuronCount = std::min(10, (int)pImpl->neuronActivities.size());
    std::vector<float> activities;
    
    for (const auto& pair : pImpl->neuronActivities) {
        activities.push_back(pair.second);
        if (activities.size() >= (size_t)neuronCount) break;
    }
    
    // Draw activity bars
    for (int i = 0; i < neuronCount; ++i) {
        float activity = activities[i];
        int barHeight = (int)(activity * 15);
        std::string color = getColorCode(activity, pImpl->neuronColorScheme);
        
        // Draw bar
        for (int h = 0; h < barHeight; ++h) {
            drawText(startX + i * (barWidth + barSpacing), startY - h, color + "█" + "\033[0m");
        }
        
        // Draw neuron ID
        std::stringstream ss;
        ss << std::setw(3) << neuronIds[i].index() << ": " << std::fixed << std::setprecision(1) << activity * 100 << "%";
        drawText(startX + i * (barWidth + barSpacing), startY + 3, ss.str());
    }
}

void VisualizationInterface::visualizeSpikesTerminal() {
    if (!pImpl->active) return;
    
    // Draw spike raster
    drawRectangle(5, 45, 140, 25);
    
    // Draw title
    drawText(10, 47, "SPIKE RASTER PLOT");
    
    // Draw spike points
    int neuronsToShow = std::min(20, (int)pImpl->spikeHistory.size() + 10);
    int startX = 15;
    int startY = 55;
    
    // Draw time axis
    for (int t = 0; t <= 100; t += 20) {
        drawLine(startX + t, startY - 5, startX + t, startY + 10);
    }
    
    // Draw spike points
    int spikeCount = std::min(50, (int)pImpl->spikeHistory.size());
    for (int i = 0; i < spikeCount; ++i) {
        int neuronIndex = i % neuronsToShow;
        int timeStep = i * 2 % 100;
        
        drawText(startX + timeStep, startY - neuronIndex * 2, "·");
    }
    
    // Draw neuron labels
    for (int i = 0; i < neuronsToShow; ++i) {
        int neuronIndex = i % neuronsToShow;
        drawText(startX - 25, startY - neuronIndex * 2, "N" + std::to_string(i));
    }
}

void VisualizationInterface::visualizeWeightsTerminal() {
    if (!pImpl->active) return;
    
    // Draw weight matrix
    drawRectangle(150, 45, 70, 25);
    
    // Draw title
    drawText(155, 47, "SYNAPTIC WEIGHT MATRIX");
    
    // Draw weight matrix heatmap
    int matrixSize = 5;
    int cellSize = 3;
    int startX = 155;
    int startY = 60;
    
    // Draw matrix cells
    for (int row = 0; row < matrixSize; ++row) {
        for (int col = 0; col < matrixSize; ++col) {
            float weight = 0.5f; // Simulated weight value
            std::string color = getColorCode(weight, "gradient");
            
            // Draw cell
            for (int r = 0; r < cellSize; ++r) {
                for (int c = 0; c < cellSize; ++c) {
                    drawText(startX + col * (matrixSize * cellSize + 2) + c,
                             startY - row * (matrixSize * cellSize + 2) - r,
                             color + "░" + "\033[0m");
                }
            }
        }
    }
    
    // Draw legend
    drawText(150, 80, "WEIGHT SCALE:");
    drawText(150, 82, "  " + getColorCode(0.0, "gradient") + "━━ DARK (INHIB.)" + "\033[0m");
    drawText(150, 84, "  " + getColorCode(0.5, "gradient") + "━━ NEUTRAL" + "\033[0m");
    drawText(150, 86, "  " + getColorCode(1.0, "gradient") + "━━ STRONG (EXC.)" + "\033[0m");
}

// Helper methods
std::string VisualizationInterface::getColorCode(float value, const std::string& scheme) {
    if (scheme == "gradient") {
        if (value > 0.7) return "\033[91m"; // Red
        else if (value > 0.4) return "\033[93m"; // Yellow
        else if (value > 0.1) return "\033[92m"; // Green
        else return "\033[94m"; // Blue
    } else if (scheme == "fire") {
        if (value > 0.8) return "\033[91m\033[1m"; // Bright red
        else if (value > 0.6) return "\033[91m"; // Red
        else if (value > 0.4) return "\033[93m"; // Orange
        else if (value > 0.2) return "\033[93m"; // Yellow
        else return "\033[96m"; // Cyan
    }
    return "\033[97m"; // White
}

std::string VisualizationInterface::getNeuronSymbol(float activity) {
    if (activity > 0.7) return "●"; // Full
    else if (activity > 0.4) return "○"; // Empty
    else return "▫"; // Half
}

void VisualizationInterface::drawRectangle(int x, int y, int width, int height) {
    // Draw border
    for (int i = 0; i < width; ++i) {
        drawText(x + i, y, "─");
        drawText(x + i, y + height, "─");
    }
    for (int i = 0; i <= height; ++i) {
        drawText(x, y + i, "│");
        drawText(x + width, y + i, "│");
    }
    
    // Draw corners
    drawText(x, y, "┌");
    drawText(x + width, y, "┐");
    drawText(x, y + height, "└");
    drawText(x + width, y + height, "┘");
}

void VisualizationInterface::drawText(int x, int y, const std::string& text) {
    if (x < 0 || y < 0 || x >= 200 || y >= 50) return;
    
    // Simple terminal drawing - in a real implementation, this would use a proper graphics library
    // For now, we'll just print to stdout with carriage return positioning
    // This is a simplified implementation for demonstration
    static std::vector<std::string> buffer(50, std::string(200, ' '));
    
    if (y >= 0 && y < 50 && x >= 0 && x < 200) {
        for (size_t i = 0; i < text.length(); ++i) {
            if (x + (int)i < 200) {
                buffer[y][x + i] = text[i];
            }
        }
    }
}

void VisualizationInterface::drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        drawText(x1, y1, "·");
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}
