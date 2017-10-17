/* libsmp
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */
/**
 * @file
 */

#ifndef LIBSMP_H
#define LIBSMP_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Message */

/** The maximum numbers of arguments in a message */
#define SMP_MESSAGE_MAX_VALUES 16

/**
 * \ingroup message
 * Type of an argument
 */
typedef enum
{
    SMP_TYPE_NONE = 0x00,
    SMP_TYPE_UINT8 = 0x01,
    SMP_TYPE_INT8 = 0x02,
    SMP_TYPE_UINT16 = 0x03,
    SMP_TYPE_INT16 = 0x04,
    SMP_TYPE_UINT32 = 0x05,
    SMP_TYPE_INT32 = 0x06,
    SMP_TYPE_UINT64 = 0x07,
    SMP_TYPE_INT64 = 0x08,
    SMP_TYPE_STRING = 0x09,

    SMP_TYPE_MAX = 0x7f
} SmpType;

/**
 * \ingroup message
 * Structure describing an argument in the message
 */
typedef struct
{
    SmpType type;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;

        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;

        const char *cstring;
    } value;
} SmpValue;

/**
 * \ingroup message
 * Message structure
 */
typedef struct
{
    /** The message id */
    uint32_t msgid;

    SmpValue values[SMP_MESSAGE_MAX_VALUES];
} SmpMessage;

void smp_message_init(SmpMessage *msg, uint32_t msgid);
int smp_message_init_from_buffer(SmpMessage *msg, const uint8_t *buffer,
        size_t size);
void smp_message_clear(SmpMessage *msg);

ssize_t smp_message_encode(SmpMessage *msg, uint8_t *buffer, size_t size);

uint32_t smp_message_get_msgid(SmpMessage *msg);
int smp_message_n_args(SmpMessage *msg);

int smp_message_get(SmpMessage *msg, int index, ...);
int smp_message_get_value(SmpMessage *msg, int index, SmpValue *value);
int smp_message_get_uint8(SmpMessage *msg, int index, uint8_t *value);
int smp_message_get_int8(SmpMessage *msg, int index, int8_t *value);
int smp_message_get_uint16(SmpMessage *msg, int index, uint16_t *value);
int smp_message_get_int16(SmpMessage *msg, int index, int16_t *value);
int smp_message_get_uint32(SmpMessage *msg, int index, uint32_t *value);
int smp_message_get_int32(SmpMessage *msg, int index, int32_t *value);
int smp_message_get_uint64(SmpMessage *msg, int index, uint64_t *value);
int smp_message_get_int64(SmpMessage *msg, int index, int64_t *value);
int smp_message_get_cstring(SmpMessage *msg, int index, const char **value);

int smp_message_set(SmpMessage *msg, int index, ...);
int smp_message_set_value(SmpMessage *msg, int index, const SmpValue *value);
int smp_message_set_uint8(SmpMessage *msg, int index, uint8_t value);
int smp_message_set_int8(SmpMessage *msg, int index, int8_t value);
int smp_message_set_uint16(SmpMessage *msg, int index, uint16_t value);
int smp_message_set_int16(SmpMessage *msg, int index, int16_t value);
int smp_message_set_uint32(SmpMessage *msg, int index, uint32_t value);
int smp_message_set_int32(SmpMessage *msg, int index, int32_t value);
int smp_message_set_uint64(SmpMessage *msg, int index, uint64_t value);
int smp_message_set_int64(SmpMessage *msg, int index, int64_t value);
int smp_message_set_cstring(SmpMessage *msg, int index, const char *value);

/* Serial frame API */

/**
 * \ingroup serial_frame
 * The maximum frame size in bytes.
 */
#define SMP_SERIAL_FRAME_MAX_FRAME_SIZE 1024

/**
 * \ingroup serial_frame
 * Error thrown during decoding
 */
typedef enum
{
    /** No error */
    SMP_SERIAL_FRAME_ERROR_NONE = 0,
    /** Payload is corrupted (bad crc, missing end byte...) */
    SMP_SERIAL_FRAME_ERROR_CORRUPTED = -1,
    /** Payload too big to fit in buffer */
    SMP_SERIAL_FRAME_ERROR_PAYLOAD_TOO_BIG = -2,
} SmpSerialFrameError;

typedef enum
{
    SMP_SERIAL_FRAME_DECODER_STATE_WAIT_HEADER,
    SMP_SERIAL_FRAME_DECODER_STATE_IN_FRAME,
    SMP_SERIAL_FRAME_DECODER_STATE_IN_FRAME_ESC,
} SmpSerialFrameDecoderState;

/**
 * \ingroup serial_frame
 * Callback structure passed to decoder
 */
typedef struct
{
    /** called when a new valid frame is available */
    void (*new_frame)(uint8_t *frame, size_t size, void *userdata);

    /** called when an error occurs while decoding incoming data */
    void (*error)(SmpSerialFrameError error, void *userdata);
} SmpSerialFrameDecoderCallbacks;

typedef struct
{
    SmpSerialFrameDecoderState state;
    SmpSerialFrameDecoderCallbacks cbs;

    uint8_t frame[SMP_SERIAL_FRAME_MAX_FRAME_SIZE];
    size_t frame_offset;

    void *userdata;
} SmpSerialFrameDecoder;

typedef struct
{
    SmpSerialFrameDecoder decoder;

    int serial_fd;
} SmpSerialFrameContext;

int smp_serial_frame_init(SmpSerialFrameContext *ctx, const char *device,
        const SmpSerialFrameDecoderCallbacks *cbs, void *userdata);
void smp_serial_frame_deinit(SmpSerialFrameContext *ctx);

int smp_serial_frame_get_fd(SmpSerialFrameContext *ctx);

int smp_serial_frame_send(SmpSerialFrameContext *ctx, const uint8_t *buf,
        size_t size);
int smp_serial_frame_process_recv_fd(SmpSerialFrameContext *ctx);

#ifdef __cplusplus
}
#endif

#endif
