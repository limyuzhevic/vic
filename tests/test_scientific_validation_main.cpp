// NLM Scientific Validation Test Main
// Phase 6 Comprehensive Scientific Testing

#include "test_scientific_validation.h"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Scientific Validation Tests ===" << std::endl;
    std::cout << "Running comprehensive scientific validation tests for the NLM brain simulation..." << std::endl;
    std::cout << "Based on Phase 6 audit requirements:" << std::endl;
    std::cout << "  - Memory retention over time tests" << std::endl;
    std::cout << "  - Learning progress measurements" << std::endl;
    std::cout << "  - Continual learning without catastrophic forgetting" << std::endl;
    std::cout << "  - Generalization testing" << std::endl;
    std::cout << "  - Prediction accuracy improvement tracking" << std::endl;
    std::cout << "  - Individual differences from different histories" << std::endl;
    std::cout << std::endl;
    
    test_scientific_validation::ScientificValidationTestSuite testSuite;
    testSuite.runAllTests();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Scientific Validation Tests Completed Successfully ===" << std::endl;
    
    return 0;
}
