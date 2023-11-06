# quake-export
Command line utilities for converting files into (and from) various Quake formats.

## quake-colormap-export

Generate colormap.lmp from palette.

## quake-mdl-export

Converts OBJ meshes into Quake MDL meshes. Multiple frames and skins can be specified by passing a JSON description instead of just an OBJ file. Skin groups and frame groups are also supported.

Example JSON file:

```json
{
	"mesh": "monster-idle.obj",
	"skins": [
		{
			"image": "monster-idle.png"
		}
	],
	"frames": [
		{
			"name": "walk1",
			"mesh": "monster-walk1.obj"
		},
		{
			"name": "walk2",
			"mesh": "monster-walk2.obj"
		},
		{
			"name": "walk3",
			"mesh": "monster-walk3.obj"
		}
	]
}
```

Make sure the vertex count and order is the same across OBJ files for all frames.

## quake-mdl-info

Display information about the contents of an MDL file.

## quake-miptex-export

Convert images into MIPTEX files. These usually are embedded into WADs or BSPs. Features:

- Optional dithering.
- Alpha transparency support. Alpha values below 50% are mapped to index 255 in the texture.
- Optional custom palette.

## quake-picture-export

Convert images to Quake picture lumps for menus etc.

## quake-picture-import

Convert Quake picture lumps to proper image files.

## quake-wad-export

Archive MIPTEX and other files into a single WAD file.

## quake-wad-info

Display information about the contents of a WAD file.
