# include "TrackPerf/FilterTracks.hxx"

#include <math.h>

#include <DD4hep/Detector.h>

#include <EVENT/Track.h>
#include <IMPL/LCCollectionVec.h>

#include <marlin/AIDAProcessor.h>

#include <AIDA/ITree.h>

FilterTracks aFilterTracks ;

FilterTracks::FilterTracks()
  : Processor("FilterTracks")
{
  // modify processor description
  _description = "FilterTracks processor filters a collection of tracks based on NHits and MinPt and outputs a filtered collection";

  // register steering parameters: name, description, class-variable, default value
  registerProcessorParameter("NHitsTotal",
		  	     "Minimum number of hits on track",
			     _NHitsTotal,
			     _NHitsTotal
			      );
  
  registerProcessorParameter("NHitsVertex",
		  	     "Minimum number of hits on vertex detector",
			     _NHitsVertex,
			     _NHitsVertex
			      );

  registerProcessorParameter("NHitsInner",
		  	     "Minimum number of hits on inner tracker",
			     _NHitsInner,
			     _NHitsInner
			      );

  registerProcessorParameter("NHitsOuter",
		  	     "Minimum number of hits on outer tracker",
			     _NHitsOuter,
			     _NHitsOuter
			      );

  registerProcessorParameter("MinPt",
		  	     "Minimum transverse momentum",
			     _MinPt,
			     _MinPt
		 	      );

  registerInputCollection( LCIO::TRACK,
		  	   "InTrackCollection" ,
			   "Name of the input collection",
			   _InTrackCollection,
		           _InTrackCollection
		 	    );

  registerOutputCollection( LCIO::TRACK,
		  	   "OutTrackCollection" ,
			   "Name of output collection",
			   _OutTrackCollection,
			   std::string("FilteredTracks")
			    );

}

void FilterTracks::init()
{
  // Print the initial parameters
  printParameters() ;
  buildBfield() ;
}

void FilterTracks::processRunHeader( LCRunHeader* /*run*/)
{ }

void FilterTracks::buildBfield() 
{
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
  _Bz = magneticFieldVector[2]/dd4hep::tesla;
}

void FilterTracks::processEvent( LCEvent * evt )
{
  // Make the output track collection
  LCCollectionVec *OutTrackCollection = new LCCollectionVec(LCIO::TRACK);
  OutTrackCollection->setSubset(true);

  // Get input collection
  LCCollection* InTrackCollection  =evt->getCollection(_InTrackCollection);

  if( InTrackCollection->getTypeName() != lcio::LCIO::TRACK )
    { throw EVENT::Exception( "Invalid collection type: " + InTrackCollection->getTypeName() ) ; }

  // Filter
  for(int i=0; i<InTrackCollection->getNumberOfElements(); i++)
    { 
      EVENT::Track *trk=static_cast<EVENT::Track*>(InTrackCollection->getElementAt(i));

      int nhittotal  = trk->getTrackerHits().size();
      int nhitvertex = trk->getSubdetectorHitNumbers()[1]+trk->getSubdetectorHitNumbers()[2];
      int nhitinner = trk->getSubdetectorHitNumbers()[3]+trk->getSubdetectorHitNumbers()[4];
      int nhitouter = trk->getSubdetectorHitNumbers()[5]+trk->getSubdetectorHitNumbers()[6];
      
      float pt=fabs(0.3*_Bz/trk->getOmega()/1000);

      if(nhittotal > _NHitsTotal and nhitvertex > _NHitsVertex and nhitinner > _NHitsInner and nhitouter > _NHitsOuter and pt > _MinPt)
	      {OutTrackCollection->addElement(trk);}
	  }

  // Save output track collection
  evt->addCollection(OutTrackCollection, _OutTrackCollection);  
}

void FilterTracks::end()
{ }
