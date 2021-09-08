#pragma once

#include <EVENT/LCEvent.h>
#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>

#include <LCParquet/BaseFile.hxx>

//! Holds columns with data from LCEvent
class TrackPerfFile : public BaseFile
{ 
public: 
  TrackPerfFile() =default;
  virtual ~TrackPerfFile() =default;

  virtual void fill(const EVENT::LCEvent* evt, const EVENT::MCParticle* mcp, const EVENT::Track* trk, float match=0);

protected:
  virtual std::vector<std::shared_ptr<arrow::Field>> fields();
  virtual std::vector<std::shared_ptr<arrow::Array>> arrays();

private:

  //
  // Builders

  // Match Status
  arrow::BooleanBuilder  _st_truth_builder;
  arrow::BooleanBuilder  _st_track_builder;
  arrow::FloatBuilder   _st_match_builder;

  // Truth
  
  arrow::Int32Builder  _mc_pdg_builder;
  arrow::DoubleBuilder _mc_vtx_builder;
  arrow::DoubleBuilder _mc_vty_builder;
  arrow::DoubleBuilder _mc_vtz_builder;
  arrow::DoubleBuilder _mc_mox_builder;
  arrow::DoubleBuilder _mc_moy_builder;
  arrow::DoubleBuilder _mc_moz_builder;

  // Track

  arrow::FloatBuilder  _tr_ch2_builder;
  arrow::Int32Builder  _tr_ndf_builder;
  arrow::Int32Builder  _tr_nhit_builder;

  // Track State
  
  arrow::FloatBuilder _ts_dze_builder;
  arrow::FloatBuilder _ts_phi_builder;
  arrow::FloatBuilder _ts_ome_builder;
  arrow::FloatBuilder _ts_zze_builder;
  arrow::FloatBuilder _ts_tnl_builder;
};
