#!/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2022 Konrad Beckmann

import argparse
import os
import struct

def uncompressed_rom(rom_data, fps):
    rom_data_mv = memoryview(rom_data)

    # code = 'const unsigned char __attribute__((section(".n64_rom"))) frames_rgb[] = {\n'
    code = 'const unsigned char __in_flash("rom_file") frames_rgb[] = {\n'
    code += ','.join([hex(c) for c in rom_data])
    code += '\n};\n'
    code += f"const int frames_rgb_len = {len(rom_data)};\n"
    code += f"const int frames_rgb_fps = {fps};\n"
    return code


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Load file into frames.h')
    parser.add_argument('video', type=str)
    parser.add_argument('fps', type=str)
    parser.add_argument('outfile', type=str)

    args = parser.parse_args()

    print(args.video)
    print(args.fps)
    print(args.outfile)

    with open(args.video, 'rb') as r:
        blob_data = r.read()
    
        header = struct.pack("<II", len(blob_data), int(args.fps))
        # print(header.hex())

        with open(args.outfile, 'wb') as w:
            w.write(header)
            w.write(blob_data)
            print(f'Wrote blob to {args.outfile}')