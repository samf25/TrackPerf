#include "TrackPerf/TrackHists.hxx"

#include <edm4hep/TrackState.h>
#include <math.h>

using namespace TrackPerf;

TrackHists::TrackHists(ServiceHandle<ITHistSvc> histSvc, std::string& folder, bool effi) {
	// Make Histograms
	h_pt = new TH1F("reco_pt", 
			";Track p_{T} [GeV];Tracks [/0.1 GeV]", 100, 0, 10);
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
			100, 0, 10, 20, -0.5, 19.5);
	h_pt_lambda = new TH2F("pt_vs_lambda", 
			";Track p_{T} [GeV]; Track #lambda", 
			100, 0, 10, 100, -3.14, 3.14);
	h_nhit1 = new TH1F("reco_nhit_vtx", 
			";Vertex detector track hits; Tracks [/hit]", 20, -0.5, 19.5);
	h_nhit2 = new TH1F("reco_nhit_inner",
			";Inner tracker track hits; Tracks [/hit]", 20, -0.5, 19.5);
	h_nhit3 = new TH1F("reco_nhit_outer", 
			";Outer tracker track hits; Tracks [/hit]", 20, -0.5, 19.5);
	
	// Register Histograms
	histSvc->regHist("/histos/"+folder+"/track_pt", h_pt);
	histSvc->regHist("/histos/"+folder+"/track_lambda", h_lambda);
	histSvc->regHist("/histos/"+folder+"/track_phi", h_phi);
	histSvc->regHist("/histos/"+folder+"/track_d0", h_d0);
	histSvc->regHist("/histos/"+folder+"/track_z0", h_z0);
	histSvc->regHist("/histos/"+folder+"/track_nhit", h_nhit);
	histSvc->regHist("/histos/"+folder+"/track_lambda_nhit", h_lambda_nhit);
	histSvc->regHist("/histos/"+folder+"/track_pt_nhit", h_pt_nhit);
	histSvc->regHist("/histos/"+folder+"/track_pt_lambda", h_pt_lambda);
	histSvc->regHist("/histos/"+folder+"/track_nhit1", h_nhit1);
	histSvc->regHist("/histos/"+folder+"/track_nhit2", h_nhit2);
	histSvc->regHist("/histos/"+folder+"/track_nhit3", h_nhit3);
	
	// Efficiency plots
	if (effi) {
		h_effpt = new TEfficiency("fake_pt_vs_eff", 
				"Fake Track Rate vs pT;Truth pT [GeV]; Efficiency", 50, 0, 12);
		h_effeta = new TEfficiency("fake_eta_vs_eff", 
				"Fake Track Rate vs Eta;Truth eta; Efficiency", 50, -5, 5);

		histSvc->regHist("/histos/"+folder+"/track", h_);
		histSvc->regHist("/histos/"+folder+"/track", h_);
	}
}

// Fill Histograms with relevant data
void TrackHists::fill(const edm4hep::Track* track) {
	const edm4hep::TrackState& state = track->getTrackStates(edm4hep::TrackState::AtIP);

	// pT
	float pt = fabs(0.3 * m_Bz / state.omega / 1000);
	h_pt->Fill(pt);

	// Lambda, phi, d0, z0
	float lambda = std::atan(state.tanLambda);
	h_lambda->Fill(lambda);
	h_phi->Fill(state.phi);
	h_d0->Fill(state.D0);
	h_z0->Fill(state.Z0);

	// nhit
	h_nhit->Fill(track->trackerHits_size());

	// 3d histos
	h_lambda_nhit->Fill(lambda, track->trackerHits_size());
	h_pt_nhit->Fill(pt, track->trackerHits_size());
	h_pt_lambda->Fill(pt, lambda);

	// Subdetector nhits
	std::vector<int> subdetectorHitNumbers(7);
	for (std::size_t i = 0; i < 7; ++i) {
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
void TrackHists::effi(const edm4hep::Track* track, bool passed) {
	const edm4hep::TrackState& state = track->getTrackStates(edm4hep::TrackState::AtIP);

	double pt = fabs(0.3 * m_Bz / state.omega /1000);
	double eta = -std::log(std::tan((1.57079632679 - std::atan(state.tanLambda))/2));
	
	if (fabs(eta) < 2) h_effpt->Fill(passed, pt);
	if (pt > 0.5) h_effeta->Fill(passed, eta);
}
