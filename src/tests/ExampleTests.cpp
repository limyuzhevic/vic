// Example unit test demonstrating the improved NLM test framework

#include "TestFramework.hpp"
#include <cmath>

namespace nlm {

class VectorUtilsTest : public Test {
public:
    std::string getName() const override { return "VectorUtilsTest"; }
    std::string getDescription() const override { return "Tests for VectorUtils functionality"; }
    
    TestResult run() override {
        TestCase testCase(std::make_shared<VectorUtilsTest>(this));
        
        std::vector<float> v1 = {1.0f, 2.0f, 3.0f};
        std::vector<float> v2 = {4.0f, 5.0f, 6.0f};
        
        // Test euclidean distance
        float distance = VectorUtils::euclideanDistance(v1, v2);
        float expectedDistance = std::sqrt(50.0f);
        testCase.EXPECT_EQ(distance, expectedDistance, 1e-6f, "Euclidean distance calculation");
        testCase.log("Calculated Euclidean distance: " + std::to_string(distance));
        
        // Test cosine similarity
        float similarity = VectorUtils::cosineSimilarity(v1, v2);
        float expectedSimilarity = (1.0f*4.0f + 2.0f*5.0f + 3.0f*6.0f) / (std::sqrt(14.0f) * std::sqrt(77.0f));
        testCase.EXPECT_EQ(similarity, expectedSimilarity, 1e-6f, "Cosine similarity calculation");
        testCase.log("Calculated cosine similarity: " + std::to_string(similarity));
        
        // Test normalization
        std::vector<float> normalized = VectorUtils::normalize(v1);
        std::vector<float> expectedNormalized = {1.0f/std::sqrt(14.0f), 2.0f/std::sqrt(14.0f), 3.0f/std::sqrt(14.0f)};
        for (size_t i = 0; i < normalized.size(); ++i) {
            testCase.EXPECT_EQ(normalized[i], expectedNormalized[i], 1e-6f, "Vector normalization");
        }
        testCase.log("Vector normalization successful");
        
        // Test clamping
        std::vector<float> clamped = VectorUtils::clamp(v1, 1.5f, 2.5f);
        std::vector<float> expectedClamped = {1.5f, 2.0f, 2.5f};
        for (size_t i = 0; i < clamped.size(); ++i) {
            testCase.EXPECT_EQ(clamped[i], expectedClamped[i], 1e-6f, "Vector clamping");
        }
        testCase.log("Vector clamping successful");
        
        // Test mean
        float mean = VectorUtils::mean(v1);
        testCase.EXPECT_EQ(mean, 2.0f, 1e-6f, "Vector mean calculation");
        testCase.log("Mean calculation: " + std::to_string(mean));
        
        // Test standard deviation
        float stddev = VectorUtils::standardDeviation(v1);
        float expectedStddev = std::sqrt(0.6666666f);  // Variance = (2/3), stddev = sqrt(2/3)
        testCase.EXPECT_EQ(stddev, expectedStddev, 1e-6f, "Vector standard deviation calculation");
        testCase.log("Standard deviation calculation: " + std::to_string(stddev));
        
        // Test equality
        std::vector<float> v3 = {1.0f, 2.0f, 3.0f};
        bool equal = VectorUtils::isEqual(v1, v3, 1e-6f);
        testCase.EXPECT_TRUE(equal, "Vector equality check");
        testCase.log("Vector equality check: " + std::to_string(equal));
        
        return testCase.getResult();
    }
};

class MathUtilsTest : public Test {
public:
    std::string getName() const override { return "MathUtilsTest"; }
    std::string getDescription() const override { return "Tests for mathematical utility functions"; }
    
    TestResult run() override {
        TestCase testCase(std::make_shared<MathUtilsTest>(this));
        
        // Test Pearson correlation
        std::vector<float> x = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        std::vector<float> y = {2.0f, 4.0f, 6.0f, 8.0f, 10.0f};
        
        float correlation = StatsUtils::pearsonCorrelation(x, y);
        testCase.EXPECT_EQ(correlation, 1.0f, 1e-6f, "Perfect positive correlation");
        testCase.log("Pearson correlation: " + std::to_string(correlation));
        
        // Test coefficient of variation
        std::vector<float> data = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f};
        float cv = StatsUtils::coefficientOfVariation(data);
        testCase.EXPECT_GT(cv, 0.0f, "Coefficient of variation should be positive");
        testCase.log("Coefficient of variation: " + std::to_string(cv));
        
        // Test median
        std::vector<float> sorted = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        float median = StatsUtils::median(sorted);
        testCase.EXPECT_EQ(median, 3.0f, 1e-6f, "Median calculation");
        testCase.log("Median: " + std::to_string(median));
        
        // Test percentile
        float p90 = StatsUtils::percentile(sorted, 90.0f);
        testCase.EXPECT_EQ(p90, 4.6f, 1e-6f, "90th percentile calculation");
        testCase.log("90th percentile: " + std::to_string(p90));
        
        return testCase.getResult();
    }
};

class StringUtilsTest : public Test {
public:
    std::string getName() const override { return "StringUtilsTest"; }
    std::string getDescription() const override { return "Tests for string utility functions"; }
    
