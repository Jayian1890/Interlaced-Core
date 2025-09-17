# interlaced_core

A C++ core library providing essential utilities for filesystem operations, JSON parsing, logging, and network operations.

## Features

- **Filesystem**: Utilities for file and directory operations
- **JSON**: Simple JSON parsing and generation
- **Logging**: Configurable logging with multiple levels
- **Network**: Basic network operations and HTTP requests

## Building

This project uses CMake for building:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Include the header files in your project:

```cpp
#include "interlaced_core/filesystem.hpp"
#include "interlaced_core/json.hpp"
#include "interlaced_core/logging.hpp"
#include "interlaced_core/network.hpp"
```

## License

MIT