#include "input/input.h"

namespace gpp
{
	Input::Input()
	{
		init();
	}

	void Input::init()
	{
		for (int i = 0; i < ACTION_MAX; ++i)
		{
			m_states[i] = STATE_IDLE;
			m_prevStates[i] = STATE_IDLE;
		}
	}

	void Input::shutdown()
	{
		// Reservado para futuro (si agregas buffers o devices)
	}

	void Input::update()
	{
		for (int i = 0; i < ACTION_MAX; ++i)
		{
			m_prevStates[i] = m_states[i];

			if (m_states[i] == STATE_PRESSED)
			{
				m_states[i] = STATE_HELD;
			}
			else if (m_states[i] == STATE_RELEASED)
			{
				m_states[i] = STATE_IDLE;
			}
		}
	}

	void Input::setActionState(InputAction action, bool pressed)
	{
		if (action <= ACTION_NONE || action >= ACTION_MAX)
			return;

		if (pressed)
		{
			if (m_states[action] == STATE_IDLE)
			{
				m_states[action] = STATE_PRESSED;
			}
		}
		else
		{
			if (m_states[action] == STATE_HELD)
			{
				m_states[action] = STATE_RELEASED;
			}
		}
	}

	bool Input::isPressed(InputAction action) const
	{
		return m_states[action] == STATE_PRESSED;
	}

	bool Input::isHeld(InputAction action) const
	{
		return m_states[action] == STATE_HELD;
	}

	bool Input::isReleased(InputAction action) const
	{
		return m_states[action] == STATE_RELEASED;
	}

	bool Input::pollEvent(InputEvent & event)
	{
		for (int i = 0; i < ACTION_MAX; ++i)
		{
			if (m_states[i] == STATE_PRESSED || m_states[i] == STATE_RELEASED)
			{
				event.action = static_cast < InputAction > (i);
				event.state = m_states[i];
				return true;
			}
		}
		return false;
	}
}
