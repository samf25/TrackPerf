#pragma once

#include <TH1.h>
#include <TEfficiency.h>

namespace EVENT {
class MCParticle;
class Track;
}

namespace TrackPerf {
//! Histograms for reconstructed tracks
class TruthHists {
 public:
  TruthHists(const TruthHists&) = delete;
  TruthHists& operator=(const TruthHists&) = delete;

  //! Initialize empty histograms
  TruthHists(bool effi);

  // Fill histograms with a single track
  void fill(const EVENT::MCParticle* particle);
  void effi(const EVENT::MCParticle* particle, bool passed);
  void deltaR(const EVENT::MCParticle* particle, const EVENT::Track* track);

 private:
  //! Reconstructed track pT
  TH1* h_pt;
  TH1* h_lambda;
  TH1* h_phi;
  TH1* h_vtr;
  TH1* h_vtz;
  TH1* h_deltaR;
  TEfficiency* h_effpt;
  TEfficiency* h_effeta;
};
}  // namespace TrackPerf
