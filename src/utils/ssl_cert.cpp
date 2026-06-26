#include "ssl_cert.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QSslCertificate>
#include <QStandardPaths>
#include <stdexcept>

QString bioToString(BIO *bio)
{
    char *data = nullptr;
    int len = BIO_get_mem_data(bio, &data);
    if (len <= 0 || !data)
        return {};
    return {data};
}

bool generateSelfSignedEcdsaCertificate()
{
    // 1. Generate key on P-256 curve
    UniqueEvpPkeyCtx pctx(EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr));
    if (!pctx)
        throw std::runtime_error("EVP_PKEY_CTX_new_id failed");

    if (EVP_PKEY_keygen_init(pctx.get()) <= 0)
        throw std::runtime_error("EVP_PKEY_keygen_init failed");

    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx.get(), NID_X9_62_prime256v1) <= 0)
        throw std::runtime_error("EVP_PKEY_CTX_set_ec_paramgen_curve_nid failed");

    EVP_PKEY *rawPkey = nullptr;
    if (EVP_PKEY_keygen(pctx.get(), &rawPkey) <= 0)
        throw std::runtime_error("EVP_PKEY_keygen failed");
    UniqueEvpPkey pkey(rawPkey);
    pctx.reset(); // context no longer needed

    // 2. Create self-signed X.509 certificate
    UniqueX509 x509(X509_new());
    if (!x509)
        throw std::runtime_error("X509_new failed");

    // Serial number: 8 random bytes
    unsigned char serial[8];
    if (RAND_bytes(serial, sizeof(serial)) != 1)
        throw std::runtime_error("RAND_bytes failed");
    if (!ASN1_INTEGER_set_uint64(X509_get_serialNumber(x509.get()),
                                 *reinterpret_cast<uint64_t *>(serial)))
        throw std::runtime_error("ASN1_INTEGER_set_uint64 failed");

    // Validity: now + 10 years
    X509_gmtime_adj(X509_getm_notBefore(x509.get()), 0);
    X509_gmtime_adj(X509_getm_notAfter(x509.get()), 60 * 60 * 24 * 365 * 10);

    // Attach public key
    if (!X509_set_pubkey(x509.get(), pkey.get()))
        throw std::runtime_error("X509_set_pubkey failed");

    // Subject and Issuer: CN only
    X509_NAME *name = X509_get_subject_name(x509.get());
    if (!X509_NAME_add_entry_by_txt(name,
                                    "CN",
                                    MBSTRING_ASC,
                                    reinterpret_cast<const unsigned char *>("Glowworm"),
                                    -1,
                                    -1,
                                    0))
        throw std::runtime_error("X509_NAME_add_entry_by_txt failed");
    if (!X509_set_issuer_name(x509.get(), name))
        throw std::runtime_error("X509_set_issuer_name failed");

    // SHA-256 signature
    if (!X509_sign(x509.get(), pkey.get(), EVP_sha256()))
        throw std::runtime_error("X509_sign failed");

    // 3. Serialize to PEM
    UniqueBio certBio(BIO_new(BIO_s_mem()));
    UniqueBio keyBio(BIO_new(BIO_s_mem()));
    if (!certBio || !keyBio)
        throw std::runtime_error("BIO_new failed");

    if (!PEM_write_bio_X509(certBio.get(), x509.get()))
        throw std::runtime_error("PEM_write_bio_X509 failed");
    if (!PEM_write_bio_PrivateKey(keyBio.get(), pkey.get(), nullptr, nullptr, 0, nullptr, nullptr))
        throw std::runtime_error("PEM_write_bio_PrivateKey failed");

    return saveCertificateToFile(bioToString(certBio.get()), bioToString(keyBio.get()));
}

bool saveCertificateToFile(const QString &certificate, const QString &privateKey)
{
    const QString serverCertPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                   + "/appdata/server/server_cert.crt";
    const QString serverKeyPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/appdata/server/server_key.key";

    QFile fileCert(serverCertPath);
    QFile fileKey(serverKeyPath);
    qDebug() << serverCertPath;
    qDebug() << serverKeyPath;

    fileCert.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    fileKey.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

    if (!fileCert.open(QIODevice::WriteOnly)) {
        qDebug() << "File error:" << fileCert.errorString();
        return false;
    } else {
        if (fileCert.write(certificate.toUtf8()) == -1) {
            return false;
        }
    }

    if (!fileKey.open(QIODevice::WriteOnly)) {
        qDebug() << "File error:" << fileKey.errorString();
        return false;
    } else {
        if (fileKey.write(privateKey.toUtf8()) == -1) {
            return false;
        }
    }

    return true;
}

QByteArray getCertificateFingerprint()
{
    const QString ServerCertPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                   + "/appdata/server/server_cert.crt";
    QFile fileCert(ServerCertPath);

    if (!fileCert.open(QIODevice::ReadOnly)) {
        qDebug() << "File error:" << fileCert.error();
        return {};
    }

    QSslCertificate sslCertificate(fileCert.readAll(), QSsl::Pem);

    return sslCertificate.digest(QCryptographicHash::Sha256);
}
