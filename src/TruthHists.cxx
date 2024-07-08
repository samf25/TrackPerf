#include "TrackPerf/TruthHists.hxx"

#include <EVENT/MCParticle.h>

using namespace TrackPerf;

TruthHists::TruthHists() {
	h_pt = new TH1F("truth_pt", 
			";Particle p_{T} [GeV];Particles [/0.1 GeV]", 100, 0, 10);
	h_lambda = new TH1F("truth_lambda",
			";Particle #lambda; Particles", 100, -3.14, 3.14);
	h_phi = new TH1F("truth_phi",
			";Particle #phi; Particles", 100, -3.14, 3.14);
	h_vtr = new TH1F("truth_vtr", 
			";Particle vtx_{r} [mm]; Particles [/0.2 mm]", 100, -10, 10);
	h_vtz = new TH1F("truth_vtz",
		       	";Particle vtx_{z} [mm]; Particles [/0.2 mm]", 100, -10, 10);
	h_effpt = new TEfficiency("truth_pt_vs_eff", 
			"Truth Efficiency vs pT;Truth pT [GeV]; Efficiency", 50, 0, 12);
	h_effeta = new TEfficiency("truth_eta_vs_eff", 
			"Truth Efficiency vs Eta;Truth eta; Efficiency", 50, -5, 5);
}

void TruthHists::fill(const edm4hep::MCParticle* particle) {
	const edm4hep::Vector3d& mom = particle->getMomentum();
	double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
	h_pt->Fill(pt);

	double lambda = std::atan2(mom.z, pt);
	h_lambda->Fill(lambda);

	double phi = std::atan2(mom.y, mom[0]);
	h_phi->Fill(phi);

	const edm4hep::Vector3d& vtx = particle->getVertex();
	double vtr = std::sqrt(std::pow(vtx.x, 2) + std::pow(vtx.y, 2));
	h_vtr->Fill(vtr);
	h_vtz->Fill(vtx.z);
}

void TruthHists::effi(const edm4hep::MCParticle* particle, bool passed) {
	const edm4hep::Vector3d& mom = particle->getMomentum();
	double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
	double eta = std::atanh(mom.z / std::sqrt(std::pow(pt, 2) + std::pow(mom.z, 2)));
	
	if (fabs(eta) < 2) h_effpt->Fill(passed, pt);
	if (pt > 0.5) h_effeta->Fill(passed, eta);
}
