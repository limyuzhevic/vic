// NLM Enhanced Test Isolation Implementation
// Phase 7: Comprehensive Test Suite

#include "test_infrastructure/base/test_isolation.hpp"

// Global test isolation controller
TestIsolationController global_test_isolation(TestEnvironment("Default Environment", IsolationMode::INDEPENDENT));

// Test environment implementation
TestEnvironment::TestEnvironment(const std::string& env_name, IsolationMode default_mode)
    : environment_name(env_name), default_isolation_mode(default_mode) {
    
    // Set up default environment configuration
    system_paths["TEMP"] = "/tmp/nlm_tests";
    system_paths["LOGS"] = "/var/log/nlm";
    system_paths["DATA"] = "/var/lib/nlm";
    
    library_paths["LD_LIBRARY_PATH"] = "/usr/local/lib:/usr/lib";
    library_paths["PYTHONPATH"] = "/usr/local/lib/python3.10";
    
    configuration_files["DEFAULT"] = "configs/default.cfg";
    
    allowed_files = {
        "*.log", "*.txt", "*.json", "*.xml", "*.csv", "*.cfg", "*.conf"
    };
    
    allowed_directories = {
        "/tmp/nlm_tests", "/var/log/nlm", "/var/lib/nlm"
    };
    
    forbidden_operations = {
        "write_to_root", "modify_system", "access_network", "execute_external"
    };
}

// Test resource manager implementation
void TestResourceManager::cleanup_resources(const std::string& resource_name) {
    std::lock_guard<std::mutex> lock(resource_mutex);
    
    auto it = allocated_resources.find(resource_name);
    if (it != allocated_resources.end()) {
        for (void* ptr : it->second) {
            // Use type-erased deletion
            delete reinterpret_cast<void*>(ptr);
        }
        allocated_resources.erase(it);
        resource_metadata.erase(resource_name);
    }
}

void TestResourceManager::cleanup_all_resources() {
    std::lock_guard<std::mutex> lock(resource_mutex);
    
    for (auto& pair : allocated_resources) {
        for (void* ptr : pair.second) {
            delete reinterpret_cast<void*>(ptr);
        }
    }
    
    allocated_resources.clear();
    resource_metadata.clear();
    total_resources_allocated = 0;
    total_memory_allocated = 0;
}

void TestResourceManager::print_resource_report() const {
    std::cout << "=== Test Resource Management Report ===" << std::endl;
    std::cout << "Total Resources Allocated: " << total_resources_allocated << std::endl;
    std::cout << "Total Memory Allocated: " << total_memory_allocated << " bytes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Resource Types:" << std::endl;
    for (const auto& pair : allocated_resources) {
        std::cout << "  " << pair.first << ": " << pair.second.size() << " instances" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Resource Metadata:" << std::endl;
    for (const auto& pair : resource_metadata) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

// Test isolation controller implementation
TestIsolationController::TestIsolationController(const TestEnvironment& environment) 
    : test_environment(environment) {
    // Initialize with default setup/teardown functions
    setup_functions["global_setup"] = [this]() {
        std::cout << "[ISOLATION] Global setup completed" << std::endl;
    };
    
    teardown_functions["global_teardown"] = [this]() {
        std::cout << "[ISOLATION] Global teardown completed" << std::endl;
    };
    
    validation_functions["global_validation"] = [this]() {
        return true;
    };
}

void TestIsolationController::cleanup_test(const std::string& test_name) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        TestState& state = it->second;
        
        // Cleanup resources
        resource_manager.cleanup_resources(test_name);
        
        // Reset state
        state.status = TestStatus::NOT_STARTED;
        state.setup_completed = false;
        state.teardown_completed = false;
        state.memory_allocated = 0;
        state.peak_memory = 0;
        state.file_handles_open = 0;
        state.open_files.clear();
        state.network_connections.clear();
        
        std::cout << "[ISOLATION] Test " << test_name << " cleaned up" << std::endl;
    }
}

void TestIsolationController::cleanup_all_tests() {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    for (auto& pair : test_states) {
        cleanup_test(pair.first);
    }
    
    resource_manager.cleanup_all_resources();
    
    std::cout << "[ISOLATION] All tests cleaned up" << std::endl;
}

void TestIsolationController::add_dependency(const std::string& test_name, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        it->second.dependencies.push_back(dependency);
        
        auto dep_it = test_states.find(dependency);
        if (dep_it != test_states.end()) {
            dep_it->second.dependents.push_back(test_name);
        }
    }
}

