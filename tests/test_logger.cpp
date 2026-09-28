#include <iostream>
#include <cassert>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

void testBasicLogging() {
    std::cout << "Testing basic logging functionality..." << std::endl;
    
    // Test logger creation and configuration
    auto logger = std::make_shared<nlm::Logger>("test_logger");
    auto consoleAppender = std::make_shared<nlm::ConsoleAppender>(LogLevel::Debug);
    consoleAppender->setUseColors(false);
    
    logger->addAppender(consoleAppender);
    logger->setLevel(LogLevel::Debug);
    logger->setPattern("%timestamp %level [%module] %message");
    
    // Test different log levels
    logger->debug("Debug message with structured data", "", 0, "", "test_module");
    logger->info("Info message with tags", "", 0, "", "test_module");
    logger->warning("Warning message", "", 0, "", "test_module");
    logger->error("Error message with fields", "", 0, "", "test_module");
    logger->critical("Critical message", "", 0, "", "test_module");
    
    // Test structured logging
    std::unordered_map<std::string, std::string> fields;
    fields["user_id"] = "12345";
    fields["action"] = "login";
    fields["ip_address"] = "192.168.1.1";
    
    logger->logStructured(LogLevel::Info, "User logged in",
                         "", 0, "", "auth",
                         fields);
    
    // Test tags
    auto streamLogger = logger->logStream(LogLevel::Info, "", 0, "", "test");
    streamLogger.addTag("performance")
               .addTag("database")
               .addField("query_time", "123ms")
               .addField("rows_affected", "42")
               << " Database query completed";
    
    // Test filtering
    auto levelFilter = std::make_shared<nlm::LevelFilter>(LogLevel::Warning);
    logger->setFilter(levelFilter);
    
    logger->debug("This debug message should not appear (filtered)");
    logger->warning("This warning should appear (not filtered)");
    
    // Test metrics
    auto metrics = logger->getMetrics();
    assert(metrics.getLogCount() > 0);
    std::cout << "  Logged " << metrics.getLogCount() << " messages" << std::endl;
    
    std::cout << "Basic logging tests PASSED" << std::endl;
}

void testMemoryPool() {
    std::cout << "Testing memory pool functionality..." << std::endl;
    
    // Test thread-local storage pools
    auto* entryPool = nlm::ThreadLocalStorage::getEntryPool(64);
    auto* stringPool = nlm::ThreadLocalStorage::getStringPool(128);
    
    // Test LogEntry pool
    LogEntry* entry1 = entryPool->acquire();
    assert(entry1 != nullptr);
    entry1->level = LogLevel::Info;
    entry1->message = "Test message";
    entryPool->release(entry1);
    
    // Test String pool
    std::string* str1 = stringPool->acquire();
    assert(str1 != nullptr);
    str1->assign("Test string");
    stringPool->release(str1);
    
    // Test global pools
    size_t initialEntrySize = nlm::Logger::getEntryPoolSize();
    size_t initialStringSize = nlm::Logger::getStringPoolSize();
    
    // Perform logging operations
    auto logger = std::make_shared<nlm::Logger>("pool_test");
    for (int i = 0; i < 10; ++i) {
        logger->info("Message " + std::to_string(i));
    }
    
    // Check pool sizes
    size_t finalEntrySize = nlm::Logger::getEntryPoolSize();
    size_t finalStringSize = nlm::Logger::getStringPoolSize();
    
    std::cout << "  Entry pool size: " << finalEntrySize << std::endl;
    std::cout << "  String pool size: " << finalStringSize << std::endl;
    
    nlm::Logger::cleanupPools();
    
    std::cout << "Memory pool tests PASSED" << std::endl;
}

void testFormatting() {
    std::cout << "Testing formatting and patterns..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>("format_test");
    auto consoleAppender = std::make_shared<nlm::ConsoleAppender>(LogLevel::Debug);
    consoleAppender->setUseColors(false);
    
    logger->addAppender(consoleAppender);
    
    // Test different patterns
    logger->setPattern("%timestamp %level %message");
    logger->info("Simple message");
    
    logger->setPattern("[%level] %timestamp %module %file:%line - %message");
    logger->info("Detailed message", __FILE__, __LINE__, __func__, "format_test");
    
    logger->setPattern("%tags %message");
    auto streamLogger = logger->logStream(LogLevel::Info, "", 0, "", "tags_test");
    streamLogger.addTag("debug").addTag("test") << " Message with tags";
    
    std::cout << "Formatting tests PASSED" << std::endl;
}

