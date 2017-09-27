#ifndef HPSSIM_PARAMETERS_
#define HPSSIM_PARAMETERS_

namespace hpssim {

class Parameters {

    public:

        typedef std::map<std::string, double> ParamMap;

        void set(std::string name, double value) {
            params_[name] = value;
        }

        double get(std::string name, double defaultValue) {
            if (has(name)) {
                return get(name);
            } else {
                return defaultValue;
            }
        }

        double get(std::string name) {
            if (params_.find(name) != params_.end()) {
                return params_[name];
            } else {
                std::cerr << "The parameter '" << name << "' does not exist." << std::endl;
                G4Exception("", "", FatalException, "Parameter does not exist.");
                return 0;
            }
        }

        bool has(std::string name) {
            return params_.find(name) != params_.end();
        }

        std::ostream& print(std::ostream& os) {
            for (auto entry : params_) {
                os << "  " << entry.first << " = " << entry.second << std::endl;
            }
            return os;
        }

    public:
        ParamMap params_;

};

}

#endif
