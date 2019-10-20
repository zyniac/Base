#pragma once

#include <openssl/opensslv.h>
#include <openssl/ssl.h>

#include <openssl/bio.h>
#include <openssl/err.h>

#include <openssl/evp.h>
#include <openssl/x509.h>

#include <openssl/rsa.h>

#include <openssl/x509.h>
#include "openssl/pem.h"

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

namespace SSLUtil {
	void EasyInit() {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		SSL_library_init();
#else
		OPENSSL_init_ssl(0, 0);
#endif
	}

	EVP_PKEY* GenerateKey() {
		return EVP_PKEY_new();
	}

	void FreeKey(EVP_PKEY* key) {
		EVP_PKEY_free(key);
	}

	RSA* GenerateRSAKey(EVP_PKEY* key) { // NULL if failed
		RSA* rsa = RSA_generate_key( // Generating rsa
			2048,
			RSA_F4,
			0, // Callback
			0 // Argument for callback
		);
		if(rsa != nullptr)
			EVP_PKEY_assign_RSA(key, rsa); // Assign to key
		return rsa;
	}

	X509* GenerateValidx509(EVP_PKEY* key, const char* country, const char* company, const char* url) { // x509 is representor to x509 certificate (valid certificate for https)
		X509* x509;
		x509 = X509_new();
		ASN1_INTEGER_set(X509_get_serialNumber(x509), 1); // Number 1 is accepted in almost all browsers
		X509_gmtime_adj(X509_get_notBefore(x509), 0); // Set Time to current
		X509_gmtime_adj(X509_get_notAfter(x509), 31536000L); // 1 Year validation
		X509_set_pubkey(x509, key);
		X509_NAME* name;
		name = X509_get_subject_name(x509);
		X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)country, -1, -1, 0);
		X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)company, -1, -1, 0);
		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)url, -1, -1, 0);
		X509_set_issuer_name(x509, name);
		X509_sign(x509, key, EVP_sha1());
		return x509;
	}

	void Freex509(X509* x509) {
		X509_free(x509);
	}

	bool WritePrivateKey(EVP_PKEY* key, const char* path, const char* passphrase) {
		FILE* f;
		errno_t err = fopen_s(&f, path, "wb");
		if (err) return false;
		PEM_write_PrivateKey(
			f,
			key,
			EVP_des_ede3_cbc(),
			(unsigned char*)passphrase,
			strlen(passphrase),
			0,
			0
		);
		return true;
	}

	bool Writex509(X509* x509, const char* path) {
		FILE* f;
		errno_t err = fopen_s(&f, path, "wb");
		if (err) return false;
		PEM_write_X509(
			f,
			x509
		);
		return true;
	}

	bool GenerateCertificate(const char* password, const char* keypath, const char* x509path,
			const char* country, const char* company, const char* url) {
		EVP_PKEY* key = GenerateKey();
		bool success = GenerateRSAKey(key) != nullptr;
		if (!success) return false;
		X509* x509 = GenerateValidx509(key, country, company, url);
		success = WritePrivateKey(key, keypath, password);
		if (!success) return false;
		success = Writex509(x509, x509path);
		if (!success) return false; else return true;
	}
}