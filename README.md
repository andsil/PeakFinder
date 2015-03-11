# PeakFinder
This project aims to calculate the bright points of an image without any user input, as efficiently as possible. 

The target system is a cluster based computer where are hundreds of cpus/threads.
Because of this, the project will include OpenMP pragmas.

At certain point, there are plans to make the application run in the Intel Xeon Phi platform.

# Library requirements

The end user must have libtiff installed.

For Linux OS just do the following command:

`$ sudo apt-get install libtiff4 libtiff4-dev`

# Project structure

At this point, the application is being made in Netbeans IDE. So, there are files regarding this IDE included in the repository.
