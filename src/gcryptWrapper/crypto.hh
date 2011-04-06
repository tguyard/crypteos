#ifndef CRYPTO_HH
#define CRYPTO_HH

#include <memory>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

#include <gcrypt.h>

/*
 * Please note that for shared_ptr to be used to need to compile this with
 * -std=c++0x
 */

namespace crypto
{
	using std::shared_ptr;
	using std::string;

	typedef unsigned char byte;
	typedef std::vector<byte> bytes;

	static const int SECMEM_SIZE = 16384;

	enum blk_cipher {
		CIPHER_AES128 = GCRY_CIPHER_AES,
		CIPHER_AES192 = GCRY_CIPHER_AES192,
		CIPHER_AES256 = GCRY_CIPHER_AES256,
		CIPHER_SERPENT128 = GCRY_CIPHER_SERPENT128,
		CIPHER_SERPENT192 = GCRY_CIPHER_SERPENT192,
		CIPHER_SERPENT256 = GCRY_CIPHER_SERPENT256
	};

	enum blk_cipher_mode {
		CIPHER_MODE_CBC = GCRY_CIPHER_MODE_CBC,
		CIPHER_MODE_ECB = GCRY_CIPHER_MODE_ECB,
		CIPHER_MODE_CTR = GCRY_CIPHER_MODE_CTR
	};

	enum random_level {
		WEAK = GCRY_WEAK_RANDOM,
		STRONG = GCRY_STRONG_RANDOM,
		VERY_STRONG = GCRY_VERY_STRONG_RANDOM
	};

	void gen_random(bytes& array, random_level level);

	class blockcipher {
	public:
		blockcipher(blk_cipher cipher, blk_cipher_mode mode,
				const bytes& key, const bytes& initvec,
				bool secmem = true);
		blockcipher(blk_cipher cipher, blk_cipher_mode mode,
				bool secmem = true);
		~blockcipher();

		void reload();

		void encrypt(const bytes& value);
		void encrypt(const string& value);

		void decrypt(const bytes& value);
		void decrypt(const string& value);

		void secretkey(const bytes& value);
		void secretkey(const string& value);

		void iv(const bytes& value);
		void iv(const string& value);

		string secretkey();
		string iv();

		const bytes& final() const;

	private:
		void init_(blk_cipher cipher, blk_cipher_mode mode,
				bool secmem = true);

		gcry_cipher_hd_t cipherhd_;
		blk_cipher cipher_;
		blk_cipher_mode mode_;
		uint keylen_;
		uint blklen_;
		bytes key_;
		bytes initvec_;
		bytes buf_;
		bool secmem_;
	};

} // namespace crypto

#endif // CRYPTO_HH
