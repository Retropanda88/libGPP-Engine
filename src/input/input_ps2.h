#ifndef GPP_INPUT_PS2_H
#define GPP_INPUT_PS2_H

#if defined(PS2_BUILD)

#include <input/input.h>

// headers PS2
#include <kernel.h>
#include <libpad.h>

namespace gpp
{
    class InputPS2
    {
    public:
        InputPS2();

        bool init();
        void shutdown();

        // Se llama cada frame
        void update(Input& input);

    private:
        int m_port;
        int m_slot;
    };
}

#endif // PS2_BUILD
#endif // GPP_INPUT_PS2_H