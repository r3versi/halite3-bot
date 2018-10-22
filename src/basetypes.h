#pragma once

#include <constants.h>

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
  private:
    T **grid;

  public:
    size_t width, height;
    Grid() : width(0), height(0) { grid = nullptr; }
    Grid(size_t width, size_t height) : width(width), height(height)
    {
        grid = (T **)calloc(width, sizeof(T *) * height);
        for (size_t i = 0; i < width; i++)
            grid[i] = (T *)calloc(height, sizeof(T));
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
    Ship() : Entity(), halite(0) {}
    Ship(int id, int owner, int x, int y, int halite) : Entity(id, owner, x, y), halite(halite) {}

    inline void update(int x, int y, int halite)
    {
        active = true;
        pos = Point(x, y);
        this->halite = halite;
    }

    inline friend std::ostream &operator<<(std::ostream &os, Ship &s) { return os << "S" << s.id << " " << s.owner << " " << s.pos << " " << s.halite; }
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

    inline friend std::ostream &operator<<(std::ostream &os, Dropoff &d) { return os << "D" << d.id << " " << d.owner << " " << d.pos; }
};

class Player
{
  public:
    size_t id, halite;
    Dropoff spawn;

    size_t n_ships, n_dropoffs;
    size_t ships[MAX_SHIPS];
    size_t dropoffs[MAX_DROPOFFS];

    Player() : id(0), halite(0), spawn(0, 0, 0, 0), n_ships(0), n_dropoffs(0), ships{0}, dropoffs{0} {}
    Player(size_t id, int x, int y) : id(id), halite(0), spawn(id, id, x, y), n_ships(0), n_dropoffs(1), ships{0}, dropoffs{id} {}

    void update(size_t n_ships, size_t n_dropoffs, size_t halite)
    {
        this->n_ships = n_ships;
        this->n_dropoffs = n_dropoffs;
        this->halite = halite;
    }

    inline friend std::ostream &operator<<(std::ostream &os, Player& p) { return os << "P" << p.id << " " << p.halite << " " << p.spawn; }
};

using Map = Grid<int>;
using Mask = Grid<bool>;
