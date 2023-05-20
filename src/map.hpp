#ifndef MAP
#define MAP

#include <map>

using namespace std;

class Map {

    private:

        Map(Map const &) = delete;
        Map(Map &&) = delete;
        Map &operator=(Map const &) = delete;
        Map &operator=(Map &&) = delete;

    public:
        Map() noexcept;
        ~Map() = default;

    public:
        
        void cut_tile(int, int);
        void update_tile(int, int, int);
        void document_rain(int, int, int);
        void remove_rain(int, int);
        int get_cut(int, int);
        int get_rain(int, int);

    private:
        
        map<int, map<int, int>> _cut_map;
        map<int, map<int, int>> _rain_map;
        
        int _current_rain_x;
        int _current_rain_y;

}    
#endif
