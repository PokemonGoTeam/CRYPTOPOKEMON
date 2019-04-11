#pragma once
#include "prog_defines.h"

class TextUtils
{
public:
	/// const char* to const wchar_t*
	static const wchar_t* get_wc(const char *c);

	/// String (wstring) to string
	static std::string w_convert_to_string(String ws);
};

