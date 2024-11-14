#include "TrackPerf/TrackPerfHistProc.hxx"

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>

// DD4hep
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/Detector.h>

// ACTS
#include <Acts/Definitions/Units.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>

#include <AIDA/ITree.h>
#include <marlin/AIDAProcessor.h>
#include <set>

#include "TrackPerf/ResoHists.hxx"
#include "TrackPerf/TrackHists.hxx"
#include "TrackPerf/TruthHists.hxx"
#include "TrackPerf/EfficiencyHists.hxx"

TrackPerfHistProc aTrackPerfHistProc;

TrackPerfHistProc::TrackPerfHistProc() : Processor("TrackPerfHistProc") {
  // modify processor description
  _description =
      "TrackPerfHistProc creates a series of output histograms for tracking "
      "performance studies.";

  // register steering parameters: name, description, class-variable, default
  // value
  registerProcessorParameter(
      "MatchProb",
      "Minimum matching probabilty to be considered a good track-mc match.",
      _matchProb, _matchProb);

  registerInputCollection(LCIO::MCPARTICLE, "MCParticleCollection",
                          "Name of the MCParticle collection", _mcpColName,
                          _mcpColName);

  registerInputCollection(LCIO::TRACK, "TrackCollection",
                          "Name of the Track collection", _trkColName,
                          _trkColName);

  registerInputCollection(
      LCIO::LCRELATION, "MCTrackRelationCollection",
      "Name of LCRelation collection with track to MC matching",
      _trkMatchColName, _trkMatchColName);
}

void TrackPerfHistProc::buildBfield() {
  // Get the magnetic field
  dd4hep::Detector& lcdd = dd4hep::Detector::getInstance();
  const double position[3] = {
      0, 0,
      0};  // position to calculate magnetic field at (the origin in this case)
  double magneticFieldVector[3] = {
      0, 0, 0};  // initialise object to hold magnetic field
  lcdd.field().magneticField(
      position,
      magneticFieldVector);  // get the magnetic field vector from DD4hep

  // Build ACTS representation of field
  // Note:
  //  magneticFieldVector[2] = 3.57e-13
  //  dd4hep::tesla = 1e-13
  //  Acts::UnitConstants::T = 0.000299792
  _magneticField = std::make_shared<Acts::ConstantBField>(Acts::Vector3(
      magneticFieldVector[0] / dd4hep::tesla * Acts::UnitConstants::T,
      magneticFieldVector[1] / dd4hep::tesla * Acts::UnitConstants::T,
      magneticFieldVector[2] / dd4hep::tesla * Acts::UnitConstants::T));
  _magFieldContext = Acts::MagneticFieldContext();
}


void TrackPerfHistProc::init() {
  // Print the initial parameters
  printParameters();

  // Create histograms
  AIDA::ITree* tree = marlin::AIDAProcessor::tree(this);
  marlin::AIDAProcessor::histogramFactory(this);

  // Create ROOT histograms, with location setup by the above factory
  tree->mkdir("all");
  tree->cd("all");
  _allTracks = std::make_shared<TrackPerf::TrackHists>();
  _allTruths = std::make_shared<TrackPerf::TruthHists>();
  h_number_of_tracks = new TH1F(
      "number_of_tracks", ";Number of seeds/tracks;Events", 100, 0, 300000);
  tree->mkdir("../real");
  tree->cd("../real");
  _realTracks = std::make_shared<TrackPerf::TrackHists>();
  _realTruths = std::make_shared<TrackPerf::TruthHists>();
  _realReso = std::make_shared<TrackPerf::ResoHists>();
  tree->mkdir("../fake");
  tree->cd("../fake");
  _fakeTracks = std::make_shared<TrackPerf::TrackHists>();
  h_number_of_fakes = new TH1F("number_of_fakes",
                               ";Number of fake tracks;Events", 100, 0, 300000);
  tree->mkdir("../unmt");
  tree->cd("../unmt");
  _unmtTruths = std::make_shared<TrackPerf::TruthHists>();
  tree->mkdir("../efficiency");
  tree->cd("../efficiency");
  _effiPlots = std::make_shared<TrackPerf::EfficiencyHists>(true);
  _fakePlots = std::make_shared<TrackPerf::EfficiencyHists>(false);
  
  TrackPerfHistProc::buildBfield();
}