void TestIsolationController::add_tag(const std::string& test_name, const std::string& tag) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        it->second.tags.push_back(tag);
    }
}

std::vector<std::string> TestIsolationController::get_tests_by_tag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    std::vector<std::string> tagged_tests;
    for (const auto& pair : test_states) {
        if (std::find(pair.second.tags.begin(), pair.second.tags.end(), tag) != pair.second.tags.end()) {
            tagged_tests.push_back(pair.first);
        }
    }
    
    return tagged_tests;
}

std::vector<std::string> TestIsolationController::get_tests_by_status(TestStatus status) const {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    std::vector<std::string> status_tests;
    for (const auto& pair : test_states) {
        if (pair.second.status == status) {
            status_tests.push_back(pair.first);
        }
    }
    
    return status_tests;
}

std::vector<std::string> TestIsolationController::get_tests_by_isolation_mode(IsolationMode mode) const {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    std::vector<std::string> mode_tests;
    for (const auto& pair : test_states) {
        if (pair.second.mode == mode) {
            mode_tests.push_back(pair.first);
        }
    }
    
    return mode_tests;
}

void TestIsolationController::print_test_summary() const {
    std::cout << "=== Test Isolation Summary ===" << std::endl;
    std::cout << "Total Tests: " << test_states.size() << std::endl;
    std::cout << "Resource Manager Stats:" << std::endl;
    resource_manager.print_resource_report();
    std::cout << std::endl;
    
    std::cout << "Test Status Distribution:" << std::endl;
    std::map<TestStatus, int> status_counts;
    for (const auto& pair : test_states) {
        status_counts[pair.second.status]++;
    }
    
    for (const auto& pair : status_counts) {
        std::string status_name = get_test_status_string(pair.first);
        std::cout << "  " << status_name << ": " << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Isolation Mode Distribution:" << std::endl;
    std::map<IsolationMode, int> mode_counts;
    for (const auto& pair : test_states) {
        mode_counts[pair.second.mode]++;
    }
    
    for (const auto& pair : mode_counts) {
        std::string mode_name = get_isolation_mode_string(pair.first);
        std::cout << "  " << mode_name << ": " << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Test Tags:" << std::endl;
    std::map<std::string, int> tag_counts;
    for (const auto& pair : test_states) {
        for (const auto& tag : pair.second.tags) {
            tag_counts[tag]++;
        }
    }
    
    for (const auto& pair : tag_counts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void TestIsolationController::track_allocation(const std::string& test_name, const std::string& resource_type, void* ptr, size_t size) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        TestState& state = it->second;
        state.memory_allocated += size;
        state.peak_memory = std::max(state.peak_memory, state.memory_allocated);
        
        std::cout << "[ISOLATION] Resource allocated for " << test_name 
                  << " (type: " << resource_type << ", size: " << size << " bytes)" << std::endl;
    }
}

void TestIsolationController::track_deallocation(const std::string& test_name, void* ptr) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        TestState& state = it->second;
        state.memory_allocated -= 16; // Simplified
        
        std::cout << "[ISOLATION] Resource deallocated for " << test_name << std::endl;
    }
}

void TestIsolationController::track_file_operation(const std::string& test_name, const std::string& filename, const std::string& operation) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        TestState& state = it->second;
        
        if (operation == "open") {
            state.file_handles_open++;
            state.open_files.push_back(filename);
        } else if (operation == "close") {
            state.file_handles_open--;
            auto file_it = std::find(state.open_files.begin(), state.open_files.end(), filename);
            if (file_it != state.open_files.end()) {
                state.open_files.erase(file_it);
            }
        }
        
        std::cout << "[ISOLATION] File operation for " << test_name 
                  << " (file: " << filename << ", operation: " << operation << ")" << std::endl;
    }
}

void TestIsolationController::track_network_operation(const std::string& test_name, const std::string& connection) {
    std::lock_guard<std::mutex> lock(controller_mutex);
    
    auto it = test_states.find(test_name);
    if (it != test_states.end()) {
        TestState& state = it->second;
        state.network_connections.push_back(connection);
        
        std::cout << "[ISOLATION] Network operation for " << test_name 
                  << " (connection: " << connection << ")" << std::endl;
    }
}

// Export isolation components
extern "C" {
    void export_isolation_controller(TestIsolationController* controller) {
        if (controller) {
            global_test_isolation = *controller;
        }
    }
    
    void export_test_environment(TestEnvironment* environment) {
        if (environment) {
            global_test_isolation.test_environment = *environment;
        }
    }
}