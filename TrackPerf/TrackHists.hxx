#pragma once

#include <TH1.h>
#include <TH2.h>
#include <string>

#include <edm4hep/Track.h>

#include <GaudiKernel/ITHistSvc.h>

namespace TrackPerf {
//! Histograms for reconstructed tracks
class TrackHists {
 public:
  TrackHists(const TrackHists&) = delete;
  TrackHists& operator=(const TrackHists&) = delete;

  //! Initialize empty histograms
  TrackHists(ITHistSvc* histSvc, std::string folder, bool effi);

  // Fill histograms with a single track
  void fill(const edm4hep::Track* track);

  void effi(const edm4hep::Track* track, bool passed);

 private:
  //! magnetic field to use for curvature -> pT conversion
  float m_Bz = 3.57;

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
  TH1* h_effpt_total;
  TH1* h_effpt_passed;
  TH1* h_effeta_total;
  TH1* h_effeta_passed;};
}  // namespace TrackPerf
