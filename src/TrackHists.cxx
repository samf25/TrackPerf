#include "TrackPerf/TrackHists.hxx"

#include <EVENT/Track.h>

using namespace TrackPerf;

TrackHists::TrackHists(bool effi) {
  h_pt =
      new TH1F("reco_pt", ";Track p_{T} [GeV];Tracks [/0.1 GeV]", 100, 0, 10);
  h_lambda =
      new TH1F("reco_lambda", ";Track #lambda; Tracks", 100, -3.14, 3.14);
  h_phi = new TH1F("reco_phi", ";Track #phi; Tracks", 100, -3.14, 3.14);
  h_d0 =
      new TH1F("reco_d0", ";Track d_{0} [mm]; Tracks [/0.2 mm]", 100, -10, 10);
  h_z0 =
      new TH1F("reco_z0", ";Track z_{0} [mm]; Tracks [/0.2 mm]", 100, -10, 10);
  h_nhit = new TH1F("reco_nhit", ";Track Hits; Tracks [/hit]", 20, -0.5, 19.5);
  h_lambda_nhit = new TH2F("lambda_vs_nhit", ";Track #lambda; Track Hits", 100,
                           -3.14, 3.14, 20, -0.5, 19.5);
  h_pt_nhit = new TH2F("pt_vs_nhit", ";Track p_{T} [GeV]; Track Hits", 100, 0,
                       10, 20, -0.5, 19.5);
  h_pt_lambda = new TH2F("pt_vs_lambda", ";Track p_{T} [GeV]; Track #lambda",
                         100, 0, 10, 100, -3.14, 3.14);
  h_nhit1 =
      new TH1F("reco_nhit_vtx", ";Vertex detector track hits; Tracks [/hit]",
               20, -0.5, 19.5);
  h_nhit2 =
      new TH1F("reco_nhit_inner", ";Inner tracker track hits; Tracks [/hit]",
               20, -0.5, 19.5);
  h_nhit3 =
      new TH1F("reco_nhit_outer", ";Outer tracker track hits; Tracks [/hit]",
               20, -0.5, 19.5);
  if (effi) {
    h_effpt = new TEfficiency("fake_pt_vs_eff", 
      "Fake Track Rate vs pT;Truth pT [GeV]; Efficiency", 50, 0, 12);
    h_effeta = new TEfficiency("fake_eta_vs_eff", 
      "Fake Track Rate vs Eta;Truth eta; Efficiency", 50, -5, 5);
  }
}

void TrackHists::fill(const EVENT::Track* track) {
  float pt = fabs(0.3 * _Bz / track->getOmega() / 1000);
  h_pt->Fill(pt);

  float lambda = std::atan(track->getTanLambda());
  h_lambda->Fill(lambda);
  h_phi->Fill(track->getPhi());
  h_d0->Fill(track->getD0());
  h_z0->Fill(track->getZ0());

  h_nhit->Fill(track->getTrackerHits().size());

  h_lambda_nhit->Fill(lambda, track->getTrackerHits().size());
  h_pt_nhit->Fill(pt, track->getTrackerHits().size());
  h_pt_lambda->Fill(pt, lambda);

  const EVENT::IntVec& subdetectorHitNumbers =
      track->getSubdetectorHitNumbers();
  if (subdetectorHitNumbers.size() > 6) {
    h_nhit1->Fill(subdetectorHitNumbers[1] + subdetectorHitNumbers[2]);
    h_nhit2->Fill(subdetectorHitNumbers[3] + subdetectorHitNumbers[4]);
    h_nhit3->Fill(subdetectorHitNumbers[5] + subdetectorHitNumbers[6]);
  }
}

void TrackHists::effi(const EVENT::Track* track, bool passed) {
	float pt = fabs(0.3 * _Bz / track->getOmega() / 1000);
	float eta = -std::log(std::tan((1.57079632679 - std::atan(track->getTanLambda()))/2));

	if (fabs(eta) < 2) h_effpt->Fill(passed, pt);
	if (pt > 0.5) h_effeta->Fill(passed, eta);
}
