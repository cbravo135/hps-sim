#ifndef HPSSIM_PRIMARYGENERATOR_H_
#define HPSSIM_PRIMARYGENERATOR_H_

#include "G4VPrimaryGenerator.hh"

#include "EventSampling.h"
#include "EventTransform.h"
#include "Parameters.h"
#include "PrimaryGeneratorMessenger.h"

#include "PrimaryGeneratorAction.h"

#include <map>
#include <queue>
#include <exception>
#include <vector>
#include <deque>
#include <random>

namespace hpssim {

class PrimaryGeneratorMessenger;

/*
 * Exception to throw when all events are read from file.
 */
class EndOfFileException: public std::exception {
    public:

        virtual const char* what() const throw() {
            return "No more events to read from this generator file.";
        }
};

/*
 * Exception to throw when all files have been read.
 */
class EndOfDataException: public std::exception {

    public:

        virtual const char* what() const throw() {
            return "No more event files to read from this generator.";
        }
};

/*
 * Exception to throw when an invalid index is used to access a file.
 */
class NoSuchRecordException: public std::exception {

    public:

        NoSuchRecordException(int recordIndex) {
            recordIndex_ = recordIndex;
        }

        virtual const char* what() const throw() {
            auto indexStr = std::to_string(recordIndex_);
            return std::string("No such record index: " + indexStr).c_str();
        }

    private:

        int recordIndex_ = -1;
};

/**
 * @class PrimaryGenerator
 * @brief Abstract class which event generators must implement
 *
 * @par
 * Features:
 * <ul>
 * <li>Generators can be run sequentially or in random mode to uniformly sample from an input file.</li>
 * <li>Generators can have any number of named double parameters.</li>
 * <li>An EventSampling object determines how many events to overlay from this source for a single Geant4 event.</li>
 * <li>Each generator has an arbitrarily long list of input files which is copied into a queue that is emptied during job processing.</li>
 * <li>For file-based generators, there are a series of methods that should be implemented for reading event data (see method comments).</li>
 * <li>There is an optional list of EventTransform objects that can be used to transform events from the generator.</li>
 * <li>A verbose level can be set between 1 and 4 (following Geant4 convention).
 * </ul>
 *
 * @todo
 * <ul>
 * <li>activate and deactivate</li>
 * <li>print out info: name, parameters, event sampling and transforms</li>
 * <li>delete</li>
 * <li>max events to buffer from file in random mode</li>
 * <li>implement a hasNextEvent() method to simplify file management</li>
 * </ul>
 */
class PrimaryGenerator : public G4VPrimaryGenerator {

    public:

        /**
         * Generator read mode, either sequential or random.
         * Random mode requires that this generator support
         * random access by index of its event data.
         */
        enum ReadMode {
            Sequential,  // Does not cache the file.
            Linear,      // Cache the file, then read content in order.
            Random,      // Cache the file, then read the events randomly, making sure there are no duplicates.
            PureRandom,  // Cache the file, then reads the events randomly, with possible duplicates.
            SemiRandom   // Cache the file, then read the events randomly among 1k blocks.
        };

        /**
         * Constructor which takes a name so each event generator can be
         * managed using macro commands.
         */
        PrimaryGenerator(std::string name);

        virtual ~PrimaryGenerator();

        /**
         * Sub-classes must implement this method to generate an event.
         */
        virtual void GeneratePrimaryVertex(G4Event* anEvent) = 0;

        /**
         * Initialization callback to perform extra setup before run starts.
         *
         * Initializes the random generator so the event_list_ can be std::shuffle-ed.
         * We want this here so the same generator is used by all sub classes.
         */
        virtual void initialize() {
            
//            static bool is_initialized = false;    // This static makes sure that we don't keep re-initializing the random generator.
//            if(!is_initialized){
//
//                const long seed = CLHEP::HepRandom::getTheSeed();
//                random_gen.seed(seed);
//                if(verbose_ > 0){
//                    std::cout << "PrimaryGenerator::initialize() -- Initialized random_gen with seed: " << seed << std::endl;
//                    std::cout << "Trials: : " << random_gen() << " " << random_gen() << std::endl;
//                }
//            }
        }
        /**
         * Get the list of name-value double parameters assigned to this generator.
         */
        Parameters& getParameters() {
            return params_;
        }

