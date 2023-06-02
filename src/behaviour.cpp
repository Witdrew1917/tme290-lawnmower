#include <iostream>
#include "behaviour.hpp"
#include <iterator>
#include <string>
#include <map>

Behaviour::Behaviour() noexcept:
    state{},

    map_width{0},
    map_height{0},
    battery{0.0},
    _standard_power_consumption{0.004},

    _impassable_objects{},
    _instruction_set{},
    _current_x{0},
    _current_y{0},
    _heading{"EAST"},
    
    _home_x{0},
    _home_y{0},
    
    _x_prior_to_charge{0},
    _y_prior_to_charge{0},
    _rain_steps{0},
    _complete_passes{0}
{
}

void Behaviour::set_initial_states()
{
    state["CUTGRASS"] = true;
    state["CUTWETGRASS"] = false;
    state["MOVINGTOCHARGER"] = false;
    state["CHARGIN"] = false;
    state["RESTARTCUT"] = false;
}


void Behaviour::set_impassable_objects()
{
    for (int i = 0; i < 22; ++i) {
            _impassable_objects[i][-17] = 1;
        }
}

void Behaviour::set_instruction_set()
{
    _instruction_set[0][0] = 0;
    _instruction_set[-1][1] = 1;
    _instruction_set[0][1] = 2;
    _instruction_set[1][1] = 3;
    _instruction_set[1][0] = 4;
    _instruction_set[1][-1] = 5;
    _instruction_set[0][-1] = 6;
    _instruction_set[-1][-1] = 7;
    _instruction_set[-1][0] = 8;
}

int Behaviour::_cut_dry_grass()
{
    // check heading
    int x = 0;
    int y = 0;
        

    if ((_complete_passes % 2 == 1) && (((_current_x == 0) 
        && (_current_y == 0) && (_heading == "WEST")) || ((_current_x == 39)
            && (_current_y == 0) && (_heading == "EAST"))))
    {
        std::cout << " pass complete " << std::endl;
        ++_complete_passes;
    }

    if (((_current_x == 0) && (_current_y == -39) 
        && (_heading == "WEST")) || ((_current_x == 39)
            && (_current_y == -39) && (_heading == "EAST")))
    {
        std::cout << " pass complete " << std::endl;
        ++_complete_passes;
    }

    int reverse = 1;

    if (_complete_passes % 2 == 1) { reverse = -1; }

    if (_heading == "EAST") { x = 1; }

    if (_heading == "WEST") { x = -1; }

    // check if at edge of map
    
    if ((_heading == "EAST") && (_current_x == map_width - 1)) {
            
        x = 0;
        y = -1*reverse;
        _heading = "WEST";
    
    }

    if ((_heading == "WEST") && (_current_x == 0)) {
            
        x = 0;
        y = -1*reverse;
        _heading = "EAST";
        
    }
 
    if (_impassable_objects[_current_x + x][_current_y + y] == 1
            && _heading == "EAST") {
        
        // always round objects from the east
        x = 1;
        y = 0;
        _heading = "EAST";
    }

    if (_impassable_objects[_current_x + x][_current_y + y] == 1
            && _heading == "WEST") {
        
        // always round objects from the east
        x = 1;
        y = 0;
        _heading = "EAST";
    }

    _current_x += x;
    _current_y += y;

    return _instruction_set[x][y];

}

int Behaviour::_cut_wet_grass()
{
    int next_action{0};

    if (_rain_steps % 2 == 1) { next_action = _cut_dry_grass(); }
    
    ++_rain_steps;
    
    return next_action;

}

int Behaviour::_move_to_charger()
{
    int y = 1;
    int x = -1;

    if (_current_x == 0) { x = 0; }
    
    if (_current_y == 0) { y = 0; }

    // if behind barrier
    
    if (_current_x <= 21 && _current_y < -17) { x = 1; }
    
    // if at barrier
    if (_impassable_objects[_current_x + x][_current_y + y] == 1)
    {
        y = 0;
        x = 1;
    }
    
    // edge case: at corner
    if (_current_x == 22 && _current_y == -18)
    {
        y = 1;
        x = 0;
    }
    
    _current_x += x;
    _current_y += y;
    
    if ((_current_x == _home_x) && (_current_y == _home_y))
    {
        state["MOVETOCHARGER"] = false;
        state["CHARGIN"] = true;
    }
    return _instruction_set[x][y];

}

