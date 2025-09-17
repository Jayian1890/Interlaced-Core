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

#ifndef INTERLACED_CORE_NETWORK_HPP
#define INTERLACED_CORE_NETWORK_HPP

#include <string>
#include <iostream>
#include <vector>

// Platform-specific includes for network operations
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/time.h>
#endif
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace interlaced::core::network {
    
    /**
     * @brief Result structure for network operations
     * 
     * NetworkResult encapsulates the outcome of network operations, providing detailed
     * information about success or failure states. This structure allows callers to
     * determine not just whether an operation succeeded, but also to get specific 
     * error codes and descriptive messages for better error handling.
     * 
     * Error codes:
     * - 0: Success
     * - 1: Invalid input (e.g., empty host, URL)
     * - 2: Invalid destination path
     * - Other positive integers: Operation-specific errors
     */
    struct NetworkResult {
        /**
         * @brief Success indicator
         * 
         * True if the network operation completed successfully, false otherwise.
         */
        bool success;
        
        /**
         * @brief Error code
         * 
         * Numeric error code providing specific information about the result:
         * - 0: Success
         * - 1: Invalid input (e.g., empty host, URL)
         * - 2: Invalid destination path
         * - Other positive integers: Operation-specific errors
         */
        int error_code;
        
        /**
         * @brief Descriptive message
         * 
         * Human-readable message describing the result of the operation.
         * Contains error descriptions when success is false, or success
         * confirmation when success is true.
         */
        std::string message;
        
        /**
         * @brief Constructor for NetworkResult
         * 
         * @param s Success status (true for success, false for failure)
         * @param ec Error code (0 for success, positive integer for specific errors)
         * @param m Descriptive message about the operation result
         */
        NetworkResult(bool s, int ec, const std::string& m) : success(s), error_code(ec), message(m) {}
    };

    /**
     * @brief Network utility functions
     * 
     * This class provides basic network functionality such as
     * checking host reachability, downloading files, and performing HTTP GET requests.
     * Note: This is a simplified implementation for demonstration purposes.
     */
    class Network {
    private:
        /**
         * @brief Initialize Windows Sockets API (Windows only)
         * 
         * @return true if initialization successful or not needed, false on error
         */
        static bool initialize_winsock() {
#ifdef _WIN32
            WSADATA wsaData;
            return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
            return true; // No initialization needed on Unix-like systems
#endif
        }
        
        /**
         * @brief Cleanup Windows Sockets API (Windows only)
         */
        static void cleanup_winsock() {
#ifdef _WIN32
            WSACleanup();
#endif
        }
        
        /**
         * @brief Set socket timeout values
         * 
         * @param sockfd Socket file descriptor
         * @param timeout_seconds Timeout in seconds
         */
        static void set_socket_timeout(int sockfd, int timeout_seconds) {
#ifdef _WIN32
            DWORD timeout = timeout_seconds * 1000;
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
            setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
            struct timeval timeout;
            timeout.tv_sec = timeout_seconds;
            timeout.tv_usec = 0;
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
            setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#endif
        }
        
        /**
         * @brief Close socket connection
         * 
         * @param sockfd Socket file descriptor
         */
        static void close_socket(int sockfd) {
#ifdef _WIN32
            closesocket(sockfd);
#else
            close(sockfd);
#endif
        }
        
        /**
         * @brief Get connection error details
         * 
         * @param is_timeout Set to true if the error was a timeout
         * @param is_refused Set to true if the connection was refused
         * @return Appropriate error code
         */
        static int get_connection_error(bool& is_timeout, bool& is_refused) {
            is_timeout = false;
            is_refused = false;
            
#ifdef _WIN32
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT) {
                is_timeout = true;
                return 3; // Connection timeout
            } else if (error == WSAECONNREFUSED) {
                is_refused = true;
                return 4; // Connection refused
            }
#else
            if (errno == ETIMEDOUT) {
                is_timeout = true;
                return 3; // Connection timeout
            } else if (errno == ECONNREFUSED) {
                is_refused = true;
                return 4; // Connection refused
            }
#endif
            return 5; // General network error
        }
        
    public:
        /**
         * @brief Resolve hostname to IP address
         * 
         * Resolves a hostname to its corresponding IP address(es) using the system's
         * DNS resolver. This function can handle both IPv4 and IPv6 addresses.
         * 
         * @param hostname The hostname to resolve
         * @return NetworkResult containing success status and details
         *         On success, the message field contains the resolved IP address
         * 
         * Error codes:
         * - 0: Hostname resolved successfully
         * - 1: Hostname is empty
         * - 2: Hostname resolution failed
         * - 3: No addresses found for hostname
         */
        static NetworkResult resolve_hostname(const std::string& hostname) {
            // Validate input
            if (hostname.empty()) {
                return NetworkResult(false, 1, "Hostname is empty");
            }
            
            // Platform-specific socket initialization
            if (!initialize_winsock()) {
                return NetworkResult(false, 2, "Failed to initialize Winsock");
            }
            
            // Resolve hostname to IP address
            struct addrinfo hints, *result = nullptr;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
            hints.ai_socktype = SOCK_STREAM; // TCP socket
            hints.ai_flags = 0;
            hints.ai_protocol = 0;           // Any protocol
            
            int status = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
            if (status != 0) {
                cleanup_winsock();
                return NetworkResult(false, 2, "Hostname resolution failed: " + std::string(gai_strerror(status)));
            }
            
            // Get the first IP address from the result
            std::string ip_address;
            char ip_str[INET6_ADDRSTRLEN];
            
            if (result->ai_family == AF_INET) {
                // IPv4
                struct sockaddr_in* ipv4 = (struct sockaddr_in*)result->ai_addr;
                inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, INET_ADDRSTRLEN);
                ip_address = std::string(ip_str);
            } else if (result->ai_family == AF_INET6) {
                // IPv6
                struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)result->ai_addr;
                inet_ntop(AF_INET6, &ipv6->sin6_addr, ip_str, INET6_ADDRSTRLEN);
                ip_address = std::string(ip_str);
            }
            
            // Clean up
            freeaddrinfo(result);
            cleanup_winsock();
            
            if (ip_address.empty()) {
                return NetworkResult(false, 3, "No addresses found for hostname");
            }
            
            return NetworkResult(true, 0, ip_address);
        }
        /**
         * @brief Check if the host is reachable
         * 
         * Attempts to establish a connection to the specified host on port 80
         * to determine if it's reachable. This implementation uses the resolve_hostname
         * function to get the IP address and then establishes a basic TCP connection.
         * 
         * @param host The host to check (hostname or IP address)
         * @return NetworkResult containing success status and details
         * 
         * Error codes:
         * - 0: Host is reachable
         * - 1: Host string is empty
         * - 2: Hostname resolution failed
         * - 3: Connection timeout
         * - 4: Connection refused
         * - 5: General network error
         */
        static NetworkResult is_host_reachable(const std::string& host) {
            // Validate input
            if (host.empty()) {
                return NetworkResult(false, 1, "Host is empty");
            }
            
            // First resolve the hostname to an IP address
            NetworkResult resolve_result = resolve_hostname(host);
            if (!resolve_result.success) {
                // Pass through the error from resolve_hostname
                return NetworkResult(false, resolve_result.error_code, resolve_result.message);
            }
            
            // Extract the resolved IP address from the message
            std::string ip_address = resolve_result.message;
            
            // Platform-specific socket initialization
            if (!initialize_winsock()) {
                return NetworkResult(false, 5, "Failed to initialize Winsock");
            }
            
            // Create socket based on IP address format (IPv4 or IPv6)
            int sockfd;
            struct sockaddr_in addr4;
            struct sockaddr_in6 addr6;
            void* addr_ptr;
            int addr_len;
            bool is_ipv6 = (ip_address.find(':') != std::string::npos);
            
            if (is_ipv6) {
                // IPv6
                sockfd = socket(AF_INET6, SOCK_STREAM, 0);
                if (sockfd < 0) {
                    cleanup_winsock();
                    return NetworkResult(false, 5, "Failed to create IPv6 socket");
                }
                
                memset(&addr6, 0, sizeof(addr6));
                addr6.sin6_family = AF_INET6;
                addr6.sin6_port = htons(80);
                
                if (inet_pton(AF_INET6, ip_address.c_str(), &addr6.sin6_addr) <= 0) {
                    close_socket(sockfd);
                    cleanup_winsock();
                    return NetworkResult(false, 2, "Invalid IPv6 address format");
                }
                
                addr_ptr = &addr6;
                addr_len = sizeof(addr6);
            } else {
                // IPv4
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) {
                    cleanup_winsock();
                    return NetworkResult(false, 5, "Failed to create IPv4 socket");
                }
                
                memset(&addr4, 0, sizeof(addr4));
                addr4.sin_family = AF_INET;
                addr4.sin_port = htons(80);
                
                if (inet_pton(AF_INET, ip_address.c_str(), &addr4.sin_addr) <= 0) {
                    close_socket(sockfd);
                    cleanup_winsock();
                    return NetworkResult(false, 2, "Invalid IPv4 address format");
                }
                
                addr_ptr = &addr4;
                addr_len = sizeof(addr4);
            }
            
            // Set socket timeout (5 seconds)
            set_socket_timeout(sockfd, 5);
            
            // Attempt to connect
            int status = connect(sockfd, (struct sockaddr*)addr_ptr, addr_len);
            
            // Clean up
            close_socket(sockfd);
            cleanup_winsock();
            
            // Check connection result
            if (status < 0) {
                bool is_timeout, is_refused;
                int error_code = get_connection_error(is_timeout, is_refused);
                if (is_timeout) {
                    return NetworkResult(false, 3, "Connection timeout");
                } else if (is_refused) {
                    return NetworkResult(false, 4, "Connection refused");
                }
                return NetworkResult(false, error_code, "General network error");
            }
            
            return NetworkResult(true, 0, "Host is reachable");
        }
    
        /**
         * @brief Download file from URL
         * 
         * Downloads a file from the specified URL and saves it to the destination path.
         * This implementation performs an HTTP GET request to retrieve the file content.
         * 
         * @param url The URL to download from
         * @param destination The destination file path
         * @return NetworkResult containing success status and details
         * 
         * Error codes:
         * - 0: File downloaded successfully
         * - 1: URL is empty
         * - 2: Destination path is empty
         * - 6: Invalid URL format
         * - 7: Failed to create output file
         * - 8: Network error during download
         * - 9: HTTP error response
         */
        static NetworkResult download_file(const std::string& url, const std::string& destination) {
            // Validate input
            if (url.empty()) {
                return NetworkResult(false, 1, "URL is empty");
            }
            if (destination.empty()) {
                return NetworkResult(false, 2, "Destination path is empty");
            }
            
            // Simple URL validation (check if it starts with http:// or https://)
            if (url.find("http://") != 0 && url.find("https://") != 0) {
                return NetworkResult(false, 6, "Invalid URL format");
            }
            
            // Parse URL to extract protocol, host, port, and path
            std::string protocol, host, path;
            int port = 80;
            
            size_t protocol_end = url.find("://");
            if (protocol_end != std::string::npos) {
                protocol = url.substr(0, protocol_end);
                if (protocol == "https") {
                    // Note: This implementation doesn't actually handle HTTPS,
                    // it would require additional libraries like OpenSSL
                    port = 443;
                }
                
                size_t host_start = protocol_end + 3;
                size_t host_end = url.find("/", host_start);
                
                if (host_end == std::string::npos) {
                    host = url.substr(host_start);
                    path = "/";
                } else {
                    host = url.substr(host_start, host_end - host_start);
                    path = url.substr(host_end);
                }
                
                // Check if host contains port
                size_t port_pos = host.find(":");
                if (port_pos != std::string::npos) {
                    std::string port_str = host.substr(port_pos + 1);
                    host = host.substr(0, port_pos);
                    port = atoi(port_str.c_str());
                }
            } else {
                return NetworkResult(false, 6, "Invalid URL format");
            }
            
            // Convert port to string
            std::string port_str = std::to_string(port);
            
            // Platform-specific socket initialization
            if (!initialize_winsock()) {
                return NetworkResult(false, 8, "Failed to initialize Winsock");
            }
            
            // Resolve hostname to IP address
            struct addrinfo hints, *result = nullptr;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            
            int status = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result);
            if (status != 0) {
                cleanup_winsock();
                return NetworkResult(false, 8, "Hostname resolution failed: " + std::string(gai_strerror(status)));
            }
            
            // Create socket
            int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (sockfd < 0) {
                freeaddrinfo(result);
                cleanup_winsock();
                return NetworkResult(false, 8, "Failed to create socket");
            }
            
            // Set socket timeout (30 seconds)
            set_socket_timeout(sockfd, 30);
            
            // Attempt to connect
            status = connect(sockfd, result->ai_addr, result->ai_addrlen);
            if (status < 0) {
                freeaddrinfo(result);
                close_socket(sockfd);
                cleanup_winsock();
                return NetworkResult(false, 8, "Failed to connect to host");
            }
            
            // Send HTTP GET request
            std::string request = "GET " + path + " HTTP/1.1\r\n";
            request += "Host: " + host + "\r\n";
            request += "Connection: close\r\n\r\n";
            
            status = send(sockfd, request.c_str(), request.length(), 0);
            if (status < 0) {
                freeaddrinfo(result);
                close_socket(sockfd);
                cleanup_winsock();
                return NetworkResult(false, 8, "Failed to send HTTP request");
            }
            
            // Open output file
            FILE* file = fopen(destination.c_str(), "wb");
            if (!file) {
                freeaddrinfo(result);
                close_socket(sockfd);
                cleanup_winsock();
                return NetworkResult(false, 7, "Failed to create output file");
            }
            
            // Receive HTTP response and write to file
            char buffer[4096];
            bool headers_parsed = false;
            std::string headers;
            
            while ((status = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
                buffer[status] = '\0';
                
                if (!headers_parsed) {
                    headers += buffer;
                    size_t header_end = headers.find("\r\n\r\n");
                    if (header_end != std::string::npos) {
                        headers_parsed = true;
                        std::string header_part = headers.substr(0, header_end);
                        std::string body_part = headers.substr(header_end + 4);
                        
                        // Check HTTP status code
                        size_t status_pos = header_part.find(" ");
                        if (status_pos != std::string::npos) {
                            size_t status_end = header_part.find(" ", status_pos + 1);
                            if (status_end != std::string::npos) {
                                std::string status_code = header_part.substr(status_pos + 1, status_end - status_pos - 1);
                                int code = atoi(status_code.c_str());
                                if (code >= 400) {
                                    fclose(file);
                                    freeaddrinfo(result);
                                    close_socket(sockfd);
                                    cleanup_winsock();
                                    return NetworkResult(false, 9, "HTTP error: " + status_code);
                                }
                            }
                        }
                        
                        // Write body part to file
                        fwrite(body_part.c_str(), 1, body_part.length(), file);
                    }
                } else {
                    fwrite(buffer, 1, status, file);
                }
            }
            
            // Clean up
            fclose(file);
            freeaddrinfo(result);
            close_socket(sockfd);
            cleanup_winsock();
            
            if (status < 0) {
                return NetworkResult(false, 8, "Network error during download");
            }
            
            return NetworkResult(true, 0, "File downloaded successfully");
        }
    
        /**
         * @brief Perform an HTTP GET request
         * 
         * @param url The URL to request
         * @return std::string The HTTP response
         */
        static std::string http_get(const std::string& url) {
            // Simplified implementation
            // In a real implementation, this would perform an HTTP GET request
            return "HTTP response from " + url;
        }
        
        /**
         * @brief Perform an HTTP POST request
         * 
         * @param url The URL to request
         * @param payload The data to send in the POST request
         * @return std::string The HTTP response
         */
        static std::string http_post(const std::string& url, const std::string& payload) {
            // Simplified implementation
            // In a real implementation, this would perform an HTTP POST request
            return "HTTP POST response from " + url + " with payload: " + payload;
        }
        
        /**
         * @brief Perform an HTTPS GET request
         * 
         * @param url The URL to request
         * @return std::string The HTTPS response
         */
        static std::string https_get(const std::string& url) {
            // Simplified implementation
            // In a real implementation, this would perform an HTTPS GET request
            return "HTTPS response from " + url;
        }
        
        /**
         * @brief Perform an HTTPS POST request
         * 
         * @param url The URL to request
         * @param payload The data to send in the POST request
         * @return std::string The HTTPS response
         */
        static std::string https_post(const std::string& url, const std::string& payload) {
            // Simplified implementation
            // In a real implementation, this would perform an HTTPS POST request
            return "HTTPS POST response from " + url + " with payload: " + payload;
        }
        
        /**
         * @brief URL encode a string
         * 
         * @param value The string to encode
         * @return std::string The URL encoded string
         */
        static std::string url_encode(const std::string& value) {
            // Simplified implementation
            // In a real implementation, this would properly encode special characters
            return value; // Placeholder implementation
        }
        
        /**
         * @brief URL decode a string
         * 
         * @param value The string to decode
         * @return std::string The URL decoded string
         */
        static std::string url_decode(const std::string& value) {
            // Simplified implementation
            // In a real implementation, this would properly decode encoded characters
            return value; // Placeholder implementation
        }
        
        /**
         * @brief Get list of network interfaces
         * 
         * @return std::vector<std::string> List of network interface names
         */
        static std::vector<std::string> get_network_interfaces() {
            // Simplified implementation
            // In a real implementation, this would query the system for network interfaces
            return {"eth0", "wlan0", "lo"}; // Placeholder implementation
        }
        
        /**
         * @brief Validate IPv4 address
         * 
         * @param ip The IP address to validate
         * @return true if valid IPv4 address, false otherwise
         */
        static bool is_valid_ipv4(const std::string& ip) {
            // Simplified implementation
            // In a real implementation, this would validate the IPv4 format
            return !ip.empty(); // Placeholder implementation
        }
        
        /**
         * @brief Validate IPv6 address
         * 
         * @param ip The IP address to validate
         * @return true if valid IPv6 address, false otherwise
         */
        static bool is_valid_ipv6(const std::string& ip) {
            // Simplified implementation
            // In a real implementation, this would validate the IPv6 format
            return !ip.empty(); // Placeholder implementation
        }
        
        /**
         * @brief Create a socket connection
         * 
         * @param host The host to connect to
         * @param port The port to connect to
         * @return int Socket file descriptor or -1 on failure
         */
        static int create_socket_connection(const std::string& host, int port) {
            // Simplified implementation
            // In a real implementation, this would create an actual socket connection
            return 0; // Placeholder implementation
        }
        
        /**
         * @brief Close a socket connection
         * 
         * @param socket_fd The socket file descriptor to close
         * @return true if successful, false otherwise
         */
        static bool close_socket_connection(int socket_fd) {
            // Simplified implementation
            // In a real implementation, this would close the socket connection
            return socket_fd >= 0; // Placeholder implementation
        }
        
        /**
         * @brief Parse HTTP response code from response
         * 
         * @param response The HTTP response string
         * @return int The HTTP response code (e.g., 200, 404, 500)
         */
        static int parse_http_response_code(const std::string& response) {
            // Simplified implementation
            // In a real implementation, this would parse the actual HTTP response code
            return 200; // Placeholder implementation
        }
        
        /**
         * @brief Check if HTTP response code indicates success
         * 
         * @param response_code The HTTP response code
         * @return true if response code indicates success (2xx), false otherwise
         */
        static bool is_http_success(int response_code) {
            // Simplified implementation
            // In a real implementation, this would check if the response code is 2xx
            return response_code >= 200 && response_code < 300; // Placeholder implementation
        }
        
        /**
         * @brief Measure network latency to a host
         * 
         * @param host The host to ping
         * @param count Number of ping requests to send
         * @return double Average latency in milliseconds
         */
        static double measure_latency(const std::string& host, int count = 4) {
            // Simplified implementation
            // In a real implementation, this would perform actual ping requests
            return 10.5; // Placeholder implementation
        }
        
        /**
         * @brief Measure network bandwidth
         * 
         * @param host The host to test bandwidth with
         * @return double Bandwidth in Mbps
         */
        static double measure_bandwidth(const std::string& host) {
            // Simplified implementation
            // In a real implementation, this would perform actual bandwidth tests
            return 100.0; // Placeholder implementation
        }
    };

}

#endif // INTERLACED_CORE_NETWORK_HPP
