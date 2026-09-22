// Development System Tests - Modern Google Test Version
#include "development/DevelopmentSystem.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class DevelopmentTests : public NLMTestBase {
public:
    void TestDevelopmentSystemCreation() {
        DevelopmentSystem dev;
        EXPECT_EQ(dev.getStage(), DevelopmentalStage::Embryo);
        EXPECT_EQ(dev.getStageName(), std::string("Embryo"));
    }
    
    void TestMaturation() {
        Maturation maturation;
        EXPECT_FLOAT_EQ(maturation.getProgress(), 0.0f);
        
        maturation.setProgress(0.5f);
        EXPECT_FLOAT_EQ(maturation.getProgress(), 0.5f);
    }
    
    void TestSynaptogenesis() {
        Synaptogenesis synaptogenesis;
        EXPECT_FLOAT_EQ(synaptogenesis.getFormationRate(), 0.0f);
        EXPECT_FLOAT_EQ(synaptogenesis.getTargetDensity(), 0.0f);
        
        synaptogenesis.setFormationRate(0.1f);
        synaptogenesis.setTargetDensity(0.2f);
        
        EXPECT_FLOAT_EQ(synaptogenesis.getFormationRate(), 0.1f);
        EXPECT_FLOAT_EQ(synaptogenesis.getTargetDensity(), 0.2f);
    }
};

TEST_F(DevelopmentTests, TestDevelopmentSystemCreation) { TestDevelopmentSystemCreation(); }
TEST_F(DevelopmentTests, TestMaturation) { TestMaturation(); }
TEST_F(DevelopmentTests, TestSynaptogenesis) { TestSynaptogenesis(); }

} // namespace test
} // namespace nlm

