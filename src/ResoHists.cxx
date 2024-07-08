#include "TrackPerf/ResoHists.hxx"

#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>

using namespace TrackPerf;

ResoHists::ResoHists() {
	h_track_truth_pt = new TH2F("track_vs_truth_pt", 
			";Track p_{T} [GeV];Truth p_{T} [GeV]", 100, 0, 10, 100, 0, 10);
	h_reso_pt_rel = new TH1F("reso_pt", 
			";(truth pt - track pt)/truth pt;Tracks", 100, -1, 1);
	h_reso_lambda = new TH1F("reso_lambda", 
			";truth lambda - track lambda;Tracks", 100, -1, 1);
	h_reso_phi = new TH1F("reso_phi", 
			";truth phi - track phi;Tracks", 100, -1, 1);
}

void ResoHists::fill(const edm4hep::Track* track, const edm4hep::MCParticle* particle) {
	float track_pt = fabs(0.3 * m_Bz / track->getOmega() / 1000);
	float track_lambda = std::atan(track->getTanLambda());

	const edm4hep::Vector3d mom = particle->getMomentum();
	double truth_pt = std::sqrt(std::pow(mom.x, 2) + std::pow(mom.y, 2));
	double truth_lambda = std::atan2(mom.z, truth_pt);
	double truth_phi = std::atan2(mom.y, mom.x);
	double delta_phi = TVector2::Phi_mpi_pi(truth_phi - track->getPhi());

	h_track_truth_pt->Fill(track_pt, truth_pt);
	h_reso_pt_rel->Fill((truth_pt - track_pt) / truth_pt);
	h_reso_lambda->Fill(truth_lambda - track_lambda);
	h_reso_phi->Fill(delta_phi);
}
