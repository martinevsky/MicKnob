#pragma once

#include <memory>

class Mic
{
public:
    Mic();
    ~Mic();

public:
    void ToggleMute();
    void IncVol();
    void DecVol();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};