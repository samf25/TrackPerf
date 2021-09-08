########################################################
# cmake file for building Marlin TrackPerf Package
# @author Karol Krizka
CMAKE_MINIMUM_REQUIRED(VERSION 3.15 FATAL_ERROR)
########################################################

# project name
PROJECT( TrackPerf )

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

FIND_PACKAGE( Marlin 1.0 REQUIRED ) # minimum required Marlin version

FIND_PACKAGE(Arrow REQUIRED)

GET_FILENAME_COMPONENT(Arrow_SEARCH_DIR ${Arrow_CONFIG} DIRECTORY)
FIND_PACKAGE(Parquet REQUIRED HINTS ${Arrow_SEARCH_DIR})

### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ENDIF()

### DATA ###########################################################

### LIBRARY #################################################################

# sources
ADD_LIBRARY(TrackPerf SHARED
  ./src/TrackPerfParquet.cxx
  ./src/TrackPerfFile.cxx
  )
TARGET_INCLUDE_DIRECTORIES(TrackPerf
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
  ${Marlin_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(TrackPerf PRIVATE LCParquet)
