#pragma once

// edm4hep
#include <edm4hep/TrackCollection.h>

// Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiAlg/Transformer.h>
#include <k4FWCore/BaseClass.h>

// k4FWCore
#include <k4WFCore/DataHandle.h>

#include <memory>

namespace TrackPerf {}

class FilterTracksAlg : public Gaudi::Functional::Transformer<edm4hep::TrackCollection>(const edm4hep::TrackCollection&) {
	public:
		// Constructor
		FilterTracksAlg(const std::string& name, ISvcLocator* pSvcLocator);

		StatusCode initialize();

		edm4hep::TrackCollection operator() const;

	private:
		void buildBfield();

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
