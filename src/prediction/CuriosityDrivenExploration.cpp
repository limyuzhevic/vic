#include "CuriosityDrivenExploration.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

void CuriosityDrivenExploration::updateStateNovelty(const std::vector<float>& state) {
    std::string stateKey = stateToKey(state);
    
    auto it = stateVisits_.find(stateKey);
    if (it == stateVisits_.end()) {
        stateVisits_[stateKey] = 1;
    } else {
        it->second++;
    }
}

float CuriosityDrivenExploration::getStateNoveltyBonus(const std::vector<float>& state) {
    std::string stateKey = stateToKey(state);
    
    auto it = stateVisits_.find(stateKey);
    if (it == stateVisits_.end()) {
        return 1.0f; // Unseen state = maximum novelty
    }
    
    int visits = it->second;
    return std::max(0.0f, 1.0f / (visits * 0.1f)); // Decay with visits
}

std::string CuriosityDrivenExploration::stateToKey(const std::vector<float>& state) {
    std::stringstream ss;
    for (size_t i = 0; i < state.size(); ++i) {
        ss << state[i];
        if (i < state.size() - 1) ss << ",";
    }
    return ss.str();
}

} // namespace nlm