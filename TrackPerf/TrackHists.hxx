#pragma once

#include <TH1.h>
#include <TH2.h>

// ACTS
#include <Acts/MagneticField/MagneticFieldProvider.hpp>
#include <Acts/Definitions/Units.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>

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
            std::shared_ptr<Acts::MagneticFieldProvider> magField,
            Acts::MagneticFieldProvider::Cache& magCache);

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
