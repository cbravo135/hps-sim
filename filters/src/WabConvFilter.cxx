/**
 * @file WabConvFilter.h
 * @brief Class defining a UserActionPlugin that filters out events where a 
 *        wide angle brem doesn't convert.
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#include "WabConvFilter.h"

//------------//
//   Geant4   //
//------------//
#include "G4RunManager.hh"

namespace hpssim {

    void WabConvFilter::stepping(const G4Step* step) { 
        
        if (hasWabConv_) return; 

        // Get the track associated with this step.
        G4Track* track = step->GetTrack();

        // get the PDGID of the track.
        G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();

        // Make sure that the first particle that is processed is a photon. 
        // If another particle is encountered before the wide angle brem, 
        // put it on the waiting stack.
        if (pdgID != 22) return;

        // Get the volume the particle is in.
        G4VPhysicalVolume* volume = track->GetVolume();
        G4String volumeName = volume->GetName();

        // Get the particle type.
        G4String particleName = track->GetParticleDefinition()->GetParticleName();
        
        // Get the kinetic energy of the particle.
        //double incidentParticleEnergy = step->GetPostStepPoint()->GetTotalEnergy();

        /*
        std::cout << "*******************************" << std::endl;
        std::cout << "*   Step " << track->GetCurrentStepNumber() << std::endl;
        std::cout << "********************************" << std::endl;

        std::cout << "[ TargetBremFilter ]: " << "\n" 
                    << "\tTotal energy of " << particleName      << " ( PDG ID: " << pdgID
                    << " ) : " << incidentParticleEnergy       << "\n"
                    << "\tTrack ID: " << track->GetTrackID()     << "\n" 
                    << "\tStep #: " << track->GetCurrentStepNumber() << "\n"
                    << "\tParticle currently in " << volumeName  
                    << "\tPost step process: " << step->GetPostStepPoint()->GetStepStatus() 
                    << std::endl;*/

        // Only conversions that happen in the target, first or second layers
        // of the tracker are of interest.  If the photon has propagated past
        // the second layer and didn't convert, kill the event.
        if (volumeName.find("module_L3") != std::string::npos) {
            /*std::cout << "[ WabConvFilter ]: Photon is beyond the sensitive" 
                      << " detectors of interest. Killing event." << std::endl;*/
            track->SetTrackStatus(fKillTrackAndSecondaries); 
            G4RunManager::GetRunManager()->AbortEvent();
            return;
        } else if ((volumeName.find("module_L1") == std::string::npos) &&
                    (volumeName.find("module_L2") == std::string::npos)) {
            /*std::cout << "[ WabConvFilter ]: Photon is not within sensitive " 
                      << " detectors of interest." << std::endl;*/
            return;
        }

        // Check if any secondaries were produced in the volume.
        const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
           
        /*std::cout << "[ WabConvFilter ]: "
                  << particleName  << " produced " << secondaries->size() 
                  << " secondaries." << std::endl;*/

            // If the particle didn't produce any secondaries, stop processing
            // the event.
            if (secondaries->size() == 0) { 
                /*std::cout << "[ WabConvFilter ]: "
                            << "Primary did not produce secondaries!" 
                            << std::endl;*/
                return;
            } 
        
        G4String processName = (*secondaries)[0]->GetCreatorProcess()->GetProcessName();
        if (processName.compareTo("conv") == 0) { 
            hasWabConv_ = true;
            std::cout << "[ WabConvFilter ]: " 
                      << "WAB converted in " << volumeName << std::endl;

        } else { 
            track->SetTrackStatus(fKillTrackAndSecondaries);
            G4RunManager::GetRunManager()->AbortEvent();
            return;
        } 
    }

    void WabConvFilter::endEvent(const G4Event*) { 
        hasWabConv_ = false; 
    }

} // hpssim

SIM_PLUGIN(hpssim, WabConvFilter)
