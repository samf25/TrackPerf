#include "TrackHists.hxx"

#include <edm4hep/TrackState.h>

#include <math.h>
#include <TGraph.h>

using namespace TrackPerf;

TrackHists::TrackHists(ITHistSvc* histSvc, std::string folder, bool effi) {
	// Make Log Bounds
	Double_t* binEdges = new Double_t[201];
	for (int i =0; i <= 200; ++i) {
		binEdges[i] = std::pow(10, i * 3.0/200.0);
	}
	// Make Histograms
	h_pt = new TH1F("reco_pt", 
			";Track p_{T} [GeV];Tracks [/0.1 GeV]", 200, 0, 1000);
	h_ptlog = new TH1F("reco_ptlog",
                        ";Track p_{T} [GeV];Tracks [/0.1 GeV]", 200, binEdges);
	h_lambda = new TH1F("reco_lambda", 
			";Track #lambda; Tracks", 100, -3.14, 3.14);
	h_phi = new TH1F("reco_phi", 
			";Track #phi; Tracks", 100, -3.14, 3.14);
	h_d0 = new TH1F("reco_d0", 
			";Track d_{0} [mm]; Tracks [/0.2 mm]", 100, -10, 10);
	h_z0 = new TH1F("reco_z0", 
			";Track z_{0} [mm]; Tracks [/0.2 mm]", 100, -10, 10);
	h_nhit = new TH1F("reco_nhit", 
			";Track Hits; Tracks [/hit]", 20, -0.5, 19.5);
	h_lambda_nhit = new TH2F("lambda_vs_nhit", 
			";Track #lambda; Track Hits", 
			100, -3.14, 3.14, 20, -0.5, 19.5);
	h_pt_nhit = new TH2F("pt_vs_nhit", 
			";Track p_{T} [GeV]; Track Hits", 
			200, 0, 1000, 20, -0.5, 19.5);
	h_pt_lambda = new TH2F("pt_vs_lambda", 
			";Track p_{T} [GeV]; Track #lambda", 
			200, 0, 1000, 100, -3.14, 3.14);
	h_nhit1 = new TH1F("reco_nhit_vtx", 
			";Vertex detector track hits; Tracks [/hit]", 20, -0.5, 19.5);
	h_nhit2 = new TH1F("reco_nhit_inner",
			";Inner tracker track hits; Tracks [/hit]", 20, -0.5, 19.5);
	h_nhit3 = new TH1F("reco_nhit_outer", 
			";Outer tracker track hits; Tracks [/hit]", 20, -0.5, 19.5);
	h_z0_nhit = new TH2F("z0_vs_nhit",
			";Track z0; Track Hits",
			100, -10, 10, 20, -0.5, 19.5);
	h_z0_pt = new TH2F("z0_vs_pt",
			";Track z0; Track p_{T} [GeV]",
			100, -10, 10, 200, 0, 1000);

	
	// Register Histograms
	(void)histSvc->regHist("/histos/"+folder+"/track_pt", h_pt);
	(void)histSvc->regHist("/histos/"+folder+"/track_ptlog", h_ptlog);
	(void)histSvc->regHist("/histos/"+folder+"/track_lambda", h_lambda);
	(void)histSvc->regHist("/histos/"+folder+"/track_phi", h_phi);
	(void)histSvc->regHist("/histos/"+folder+"/track_d0", h_d0);
	(void)histSvc->regHist("/histos/"+folder+"/track_z0", h_z0);
	(void)histSvc->regHist("/histos/"+folder+"/track_nhit", h_nhit);
	(void)histSvc->regHist("/histos/"+folder+"/track_lambda_nhit", h_lambda_nhit);
	(void)histSvc->regHist("/histos/"+folder+"/track_pt_nhit", h_pt_nhit);
	(void)histSvc->regHist("/histos/"+folder+"/track_pt_lambda", h_pt_lambda);
	(void)histSvc->regHist("/histos/"+folder+"/track_nhit1", h_nhit1);
	(void)histSvc->regHist("/histos/"+folder+"/track_nhit2", h_nhit2);
	(void)histSvc->regHist("/histos/"+folder+"/track_nhit3", h_nhit3);
	(void)histSvc->regHist("/histos/"+folder+"/track_z0_nhit", h_z0_nhit);
	(void)histSvc->regHist("/histos/"+folder+"/track_z0_pt", h_z0_pt);
	// Efficiency plots
	if (effi) {
		h_effpt_total = new TH1F("eff_fake_pt_total", 
				"pT of All Tracks for Eff plot;Track pT [GeV];Count", 500, 0, 1200);
		h_effpt_passed = new TH1F("eff_fake_pt_passed",
				"pT of Fake Tracks for Eff plot;Track pT [GeV];Count", 500, 0, 1200);
		h_effeta_total = new TH1F("eff_fake_eta_total", 
				"eta of All Tracks for Eff plot;Track eta;Count", 50, -3, 3);
		h_effeta_passed = new TH1F("eff_fake_eta_passed",
				"eta of Fake Tracks for Eff plot;Track eta;Count", 50, -3, 3);

		(void)histSvc->regHist("/histos/"+folder+"/track_effpt_total", h_effpt_total);
		(void)histSvc->regHist("/histos/"+folder+"/track_effpt_passed", h_effpt_passed);
		(void)histSvc->regHist("/histos/"+folder+"/track_effeta_total", h_effeta_total);
		(void)histSvc->regHist("/histos/"+folder+"/track_effeta_passed", h_effeta_passed);
	}
}

