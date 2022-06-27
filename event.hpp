struct Event
{
    int     &fd;
    void    *default_server_ptr;

    Event(int &fd, void *ptr) : fd(fd), default_server_ptr(ptr) {}
    ~Event() {}
};
