#ifndef GENERATESSLCERT_HPP
#define GENERATESSLCERT_HPP

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <QString>
#include <memory>

// === RAII-deleter ===
struct EvpPkeyCtxDeleter
{
    void operator()(EVP_PKEY_CTX *ctx) const noexcept
    {
        if (ctx) {
            EVP_PKEY_CTX_free(ctx);
        }
    }
};
struct EvpPkeyDeleter
{
    void operator()(EVP_PKEY *pkey) const noexcept
    {
        if (pkey) {
            EVP_PKEY_free(pkey);
        }
    }
};
struct X509Deleter
{
    void operator()(X509 *x) const noexcept
    {
        if (x) {
            X509_free(x);
        }
    }
};
struct BioDeleter
{
    void operator()(BIO *bio) const noexcept
    {
        if (bio) {
            BIO_free(bio);
        }
    }
};

using UniqueEvpPkeyCtx = std::unique_ptr<EVP_PKEY_CTX, EvpPkeyCtxDeleter>;
using UniqueEvpPkey = std::unique_ptr<EVP_PKEY, EvpPkeyDeleter>;
using UniqueX509 = std::unique_ptr<X509, X509Deleter>;
using UniqueBio = std::unique_ptr<BIO, BioDeleter>;

QString bioToString(BIO *bio);

struct GeneratedCert
{
    QString certificatePem;
    QString privateKeyPem;
};

bool generateSelfSignedEcdsaCertificate();

bool saveCertificateToFile(const QString &certificate, const QString &privateKey);

QByteArray getCertificateFingerprint();

#endif // GENERATESSLCERT_HPP
