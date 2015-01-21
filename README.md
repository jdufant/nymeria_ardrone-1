[nymeria_ardrone](https://sites.google.com/site/projetsecinsa/projets-2014-2015/projet-nymeria) : Obstacle detection and avoidance for ARDrone 2.0
===============

nymeria_ardrone is a [ROS](http://ros.org/ "Robot Operating System") package for [Parrot AR-Drone](http://ardrone2.parrot.com/) quadrocopter. It acts as a layer and filters drone commands sent from an external controller. It helps the drone determine if movement orders are safe or not depending on the trajectory of an obstacle and, if so, to move accordingly. In practice it contains three main modules. The first, linked to sensors, allows the drone to detect an obstacle. The second gets drone commands and the last makes the link between them. User defines radius of an obstacle and drone is controlled by Nymeria to slow down and stop in front of it. The driver supports AR-Drone 2.0.

## Table of Contents

- [Requirements](#requirements)
- [Installation](#installation)
- [How to run it](#how-to-run)
- [How does it work](#how-does-it-work)
- [Some examples](#some-examples)

## Requirements

- *ROS*: [Robot Operating System](http://wiki.ros.org/ROS/Installation)
- *ardrone_autonomy*: [Driver for Ardrone 1.0 & 2.0](https://github.com/AutonomyLab/ardrone_autonomy)
- *Sensor*: any kind of tool enabling to retrieve range between drone and front obstacles

## Installation

The first step is to install ROS following the [(Robot Operating System installation tutorial)](http://wiki.ros.org/ROS/Installation). We have successfully tested two versions : hydro and indigo.

Then create a [ROS workspace](http://wiki.ros.org/ROS/Tutorials/InstallingandConfiguringROSEnvironment#Create_a_ROS_Workspace).

In order to communicate with the drone you will need to download [ardrone_autonomy](https://github.com/AutonomyLab/ardrone_autonomy) which provide the ardrone_driver. Follow the instruction in the [installation section](https://github.com/AutonomyLab/ardrone_autonomy#installation).

Navigate to your catkin_workspace sources repository.
> $ cd ~/catkin_ws/src

Download the nymeria_ardrone package using the following command in a terminal.
> $ git clone https://github.com/ProjectNymeria/nymeria_ardrone

You might prefer to reach [nymeria_ardrone webpage](https://github.com/ProjectNymeria/nymeria_ardrone) and download and unpack the nymeria_ardrone package.

Go back to your root workspace repository.
> $ cd ~/catkin_ws

Use the catkin_make command to compile
> $ catkin_make

## How to run it

First switch on Wifi on your computer and connect it to your Ardrone 2.0.

You must launch the master node. Navigate to your catkin_workspace (`$ cd ~/catkin_ws`) and type the following command :
> $ roscore

Then launch the ardrone_autonomy driver's executable node. You can use :
> $ rosrun ardrone_autonomy ardrone_driver

Or put it in a custom launch file with your desired parameters.

Navigate to ~/catkin_ws/src/nymeria_ardrone/src/SensorInterface.cpp and find the line
*nco.inputCurFrontDist(cutValue);* Replace the 'cutValue' variable by the current distance of the front sensor of your drone. Once done, run the sensor_interface node :
> $ rosrun nymeria_ardrone nymeria_sensor_interface

By default the security distance is 100 cm.
To change it just call the setSecurityDist(double secDist) from the class NymeriaCheckObstacle.
~~~~~~~~~~~~~{.cpp}
double getSecurityDist();
void setSecurityDist(double secDist);
~~~~~~~~~~~~~

By default the sensor max range is 350 cm.
To change it just call the setSensorMaxRange(double range) from the class NymeriaCheckObstacle.
~~~~~~~~~~~~~{.cpp}
double getSensorMaxRange();
void setSensorMaxRange(double range);
~~~~~~~~~~~~~

Launch the nymeria_command executable node using :
> $ rosrun nymeria_ardrone nymeria_command

This node is the interface between you as a user who wish to send orders and the drone. Command are sent from keystroke detailled below.
- *ENTER* : LAND / TAKE OFF
- *Z* : move forward
- *S* : move backward
- *Q* : rotate left
- *D* : rotate right
- *UP* : move up
- *DOWN* : move down
- *i* : move down
- *k* : move down
- *o* : move down
- *l* : move down
- *p* : move down
- *m* : move down
- *SPACE* : stop

The last step consists to run the launch the Controller node
> $ rosrun nymeria_ardrone controller

You are ready to go. Just stroke the appropriate key from the nymeria_command interface. Your drone will naturally keep the inputed security distance between any front obstacle and itself.

## How does it work

As explain in the introduction above, nymeria_ardrone uses various nodes.
+ *roscore* is the implicit one. It is a ROS requirement allowing nodes to communicate with each others.

+ *ardrone_driver* allows communicating with the drone.

+ *nymeria_sensor_interface* runs constantly, and obtains data from any kind of tool that enables you to retrieve the distance between the drone and obstacles in front.

    It also gets the pitch which is the degree of inclination of the drone and represents its speed.

    This component then provides a regulated speed factor for the drone.

+ *nymeria_command* is the interface with the user. From there, he sends navigation commands to the drone.

+ *nymeria_controller* takes into account all the parameters and determines whether or not, the user's command is safe for the drone in terms of obstacle& detection.

    Just as the sensor interface, it runs constantly since dangers can arise anytime.

    The first action of the controller is to check the user's command : 

    If the command includes a rotating move, or the modification of configuration parameters such as the speed then it processes it immediately.
    
    If the command represents a linear move, it might not be safe because, in that case, the drone can run into an     obstacle. So there are two choices : either the security distance provided by the user has already been violated, or we can still anticipate the obstacle.
    
    In this last case we apply a slow down algorithm which adapts the speed factor periodically based on the current     distance from an obstacle in front : if the obstacle is far from the drone, the speed factor is highest. If the obstacle is close to the drone the speed factor is lowest.
    
    This allows for smoothly stopping at the desired security distance.
    
    If the security distance has already been violated then nymeria quickly reacts by stopping the drone and moving backwards in order to keep the security distance.
    
    The latter behavior conducts the drone in every critical situation : it must keep a security distance. That means that if the drone is running an unsafe action such as moving forward, and an obstacle is approaching it, then the drone must react and move backwards in order to keep the security distance. It also means that, if the user inputs a new command while the security distance is not keeped, the command will be ignored until the security distance is reestablished.
    
Nymeria is a library providing navigation commands which can be for instance accessed by the controller, the user and so on. 

The graph below summarizes the behavior of the *nymeria_ardrone* package.

![nymeria_ardrone behavior](https://raw.githubusercontent.com/ProjectNymeria/nymeria_ardrone/master/img/nymeria_behavior.jpg)

## Some examples

In this package you will find the *sensor.ino* file in the *arduino/* repository.
In order to test our librairy we used an arduino nano coupled with an ultrasonic sensor and linked by the USB port to the drone. The simple code in *sensor.ino* enables us to retrieve the distance between the drone and obstacles in front. Belows is a part of it.
~~~~~~~~~~~~~{.java}
[...]

// convert the time into a distance
cm = microsecondsToCentimeters(duration);

// cast distance into a string
String dist = String(cm);

// send distance
bytesSent = Serial.print(dist);

// send separator
Serial.print('x');

[...]
~~~~~~~~~~~~~

You will also find the *embedded/* repository.
Still for test purposes we needed to embed a server on the drone so that the sensor_interface, by simply making request to it, can retrieve the distance from the sensor. The *DroneUDPServer.elf* is the compiled version of the *DroneUDPServer.cpp*. If you want to run your own version of the server please refer to the [Cross compilation tutorial](https://sites.google.com/site/projetsecinsa/documents-techniques/outils-et-methodes/cross-compilation-ar-drone-2-0) we followed.
It first connects to the USB serial then send the distance read.

~~~~~~~~~~~~~{.cpp}
[...]

/* Open serial port
try to open ttyUSB0 first. If failed, try to open ttyUSB1
*/
fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY | O_NDELAY);

[...]

// Open UDP server //
UDPServer server("192.168.1.1", 7777);

[...]

bsent = server.send(sendBuffer, 4);

[...]
~~~~~~~~~~~~~

By default the UDP port is 7777. If you would like to change it, do not forget to make the update in the NymeriaCheckObstacle too.
