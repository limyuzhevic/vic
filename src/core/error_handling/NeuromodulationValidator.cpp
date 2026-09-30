#include "NeuromodulationValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void NeuromodulationValidator::validateNeuromodulatorLevels(const Neuromodulator& signal) {
    // Validate dopamine level
    if (signal.dopamine < 0.0f || signal.dopamine > 10.0f) {
        std::map<std::string, std::string> context;
        context["dopamine"] = std::to_string(signal.dopamine);
        context["valid_range"] = "[0.0, 10.0]";
        
        throw NeuromodulationException(
            "Invalid dopamine level",
            context,
            "NeuromodulationValidator"
        );
    }
    
    // Validate curiosity level
    if (signal.curiosity < 0.0f || signal.curiosity > 10.0f) {
        std::map<std::string, std::string> context;
        context["curiosity"] = std::to_string(signal.curiosity);
        context["valid_range"] = "[0.0, 10.0]";
        
        throw NeuromodulationException(
            "Invalid curiosity level",
            context,
            "NeuromodulationValidator"
        );
    }
    
    // Validate novelty level
    if (signal.novelty < 0.0f || signal.novelty > 10.0f) {
        std::map<std::string, std::string> context;
        context["novelty"] = std::to_string(signal.novelty);
        context["valid_range"] = "[0.0, 10.0]";
        
        throw NeuromodulationException(
            "Invalid novelty level",
            context,
            "NeuromodulationValidator"
        );
    }
}

void NeuromodulationValidator::validateNeuromodulationParameters(const std::map<std::string, double>& params) {
    // Validate neuromodulation parameters
    if (params.find("learning_rate") != params.end()) {
        double rate = params.at("learning_rate");
        Validator::validateRange(rate, 0.0, 1.0, "learning_rate");
    }
    
    if (params.find("decay_rate") != params.end()) {
        double decay = params.at("decay_rate");
        Validator::validateRange(decay, 0.0, 1.0, "decay_rate");
    }
}

void NeuromodulationValidator::validateNeuromodulationTiming(const std::map<std::string, double>& timing) {
    // Validate timing parameters
    if (timing.find("onset_time") != timing.end()) {
        double onset = timing.at("onset_time");
        Validator::validateRange(onset, 0.0, 1e6, "onset_time");
    }
    
    if (timing.find("duration") != timing.end()) {
        double duration = timing.at("duration");
        Validator::validateRange(duration, 0.0, 1e6, "duration");
    }
}

void NeuromodulationValidator::handleNeuromodulationError(const std::exception& e) {
    try {
        throw;
    } catch (const BrainException& be) {
        LOG_ERROR("Neuromodulation system error: " + std::string(be.whatWithDetails()));
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected neuromodulation system error: " + std::string(e.what()));
        throw NeuromodulationException(
            "Unexpected error in neuromodulation system",
            {{ "error", std::string(e.what()) }},
            "NeuromodulationValidator"
        );
    }
}

} // namespace nlm
