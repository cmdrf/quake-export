# quake-export
Command line utilities for converting files into various Quake formats.

## mdl-export

Converts OBJ meshes into Quake MDL meshes. Animation not supported yet.

## miptex-export

Convert images into MIPTEX files. These usually are embedded into WADs or BSPs. Features:

- Optional dithering.
- Alpha transparency support. Alpha values below 50% are mapped to index 255 in the texture.

## wad-export

Archive MIPTEX files into a single WAD file.
