# include "TrackPerf/FilterTracks.hxx"

#include "TrackPerf/TrackHists.hxx"
#include "TrackPerf/TruthHists.hxx"

#include <set>

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
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
  registerProcessorParameter("NHits",
		  	     "Minimum number of hits on track",
			     _NHits,
			     _NHits
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
}

void FilterTracks::processRunHeader( LCRunHeader* /*run*/)
{ }

void FilterTracks::processEvent( LCEvent * evt )
{
  // Make the output track collection
  LCCollectionVec *OutTrackCollection = new LCCollectionVec(LCIO::TRACK);

  // Get input collection
  LCCollection* InTrackCollection  =evt->getCollection(_InTrackCollection);

  if( InTrackCollection->getTypeName() != lcio::LCIO::TRACK )
    { throw EVENT::Exception( "Invalid collection type: " + InTrackCollection->getTypeName() ) ; }

  // Filter
  for(int i=0; i<InTrackCollection->getNumberOfElements(); i++)
    { 
      const EVENT::Track *trk=static_cast<const EVENT::Track*>(InTrackCollection->getElementAt(i));

      int nhit = trk->getTrackerHits().size();
      float _Bz=3.57;
      float pt=fabs(0.3*_Bz/trk->getOmega()/1000);
      
      EVENT::Track *track=static_cast<EVENT::Track*>(InTrackCollection->getElementAt(i));

      if(nhit > _NHits and pt > _MinPt)
	      {OutTrackCollection->addElement(track);}

      // Save output track collection
      evt->addCollection(OutTrackCollection, _OutTrackCollection);
	  }
    
}

void FilterTracks::end()
{ }
