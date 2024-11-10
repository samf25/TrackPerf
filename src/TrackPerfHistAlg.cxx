#include "TrackPerfHistAlg.hxx"

// Gaudi
#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/MsgStream.h>

// DD4hep
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/Detector.h>

// ACTS
#include <Acts/Definitions/Units.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>

// Root
#include <TH1.h>
#include <TFile.h>

// TrackPerf
#include "ResoHists.hxx"
#include "TrackHists.hxx"
#include "TruthHists.hxx"

DECLARE_COMPONENT(TrackPerfHistAlg)

// Implement Constructor
TrackPerfHistAlg::TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator) : Consumer(name, pSvcLocator, {
		KeyValue("InputMCParticleCollectionName", "MCParticle"),
		KeyValue("InputTrackCollectionName", "Tracks"),
	       	KeyValue("InputMCTrackRelationCollectionName", "MCTrackRelations")}) {}

void TrackPerfHistAlg::buildBfield() {
  // Get the magnetic field
  dd4hep::Detector& lcdd = dd4hep::Detector::getInstance();
  const double position[3] = {
      0, 0,
      0};  // position to calculate magnetic field at (the origin in this case)
  double magneticFieldVector[3] = {
      0, 0, 0};  // initialise object to hold magnetic field
  lcdd.field().magneticField(
      position,
      magneticFieldVector);  // get the magnetic field vector from DD4hep

  // Build ACTS representation of field
  // Note:
  //  magneticFieldVector[2] = 3.57e-13
  //  dd4hep::tesla = 1e-13
  //  Acts::UnitConstants::T = 0.000299792
  m_magneticField = std::make_shared<Acts::ConstantBField>(Acts::Vector3(
      magneticFieldVector[0] / dd4hep::tesla * Acts::UnitConstants::T,
      magneticFieldVector[1] / dd4hep::tesla * Acts::UnitConstants::T,
      magneticFieldVector[2] / dd4hep::tesla * Acts::UnitConstants::T));
  m_magFieldContext = Acts::MagneticFieldContext();
}

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
	MsgStream log(msgSvc(), name());
	// MC Particles
	std::vector<edm4hep::MCParticle> mcpSet;
	for (const auto& mcp : *mcParticles.getData()) {
		if (mcp.getGeneratorStatus() != 1) { continue; }
		if (mcp.getCharge() == 0) { continue; }
		if (mcp.isDecayedInTracker()) { continue; }
	
		// Tracker Acceptance
		const edm4hep::Vector3f& mom = mcp.getMomentum();
		double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
		double lambda = std::atan2(mom.z, pt);
		
		if (fabs(lambda) > 75. / 180 * M_PI) { continue; }

		mcpSet.push_back(mcp);
		m_allTruths->fill(&mcp);
	}

	// Tracks
	std::vector<edm4hep::Track> trkSet;
	log << MSG::DEBUG << "Track Collection Size: " << tracks.size() << endmsg;
	for (const auto& trk : tracks) {
		trkSet.push_back(trk);
		m_allTracks->fill(&trk, m_magneticField);
	}
	m_hNumber_of_tracks->Fill(trkSet.size());

	// Loop over MC Relations and save matched objects
	for (const auto& rel : trackToMCRelations) {
		const edm4hep::MCParticle mcpObj = rel.getSim();
		const edm4hep::MCParticle* mcp = &mcpObj;
		const edm4hep::Track trkObj = rel.getRec();
		const edm4hep::Track* trk = &trkObj;
		// Look for mcpObj in mcpSet
		auto itMC = std::find_if(mcpSet.begin(), mcpSet.end(), [this, mcpObj](edm4hep::MCParticle obj) { 
			return obj == mcpObj;
		});
		
		if (itMC == mcpSet.end()) { // Truth particle not accepted
			log << MSG::DEBUG << "Actual MCParticle:\n" << mcpObj << endmsg;
			for(const auto mc : mcpSet) { log << MSG::DEBUG << "MCPSET: " << mc << endmsg;}
			log << MSG::DEBUG << endmsg;
			continue;
		}
		if (rel.getWeight() > m_matchProb) {
			// Look for trkObj and move all other objects to front of list
			auto itTRK = std::find_if(trkSet.begin(), trkSet.end(), [this, trkObj](const edm4hep::Track obj) {
				return obj == trkObj;
			});
			if (itTRK != trkSet.end()) {
				m_realTracks->fill(trk, m_magneticField);
				m_realTruths->fill(mcp);
				m_realTruths->effi(mcp, true);
				m_realReso->fill(trk, mcp, m_magneticField);
				m_fakeTracks->effi(trk, false, m_magneticField);

				mcpSet.erase(itMC);
				trkSet.erase(itTRK);
			}	
		}
	}

	// Save unmatched objects
	for (auto& mcp : mcpSet) {
		m_unmtTruths->fill(&mcp);
		m_realTruths->effi(&mcp, false);
	}
	for (auto& trk : trkSet) {
		m_fakeTracks->fill(&trk, m_magneticField);
		m_fakeTracks->effi(&trk, true, m_magneticField);
	}
	m_hNumber_of_fakes->Fill(trkSet.size());
}

