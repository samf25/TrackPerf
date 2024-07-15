#pragma once

#include <TH1.h>
#include <TEfficiency.h>

namespace EVENT {
class MCParticle;
}

namespace TrackPerf {
//! Histograms for reconstructed tracks
class TruthHists {
 public:
  TruthHists(const TruthHists&) = delete;
  TruthHists& operator=(const TruthHists&) = delete;

  //! Initialize empty histograms
  TruthHists();

  // Fill histograms with a single track
  void fill(const EVENT::MCParticle* track);
  void effi(const EVENT::MCParticle* track);

 private:
  //! Reconstructed track pT
  TH1* h_pt;
  TH1* h_lambda;
  TH1* h_phi;
  TH1* h_vtr;
  TH1* h_vtz;
  TEfficiency* h_effpt;
  TEfficiency* h_effeta;
  TEfficiency* h_efftheta;
};
}  // namespace TrackPerf
