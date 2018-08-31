#ifndef DLL_INJECT_HPP_
#define DLL_INJECT_HPP_

#include <Windows.h>
#include <string>

namespace Shkolnik
{
	namespace Windows_Native
	{
		class DLL_Inject
		{
		// This section contains all private variables
		private:
			std::wstring dll_path{};
			int process_id{};
		
		// This section contains all private methods
		private:
			// Native method, that inject dll into process by id
			bool NativeInjectDLL()
			{
				HANDLE hProcess{ OpenProcess(PROCESS_ALL_ACCESS, false, this->process_id) };
				if (!hProcess)
					return false;

				LPVOID mem_allocation = VirtualAllocEx(hProcess, NULL, dll_path.size() + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				if (!mem_allocation)
					return false;

				if (int alloc_bytes{ WriteProcessMemory(hProcess, mem_allocation, this->dll_path.c_str(), this->dll_path.size()*2, 0) }; alloc_bytes <= 0)
					return false;

				DWORD dummy{};
				HANDLE RemThread{};
				if (RemThread = CreateRemoteThread(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(LoadLibrary(L"kernel32"), "LoadLibraryW")),
					mem_allocation, 0, &dummy); !RemThread)
					return false;

				CloseHandle(RemThread);
				CloseHandle(hProcess);
				return true;
			}

		public:
			// CTOR
			DLL_Inject(const std::wstring &dll_path, const int &process_id)
				: dll_path(dll_path), process_id(process_id)
			{
				NativeInjectDLL();
			}

			// DTOR
			~DLL_Inject() {}
		};
	}
}

#endif // !DLL_INJECT_HPP_
