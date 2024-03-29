#pragma once

#define READ 0
#define WRITE 1
#define ERROR 2

struct Event
{
    int			fd;
    bool        is_hang_up;
    bool        is_error;
    void        *default_server_ptr;
    void        *owner;
    int         events;

    Event(int fd, void *default_server_ptr, void *owner) : fd(fd), is_hang_up(false), is_error(false), default_server_ptr(default_server_ptr), owner(owner), events(-1) {}
    Event(Event const &other) : fd(other.fd) { *this = other; }
    Event &operator=(Event const &other)
    {
        is_hang_up = other.is_hang_up;
		is_error = other.is_error;
        default_server_ptr = other.default_server_ptr;
        owner = other.owner;
        events = other.events;
        return *this;
    }
    ~Event() {}
};
