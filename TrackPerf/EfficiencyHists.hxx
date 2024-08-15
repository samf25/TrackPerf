#pragma once

#include <TH1.h>
#include <TEfficiency.h>
#include <TVector2.h>
#include <TMath.h>

namespace EVENT {
class Track;
class MCParticle;
}

namespace TrackPerf {
//! Histograms for reconstructed tracks
class EfficiencyHists {
 public:
  EfficiencyHists(const EfficiencyHists&) = delete;
  EfficiencyHists& operator=(const EfficiencyHists&) = delete;

  //! Initialize empty histograms
  EfficiencyHists(bool effi);

  // Fill histograms with a single track
  void fillMC(const EVENT::MCParticle* track, bool passed);
  void fillTrack(const EVENT::Track* track, bool passed);

 private:
  //! magnetic field to use for curvature -> pT conversion
  float _Bz = 3.57;

  //! Efficiency plots
  TEfficiency* h_effpt;
  TEfficiency* h_efftheta;
};
}  // namespace TrackPerf
