# Marker-Reconstruction-Platform
This code manage the controller of a platform used for image analysis. It is composed basically of the main, in which the process is executed as a finite state machine, and three libraries. Into the libraries it can be found the following functionalities:

1) Managing of the communication between the controller and a FastApi server
2) Managing of serial communication with the hardware dedicated to the image analysys and neural network (Nvidia Jetson Nano)
3) Communication between the controller and a thingsboard interface, via MQTT

N.B. = The software development is still ongoing, due to lack of part of the hardware
