#pragma once

#include <constants.h>
#include <container.h>

#include <string>
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
    //Point operator-() const { return Point(-x, -y);}

    inline friend std::istream &operator>>(std::istream &is, Point &p) { return is >> p.x >> p.y; }
    inline friend std::ostream &operator<<(std::ostream &os, Point &p) { return os << "(" << p.x << ", " << p.y << ")"; }
};
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

    T &at(size_t x, size_t y) { return data[y * width + x]; }
    T &at(const Point &p) { return at(p.x, p.y); }
    T &operator[](const Point &p) { return at(p.x, p.y); }

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

/*
template <class T>
class Grid
{
  public:
    T **grid;

    size_t width, height;
    Grid() : width(0), height(0) { grid = nullptr; }

    Grid(size_t width, size_t height) : width(width), height(height)
    {
        grid = (T **)calloc(width, sizeof(T *) * height);
        for (size_t i = 0; i < width; i++)
            grid[i] = (T *)calloc(height, sizeof(T));
    }

    Grid(const Grid<T>& g)
    {
        Grid(g.width, g.height);
        
        memcpy(grid, g.grid, sizeof(T)*width*height);
    }

    T &at(int x, int y) { return grid[x][y]; }
    const T &at(int x, int y) const { return grid[x][y]; }

    T &at(const Point &p) { return grid[p.x][p.y]; }
    const T &at(const Point &p) const { return grid[p.x][p.y]; }

    T &operator[](const Point &p) { return grid[p.x][p.y]; }
    const T &operator[](const Point &p) const { return grid[p.x][p.y]; }

    inline void normalize(Point &p)
    {
        p.x = (p.x + width) % width;
        p.y = (p.y + height) % height;
    }

    inline friend std::ostream &operator<<(std::ostream &os, Grid<T> &g)
    {
        for (size_t i = 0; i < g.height; i++)
        {
            for (size_t j = 0; j < g.width; j++)
                os << g.at(i, j) << " ";
            os << std::endl;
        }
        return os;
    }

    int dist(const Point& p, const Point& q) 
    {
        int dx = std::abs(p.x - q.x);
        int dy = std::abs(p.y - q.y);

        return std::min(dx, (int)width - dx) + std::min(dy, (int)height - dy);
    }
};
*/
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
    int action;
    Ship() : Entity(), halite(0), inspired(false), just_moved(false), action(-1) {}
    Ship(int id, int owner, int x, int y, int halite) : Entity(id, owner, x, y), halite(halite), inspired(false), just_moved(false), action(-1) {}

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

    inline friend std::ostream &operator<<(std::ostream &os, Ship &s) { return os << "S={id: " << s.id << ", owner: " << s.owner << ", pos: " << s.pos << ", halite: " << s.halite << ", active: " << s.active << "}"; }
};

class Dropoff : public Entity
{
  public:
    Dropoff() : Entity() {}
    Dropoff(int id, int owner, int x, int y) : Entity(id, owner, x, y) {}

    inline void update(int x, int y)
    {
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
    size_t id, halite;
    Point spawn;

    int action;

    PlayerShips ships;
    PlayerDropoffs dropoffs;

    Player() : id(0), halite(0), spawn(0, 0), action(0), ships(), dropoffs() {}
    Player(size_t id, int x, int y, Dropoff *shipyard) : id(id), halite(0), spawn(x, y), action(0)
    {
        dropoffs.put(shipyard);
    }

    void update(size_t halite)
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
