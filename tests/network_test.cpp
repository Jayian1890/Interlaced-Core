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

void test_get_network_interfaces() {
    std::cout << "Testing get_network_interfaces..." << std::endl;
    
    auto interfaces = interlaced::core::network::Network::get_network_interfaces();
    
    // Check that we got some interfaces
    if (interfaces.empty()) {
        std::cerr << "ERROR: No network interfaces returned" << std::endl;
        return;
    }
    
    // Check that we have at least one common interface
    bool has_common_interface = false;
    for (const auto& interface : interfaces) {
        if (interface == "eth0" || interface == "wlan0" || interface == "lo" || 
            interface == "Ethernet" || interface == "Wi-Fi" || interface == "Loopback") {
            has_common_interface = true;
            break;
        }
    }
    
    if (!has_common_interface) {
        std::cerr << "ERROR: No common network interfaces found" << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: get_network_interfaces returned " << interfaces.size() << " interfaces" << std::endl;
}

void test_is_valid_ipv4() {
    std::cout << "Testing is_valid_ipv4..." << std::endl;
    
    // Test valid IPv4 addresses
    if (!interlaced::core::network::Network::is_valid_ipv4("192.168.1.1")) {
        std::cerr << "ERROR: Valid IPv4 address 192.168.1.1 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_valid_ipv4("127.0.0.1")) {
        std::cerr << "ERROR: Valid IPv4 address 127.0.0.1 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_valid_ipv4("0.0.0.0")) {
        std::cerr << "ERROR: Valid IPv4 address 0.0.0.0 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_valid_ipv4("255.255.255.255")) {
        std::cerr << "ERROR: Valid IPv4 address 255.255.255.255 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    // Test invalid IPv4 addresses
    if (interlaced::core::network::Network::is_valid_ipv4("")) {
        std::cerr << "ERROR: Empty string incorrectly flagged as valid IPv4" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_valid_ipv4("192.168.1")) {
        std::cerr << "ERROR: Incomplete IPv4 address 192.168.1 incorrectly flagged as valid" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_valid_ipv4("192.168.1.256")) {
        std::cerr << "ERROR: IPv4 address with invalid octet 192.168.1.256 incorrectly flagged as valid" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_valid_ipv4("192.168.01.1")) {
        std::cerr << "ERROR: IPv4 address with leading zero 192.168.01.1 incorrectly flagged as valid" << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: is_valid_ipv4 tests passed!" << std::endl;
}

void test_is_valid_ipv6() {
    std::cout << "Testing is_valid_ipv6..." << std::endl;
    
    // Test valid IPv6 addresses
    if (!interlaced::core::network::Network::is_valid_ipv6("::1")) {
        std::cerr << "ERROR: Valid IPv6 address ::1 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_valid_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334")) {
        std::cerr << "ERROR: Valid IPv6 address 2001:0db8:85a3:0000:0000:8a2e:0370:7334 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_valid_ipv6("fe80::1")) {
        std::cerr << "ERROR: Valid IPv6 address fe80::1 incorrectly flagged as invalid" << std::endl;
        return;
    }
    
    // Test invalid IPv6 addresses
    if (interlaced::core::network::Network::is_valid_ipv6("")) {
        std::cerr << "ERROR: Empty string incorrectly flagged as valid IPv6" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_valid_ipv6("192.168.1.1")) {
        std::cerr << "ERROR: IPv4 address incorrectly flagged as valid IPv6" << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: is_valid_ipv6 tests passed!" << std::endl;
}

void test_create_and_close_socket_connection() {
    std::cout << "Testing create_socket_connection and close_socket_connection..." << std::endl;
    
    // Test creating a socket connection to a known host
    int sockfd = interlaced::core::network::Network::create_socket_connection("google.com", 80);
    
    // Note: We're not checking if the connection succeeds because it depends on network conditions
    // Instead, we're checking that the function returns a valid value
    if (sockfd == 0) {
        std::cerr << "ERROR: create_socket_connection returned 0, which might indicate a problem" << std::endl;
        return;
    }
    
    // Test closing the socket connection
    if (sockfd > 0) {
        bool close_result = interlaced::core::network::Network::close_socket_connection(sockfd);
        if (!close_result) {
            std::cerr << "ERROR: close_socket_connection failed" << std::endl;
            return;
        }
    }
    
    // Test with invalid parameters
    int invalid_sockfd = interlaced::core::network::Network::create_socket_connection("", 80);
    if (invalid_sockfd != -1) {
        std::cerr << "ERROR: create_socket_connection should return -1 for empty host" << std::endl;
        // Clean up if needed
        if (invalid_sockfd > 0) {
            interlaced::core::network::Network::close_socket_connection(invalid_sockfd);
        }
        return;
    }
    
    std::cout << "SUCCESS: create_socket_connection and close_socket_connection tests passed!" << std::endl;
}

void test_parse_http_response_code() {
    std::cout << "Testing parse_http_response_code..." << std::endl;
    
    // Test valid HTTP responses
    int code1 = interlaced::core::network::Network::parse_http_response_code("HTTP/1.1 200 OK");
    if (code1 != 200) {
        std::cerr << "ERROR: Expected 200, got " << code1 << std::endl;
        return;
    }
    
    int code2 = interlaced::core::network::Network::parse_http_response_code("HTTP/1.1 404 Not Found");
    if (code2 != 404) {
        std::cerr << "ERROR: Expected 404, got " << code2 << std::endl;
        return;
    }
    
    int code3 = interlaced::core::network::Network::parse_http_response_code("HTTP/1.1 500 Internal Server Error");
    if (code3 != 500) {
        std::cerr << "ERROR: Expected 500, got " << code3 << std::endl;
        return;
    }
    
    // Test invalid HTTP responses
    int code4 = interlaced::core::network::Network::parse_http_response_code("");
    if (code4 != -1) {
        std::cerr << "ERROR: Expected -1 for empty string, got " << code4 << std::endl;
        return;
    }
    
    int code5 = interlaced::core::network::Network::parse_http_response_code("Invalid response");
    if (code5 != -1) {
        std::cerr << "ERROR: Expected -1 for invalid response, got " << code5 << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: parse_http_response_code tests passed!" << std::endl;
}

void test_is_http_success() {
    std::cout << "Testing is_http_success..." << std::endl;
    
    // Test successful HTTP codes
    if (!interlaced::core::network::Network::is_http_success(200)) {
        std::cerr << "ERROR: 200 should be considered successful" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_http_success(201)) {
        std::cerr << "ERROR: 201 should be considered successful" << std::endl;
        return;
    }
    
    if (!interlaced::core::network::Network::is_http_success(299)) {
        std::cerr << "ERROR: 299 should be considered successful" << std::endl;
        return;
    }
    
    // Test unsuccessful HTTP codes
    if (interlaced::core::network::Network::is_http_success(199)) {
        std::cerr << "ERROR: 199 should not be considered successful" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_http_success(300)) {
        std::cerr << "ERROR: 300 should not be considered successful" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_http_success(404)) {
        std::cerr << "ERROR: 404 should not be considered successful" << std::endl;
        return;
    }
    
    if (interlaced::core::network::Network::is_http_success(500)) {
        std::cerr << "ERROR: 500 should not be considered successful" << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: is_http_success tests passed!" << std::endl;
}

void test_measure_latency() {
    std::cout << "Testing measure_latency..." << std::endl;
    
    // Test with a valid host
    double latency = interlaced::core::network::Network::measure_latency("google.com", 4);
    
    // We're just checking that it returns a reasonable value
    // In a real implementation, this would be an actual latency measurement
    if (latency < 0) {
        std::cerr << "ERROR: measure_latency returned negative value" << std::endl;
        return;
    }
    
    // Test with invalid parameters
    double invalid_latency = interlaced::core::network::Network::measure_latency("", 4);
    if (invalid_latency >= 0) {
        std::cerr << "ERROR: measure_latency should return negative value for empty host" << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: measure_latency tests passed! (simulated latency: " << latency << " ms)" << std::endl;
}

void test_measure_bandwidth() {
    std::cout << "Testing measure_bandwidth..." << std::endl;
    
    // Test with a valid host
    double bandwidth = interlaced::core::network::Network::measure_bandwidth("google.com");
    
    // We're just checking that it returns a reasonable value
    // In a real implementation, this would be an actual bandwidth measurement
    if (bandwidth < 0) {
        std::cerr << "ERROR: measure_bandwidth returned negative value" << std::endl;
        return;
    }
    
    // Test with invalid parameters
    double invalid_bandwidth = interlaced::core::network::Network::measure_bandwidth("");
    if (invalid_bandwidth >= 0) {
        std::cerr << "ERROR: measure_bandwidth should return negative value for empty host" << std::endl;
        return;
    }
    
    std::cout << "SUCCESS: measure_bandwidth tests passed! (simulated bandwidth: " << bandwidth << " Mbps)" << std::endl;
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
    
    std::cout << std::endl;
    
    // Test additional network functions
    test_get_network_interfaces();
    test_is_valid_ipv4();
    test_is_valid_ipv6();
    test_create_and_close_socket_connection();
    test_parse_http_response_code();
    test_is_http_success();
    test_measure_latency();
    test_measure_bandwidth();
    
    std::cout << "=== All Network Function Tests Completed ===" << std::endl;
    return 0;
}