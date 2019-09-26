# declare SimApplication module
module(
  NAME sim_app
  EXECUTABLES src/hps-sim.cxx
  DEPENDENCIES 
  EXTERNAL_DEPENDENCIES Geant4 LCIO LCDD GDML
)