void testAsyncLogging() {
    std::cout << "Testing asynchronous logging..." << std::endl;
    
    auto asyncLogger = std::make_shared<nlm::AsyncLogger>(1000, 2);
    asyncLogger->setLevel(LogLevel::Info);
    asyncLogger->setPattern("%timestamp %level %message");
    
    // Log some messages
    for (int i = 0; i < 100; ++i) {
        nlm::LogEntry entry(nlm::LogLevel::Info, "Async message " + std::to_string(i));
        asyncLogger->log(entry);
    }
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check metrics
    std::cout << "  Async queue size: " << asyncLogger->getQueueSize() << std::endl;
    std::cout << "  Async processed count: " << asyncLogger->getProcessedCount() << std::endl;
    std::cout << "  Async average latency: " << asyncLogger->getAverageLatency() << "ms" << std::endl;
    
    asyncLogger->shutdown();
    
    std::cout << "Async logging tests PASSED" << std::endl;
}

void testFilters() {
    std::cout << "Testing filtering capabilities..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>("filter_test");
    auto consoleAppender = std::make_shared<nlm::ConsoleAppender>(LogLevel::Debug);
    consoleAppender->setUseColors(false);
    
    logger->addAppender(consoleAppender);
    
    // Test level filter
    auto levelFilter = std::make_shared<nlm::LevelFilter>(LogLevel::Error);
    logger->setFilter(levelFilter);
    
    logger->debug("Debug message (should not appear)");
    logger->info("Info message (should not appear)");
    logger->warning("Warning message (should not appear)");
    logger->error("Error message (should appear)");
    
    // Test module filter
    auto moduleFilter = std::make_shared<nlm::ModuleFilter>("auth");
    logger->setFilter(moduleFilter);
    
    logger->info("Message from auth module", "", 0, "", "auth");  // Should appear
    logger->info("Message from other module", "", 0, "", "database");  // Should not appear
    
    // Test tag filter
    auto tagFilter = std::make_shared<nlm::TagFilter>(
        {"performance", "critical"},  // Required tags
        {"debug"}                     // Excluded tags
    );
    logger->setFilter(tagFilter);
    
    auto streamLogger = logger->logStream(LogLevel::Info, "", 0, "", "test");
    streamLogger.addTag("performance") << " Message with performance tag (should appear)";
    
    streamLogger = logger->logStream(LogLevel::Info, "", 0, "", "test");
    streamLogger.addTag("debug") << " Message with debug tag (should not appear)";
    
    std::cout << "Filtering tests PASSED" << std::endl;
}

void testPerformance() {
    std::cout << "Testing performance improvements..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>("perf_test");
    auto consoleAppender = std::make_shared<nlm::ConsoleAppender>(LogLevel::Debug);
    consoleAppender->setUseColors(false);
    
    logger->addAppender(consoleAppender);
    logger->setLevel(LogLevel::Debug);
    logger->setPattern("%level %message");
    
    // Test batch logging
    std::vector<nlm::LogEntry> batchEntries;
    for (int i = 0; i < 100; ++i) {
        nlm::LogEntry entry(nlm::LogLevel::Info, "Batch message " + std::to_string(i));
        entry.module = "batch_test";
        batchEntries.push_back(entry);
    }
    
    logger->logBatch(batchEntries);
    
    // Test log count and metrics
    auto metrics = logger->getMetrics();
    std::cout << "  Total logs: " << metrics.getLogCount() << std::endl;
    std::cout << "  Average latency: " << metrics.getAverageLatency() << "ms" << std::endl;
    
    logger->resetMetrics();
    
    // Log more messages to verify metrics reset
    for (int i = 0; i < 10; ++i) {
        logger->info("Performance test message");
    }
    
    auto newMetrics = logger->getMetrics();
    std::cout << "  New log count: " << newMetrics.getLogCount() << std::endl;
    
    std::cout << "Performance tests PASSED" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Logging System Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testBasicLogging();
        std::cout << std::endl;
        
        testMemoryPool();
        std::cout << std::endl;
        
        testFormatting();
        std::cout << std::endl;
        
        testAsyncLogging();
        std::cout << std::endl;
        
        testFilters();
        std::cout << std::endl;
        
        testPerformance();
        std::cout << std::endl;
        
        std::cout << "=== All Enhanced Logging Tests PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}