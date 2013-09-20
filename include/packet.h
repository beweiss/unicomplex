/*
 *	unicomplex/include/packet.h
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#include <uc/compiler.h>

struct __uc_packet_head {
	uint32_t magic;
#define UC_PACKET_MAGIC	0x55432d50U /* "UC-P" */
	uint32_t type;
	uint32_t sender;
	unsigned char signature[256];
	uint32_t length;
	uint32_t reserved;
} __packed;

struct uc_packet {
	struct __uc_packet_head head;
	/*
	 * Having the ->body member as a flexible array member eases
	 * computing the message digest greatly and so increases
	 * performance. The `body' content is normally known when creating
	 * the packet (so memory re-allocation overhead is non-existent).
	 */
	unsigned char body[];
};

#endif /* PACKET_H */