    TestResult run() override {
        TestCase testCase(std::make_shared<StringUtilsTest>(this));
        
        // Test format
        std::string formatted = StringUtils::format("Hello %s, age %d", "World", 25);
        testCase.EXPECT_TRUE(StringUtils::contains(formatted, "Hello"), "String formatting");
        testCase.EXPECT_TRUE(StringUtils::contains(formatted, "World"), "String formatting");
        testCase.EXPECT_TRUE(StringUtils::contains(formatted, "25"), "String formatting");
        testCase.log("Formatted string: " + formatted);
        
        // Test split
        std::string testStr = "apple,banana,cherry";
        std::vector<std::string> split = StringUtils::split(testStr, ',');
        testCase.EXPECT_TRUE(split.size() == 3, "String split");
        testCase.EXPECT_TRUE(split[0] == "apple", "String split first element");
        testCase.EXPECT_TRUE(split[1] == "banana", "String split second element");
        testCase.EXPECT_TRUE(split[2] == "cherry", "String split third element");
        testCase.log("String split successful");
        
        // Test trim
        std::string trimmed = StringUtils::trim("  hello world  ");
        testCase.EXPECT_EQ(trimmed, "hello world", "String trim");
        testCase.log("Trimmed string: " + trimmed);
        
        // Test toLower
        std::string lower = StringUtils::toLower("HELLO WORLD");
        testCase.EXPECT_EQ(lower, "hello world", "String toLower");
        testCase.log("Lowercase string: " + lower);
        
        // Test toUpper
        std::string upper = StringUtils::toUpper("hello world");
        testCase.EXPECT_EQ(upper, "HELLO WORLD", "String toUpper");
        testCase.log("Uppercase string: " + upper);
        
        // Test replaceAll
        std::string replaced = StringUtils::replaceAll("hello world hello", "hello", "hi");
        testCase.EXPECT_TRUE(StringUtils::contains(replaced, "hi world hi"), "String replaceAll");
        testCase.log("Replaced string: " + replaced);
        
        return testCase.getResult();
    }
};

class RandomUtilsTest : public Test {
public:
    std::string getName() const override { return "RandomUtilsTest"; }
    std::string getDescription() const override { return "Tests for random number generation utilities"; }
    
    TestResult run() override {
        TestCase testCase(std::make_shared<RandomUtilsTest>(this));
        
        // Test random float range
        float r1 = RandomUtils::randomFloat(0.0f, 1.0f);
        testCase.EXPECT_TRUE(r1 >= 0.0f && r1 <= 1.0f, "Random float in range");
        testCase.log("Random float: " + std::to_string(r1));
        
        // Test random integer range
        int r2 = RandomUtils::randomInt(1, 10);
        testCase.EXPECT_TRUE(r2 >= 1 && r2 <= 10, "Random integer in range");
        testCase.log("Random integer: " + std::to_string(r2));
        
        // Test random boolean
        bool r3 = RandomUtils::randomBool(0.5f);
        testCase.EXPECT_TRUE(r3 == true || r3 == false, "Random boolean");
        testCase.log("Random boolean: " + std::to_string(r3));
        
        // Test random permutation
        std::vector<size_t> perm = RandomUtils::randomPermutation(5);
        testCase.EXPECT_TRUE(perm.size() == 5, "Random permutation size");
        
        bool allUnique = true;
        for (size_t i = 0; i < perm.size(); ++i) {
            for (size_t j = i + 1; j < perm.size(); ++j) {
                if (perm[i] == perm[j]) {
                    allUnique = false;
                    break;
                }
            }
        }
        testCase.EXPECT_TRUE(allUnique, "Random permutation uniqueness");
        testCase.log("Random permutation created");
        
        return testCase.getResult();
    }
};

} // namespace nlm

// Register test suites
NLM_TEST_SUITE("VectorUtilsTest", "Tests for VectorUtils functionality")
NLM_TEST_SUITE("MathUtilsTest", "Tests for mathematical utility functions")
NLM_TEST_SUITE("StringUtilsTest", "Tests for string utility functions")
NLM_TEST_SUITE("RandomUtilsTest", "Tests for random number generation utilities")

// Entry point for running the tests
int main() {
    std::cout << "Running NLM Test Suite\n";
    std::cout << "======================\n\n";
    
    // Get all test suites
    auto testSuites = nlm::TestFactory::getAllTestSuites();
    
    // Run all tests
    std::vector<nlm::TestResult> allResults;
    
    for (const auto& suite : testSuites) {
        auto results = suite->runAll();
        allResults.insert(allResults.end(), results.begin(), results.end());
    }
    
    // Display final results
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "TEST SUMMARY\n";
    std::cout << std::string(70, '=') << "\n";
    
    int passed = 0, failed = 0;
    for (const auto& result : allResults) {
        if (result.passed) passed++; else failed++;
    }
    
    std::cout << "Total tests: " << allResults.size() << "\n";
    std::cout << "Passed: " << passed << "\n";
    std::cout << "Failed: " << failed << "\n";
    
    if (failed > 0) {
        std::cout << "\nFailed tests:\n";
        for (const auto& result : allResults) {
            if (!result.passed) {
                std::cout << "  - " << result.testName << ": " << result.errorMessage << "\n";
            }
        }
        return 1;
    }
    
    std::cout << "\nAll tests passed!\n";
    return 0;
}
