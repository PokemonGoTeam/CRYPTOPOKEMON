#include "TextUtils.h"
#include <clocale>

const wchar_t* TextUtils::get_wc(const char *c)
{
	const auto c_size = strlen(c) + 1;
	const auto wc = new wchar_t[c_size];
	mbstowcs(wc, c, c_size);

	return wc;
}

std::string TextUtils::w_convert_to_string(String ws)
{
	std::setlocale(LC_ALL, "");
	const std::locale locale("");
	typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
	const auto& converter = std::use_facet<converter_type>(locale);
	std::vector<char> to(ws.length() * converter.max_length());
	std::mbstate_t state;
	const wchar_t* from_next;
	char* to_next;
	const auto result = converter.out(state, ws.data(), ws.data() + ws.length(), from_next, &to[0], &to[0] + to.size(), to_next);
	if (result == converter_type::ok || result == converter_type::noconv) {
		const std::string s(&to[0], to_next);
		return s;
	}
	return nullptr;
}