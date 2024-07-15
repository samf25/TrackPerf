########################################################
# cmake file for building Marlin TrackPerf Package
# @author Karol Krizka, Samuel Ferraro
CMAKE_MINIMUM_REQUIRED(VERSION 3.15 FATAL_ERROR)
########################################################

# project name
PROJECT( TrackPerf VERSION 0.2.0 )

### DEPENDENCIES ############################################################

FIND_PACKAGE( Gaudi REQUIRED )
FIND_PACKAGE( EDM4HEP REQUIRED )
FIND_PACKAGE( DD4hep REQUIRED COMPONENTS DDCore )
FIND_PACKAGE( ROOT REQUIRED )
FIND_PACKAGE( k4FWCore REQUIRED )

### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ENDIF()

### DATA ###########################################################

### LIBRARY #################################################################

# Main library with Processors
ADD_LIBRARY(TrackPerf SHARED
	./src/TrackPerfHistAlg.cxx
	./src/TrackHists.cxx
	./src/TruthHists.cxx
	./src/FilterTracksAlg.cxx
	./src/ResoHists.cxx
  )
TARGET_INCLUDE_DIRECTORIES(TrackPerf
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${Gaudi_INCLUDE_DIRS}
	${EDM4HEP_INCLUDE_DIRS}
	${ROOT_INCLUDE_DIRS}
	${DD4hep_INCLUDE_DIRS}
	${k4FWCore_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(TrackPerf
	PRIVATE
	Gaudi::GaudiKernel
	Gaudi::GaudiAlg
	EDM4HEP::edm4hep
	ROOT::Hist
	${ROOT_LIBRARIES} 
	DD4hep::DDCore
	k4FWCore::k4FWCore
)
