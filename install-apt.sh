#!/bin/sh

sudo apt-get update -qq
sudo apt-get install build-essential gfortran f2c \
     autoconf automake libtool autoconf-archive autotools-dev \
     maxima libblas-dev liblapack-dev \
     libglew-dev libsdl2-dev \
     libompl-dev \
     doxygen groff \
     sbcl \
     default-jdk \
     blender flex povray libav-tools \
     coinor-libclp-dev libglpk-dev liblpsolve55-dev
