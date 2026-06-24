# Archive — IRIS Explorer Adaptation

> **⚠️ This is an archive.** The platform this code was written for — **NAG IRIS Explorer** —
> no longer exists. It is preserved here for reference and historical interest only. None of it
> is expected to build or run on a modern machine without the original, now-unavailable toolchains
> and runtime libraries (IRIS Explorer, VHTK, H3DAPI, Open Inventor/Coin3D).

## What was IRIS Explorer?

IRIS Explorer was a visual-programming application for scientific visualisation. Users built
"maps" by wiring together **modules** (data readers, filters, renderers, interactors) in a visual
data-flow editor. It was originally created by **Silicon Graphics (SGI)** and later developed and
sold by the **Numerical Algorithms Group (NAG)**. NAG discontinued IRIS Explorer and ended
support, so the product is no longer available for purchase or download — which is why this
repository is an archive rather than a live project.

## What this project is

This repository collects a body of work around **3D flow visualisation and haptic (force-feedback)
interaction** with fluid-flow datasets. It spans two technology eras:

1. **IRIS Explorer modules** — custom visualisation modules and the network "maps" that wire them
   together, plus a C++/C# SDK that was being built to author IRIS Explorer modules more easily.
2. **Haptic flow visualisation** — a later, standalone effort built on **H3DAPI / X3D** and the
   **Volume Haptics Toolkit (VHTK)** that lets a user *feel* a vector flow field through a haptic
   device.

There are also some small **Open Inventor / Coin3D** learning examples.

## Repository layout

```
.
├── haptic-flow/              H3DAPI / X3D haptic flow visualisation (the most recent work)
├── iris-explorer/
│   ├── module-sdk/           C++/C# SDK (Visual Studio solution) for authoring IRIS Explorer modules
│   └── modules/
│       ├── slicer/           "slicer" map + its modules (Euler, Renderer, insert-interactor)
│       └── smokestack/       "SmokeStack" map + its modules (flow viz over a chimney/smokestack)
└── open-inventor-examples/
    ├── cone/                 SoWin "Hello Cone" example
    └── sphere/               SoWin "Hello Sphere" example
```

### `haptic-flow/`

A haptic flow-visualisation application built on H3DAPI. The runnable pieces live at the top of the
folder and are tightly coupled by relative paths, so they have been left together:

- **`*.x3d`** — X3D scene graphs (the scenes you load). `demo1.bat` / `demo2.bat` launch
  `base.x3d` / `base2.x3d` via `H3DLoad`.
- **`*.py`** — H3D Python scripts that drive the scenes (`IrisReader.py`, `flowpath.py`,
  `PutStreamTubes.py`, `PutStreamRibbons.py`, `ProbeDisplay.py`, `force_field.py`, `state.py`, …).
- **`*.raw` / `*.dat` / `*.asc` / `*.info`** — volumetric vector-field datasets (e.g. `flume.raw`,
  `flowpath.raw`) and helper data. `generate_raw.c/.py` and `raw2ascii.c` convert between formats.
- **Native node libraries** (each builds a `.dll` / `.so` with **SCons**; referenced from the X3D
  scenes by relative path):
  - `torque/` — force/torque field haptic node.
  - `IBFVStreamSurface/` — Image-Based Flow Visualisation stream-surface rendering.
  - `ImprovedVectorVolume/` — improved vector-volume haptic node.
  - `KeyEventGenerator/` — keyboard event helper node.
  - `get_raw_val/`, `test_font/` — small support/test utilities.
- **`get_last_vhtk.bat`** — copies `VHTK.dll` from `%VHTK_ROOT%` (the Volume Haptics Toolkit, an
  external dependency that is not included here).

### `iris-explorer/module-sdk/`

A Visual Studio solution (`IrisModules.sln`) for authoring IRIS Explorer modules in C++ and .NET:

- **`im/`** — the "Iris Module" core: `imMain` (native), `imManaged` (C++/CLI bridge),
  `imSharp` (C# wrapper exposing `Module`, `Port`, `Connection`, `IrisNative`), and `imSharpTest`.
- **`iim/`** — interaction & rendering: `iimInteractors` (Mouse, P5 Glove, 5DT/Vicon glove),
  `iimRender`, `iimRenderLite`, `iimSelector`.
- **`other/`** — prototypes and templates (`IrisModuleTemplate`, `CoinInteractors`,
  managed-module prototype, test module).

### `iris-explorer/modules/`

Finished IRIS Explorer modules together with the network **maps** (`*.map`) that wire them up.
Each module directory typically contains IRIS Explorer's file set: `*.cx.c` (module wrapper),
`*.uf.h` (user-function header), `*.mres` (module resource), `*.doc`/`*.help` (binary help),
`*.mak` (makefile), plus the actual C/C++ source.

- **`slicer/`** — `slicer.map` plus the `Euler`, `Renderer`, and `insertInteractorHW` modules.
- **`smokestack/`** — `SmokeStack.map` plus the `CropPyrHW`, `FVHW`, `PyrToGeomHW`, `RendererHW2`,
  and tri-state-interactor modules, with geometry data under `maps/`.

### `open-inventor-examples/`

Minimal Open Inventor / Coin3D examples using the SoWin GUI binding (`cone/`, `sphere/`) — the
classic "Hello Cone" tutorial, kept as learning references.

## A note on file types you'll see

Because much of this targets IRIS Explorer, you'll encounter file types that aren't common today:

| Extension | Meaning |
|-----------|---------|
| `.map`    | IRIS Explorer network/map (a `cxNetResources` script — plain text) |
| `.cx.c`   | Module "wrapper" generated/edited for IRIS Explorer |
| `.uf.h`   | Module user-function header |
| `.mres`   | Module resource definition |
| `.doc`, `.help`, `.credit` | Binary IRIS Explorer module documentation/help blobs |
| `.x3d`    | X3D scene graph (used by the haptic-flow H3D work) |
| `SConstruct` | SCons build script (haptic-flow native libraries) |

## Build artifacts

Generated build outputs (`.obj`, `.exe`, `.lib`, `.exp`, `.ilk`, `.pdb`, `.pch`, `.sconsign`,
`.aps`, `.res`) have been removed from this archive — they were machine-specific binaries for
toolchains and runtimes that are no longer available. Only source, project/build files, scene
definitions, and data have been kept.
