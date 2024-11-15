#pragma once

#include <TH1.h>
#include <TH2.h>

// DD4hep
#include <DD4hep/Detector.h>
#include <DD4hep/DD4hepUnits.h>

namespace EVENT {
class Track;
}

namespace TrackPerf {
//! Histograms for reconstructed tracks
class TrackHists {
 public:
  TrackHists(const TrackHists&) = delete;
  TrackHists& operator=(const TrackHists&) = delete;

  //! Initialize empty histograms
  TrackHists();

  // Fill histograms with a single track
  void fill(const EVENT::Track* track,
            dd4hep::Detector* lcdd);

 private:
  //! Reconstructed track pT
  TH1* h_pt;
  TH1* h_lambda;
  TH1* h_phi;
  TH1* h_d0;
  TH1* h_z0;
  TH1* h_nhit;
  TH2* h_lambda_nhit;
  TH2* h_pt_nhit;
  TH2* h_pt_lambda;
  TH1* h_nhit1;
  TH1* h_nhit2;
  TH1* h_nhit3;
  TH1* h_nhit4;
  TH1* h_nhit5;
  TH1* h_nhit6;
};
}  // namespace TrackPerf
