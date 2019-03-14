/**
 * @file WabConvFilter.h
 * @brief Class defining a UserActionPlugin that filters out events where a 
 *        wide angle brem doesn't convert.
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#ifndef _WAB_CONV_FILTER_H_
#define _WAB_CONV_FILTER_H_

//----------------//
//   C++ StdLib   //
//----------------//
#include <iostream>

//------------//
//   Geant4   //
//------------//
#include "G4RunManager.hh"

//-------------//
//   hps-sim   //
//-------------//
#include "SimPlugin.h"

namespace hpssim {

    class WabConvFilter : public SimPlugin {

        public:

            /**
             * Class constructor.
             */
            WabConvFilter();

            /**
             * Class destructor.
             */
            ~WabConvFilter();

            /**
             * Get the name of the plugin.
             * @return The name of the plugin.
             */
            virtual std::string getName() {
                return "WabConvFilter";
            }

            std::vector<PluginAction> getActions() {
                return {PluginAction::STEPPING, PluginAction::EVENT};
            }

            /**
             * End of event action.
             */
            virtual void endEvent(const G4Event*);
            
            /**
             * Implementmthe stepping action which performs the target volume biasing.
             * @param step The Geant4 step.
             */
            void stepping(const G4Step* step);

        private:

            /** Process to filter on. */
            std::string processName_{""};

            /** Flag that denotes whether a conversion has been found. */
            bool hasWabConv_{false}; 

    }; // WabConvFilter

} // hpssim

#endif // _WAB_CONV_FILTER_H__

