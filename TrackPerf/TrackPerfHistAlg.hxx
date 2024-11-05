#pragma once

// edm4hep
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/TrackCollection.h>
#include <edm4hep/MCParticle.h>
#include <edm4hep/Track.h>
#include <edm4hep/MCRecoTrackParticleAssociationCollection.h>

// Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiAlg/Consumer.h>
#include <Gaudi/Property.h>
#include <k4FWCore/BaseClass.h> // Is this needed?
#include <GaudiKernel/ITHistSvc.h>

// k4FWCore
#include <k4FWCore/DataHandle.h>

// Root
#include <TH1.h>

// Standard
#include <tuple>
#include <memory>

namespace TrackPerf {
class TrackHists;
class TruthHists;
class ResoHists;
}  // namespace TrackPerf

/**
 * @brief A consumer that makes all of the Track Performance Plots
 * This Consumer uses the Associations created by TrackTruthAlg to
 * Connect reconstructed tracks to the original MC particles to
 * determine the efficiency and performance of the reconstruction
 * algorithm
 *
 * @author Samuel Ferraro
 * @author Unknown
 */
struct TrackPerfHistAlg final : Gaudi::Functional::Consumer<void(
		const DataWrapper<edm4hep::MCParticleCollection> &,
		const edm4hep::TrackCollection &,
		const edm4hep::MCRecoTrackParticleAssociationCollection &)> {
	public:
		/**
 	 	 * @brief Constructer for the TrackPerfHistAlg
 	 	 * @param name unique string identifier for this instance
 	 	 * @param svcLoc a Service Locator passed by the Gaudi AlgManager 
 		 */
		TrackPerfHistAlg(const std::string& name, ISvcLocator* pSvcLocator);

		/**
 		 * @brief Register and create all histograms (and Histogram Service)
 		 */
	 	StatusCode initialize();

		/**
 		 * @brief The TrackPerfHistAlg operation. The workhorse of this MultiTransformer.
 		 * @param tracks The collection of reconstructed, deduped, filtered tracks
 		 * @param trackToMCRelations The collection of associations between MC particles and tracks
 		 */ 
		void operator()(const DataWrapper<edm4hep::MCParticleCollection>& mcParticles,
                	const edm4hep::TrackCollection& tracks,
                	const edm4hep::MCRecoTrackParticleAssociationCollection& trackToMCRelations) const;

	private:
		//! Determination of good vs bad match
		Gaudi::Property<float> m_matchProb{this, "MatchProb", 0.5, "Minimum matching probability to be considered a good track-MC match."};

		//! Histogram objects
		///@{
		std::shared_ptr<TrackPerf::TrackHists> m_allTracks;
		std::shared_ptr<TrackPerf::TrackHists> m_realTracks;
		std::shared_ptr<TrackPerf::TrackHists> m_fakeTracks;
		std::shared_ptr<TrackPerf::TruthHists> m_allTruths;
		std::shared_ptr<TrackPerf::TruthHists> m_realTruths;
		std::shared_ptr<TrackPerf::TruthHists> m_unmtTruths;
		std::shared_ptr<TrackPerf::ResoHists> m_realReso;
		///@}

		//! Multiplicity histograms
		///@{
		TH1* m_hNumber_of_fakes;
		TH1* m_hNumber_of_tracks;
		///@}
};
