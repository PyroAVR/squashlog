#!/bin/sh

# Extract binfmt, strings, and indices from an ELF file

BINFMT_SEC_NAME=.binfmt_data
IDXS_SEC_NAME=.twig.stridxs
STRS_SEC_NAME=.twig.strtab

OBJCOPY=objcopy

function extract_strings() {
    ifname=$1
    ofname=$2
    $OBJCOPY --only-section $STRS_SEC_NAME -O binary $ifname ${ofname}.strings.bin
}

function extract_idxs() {
    ifname=$1
    ofname=$2
    $OBJCOPY --only-section $IDXS_SEC_NAME -O binary $ifname ${ofname}.idxs.bin
}

function extract_binfmt() {
    ifname=$1
    ofname=$2
    $OBJCOPY --only-section $BINFMT_SEC_NAME -O binary $ifname ${ofname}.binfmt.bin
}

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 executable"
    exit 1
fi

outname=$(basename $1)

extract_strings $1 $outname
extract_idxs $1 $outname
extract_binfmt $1 $outname
