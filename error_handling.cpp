#include "error_handling.h"

#include <iostream>

namespace
{

std::ostream& operator<< (std::ostream& os, const std::exception& ex)
{
    os << ex.what();
    return os;
}

std::ostream& operator<< (std::ostream& os, const std::source_location& location)
{
    os << location.file_name() << '('
       << location.line() << ':'
       << location.column() << ") `"
       << location.function_name();
    return os;
}

void PrintException (const auto& e);

void PrintKnownExceptions (const auto& f)
{
    try
    {
        f();
    }
    catch (const TraceableException& ex)
    {
        PrintException (ex);
    }
    catch (const std::exception& ex)
    {
        PrintException (ex);
    }    
}

void PrintException (const auto& e)
{
    std::cerr << e << std::endl;
    PrintKnownExceptions(
        [&]()
        {
            std::rethrow_if_nested (e);
        }
    );
}

} // namespace

std::ostream& operator<< (std::ostream& os, const TraceableException& ex)
{
    os << ex.what() << " failed: ";
    ex.Format (os);
    os << " at " << ex.m_location;
    return os;
}

void TraceException (std::source_location location) noexcept
{
    try
    {
        std::cerr << "Exception caught at " << location << std::endl;
        PrintKnownExceptions(
            [&]()
            {
                throw;
            }
        );    
    }
    catch (...)
    {
        try
        {
            std::cerr << "Exception caught while exception formating" << std::endl;
        }
        catch (...)
        {
        }
    }    
}