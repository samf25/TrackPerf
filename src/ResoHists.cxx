#include "ResoHists.hxx"

using namespace TrackPerf;

ResoHists::ResoHists(ITHistSvc* histSvc, std::string folder) {
	//Make Histograms
	h_track_truth_pt = new TH2F("track_vs_truth_pt", 
			";Track p_{T} [GeV];Truth p_{T} [GeV]", 100, 0, 10, 100, 0, 10);
	h_reso_pt_rel = new TH1F("reso_pt", 
			";(truth pt - track pt)/truth pt;Tracks", 100, -1, 1);
	h_reso_lambda = new TH1F("reso_lambda", 
			";truth lambda - track lambda;Tracks", 100, -1, 1);
	h_reso_phi = new TH1F("reso_phi", 
			";truth phi - track phi;Tracks", 100, -1, 1);

	//Register Histograms
	(void)histSvc->regHist("/histos/"+folder+"/track_truth_pt", h_track_truth_pt);
	(void)histSvc->regHist("/histos/"+folder+"/reso_pt_rel", h_reso_pt_rel);
	(void)histSvc->regHist("/histos/"+folder+"/reso_lambda", h_reso_lambda);
	(void)histSvc->regHist("/histos/"+folder+"/reso_phi", h_reso_phi);

}

// Fill Histograms with relavant data
void ResoHists::fill(const edm4hep::Track* track, const edm4hep::MCParticle* particle, dd4hep::Detector& lcdd) {
	//TODO: This assumes uniform magnetic field
	const double position[3] = {0, 0, 0};           // position to calculate magnetic field (here, the origin)
        double magneticFieldVector[3] = {0, 0, 0};      // initialise object to hold magnetic field
        lcdd.field().magneticField(
                        position, magneticFieldVector); // get the magnetic field vector from DD4hep
        float Bz = magneticFieldVector[2] / dd4hep::tesla;

	
	// Get data
	// TODO: This was initially edm4hep::TrackState::AtIP, but that was wrong. 0 is right. Better way to do this?
	const edm4hep::TrackState& state = track->getTrackStates(0);
	float track_pt = fabs(0.3 * Bz / state.omega / 1000);
	float track_lambda = std::atan(state.tanLambda);

	const edm4hep::Vector3f& mom = particle->getMomentum();
	double truth_pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
	double truth_lambda = std::atan2(mom.z, truth_pt);
	double truth_phi = std::atan2(mom.y, mom.x);
	double delta_phi = TVector2::Phi_mpi_pi(truth_phi - state.phi);

	// Fill histograms
	h_track_truth_pt->Fill(track_pt, truth_pt);
	h_reso_pt_rel->Fill((truth_pt - track_pt) / truth_pt);
	h_reso_lambda->Fill(truth_lambda - track_lambda);
	h_reso_phi->Fill(delta_phi);
}
