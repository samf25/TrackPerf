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

# extra options
OPTION(WITH_PARQUET "Enable output to Apache Parquet." OFF)

### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

FIND_PACKAGE( Marlin 1.0 REQUIRED ) # minimum required Marlin version

FIND_PACKAGE( ROOT REQUIRED COMPONENTS Hist)

FIND_PACKAGE( RAIDA REQUIRED )

IF(WITH_PARQUET)
  FIND_PACKAGE( Arrow  REQUIRED )

  GET_FILENAME_COMPONENT(Arrow_SEARCH_DIR ${Arrow_CONFIG} DIRECTORY)
  FIND_PACKAGE(Parquet REQUIRED HINTS ${Arrow_SEARCH_DIR})
ELSE()
  MESSAGE("Parquet output disabled.")
ENDIF()

### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ENDIF()

### DATA ###########################################################

### LIBRARY #################################################################

# Main library with Processors
ADD_LIBRARY(TrackPerf SHARED
  ./src/TrackPerfHistProc.cxx
  ./src/TrackHists.cxx
  ./src/TruthHists.cxx
  ./src/FilterTracks.cxx
  )
TARGET_INCLUDE_DIRECTORIES(TrackPerf
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
  ${Marlin_INCLUDE_DIRS}
  ${RAIDA_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(TrackPerf
  PRIVATE
  ${RAIDA_LIBRARIES}
  ROOT::Hist)
TARGET_COMPILE_DEFINITIONS(TrackPerf
  PRIVATE
  ${RAIDA_DEFINITIONS})

IF(WITH_PARQUET)
  TARGET_SOURCES(TrackPerf PRIVATE
    ./src/TrackPerfParquet.cxx
    ./src/TrackPerfFile.cxx)
  TARGET_LINK_LIBRARIES(TrackPerf PRIVATE LCParquet)
ENDIF()
