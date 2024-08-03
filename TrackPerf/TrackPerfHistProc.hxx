#pragma once

#include <TH1.h>

#include <marlin/Processor.h>

namespace TrackPerf {
class TrackHists;
class TruthHists;
class ResoHists;
}  // namespace TrackPerf

//! Creates a simple column wise ntuple in a HistProc from LCIO collections.
class TrackPerfHistProc : public marlin::Processor {
 public:
  virtual Processor* newProcessor() { return new TrackPerfHistProc; }

  TrackPerfHistProc(const TrackPerfHistProc&) = delete;
  TrackPerfHistProc& operator=(const TrackPerfHistProc&) = delete;
  TrackPerfHistProc();

  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init();

  /** Called for every run.
   */
  virtual void processRunHeader(LCRunHeader* run);

  /** Called for every event - the working horse.
   */
  virtual void processEvent(LCEvent* evt);

  virtual void check(LCEvent* evt);

  /** Called after data processing for clean up.
   */
  virtual void end();

 private:
  //! Track Collection
  std::string _trkColName{};

  //! MC Particle Collection
  std::string _mcpColName{};

  //! Track to MC truth match collection
  std::string _trkMatchColName{};

  //! Determination of good vs bad match
  float _matchProb = 0.5;

  //! Histograms
  std::shared_ptr<TrackPerf::TrackHists> _allTracks;
  std::shared_ptr<TrackPerf::TrackHists> _realTracks;
  std::shared_ptr<TrackPerf::TrackHists> _fakeTracks;
  std::shared_ptr<TrackPerf::TruthHists> _allTruths;
  std::shared_ptr<TrackPerf::TruthHists> _realTruths;
  std::shared_ptr<TrackPerf::TruthHists> _unmtTruths;
  std::shared_ptr<TrackPerf::ResoHists> _realReso;

  TH1* h_number_of_fakes;
  TH1* h_number_of_tracks;
  TH1* h_count;
};
