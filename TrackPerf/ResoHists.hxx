#pragma once

// Root
#include <TH1.h>
#include <TH2.h>
#include <TVector2.h>

// Standard
#include <string>

// edm4hep
#include <edm4hep/Track.h>
#include <edm4hep/MCParticle.h>

// Gaudi
#include <GaudiKernel/ITHistSvc.h>

namespace TrackPerf {
/**
 * @brief Histograms for reconstructed tracks
 */
class ResoHists {
 public:
  ResoHists(const ResoHists&) = delete;
  ResoHists& operator=(const ResoHists&) = delete;

  /**
   * @brief Constructor for ResoHists. Initialize empty histograms
   * @param histSvc The Gaudi histogram service to register the histograms
   * @param folder The folder in which to register the histograms
   */
  ResoHists(ITHistSvc* histSvc, std::string folder);

  /**
   * @brief Fill histograms with a single track
   * @param track The track to fill histograms
   * @param particle The MC particle connected to the Track
   */
  void fill(const edm4hep::Track* track, const edm4hep::MCParticle* particle);

 private:
  //! magnetic field to use for curvature -> pT conversion
  float m_Bz = 3.57;

  //! Histograms to register information
  ///@{
  TH2* h_track_truth_pt;
  TH1* h_reso_pt_rel;
  TH1* h_reso_lambda;
  TH1* h_reso_phi;
  ///@}
};
}  // namespace TrackPerf
