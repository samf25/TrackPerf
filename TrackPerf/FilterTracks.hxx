#pragma once

#include <marlin/Processor.h>


namespace TrackPerf
{
}

class FilterTracks : public marlin::Processor
{
public:
   virtual Processor* newProcessor() { return new FilterTracks ; }

   FilterTracks(const FilterTracks &) = delete ;
   FilterTracks& operator =(const FilterTracks &) = delete ;
   FilterTracks() ;

   /** Called at the begin of the job before anything is read.
    * Use to initialize the processor, e.g. book histograms.
    */
   virtual void init() ;

   /** Called for every run.
    */
   virtual void processRunHeader( LCRunHeader* run ) ;

   virtual void buildBfield() ;

   /** Called for every event - the working horse.
    */
   virtual void processEvent(LCEvent* evt) ;


   /** Called after data processing for clean up.
    */
   virtual void end() ;

private:
   //! Input track collection
   std::string _InTrackCollection {};

   //! Output track collection
   std::string _OutTrackCollection {};

   //! Cut off for number of hits
   int _NHits = 0;      
   
   //! Cut off for momentum
   float _MinPt = 1.0;   //units GeV

   //! Default magnetic field value
   float _Bz = 3.57;   //units Tesla
};

