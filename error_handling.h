#pragma once

#include <iosfwd>
#include <utility>
#include <stdexcept>
#include <source_location>

class TraceableException : std::runtime_error
{
public:
    explicit TraceableException (const char* msg, std::source_location location):
        std::runtime_error {msg}, m_location {std::move (location)}
    {        
    }

public:
    friend std::ostream& operator<< (std::ostream&, const TraceableException&);

protected:
    virtual void Format (std::ostream&) const = 0;

private:
    const std::source_location m_location;
};

void TraceException (std::source_location location) noexcept;

[[nodiscard]]
bool DispatchException (const auto& f, std::source_location location = std::source_location::current()) noexcept
{
    try
    {
        f();
        return true;
    }
    catch (...)
    {
        TraceException (std::move (location));
        return false;
    }
}

void IgnoreException (const auto& f, std::source_location location = std::source_location::current()) noexcept
{
    (void)DispatchException (f, std::move (location));
}
