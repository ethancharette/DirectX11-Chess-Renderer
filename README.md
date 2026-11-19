# DirectX 11 Chess Renderer

A real-time 3D chess scene rendered with **C++ and Direct3D 11**: a lit, textured marble board and a full set of pieces inside a cube-mapped skybox, viewed through an orbiting camera.

Every piece is built **procedurally from primitive meshes** (cylinders, spheres, tori, cones and cubes) arranged with hierarchical transforms. No imported models.

<!-- Add a screenshot or GIF here, e.g.:
![Chess renderer](docs/screenshot.png)
-->

## Features

- **Custom HLSL shaders** (vertex + pixel), compiled at build time
- **Blinn-Phong lighting** with ambient light plus **three coloured directional lights**, each with its own diffuse and specular contribution
- **Diffuse + specular texture maps** (marble) sampled per pixel
- **Per-object materials** through a dedicated constant buffer (ambient / diffuse / specular + specular power)
- **Cube-map skybox** drawn with front-face culling and a read-only depth state so it always sits behind the scene
- **Procedural geometry**: indexed vertex/index buffers generated for cubes, spheres, cylinders, cones and tori
- **Hierarchical modelling**: each chess piece is several primitives combined with local transforms, then placed on the board with a parent matrix
- **Orbit camera** using spherical coordinates, with clamped pitch and zoom

## Controls

| Input | Action |
|---|---|
| Click **Start** | Enter the scene |
| ← / → | Orbit the camera around the board |
| ↑ / ↓ | Tilt the camera up / down |
| Numpad **+** / **−** | Zoom in / out |
| **Space** | Reset the camera |
| **0–4** | Change the vsync / present interval |

## Building

**Requirements:** Windows 10/11 and Visual Studio 2022 with the **Desktop development with C++** workload.

1. Clone the repo and open `ChessRenderer.sln`.
2. Visual Studio restores the [DirectX Tool Kit](https://github.com/microsoft/DirectXTK) NuGet package (`directxtk_desktop_win10`) automatically. If it doesn't, right-click the solution and choose **Restore NuGet Packages**.
3. Set the configuration to **Debug | x86** (or Release | x86) and press **F5**.

The app loads its assets relative to the output folder (`../Textures`, `../Font`), and the compiled shaders (`.cso`) are written next to the executable. The debugger's working directory is already set up for this.

## Project layout

```
ChessRenderer/
  ChessScene.*          Scene setup, camera, input, render loop
  Board.*, Pawn.*, ...  Procedurally modelled chess pieces
  LitColourShader.*     Shader wrapper: constant buffers, lights, sampler
  SkyBox.*              Cube-map skybox
  IndexedPrimitive.*    Vertex/index buffer wrapper
  Models.*              Primitive mesh generators
  Shaders/              HLSL source
  DirectX.*, Sprite.*, Font.*, TextureType.*, Timer.*   Window/D3D framework
Textures/               Marble diffuse/specular maps, skybox cube map, UI panel
Font/                   SpriteFont used for UI text
```

## Credits

- The window / Direct3D setup, sprite, font, texture and timer helpers come from a small starter framework provided in a graphics programming course. The primitive mesh generators in `Models.cpp` are adapted from the [DirectX Tool Kit](https://github.com/microsoft/DirectXTK) (MIT).
- Built with the [DirectX Tool Kit](https://github.com/microsoft/DirectXTK) (MIT).
