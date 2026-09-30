#include "DevelopmentalValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void DevelopmentalValidator::validateDevelopmentalStage(const DevelopmentalStage& stage) {
    // Validate that developmental stage is within valid range
    if (static_cast<int>(stage) < static_cast<int>(DevelopmentalStage::Initial) ||
        static_cast<int>(stage) > static_cast<int>(DevelopmentalStage::Aging)) {
        std::map<std::string, std::string> context;
        context["stage"] = std::to_string(static_cast<int>(stage));
        context["valid_stages"] = "0-4 (Initial, CriticalPeriod, Maturation, Adult, Aging)";
        
        throw BrainInitializationException(
            "Invalid developmental stage",
            context,
            "DevelopmentalValidator"
        );
    }
}

void DevelopmentalValidator::validateStageTransition(const DevelopmentalStage& from,
                                                     const DevelopmentalStage& to) {
    // Validate that stage transition is valid
    if (to < from) {
        throw BrainInitializationException(
            "Developmental stage cannot regress",
            {{ "from_stage", std::to_string(static_cast<int>(from)) },
             { "to_stage", std::to_string(static_cast<int>(to)) }},
            "DevelopmentalValidator"
        );
    }
    
    // Validate specific stage transitions
    if (from == DevelopmentalStage::Initial && to == DevelopmentalStage::CriticalPeriod) {
        // Valid transition
    } else if (from == DevelopmentalStage::CriticalPeriod && to == DevelopmentalStage::Maturation) {
        // Valid transition
    } else if (from == DevelopmentalStage::Maturation && to == DevelopmentalStage::Adult) {
        // Valid transition
    } else if (from == DevelopmentalStage::Adult && to == DevelopmentalStage::Aging) {
        // Valid transition
    } else {
        std::map<std::string, std::string> context;
        context["from_stage"] = std::to_string(static_cast<int>(from));
        context["to_stage"] = std::to_string(static_cast<int>(to));
        context["valid_transitions"] = "Initial->CriticalPeriod, CriticalPeriod->Maturation, "
                                      "Maturation->Adult, Adult->Aging";
        
        throw BrainInitializationException(
            "Invalid developmental stage transition",
            context,
            "DevelopmentalValidator"
        );
    }
}

void DevelopmentalValidator::validateCriticalPeriod(const std::map<std::string, double>& period) {
    // Validate critical period parameters
    if (period.find("start_time") != period.end()) {
        double start = period.at("start_time");
        Validator::validateRange(start, 0.0, 1e6, "critical_period_start");
    }
    
    if (period.find("end_time") != period.end()) {
        double end = period.at("end_time");
        Validator::validateRange(end, 0.0, 1e6, "critical_period_end");
    }
    
    if (period.find("duration") != period.end()) {
        double duration = period.at("duration");
        Validator::validateRange(duration, 0.0, 1e6, "critical_period_duration");
    }
    
    // Validate that end_time > start_time if both are provided
    if (period.find("start_time") != period.end() && period.find("end_time") != period.end()) {
        double start = period.at("start_time");
        double end = period.at("end_time");
        if (end <= start) {
            throw BrainInitializationException(
                "Critical period end time must be after start time",
                {{ "start_time", std::to_string(start) },
                 { "end_time", std::to_string(end) }},
                "DevelopmentalValidator"
            );
        }
    }
}

void DevelopmentalValidator::handleDevelopmentalError(const std::exception& e) {
    try {
        throw;
    } catch (const BrainException& be) {
        LOG_ERROR("Developmental system error: " + std::string(be.whatWithDetails()));
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected developmental system error: " + std::string(e.what()));
        throw BrainInitializationException(
            "Unexpected error in developmental system",
            {{ "error", std::string(e.what()) }},
            "DevelopmentalValidator"
        );
    }
}

} // namespace nlm
