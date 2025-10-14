# SMF0 Stream Reader

(C)2025 nasTTY

SMF0 Stream Reader is a library for reading Standard MIDI file (format 0) sequentially.

The library is written in C (the extension is cpp but I ended up avoiding to use any C++ features so I think it is C not C++), and works with MIDI file in any FILE structure.

It works with embedded C++ compilers, as well as compilers for PC. Use this with `fmemopen` and you can read and utilize SMF file from char arrays or constexprs for this library.

## It does support:
- SMF format 0
- Notes On / Off Messages
- CC Messages
- PC Messages
- running status
- meta events
  - end of track

## It does NOT support:
- SMF format 1/2
- any varying-length data, including text data[^1]
- SysEx (it might not work correctly)[^1]


[^1]: top 4 bytes are read, but any latter characters are ignored and if there was more than 5 byte or data the frag `has_extra_data` are set to 1.
