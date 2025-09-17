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

#include <iostream>
#include <sstream>
#include "interlaced_core/filesystem.hpp"
#include "interlaced_core/json.hpp"
#include "interlaced_core/logging.hpp"
#include "interlaced_core/network.hpp"

int main() {
    using namespace interlaced::core::logging;
    
    // Test logging with different levels
    Logger::set_level(LOG_DEBUG);
    LOG_INFO("Starting interlaced_core tests");
    LOG_DEBUG("Debug information");
    LOG_WARNING("This is a warning");
    LOG_ERROR("This is an error");
    
    // Test custom output streams
    std::ostringstream output_stream, error_stream;
    Logger::set_output_streams(output_stream, error_stream);
    
    Logger::info("Testing custom streams");
    Logger::error("Error to custom stream");
    
    // Test filesystem
    bool exists = interlaced::core::filesystem::FileSystem::exists("test.txt");
    LOG_INFO("File exists: " + std::to_string(exists));
    
    // Test JSON
    std::map<std::string, std::string> data;
    data["key"] = "value";
    std::string json = interlaced::core::json::JSON::stringify(data);
    LOG_INFO("JSON: " + json);
    
    // Test network
    std::string response = interlaced::core::network::Network::http_get("http://example.com");
    LOG_INFO("HTTP Response: " + response);
    
    LOG_INFO("Tests completed");
    return 0;
}