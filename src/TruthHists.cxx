#include "TrackPerf/TruthHists.hxx"

#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>

using namespace TrackPerf;

TruthHists::TruthHists(bool effi) {
  h_pt = new TH1F("truth_pt", ";Particle p_{T} [GeV];Particles [/0.1 GeV]", 100,
                  0, 10);
  h_lambda = new TH1F("truth_lambda", ";Particle #lambda; Particles", 100,
                      -3.14, 3.14);
  h_phi = new TH1F("truth_phi", ";Particle #phi; Particles", 100, -3.14, 3.14);
  h_vtr = new TH1F("truth_vtr", ";Particle vtx_{r} [mm]; Particles [/0.2 mm]",
                   100, -10, 10);
  h_vtz = new TH1F("truth_vtz", ";Particle vtx_{z} [mm]; Particles [/0.2 mm]",
                   100, -10, 10);
  h_deltaR = new TH1F("deltaR", ";DeltaR;Count", 50, 0, 0.02);
  h_deltaRlow = new TH1F("deltaRlow", "(Eta < -0.66);Delta R;Count", 50, 0, 0.02);
  h_deltaRmid = new TH1F("deltaRmid", "(-0.66 < Eta < 0.66);Delta R;Count", 50, 0, 0.02);
  h_deltaRhigh = new TH1F("deltaRhigh", "(0.66 < Eta);Delta R;Count", 50, 0, 0.02);

  if (effi) {
	  h_effpt = new TEfficiency("pt_vs_eff", ";Truth pT [GeV]; Efficiency", 50, 0, 12 );
	  h_effeta = new TEfficiency("eta_vs_eff", ";Truth eta; Efficiency", 50, -4, 4);

  }
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
	double eta = std::atanh(mom[2] / std::sqrt(std::pow(pt, 2) + std::pow(mom[2], 2)));
	if (pt > 0.5) h_effeta->Fill(passed, eta);
	if (fabs(eta) < 2) h_effpt->Fill(passed, pt);
}

void TruthHists::deltaR(const EVENT::MCParticle* particle, const EVENT::Track* track) {
	const double* mom = particle->getMomentum();
        double pt = std::sqrt(std::pow(mom[0], 2) + std::pow(mom[1], 2));
        double eta = std::atanh(mom[2] / std::sqrt(std::pow(pt, 2) + std::pow(mom[2], 2)));
        double trackEta = -std::log(std::tan((1.57079632679 - std::atan(track->getTanLambda()))/2));
        double phi = std::atan(mom[1] / mom[0]);
        double trackPhi = track->getPhi();
	double dphi = phi - trackPhi;
	double deta = eta - trackEta;
	if (dphi > 3) { dphi -= 3.1415926535; }
	if (dphi < -3) { dphi += 3.1415926535; }
	if (deta > 3) { deta -= 3.1415926535; }
	if (deta < -3) { deta += 3.1415926535; }
        if (fabs(eta) < 2 && pt > 50) {
		double dR = std::sqrt(std::pow(deta,2)+std::pow(dphi,2));
		h_deltaR->Fill(dR);
		if (eta < -0.66) { h_deltaRlow->Fill(dR); }
		if (eta < 0.66 && eta > -0.66) { h_deltaRmid->Fill(dR); }
		if (eta > 0.66) { h_deltaRhigh->Fill(dR); }
	}
}
