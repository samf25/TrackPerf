#include "TrackPerf/TrackPerfFile.hxx"

#include <arrow/io/file.h>

std::vector<std::shared_ptr<arrow::Field>> TrackPerfFile::fields()
{
  std::vector<std::shared_ptr<arrow::Field>> fields = {
    // Status
    arrow::field("truthOK",arrow::boolean()),
    arrow::field("trackOK",arrow::boolean()),
    // MC Particle
    arrow::field("mc_pdg",arrow::int32  ()),
    arrow::field("mc_vtx",arrow::float64()),
    arrow::field("mc_vty",arrow::float64()),
    arrow::field("mc_vtz",arrow::float64()),
    arrow::field("mc_mox",arrow::float64()),
    arrow::field("mc_moy",arrow::float64()),
    arrow::field("mc_moz",arrow::float64()),
    // Track
    arrow::field("tr_ch2" ,arrow::float32()),
    arrow::field("tr_ndf" ,arrow::int32  ()),
    arrow::field("tr_nhit",arrow::int32  ()),
    // Track State
    arrow::field("tr_dze",arrow::float32()),
    arrow::field("tr_phi",arrow::float32()),
    arrow::field("tr_ome",arrow::float32()),
    arrow::field("tr_zze",arrow::float32()),
    arrow::field("tr_tnl",arrow::float32()),
    // common
    arrow::field("match" ,arrow::float32()),
  };

  return fields;
}

std::vector<std::shared_ptr<arrow::Array>> TrackPerfFile::arrays()
{
  std::vector<std::shared_ptr<arrow::Array>> arrays = {};
  std::shared_ptr<arrow::Array> array;

  // Status
  PARQUET_THROW_NOT_OK(_st_truth_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_st_track_builder  .Finish(&array   ));
  arrays.push_back(array);

  // MCParticle
  PARQUET_THROW_NOT_OK(_mc_pdg_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_mc_vtx_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_mc_vty_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_mc_vtz_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_mc_mox_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_mc_moy_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_mc_moz_builder  .Finish(&array   ));
  arrays.push_back(array);

  // Track
  PARQUET_THROW_NOT_OK(_tr_ch2_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_tr_ndf_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_tr_nhit_builder .Finish(&array   ));
  arrays.push_back(array);

  // Track State
  PARQUET_THROW_NOT_OK(_ts_dze_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_ts_phi_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_ts_ome_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_ts_zze_builder  .Finish(&array   ));
  arrays.push_back(array);

  PARQUET_THROW_NOT_OK(_ts_tnl_builder  .Finish(&array   ));
  arrays.push_back(array);

  // Common
  PARQUET_THROW_NOT_OK(_st_match_builder.Finish(&array   ));
  arrays.push_back(array);

  return arrays;
}

void TrackPerfFile::fill(const EVENT::LCEvent* evt, const EVENT::MCParticle* mcp, const EVENT::Track* trk, float match)
{
  //
  // Save MCParticle
  if(mcp!=nullptr)
    {
      PARQUET_THROW_NOT_OK(_st_truth_builder.Append(true));

      PARQUET_THROW_NOT_OK(_mc_pdg_builder  .Append(mcp->getPDG()));
      PARQUET_THROW_NOT_OK(_mc_vtx_builder  .Append(mcp->getVertex()[0]));
      PARQUET_THROW_NOT_OK(_mc_vty_builder  .Append(mcp->getVertex()[1]));
      PARQUET_THROW_NOT_OK(_mc_vtz_builder  .Append(mcp->getVertex()[2]));
      PARQUET_THROW_NOT_OK(_mc_mox_builder  .Append(mcp->getMomentum()[0]));
      PARQUET_THROW_NOT_OK(_mc_moy_builder  .Append(mcp->getMomentum()[1]));
      PARQUET_THROW_NOT_OK(_mc_moz_builder  .Append(mcp->getMomentum()[2]));
    }
  else
    {
      PARQUET_THROW_NOT_OK(_st_truth_builder.Append(false));

      PARQUET_THROW_NOT_OK(_mc_pdg_builder  .Append(0));
      PARQUET_THROW_NOT_OK(_mc_vtx_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_mc_vty_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_mc_vtz_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_mc_mox_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_mc_moy_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_mc_moz_builder  .Append(0.));
    }

  //
  // Save Track
  if(trk!=nullptr)
    {
      PARQUET_THROW_NOT_OK(_st_track_builder.Append(true));

      PARQUET_THROW_NOT_OK(_tr_ch2_builder  .Append(trk->getChi2()));
      PARQUET_THROW_NOT_OK(_tr_ndf_builder  .Append(trk->getNdf()));
      PARQUET_THROW_NOT_OK(_tr_nhit_builder .Append(trk->getTrackerHits().size()));

      const EVENT::TrackState *ts = trk->getTrackState( EVENT::TrackState::AtIP);

      PARQUET_THROW_NOT_OK(_ts_dze_builder  .Append(ts->getD0()));
      PARQUET_THROW_NOT_OK(_ts_phi_builder  .Append(ts->getPhi()));
      PARQUET_THROW_NOT_OK(_ts_ome_builder  .Append(ts->getOmega()));
      PARQUET_THROW_NOT_OK(_ts_zze_builder  .Append(ts->getZ0()));
      PARQUET_THROW_NOT_OK(_ts_tnl_builder  .Append(ts->getTanLambda()));
    }
  else
    {
      PARQUET_THROW_NOT_OK(_st_track_builder.Append(false));

      PARQUET_THROW_NOT_OK(_tr_ch2_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_tr_ndf_builder  .Append(0));
      PARQUET_THROW_NOT_OK(_tr_nhit_builder .Append(0));

      PARQUET_THROW_NOT_OK(_ts_dze_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_ts_phi_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_ts_ome_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_ts_zze_builder  .Append(0.));
      PARQUET_THROW_NOT_OK(_ts_tnl_builder  .Append(0.));
    }

  // common
  PARQUET_THROW_NOT_OK(_st_match_builder.Append(match));
  BaseFile::fill(evt);
}
