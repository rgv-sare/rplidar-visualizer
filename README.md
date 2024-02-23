# RPLidar Visiualizer
A C++ application the reads serial input from a RPLidar sensor and display it's output.

![Lidar Visiualizer Screenshot](./screenshots/2024-02-23%2006-55-57.png)

Within the application, you are able to select the serial port to read from. Once it's connected and initialized, the application recieves a list of nodes to draw.

This application is a fork of [sare_init_visiulizer](https://github.com/rgv-sare/sare-init-visualizer) because it allows us to quickly render graphics (it's sort of a game engine). On top of that, it uses [ImGui](https://github.com/ocornut/imgui) for the UI, and, for communicating with LIDAR, the offical SDK from Slamtec, [rplidar_sdk](https://github.com/Slamtec/rplidar_sdk).

## How to Build & Run

At the moment, this application mainly intended to be used on Linux. MacOS and Windows builds are yet to be tested.

#### Prerequisites:
* CMake 3.1 or higher

Clone the repository.
```
git clone https://github.com/rgv-sare/rplidar-visualizer.git --recursive
```
Be sure to include the `--recursive` flag.

Head into the cloned repository.
```
cd rplidar-visualizer
```
```
mkdir build && cd build
```
Make and go into the `build` directory.
```
cmake ..
```
Run CMake to configure compilation.
### Linux
On Linux, run `make`.
```
make
```
Once compilation is completed, run it with:
```
./rplidar_visiualizer
```

## Troublshooting

If connecting to a serial port fails (a timeout, or failure to get device info), that port may not have the permissions needed for the application to work.

If on Linux, try setting the permissions for reading and writing to be allowed for all users (read/write).
```
sudo chmod a+rw /dev/ttyUSB0
```
**Note:** Replace `ttyUSB0` with the corresponding serial port the Lidar is connected to.

---
If compiling on Linux, be sure to use GCC compiler. This is because rplidar_sdk has some symbols that are not present in other compilers.

---

Be sure to clone this repository with the `--recursive` flag. During complation, it is required for the applicaiton's dependancies to be downloaded too or else it may trigger a `subdirectory doesn't exist` error from CMake.

---

If program refuses to run on Linux, try enabling permissions to execute:
```
chmod +x ./rplidar_visiualizer
```