#pragma once

#include <string>
#include <vector>
#include "../core/Config/Config.hpp"

namespace nlm {

class AdvancedRL {
public:
    AdvancedRL(const Config& config);
    ~AdvancedRL();
    
    void startEpisode();
    
    std::vector<double> selectAction(const std::vector<double>& state);
    
    void takeStep(const std::vector<double>& state, const std::vector<double>& action,
                  const std::vector<double>& nextState, double reward);
    
    void finishEpisode();
    
    double getTotalReward() const;
    
    int getEpisodeLength() const;
    
    std::string getRLStatus() const;
    
    void saveModel(const Config& config) const;
    
    void loadModel(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
