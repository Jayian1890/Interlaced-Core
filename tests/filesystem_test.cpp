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

#include "interlaced_core/filesystem.hpp"
#include <iostream>
#include <cassert>

int main() {
    using namespace interlaced::core::filesystem;
    
    std::cout << "=== Starting Filesystem Tests ===" << std::endl;
    
    // Test current path
    std::string current = FileSystem::current_path();
    std::cout << "Current path: " << current << std::endl;
    assert(!current.empty());
    
    // Test temp directory path
    std::string temp = FileSystem::temp_directory_path();
    std::cout << "Temp directory: " << temp << std::endl;
    assert(!temp.empty());
    
    // Test file operations
    std::string test_file = "test_file.txt";
    std::string test_content = "Hello, World!\nThis is a test file.\n";
    
    // Write file
    bool write_result = FileSystem::write_file(test_file, test_content);
    std::cout << "Write file result: " << (write_result ? "SUCCESS" : "FAILED") << std::endl;
    assert(write_result);
    
    // Check file exists
    bool exists_result = FileSystem::exists(test_file);
    std::cout << "File exists: " << (exists_result ? "YES" : "NO") << std::endl;
    assert(exists_result);
    
    // Check is regular file
    bool is_file_result = FileSystem::is_regular_file(test_file);
    std::cout << "Is regular file: " << (is_file_result ? "YES" : "NO") << std::endl;
    assert(is_file_result);
    
    // Check file size
    long long file_size = FileSystem::file_size(test_file);
    std::cout << "File size: " << file_size << " bytes" << std::endl;
    assert(file_size > 0);
    
    // Read file
    std::string read_content = FileSystem::read_file(test_file);
    std::cout << "Read content: " << read_content << std::endl;
    assert(read_content == test_content);
    
    // Test directory operations
    std::string test_dir = "test_directory";
    std::string nested_dir = "test_directory/nested/deeply/nested";
    
    // Create directories recursively
    bool create_dirs_result = FileSystem::create_directories(nested_dir);
    std::cout << "Create directories result: " << (create_dirs_result ? "SUCCESS" : "FAILED") << std::endl;
    assert(create_dirs_result);
    
    // Check directory exists
    bool dir_exists_result = FileSystem::exists(test_dir);
    std::cout << "Directory exists: " << (dir_exists_result ? "YES" : "NO") << std::endl;
    assert(dir_exists_result);
    
    // Check is directory
    bool is_dir_result = FileSystem::is_directory(test_dir);
    std::cout << "Is directory: " << (is_dir_result ? "YES" : "NO") << std::endl;
    assert(is_dir_result);
    
    // Test file copy
    std::string copied_file = "copied_file.txt";
    bool copy_result = FileSystem::copy_file(test_file, copied_file);
    std::cout << "Copy file result: " << (copy_result ? "SUCCESS" : "FAILED") << std::endl;
    assert(copy_result);
    
    // Verify copied file
    std::string copied_content = FileSystem::read_file(copied_file);
    assert(copied_content == test_content);
    
    // Test file rename/move
    std::string moved_file = "moved_file.txt";
    bool rename_result = FileSystem::rename(copied_file, moved_file);
    std::cout << "Rename file result: " << (rename_result ? "SUCCESS" : "FAILED") << std::endl;
    assert(rename_result);
    
    // Verify moved file exists
    assert(FileSystem::exists(moved_file));
    assert(!FileSystem::exists(copied_file));
    
    // Test directory listing
    std::vector<std::string> dir_contents = FileSystem::directory_iterator(".");
    std::cout << "Directory contents (" << dir_contents.size() << " items):" << std::endl;
    for (const auto& item : dir_contents) {
        std::cout << "  " << item << std::endl;
    }
    
    // Test file removal
    bool remove_result = FileSystem::remove(moved_file);
    std::cout << "Remove file result: " << (remove_result ? "SUCCESS" : "FAILED") << std::endl;
    assert(remove_result);
    
    // Clean up test files and directories
    FileSystem::remove(test_file);
    // Note: For simplicity, we won't remove the nested directories in this test
    
    std::cout << "=== All Filesystem Tests Passed ===" << std::endl;
    return 0;
}