        /**
         * Get the unique name of this generator.
         */
        const std::string& getName() {
            return name_;
        }

        /**
         * Add a file to process.
         */
        void addFile(std::string file) {
            files_.push_back(file);
        }

        /**
         * Get the list of files to be processed by this generator.
         */
        const std::vector<std::string>& getFiles() {
            return files_;
        }

        /**
         * Set the verbose level (1-4).
         */
        void setVerbose(int verbose) {
            verbose_ = verbose;
        }

        /**
         * Set the event sampling, which determines how many events are generated
         * and then overlaid onto each Geant4 event.
         */
        void setEventSampling(EventSampling* sampling) {
            if (sampling_) {
                delete sampling_;
            }
            sampling_ = sampling;
        }

        /**
         * Get the event sampling which determines how many generator events
         * to read for each Geant4 event.
         */
        EventSampling* getEventSampling() {
            return sampling_;
        }

        /**
         * Add a transform to be applied to this generator's events.
         */
        void addTransform(EventTransform* transform) {
            transforms_.push_back(transform);
        }

        /**
         * Get the list of event transforms to apply to this generator's events.
         */
        const std::vector<EventTransform*>& getTransforms() {
            return transforms_;
        }

        /**
         * Apply transforms to a generated event.
         */
        void applyTransforms(G4Event* anEvent) {
            for (auto transform : transforms_) {
                transform->transform(anEvent);
            }
        }

        /*
         * Called in initialization to queue up all files for processing.
         */
        void queueFiles() {
            fileQueue_  = std::queue<std::string>(); // Reset queue for new run.
            for (auto file : files_) {
                fileQueue_.push(file);
            }
        }

        /*
         * Open the next file for reading but do not read the first event.
         * This method will also build the event cache if the generator is
         * running in random mode.
         */
        virtual void readNextFile() throw(EndOfDataException) {
            if (fileQueue_.size()) {
                std::string nextFile = popFile();
                openFile(nextFile);
                if (getReadMode() != PrimaryGenerator::Sequential) {
                    cacheEvents();
                }
            } else {
                throw EndOfDataException();
            }
        }

        /*
         * Generate the table to read out the cached events in a particular order.
         * This can be "linear" i.e. 0...N, or "random" where 0..N is randomized, or
         * semirandom, where 0..N is randomized among blocks of 1K events.
         */
    
        virtual void createEventList() {
            // If readout is unique random, we need to initialize the unique random array.
            
            hpssim::PrimaryGeneratorAction *PGA= hpssim::PrimaryGeneratorAction::getPrimaryGeneratorAction();
            
            if (getReadMode() == PrimaryGenerator::Random || getReadMode() == PrimaryGenerator::Linear || getReadMode() == PrimaryGenerator::SemiRandom){
                int n_evt =getNumEvents();
                if(verbose_ > 3) std::cout << "Number events in file = " << n_evt << std::endl;
                for(int i=0;i<n_evt;++i) event_list_.push_back(i); // Make the linear list of events.
                if(verbose_ > 3) std::cout << "Number events in cache = " << event_list_.size() << std::endl;
                if (getReadMode() == PrimaryGenerator::Random ){
                    std::shuffle(event_list_.begin(),event_list_.end(),hpssim::PrimaryGeneratorAction::random_gen);  // Random shuffle the list.
                }else if(getReadMode() == PrimaryGenerator::SemiRandom ){
                    int num_blocks = n_evt/1024;
                    for(int i=0;i<num_blocks;++i){
                        std::shuffle(event_list_.begin()+(i*1024),event_list_.begin()+((i+1)*1024),hpssim::PrimaryGeneratorAction::random_gen);
                    }
                    std::shuffle(event_list_.begin()+(num_blocks*1024),event_list_.end(),hpssim::PrimaryGeneratorAction::random_gen); // Shuffle the remainder.
                }
                if(verbose_ > 0){
                    std::cout<<"Sample random sequence out of " << event_list_.size() << " after shuffle. " << std::endl;;
                    for(int i=0; i<20; ++i){
                        std::cout << event_list_[i] << " ";
                    }
                    std::cout << std::endl;
                }
            }

    }
    
    
    
    
        /**
         * Set the generator read mode, either Random or Sequential, PureRandon, Linear, SemiRandom.
         * The validity of the read mode for a particular generator
         * is checked from the PrimaryGeneratorMessenger before this is set.
         */
        void setReadMode(ReadMode readMode) {
            readMode_ = readMode;
        }

