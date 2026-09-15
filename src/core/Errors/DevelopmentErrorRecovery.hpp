#include "ErrorSystem.hpp"
#include "DevelopmentSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <functional>

namespace nlm {

// Development error factory function
inline DevelopmentError developmentError(ErrorCode code, const std::string& message,
                                         const ErrorContext& ctx = makeErrorContext("Development", "update")) {
    return DevelopmentError(code, message, ctx);
}

// Error recovery for development
class DevelopmentErrorRecovery {
public:
    static bool recoverDevelopmentError(const NLMException& error, DevelopmentSystem& devSystem) {
        ErrorCode code = error.getCode();
        
        switch (code) {
            case ErrorCode::DevelopmentInvalidStage:
                // Try to reset to initial stage
                devSystem.setStage(DevelopmentalStage::Initial);
                return true;
                
            case ErrorCode::MaturationFailed:
            case ErrorCode::PruningFailed:
            case ErrorCode::SynaptogenesisFailed:
                // Try to skip this development step
                // Implementation would depend on the specific error
                return true;
                
            case ErrorCode::DevelopmentProcessTimeout:
                // Try to continue with default parameters
                devSystem.setDevelopmentalAge(devSystem.getDevelopmentalAge() - 1000);
                return true;
                
            default:
                // For other errors, just report
                return false;
        }
    }
};

} // namespace nlm