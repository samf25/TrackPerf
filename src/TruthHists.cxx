#include "TrackPerf/TruthHists.hxx"

#include <EVENT/MCParticle.h>

using namespace TrackPerf;

TruthHists::TruthHists() {
  h_pt = new TH1F("truth_pt", ";Particle p_{T} [GeV];Particles [/0.1 GeV]", 100,
                  0, 10);
  h_lambda = new TH1F("truth_lambda", ";Particle #lambda; Particles", 100,
                      -3.14, 3.14);
  h_phi = new TH1F("truth_phi", ";Particle #phi; Particles", 100, -3.14, 3.14);
  h_vtr = new TH1F("truth_vtr", ";Particle vtx_{r} [mm]; Particles [/0.2 mm]",
                   100, -10, 10);
  h_vtz = new TH1F("truth_vtz", ";Particle vtx_{z} [mm]; Particles [/0.2 mm]",
                   100, -10, 10);
  h_effpt = new TEfficiency("pt_vs_eff", ";Truth pT [GeV]; Efficiency")
  h_effeta = new TEfficiency("eta_vs_eff", ";Truth eta; Efficiency")

}

void TruthHists::fill(const EVENT::MCParticle* particle) {
  const double* mom = particle->getMomentum();
  double pt = std::sqrt(std::pow(mom[0], 2) + std::pow(mom[1], 2));
  h_pt->Fill(pt);
  
  double lambda = std::atan2(mom[2], pt);
  h_lambda->Fill(lambda);

  double phi = std::atan2(mom[1], mom[0]);
  h_phi->Fill(phi);

  const double* vtx = particle->getVertex();
  double vtr = std::sqrt(std::pow(vtx[0], 2) + std::pow(vtx[1], 2));
  h_vtr->Fill(vtr);
  h_vtz->Fill(vtx[2]);
}

void TruthHists::effi(const EVENT::MCParticle* particle, bool passed) {
	const double* mom = particle->getMomentum();
	double pt = std::sqrt(std::pow(mom[0], 2) + std::pow(mom[1], 2));
	h_effpt->Fill(passed, pt)

	double eta = std::atan2(mom[2], std::sqrt(std::pow(pt, 2) + std::pow(mom[2], 2)));
  	h_effeta->Fill(passed, eta)
}
