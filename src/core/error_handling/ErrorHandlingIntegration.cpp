#include "ErrorHandlingIntegration.hpp"
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/validation/Validator.hpp"
#include "exceptions/BrainException.hpp"

namespace nlm {

void ErrorHandlingIntegration::integrateErrorHandlingIntoBrain(Brain* brain) {
    if (!brain) {
        throw BrainInitializationException("Brain object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Integrating error handling into Brain system");
    
    // Set up error reporting
    brain->setErrorHandler([](const BrainException& e) {
        LOG_ERROR("Brain system error: " + std::string(e.whatWithDetails()));
    });
    
    // Set up error recovery
    brain->setErrorRecovery([](const BrainException& e) {
        auto mode = ErrorRecoveryManager::getRecoveryMode(e);
        return ErrorRecoveryManager::attemptRecovery(mode);
    });
    
    LOG_INFO("Error handling successfully integrated into Brain system");
}

void ErrorHandlingIntegration::integrateErrorHandlingIntoMemorySystem(MemorySystem* memory) {
    if (!memory) {
        throw BrainInitializationException("Memory system object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Integrating error handling into MemorySystem");
    
    // Set up memory validation
    memory->setMemoryValidator([](const void* ptr, size_t size) {
        Validator::validateMemoryAccess(ptr, size);
    });
    
    // Set up error handling
    memory->setErrorHandler([](const BrainException& e) {
        LOG_ERROR("Memory system error: " + std::string(e.whatWithDetails()));
    });
    
    LOG_INFO("Error handling successfully integrated into MemorySystem");
}

void ErrorHandlingIntegration::integrateErrorHandlingIntoPlasticitySystem(PlasticitySystem* plasticity) {
    if (!plasticity) {
        throw BrainInitializationException("Plasticity system object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Integrating error handling into PlasticitySystem");
    
    // Set up plasticity validation
    plasticity->setPlasticityValidator([](const PlasticityFlags& flags) {
        Validator::validatePlasticityCompatibility(flags, flags);
    });
    
    // Set up error handling
    plasticity->setErrorHandler([](const BrainException& e) {
        LOG_ERROR("Plasticity system error: " + std::string(e.whatWithDetails()));
    });
    
    LOG_INFO("Error handling successfully integrated into PlasticitySystem");
}

void ErrorHandlingIntegration::integrateErrorHandlingIntoNeuromodulationSystem(NeuromodulationSystem* neuromodulation) {
    if (!neuromodulation) {
        throw BrainInitializationException("Neuromodulation system object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Integrating error handling into NeuromodulationSystem");
    
    // Set up neuromodulation validation
    neuromodulation->setNeuromodulationValidator([](const Neuromodulator& signal) {
        Validator::validateNeuromodulatorLevels(signal);
    });
    
    // Set up error handling
    neuromodulation->setErrorHandler([](const BrainException& e) {
        LOG_ERROR("Neuromodulation system error: " + std::string(e.whatWithDetails()));
    });
    
    LOG_INFO("Error handling successfully integrated into NeuromodulationSystem");
}

void ErrorHandlingIntegration::integrateErrorHandlingIntoPredictionSystem(PredictionSystem* prediction) {
    if (!prediction) {
        throw BrainInitializationException("Prediction system object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Integrating error handling into PredictionSystem");
    
    // Set up prediction validation
    prediction->setPredictionValidator([](const std::vector<double>& predictions) {
        Validator::validatePredictionAccuracy(predictions);
    });
    
    // Set up error handling
    prediction->setErrorHandler([](const BrainException& e) {
        LOG_ERROR("Prediction system error: " + std::string(e.whatWithDetails()));
    });
    
    LOG_INFO("Error handling successfully integrated into PredictionSystem");
}

void ErrorHandlingIntegration::integrateErrorHandlingIntoDevelopmentSystem(DevelopmentSystem* development) {
    if (!development) {
        throw BrainInitializationException("Development system object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Integrating error handling into DevelopmentSystem");
    
    // Set up development validation
    development->setDevelopmentalStageValidator([](const DevelopmentalStage& stage) {
        Validator::validateDevelopmentalStage(stage);
    });
    
    // Set up error handling
    development->setErrorHandler([](const BrainException& e) {
        LOG_ERROR("Development system error: " + std::string(e.whatWithDetails()));
    });
    
    LOG_INFO("Error handling successfully integrated into DevelopmentSystem");
}

void ErrorHandlingIntegration::setupErrorRecoveryMechanisms(Brain* brain) {
    if (!brain) {
        throw BrainInitializationException("Brain object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Setting up error recovery mechanisms for Brain system");
    
    // Set up error recovery manager
    brain->setErrorRecoveryManager([](const BrainException& e) {
        return ErrorRecoveryManager::getRecoveryMode(e);
    });
    
    // Set up error recovery execution
    brain->setErrorRecoveryExecutor([](ErrorRecoveryManager::RecoveryMode mode, 
                                       const std::map<std::string, std::string>& context) {
        return ErrorRecoveryManager::attemptRecovery(mode, context);
    });
    
    // Set up state rollback
    brain->setStateRollback([](const std::string& operation) {
        ErrorRecoveryManager::rollbackState(operation);
    });
    
    LOG_INFO("Error recovery mechanisms successfully set up for Brain system");
}

void ErrorHandlingIntegration::setupErrorReporting(Brain* brain) {
    if (!brain) {
        throw BrainInitializationException("Brain object is null", "ErrorHandlingIntegration");
    }
    
    LOG_INFO("Setting up error reporting for Brain system");
    
    // Set up error reporter
    brain->setErrorReporter([](const BrainException& e) {
        LOG_ERROR("Brain system error report:\n" + 
                  UserFriendlyErrorMessages::formatErrorMessage(e));
    });
    
    // Set up health monitoring
    brain->setHealthMonitor([]() {
        SystemMonitor::generateHealthReport();
    });
    
    // Set up error correlation
    brain->setErrorCorrelator([](const std::string& error1, const std::string& error2) {
        SystemMonitor::correlateErrors(error1, error2);
    });
    
    LOG_INFO("Error reporting successfully set up for Brain system");
}

} // namespace nlm
