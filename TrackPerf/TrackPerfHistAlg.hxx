#pragma once

// edm4hep
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/TrackCollection.h>
#include <edm4hep/MCRecoTrackParticleCollection.h>

// Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiAlg/Consumer.h>
#include <Gaudi/Property.h>
#include <k4FWCore/BaseClass.h>

// k4FWCore
#include <k4FWCore/DataHandle.h>

#include <tuple>

#include <GaudiKernel/ITHistSvc.h>
#include <TH1.h>
#include <memory>

namespace TrackPerf {
class TrackHists;
class TruthHists;
class ResoHists;
}  // namespace TrackPerf

struct TrackPerfHistAlg final : Gaudi::Functional::Consumer<(
		const edm4hep::MCParticleCollection,
		const edm4hep::TrackCollection,
		const edm4hep::MCRecoTrackParticleAssociationCollection)> {
	public:
		// Constructor
		TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator);

	 	StatusCode initialize();
		void operator()(const edm4hep::MCParticleCollection mcParticles,
                	const edm4hep::TrackCollection tracks,
                	const edm4hep::MCRecoTrackParticleAssociationCollection trackToMCRelations) const;

	private:
		// Determination of good vs bad match
		Gaudi::Property<float> m_matchProb{this, "MatchProb", 0.5, "Minimum matching probability to be considered a good track-MC match."};

		// Histograms
		std::shared_ptr<TrackPerf::TrackHists> m_allTracks;
		std::shared_ptr<TrackPerf::TrackHists> m_realTracks;
		std::shared_ptr<TrackPerf::TrackHists> m_fakeTracks;
		std::shared_ptr<TrackPerf::TruthHists> m_allTruths;
		std::shared_ptr<TrackPerf::TruthHists> m_realTruths;
		std::shared_ptr<TrackPerf::TruthHists> m_unmtTruths;
		std::shared_ptr<TrackPerf::ResoHists> m_realReso;

		TH1* m_hnumber_of_fakes;
		TH1* m_hnumber_of_tracks;
};
