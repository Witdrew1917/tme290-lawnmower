#ifndef BEHAVIOUR
#define BEHAVIOUR

#include <map>
#include <string>

class Behaviour {

    private:

        Behaviour(Behaviour const &) = delete;
        Behaviour(Behaviour &&) = delete;
        Behaviour &operator=(Behaviour const &) = delete;
        Behaviour &operator=(Behaviour &&) = delete;

    public:
        Behaviour() noexcept;
        ~Behaviour() = default;
        
        std::map< std::string, bool>  state;

        int map_width;
        int map_height;
        double battery;


    public:
        
        void set_initial_states();
        void set_impassable_objects();
        void set_instruction_set();

        int action();
        void transition(std::string);        
        double cost_to_home();

    private:
        
        double _standard_power_consumption;
        int _cut_dry_grass(); 
        int _cut_wet_grass();
        int _move_to_charger();
        int _charge();
        int _restart_cut();
        int _pass_object();
        void _reset_steps_in_rain();
        
        std::map<int, std::map<int, int>> _impassable_objects;
        std::map<int, std::map<int, int>> _instruction_set;

        int _current_x;
        int _current_y;
        std::string _heading;

        int _home_x;
        int _home_y;

        int _x_prior_to_charge;
        int _y_prior_to_charge;
        int _rain_steps;
        int _complete_passes;
};

#endif
