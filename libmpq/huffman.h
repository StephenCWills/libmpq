/*
 *  huffman.h -- structures used for huffman compression.
 *
 *  Copyright (c) 2003-2011 Maik Broemme <mbroemme@libmpq.org>
 *
 *  This source was adepted from the C++ version of huffman.h included
 *  in stormlib. The C++ version belongs to the following authors:
 *
 *  Ladislav Zezula <ladik@zezula.net>
 *  ShadowFlare <BlakFlare@hotmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _HUFFMAN_H
#define _HUFFMAN_H

/* define huffman compression and decompression values. */
#define LIBMPQ_HUFF_DECOMPRESS			0			/* we want to decompress using huffman trees. */
#define HUFF_ITEM_COUNT					0x203		/* number of items in the item pool. */
#define LINK_ITEM_COUNT					0x80		/* maximum number of quick-link items. */

/* define item handling. */
#define INSERT_AFTER				1
#define INSERT_BEFORE				2

/* input stream for huffman decompression. */
struct huffman_input_stream_s {
	uint8_t		*in_buf_end;			/* end position in the input buffer. */
	uint8_t		*in_buf;				/* current position in the input buffer. */
	uint16_t	bit_buf;				/* input bit buffer. */
	uint16_t	bits;					/* number of bits remaining current position. */
};

/* huffman tree item. */
struct huffman_tree_item_s {
	struct		huffman_tree_item_s *next;		/* pointer to lower-weight tree item. */
	struct		huffman_tree_item_s *prev;		/* pointer to higher-weight item. */
	uint32_t	dcmp_byte;						/* decompressed byte value (also index in the array). */
	uint32_t	weight;							/* weight. */
	struct		huffman_tree_item_s *parent;	/* pointer to parent item (NULL if none). */
	struct		huffman_tree_item_s *child;		/* pointer to the child with lower-weight child ("left child"). */
};

/* structure used for quick decompression. */
struct huffman_quick_link_s {
	uint32_t	valid_value;						/* if greater than huffman tree min_valid_value, the entry is valid. */
	uint32_t	valid_bits;							/* number of bits that are valid for this item link. */
	union {
		struct		huffman_tree_item_s *hi;		/* pointer to the item within the huffman tree. */
		uint32_t	dcmp_byte;						/* value for direct decompression. */
	};
};

/* structure for huffman tree. */
struct huffman_tree_s {
	struct		huffman_tree_item_s item_buffer[HUFF_ITEM_COUNT];	/* buffer for tree items. no memory allocation is needed. */
	uint32_t	items_used;											/* number of tree items used from item_buffer */
	struct		huffman_tree_item_s *first;							/* pointer to the highest weight item. */
	struct		huffman_tree_item_s *last;							/* pointer to the lowest weight item. */
	struct		huffman_tree_item_s *items_by_byte[0x102];			/* array of item pointers, one for each possible byte value. */
	struct		huffman_quick_link_s quick_links[LINK_ITEM_COUNT];	/* array of quick-link items. */
	uint32_t	min_valid_value;									/* a minimum value of quick link valid_value to be considered valid */
	int			isCmp0;												/* true if compression type 0. */
};

/* creates a new item for the huffman tree. */
struct huffman_tree_item_s *libmpq__create_new_item(
	struct huffman_tree_s *ht,
	uint32_t dcmp_value,
	uint32_t weight,
	uint32_t where
);

/* insert a new item into huffman tree. */
void libmpq__huffman_insert_item(
	struct		huffman_tree_s *ht,
	struct		huffman_tree_item_s *hi,
	uint32_t	where,
	struct		huffman_tree_item_s *insert_point
);

/* remove item from huffman tree. */
void libmpq__huffman_remove_item(
	struct		huffman_tree_item_s *hi
);

/* insert item2 after item1. */
void libmpq__link_two_items(
	struct		huffman_tree_item_s *item1,
	struct		huffman_tree_item_s *item2
);

/* get huffman tree item with higher weight. */
struct huffman_tree_item_s *libmpq__huffman_find_higher_or_equal_item(
	struct		huffman_tree_s *ht,
	struct		huffman_tree_item_s *hi,
	uint32_t	weight
);

/* get one bit from stream. */
uint32_t libmpq__huffman_get_1bit(
	struct		huffman_input_stream_s *is
);

/* get seven bit from stream. */
uint32_t libmpq__huffman_peek_7bit(
	struct		huffman_input_stream_s *is
);

/* get eight bit from stream. */
uint32_t libmpq__huffman_get_8bit(
	struct		huffman_input_stream_s *is
);

/* skip over bits in the input stream without using them. */
void libmpq__huffman_skip_bits(
	struct huffman_input_stream_s *is,
	uint32_t bits_to_skip
);

/* initialize the huffman tree. */
void libmpq__huffman_tree_init(
	struct		huffman_tree_s *ht,
	uint32_t	cmp
);

/* build the huffman tree. */
int libmpq__huffman_tree_build(
	struct		huffman_tree_s *ht,
	uint32_t	cmp_type
);

/* decompress the stream using huffman compression. */
int32_t libmpq__do_decompress_huffman(
	struct		huffman_tree_s *ht,
	struct		huffman_input_stream_s *is,
	uint8_t		*out_buf,
	uint32_t	out_length
);

/* maintain balance while inserting new item. */
int libmpq__insert_new_branch_and_rebalance(
	struct huffman_tree_s *ht,
	uint32_t value1,
	uint32_t value2
);

/* find place for new item and fix weights. */
uint32_t libmpq__fixup_item_pos_by_weight(
	struct huffman_tree_s *ht,
	struct huffman_tree_item_s *new_item,
	uint32_t max_weight
);

/* maintain balance while fixing weights of item ancestors. */
void libmpq__inc_weights_and_rebalance(
	struct huffman_tree_s *ht,
	struct huffman_tree_item_s *item
);

#endif						/* _HUFFMAN_H */
