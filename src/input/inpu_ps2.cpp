#if defined(PS2_BUILD)

#include <input_ps2.h>
#include <cstring>

namespace gpp
{
	InputPS2::InputPS2():m_port(0), m_slot(0)
	{
	}

	bool InputPS2::init()
	{
		padInit(0);

		if (padPortOpen(m_port, m_slot, nullptr) == 0)
			return false;

		// esperar a que el pad esté listo
		int state;
		do
		{
			state = padGetState(m_port, m_slot);
		}
		while (state != PAD_STATE_STABLE && state != PAD_STATE_FINDCTP1);

		return true;
	}

	void InputPS2::shutdown()
	{
		padPortClose(m_port, m_slot);
	}

	void InputPS2::update(Input & input)
	{
		padButtonStatus buttons;

		if (padRead(m_port, m_slot, &buttons) == 0)
			return;

		// En PS2: 0 = presionado, 1 = libre
		unsigned short btns = buttons.btns;

		input.setActionState(ACTION_UP, !(btns & PAD_UP));
		input.setActionState(ACTION_DOWN, !(btns & PAD_DOWN));
		input.setActionState(ACTION_LEFT, !(btns & PAD_LEFT));
		input.setActionState(ACTION_RIGHT, !(btns & PAD_RIGHT));

		input.setActionState(ACTION_CONFIRM, !(btns & PAD_CROSS));
		input.setActionState(ACTION_CANCEL, !(btns & PAD_CIRCLE));
		input.setActionState(ACTION_MENU, !(btns & PAD_START));
		input.setActionState(ACTION_BACK, !(btns & PAD_TRIANGLE));
	}
}

#endif // PS2_BUILD
