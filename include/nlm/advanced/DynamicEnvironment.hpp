#pragma once

#include <string>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class AdvancedDynamicEnvironment {
public:
    AdvancedDynamicEnvironment(const std::string& id, const Config& config);
    ~AdvancedDynamicEnvironment();
    
    void updateAllEnvironments(const Brain& brain);
    
    double getOverallStability() const;
    
    std::vector<double> getEnvironmentParameters(size_t simulationIndex) const;
    
    std::string getEnvironmentManagerStatus() const;
    
    void saveAllStates(const Config& config) const;
    
    void loadAllStates(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
