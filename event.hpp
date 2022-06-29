#pragma once

struct Event
{
    int const   &fd;
    bool        is_hang_up;
    void        *default_server_ptr;
    void        *owner;

    Event(int const &fd, void *default_server_ptr, void *owner) : fd(fd), is_hang_up(false), default_server_ptr(default_server_ptr), owner(owner)  {}
    ~Event() {}
};
