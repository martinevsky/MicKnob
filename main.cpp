#include "mic.h"
#include "knob.h"

#include "error_handling.h"

int main()
{
    const bool res = DispatchException (
        []()
        {
            Mic mic;
            Knob knob;

            const auto ignoreExceptions = [](auto fun)
                {
                    return [fun = std::move (fun)]() noexcept
                        {
                            IgnoreException (fun);
                        };
                };

            knob.SetPushHandler (ignoreExceptions(
                [&]()
                {
                    mic.ToggleMute();
                }
            ));

            knob.SetSpinLeftHandler (ignoreExceptions(
                [&]()
                {
                    mic.DecVol();
                }
            ));

            knob.SetSpinRightHandler (ignoreExceptions(
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