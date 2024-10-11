# Pin Tool for Dynamic Binary Instrumentation

## Introduction

This README provides instructions for setting up, compiling, and running a very simple Pin tool designed to instrument an executable program. The Pin tool allows you to analyze and simulate memory usage by inserting instrumentation code into the target program dynamically.

## Dependencies

To use this Pin tool, you must first download and install the Intel Pin tool. 

1. **Download the Pin tool:**
   - Visit [Intel Pin Tool Download Page](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html) to download the Pin tool.

2. **Install Pin tool:**
   - After downloading the Pin tool, decompress the file.
   - It is recommended to add the Pin tool folder to your system's `PATH` environment variable. This ensures that the Pin tool is recognized from any directory in the terminal.

## How to Compile the Pin Tool

Compiling the Pin tool depends on how you have set up the Pin tool on your system. You may create Makefile that build and run the simulator for you. However, the following steps outline a straightforward approach:

1. **Move the Source Files:**
   - Move `PinTool.cpp` and any required source files (e.g., `Cache.cpp`, `Cache.h`) to the `ManualExamples` subfolder located at `/pin-3.30/source/tools/`.
   - This ensures that `PinTool.cpp` can locate and include the necessary files.

2. **Compile the Tool:**
   - Navigate to the `ManualExamples` directory:
     ```bash
     cd /pin-3.30/source/tools/ManualExamples
     ```
   - Execute the following command to build the tool:
     ```bash
     make obj-intel64/PinTool.so TARGET=intel64
     ```
   - This command compiles the `PinTool.cpp` file and produces the shared library `PinTool.so`.

3. **Move the Compiled Tool:**
   - After compilation, move the `PinTool.so` file from the `obj-intel64` subfolder to your assignment folder where you intend to use the tool.

## How to Run It

To instrument an executable program with your Pin tool, use the following command:

```bash
pin -t obj-intel64/PinTool.so -- path_to_the_executable_to_instrument

