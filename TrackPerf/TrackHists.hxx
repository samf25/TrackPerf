#pragma once

#include <TH1.h>

namespace EVENT
{
  class Track;
}

namespace TrackPerf
{
  //! Histograms for reconstructed tracks
  class TrackHists
  {
  public:
    TrackHists(const TrackHists &) = delete ;
    TrackHists& operator =(const TrackHists &) = delete ;

    //! Initialize empty histograms
    TrackHists() ;

    // Fill histograms with a single track
    void fill(const EVENT::Track* track);

  private:
    //! magnetic field to use for curvature -> pT conversion
    float _Bz=3.57;

    //! Reconstructed track pT
    TH1* h_pt;
    TH1* h_lambda;
    TH1* h_phi;
    TH1* h_d0;
    TH1* h_z0;
  };
}
