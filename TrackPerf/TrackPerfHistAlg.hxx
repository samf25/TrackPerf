#pragma once


// Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiAlg/Consumer.h>
#include <k4FWCore/BaseClass.h>

// k4FWCore
#include <k4WFCore/DataHandle.h>

#include <tuple>

#include <GaudiKernel/ITHistSvc.h>
#include <TH1.h>
#include <memory>

namespace TrackPerf {
class TrackHists;
class TruthHists;
class ResoHists;
}  // namespace TrackPerf

//! Creates a simple column wise ntuple in a HistProc from LCIO collections.
class TrackPerfHistAlg : public Gaudi::Functional::Consumer<(
		const edm4hep::MCPatricleCollection,
		const edm4hep::TrackCollection,
		const edm4hep::MCRecoParticleAssociationCollection), BaseClass_t> {
	public:
		// Constructor
		TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator);

	 	StatusCode initialize();
		void operator()(const edm4hep::MCPatricleCollection mcParticles,
                	const edm4hep::TrackCollection tracks,
                	const edm4hep::MCRecoParticleAssociationCollection trackToMCRelations) const;

	private:
		// Determination of good vs bad match
		float m_matchProb = 0.5;

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
