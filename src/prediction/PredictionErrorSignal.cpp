#include "PredictionErrorSignal.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void PredictionErrorSignal::recordError(float error, SimulationStep step) {
    lastError_ = error;
    errorHistory_.push_back(error);
    
    if (errorHistory_.size() > 100) {
        errorHistory_.pop_front();
    }
    
    // Log significant prediction errors
    if (error > surpriseThreshold_) {
        NLM_LOG_WARNING("PredictionErrorSignal: Large prediction error detected: " + std::to_string(error));
    }
}

} // namespace nlm