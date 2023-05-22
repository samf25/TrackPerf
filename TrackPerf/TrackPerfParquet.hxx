#pragma once

#include <LCParquet/LCParquetBase.hxx>

#include "TrackPerf/TrackPerfFile.hxx"

//! Creates a simple column wise ntuple in a Parquet from LCIO collections.
class TrackPerfParquet : public LCParquetBase {
 public:
  virtual Processor* newProcessor() { return new TrackPerfParquet; }

  TrackPerfParquet(const TrackPerfParquet&) = delete;
  TrackPerfParquet& operator=(const TrackPerfParquet&) = delete;
  TrackPerfParquet();

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

  //! Output file
  std::shared_ptr<TrackPerfFile> _file;
};
