#pragma once

#include "PluginBase.hpp"
#include "../visualization/VisualizationInterface.hpp"

namespace nlm {

class VisualizationPlugin : public NLMPlugin {
public:
    // Visualization extension
    virtual void updateVisualization(double deltaTime) = 0;
    virtual void renderVisualization(VisualizationInterface* visualizer) = 0;
    virtual VisualizationMode getVisualizationMode() const = 0;
    
    // Visualization parameters
    virtual std::string getOutputFormat() const = 0;
    virtual double getUpdateInterval() const = 0;
    virtual bool getEnable3D() const = 0;
};

} // namespace nlm
