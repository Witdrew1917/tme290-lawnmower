#include "cluon-complete.hpp"
#include "tme290-sim-grass-msg.hpp"
#include "behaviour.hpp"

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << argv[0] 
            << " is a lawn mower control algorithm." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OpenDLV session>" 
            << "[--verbose]" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=111 --verbose"
            << std::endl;
        retCode = 1;
    } else {
        bool const verbose{commandlineArguments.count("verbose") != 0};
        uint16_t const cid = std::stoi(commandlineArguments["cid"]);
    
        cluon::OD4Session od4{cid};

        Behaviour behaviour;
        behaviour.set_initial_states();
        behaviour.set_impassable_objects();
        behaviour.set_instruction_set();
        behaviour.map_width = 40;
        behaviour.map_height = 40;

        float battery_power{0.0};

        auto onSensors{[&od4, &behaviour, &battery_power]
            (cluon::data::Envelope &&envelope)
        {
            auto msg = cluon::extractMessage<tme290::grass::Sensors>(
            std::move(envelope));
        
            tme290::grass::Control control;
            
            if (msg.rain() >= 0.001) { 
                behaviour.transition("CUTWETGRASS"); 
            }

            if (msg.rain() < 0.001) { 
                behaviour.transition("CUTGRASS"); 
            }
            
            const double cost = behaviour.cost_to_home();

            if (double(msg.battery()) <= 1.1*cost) {
                behaviour.transition("MOVETOCHARGER");
            }

            control.command(behaviour.action());
            


            std::cout << " Change in batery: " 
                << (msg.battery() - behaviour.battery) << std::endl;
            
            behaviour.battery = msg.battery();

            
            /*std::cout << "Rain reading " << msg.rain() << ", direction (" 
                << msg.rainCloudDirX() << ", " << msg.rainCloudDirY()
                << ")" << std::endl; 
            */
            od4.send(control);
        }};

        auto onStatus{[&verbose](cluon::data::Envelope &&envelope)
        {
            auto msg = cluon::extractMessage<tme290::grass::Status>(
                std::move(envelope));
            /*
            if (verbose) {
                std::cout << "Status at time " << msg.time() << ": " 
                    << msg.grassMean() << "/" << msg.grassMax() << std::endl;
            }
            */
        }};

        od4.dataTrigger(tme290::grass::Sensors::ID(), onSensors);
        od4.dataTrigger(tme290::grass::Status::ID(), onStatus);

        if (verbose) {
            std::cout << 
                "All systems ready, let's cut some grass!" << std::endl;
        }

        tme290::grass::Control control;
        control.command(0);
        od4.send(control);

        while (od4.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        retCode = 0;
    }
    return retCode;
}

