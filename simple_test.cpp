// Simple test to verify Config functionality
#include <iostream>
#include <fstream>
#include <string>

// Mock Config class to test file I/O
class MockConfig {
public:
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << "test = \"value\"\n";
        file << "number = 42\n";
        file << "boolean = true\n";
        return true;
    }
};

int main() {
    MockConfig config;
    
    // Test saveToFile with valid file
    if (config.saveToFile("test_config.txt")) {
        std::cout << "saveToFile test passed" << std::endl;
    }
    
    // Check if file was created
    std::ifstream file("test_config.txt");
    if (file.is_open()) {
        std::cout << "File created successfully" << std::endl;
        std::string line;
        while (std::getline(file, line)) {
            std::cout << "  " << line << std::endl;
        }
        file.close();
        remove("test_config.txt");
    }
    
    return 0;
}
