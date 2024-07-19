#pragma once

#include <TH1.h>
#include <TH2.h>
#include <TVector2.h>
#include <string>

#include <edm4hep/Track.h>
#include <edm4hep/MCParticle.h>

#include <GaudiKernel/ITHistSvc.h>

namespace TrackPerf {
//! Histograms for reconstructed tracks
class ResoHists {
 public:
  ResoHists(const ResoHists&) = delete;
  ResoHists& operator=(const ResoHists&) = delete;

  //! Initialize empty histograms
  ResoHists(ITHistSvc* histSvc, std::string folder);

  // Fill histograms with a single track
  void fill(const edm4hep::Track* track, const edm4hep::MCParticle* particle);

 private:
  //! magnetic field to use for curvature -> pT conversion
  float _Bz = 3.57;

  //! Reconstructed track pT
  TH2* h_track_truth_pt;
  TH1* h_reso_pt_rel;
  TH1* h_reso_lambda;
  TH1* h_reso_phi;
};
}  // namespace TrackPerf
