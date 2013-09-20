/*
 *	unicomplex/proto/signature.c - Packet Signature field treatment
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#include <string.h>

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include <uc/packet.h>
#include <uc/signature.h>

/*
 * sign_packet() - Sign the given packet using an RSA 2048 bits private key
 * @packet : packet to sign
 * @privkey: private key used to sign
 *
 * This function fills the ->signature field of `packet' with the value
 * obtained when signing the SHA-1 message digest of the until-now prepared
 * packet using the author's RSA 2048 bits private key.
 *
 * OpenSSL apparently forces us to have 'privkey' in the pretty PEM format.
 *
 * This function returns non-zero on error.
 *
 * TODO: Proper error handling with a central error reporting mechanism.
 */
int sign_packet(struct uc_packet *packet, unsigned char *privkey)
{
	struct __uc_packet_head *head = &packet->head;
	BIO *bio;
	EVP_PKEY *key;
	EVP_MD_CTX ctx;
	unsigned int len = sizeof(head->signature);

	/* Make sure Signature and reserved are all zeroes */
	bzero(head->signature, sizeof(head->signature));
	head->reserved = 0;

	/* Damnit Beavis! I think openssl coaxed us to use their unwieldy
	 * interfaces to move some memory around. */
	bio = BIO_new_mem_buf(privkey, -1);
	if (!bio)
		return 1;
	key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
	if (!key) {
		BIO_free(bio);
		return 1;
	}
	if (!BIO_free(bio))
		goto error_free;

	/* Do a signature length check to be sure */
	if (EVP_PKEY_size(key) != sizeof(head->signature))
		goto error_free;

	EVP_SignInit(&ctx, EVP_sha1());
	if (EVP_SignUpdate(&ctx, head, sizeof(head) + head->length))
		goto error_free2;
	if (EVP_SignFinal(&ctx, head->signature, &len, key))
		goto error_free2;
	if (EVP_MD_CTX_cleanup(&ctx))
		goto error_free;
	EVP_PKEY_free(key);
	return 0;

error_free2:
	EVP_MD_CTX_cleanup(&ctx);
error_free:
	EVP_PKEY_free(key);
	return 1;
}

/*
 * verify_packet() - Verify the claimed authorship of the packet
 * @packet: packet to verify
 * @pubkey: public key of author
 *
 * This function returns non-zero on verification success, according to its
 * name.
 *
 * TODO: The function.
 */
int verify_packet(struct uc_packet *packet, unsigned char *pubkey)
{
	struct __uc_packet_head *head = &packet->head;
	unsigned char signature[256];

	/* The message digest was computed when the Signature field was all
	 * zeroes, let's replicate this situation. */
	memcpy(signature, head->signature, sizeof(signature));
	bzero(head->signature, sizeof(head->signature));

	return 0;
}
