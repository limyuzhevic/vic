#ifndef NLM_PLUGIN_IMEMORYPLUGIN_H
#define NLM_PLUGIN_IMEMORYPLUGIN_H

#include "IPlugin.h"
#include "src/memory/Memory.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace plugin {

class IMemoryPlugin : public virtual IPlugin {
public:
    virtual ~IMemoryPlugin() = default;

    // Memory system type
    virtual std::string getMemorySystemName() const = 0;

    // Memory operations
    virtual std::shared_ptr<Memory> createMemorySystem() = 0;
    virtual bool storeMemory(const std::string& key, const std::vector<double>& data) = 0;
    virtual std::vector<double> retrieveMemory(const std::string& key) = 0;

    // Memory configuration
    virtual void configureMemorySystem() = 0;
    virtual size_t getMemoryCapacity() const = 0;
    virtual double getForgettingRate() const = 0;

    // Learning parameters
    virtual std::map<std::string, double> getLearningParameters() const = 0;
    virtual void setLearningParameters(const std::map<std::string, double>& params) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_IMEMORYPLUGIN_H
