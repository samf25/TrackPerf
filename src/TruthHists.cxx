#include "TruthHists.hxx"

using namespace TrackPerf;

TruthHists::TruthHists(ITHistSvc* histSvc, std::string folder, bool effi) {
	// Make Histograms
	h_pt = new TH1F("truth_pt", 
			";Particle p_{T} [GeV];Particles [/0.1 GeV]", 200, 0, 1000);
	h_lambda = new TH1F("truth_lambda",
			";Particle #lambda; Particles", 100, -3.14, 3.14);
	h_phi = new TH1F("truth_phi",
			";Particle #phi; Particles", 100, -3.14, 3.14);
	h_vtr = new TH1F("truth_vtr", 
			";Particle vtx_{r} [mm]; Particles [/0.2 mm]", 100, -10, 10);
	h_vtz = new TH1F("truth_vtz",
		       	";Particle vtx_{z} [mm]; Particles [/0.2 mm]", 100, -10, 10);
	// Register Histograms
	(void)histSvc->regHist("/histos/"+folder+"/truth_pt", h_pt);
	(void)histSvc->regHist("/histos/"+folder+"/truth_lambda", h_lambda);
	(void)histSvc->regHist("/histos/"+folder+"/truth_phi", h_phi);
	(void)histSvc->regHist("/histos/"+folder+"/truth_vtr", h_vtr);
	(void)histSvc->regHist("/histos/"+folder+"/truth_vtz", h_vtz);
	
	// Efficiency Plots
	if (effi) {
		h_effpt_total = new TH1F("eff_fake_pt_total",
                                "pT of All Truths for Eff plot;Truth pT [GeV];Count", 500, 0, 1200);
                h_effpt_passed = new TH1F("eff_fake_pt_passed",
                                "pT of Matched Truth for Eff plot;Truth pT [GeV];Count", 500, 0, 1200);
                h_effeta_total = new TH1F("eff_fake_eta_total",
                                "eta of All Truth for Eff plot;Truth eta;Count", 50, -3, 3);
                h_effeta_passed = new TH1F("eff_fake_eta_passed",
                                "eta of Matched Truth for Eff plot;Truth eta;Count", 50, -3, 3);
                (void)histSvc->regHist("/histos/"+folder+"/truth_effpt_total", h_effpt_total);
                (void)histSvc->regHist("/histos/"+folder+"/truth_effpt_passed", h_effpt_passed);
                (void)histSvc->regHist("/histos/"+folder+"/truth_effeta_total", h_effeta_total);
                (void)histSvc->regHist("/histos/"+folder+"/truth_effeta_passed", h_effeta_passed);


	}
}

// Fill all Histograms with relavent data
void TruthHists::fill(const edm4hep::MCParticle* particle) {
	// pT
	const edm4hep::Vector3f& mom = particle->getMomentum();
	double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
	h_pt->Fill(pt);

	// Lambda
	double lambda = std::atan2(mom.z, pt);
	h_lambda->Fill(lambda);

	// Phi
	double phi = std::atan2(mom.y, mom[0]);
	h_phi->Fill(phi);

	// Vertex
	const edm4hep::Vector3d& vtx = particle->getVertex();
	double vtr = std::sqrt(std::pow(vtx.x, 2) + std::pow(vtx.y, 2));
	h_vtr->Fill(vtr);
	h_vtz->Fill(vtx.z);
}

// Fill Efficiency Plots with data
void TruthHists::effi(const edm4hep::MCParticle* particle, bool passed) {
	// Get particle pt and eta
	const edm4hep::Vector3f& mom = particle->getMomentum();
	double pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
	double eta = std::atanh(mom.z / std::sqrt(std::pow(pt, 2) + std::pow(mom.z, 2)));
	
	// Fill pt if within eta range
	if (fabs(eta) < 2) {
		h_effpt_total->Fill(pt);
		if (passed) { h_effpt_passed->Fill(pt); }
	}
	// Fill eta if within pt range
	if (pt > 0.5) {
		h_effeta_total->Fill(eta);
		if (passed) { h_effeta_passed->Fill(eta); }
	}
}
