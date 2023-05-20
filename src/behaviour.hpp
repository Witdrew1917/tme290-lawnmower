#ifndef BEHAVIOUR
#define BEHAVIOUR


class Behaviour {

    private:

        Behaviour(Behaviour const &) = delete;
        Behaviour(Behaviour &&) = delete;
        Behaviour &operator=(Behaviour const &) = delete;
        Behaviour &operator=(Behaviour &&) = delete;

    public:
        Behaviour() noexcept;
        ~Behaviour() = default;
        
        bool CUTGRASS;
        bool CUTWETGRASS;
        bool MOVINGTOCHARGER;
        bool CHARGING;

    public:

        int _cut_dry_grass(); 
        int _cut_wet_grass();
        int _move_to_charger();
        int _charge();

    private:
        int _current_x;
        int _current_y;

        int _home_x;
        int _home_y;
}
#endif
