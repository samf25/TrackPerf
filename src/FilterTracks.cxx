#include "TrackPerf/FilterTracks.hxx"

#include <DD4hep/Detector.h>

#include <EVENT/Track.h>

#include <IMPL/LCCollectionVec.h>

#include <AIDA/ITree.h>
#include <marlin/AIDAProcessor.h>
#include <math.h>

FilterTracks aFilterTracks;

FilterTracks::FilterTracks() : Processor("FilterTracks") {
  // modify processor description
  _description =
      "FilterTracks processor filters a collection of tracks based on NHits "
      "and MinPt and outputs a filtered collection";

  // register steering parameters: name, description, class-variable, default
  // value
  registerProcessorParameter("NHitsTotal", "Minimum number of hits on track",
                             _NHitsTotal, _NHitsTotal);

  registerProcessorParameter("NHitsVertex",
                             "Minimum number of hits on vertex detector",
                             _NHitsVertex, _NHitsVertex);

  registerProcessorParameter("NHitsInner",
                             "Minimum number of hits on inner tracker",
                             _NHitsInner, _NHitsInner);

  registerProcessorParameter("NHitsOuter",
                             "Minimum number of hits on outer tracker",
                             _NHitsOuter, _NHitsOuter);

  registerProcessorParameter("MinPt", "Minimum transverse momentum", _MinPt,
                             _MinPt);

  registerInputCollection(LCIO::TRACK, "InputTrackCollectionName",
			  "Name of the input collection", _InTrackCollection,
			  _InTrackCollection
			  );

  registerOutputCollection(LCIO::TRACK, "OutputTrackCollectionName",
			   "Name of output collection", _OutTrackCollection,
			   _OutTrackCollection
			   );

  registerOutputCollection(LCIO::TRACK, "OutTrackCollection",
                           "Name of output collection", _OutTrackCollection,
                           _OutTrackCollection);
}

void FilterTracks::init() {
  // Print the initial parameters
  printParameters();
  buildBfield();
}

void FilterTracks::processRunHeader(LCRunHeader* /*run*/) {}

void FilterTracks::buildBfield() {
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
  _Bz = magneticFieldVector[2] / dd4hep::tesla;
}

void FilterTracks::processEvent(LCEvent* evt) {
  // Make the output track collection
  LCCollectionVec* OutTrackCollection = new LCCollectionVec(LCIO::TRACK);
  OutTrackCollection->setSubset(true);

  // Get input collection
  LCCollection* InTrackCollection = evt->getCollection(_InTrackCollection);

  if (InTrackCollection->getTypeName() != lcio::LCIO::TRACK) {
    throw EVENT::Exception("Invalid collection type: " +
                           InTrackCollection->getTypeName());
  }

  // Filter
  for (int i = 0; i < InTrackCollection->getNumberOfElements(); i++) {
    EVENT::Track* trk =
        static_cast<EVENT::Track*>(InTrackCollection->getElementAt(i));

    int nhittotal = trk->getTrackerHits().size();
    if (_NHitsTotal > 0 && nhittotal <= _NHitsTotal) {
      continue;
    }

    const EVENT::IntVec& subdetectorHitNumbers =
        trk->getSubdetectorHitNumbers();
    if (_NHitsVertex > 0) {
      int nhitvertex = trk->getSubdetectorHitNumbers()[1] +
                       trk->getSubdetectorHitNumbers()[2];
      if (nhitvertex <= _NHitsVertex) {
        continue;
      }
    }

    if (_NHitsInner > 0) {
      int nhitinner = trk->getSubdetectorHitNumbers()[3] +
                      trk->getSubdetectorHitNumbers()[4];
      if (nhitinner <= _NHitsInner) {
        continue;
      }
    }

    if (_NHitsOuter > 0) {
      int nhitouter = trk->getSubdetectorHitNumbers()[5] +
                      trk->getSubdetectorHitNumbers()[6];
      if (nhitouter <= _NHitsOuter) {
        continue;
      }
    }

    float pt = fabs(0.3 * _Bz / trk->getOmega() / 1000);
    if (_MinPt > 0 && pt < _MinPt) {
      continue;
    }

    OutTrackCollection->addElement(trk);
  }

  // Save output track collection
  evt->addCollection(OutTrackCollection, _OutTrackCollection);
}

void FilterTracks::end() {}
