#include "Clipboard.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace flo {
	std::string getFromClipboard() {
#ifdef _WIN32
		OpenClipboard(HWND());
		HANDLE data = GetClipboardData(CF_TEXT);
		CloseClipboard();
		if (!data) return std::string();
		return std::string((char*)data);
#else
		return ""
#endif
	}

	void copyToClipboard(const std::string& str) {
#ifdef _WIN32
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, str.size() + 1);
		memcpy(GlobalLock(hMem), str.data(), str.size() + 1);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
#endif
	}
}