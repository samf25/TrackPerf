#pragma once

#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/ITHistSvc.h>
#include <TH1.h>
#include <memory>

namespace TrackPerf {
class TrackHists;
class TruthHists;
class ResoHists;
}  // namespace TrackPerf

//! Creates a simple column wise ntuple in a HistProc from LCIO collections.
class TrackPerfHistAlg : public GaudiAlgorithm {
	public:
		// Constructor
		TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator);
		// Destructor
		virtual ~TrackPerfHistAlg();

		// Initialization method
		virtual StatusCode initialize();

		// Execute method (called for every event)
		virtual StatusCode execute();

		// Called after data processing for clean up.
		virtual StatusCode finalize();

	private:
		// Track Collection
		std::string m_trkColName{};

		// MC Particle Collection
		std::string m_mcpColName{};

		// Track to MC truth match collection
		std::string m_trkMatchColName{};

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

DECLARE_COMPONENT(TrackPerfHistAlg)
