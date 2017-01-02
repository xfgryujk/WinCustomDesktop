#include "stdafx.h"
#include "InlineHook.h"


namespace cd
{
	namespace
	{
#pragma pack(push)
#pragma pack(1)
		struct JmpCode
		{
		private:
#ifndef _WIN64
			const BYTE jmpCode = 0xE9; // JMP指令的机器码，近跳转为E9，可跳至同一个段的范围内的地址
			uintptr_t relativeAddress; // 相对地址 = 目标地址 - 下一条指令地址 = 目标地址 - 当前地址 - JMP指令长度
#else
			const BYTE rexPrefix = 0x48;
			const BYTE movRax = 0xB8;
			uintptr_t address; // 绝对地址
			const BYTE jmp = 0xFF;
			const BYTE jmpRax = 0xE0;
#endif

		public:
			JmpCode(void* srcAddr, void* dstAddr)
			{
				setAddress(srcAddr, dstAddr);
			}

#ifndef _WIN64
			void setAddress(void* srcAddr, void* dstAddr)
			{
				relativeAddress = (uintptr_t)dstAddr - (uintptr_t)srcAddr - sizeof(JmpCode); // JMP指令长度 = sizeof(JmpCode) = 5 (32位)
			}
#else
			void setAddress(void* srcAddr, void* dstAddr)
			{
				address = (uintptr_t)dstAddr;
			}
#endif
		};
#pragma pack(pop)
	}

	InlineHook::InlineHook(void* originalFunction, void* hookFunction, bool enable) :
		m_originalFunction(originalFunction),
		m_hookFunction(hookFunction),
		m_newEntry(VirtualAlloc(NULL, sizeof(JmpCode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
	{
		if (m_newEntry == NULL)
			return;

		// 保存originalFunction开头前sizeof(JmpCode)字节
		memcpy(m_newEntry, m_originalFunction, sizeof(JmpCode));

		if (enable)
			Enable();
	}

	InlineHook::InlineHook(InlineHook& other) :
		m_originalFunction(other.m_originalFunction),
		m_hookFunction(other.m_hookFunction),
		m_newEntry(other.m_newEntry)
	{
		(void*&)other.m_originalFunction = NULL;
		(void*&)other.m_hookFunction = NULL;
		(void*&)other.m_newEntry = NULL;
	}

	InlineHook::~InlineHook()
	{
		Disable();

		// 释放newEntry
		if (m_newEntry != NULL)
			VirtualFree(m_newEntry, 0, MEM_RELEASE);
	}

	bool InlineHook::Enable()
	{
		if (m_newEntry == NULL)
			return false;
		if (m_isEnabled)
			return true;

		// 改变虚拟内存保护，使originalFunction开头可写，用WriteProcessMemory写内存的话好像不用改保护
		DWORD oldProtect, oldProtect2;
		if (!VirtualProtect(m_originalFunction, sizeof(JmpCode), PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		// 从originalFunction开头跳到hookFunction开头的指令
		JmpCode code(m_originalFunction, m_hookFunction);
		// 修改originalFunction开头的指令
		memcpy(m_originalFunction, &code, sizeof(code));

		// 恢复虚拟内存保护
		VirtualProtect(m_originalFunction, sizeof(JmpCode), oldProtect, &oldProtect2);

		m_isEnabled = true;
		return true;
	}

	bool InlineHook::Disable()
	{
		if (m_newEntry == NULL)
			return false;
		if (!m_isEnabled)
			return true;

		// 改变虚拟内存保护，使originalFunction开头可写
		DWORD oldProtect, oldProtect2;
		if (!VirtualProtect(m_originalFunction, sizeof(JmpCode), PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		// 恢复originalFunction开头的指令
		memcpy(m_originalFunction, m_newEntry, sizeof(JmpCode));

		// 恢复虚拟内存保护
		VirtualProtect(m_originalFunction, sizeof(JmpCode), oldProtect, &oldProtect2);

		m_isEnabled = false;
		return true;
	}

	bool InlineHook::IsEnabled()
	{
		return m_isEnabled;
	}
}
