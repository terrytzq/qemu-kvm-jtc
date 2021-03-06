/*
 * Block driver for Hyper-V VHDX Images
 *
 * Copyright (c) 2013 Red Hat, Inc.,
 *
 * Authors:
 *  Jeff Cody <jcody@redhat.com>
 *
 *  This is based on the "VHDX Format Specification v1.00", published 8/25/2012
 *  by Microsoft:
 *      https://www.microsoft.com/en-us/download/details.aspx?id=34750
 *
 * This work is licensed under the terms of the GNU LGPL, version 2 or later.
 * See the COPYING.LIB file in the top-level directory.
 *
 */

#include "qemu-common.h"
#include "block/block_int.h"
#include "block/vhdx.h"

#include <uuid/uuid.h>


/*
 * All the VHDX formats on disk are little endian - the following
 * are helper import/export functions to correctly convert
 * endianness from disk read to native cpu format, and back again.
 */


/* VHDX File Header */


void vhdx_header_le_import(VHDXHeader *h)
{
    assert(h != NULL);

    le32_to_cpus(&h->signature);
    le32_to_cpus(&h->checksum);
    le64_to_cpus(&h->sequence_number);

    leguid_to_cpus(&h->file_write_guid);
    leguid_to_cpus(&h->data_write_guid);
    leguid_to_cpus(&h->log_guid);

    le16_to_cpus(&h->log_version);
    le16_to_cpus(&h->version);
    le32_to_cpus(&h->log_length);
    le64_to_cpus(&h->log_offset);
}

void vhdx_header_le_export(VHDXHeader *orig_h, VHDXHeader *new_h)
{
    assert(orig_h != NULL);
    assert(new_h != NULL);

    new_h->signature       = cpu_to_le32(orig_h->signature);
    new_h->checksum        = cpu_to_le32(orig_h->checksum);
    new_h->sequence_number = cpu_to_le64(orig_h->sequence_number);

    memcpy(&new_h->file_write_guid, &orig_h->file_write_guid, sizeof(MSGUID));
    memcpy(&new_h->data_write_guid, &orig_h->data_write_guid, sizeof(MSGUID));
    memcpy(&new_h->log_guid,        &orig_h->log_guid,        sizeof(MSGUID));

    cpu_to_leguids(&new_h->file_write_guid);
    cpu_to_leguids(&new_h->data_write_guid);
    cpu_to_leguids(&new_h->log_guid);

    new_h->log_version     = cpu_to_le16(orig_h->log_version);
    new_h->version         = cpu_to_le16(orig_h->version);
    new_h->log_length      = cpu_to_le32(orig_h->log_length);
    new_h->log_offset      = cpu_to_le64(orig_h->log_offset);
}


/* VHDX Log Headers */


void vhdx_log_desc_le_import(VHDXLogDescriptor *d)
{
    assert(d != NULL);

    le32_to_cpus(&d->signature);
    le32_to_cpus(&d->trailing_bytes);
    le64_to_cpus(&d->leading_bytes);
    le64_to_cpus(&d->file_offset);
    le64_to_cpus(&d->sequence_number);
}

void vhdx_log_desc_le_export(VHDXLogDescriptor *d)
{
    assert(d != NULL);

    cpu_to_le32s(&d->signature);
    cpu_to_le32s(&d->trailing_bytes);
    cpu_to_le64s(&d->leading_bytes);
    cpu_to_le64s(&d->file_offset);
    cpu_to_le64s(&d->sequence_number);
}

void vhdx_log_data_le_export(VHDXLogDataSector *d)
{
    assert(d != NULL);

    cpu_to_le32s(&d->data_signature);
    cpu_to_le32s(&d->sequence_high);
    cpu_to_le32s(&d->sequence_low);
}

void vhdx_log_entry_hdr_le_import(VHDXLogEntryHeader *hdr)
{
    assert(hdr != NULL);

    le32_to_cpus(&hdr->signature);
    le32_to_cpus(&hdr->checksum);
    le32_to_cpus(&hdr->entry_length);
    le32_to_cpus(&hdr->tail);
    le64_to_cpus(&hdr->sequence_number);
    le32_to_cpus(&hdr->descriptor_count);
    leguid_to_cpus(&hdr->log_guid);
    le64_to_cpus(&hdr->flushed_file_offset);
    le64_to_cpus(&hdr->last_file_offset);
}

void vhdx_log_entry_hdr_le_export(VHDXLogEntryHeader *hdr)
{
    assert(hdr != NULL);

    cpu_to_le32s(&hdr->signature);
    cpu_to_le32s(&hdr->checksum);
    cpu_to_le32s(&hdr->entry_length);
    cpu_to_le32s(&hdr->tail);
    cpu_to_le64s(&hdr->sequence_number);
    cpu_to_le32s(&hdr->descriptor_count);
    cpu_to_le64s(&hdr->flushed_file_offset);
    cpu_to_le64s(&hdr->last_file_offset);
    cpu_to_leguids(&hdr->log_guid);
}


