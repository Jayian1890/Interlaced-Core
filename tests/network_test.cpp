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

#include "interlaced_core/network.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

void test_resolve_hostname_valid() {
    std::cout << "Testing resolve_hostname with valid hostname..." << std::endl;
    
    // Test with a known reliable hostname
    auto result = interlaced::core::network::Network::resolve_hostname("google.com");
    
    if (!result.success) {
        std::cerr << "ERROR: Failed to resolve google.com. Error code: " << result.error_code 
                  << ", Message: " << result.message << std::endl;
        return;
    }
    
    // Check that we got an IP address (should contain dots or colons)
    if (result.message.find('.') == std::string::npos && result.message.find(':') == std::string::npos) {
        std::cerr << "ERROR: Resolution succeeded but didn't return a valid IP address: " << result.message << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: resolve_hostname with google.com passed! IP: " << result.message << std::endl;
}

void test_resolve_hostname_empty() {
    std::cout << "Testing resolve_hostname with empty hostname..." << std::endl;
    
    auto result = interlaced::core::network::Network::resolve_hostname("");
    
    if (result.success) {
        std::cerr << "ERROR: Empty hostname should not resolve" << std::endl;
        return;
    }
    
    if (result.error_code != 1) {
        std::cerr << "ERROR: Expected error code 1 for empty hostname, got " << result.error_code << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: resolve_hostname with empty hostname correctly failed!" << std::endl;
}

void test_resolve_hostname_invalid() {
    std::cout << "Testing resolve_hostname with invalid hostname..." << std::endl;
    
    auto result = interlaced::core::network::Network::resolve_hostname("this-domain-should-not-exist-12345.com");
    
    if (result.success) {
        std::cerr << "ERROR: Invalid hostname should not resolve" << std::endl;
        return;
    }
    
    if (result.error_code != 2) {
        std::cerr << "ERROR: Expected error code 2 for invalid hostname, got " << result.error_code << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: resolve_hostname with invalid hostname correctly failed!" << std::endl;
}

void test_is_host_reachable_valid_host() {
    std::cout << "Testing is_host_reachable with valid host..." << std::endl;
    
    // Test with a known reliable host
    auto result = interlaced::core::network::Network::is_host_reachable("google.com");
    
    if (!result.success) {
        std::cerr << "ERROR: Failed to reach google.com. Error code: " << result.error_code 
                  << ", Message: " << result.message << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: is_host_reachable with google.com passed!" << std::endl;
}

void test_is_host_reachable_empty_host() {
    std::cout << "Testing is_host_reachable with empty host..." << std::endl;
    
    auto result = interlaced::core::network::Network::is_host_reachable("");
    
    if (result.success) {
        std::cerr << "ERROR: Empty host should not be reachable" << std::endl;
        return;
    }
    
    if (result.error_code != 1) {
        std::cerr << "ERROR: Expected error code 1 for empty host, got " << result.error_code << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: is_host_reachable with empty host correctly failed!" << std::endl;
}

void test_is_host_reachable_invalid_host() {
    std::cout << "Testing is_host_reachable with invalid host..." << std::endl;
    
    auto result = interlaced::core::network::Network::is_host_reachable("this-domain-should-not-exist-12345.com");
    
    // Note: This test might pass or fail depending on DNS configuration
    // The important thing is that it returns a proper NetworkResult
    std::cout << "Result for invalid host: success=" << result.success 
              << ", error_code=" << result.error_code 
              << ", message=" << result.message << std::endl;
    
    std::cout << "INFO: is_host_reachable with invalid host test completed!" << std::endl;
}

void test_download_file_valid_url() {
    std::cout << "Testing download_file with valid URL..." << std::endl;
    
    const std::string test_file = "test_download.txt";
    
    // Test downloading a small file
    auto result = interlaced::core::network::Network::download_file(
        "http://httpbin.org/json", test_file);
    
    if (!result.success) {
        std::cerr << "ERROR: Failed to download file. Error code: " << result.error_code 
                  << ", Message: " << result.message << std::endl;
        
        // Try an alternative URL if the first one fails
        std::cout << "Trying alternative URL..." << std::endl;
        result = interlaced::core::network::Network::download_file(
            "http://httpbin.org/get", test_file);
        
        if (!result.success) {
            std::cerr << "ERROR: Failed to download file from alternative URL. Error code: " 
                      << result.error_code << ", Message: " << result.message << std::endl;
            return;
        }
    }
    
    // Check if file was created
    if (!std::filesystem::exists(test_file)) {
        std::cerr << "ERROR: Downloaded file was not created" << std::endl;
        return;
    }
    
    // Check if file has content
    std::ifstream file(test_file);
    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "ERROR: Downloaded file is empty" << std::endl;
        file.close();
        std::filesystem::remove(test_file);
        return;
    }
    file.close();
    
    // Clean up
    std::filesystem::remove(test_file);
    
    std::cout << "SUCCESS: download_file with valid URL passed!" << std::endl;
}

void test_download_file_empty_url() {
    std::cout << "Testing download_file with empty URL..." << std::endl;
    
    const std::string test_file = "test_empty_url.txt";
    auto result = interlaced::core::network::Network::download_file("", test_file);
    
    if (result.success) {
        std::cerr << "ERROR: Empty URL should not succeed" << std::endl;
        return;
    }
    
    if (result.error_code != 1) {
        std::cerr << "ERROR: Expected error code 1 for empty URL, got " << result.error_code << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: download_file with empty URL correctly failed!" << std::endl;
}

void test_download_file_empty_destination() {
    std::cout << "Testing download_file with empty destination..." << std::endl;
    
    auto result = interlaced::core::network::Network::download_file("http://httpbin.org/json", "");
    
    if (result.success) {
        std::cerr << "ERROR: Empty destination should not succeed" << std::endl;
        return;
    }
    
    if (result.error_code != 2) {
        std::cerr << "ERROR: Expected error code 2 for empty destination, got " << result.error_code << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: download_file with empty destination correctly failed!" << std::endl;
}

void test_download_file_invalid_url() {
    std::cout << "Testing download_file with invalid URL..." << std::endl;
    
    const std::string test_file = "test_invalid_url.txt";
    auto result = interlaced::core::network::Network::download_file("invalid-url", test_file);
    
    if (result.success) {
        std::cerr << "ERROR: Invalid URL should not succeed" << std::endl;
        return;
    }
    
    if (result.error_code != 6) {
        std::cerr << "ERROR: Expected error code 6 for invalid URL, got " << result.error_code << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: download_file with invalid URL correctly failed!" << std::endl;
}

int main() {
    std::cout << "=== Starting Network Function Tests ===" << std::endl;
    
    // Test resolve_hostname function
    test_resolve_hostname_valid();
    test_resolve_hostname_empty();
    test_resolve_hostname_invalid();
    
    std::cout << std::endl;
    
    // Test is_host_reachable function
    test_is_host_reachable_valid_host();
    test_is_host_reachable_empty_host();
    test_is_host_reachable_invalid_host();
    
    std::cout << std::endl;
    
    // Test download_file function
    test_download_file_valid_url();
    test_download_file_empty_url();
    test_download_file_empty_destination();
    test_download_file_invalid_url();
    
    std::cout << "=== All Network Function Tests Completed ===" << std::endl;
    return 0;
}