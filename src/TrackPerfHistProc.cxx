#include "TrackPerf/TrackPerfHistProc.hxx"

#include "TrackPerf/TrackHists.hxx"
#include "TrackPerf/TruthHists.hxx"

#include <set>

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>

#include <marlin/AIDAProcessor.h>

#include <AIDA/ITree.h>

TrackPerfHistProc aTrackPerfHistProc ;

TrackPerfHistProc::TrackPerfHistProc()
  : Processor("TrackPerfHistProc")
{  
  // modify processor description
  _description = "TrackPerfHistProc creates a series of output histograms for tracking performance studies." ;

  // register steering parameters: name, description, class-variable, default value
  registerProcessorParameter("MatchProb",
                             "Minimum matching probabilty to be considered a good track-mc match.",
                             _matchProb,
                             _matchProb);

  registerInputCollection( LCIO::MCPARTICLE,
			   "MCParticleCollection" , 
			   "Name of the MCParticle collection"  ,
			   _mcpColName,
			   _mcpColName
			   );

  registerInputCollection( LCIO::TRACK,
			   "TrackCollection" ,
			   "Name of the Track collection" ,
			   _trkColName,
			   _trkColName
			   );

  registerInputCollection( LCIO::LCRELATION,
			   "MCTrackRelationCollection" , 
			   "Name of LCRelation collection with track to MC matching",
			   _trkMatchColName,
			   _trkMatchColName
			   );
}

void TrackPerfHistProc::init()
{
  // Print the initial parameters
  printParameters() ;

  // Create histograms
  AIDA::ITree* tree=marlin::AIDAProcessor::tree(this);
  marlin::AIDAProcessor::histogramFactory(this);

  // Create ROOT histograms, with location setup by the above factory
  tree->mkdir("all" ); tree->cd("all" );
  _allTracks =std::make_shared<TrackPerf::TrackHists>();
  _allTruths =std::make_shared<TrackPerf::TruthHists>();
  tree->mkdir("../real"); tree->cd("../real");
  _realTracks=std::make_shared<TrackPerf::TrackHists>();
  _realTruths=std::make_shared<TrackPerf::TruthHists>();
  tree->mkdir("../fake"); tree->cd("../fake");
  _fakeTracks=std::make_shared<TrackPerf::TrackHists>();
  tree->mkdir("../unmt"); tree->cd("../unmt");
  _unmtTruths=std::make_shared<TrackPerf::TruthHists>();
}

void TrackPerfHistProc::processRunHeader( LCRunHeader* /*run*/)
{ } 


void TrackPerfHistProc::processEvent( LCEvent * evt )
{
  //
  // Get object required collections and create lists
  // to keep track of unsaved objects.

  // MCParticles

  LCCollection* mcpCol  =evt->getCollection(_mcpColName);

  if( mcpCol->getTypeName() != lcio::LCIO::MCPARTICLE )
    { throw EVENT::Exception( "Invalid collection type: " + mcpCol->getTypeName() ) ; }

  std::set<const EVENT::MCParticle*> mcpSet;
  for(uint32_t i=0;i<mcpCol->getNumberOfElements();i++)
    {
      const EVENT::MCParticle *mcp=static_cast<const EVENT::MCParticle*>(mcpCol->getElementAt(i));

      if(mcp->getGeneratorStatus()!=1)
	{ continue; }

      if(mcp->getCharge()==0)
	{ continue; }

      if(mcp->isDecayedInTracker())
	{ continue; }

      // Tracker acceptance
      const double* mom=mcp->getMomentum();
      double pt=std::sqrt(std::pow(mom[0],2)+std::pow(mom[1],2));
      double lambda=std::atan2(mom[2],pt);
      if(abs(lambda)>75/180*3.14)
	{ continue; }

      mcpSet.insert(mcp);
      _allTruths->fill(mcp);
    }

  // Tracks

  LCCollection* trkCol  =evt->getCollection(_trkColName);

  if( trkCol->getTypeName() != lcio::LCIO::TRACK )
    { throw EVENT::Exception( "Invalid collection type: " + trkCol->getTypeName() ) ; }

  std::set<const EVENT::Track*> trkSet;
  for(uint32_t i=0;i<trkCol->getNumberOfElements();i++)
    {
      const EVENT::Track *trk=static_cast<const EVENT::Track*>(trkCol->getElementAt(i));

      trkSet.insert(trk);
      _allTracks->fill(trk);
    }

  //
  // Loop over track to MC associations to save matched objects
  LCCollection* tr2mcCol=evt->getCollection(_trkMatchColName);
  if( tr2mcCol->getTypeName() != lcio::LCIO::LCRELATION )
    { throw EVENT::Exception( "Invalid collection type: "+ tr2mcCol->getTypeName() ); }

  for(int i=0; i<tr2mcCol->getNumberOfElements(); ++i)
    {
      const EVENT::LCRelation *rel=static_cast<const EVENT::LCRelation*>(tr2mcCol->getElementAt(i));
      const EVENT::MCParticle *mcp=static_cast<const EVENT::MCParticle*>(rel->getFrom());
      const EVENT::Track      *trk=static_cast<const EVENT::Track     *>(rel->getTo  ());

      if(mcpSet.count(mcp)==0)
	{ continue; } // truth particle not selected

      if(rel->getWeight()>_matchProb)
	{
	  _realTracks->fill(trk);
	  _realTruths->fill(mcp);

	  mcpSet.erase(mcp);
	  trkSet.erase(trk);
	}
    }

  //
  // Save unmatched objects
  for(const EVENT::MCParticle* mcp : mcpSet)
    { _unmtTruths->fill(mcp); }
  for(const EVENT::Track* trk : trkSet)
    { _fakeTracks->fill(trk); }
}

void TrackPerfHistProc::check( LCEvent * /*evt*/ )
{ }

void TrackPerfHistProc::end()
{ }
