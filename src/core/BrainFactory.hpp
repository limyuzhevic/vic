#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <functional>

namespace nlm {

// Brain factory for creating pre-configured brain instances
class BrainFactory {
public:
    BrainFactory();
    ~BrainFactory();
    
    // Create brain with preset configurations
    static std::shared_ptr<Brain> createDefaultBrain(const std::string& configName = "default");
    static std::shared_ptr<Brain> createMinimalBrain();
    static std::shared_ptr<Brain> createExperimentalBrain();
    static std::shared_ptr<Brain> createResearchBrain();
    static std::shared_ptr<Brain> createFromConfig(const std::string& configFile);
    static std::shared_ptr<Brain> createWithCallback(const std::function<void(std::shared_ptr<Brain>)>& initCallback);
    
    // Python bindings registration
    static void registerPythonBindings(py::module_& m);
};

} // namespace nlm
