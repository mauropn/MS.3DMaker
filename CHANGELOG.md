# Changelog

All notable changes to MS3DMaker are documented in this file.

Format inspired by [Keep a Changelog](https://keepachangelog.com/). Versioning will follow SemVer once 0.1.0 is formally released (founder approval required for releases).

## [Unreleased]

### Added

- Phase 0: Windows environment verification; Linux `scripts/setup-linux.sh`; Qt/OCCT smoke examples
- Phase 1: REA MCP notes; repository linked to https://github.com/mauropn/MS.3DMaker
- Phase 2: i18n foundation (`en` / `pt-BR`), `LanguageService`, Preferences language UI, ADR 0001
- Phase 3: Founder-led governance docs; license locked as MIT OR Apache-2.0 (ADR 0002 Accepted, aligned with SolidCraft)
- Phase 4: OCCT 3D viewport (`OcctView`) with triad, view cube, orbit/pan/zoom, selection highlight; demo box solid (100×60×30 mm); `SolidFactory` + geometry tests; CMake OCCT link and Windows runtime DLL copy
- Phase 5.1 (Milestone 2): Cylinder, sphere, extrusion, revolution, and boolean union/cut/common in `SolidFactory`; Create menu demos; `displaySolid`; expanded geometry tests; TKBO/TKBool link
- Phase 5.2 (Milestone 3): Sketcher — plane selection, line/circle/rectangle tools, entity selection, dimensions, basic H/V/length constraints, validation; ADR 0003 (lightweight solver strategy)
- Phase 5.3 (Milestone 4): Feature history — definitions, parameters, dependency graph, invalidate/recompute, edit prior features, history dock + error handling