void TrackPerfHistProc::processRunHeader(LCRunHeader* /*run*/) {}

void TrackPerfHistProc::processEvent(LCEvent* evt) {
  // Mag Cache
  Acts::MagneticFieldProvider::Cache magCache = _magneticField->makeCache(m_magFieldContext);

  //
  // Get object required collections and create lists
  // to keep track of unsaved objects.

  // MCParticles
  LCCollection* mcpCol = evt->getCollection(_mcpColName);

  if (mcpCol->getTypeName() != lcio::LCIO::MCPARTICLE) {
    throw EVENT::Exception("Invalid collection type: " + mcpCol->getTypeName());
  }

  std::set<const EVENT::MCParticle*> mcpSet;
  for (uint32_t i = 0; i < mcpCol->getNumberOfElements(); i++) {
    const EVENT::MCParticle* mcp =
        static_cast<const EVENT::MCParticle*>(mcpCol->getElementAt(i));

    if (mcp->getGeneratorStatus() != 1) {
	    continue;
    }

    if (mcp->getCharge() == 0) {
      continue;
    }

    if (mcp->isDecayedInTracker()) {
      continue;
    }

    // Tracker acceptance
    const double* mom = mcp->getMomentum();
    double pt = std::sqrt(std::pow(mom[0], 2) + std::pow(mom[1], 2));
    double lambda = std::atan2(mom[2], pt);
    if (fabs(lambda) > 75. / 180 * 3.14) {
      continue;
    }

    mcpSet.insert(mcp);
    _allTruths->fill(mcp);
  }

  // Tracks

  LCCollection* trkCol = evt->getCollection(_trkColName);

  if (trkCol->getTypeName() != lcio::LCIO::TRACK) {
    throw EVENT::Exception("Invalid collection type: " + trkCol->getTypeName());
  }

  std::set<const EVENT::Track*> trkSet;
  for (uint32_t i = 0; i < trkCol->getNumberOfElements(); i++) {
    const EVENT::Track* trk =
        static_cast<const EVENT::Track*>(trkCol->getElementAt(i));

    trkSet.insert(trk);
    _allTracks->fill(trk, _magneticField, magCache);
  }
  h_number_of_tracks->Fill(trkSet.size());

  //
  // Loop over track to MC associations to save matched objects
  LCCollection* tr2mcCol = evt->getCollection(_trkMatchColName);
  if (tr2mcCol->getTypeName() != lcio::LCIO::LCRELATION) {
    throw EVENT::Exception("Invalid collection type: " +
                           tr2mcCol->getTypeName());
  }

  for (int i = 0; i < tr2mcCol->getNumberOfElements(); ++i) {
    const EVENT::LCRelation* rel =
        static_cast<const EVENT::LCRelation*>(tr2mcCol->getElementAt(i));
    const EVENT::MCParticle* mcp =
        static_cast<const EVENT::MCParticle*>(rel->getFrom());
    const EVENT::Track* trk = static_cast<const EVENT::Track*>(rel->getTo());

    if (mcpSet.count(mcp) == 0) {
      continue;
    }  // truth particle not selected

    if (rel->getWeight() > _matchProb) {
      if (trkSet.find(trk) != trkSet.end()) {
        _realTracks->fill(trk, _magneticField, magCache);
        _realTruths->fill(mcp);
	_effiPlots->fillMC(mcp, true);
        _realReso->fill(trk, mcp, _magneticField, magCache);
	_fakePlots->fillTrack(trk, false, _magneticField, magCache);

        mcpSet.erase(mcp);
        trkSet.erase(trk);
      }
    }
  }

  //
  // Save unmatched objects
  for (const EVENT::MCParticle* mcp : mcpSet) {
    _unmtTruths->fill(mcp);
    _effiPlots->fillMC(mcp, false);
  }
  for (const EVENT::Track* trk : trkSet) {
    _fakeTracks->fill(trk, _magneticField, magCache);
    _fakePlots->fillTrack(trk, true, _magneticField, magCache);
  }
  h_number_of_fakes->Fill(trkSet.size());
}

void TrackPerfHistProc::check(LCEvent* /*evt*/) {}

void TrackPerfHistProc::end() {}
