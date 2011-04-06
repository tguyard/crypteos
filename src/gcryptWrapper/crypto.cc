#include <vector>
#include "crypto.hh"
#include <iostream>

namespace crypto {

using std::vector;
using std::runtime_error;
using std::invalid_argument;

void check_err(gcry_error_t a, const char* ( getError)(gcry_error_t)) {
	if (a) {
		std::cerr << getError(a) << std::endl;
	}
}

string to_hex(const bytes& value) {
	string ret;
	for (bytes::const_iterator it = value.begin(); it != value.end(); ++it) {
		ret += (*it < 10 ? *it + '0' : *it - 10 + 'a');
	}
	return ret;
}

const bytes from_hex(const string& hex) {
	bytes ret;
	for (string::const_iterator it = hex.begin(); it != hex.end(); ++it) {
		char x = tolower(*it);
		if (!isdigit(x) && (!isalpha(x) || x > 'f'))
			throw invalid_argument(
					string("All characters must be in [0-9a-f]; invalid: ") + x);

		ret.push_back(isdigit(x) ? x - '0' : x - 'a' + 10);
	}
	return ret;
}

bytes gen_random(size_t size, random_level level) {
	bytes buf;
	buf.resize(size, '\0');

	gcry_control(GCRYCTL_FAST_POLL, NULL);
	gcry_randomize(buf.data(), size, gcry_random_level(level));

	return buf;
}

blockcipher::blockcipher(blk_cipher cipher, blk_cipher_mode mode,
		const bytes& key, const bytes& initvec, bool secmem) :
	cipher_(cipher), mode_(mode), key_(key), initvec_(initvec), secmem_(secmem) {
	init_(cipher, mode, secmem);
}

blockcipher::blockcipher(blk_cipher cipher, blk_cipher_mode mode, bool secmem) :
	cipher_(cipher), mode_(mode), secmem_(secmem) {
	keylen_ = gcry_cipher_get_algo_keylen(cipher);
	blklen_ = gcry_cipher_get_algo_blklen(cipher);

	key_ = gen_random(keylen_, WEAK);
	initvec_ = gen_random(blklen_, WEAK);

	init_(cipher, mode, secmem);
}

void blockcipher::reload() {
	gcry_cipher_ctl(cipherhd_, GCRYCTL_RESET, NULL, 0);

	check_err(gcry_cipher_setiv(cipherhd_, initvec_.data(), blklen_),
			gcry_strerror);

	check_err(gcry_cipher_setkey(cipherhd_, key_.data(), keylen_),
			gcry_strerror);
}

void blockcipher::encrypt(const bytes& value) {
	byte* out = new byte[value.size()];

	check_err(
			gcry_cipher_encrypt(cipherhd_, out, value.size(), value.data(),
					value.size()), gcry_strerror);

	buf_.assign(out, out + value.size());
}

void blockcipher::encrypt(const string& value) {
	byte* out = new byte[value.size()+10];

	check_err(
			gcry_cipher_encrypt(cipherhd_, out, value.size()+10, value.c_str(),
					value.size()), gcry_strerror);

	std::copy(out, out + value.size(), std::back_inserter(buf_));
}

void blockcipher::decrypt(const bytes& value) {
	reload();

	byte* out = new byte[value.size()];

	check_err(
			gcry_cipher_decrypt(cipherhd_, out, value.size(), value.data(),
					value.size()), gcry_strerror);

	buf_.assign(out, out + value.size());
}

void blockcipher::decrypt(const string& value) {
	reload();

	byte* out = new byte[value.size()];

	check_err(
			gcry_cipher_decrypt(cipherhd_, out, value.size(), value.c_str(),
					value.size()), gcry_strerror);

	buf_.assign(out, out + value.size());
}

void blockcipher::secretkey(const bytes& value) {
	key_ = value;
}

void blockcipher::secretkey(const string& value) {
	key_ = from_hex(value);
}

void blockcipher::iv(const bytes& value) {
	initvec_ = value;
}

void blockcipher::iv(const string& value) {
	initvec_ = from_hex(value);
}

string blockcipher::secretkey() {
	return to_hex(key_);
}

string blockcipher::iv() {
	return to_hex(initvec_);
}

const bytes&
blockcipher::final() const {
	return buf_;
}

void blockcipher::init_(blk_cipher cipher, blk_cipher_mode mode, bool secmem) {
	keylen_ = gcry_cipher_get_algo_keylen(cipher);
	blklen_ = gcry_cipher_get_algo_blklen(cipher);

	if (blklen_ != initvec_.size())
		throw runtime_error("IV size must be: ");

	if (keylen_ != key_.size())
		throw runtime_error("Key size must be: ");

	if (secmem)
		gcry_control(GCRYCTL_INIT_SECMEM, SECMEM_SIZE, 0);

	check_err(
			gcry_cipher_open(&cipherhd_, cipher, mode,
					secmem ? GCRY_CIPHER_SECURE : 0), gcry_strerror);

	reload();
}

blockcipher::~blockcipher() {
	gcry_cipher_close(cipherhd_);

	if (secmem_)
		gcry_control(GCRYCTL_TERM_SECMEM, 0);
}

} // namespace crypto
