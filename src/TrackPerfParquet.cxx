#include "TrackPerf/TrackPerfParquet.hxx"

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>

#include <LCParquet/Defines.hxx>
#include <set>

TrackPerfParquet aTrackPerfParquet;

TrackPerfParquet::TrackPerfParquet() : LCParquetBase("TrackPerfParquet") {
  // modify processor description
  _description =
      "TrackPerfParquet creates a Parquet file suitable for tracking "
      "performance studies";

  // register steering parameters: name, description, class-variable, default
  // value
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

void TrackPerfParquet::init() {
  LCParquetBase::init();

  //
  // Create output structures
  _file = std::make_shared<TrackPerfFile>();
  _file->init(_fs, "trackperf.parquet");
}

void TrackPerfParquet::processRunHeader(LCRunHeader* /*run*/) {}

void TrackPerfParquet::processEvent(LCEvent* evt) {
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

    mcpSet.insert(mcp);
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
  }

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

    _file->fill(evt, mcp, trk, rel->getWeight());

    mcpSet.erase(mcp);
    trkSet.erase(trk);
  }

  //
  // Save unmatched objects
  for (const EVENT::MCParticle* mcp : mcpSet) {
    _file->fill(evt, mcp, nullptr);
  }
  for (const EVENT::Track* trk : trkSet) {
    _file->fill(evt, nullptr, trk);
  }
}

void TrackPerfParquet::check(LCEvent* /*evt*/) {}

void TrackPerfParquet::end() { _file->close(); }
