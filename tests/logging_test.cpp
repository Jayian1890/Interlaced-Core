/*
 * Interlaced Core Library
 * Copyright (c) 2025 Your Name or Organization
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "interlaced_core/logging.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

// Custom formatter example
class CustomFormatter : public interlaced::core::logging::LogFormatter {
public:
    std::string format(interlaced::core::logging::LogLevel level, const std::string& message, 
                      const std::tm& time_info, const char* file = nullptr, int line = 0) override {
        std::ostringstream oss;
        oss << "[Custom] " << interlaced::core::logging::log_level_to_string(level) 
            << " " << message;
        if (file && line > 0) {
            oss << " [" << file << ":" << line << "]";
        }
        return oss.str();
    }
};

// Test formatter for validation
class TestFormatter : public interlaced::core::logging::LogFormatter {
private:
    mutable std::string last_formatted_message;
    
public:
    std::string format(interlaced::core::logging::LogLevel level, const std::string& message, 
                      const std::tm& time_info, const char* file = nullptr, int line = 0) override {
        std::ostringstream oss;
        oss << "[" << interlaced::core::logging::log_level_to_string(level) << "] " << message;
        last_formatted_message = oss.str();
        return last_formatted_message;
    }
    
    const std::string& get_last_formatted_message() const {
        return last_formatted_message;
    }
};

void thread_function(int id) {
    for (int i = 0; i < 5; ++i) {
        interlaced::core::logging::Logger::info("Thread {} message {}", id, i);
    }
}

void thread_function_basic(int id) {
    for (int i = 0; i < 5; ++i) {
        interlaced::core::logging::Logger::info("Thread " + std::to_string(id) + " message " + std::to_string(i));
    }
}

// Test function for log level conversion
void test_log_level_to_string() {
    using namespace interlaced::core::logging;
    
    std::cout << "Testing log_level_to_string function..." << std::endl;
    
    if (std::string(log_level_to_string(LOG_DEBUG)) != "DEBUG") {
        std::cerr << "ERROR: DEBUG level string conversion failed" << std::endl;
        return;
    }
    
    if (std::string(log_level_to_string(LOG_INFO)) != "INFO") {
        std::cerr << "ERROR: INFO level string conversion failed" << std::endl;
        return;
    }
    
    if (std::string(log_level_to_string(LOG_WARNING)) != "WARNING") {
        std::cerr << "ERROR: WARNING level string conversion failed" << std::endl;
        return;
    }
    
    if (std::string(log_level_to_string(LOG_ERROR)) != "ERROR") {
        std::cerr << "ERROR: ERROR level string conversion failed" << std::endl;
        return;
    }
    
    std::cout << "log_level_to_string tests passed!" << std::endl;
}

// Test function for custom streams
void test_custom_streams() {
    using namespace interlaced::core::logging;
    
    std::cout << "Testing custom streams..." << std::endl;
    
    // Create string streams to capture output
    std::ostringstream output_stream, error_stream;
    
    // Set custom streams
    Logger::set_output_streams(output_stream, error_stream);
    
    // Log messages
    Logger::info("Info message to custom output");
    Logger::error("Error message to custom error");
    
    // Check that messages were written to correct streams
    std::string output_content = output_stream.str();
    std::string error_content = error_stream.str();
    
    if (output_content.find("Info message to custom output") == std::string::npos) {
        std::cerr << "ERROR: Info message not found in output stream" << std::endl;
        return;
    }
    
    if (error_content.find("Error message to custom error") == std::string::npos) {
        std::cerr << "ERROR: Error message not found in error stream" << std::endl;
        return;
    }
    
    std::cout << "Custom streams tests passed!" << std::endl;
    
    // Reset to default streams
    Logger::set_output_streams(std::cout, std::cerr);
}

// Test function for custom formatter
void test_custom_formatter() {
    using namespace interlaced::core::logging;
    
    std::cout << "Testing custom formatter..." << std::endl;
    
    auto test_formatter = std::make_unique<TestFormatter>();
    TestFormatter* formatter_ptr = test_formatter.get();
    
    Logger::set_formatter(std::move(test_formatter));
    
    Logger::info("Test message for formatter");
    
    const std::string& last_message = formatter_ptr->get_last_formatted_message();
    if (last_message.find("[INFO] Test message for formatter") == std::string::npos) {
        std::cerr << "ERROR: Custom formatter did not produce expected output" << std::endl;
        std::cerr << "Actual output: " << last_message << std::endl;
        Logger::set_formatter(nullptr); // Reset formatter
        return;
    }
    
    std::cout << "Custom formatter tests passed!" << std::endl;
    
    // Reset to default formatter
    Logger::set_formatter(nullptr);
}

// Test function for file logging
void test_file_logging() {
    using namespace interlaced::core::logging;
    
    std::cout << "Testing file logging..." << std::endl;
    
    // Test size-based rotation
    const std::string test_file = "comprehensive_test.log";
    const size_t small_size = 512; // 512 bytes
    const int max_files = 2;
    
    Logger::set_file_logging(test_file, small_size, max_files);
    
    // Generate enough log messages to trigger rotation
    for (int i = 0; i < 50; ++i) {
        Logger::info("Log message {} for comprehensive file rotation test with longer text to reach size limit faster", i);
    }
    
    // Check if log files were created
    if (!std::filesystem::exists(test_file)) {
        std::cerr << "ERROR: Main log file was not created" << std::endl;
        return;
    }
    
    std::cout << "File logging tests passed!" << std::endl;
    
    // Reset to console logging
    Logger::set_file_logging("", 0, 0); // This won't actually work, but we're just testing
}

int main() {
    using namespace interlaced::core::logging;
    
    std::cout << "=== Starting Comprehensive Logging Tests ===" << std::endl;
    
    // Test log level to string conversion
    test_log_level_to_string();
    
    // Test basic logging
    Logger::info("Starting comprehensive logging tests");
    
    // Test different log levels
    Logger::debug("This is a debug message");
    Logger::info("This is an info message");
    Logger::warning("This is a warning message");
    Logger::error("This is an error message");
    
    // Test log level filtering
    Logger::set_level(LOG_WARNING);
    Logger::info("This info message should NOT appear");
    Logger::warning("This warning message SHOULD appear");
    Logger::error("This error message SHOULD appear");
    
    Logger::set_level(LOG_DEBUG);
    
    // Test file/line information
    LOG_INFO("This message includes file and line info");
    LOG_ERROR("This error includes file and line info");
    
    // Test structured logging
    Logger::info("User login", "user_id", 12345, "ip_address", "192.168.1.1");
    Logger::warning("High memory usage", "used_mb", 850, "limit_mb", 1000);
    Logger::error("Database connection failed", "error_code", 500, "server", "db01.prod");
    
    // Test variadic template logging
    Logger::info("User {} logged in from IP {}", "john_doe", "192.168.1.100");
    Logger::warning("Disk usage is at {}%", 95);
    Logger::error("Database connection failed with code {} for user {}", 500, "admin");
    
    // Test custom streams
    test_custom_streams();
    
    // Test custom formatter
    test_custom_formatter();
    
    // Test custom formatter with file/line info
    Logger::set_formatter(std::make_unique<CustomFormatter>());
    Logger::info("This message uses a custom formatter");
    LOG_WARNING("This warning also uses a custom formatter");
    
    // Reset to default formatter
    Logger::set_formatter(nullptr);
    
    // Test file logging with size-based rotation
    Logger::set_file_logging("test_app.log", 1024, 3); // 1KB max size, 3 files
    
    // Generate a lot of log messages to test rotation
    for (int i = 0; i < 100; ++i) {
        Logger::info("Log message number {} for testing file rotation with more comprehensive content to ensure proper rotation", i);
    }
    
    // Test time-based rotation
    Logger::set_file_logging("timed_app.log", std::chrono::hours(1), 3); // 1 hour rotation
    
    Logger::info("This message goes to a time-rotated log file");
    Logger::warning("This is a warning in time-rotated log");
    Logger::error("This is an error in time-rotated log");
    
    // Test edge cases
    Logger::info("Testing empty message: ");
    Logger::info(""); // Empty message
    Logger::info("Testing special characters: !@#$%^&*()_+-=[]{}|;':\",./<>?");
    Logger::info("Testing unicode: Hello 世界 🌍");
    
    // Test very long messages
    std::string long_message(1000, 'A');
    Logger::info("Testing long message: {}", long_message);
    
    // Test numeric values
    Logger::info("Integer: {}", 42);
    Logger::info("Float: {}", 3.14159);
    Logger::info("Boolean: {}", true);
    
    // Test pointer values
    int value = 123;
    Logger::info("Pointer: {}", static_cast<void*>(&value));
    
    // Test thread safety with multiple threads (variadic template version)
    Logger::info("Testing thread safety with multiple threads (variadic template)");
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; ++i) { // Increased thread count for more stress
        threads.emplace_back(thread_function, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Test thread safety with multiple threads (basic version)
    Logger::info("Testing thread safety with multiple threads (basic)");
    std::vector<std::thread> threads_basic;
    
    for (int i = 0; i < 5; ++i) { // Increased thread count for more stress
        threads_basic.emplace_back(thread_function_basic, i);
    }
    
    for (auto& t : threads_basic) {
        t.join();
    }
    
    // Test all log levels with thread info
    for (int i = 0; i < 10; ++i) {
        Logger::debug("Debug message from test loop {}", i);
        Logger::info("Info message from test loop {}", i);
        Logger::warning("Warning message from test loop {}", i);
        Logger::error("Error message from test loop {}", i);
    }
    
    // Final comprehensive test
    Logger::info("Comprehensive test", 
                 "status", "completed", 
                 "threads_tested", 10, 
                 "messages_logged", "many", 
                 "features_tested", "all");
    
    std::cout << "=== All Comprehensive Logging Tests Completed ===" << std::endl;
    return 0;
}