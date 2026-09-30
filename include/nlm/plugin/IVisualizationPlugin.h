#ifndef NLM_PLUGIN_IVISUALIZATIONPLUGIN_H
#define NLM_PLUGIN_IVISUALIZATIONPLUGIN_H

#include "IPlugin.h"
#include "src/visualization/VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace nlm {
namespace plugin {

class IVisualizationPlugin : public virtual IPlugin {
public:
    virtual ~IVisualizationPlugin() = default;

    // Visualization type
    virtual std::string getVisualizationType() const = 0;

    // Visualization operations
    virtual std::shared_ptr<VisualizationInterface> createVisualizer() = 0;
    virtual bool render(const std::vector<double>& data, const std::string& outputFormat) = 0;
    virtual std::vector<uint8_t> exportVisualization(const std::string& format) = 0;

    // Visualization configuration
    virtual void configureVisualization() = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual double getFieldOfView() const = 0;

    // Visualization parameters
    virtual std::map<std::string, double> getVisualizationParameters() const = 0;
    virtual void setVisualizationParameters(const std::map<std::string, double>& params) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_IVISUALIZATIONPLUGIN_H
