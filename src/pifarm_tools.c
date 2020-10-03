#include "pifarm_tools.h"

void rolling_buffer_create (rolling_buffer_t *rb, size_t bufsz)
{
    if (rb == NULL) return;
    if ((rb->buf = malloc (bufsz)) == NULL)
    {
        perror ("malloc");
        return;
    }
    rb->bufsize = bufsz;
    rb->head = rb->used = 0;
}

void rolling_buffer_delete (rolling_buffer_t *rb)
{
    free (rb->buf);
    rb->buf = NULL;
    rb->bufsize = 0;
    rb->head = rb->used = 0;
}

size_t rolling_buffer_pushback (rolling_buffer_t *rb, const void *data, size_t sz)
{
    size_t   freespace;
    size_t   written = 0;
    uint32_t first;

    if ((rb == NULL) || (data == NULL) || (sz == 0) || (rb->bufsize == 0)) return 0;

    /* bytes to write */
    freespace = rb->bufsize - rb->used;
    sz = (sz <= freespace) ? sz : freespace;
    if (sz == 0) return 0;

    first = (rb->head + rb->used)%rb->bufsize;
    if (first + sz <= rb->bufsize)
    {
        /* only one part to copy */
        memcpy (rb->buf + first, data, sz);
        written += sz;
        rb->used += sz;
    }
    else
    {
        /* first part */
        written = rolling_buffer_pushback (rb, data, rb->bufsize - rb->head - rb->used);

        /* secont part */
        written += rolling_buffer_pushback (rb, ((uint8_t*)data)+written, sz-written);
    }

    return written;
}

size_t rolling_buffer_read (rolling_buffer_t *rb, void *data, size_t sz)
{
    if ((rb == NULL) || (data == NULL) || (sz == 0) || (rb->bufsize == 0)) return 0;

    /* bytes to read */
    sz = (sz <= rb->used) ? sz : rb->used;

    if (rb->head + sz <= rb->bufsize)
    {
        /* only one part */
        memcpy (data, rb->buf + rb->head, sz);
        return sz;
    }
    else
    {
        /* two parts */
        memcpy (data, rb->buf + rb->head, rb->bufsize-rb->head);
        memcpy ((uint8_t*)data+(rb->bufsize-rb->head), rb->buf, sz-(rb->bufsize-rb->head));
        return sz;
    }
}

size_t rolling_buffer_popfront (rolling_buffer_t *rb, void *data, size_t sz)
{
    if ((rb == NULL) || (data == NULL) || (sz == 0) || (rb->bufsize == 0)) return 0;

    sz = rolling_buffer_read (rb, data, sz);
    rb->head = (rb->head + sz) % rb->bufsize;
    rb->used = rb->used - sz;

    return sz;
}

/*
void main(int argc, char **argv)
{
#define BUFFSZ 100
    rolling_buffer_t rb;
    float   data[BUFFSZ] = {} ;
    uint8_t iter;
    float   toto[BUFFSZ] = {} ;

    rolling_buffer_create   (&rb, BUFFSZ * sizeof(float));

    for (iter = 0; iter<BUFFSZ ; iter++)
    {
        data[iter] = 12.1 + (float)iter ;
    }

    for (iter = 0; iter<10 ; iter++)
    {
        rolling_buffer_pushback (&rb, &data[iter], sizeof(float));
    }

    rolling_buffer_read (&rb, &toto, BUFFSZ * sizeof(float));
    for (iter=0; iter<100; iter++)
    {
        printf ("%d - %f\n", iter, toto[iter]);
    }
}
*/
