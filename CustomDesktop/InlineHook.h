#pragma once


namespace cd
{
	class InlineHook final
	{
	private:
		void* const m_newEntry = NULL;
		void* const m_hookFunction;
		bool m_isEnabled = false;
	public:
		void* const m_originalFunction;

		InlineHook(void* originalFunction, void* hookFunction, bool enable = true);
		InlineHook(InlineHook& other);
		~InlineHook();

		bool Enable();
		bool Disable();

		bool IsEnabled();
	};
}