int Behaviour::_charge()
{
    if (battery >= 0.999)
    {
        state["CHARGIN"] = false;
        state["RESTARTCUT"] = true;
    };  
    
    return 0;
}

int Behaviour::_restart_cut()
{
    std::cout << " IN RESTART STATE " << std::endl;
    int x{0};
    int y{0};

    if (_current_y > _y_prior_to_charge) { y = -1; }

    if (_current_x < _x_prior_to_charge) { x = 1; }

    if (_current_y < _y_prior_to_charge) { y = 1; }

    if (_current_x > _x_prior_to_charge) { x = -1; }
    
    if (_impassable_objects[_current_x + x][_current_y + y] == 1) {
        
        // always round objects from the east
        x = 1;
        y = 0;
    }

    _current_x += x;
    _current_y += y;

    if ((_current_x == _x_prior_to_charge) && 
        (_current_y == _y_prior_to_charge)) {

        state["RESTARTCUT"] = false;
        state["CUTGRASS"] = true;
    }

    return _instruction_set[x][y];
}

int Behaviour::_pass_object()
{
    return 0;
}

void Behaviour::_reset_steps_in_rain()
{
    _rain_steps = 0;
}

int Behaviour::action() {
    
    int action{0};

    if (state["CUTGRASS"]) 
        {
            action = _cut_dry_grass(); 
            std::cout << "CUTGRASS" << std::endl;
        }
    
    if (state["CUTWETGRASS"])
        {
            action = _cut_wet_grass();
            std::cout << "CUTWETGRASS" << std::endl;
        }
  
    if (state["RESTARTCUT"]) { 
        action = _restart_cut(); 
        std::cout << "RESTARTCUT" << std::endl;
    }

    if (state["CHARGIN"]) {
        action = _charge(); 
        std::cout << "CHARGIN" << std::endl;
    }
    
    if (state["MOVETOCHARGER"]) { 
        action =_move_to_charger(); 
        std::cout << "MOVETOCHARGER" << std::endl;
    }
    
    std::cout << " Pos x: " << _current_x << " , y; " 
        << _current_y << std::endl;
    std::cout << "Passes; " << _complete_passes << std::endl;
    return action;

}

void Behaviour::transition(std::string command) {
    
    if ((command == "MOVETOCHARGER") && 
            !state["MOVETOCHARGER"] && !state["CHARGIN"] 
             && !state["RESTARTCUT"])
    {
        std::cout << " Cost: " << cost_to_home() << " Battery: "
            << battery << std::endl;
        _x_prior_to_charge = _current_x;
        _y_prior_to_charge = _current_y;
        state["CUTGRASS"] = false;
        state["CUTWETGRASS"] = false;
        state["MOVETOCHARGER"] = true;
    }
    
    if ((command == "CUTGRASS") && (state["CUTWETGRASS"]))
    {
        state["CUTGRASS"] = true;
        state["CUTWETGRASS"] = false;
    }

    if ((command == "CUTWETGRASS") && (state["CUTGRASS"]))
    {
        state["CUTGRASS"] = false;
        state["CUTWETGRASS"] = true;
    }
}

double Behaviour::cost_to_home() {

    // basic scenario
    int extra_padding{15};
    double cost = 0;
    
    if (_current_x > -_current_y) {
        cost = _current_x + 3;
    } else {
        cost = -_current_y + 3;
    }
    
    if (_current_y < -17) 
    {
        cost = 25 + (-17 - _current_y);
    }

    if ((_current_x < 22) && (_current_y < -17))
    {
        int temp_x = 22 - _current_x;
        int temp_y = _current_y - 17;
        cost = 28;
        if (temp_x > temp_y) {
            cost += temp_x;
        } else {
            cost += temp_y;
        }
    }

    cost += extra_padding;
    cost *= _standard_power_consumption;
    
    return cost;

}

