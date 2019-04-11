/*
 * ManchineID.cpp
 *
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */

#include "ManchineID.h"
#include <intrin.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

SHORT hashMacAddress(PIP_ADAPTER_INFO info)
{
	SHORT hash = 0;
	for (UINT i = 0; i < info->AddressLength; i++)
	{
		hash += (info->Address[i] << ((i & 1) * 8));
	}
	return hash;
}

void ManchineID::get_mac_hash(SHORT& mac1, SHORT& mac2)
{
	IP_ADAPTER_INFO AdapterInfo[32];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS)
		return; // no adapters.

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	mac1 = hashMacAddress(pAdapterInfo);
	if (pAdapterInfo->Next)
		mac2 = hashMacAddress(pAdapterInfo->Next);

	// sort the mac addresses. We don't want to invalidate
	// both macs if they just change order.
	if (mac1 > mac2)
	{
		SHORT tmp = mac2;
		mac2 = mac1;
		mac1 = tmp;
	}
}

SHORT ManchineID::get_volume_hash()
{
	DWORD serialNum = 0;

	// Determine if this volume uses an NTFS file system.
	GetVolumeInformation(L"c:\\", nullptr, 0, &serialNum, nullptr, nullptr, nullptr, 0);
	const auto hash = static_cast<SHORT>(serialNum + (serialNum >> 16) & 0xFFFF);

	return hash;
}

SHORT ManchineID::get_cpu_hash()
{
	int cpuinfo[4] = { 0, 0, 0, 0 };
	__cpuid(cpuinfo, 0);
	SHORT hash = 0;
	const auto ptr = reinterpret_cast<SHORT*>(&cpuinfo[0]);
	for (auto i = 0; i < 8; i++)
		hash += ptr[i];

	return hash;
}

const wchar_t* ManchineID::get_machine_name()
{
	static wchar_t computerName[1024];
	DWORD size = 1024;
	GetComputerName(computerName, &size);
	return &computerName[0];
}
