
#pragma once

#include "../types.h"	// library types

/*
 * Simple mailbox:
 * - holding one item
 * - listener polls
 * - not thread-safe (only one poster and listener)
 *
 * Statically configured to empty.
 * Algebra:
 * reset; isMail() == false
 *
 * Note there is no init() to empty the mailbox.
 * After an exception, it might be necessary to flush the mailbox by reading it.
 */



class Mailbox {
public:
	// put overwrites a mail when overflows
	static void put(WorkPayload item);

	// fetch first mail in box (if queued)
	static WorkPayload fetch();

	//
	static bool isMail();
};