        /**
         * Get the generator read mode, either Random or Sequential.
         */
        ReadMode getReadMode() {
            return readMode_;
        }

        /**
         * This should be overridden to return the total number of events left in the
         * cache that is used for randomly sampling events by their index.
         */
        virtual int getNumEvents() {
            return 0;
        }

        /**
         * Override to return true if the generator has files that it manages
         * as input.
         */
        virtual bool isFileBased() {
           return false;
        }

        /**
         * File-based generators should override this hook to return true
         * if they support random access of their event data by sequential
         * index in the file.  Random access is actually implemented using
         * data caches in each generator implementation.
         */
        virtual bool supportsRandomAccess() {
            return false;
        }

        /**
         * File-based generators should override this hook to return an event
         * by its sequential index in an internal data cache.  This event will 
         * then be removed for subsequent usage in random access according 
         * to the flag (true to remove the record after sampling it).
         */
        virtual void readEvent(long, bool) throw(NoSuchRecordException) {
        }

        /**
         * File-based generators should override this to cache all the events from
         * a file into a data structure for random access.
         */
        virtual void cacheEvents() {
        }

        /*
         * File-based generators should use this hook to read in the next event
         * from the current file for processing and throw an EndOfFileException
         * if there are no more events in the file.
         */
        virtual void readNextEvent() throw(EndOfFileException) {
        }

        /**
         * File-based generators should use this hook to open the specified file.
         */
        virtual void openFile(std::string) {
        }

        /**
         * Generators should use this hook to cleanup event data that needs to be deleted.
         */  
        virtual void deleteEvent() {
        }

        void setReadFlag(bool readFlag) {
            readFlag_ = readFlag;
        }
  
        bool getReadFlag() {
            return readFlag_;
        }

    private:

        /**
         * Pop and return the next file to open.
         */
        std::string popFile() {
            // TODO: check if queue is empty
            std::string nextFile = fileQueue_.front();
            fileQueue_.pop();
            return nextFile;
        }

    protected:

        /** Verbose level for print output (1-4). */
        int verbose_{1};

    private:

        /** Unique name of the generator. */
        std::string name_;

        /** List of files with generator data. */
        std::vector<std::string> files_;

        /** File processing queue. */
        std::queue<std::string> fileQueue_;

        /** Messenger for command steering. */
        PrimaryGeneratorMessenger* messenger_;

        /** List of transforms that are applied to the events from this generator. */
        std::vector<EventTransform*> transforms_;

        /** The event sampling for getting the number of events to overlay (default of 1). */
        EventSampling* sampling_{new UniformEventSampling};

        /** Access to simple key-value double parameters set from steering macros. */
        Parameters params_;

        /** The read mode of the generator: either sequential or random. */
        ReadMode readMode_{Sequential};
 
        /** The event list, which determines the order in which events are read */
        std::vector<int> event_list_;

        /* Flag that controls whether generator rereads the same event (e.g. for biasing). */
        bool readFlag_{true};
};

}

#endif
