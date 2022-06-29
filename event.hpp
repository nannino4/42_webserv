#pragma once

struct Event
{
    int const   &fd;
    bool        is_hang_up;
    void        *default_server_ptr;
    void        *owner;

    Event(int const &fd, void *default_server_ptr, void *owner) : fd(fd), is_hang_up(false), default_server_ptr(default_server_ptr), owner(owner)  {}
    Event(Event const &other) : fd(other.fd) { *this = other; }
    Event &operator=(Event const &other)
    {
        is_hang_up = other.is_hang_up;
        default_server_ptr = other.default_server_ptr;
        owner = other.owner;
        return *this;
    }
    ~Event() {}
};
