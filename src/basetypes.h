#pragma once

#include <constants.h>
#include <container.h>

#include <string>
#include <cstring>
#include <memory>
#include <cmath>
#include <algorithm>
#include <iostream>

struct Point
{
    int x, y;
    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}

    inline bool operator==(const Point &a) const { return x == a.x && y == a.y; }
    inline bool operator!=(const Point &a) const { return !(*this == a); }
    inline Point operator+(const Point &a) const { return Point(x + a.x, y + a.y); }
    inline Point operator-(const Point &a) const { return Point(x - a.x, y - a.y); }
    inline Point operator-() const { return Point(-x, -y);}
    inline int length() const {return x+y;}
    inline friend std::istream &operator>>(std::istream &is, Point &p) { return is >> p.x >> p.y; }
    inline friend std::ostream &operator<<(std::ostream &os, Point &p) { return os << "(" << p.x << ", " << p.y << ")"; }
};

const char moves_str[5] = {'o', 'n', 'e', 's', 'w'};
const Point moves_dir[5] = {Point(0, 0), Point(0, -1), Point(1, 0), Point(0, 1), Point(-1, 0)};

template <class T>
class Grid
{
  public:
    size_t width, height;
    std::unique_ptr<T[]> data;

    Grid() : width{0}, height{0} {}
    Grid(size_t width, size_t height)
        : width{width},
          height{height},
          data{std::make_unique<T[]>(width * height)} {}

    Grid(const Grid<T> &other)
        : Grid(other.width, other.height)
    {
        std::copy(other.data.get(), other.data.get() + width * height, data.get());
    }

    Grid<T> &operator=(const Grid<T> &other)
    {
        width = other.width;
        height = other.height;
        data = std::make_unique<T[]>(width * height);
        std::copy(other.data.get(), other.data.get() + width * height, data.get());
        return *this;
    }

    T &at(size_t idx) { return data[idx]; }
    T &at(size_t x, size_t y) { return data[y * width + x]; }
    T &at(const Point &p) { return at(p.x, p.y); }
    T &operator[](const Point &p) { return at(p.x, p.y); }

    inline void reset()
    {
        data = std::make_unique<T[]>(width * height);
    }

    inline void normalize(Point &p)
    {
        p.x = (p.x + width) % width;
        p.y = (p.y + height) % height;
    }

    inline friend std::ostream &operator<<(std::ostream &os, Grid<T> &g)
    {
        for (size_t y = 0; y < g.height; y++)
        {
            for (size_t x = 0; x < g.width; x++)
                os << g.at(x, y) << " ";
            os << std::endl;
        }
        return os;
    }

    inline int dist(const Point &p, const Point &q)
    {
        int dx = std::abs(p.x - q.x);
        int dy = std::abs(p.y - q.y);

        return std::min(dx, (int)width - dx) + std::min(dy, (int)height - dy);
    }
};

class Entity
{
  public:
    
    int id, owner;
    bool active;
    Point pos;

    Entity() : id(0), owner(0), active(false), pos(0, 0) {}
    Entity(int id, int owner, int x, int y) : id(id), owner(owner), active(true), pos(x, y) {}
};

class Ship : public Entity
{
  public:
    int halite;
    bool inspired, just_moved;
    int action, task;
    Point target;

    Ship() : Entity(), halite(0), inspired(false), just_moved(false), action(-1), task(-1), target(-1, -1) {}
    Ship(int id, int owner, int x, int y, int halite) : Entity(id, owner, x, y), halite(halite), inspired(false), just_moved(false), action(-1), task(-1), target(-1, -1) {}

    inline void update(int id, int owner, int x, int y, int halite)
    {
        this->id = id;
        this->owner = owner;
        this->halite = halite;

        pos = Point(x, y);
        
        active = true;
        inspired = false;

        just_moved = false;
        action = -1;
    }
    std::string get_command() 
    {
        if (action == -1)
            return "m " + std::to_string(id) + " o ";
        else if (action == 5) 
            return "c " + std::to_string(id) + " ";
        else
            return "m " + std::to_string(id) + " " + moves_str[action] + " ";
    }

    inline friend std::ostream &operator<<(std::ostream &os, Ship &s) { return os << "S={id: " << s.id << ", owner: " << s.owner << ", pos: " << s.pos << ", halite: " << s.halite << ", active: " << s.active << "}"; }
};

class Dropoff : public Entity
{
  public:
    Dropoff() : Entity() {}
    Dropoff(int id, int owner, int x, int y) : Entity(id, owner, x, y) {}

    inline void update(int id, int owner, int x, int y)
    {
        this->id = id;
        this->owner = owner;
        active = true;
        pos = Point(x, y);
    }

    inline friend std::ostream &operator<<(std::ostream &os, Dropoff &d) { return os << "D={id: " << d.id << ", owner: " << d.owner << ", pos: " << d.pos << "}"; }
};

using PlayerShips = Container<MAX_SHIPS, Ship *>;
using PlayerDropoffs = Container<MAX_DROPOFFS, Dropoff *>;

class Player
{
  public:
    int id, halite;
    Point spawn;

    int action;

    PlayerShips ships;
    PlayerDropoffs dropoffs;

    Player() : id(0), halite(0), spawn(0, 0), action(0), ships(), dropoffs() {}
    Player(int id, int x, int y, Dropoff *shipyard) : id(id), halite(0), spawn(x, y), action(0)
    {
        dropoffs.put(shipyard);
    }

    void update(int halite)
    {
        this->action = 0;
        this->halite = halite;
        ships.clear();

        Dropoff* shipyard = dropoffs[0];
        dropoffs.clear();
        dropoffs.put(shipyard);
    }

    inline friend std::ostream &operator<<(std::ostream &os, Player& p) { return os << "P={id: " << p.id << ", halite: " << p.halite << ", spawn: " << p.spawn << "}"; }
};

using Map = Grid<int>;
using Mask = Grid<bool>;

struct Sector {
    Point centroid;
    int halite;
};