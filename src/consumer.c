// SPDX-License-Identifier: BSD-3-Clause

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include "consumer.h"
#include "ring_buffer.h"
#include "packet.h"
#include "utils.h"

void consumer_thread(so_consumer_ctx_t *ctx)
{
	/* TODO: implement consumer thread */
	while (true) {
		so_packet_t pack;
		int rez = ring_buffer_dequeue(ctx->producer_rb, &pack, PKT_SZ);

		if (rez < 0)
			break;
		int act = process_packet(&pack);

		unsigned long h = packet_hash(&pack);

		pthread_mutex_lock(&ctx->log_mutex);
		fprintf(ctx->f_out, "%s %016lx %lu\n", RES_TO_STR(act), h, pack.hdr.timestamp);
		pthread_mutex_unlock(&ctx->log_mutex);
		}
}


int create_consumers(pthread_t *tids,
					 int num_consumers,
					 struct so_ring_buffer_t *rb,
					 const char *out_filename)
{
	FILE *f_out = fopen(out_filename, "a+");

	pthread_mutex_t log_mutex;

	pthread_mutex_init(&log_mutex, NULL);
	for (int i = 0; i < num_consumers; i++) {
		/*
		 * TODO: Launch consumer threads
		 **/
		so_consumer_ctx_t *ctx = malloc(sizeof(so_consumer_ctx_t));

		ctx->producer_rb = rb;
		ctx->log_mutex = log_mutex;
		ctx->f_out = f_out;
		pthread_create(&tids[i], NULL, (void *)consumer_thread, ctx);
		}
	return num_consumers;
}
