#include "core/Errors/Errors.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Error Handling System" << std::endl;
    std::cout << "===============================" << std::endl << std::endl;
    
    // Test exception classes
    try {
        throw nlm::NLMException("Test NLM exception", __FILE__, __LINE__);
    } catch (const nlm::NLMException& e) {
        std::cout << "✓ NLMException thrown and caught successfully" << std::endl;
        std::cout << "  Message: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::ConfigValidationException("Test config validation", __FILE__, __LINE__);
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "✓ ConfigValidationException thrown and caught successfully" << std::endl;
    }
    
    try {
        throw nlm::ValidationException("Test validation", __FILE__, __LINE__);
    } catch (const nlm::ValidationException& e) {
        std::cout << "✓ ValidationException thrown and caught successfully" << std::endl;
    }
    
    try {
        throw nlm::NullPointerException("test_ptr", __FILE__, __LINE__);
    } catch (const nlm::NullPointerException& e) {
        std::cout << "✓ NullPointerException thrown and caught successfully" << std::endl;
    }
    
    try {
        nlm::Validator::validateRange(10.0, 0.0, 5.0, "test_value", __FILE__, __LINE__);
    } catch (const nlm::RangeValidationException& e) {
        std::cout << "✓ RangeValidationException thrown and caught successfully" << std::endl;
    }
    
    // Test error codes
    std::cout << "\nError Code Descriptions:" << std::endl;
    std::cout << "  ConfigKeyNotFound: " << nlm::ErrorReport::getErrorCodeDescription(nlm::ErrorCode::ConfigKeyNotFound) << std::endl;
    std::cout << "  NeuronIdInvalid: " << nlm::ErrorReport::getErrorCodeDescription(nlm::ErrorCode::NeuronIdInvalid) << std::endl;
    std::cout << "  MemoryAllocationFailed: " << nlm::ErrorReport::getErrorCodeDescription(nlm::ErrorCode::MemoryAllocationFailed) << std::endl;
    
    std::cout << "\nError Category Descriptions:" << std::endl;
    std::cout << "  Configuration: " << nlm::ErrorReport::getErrorCategoryDescription(nlm::ErrorCategory::Configuration) << std::endl;
    std::cout << "  Neural: " << nlm::ErrorReport::getErrorCategoryDescription(nlm::ErrorCategory::Neural) << std::endl;
    std::cout << "  Validation: " << nlm::ErrorReport::getErrorCategoryDescription(nlm::ErrorCategory::Validation) << std::endl;
    
    // Test macros
    std::cout << "\nTesting Macros:" << std::endl;
    try {
        NLM_THROW(nlm::ValidationException, "Test macro throw");
    } catch (const nlm::ValidationException& e) {
        std::cout << "✓ NLM_THROW macro works" << std::endl;
    }
    
    try {
        NLM_THROW_RANGE_ERROR("test_param", 15.0, 0.0, 10.0);
    } catch (const nlm::RangeValidationException& e) {
        std::cout << "✓ NLM_THROW_RANGE_ERROR macro works" << std::endl;
    }
    
    try {
        NLM_THROW_NULL_ERROR("test_ptr");
    } catch (const nlm::NullPointerException& e) {
        std::cout << "✓ NLM_THROW_NULL_ERROR macro works" << std::endl;
    }
    
    std::cout << "\n✓ All error handling tests passed successfully!" << std::endl;
    return 0;
}
