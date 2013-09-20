/*
 *	unicomplex/include/signature.h
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <uc/packet.h>

int sign_packet(struct uc_packet *packet, unsigned char *privkey);
int verify_packet(struct uc_packet *packet, unsigned char *pubkey);

#endif /* SIGNATURE_H */
