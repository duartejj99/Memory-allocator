# syntax=docker/dockerfile:1

#Base
FROM gcc:latest AS base
WORKDIR /malloc



# Build tools:
#   *build-essentials who has gcc, g++, and Make 
#   *libgtest-dev for the google unit test library
#   *cmake
RUN apt update &&   \
    apt install -y  \
    build-essential libgtest-dev cmake 

#Python header
RUN apt install -y python-dev-is-python3

COPY . .
RUN mkdir build/
 
CMD ["/bin/bash"]


