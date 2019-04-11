#pragma once

#include <Windows.h>

class ManchineID
{
public:
	static void get_mac_hash(SHORT& mac1, SHORT& mac2);
	static SHORT get_volume_hash();
	static SHORT get_cpu_hash();
	static const wchar_t* get_machine_name();
};

