# Third-party notices

MS3DMaker depends on third-party components. Their licenses apply to those components independently of the (still Proposed) application license — see [docs/decisions/0002-project-license.md](docs/decisions/0002-project-license.md).

## Qt

- **Component:** Qt 6 (modules such as Qt Widgets, Qt Core, Linguist tools as used)
- **Typical open-source terms:** LGPL / GPL (module-dependent); use the open-source edition unless a commercial Qt license is purchased
- **Project use:** UI, event loop, internationalization
- **More info:** https://www.qt.io/licensing/

When distributing binaries that dynamically link Qt, comply with the applicable Qt open-source license (notices, attribution, and LGPL obligations as required).

## Open CASCADE Technology (OCCT)

- **Component:** Open CASCADE Technology 7.9.x
- **Typical terms:** LGPL 2.1 with Open CASCADE exception (verify the exact texts shipped with your OCCT install, e.g. `LICENSE_LGPL_21.txt`, `OCCT_LGPL_EXCEPTION.txt`)
- **Project use:** Geometric modeling kernel
- **Upstream:** https://github.com/Open-Cascade-SAS/OCCT

## Other tools (build / optional)

| Tool | Role | Typical license |
|------|------|-----------------|
| CMake | Build | BSD-3 |
| Ninja | Build | Apache-2.0 |
| GoogleTest (future) | Tests | BSD-3 |
| rea-agents (optional) | Agent RE tooling | See npm package |

This file will be expanded as dependencies are added. Binary release checklists must include dependency license review.
