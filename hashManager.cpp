#include "hashManager.h"
#include "md5.h"
#include "sha1.h"
#include "sha224.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"

using namespace std;

const array<string, hashManager::funcNum> hashManager::sHashFuncNames = { "md5"s, "sha1"s, "sha224"s, "sha256"s, "sha384"s , "sha512"s };
const array<std::unique_ptr<abstractHash>, hashManager::funcNum> hashManager::sHash = { make_unique<MD5>(), make_unique<SHA1>(), make_unique<SHA224>(), make_unique<SHA256>(), make_unique<SHA384>(), make_unique<SHA512>() };
const array<int, hashManager::funcNum> hashManager::sHashLen = { 32, 40, 56, 64, 96, 128 };
string hashManager::getHash(int idx, istream& is) {
	sHash[idx]->update(is);
	return sHash[idx]->finalize();
}