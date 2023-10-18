#include "mic.h"
#include "knob.h"

#include "error_handling.h"

auto SwallowEveryTime (auto fun)
{
    return [fun = std::move (fun)]() noexcept
        {
            IgnoreException (fun);
        };
}

int main()
{
    const bool res = DispatchException (
        []()
        {
            Mic mic;
            Knob knob;

            knob.SetPushHandler (SwallowEveryTime(
                [&]()
                {
                    mic.ToggleMute();
                }
            ));

            knob.SetSpinLeftHandler (SwallowEveryTime(
                [&]()
                {
                    mic.DecVol();
                }
            ));

            knob.SetSpinRightHandler (SwallowEveryTime(
                [&]()
                {
                    mic.IncVol();
                }
            ));

            knob.Loop();
       }
    );
 
    return res ? EXIT_SUCCESS : EXIT_FAILURE;
}