#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <string>
#include <cstring>
#include "Cache.h"  // Assuming you have a Cache class implemented elsewhere

using namespace std;

// Define directories and file paths needed to execute the benchmark program
string BENCHMARK_DIR = "./vcet_bench/";  // Directory containing the benchmark executable
string PIN_DIR = "../../../pin/";        // Path to the Pin tool directory
string PINFLAGS = "-t ";                 // Flag for running the Pin tool
string PINTRACER_DIR = "-- ./obj-intel64/memtracer.so -- ";  // Path to the Pin tracer (shared object) which traces memory accesses
string benchmark;  // Takes the benchmark filename from the 15th argument

int main(int argc, char** argv) {
    // Cache configuration setup
    unsigned int l1_cache_size = atoi(argv[1]) * 1024;
    unsigned int num_ways = atoi(argv[2]);
    unsigned int block_size = atoi(argv[3]);
    unsigned int num_sets = l1_cache_size / (block_size / num_ways);
    unsigned short int replacement_policy = 1;  // lru policy
    unsigned int write_policy = 1;  // write-back policy

    // Create the L1 cache object (assuming mem::Cache is implemented)
    mem::Cache* L1 = new mem::Cache("L1 Cache", num_sets, num_ways, block_size, replacement_policy, write_policy);

    // Define the benchmark
    benchmark = argv[15];

    // Construct the full command to execute the benchmark using the Pin tool and tracer
    string cmd = PIN_DIR + PINFLAGS + PINTRACER_DIR + BENCHMARK_DIR + benchmark;
    cout << cmd << endl;

    // Buffer to store the output from the executed command
    char buffer[128];

    // Open a pipe to run the constructed command and read its output
    FILE* pipe = popen((const char*)cmd.c_str(), "r");
    if (!pipe) throw runtime_error("popen() failed!");

    try {
        // Loop to read the command output line by line
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            // Assumes PinTool output is in the format AccessType:Address
            // For example:
            // W:0x00007fffffff3e78  (Write access at address)
            // Or: R:0x00007fffffff3e78  (Read access at address)

            string delimiter = ":";
            size_t pos = 0;
            string token;
            string ins = buffer;  // Convert the buffer content to a string for easier manipulation

            // Find the position of the delimiter ":"
            if ((pos = ins.find(delimiter)) != string::npos) {
                token = ins.substr(0, pos);  // Extract the AccessType (either "R" or "W")

                // If AccessType is "R" (read) or "W" (write), process it further
                if (token.compare("R") == 0 || token.compare("W") == 0) {
                    string AccessType = ins.substr(0, ins.find(delimiter));  // Extract the AccessType
                    ins.erase(0, ins.find(delimiter) + delimiter.length());  // Remove the AccessType part and keep the address
                    string address = ins.substr(0, ins.find(delimiter));  // Extract the address

                    // TODO: Call the Access method from the Cache class and pass the AccessType and address
                    // For now, just print the parsed AccessType and Address
                    cout << "AccessType: " << AccessType << ", Address: " << address << endl;

                    // Example of calling cache method (assuming your Cache class has a method for this)
                    // L1->Access(AccessType, address);
                }
            }
        }
    } catch (...) {
        // Ensure the pipe is closed and rethrow any caught exceptions
        pclose(pipe);
        throw;
    }

    // Close the pipe after finishing reading
    pclose(pipe);

    return 0;
}
