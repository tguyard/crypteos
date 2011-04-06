#ifndef CRYPTO_HH
#define CRYPTO_HH

#include <memory>
#include <exception>
#include <stdexcept>
#include <string>
#include <cstring>

#include <gcrypt.h>

/*
 * Please note that for shared_ptr to be used to need to compile this with
 * -std=c++0x
 */

namespace crypto
{
	using std::shared_ptr;
	using std::string;

	template <typename T = void> class alloc_ptr;

	typedef unsigned char byte;
	typedef alloc_ptr<byte> bytes;

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

	bytes gen_random(size_t size, random_level level);

	/**
	 * Utility wrapper around shared_ptr to facilitate working with C APIs,
	 * C strings, and all objects created using alloc family.
	 *
	 * Calls free() when it the last instance is destroyed.
	 */
	template <typename T>
	class alloc_ptr : public shared_ptr<T>
	{
	public:
		typedef void (*release_fn)(T);
		typedef void *(*alloc_fn)(size_t);
		typedef void *(*alloc2_fn)(size_t, size_t);

		alloc_ptr()
		{}

		alloc_ptr(size_t len, alloc_fn alloc_func = malloc,
				release_fn release = NULL) throw()
			: shared_ptr<T>((T*) alloc_func(sizeof(T) * len), free),
			len(len), release_(release)
		{
			if (this->get() == NULL)
				throw std::bad_alloc();
		}

		alloc_ptr(size_t len, alloc2_fn alloc_func,
				release_fn release = NULL) throw()
			: shared_ptr<T>((T*) alloc_func(len, sizeof(T)), free),
			len(len), release_(release)
		{
			if (this->get() == NULL)
				throw std::bad_alloc();
		}

		explicit alloc_ptr(T* ptr, size_t len,
				release_fn release = NULL) throw()
			: shared_ptr<T>(ptr, free), len(len), release_(release)
		{
			if (this->get() == NULL)
				throw std::bad_alloc();
		}

		alloc_ptr(const string& value, alloc_fn alloc_func = malloc,
				release_fn release = NULL) throw()
			: shared_ptr<T>((T*) alloc_func(sizeof(T) * value.size()), free),
			len(value.size()), release_(release)
		{
			if (this->get() == NULL)
				throw std::bad_alloc();

			memcpy(this->get(), value.c_str(), len);
		}

		alloc_ptr(const string& value, alloc2_fn alloc_func,
				release_fn release = NULL) throw()
			: shared_ptr<T>((T*) alloc_func(value.size(), sizeof(T)), free),
			len(value.size()), release_(release)
		{
			if (this->get() == NULL)
				throw std::bad_alloc();

			memcpy(this->get(), value.c_str(), len);
		}

		void
		release()
		{
			if (this->get()) {
				if (release_ && *this->get())
					release_(*this->get());

				free(this->get());
			}
		}

		~alloc_ptr()
		{
			this->release();
		}

		size_t length() const throw() { return len; }
		size_t size() const throw() { return len; }

	private:
		size_t len;
		release_fn release_;
	};

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
