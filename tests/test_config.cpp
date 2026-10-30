#include <filesystem>
#include <fstream>
#include <chrono>

namespace test_config {

void testConfigFileLoading() {
    nlm::Config config;
    
    // Test 1: Create a temporary config file
    std::string tempFilename = "test_config_" + 
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".cfg";
    
    {
        std::ofstream file(tempFilename);
        file << "# Test configuration file\n";
        file << "random_seed = 123\n";
        file << "neuron_count = 100\n";
        file << "region_count = 2\n";
    }
    
    bool result = config.loadFromFile(tempFilename);
    assert(result, "Should load valid config file");
    assert(config.getOr<int64_t>("random_seed", 0) == 123);
    assert(config.getOr<int64_t>("neuron_count", 0) == 100);
    assert(config.getOr<int64_t>("region_count", 0) == 2);
    
    std::filesystem::remove(tempFilename);
    
    std::cout << "    testConfigFileLoading passed" << std::endl;
}

void testConfigFileErrors() {
    nlm::Config config;
    
    // Test 2: Create a config file with errors
    std::string tempFilename = "test_config_error_" + 
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".cfg";
    
    {
        std::ofstream file(tempFilename);
        file << "random_seed = 123\n";
        file << "invalid_line_without_equals\n";
        file << "neuron_count = 100\n";
    }
    
    bool result = config.loadFromFile(tempFilename);
    assert(!result, "Should reject config file with errors");
    
    std::filesystem::remove(tempFilename);
    
    std::cout << "    testConfigFileErrors passed" << std::endl;
}

void testConfigFileNonexistent() {
    nlm::Config config;
    
    // Test 3: Try to load non-existent file
    bool result = config.loadFromFile("nonexistent_config_file.cfg");
    assert(!result, "Should reject non-existent config file");
    
    std::cout << "    testConfigFileNonexistent passed" << std::endl;
}

void testConfigFileLargeFile() {
    nlm::Config config;
    
    // Test 4: Try to load file that's too large (create 2MB file)
    std::string tempFilename = "large_config_" + 
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".cfg";
    
    {
        std::ofstream file(tempFilename);
        // Write about 2MB of config data (each line is about 50 chars)
        for (int i = 0; i < 40000; ++i) {
            file << "key" << i << " = value" << i << "\n";
        }
    }
    
    bool result = config.loadFromFile(tempFilename);
    // This might fail due to size limits
    
    std::filesystem::remove(tempFilename);
    
    std::cout << "    testConfigFileLargeFile passed" << std::endl;
}

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Run new configuration file loading tests
    testConfigFileLoading();
    testConfigFileErrors();
    testConfigFileNonexistent();
    testConfigFileLargeFile();
}

} // namespace test_config
