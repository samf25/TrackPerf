#include "TrackPerfHistAlg.hxx"

#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/ServiceHandle.h>
#include <TH1.h>
#include <TFile.h>

#include "TrackPerf/ResoHists.hxx"
#include "TrackPerf/TrackHists.hxx"
#include "TrackPerf/TruthHists.hxx"

// Implement Constructor
TrackPerfHistAlg::TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator) : GaudiAlgorithm(name, pSvcLocator) {
	declareProperty("TrackCollection", m_trkColName, "Name of the Track collection");
	declareProperty("MCParticleCollection", m_mcpColName, "Name of the MCParticle collection");
	declareProperty("MCTrackRelationCollection", m_trkMatchColName, "Name of LCRelation collection with track to MC matching");
	declareProperty("MatchProb", m_matchProb, "Minimum matching probability to be considered a good track-MC match.");
}

// Implement Destructor
TrackPerfHistAlg::~TrackPerfHistAlg() {}

// Implement Initializer
StatusCode TrackPerfHistAlg::initialize() {
	// Get Histogram Service
	ServiceHandle<ITHistSvc> histSvc("THistSvc", name());
	if (!histSvc) {
		error() << "Unable to locate Histogram service" << endmsg;
		return StatusCode::FAILURE;
	}

	// Make Histograms
	m_allTracks = std::make_shared<TrackPerf::TrackHists>();
	m_allTruths = std::make_shared<TrackPerf::TruthHists>();
	m_hNumber_of_tracks = new TH1F("hNumber_of_tracks", "Number of seeds/tracks;Events", 100, 0, 300000);
	histSvc->regHist("/histos/hNumber_of_tracks", m_hNumber_of_tracks);

	m_realTracks = std::make_shared<TrackPerf::TrackHists>();
	m_realTruths = std::make_shared<TrackPerf::TruthHists>();
	m_realReso = std::make_shared<TrackPerf::ResoHists>();
	m_fakeTracks = std::make_shared<TrackPerf::TrackHists>();
	m_hNumber_of_fakes = new TH1F("hNumber_of_fakes", "Number of fake tracks;Events", 100, 0, 300000);
	histSvc->regHist("/histos/hNumber_of_fakes", m_hNumber_of_fakes);

	m_unmtTruths = std::make_shared<TrackPerf::TruthHists>();
	
	return StatusCode::SUCCESS;
}

// Implement Execute (To be run on each event -- the workhorse)
StatusCode TrackPerfHistAlg::execute() {
	// Access data from EDM4hep (If intially input is in LCIO format,m convert using lcio2edm4hep executable that comes with key4hep)
	const edm4hep::MCParticleCollection			mcParticles = get<edm4hep::MCParticleCollection>(m_mcpColName);
	const edm4hep::TrackCollection				tracks = get<edm4hep::TrackCollection>(m_trkColName);
	const edm4hep::MCRecoTrackParticleAssociationCollection	trackToMCRelations = get<edm4hep::MCRecoParticleAssociationCollection>(m_trkMatchColName);

	// MC Particles
	std::set<const edm4hep::MCParticle*> mcpSet;
	for (const MCParticle& mcp : *mcParticles) {
		if (mcp.getGeneratorStatus() != 1) continue;
		if (mcp.getCharge() == 0) continue;
		if (mcp.isDecayedInTracker()) continue;

		// Tracker Acceptance
		const edm4hep::Vector3d& mom = mcp.getMomentum();
		double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
		double lambda = std::atan2(mom.z, pt);
		if (fabs(lambda) > 75. / 180 * M_PI) continue;

		mcpSet.insert(&mcp);
		m_allTruths->fill(&mcp);
	}

	// Tracks
	std::set<const edm4hep::Track*> trkSet;
	for (const Track& trk : *tracks) {
		trkSet.insert(&trk);
		m_allTracks->fill(&trk);
	}
	m_hNumber_of_tracks->Fill(trkSet.size());

	// Loop over MC Relations and save matched objects
	for (const MCRecoTrackParticleAssociation& rel : *trackToMCRelations) {
		const MCParticle* mcp = rel.getSim();
		const Track* trk = rel.getRec();
		
		if (mcpSet.count(mcp) == 0) continue; // Truth particle not accepted
		if (rel.getWeight() > m_matchProb) {
			if (trkSet.find(trk) != trkSet.end()) {
				m_realTracks->fill(trk);
				m_realTruths->fill(mcp);
				m_realTruths->effi(mcp, true);
				m_realReso->fill(trk, mcp);
				m_fakeTracks->effi(trk, false);

				mcpSet.erase(mcp);
				trkSet.erase(trk);
			}
		}
	}

	// Save unmatched objects
	for (const auto* mcp : mcpSet) {
		m_unmtTruths->fill(mcp);
		m_realTruths->effi(mcp, false);
	}
	for (const auto* trk : trkSet) {
		m_fakeTracks->fill(trk);
		m_fakeTracks->effi(trk, true);
	}
	m_hNumber_of_fakes->Fill(trkSet.size());

	return StatusCode::SUCCESS;
}

// Implement finalize
StatusCode TrackPerfHistAlg::finalize() {
	return StatusCode::SUCCESS;
}
