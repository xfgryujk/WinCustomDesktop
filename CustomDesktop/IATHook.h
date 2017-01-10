#pragma once


namespace cd
{
	template<class FunctionType>
	class IATHook final
	{
	private:
		FunctionType* m_importAddress = NULL;
		const FunctionType m_hookFunction = NULL;
		bool m_isEnabled = false;


		static FunctionType* FindImportAddress(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName)
		{
			// 被hook的模块基址
			uintptr_t hookModuleBase = (uintptr_t)hookModule;
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hookModuleBase;
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(hookModuleBase + dosHeader->e_lfanew);
			// 导入表
			PIMAGE_IMPORT_DESCRIPTOR importTable = (PIMAGE_IMPORT_DESCRIPTOR)(hookModuleBase
				+ ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

			// 遍历导入的模块
			for (; importTable->Characteristics != 0; importTable++)
			{
				// 不是函数所在模块
				if (_stricmp((LPCSTR)(hookModuleBase + importTable->Name), moduleName) != 0)
					continue;

				PIMAGE_THUNK_DATA info = (PIMAGE_THUNK_DATA)(hookModuleBase + importTable->OriginalFirstThunk);
				FunctionType* iat = (FunctionType*)(hookModuleBase + importTable->FirstThunk);

				// 遍历导入的函数
				for (; info->u1.AddressOfData != 0; info++, iat++)
				{
					if ((info->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0) // 是用函数名导入的
					{
						PIMAGE_IMPORT_BY_NAME name = (PIMAGE_IMPORT_BY_NAME)(hookModuleBase + info->u1.AddressOfData);
						if (strcmp((LPCSTR)name->Name, functionName) == 0)
							return iat;
					}
				}

				return NULL; // 没找到要hook的函数
			}

			return NULL; // 没找到要hook的模块
		}

		bool ModifyIAT(FunctionType newFunction)
		{
			if (m_importAddress == NULL)
				return false;

			__try
			{
				// 修改IAT中地址为hookFunction
				DWORD oldProtect = 0, oldProtect2 = 0;
				if (!VirtualProtect(m_importAddress, sizeof(FunctionType), PAGE_READWRITE, &oldProtect))
					oldProtect = 0;
				*m_importAddress = newFunction;
				VirtualProtect(m_importAddress, sizeof(FunctionType), oldProtect, &oldProtect2);
			}
			__except (GetExceptionCode() == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
			{
				return false;
			}

			return true;
		}

	public:
		const FunctionType m_oldEntry = NULL;


		IATHook(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName, FunctionType hookFunction, bool enable = true) :
			m_importAddress(FindImportAddress(hookModule, moduleName, functionName)),
			m_hookFunction(hookFunction), 
			m_oldEntry(m_importAddress != NULL ? *m_importAddress : NULL)
		{
			if (m_importAddress == NULL)
				return;

			if (enable)
				Enable();
		}

		IATHook(IATHook& other) :
			m_importAddress(other.m_importAddress),
			m_hookFunction(other.m_hookFunction),
			m_isEnabled(other.m_isEnabled),
			m_oldEntry(other.m_oldEntry)
		{
			other.m_importAddress = NULL;
			other.m_isEnabled = false;
		}

		~IATHook()
		{
			Disable();
		}


		bool Enable()
		{
			if (m_isEnabled)
				return true;
			if (ModifyIAT(m_hookFunction))
			{
				m_isEnabled = true;
				return true;
			}
			return false;
		}

		bool Disable()
		{
			if (!m_isEnabled)
				return true;
			if (ModifyIAT(m_oldEntry))
			{
				m_isEnabled = false;
				return true;
			}
			return false;
		}


		bool IsEnabled()
		{
			return m_isEnabled;
		}
	};
}
