#pragma once

#include <memory>
#include <functional>

class Knob
{
public:
    Knob();
    ~Knob();

public:
    using Handler = std::function<void()>;

    void SetPushHandler (Handler);
    void SetSpinLeftHandler (Handler);
    void SetSpinRightHandler (Handler);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};