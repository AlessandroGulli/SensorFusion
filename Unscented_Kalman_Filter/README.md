# Unscented Kalman Filter
The **Unscented Kalman Filter** takes in noisy measurement data as input and provides a robust estimation of displacement and velocity values of obstacles. 

<img src="images/CTVR.png" width="700" height="400" />

The **Constant Turn Rate and Velocity Magnitude Model (CTRV) model** is used to provide predictions of future state of obstacle, which is then weighted against sensor readings to provide reliable estimations that minimizes prediction or sensor error. 
In the image referenced below, the green path represents the predicted path by the Kalman Filter. The Root Mean Squared Error (RMSE) between estimation and ground truth values is successfully minimized.

<img src="media/ukf_highway_tracked.gif" width="700" height="400" />

# Predict Step
<img src="images/ukf_prediction1.png" width="700" height="400" />

<img src="images/ukf_prediction2.png" width="700" height="400" />

# Update Step

<img src="images/ukf_update1.png" width="700" height="400" />

<img src="images/ukf_update2.png" width="700" height="400" />

#
The main program can be built and ran by doing the following from the project top directory.

1. mkdir build
2. cd build
3. cmake ..
4. make
5. ./ukf_highway



<img src="media/ukf_highway.png" width="700" height="400" />

`main.cpp` is using `highway.h` to create a straight 3 lane highway environment with 3 traffic cars and the main ego car at the center. 
The viewer scene is centered around the ego car and the coordinate system is relative to the ego car as well. The ego car is green while the 
other traffic cars are blue. The traffic cars will be accelerating and altering their steering to change lanes. Each of the traffic car's has
it's own UKF object generated for it, and will update each indidual one during every time step. 

The red spheres above cars represent the (x,y) lidar detection and the purple lines show the radar measurements with the velocity magnitude along the detected angle. 
The Z axis is not taken into account for tracking, so you are only tracking along the X/Y axis.

---

## Other Important Dependencies
* cmake >= 3.5
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
 * PCL 1.2

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./ukf_highway`

