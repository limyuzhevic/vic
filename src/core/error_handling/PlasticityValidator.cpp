#include "PlasticityValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void PlasticityValidator::validatePlasticityRules(const PlasticityFlags& flags) {
    // Validate that at least one plasticity rule is enabled
    if (!flags.hebbian && !flags.stdp && !flags.reward_modulated) {
        throw PlasticityException(
            "At least one plasticity rule must be enabled",
            {{ "hebbian", flags.hebbian ? "true" : "false" },
             { "stdp", flags.stdp ? "true" : "false" },
             { "reward_modulated", flags.reward_modulated ? "true" : "false" }},
            "PlasticityValidator"
        );
    }
    
    // Validate that conflicting plasticity rules are not both enabled
    if (flags.hebbian && flags.reward_modulated) {
        throw PlasticityException(
            "Hebbian and reward-modulated plasticity are incompatible",
            {{ "hebbian", "true" }, { "reward_modulated", "true" }},
            "PlasticityValidator"
        );
    }
}

void PlasticityValidator::validatePlasticityParameters(const std::map<std::string, double>& params) {
    // Validate learning rates
    if (params.find("hebbian_learning_rate") != params.end()) {
        double rate = params.at("hebbian_learning_rate");
        Validator::validateRange(rate, 0.0, 1.0, "hebbian_learning_rate");
    }
    
    // Validate STDP parameters
    if (params.find("stdp_ltp_weight") != params.end()) {
        double ltp = params.at("stdp_ltp_weight");
        Validator::validateRange(ltp, -1.0, 1.0, "stdp_ltp_weight");
    }
    
    if (params.find("stdp_ltd_weight") != params.end()) {
        double ltd = params.at("stdp_ltd_weight");
        Validator::validateRange(ltd, -1.0, 1.0, "stdp_ltd_weight");
    }
    
    if (params.find("stdp_time_constant") != params.end()) {
        double tau = params.at("stdp_time_constant");
        Validator::validateRange(tau, 1.0, 1000.0, "stdp_time_constant");
    }
}

void PlasticityValidator::validateLearningRules(const std::vector<std::string>& rules) {
    // Validate that learning rules are not empty
    if (rules.empty()) {
        throw PlasticityException(
            "At least one learning rule must be specified",
            {},
            "PlasticityValidator"
        );
    }
    
    // Validate that duplicate learning rules are not present
    std::set<std::string> uniqueRules(rules.begin(), rules.end());
    if (uniqueRules.size() != rules.size()) {
        throw PlasticityException(
            "Duplicate learning rules are not allowed",
            {},
            "PlasticityValidator"
        );
    }
}

void PlasticityValidator::validateWeightChanges(const Synapse& synapse, double delta) {
    // Validate weight change magnitude
    SynapticWeight currentWeight = synapse.getWeight();
    SynapticWeight newWeight = currentWeight + static_cast<SynapticWeight>(delta);
    
    if (newWeight < -1.0f || newWeight > 1.0f) {
        std::map<std::string, std::string> context;
        context["synapse_id"] = std::to_string(synapse.getId().value);
        context["current_weight"] = std::to_string(currentWeight);
        context["weight_change"] = std::to_string(delta);
        context["new_weight"] = std::to_string(newWeight);
        context["valid_range"] = "[-1.0, 1.0]";
        
        throw PlasticityException(
            "Synapse weight change would exceed valid range",
            context,
            "PlasticityValidator"
        );
    }
}

void PlasticityValidator::handlePlasticityError(const std::exception& e) {
    try {
        throw;
    } catch (const BrainException& be) {
        LOG_ERROR("Plasticity system error: " + std::string(be.whatWithDetails()));
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected plasticity system error: " + std::string(e.what()));
        throw PlasticityException(
            "Unexpected error in plasticity system",
            {{ "error", std::string(e.what()) }},
            "PlasticityValidator"
        );
    }
}

} // namespace nlm
