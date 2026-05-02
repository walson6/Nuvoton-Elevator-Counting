# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection")
  file(MAKE_DIRECTORY "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection")
endif()
file(MAKE_DIRECTORY
  "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/1"
  "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection"
  "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection/tmp"
  "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection/src/ObjectDetection+ObjectDetection-stamp"
  "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection/src"
  "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection/src/ObjectDetection+ObjectDetection-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection/src/ObjectDetection+ObjectDetection-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/jimmy/Desktop/Nuvoton/Nuvoton-Team/repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/tmp/ObjectDetection+ObjectDetection/src/ObjectDetection+ObjectDetection-stamp${cfgdir}") # cfgdir has leading slash
endif()
