#pragma once

#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/ITHistSvc.h>
#include <TH1.h>
#include <memory>

namespace TrackPerf {}

class FilterTracksAlg : public GaudiAlgorithm {
	public:
		// Constructor
		FilterTracksAlg(const std::string& name, ISvcLocator* pSvcLocator);
		// Destructor
		virtual ~FilterTracksAlg();

		// Initialization method
		virtual StatusCode initialize();

		// Execute method (called for every event)
		virtual StatusCode execute();

		// Called after data processing for clean up.
		virtual StatusCode finalize();

	private:
		void buildBfield();


		//! Input track collection
		std::string m_InTrackCollection{};

		//! Output track collection
		std::string m_OutTrackCollection{};

		//! Cut off for total number of hits
		int m_NHitsTotal = 7;
		//! Cut off for number of hits in vertex detector (barrel and endcap combined)
		int m_NHitsVertex = 3;
		//! Cut off for number of hits in inner tracker (barrel and endcap combined)
		int m_NHitsInner = 2;
		//! Cut off for number of hits in outer tracker (barrel and endcap combined)
		int m_NHitsOuter = 1;

		//! Cut off for momentum
		float m_MinPt = 1.0;  // units GeV

		//! Default magnetic field value
		float m_Bz = 3.57;  // units Tesla
};

DECLARE_COMPONENT(FilterTracksAlg)
