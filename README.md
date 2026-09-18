# Sentinel-Vault 🛡️

A high-performance C++ Security Information and Event Management (SIEM) prototype.

## Key Features
- **Fast Ingestion:** Parses raw auth logs into structured memory.
- **Merge Sort Chronology:** Reconstructs event timelines in $O(n \log n)$ time.
- **AVL Tree Threat Engine:** Real-time IP blacklisting with logarithmic lookup speeds.
- **RAID 1 Persistence:** Binary vault storage with automated mirror recovery.

## Tech Stack
- **Language:** C++17
- **Data Structures:** AVL Trees, Vectors, Bit-fields
- **Algorithms:** Merge Sort, Binary I/O, RAID 1 Logic

## How to Run
1. `g++ -Iinclude src/main.cpp src/SentinelVault.cpp -o bin/Sentinel-Vault`
2. `./bin/Sentinel-Vault`