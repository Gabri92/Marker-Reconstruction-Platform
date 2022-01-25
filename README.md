# Marker-Reconstruction-Platform
This code manages the controller of a platform used for image analysis. It is composed basically of the main, in which the process is executed as a finite state machine, and three dedicated libraries. Into the libraries it can be found the following functionalities:

1) Managing of the communication between the controller and a FastApi server
2) Managing of serial communication with the hardware dedicated to the image analysys (Nvidia Jetson Nano)
3) Communication between the controller and a thingsboard interface, via MQTT
4) Control of the platform's hardware

<b>Hardware</b>: Arduino mkr Wifi 1010<br>
<b>IDE</b>: Visual Studio Code

N.B. = The software development is still ongoing, due to the lack of part of the hardware

