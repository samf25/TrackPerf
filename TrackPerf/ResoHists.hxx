#pragma once

#include <TH1.h>
#include <TH2.h>
#include <TVector2.h>

// DD4hep
#include <DD4hep/Detector.h>
#include <DD4hep/DD4hepUnits.h>



namespace EVENT {
class Track;
class MCParticle;
}  // namespace EVENT

namespace TrackPerf {
//! Histograms for reconstructed tracks
class ResoHists {
 public:
  ResoHists(const ResoHists&) = delete;
  ResoHists& operator=(const ResoHists&) = delete;

  //! Initialize empty histograms
  ResoHists();

  // Fill histograms with a single track
  void fill(const EVENT::Track* track,
            const EVENT::MCParticle* particle,
            dd4hep::Detector* lcdd);

 private:
  //! Reconstructed track pT
  TH2* h_track_truth_pt;
  TH1* h_reso_pt_rel;
  TH1* h_reso_lambda;
  TH1* h_reso_phi;

  TH1F* h_deltaR;
  TH1F* h_deltaRlow;
  TH1F* h_deltaRmid;
  TH1F* h_deltaRhigh;
};
}  // namespace TrackPerf
