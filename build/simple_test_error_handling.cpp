#include <iostream>
#include <cassert>
#include "src/core/Error/NLMErrors.hpp"

int main() {
    std::cout << "=== Simple NLM Error Handling Test ===" << std::endl;
    
    try {
        // Test 1: Create a validation error
        nlm::ValidationError valError("Test validation error", "ValidationTest");
        std::cout << "✓ Validation error creation works: " << valError.what() << std::endl;
        
        // Test 2: Create a resource error
        nlm::ResourceError resError("Test resource error", "ResourceTest");
        std::cout << "✓ Resource error creation works: " << resError.what() << std::endl;
        
        // Test 3: Create an IO error
        nlm::IOError ioError("Test IO error", "IOTest");
        std::cout << "✓ IO error creation works: " << ioError.what() << std::endl;
        
        // Test 4: Create a runtime error
        nlm::RuntimeError runtimeError("Test runtime error", "RuntimeTest");
        std::cout << "✓ Runtime error creation works: " << runtimeError.what() << std::endl;
        
        // Test 5: Create a configuration error
        nlm::ConfigurationError configError("Test config error", "ConfigTest");
        std::cout << "✓ Configuration error creation works: " << configError.what() << std::endl;
        
        // Test 6: Verify error messages
        std::string valMsg = valError.getFullMessage();
        assert(valMsg.find("ValidationTest") != std::string::npos);
        assert(valMsg.find("1001") != std::string::npos);
        std::cout << "✓ Validation error message format correct" << std::endl;
        
        std::string resMsg = resError.getFullMessage();
        assert(resMsg.find("ResourceTest") != std::string::npos);
        assert(resMsg.find("2001") != std::string::npos);
        std::cout << "✓ Resource error message format correct" << std::endl;
        
        std::string ioMsg = ioError.getFullMessage();
        assert(ioMsg.find("IOTest") != std::string::npos);
        assert(ioMsg.find("3001") != std::string::npos);
        std::cout << "✓ IO error message format correct" << std::endl;
        
        std::string runtimeMsg = runtimeError.getFullMessage();
        assert(runtimeMsg.find("RuntimeTest") != std::string::npos);
        assert(runtimeMsg.find("4001") != std::string::npos);
        std::cout << "✓ Runtime error message format correct" << std::endl;
        
        std::string configMsg = configError.getFullMessage();
        assert(configMsg.find("ConfigTest") != std::string::npos);
        assert(configMsg.find("5001") != std::string::npos);
        std::cout << "✓ Configuration error message format correct" << std::endl;
        
        // Test 7: Test error handler functionality
        nlm::ErrorHandler errorHandler;
        errorHandler.setThrowOnError(false);  // Don't throw, just collect
        
        errorHandler.addError(std::make_shared<nlm::ValidationError>("Handler test", "HandlerTest"));
        assert(errorHandler.hasErrors());
        assert(errorHandler.getErrors().size() == 1);
        std::cout << "✓ Error handler collection works" << std::endl;
        
        errorHandler.clear();
        assert(!errorHandler.hasErrors());
        std::cout << "✓ Error handler clearing works" << std::endl;
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        std::cout << "Error handling framework is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n*** Test failed with exception: " << e.what() << " ***" << std::endl;
        return 1;
    }
    
    return 0;
}
