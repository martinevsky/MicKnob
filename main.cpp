#include "mic.h"
#include "knob.h"

auto Swallow (auto fun)
{
    return [fun = std::move (fun)]() noexcept
        {
            try
            {
                fun();
            }
            catch(...)
            {
                // TODO: Trace something
            }
        };
}

int main()
{
    try
    {
        Mic mic;
        Knob knob;

        knob.SetPushHandler (Swallow(
            [&]()
            {
                mic.ToggleMute();
            }
        ));

        knob.SetSpinLeftHandler (Swallow(
            [&]()
            {
                mic.DecVol();
            }
        ));

        knob.SetSpinRightHandler (Swallow(
            [&]()
            {
                mic.IncVol();
            }
        ));

        knob.Loop();

        return EXIT_SUCCESS; 
    }
    catch (...)
    {
        // TODO: Trace something
        return EXIT_FAILURE;
    }
}