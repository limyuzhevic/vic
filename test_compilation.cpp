// Simple compilation test to verify error handling includes
#include "src/core/ErrorHandling/ErrorHandling.hpp"
#include "src/core/ErrorHandling/ErrorHandlingUtilities.hpp"

namespace nlm {
    void testErrorHandlingCompilation() {
        // Test that error handling infrastructure compiles correctly
        ErrorCode testCode = ErrorCode::Success;
        NLMException testException(ErrorCode::GeneralError, "Test message");
        
        Result<int> testResult(42);
        if (!testResult.success()) {
            testResult.throwIfFailed();
        }
        
        ErrorHandler errorHandler;
        errorHandler.setContext("Test", "Test operation");
        errorHandler.addContext("param1", "value1");
        
        ErrorContext context("TestContext");
        context.addParameter("int", 42);
        context.addParameter("float", 3.14f);
        context.addParameter("string", std::string("hello"));
        
        std::string formatted = context.getFormattedContext();
        
        // Test error recovery
        ErrorRecovery::RecoveryAction action = ErrorRecovery::getRecoveryAction(ErrorCode::MemoryAllocationFailed);
        std::string strategy = ErrorRecovery::getRecoveryStrategy(ErrorCode::MemoryAllocationFailed);
    }
}

// Test includes all necessary headers
#include "src/core/Logger/Logger.hpp"
#include "src/core/Config/ConfigBuilder.hpp"
#include "src/brain/Brain.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"

int main() {
    return 0;
}
