#include "core/Logger/Logger.hpp"
#include <iostream>

int main() {
    // Test backward compatibility
    nlm::Logger::getGlobal().info("Testing backward compatibility");
    
    // Test component-specific logging
    nlm::Logger::getGlobal().enableComponent("game");
    nlm::Logger::getGlobal().setComponentLevel("game", nlm::LogLevel::Debug);
    nlm::Logger::getGlobal().info("Component logging test", "", 0, "", "game");
    
    // Test hierarchical logging
    nlm::Logger::getGlobal().registerComponentLogger("graphics.render", "graphics");
    nlm::Logger::getGlobal().registerComponentLogger("audio", "");
    nlm::Logger::getGlobal().registerComponentLogger("input.keyboard", "input");
    nlm::Logger::getGlobal().registerComponentLogger("input.mouse", "input");
    
    // Test parent-child component relationship
    nlm::Logger::getGlobal().getComponentLogger("graphics.render")->setMinimumLevel(nlm::LogLevel::Warning);
    nlm::Logger::getGlobal().getComponentLogger("input.keyboard")->setMinimumLevel(nlm::LogLevel::Info);
    
    nlm::Logger::getGlobal().warning("Parent-child test", "", 0, "", "graphics.render");
    nlm::Logger::getGlobal().info("Parent-child test", "", 0, "", "graphics.render");
    
    // Test filtering
    auto filter = [](const std::string&, const nlm::LogEntry& entry) {
        return entry.level != nlm::LogLevel::Debug;
    };
    nlm::Logger::getGlobal().setComponentFilter("game", filter);
    
    nlm::Logger::getGlobal().debug("This debug message should not appear", "", 0, "", "game");
    nlm::Logger::getGlobal().info("This info message should appear", "", 0, "", "game");
    
    std::cout << "All logging tests passed!" << std::endl;
    return 0;
}
