# ntwii
Control your FRC robot with a wiimote.

## Motivation
yes.

## Running the Program
You're going to need some tools to get started.
- A working C++ compiler (clang, gcc, MSVC, etc.)
- Git
- CMake
- Ninja

Once you clone the repo, download external dependencies.
```sh
git submodules update --init --recursive
```

You're going to want to configure the program to connect to your robot/development environment.
At the top of the `src/main.cc` file, there's two constants. Make sure to change them to your liking.
```c++
// Configure me!
constexpr unsigned int TEAM_NUMBER = 2234;
constexpr bool DEBUG = true; // (enable to connect to local simulation)

// ... rest of code ...
```

Then, compile in typical CMake fashion.
```sh
mkdir build
cd build
cmake .. -G "Ninja"
ninja -j16 # where 16 = the number of physical CPU cores you have. If unsure, omit.
```

The executable file will be in the build folder, named `ntwii.exe` (no exe extention if not on Windows)

Running the file will first wait for a NetworkTables connection (connection w/ robot), and will then attempt
to connect to the Wii controller.

### IMPORTANT!
You must connect the Wii controller to Windows first. Go to Settings->Bluetooth->Add a Device.
The Wii controllers do not speak perfect Bluetooth and as such, require a bit of coaxing to get working.

If it asks you for a PIN, there is no valid PIN. Press cancel and try again. If you're not getting anywhere,
restart Bluetooth by turning it off and on again.

**Do not use a Wiimote driver!** They will alter how the Wiimote communicates with the program and probably
break things.

## In Robot Code
The program will spit out raw Wiimote gyro data into the `ntwii` network table.
The structure will be identical to the one defined in `src/main.cc`:
```c++
struct WiimoteData {
    // Gyro data
    double pitch, roll, yaw;
    // Button states
    bool a, b, one, two, plus, minus, home;
};
```