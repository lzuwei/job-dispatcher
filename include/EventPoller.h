#pragma once

class EventPoller
{
public:
    EventPoller() {}
    virtual ~EventPoller() {}

    virtual int poll(int timeout) = 0;
    virtual int service() = 0;

protected:
private:
};
