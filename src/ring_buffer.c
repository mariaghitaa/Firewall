// SPDX-License-Identifier: BSD-3-Clause

#include "ring_buffer.h"

#include <stdlib.h>
#include <pthread.h>

int ring_buffer_init(so_ring_buffer_t *ring, size_t cap)
{
	/* TODO: implement ring_buffer_init */
	ring->data = malloc(cap);
	if (ring->data == NULL)
		return -1;
	ring->cap = cap;
	ring->len = 0;
	ring->read_pos = 0;
	ring->stop = 0;
	ring->write_pos = 0;
	pthread_mutex_init(&ring->mutex, NULL);
	pthread_cond_init(&ring->not_empty, NULL);
	pthread_cond_init(&ring->not_full, NULL);
	return 0;
}

ssize_t ring_buffer_enqueue(so_ring_buffer_t *ring, void *data, size_t size)
{
	/* TODO: implement ring_buffer_enqueue */
	pthread_mutex_lock(&ring->mutex);
	while (ring->len == ring->cap && ring->stop == 0)
		pthread_cond_wait(&ring->not_full, &ring->mutex);
	if (ring->stop == 1) {
		pthread_mutex_unlock(&ring->mutex);
		return -1;
	}
	for (size_t i = 0; i < size; i++)
		ring->data[ring->write_pos + i] = ((char *)data)[i];
	ring->write_pos = (ring->write_pos + size) % ring->cap;
	ring->len = ring->len + size;
	pthread_cond_signal(&ring->not_empty);
	pthread_mutex_unlock(&ring->mutex);
	return size;
}

ssize_t ring_buffer_dequeue(so_ring_buffer_t *ring, void *data, size_t size)
{
	/* TODO: Implement ring_buffer_dequeue */
	pthread_mutex_lock(&ring->mutex);
	while (ring->len == 0 && ring->stop == 0)
		pthread_cond_wait(&ring->not_empty, &ring->mutex);
	if (ring->stop == 1 && ring->len == 0) {
		pthread_mutex_unlock(&ring->mutex);
		return -1;
	}
	for (size_t i = 0; i < size; i++)
		((char *)data)[i] = ring->data[ring->read_pos + i];
	ring->read_pos = (ring->read_pos + size) % ring->cap;
	ring->len = ring->len - size;
	pthread_cond_signal(&ring->not_full);
	pthread_mutex_unlock(&ring->mutex);
	return size;
}

void ring_buffer_destroy(so_ring_buffer_t *ring)
{
	/* TODO: Implement ring_buffer_destroy */
	free(ring->data);
	pthread_mutex_destroy(&ring->mutex);
	pthread_cond_destroy(&ring->not_empty);
	pthread_cond_destroy(&ring->not_full);
}

void ring_buffer_stop(so_ring_buffer_t *ring)
{
	/* TODO: Implement ring_buffer_stop */
	pthread_mutex_lock(&ring->mutex);
	ring->stop = 1;
	pthread_cond_broadcast(&ring->not_empty);
	pthread_cond_broadcast(&ring->not_full);
	pthread_mutex_unlock(&ring->mutex);
}
