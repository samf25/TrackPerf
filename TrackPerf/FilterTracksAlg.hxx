#pragma once
// edm4hep
#include <edm4hep/TrackCollection.h>

// Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiAlg/Transformer.h>
#include <Gaudi/Property.h>
#include <k4FWCore/BaseClass.h>

// k4FWCore
#include <k4FWCore/DataHandle.h>

#include <memory>

namespace TrackPerf {}

struct FilterTracksAlg final : Gaudi::Functional::Transformer<edm4hep::TrackCollection(const edm4hep::TrackCollection&)> {
	public:
		// Constructor
		FilterTracksAlg(const std::string& name, ISvcLocator* pSvcLocator);

		StatusCode initialize();

		edm4hep::TrackCollection operator()(const edm4hep::TrackCollection& tracks) const;

	private:
		void buildBfield();

		//! Cut off for total number of hits
		Gaudi::Property<int> m_NHitsTotal{this, "NHitsTotal", 7, "Minimum number of hits on track"};
		//! Cut off for number of hits in vertex detector (barrel and endcap combined)
		Gaudi::Property<int> m_NHitsVertex{this, "NHitsVertex", 3, "Minimum number of hits on vertex detector"};
		//! Cut off for number of hits in inner tracker (barrel and endcap combined)
		Gaudi::Property<int> m_NHitsInner{this, "NHitsInner", 2, "Minimum number of hits on inner tracker"};
		//! Cut off for number of hits in outer tracker (barrel and endcap combined)
		Gaudi::Property<int> m_NHitsOuter{this, "NHitsOuter", 1, "Minimum number of hits on outer tracker"};

		//! Cut off for momentum
		Gaudi::Property<float> m_MinPt{this, "MinPt", 1.0, "Minimum transverse momentum"};  // units GeV

		//! Default magnetic field value
		float m_Bz = 3.57;  // units Tesla
};
