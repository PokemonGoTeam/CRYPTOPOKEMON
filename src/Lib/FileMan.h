#pragma once
#include "prog_defines.h"

class FileManager {

public:

	std::vector<String> BlackPatches;

	bool is_decrypt = false;

	static String normalize_path(String path);

	static String get_temp_folder();

	static String get_defined_path(int csidl);

	static void create_info_file(String file_path);

	void recursive_search(String path);

	static std::vector<String> show_drive_letters();
};
