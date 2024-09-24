# wirefish
Reimplementation of Wireshark CLI using PcapPlusPlus library

## Build instructions

```bash
# Clone this repository
$ gh repo clone bradyhelms/wirefish

# cd into the repository
$ cd fish 

# Run CMake
$ cmake -S . -B build

# Build
$ cmake --build build

# If everything builds successfully, run using:
$ sudo ./wirefish [OPTIONS]

# If you need help, run the executable without args to print usage.
$ sudo ./wirefish
```
