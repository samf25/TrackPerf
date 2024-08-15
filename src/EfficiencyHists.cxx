#include "TrackPerf/EfficiencyHists.hxx"

#include <EVENT/Track.h>
#include <EVENT/MCParticle.h>

using namespace TrackPerf;

EfficiencyHists::EfficiencyHists(bool effi) {
  if (effi) {
     h_effpt = new TEfficiency("pt_vs_eff", ";Truth pT [GeV]; Efficiency", 50, 0, 12 );
     h_efftheta = new TEfficiency("theta_vs_eff", ";Truth theta; Efficiency", 50, -4, 4);
  } else {
    h_effpt = new TEfficiency("fake_pt_vs_eff",
      "Fake Track Rate vs pT;Reconstructed pT [GeV]; Efficiency", 50, 0, 12);
    h_efftheta = new TEfficiency("fake_theta_vs_eff",
      "Fake Track Rate vs Theta;Reconstructed theta; Efficiency", 50, -4, 4);
  }
}

void EfficiencyHists::fillTrack(const EVENT::Track* track, bool passed) {
  float pt = fabs(0.3 * _Bz / track->getOmega() / 1000);
  float theta = TMath::Pi() - std::atan(track->getTanLambda());

  if (fabs(theta) < 2*std::atan(std::exp(-2))) h_effpt->Fill(passed, pt);
  if (pt > 0.5) h_efftheta->Fill(passed, theta);
}

void EfficiencyHists::fillMC(const EVENT::MCParticle* particle, bool passed) {
  const double* mom = particle->getMomentum();
  double pt = std::sqrt(std::pow(mom[0], 2) + std::pow(mom[1], 2));
  double theta = std::atanh(mom[2] / pt);
  if (pt > 0.5) h_efftheta->Fill(passed, theta);
  if (fabs(theta) < 2*std::atan(std::exp(-2))) h_effpt->Fill(passed, pt);
}
