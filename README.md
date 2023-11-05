# quake-export
Command line utilities for converting files into various Quake formats.

## quake-colormap-export

Generate colormap.lmp from palette.

## mdl-export

Converts OBJ meshes into Quake MDL meshes. Multiple frames and skins can be specified by passing a JSON description instead of just an OBJ file.

Skin groups and frame groups are also supported.

## miptex-export

Convert images into MIPTEX files. These usually are embedded into WADs or BSPs. Features:

- Optional dithering.
- Alpha transparency support. Alpha values below 50% are mapped to index 255 in the texture.
- Optional custom palette.

## quake-picture-export

Convert images to Quake picture lumps for menus etc.

## quake-picture-import

Convert Quake picture lumps to proper image files.

## wad-export

Archive MIPTEX files into a single WAD file.
