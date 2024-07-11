#pragma once

#include <TH1.h>
#include <TEfficiency.h>
#include <string>

#include <edm4hep/MCParticle.h>

#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/ServiceHandle.h>

namespace TrackPerf {
//! Histograms for reconstructed tracks
class TruthHists {
 public:
  TruthHists(const TruthHists&) = delete;
  TruthHists& operator=(const TruthHists&) = delete;

  //! Initialize empty histograms
  TruthHists(ServiceHandle<ITHistSvc> histSvc, std::string& folder, bool effi);

  // Fill histograms with a single track
  void fill(const edm4hep::MCParticle* track);

  void effi(const edm4hep::MCParticle* track, bool passed);

 private:
  //! Reconstructed track pT
  TH1* h_pt;
  TH1* h_lambda;
  TH1* h_phi;
  TH1* h_vtr;
  TH1* h_vtz;
  TEfficiency* h_effpt;
  TEfficiency* h_effeta;
};
}  // namespace TrackPerf
