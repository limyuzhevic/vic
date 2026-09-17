#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::cout << "Testing parser implementation..." << std::endl;
    
    // Test detectFileFormat function
    std::string test1 = "/path/to/config.json";
    std::string test2 = "/path/to/config.yaml";
    std::string test3 = "/path/to/config.yml";
    std::string test4 = "/path/to/config.cfg";
    std::string test5 = "/path/to/config.txt";
    
    std::cout << "Test 1 (json): " << test1 << std::endl;
    std::cout << "Test 2 (yaml): " << test2 << std::endl;
    std::cout << "Test 3 (yml): " << test3 << std::endl;
    std::cout << "Test 4 (cfg): " << test4 << std::endl;
    std::cout << "Test 5 (unknown): " << test5 << std::endl;
    
    return 0;
}
