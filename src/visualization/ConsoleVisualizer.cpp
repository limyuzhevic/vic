#include "ConsoleVisualizer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

namespace nlm {

struct ConsoleVisualizer::Impl {
    bool active;
    double updateRate;
    int width;
    int height;
    bool colorOutput;
    std::string outputFile;
    std::ofstream outputStream;
    
    Impl() : active(false), updateRate(60.0), width(80), height(24), colorOutput(true) {}
    
    ~Impl() {
        if (outputStream.is_open()) {
            outputStream.close();
        }
    }
};

ConsoleVisualizer::ConsoleVisualizer() : pImpl(std::make_unique<Impl>()) {}

ConsoleVisualizer::~ConsoleVisualizer() = default;

bool ConsoleVisualizer::initialize() {
    pImpl->active = true;
    if (!pImpl->outputFile.empty()) {
        pImpl->outputStream.open(pImpl->outputFile, std::ios::out | std::ios::trunc);
    }
    return true;
}

void ConsoleVisualizer::update() {
    // Console update logic would go here
}

void ConsoleVisualizer::render() {
    std::ostream& output = pImpl->outputStream.is_open() ? pImpl->outputStream : std::cout;
    
    if (pImpl->colorOutput) {
        output << "\033[1;34m=== Console Visualizer ===\033[0m" << std::endl;
        output << "\033[32mActive: Yes\033[0m" << std::endl;
        output << "\033[33mUpdate Rate: " << pImpl->updateRate << " Hz\033[0m" << std::endl;
    } else {
        output << "=== Console Visualizer ===" << std::endl;
        output << "Active: Yes" << std::endl;
        output << "Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    }
}

void ConsoleVisualizer::close() {
    pImpl->active = false;
    if (pImpl->outputStream.is_open()) {
        pImpl->outputStream.close();
    }
}

bool ConsoleVisualizer::isActive() const {
    return pImpl->active;
}

void ConsoleVisualizer::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double ConsoleVisualizer::getUpdateRate() const {
    return pImpl->updateRate;
}

void ConsoleVisualizer::setConsoleWidth(int width) {
    pImpl->width = width;
}

void ConsoleVisualizer::setConsoleHeight(int height) {
    pImpl->height = height;
}

void ConsoleVisualizer::enableColorOutput(bool enable) {
    pImpl->colorOutput = enable;
}

void ConsoleVisualizer::setOutputFile(const std::string& filePath) {
    pImpl->outputFile = filePath;
    if (pImpl->outputStream.is_open()) {
        pImpl->outputStream.close();
    }
    if (!filePath.empty()) {
        pImpl->outputStream.open(filePath, std::ios::out | std::ios::trunc);
    }
}

void ConsoleVisualizer::visualizeNetwork(const class Brain& brain) {
    std::ostream& output = pImpl->outputStream.is_open() ? pImpl->outputStream : std::cout;
    output << "=== Network Visualization ===" << std::endl;
    output << "Neurons: " << brain.getNeuronCount() << std::endl;
    output << "Regions: " << brain.getRegionCount() << std::endl;
    output << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
}

void ConsoleVisualizer::visualizeActivity(const class Brain& brain) {
    std::ostream& output = pImpl->outputStream.is_open() ? pImpl->outputStream : std::cout;
    output << "=== Activity Visualization ===" << std::endl;
    output << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    output << "Max Firing Rate: " << brain.getMaxFiringRate() << " Hz" << std::endl;
    output << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
}

void ConsoleVisualizer::visualizeSpikes(const std::vector<class SpikeEvent>& spikes) {
    std::ostream& output = pImpl->outputStream.is_open() ? pImpl->outputStream : std::cout;
    output << "=== Spike Visualization ===" << std::endl;
    output << "Total Spikes: " << spikes.size() << std::endl;
    if (!spikes.empty()) {
        output << "Average Spike Interval: " << (spikes.size() > 1 ? 1000.0 / (spikes.size() - 1) : 0) << " ms" << std::endl;
    }
}

void ConsoleVisualizer::visualizeWeights(const class Brain& brain) {
    std::ostream& output = pImpl->outputStream.is_open() ? pImpl->outputStream : std::cout;
    output << "=== Weight Matrix Visualization ===" << std::endl;
    output << "Average Connection Strength: " << brain.getAverageConnectionStrength() << std::endl;
    output << "Max Connection Strength: " << brain.getMaxConnectionStrength() << std::endl;
    output << "Min Connection Strength: " << brain.getMinConnectionStrength() << std::endl;
}

bool ConsoleVisualizer::exportData(const std::string& filePath, const std::string& format) {
    // Implementation would depend on format
    if (format == "txt") {
        std::ofstream file(filePath, std::ios::out | std::ios::trunc);
        if (file.is_open()) {
            file << "Console Visualization Data" << std::endl;
            file << "Generated at: " << getCurrentTimestamp() << std::endl;
            file.close();
            return true;
        }
    }
    return false;
}

bool ConsoleVisualizer::importData(const std::string& filePath) {
    // Implementation would load data from file
    return false;
}

double ConsoleVisualizer::getMemoryUsage() const {
    return 1.0; // Placeholder
}

} // namespace nlm
