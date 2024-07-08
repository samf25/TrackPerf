#include "FilterTracksAlg.hxx"
#include <DD4hep/Detector.h>
#include <edm4hep/TrackCollection.h>
#include <edm4hep/Track.h>
#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/ServiceHandle.h>
#include <math.h>

// Implement constructor
FilterTracksAlg::FilterTracksAlg(const std::string& name, ISvcLocator* pScvLocator) : GaudiAlgorithm(name, pSvcLocator) {
	declareProperty("NHitsTotal", m_NHitsTotal, "Minimum number of hits on track");
	declareProperty("NHitsVertex", m_NHitsVertex, "Minimum number of hits on vertex detector");
	declareProperty("NHitsInner", m_NHitsInner, "Minimum number of hits on inner tracker");
	declareProperty("NHitsOuter", m_NHitsOuter, "Minimum number of hits on outer tracker");
	declareProperty("MinPt", m_MinPt, "Minimum transverse momentum");
	declareProperty("InputTrackCollectionName", m_InTrackCollection, "Name of the input collection");
	declareProperty("OutputTrackCollectionName", m_OutTrackCollection, "Name of the output collection");
}

// Implement destructor
FilterTracksAlg::~FilterTracksAlg() {}

// Implement initialize
StatusCode FilterTracksAlg::initialize() {
	// set things up
	info() << "FilterTracksAlg Initialization" << endmsg;
	buildBfield();

	return StatusCode::SUCCESS;
}

// Build magnetic field
void FilterTracksAlg::buildBfield() {
	// Get magnetic field
	dd4hep::Detector& lcdd = dd4hep::Detector::getInstance();
	const double position[3] = {0, 0, 0}		// position to calculate magnetic field (here, the origin)
	double magneticFieldVector[3] = {0, 0, 0}	// initialise object to hold magnetic field
	lcdd.field().magneticField(
			position, magneticFieldVector);	// get the magnetic field vector from DD4hep
	m_Bz = magneticFieldVector[2] / dd4hep::tesla;
}

// Implement execute
StatusCode FilterTracksAlg::execute() {
	// Retrieve input and create output collections
	const edm4hep::TrackCollection tracks = get<edm4hep::TrackCollection>(m_InTrackCollection);
	edm4hep::TrackCollection outputTracks = std::make_unique<edm4hep::TrackCollection>();

	// Filter tracks
	for (const edm4hep::Track& trk : *tracks) {
		int nhittotal = trk.trackerHits_size();
		if (m_NHitsTotal > 0 && nhittoal <= m_NHitsTotal) continue; // Hit count check

		int nhitvertex = trk.getSubdetectorHitNumbers(1) + trk.getSubdetectorHitNumbers(2);
		if (_NHitsVertex > 0 && nhitvertex <= _NHitsVertex) continue;

		int nhitinner = trk.getSubdetectorHitNumbers(3) + trk.getSubdetectorHitNumbers(4);
		if (_NHitsInner > 0 && nhitinner <= _NHitsInner) continue;

		int nhitouter = trk.getSubdetectorHitNumbers(5) + trk.getSubdetectorHitNumbers(6);
		if (_NHitsOuter > 0 && nhitouter <= _NHitsOuter) continue;

		float pt = fabs(0.3 * m_Bz / trk.getOmega() / 1000);
		if (m_MinPt > 0 && pt < m_MinPt) continue; // pT check
		
		outputTracks->push_back(trk); // add tracks that pass all tests
	}

	// Save output track collection
	put(m_OutTrackCollection, std::move(outputTracks));

	return StatusCode::SUCCESS;
}

// Implement finalize
StatusCode FilterTracksAlg:finalize() {
	info() << "FilterTracksAlg Finalize" << endmsg;
	return StatusCode::SUCCESS;
}
