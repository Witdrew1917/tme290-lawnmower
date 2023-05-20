
Map::Map() noexcept:
    _cut_map{},
    _rain_map{},
    _current_rain_x{},
    _current_rain_y{}
{
}

void Map::cut_tile(int x, int y)
{
    _cut_map[x][y] += 1;
}

void Map::update_tile(int x, int y, int val)
{
    _cut_map[x][y] = val;
}

void Map::document_rain(int x, int y, int time)
{
    _rain_map[x][y] = time;
    _current_rain_x = x;
    _current_rain_y = y;
}

void Map::remove_rain(int x, int y)
{
    _rain_map[x][y] = 0;
}

int Map::get_cut(int x, int y)
{
    return _cut_map[x][y];
}

int Map::get_rain(int x, int y)
{
    return _rain_map[x][y];
}       

