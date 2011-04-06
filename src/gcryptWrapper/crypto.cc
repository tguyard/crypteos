#include <vector>
#include "crypto.hh"
#include <iostream>

namespace crypto
{

	using std::vector;
	using std::runtime_error;
	using std::invalid_argument;

    void check_err(gcry_error_t a, const char* (getError)(gcry_error_t)){
        if (a)
        {
            std::cerr << getError(a) << std::endl;
        }
    }

	string
	to_hex(const bytes& value)
	{
		string ret;

		for (byte *tmp = value.get(); tmp < value.get() + value.size(); ++tmp) {
			ret += (*tmp < 10 ? *tmp + '0' : *tmp - 10 + 'a'); 		} 		return ret; 	} 	bytes 	from_hex(const string& hex) 	{ 		string ret; 		for(string::const_iterator it = hex.begin(); it != hex.end(); ++it) { 			char x = tolower(*it); 			if (!isdigit(x) && (!isalpha(x) || x > 'f'))
				throw invalid_argument(string("All characters must be in [0-9a-f]; invalid: ") + x);

			ret += isdigit(x) ? x - '0' : x - 'a' + 10;
		}

		return bytes(ret);
	}

	bytes
	gen_random(size_t size, random_level level)
	{
		bytes buf(size);

		gcry_control(GCRYCTL_FAST_POLL, NULL);
		gcry_randomize(buf.get(), size, gcry_random_level(level));

		return buf;
	}

	blockcipher::blockcipher(blk_cipher cipher, blk_cipher_mode mode,
			const bytes& key, const bytes& initvec, bool secmem) :
		cipher_(cipher), mode_(mode), key_(key),
		initvec_(initvec), secmem_(secmem)
	{
		init_(cipher, mode, secmem);
	}

	blockcipher::blockcipher(blk_cipher cipher, blk_cipher_mode mode,
			bool secmem) :
		cipher_(cipher), mode_(mode), secmem_(secmem)
	{
		keylen_ = gcry_cipher_get_algo_keylen(cipher);
		blklen_ = gcry_cipher_get_algo_blklen(cipher);

		key_ = gen_random(keylen_, VERY_STRONG);
		initvec_ = gen_random(blklen_, VERY_STRONG);

		init_(cipher, mode, secmem);
	}

	void
	blockcipher::reload()
	{
		gcry_cipher_ctl(cipherhd_, GCRYCTL_RESET, NULL, 0);

		check_err(gcry_cipher_setiv(cipherhd_, initvec_.get(), blklen_), gcry_strerror);

		check_err(gcry_cipher_setkey(cipherhd_, key_.get(), keylen_), gcry_strerror);
	}

	void
	blockcipher::encrypt(const bytes& value)
	{
		bytes out(value.size());

		check_err(gcry_cipher_encrypt(cipherhd_, out.get(), out.size(),
				value.get(), value.size()), gcry_strerror);

		buf_ = out;
	}

	void
	blockcipher::encrypt(const string& value)
	{
		bytes out(value.size());

		check_err(gcry_cipher_encrypt(cipherhd_, out.get(), out.size(),
				value.c_str(), value.size()), gcry_strerror);

		buf_ = out;
	}

	void
	blockcipher::decrypt(const bytes& value)
	{
		reload();

		bytes out(value.size());

		check_err(gcry_cipher_decrypt(cipherhd_, out.get(), out.size(),
				value.get(), value.size()), gcry_strerror);

		buf_ = out;
	}

	void
	blockcipher::decrypt(const string& value)
	{
		reload();

		bytes out(value.size());

		check_err(gcry_cipher_decrypt(cipherhd_, out.get(), out.size(),
				value.c_str(), value.size()), gcry_strerror);

		buf_ = out;
	}

	void
	blockcipher::secretkey(const bytes& value)
	{
		key_ = value;
	}

	void
	blockcipher::secretkey(const string& value)
	{
		key_ = from_hex(value);
	}

	void
	blockcipher::iv(const bytes& value)
	{
		initvec_ = value;
	}

	void
	blockcipher::iv(const string& value)
	{
		initvec_ = from_hex(value);
	}

	string
	blockcipher::secretkey()
	{
		return to_hex(key_);
	}

	string
	blockcipher::iv()
	{
		return to_hex(initvec_);
	}

	const bytes&
	blockcipher::final() const
	{
		return buf_;
	}

	void
	blockcipher::init_(blk_cipher cipher, blk_cipher_mode mode, bool secmem)
	{
		keylen_ = gcry_cipher_get_algo_keylen(cipher);
		blklen_ = gcry_cipher_get_algo_blklen(cipher);

		if (blklen_ != initvec_.size())
			throw runtime_error("IV size must be: " );

		if (keylen_ != key_.size())
			throw runtime_error("Key size must be: " );

		if (secmem)
			gcry_control(GCRYCTL_INIT_SECMEM, SECMEM_SIZE, 0);

		check_err(gcry_cipher_open(&cipherhd_, cipher, mode,
				secmem ? GCRY_CIPHER_SECURE : 0), gcry_strerror);

		reload();
	}

	blockcipher::~blockcipher()
	{
		gcry_cipher_close(cipherhd_);

		if (secmem_)
			gcry_control(GCRYCTL_TERM_SECMEM, 0);
	}

} // namespace crypto
