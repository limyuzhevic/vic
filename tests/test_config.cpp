// Config Tests - Modern Google Test Version
#include "core/Config/Config.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class ConfigTests : public NLMTestBase {
public:
    void TestConfigCreation() {
        nlm::Config config;
        EXPECT_FALSE(config.has("test"));
    }
    
    void TestConfigSetAndGet() {
        nlm::Config config;
        
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        EXPECT_TRUE(config.has("int_value"));
        
        auto intVal = config.get<int>("int_value");
        ASSERT_TRUE(intVal.has_value());
        EXPECT_EQ(*intVal, 42);
        
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        auto doubleVal = config.get<double>("double_value");
        ASSERT_TRUE(doubleVal.has_value());
        EXPECT_FLOAT_EQ(*doubleVal, 3.14);
        
        config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
        auto stringVal = config.get<std::string>("string_value");
        ASSERT_TRUE(stringVal.has_value());
        EXPECT_EQ(*stringVal, "hello");
        
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        auto boolVal = config.get<bool>("bool_value");
        ASSERT_TRUE(boolVal.has_value());
        EXPECT_TRUE(*boolVal);
    }
    
    void TestConfigGetOr() {
        nlm::Config config;
        
        int val = config.getOr<int>("missing", 100);
        EXPECT_EQ(val, 100);
        
        config.set("existing", 50, nlm::ConfigSource::Runtime);
        val = config.getOr<int>("existing", 100);
        EXPECT_EQ(val, 50);
    }
    
    void TestConfigRemove() {
        nlm::Config config;
        
        config.set("test", 123, nlm::ConfigSource::Runtime);
        EXPECT_TRUE(config.has("test"));
        
        config.remove("test");
        EXPECT_FALSE(config.has("test"));
    }
    
    void TestConfigKeys() {
        nlm::Config config;
        
        config.set("key1", 1, nlm::ConfigSource::Runtime);
        config.set("key2", 2, nlm::ConfigSource::Runtime);
        config.set("key3", 3, nlm::ConfigSource::Runtime);
        
        auto keys = config.getKeys();
        EXPECT_EQ(keys.size(), 3);
    }
};

TEST_F(ConfigTests, TestConfigCreation) { TestConfigCreation(); }
TEST_F(ConfigTests, TestConfigSetAndGet) { TestConfigSetAndGet(); }
TEST_F(ConfigTests, TestConfigGetOr) { TestConfigGetOr(); }
TEST_F(ConfigTests, TestConfigRemove) { TestConfigRemove(); }
TEST_F(ConfigTests, TestConfigKeys) { TestConfigKeys(); }

} // namespace test
} // namespace nlm
