#pragma once

// Root
#include <TH1.h>

// Standard
#include <string>

// edm4hep
#include <edm4hep/MCParticle.h>

// Gaudi
#include <GaudiKernel/ITHistSvc.h>

namespace TrackPerf {
/**
 * @brief Histograms for MC Tracks
 */
class TruthHists {
 public:
  TruthHists(const TruthHists&) = delete;
  TruthHists& operator=(const TruthHists&) = delete;

  /**
   * @brief Constructor for TruthHists. Initialize empty histograms
   * @param histSvc The Gaudi histogram service to register the histograms   
   * @param folder The folder in which to register the histograms
   * @param effi Whether or not this instance needs efficiency plots
   */
  TruthHists(ITHistSvc* histSvc, std::string folder, bool effi);

  /**
   * @brief Fill histograms with a single MC particle
   * @param track The MC particle to fill histograms
   */
  void fill(const edm4hep::MCParticle* track);

  /**
   * @brief Fill efficiency plots with single MC particle
   * @TODO: Currently there is no way to register TEfficiency plots. So this makes a total and a passed histogram that can be combined in an external script into a TEfficiency plot.
   * @param track The MC particle to fill the plots
   * @param passed Whether it is a passed or total element 
   */
  void effi(const edm4hep::MCParticle* track, bool passed);

 private:
  //! Histograms to register information
  ///@{
  TH1* h_pt;
  TH1* h_lambda;
  TH1* h_phi;
  TH1* h_vtr;
  TH1* h_vtz;
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
