#pragma once

// Root
#include <TH1.h>
#include <TH2.h>

// DD4hep
#include <DD4hep/Detector.h>
#include <DD4hep/DD4hepUnits.h>

// Standard
#include <string>

// edm4hep
#include <edm4hep/Track.h>

// Gaudi
#include <GaudiKernel/ITHistSvc.h>

namespace TrackPerf {
/**
 * @brief Histograms for reconstructed tracks
 */
class TrackHists {
 public:
  TrackHists(const TrackHists&) = delete;
  TrackHists& operator=(const TrackHists&) = delete;

  /**
   * @brief Constructor for TrackHists. Initialize empty histograms
   * @param histSvc The Gaudi histogram service to register the histograms
   * @param folder The folder in which to register the histograms
   * @param effi Whether or not this instance needs efficiency plots
   */
  TrackHists(ITHistSvc* histSvc, std::string folder, bool effi);

  /**
   * @brief Fill histograms with a single track
   * @param track The track to fill histograms
   */ 
  void fill(const edm4hep::Track* track, dd4hep::Detector& lcdd);

  /**
   * @brief Fill efficiency plots with sinlge track
   * @TODO: Currently there is no way to register TEfficiency plots. So this makes a total and a passed histogram that can be combined in an external script into a TEfficiency plot.
   * @param track The track to fill the plots
   * @param passed Whether it is a passed or total element 
   */ 
  void effi(const edm4hep::Track* track, bool passed, dd4hep::Detector& lcdd);

 private:
  //! Histograms to register information
  ///@{
  TH1* h_pt;
  TH1* h_ptlog;
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
  TH2* h_z0_nhit;
  TH2* h_z0_pt;
  ///@}
  //! Histograms to hold TEfficiency data
  ///@{
  TH1* h_effpt_total;
  TH1* h_effpt_passed;
  TH1* h_effeta_total;
  TH1* h_effeta_passed;
  ///@}
};
}  // namespace TrackPerf
