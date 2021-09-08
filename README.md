# Tracking Performance

Marlin package that creates outputs useful for tracking performance studies.

## Dependencies
- [LCParquet](https://gitlab.cern.ch/berkeleylab/MuonCollider/LCParquet)

## Tracker Acceptance
Truth particles failing any of the following are not considered for output.
- generator status must be 1 (stable particle)
- particle must be charged
- particle cannot decay in the tracker

## Parquet Output
Creates a Parquet file (`trackperf.parquet`) with the following columns:
`evt`, `truthOK`, `trackOK`, `match`
`mc_pdg`, `mc_vtx`, `mc_vty`, ` mc_vtz`, `mc_mox`, `mc_moy`, ` mc_moz`,
`tr_ch2`, `tr_ndf`, `tr_nhit`, `tr_dze`, `tr_phi`, `tr_ome`, `tr_zze`, `tr_tnl`

The `truthOK` and `trackOK` indicate whether the `mc_*` and `tr_*` columns are
filled. For example, `truthOK==False` means that no truth particle was matched
to a track and the corresponding `mc_*` columns are filled with dummy values.
The `match` column indicates fraction of hits that were matched to the
corresponding truth particle.

The `tr_*` paremters correspond to the track state at the interaction point.

### Example Steering File
```xml
<processor name="MyLCParquet" type="TrackPerfParquet">
  <!--Location of output-->
  <parameter name="OutputDir" type="string">data_acts</parameter>
  <!--Name of the MCParticle collection-->
  <parameter name="MCParticleCollection" type="string" lcioInType="MCParticle">MCParticle</parameter>
  <!--Name of the Track collection-->
  <parameter name="TrackCollection" type="string" lcioInType="Track">Tracks</parameter>
  <!--Relations-->
  <parameter name="MCTrackRelationCollection" type="string" lcioInType="LCRelation">MCParticle_Tracks</parameter>
</processor>
```
