#pragma once

#include "visualization/VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

class ConsoleVisualizer : public VisualizationInterface {
public:
    ConsoleVisualizer();
    ~ConsoleVisualizer() override;
    
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Console-specific methods
    void setConsoleWidth(int width);
    void setConsoleHeight(int height);
    void enableColorOutput(bool enable);
    void setOutputFile(const std::string& filePath);
    
    // Override pure virtual methods
    void visualizeNetwork(const class Brain& brain) override;
    void visualizeActivity(const class Brain& brain) override;
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes) override;
    void visualizeWeights(const class Brain& brain) override;
    
    // Export/Import methods
    bool exportData(const std::string& filePath, const std::string& format) override;
    bool importData(const std::string& filePath) override;
    
    double getMemoryUsage() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