// Fill Histograms with relevant data
void TrackHists::fill(const edm4hep::Track* track, std::shared_ptr<Acts::MagneticFieldProvider> magField, Acts::MagneticFieldProvider::Cache& magCache) {
	// TODO: This was initially edm4hep::TrackState::AtIP, but that was wrong. 0 is right. Better way to do this?
	const edm4hep::TrackState& state = track->getTrackStates(0);
	
	//TODO: This assumes uniform magnetic field
	const Acts::Vector3 zeroPos(0, 0, 0);
	Acts::Vector3 field = (*magField->getField(zeroPos, magCache));
	float Bz = field[2] / Acts::UnitConstants::T;
		
	// pT
	float pt = fabs(0.3 * Bz / state.omega / 1000);
	h_pt->Fill(pt);
	h_ptlog->Fill(pt);

	// Lambda, phi, d0, z0
	float lambda = std::atan(state.tanLambda);
	h_lambda->Fill(lambda);
	h_phi->Fill(state.phi);
	h_d0->Fill(state.D0);
	h_z0->Fill(state.Z0);

	auto trackStateBegin = track->trackStates_begin();
    	auto trackStateEnd = track->trackStates_end();

	// nhit
	h_nhit->Fill(track->trackerHits_size());

	// 3d histos
	h_lambda_nhit->Fill(lambda, track->trackerHits_size());
	h_pt_nhit->Fill(pt, track->trackerHits_size());
	h_pt_lambda->Fill(pt, lambda);
	h_z0_nhit->Fill(state.Z0, track->trackerHits_size());
	h_z0_pt->Fill(state.Z0, pt);

	// Subdetector nhits
	std::vector<int> subdetectorHitNumbers(6);
	for (std::size_t i = 0; i < track->subdetectorHitNumbers_size(); ++i) {
		subdetectorHitNumbers[i] = track->getSubdetectorHitNumbers(i);
	}
	if (subdetectorHitNumbers[1] + subdetectorHitNumbers[2] > 0) {
		h_nhit1->Fill(subdetectorHitNumbers[1] + subdetectorHitNumbers[2]);
	}
	if (subdetectorHitNumbers[3] + subdetectorHitNumbers[4] > 0) {
		h_nhit2->Fill(subdetectorHitNumbers[3] + subdetectorHitNumbers[4]);
	}
	if (subdetectorHitNumbers[5] + subdetectorHitNumbers[6] > 0) {
		h_nhit3->Fill(subdetectorHitNumbers[5] + subdetectorHitNumbers[6]);
	}	

}

// Fill efficiency plots
void TrackHists::effi(const edm4hep::Track* track, bool passed, std::shared_ptr<Acts::MagneticFieldProvider> magField, Acts::MagneticFieldProvider::Cache& magCache) {
	// TODO: This was initially edm4hep::TrackState::AtIP, but that was wrong. 0 is right. Better way to do this?
	// Get track pt and eta
	const edm4hep::TrackState& state = track->getTrackStates(0);

	//TODO: This assumes uniform magnetic field
	const Acts::Vector3 zeroPos(0, 0, 0);
        Acts::Vector3 field = (*magField->getField(zeroPos, magCache));
        float Bz = field[2] / Acts::UnitConstants::T;

	double pt = fabs(0.3 * Bz / state.omega /1000);
	double eta = -std::log(std::tan((1.57079632679 - std::atan(state.tanLambda))/2));
	
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


