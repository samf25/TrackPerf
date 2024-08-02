#include "TrackPerfHistAlg.hxx"

#include <GaudiKernel/ITHistSvc.h>
#include <TH1.h>
#include <TFile.h>

#include "ResoHists.hxx"
#include "TrackHists.hxx"
#include "TruthHists.hxx"

DECLARE_COMPONENT(TrackPerfHistAlg)

// Implement Constructor
TrackPerfHistAlg::TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator) : Consumer(name, pSvcLocator, {
		KeyValue("InputMCParticleCollectionName", "MCParticle"),
		KeyValue("InputTrackCollectionName", "Tracks"),
	       	KeyValue("InputMCTrackRelationCollectionName", "MCTrackRelations")}) {}

// Implement Initializer
StatusCode TrackPerfHistAlg::initialize() {
	// Get Histogram and Data Services
	ITHistSvc* histSvc{nullptr};
	StatusCode sc1 = service("THistSvc", histSvc);
	if ( sc1.isFailure() ) { 
		error() << "Could not locate HistSvc" << endmsg;
		return StatusCode::FAILURE; }

	// Make Histograms
	m_allTracks = std::make_shared<TrackPerf::TrackHists>(histSvc, "all", false);
	m_allTruths = std::make_shared<TrackPerf::TruthHists>(histSvc, "all", false);
	m_hNumber_of_tracks = new TH1F("Number_of_tracks", "Number of seeds/tracks;Events", 100, 0, 300000);
	(void)histSvc->regHist("/histos/all/Number_of_tracks", m_hNumber_of_tracks);

	m_realTracks = std::make_shared<TrackPerf::TrackHists>(histSvc, "real", false);
	m_realTruths = std::make_shared<TrackPerf::TruthHists>(histSvc, "real", true);
	m_realReso = std::make_shared<TrackPerf::ResoHists>(histSvc, "real");
	m_fakeTracks = std::make_shared<TrackPerf::TrackHists>(histSvc, "fake", true);
	m_hNumber_of_fakes = new TH1F("Number_of_fakes", "Number of fake tracks;Events", 100, 0, 300000);
	(void)histSvc->regHist("/histos/fake/Number_of_fakes", m_hNumber_of_fakes);

	m_unmtTruths = std::make_shared<TrackPerf::TruthHists>(histSvc, "unmt", false);
	
	return StatusCode::SUCCESS;
}

// Implement operator (To be run on each event -- the workhorse)
void TrackPerfHistAlg::operator()(
			const DataWrapper<edm4hep::MCParticleCollection>& mcParticles,
                        const edm4hep::TrackCollection& tracks,
                        const edm4hep::MCRecoTrackParticleAssociationCollection& trackToMCRelations) const{
	// MC Particles
	std::set<const edm4hep::MCParticle*> mcpSet;
	for (const auto& mcp : *mcParticles.getData()) {
		if (mcp.getGeneratorStatus() != 1) continue;
		if (mcp.getCharge() == 0) continue;
		if (mcp.isDecayedInTracker()) continue;

		// Tracker Acceptance
		const edm4hep::Vector3f& mom = mcp.getMomentum();
		double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
		double lambda = std::atan2(mom.z, pt);
		if (fabs(lambda) > 75. / 180 * M_PI) continue;

		mcpSet.insert(&mcp);
		m_allTruths->fill(&mcp);
	}

	// Tracks
	std::set<const edm4hep::Track*> trkSet;
	for (const auto& trk : tracks) {
		trkSet.insert(&trk);
		m_allTracks->fill(&trk);
	}
	m_hNumber_of_tracks->Fill(trkSet.size());

	// Loop over MC Relations and save matched objects
	for (const auto& rel : trackToMCRelations) {
		const edm4hep::MCParticle mcpObj = rel.getSim();
		const edm4hep::MCParticle* mcp = &mcpObj;
		const edm4hep::Track trkObj = rel.getRec();
		const edm4hep::Track* trk = &trkObj;

		
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
}
