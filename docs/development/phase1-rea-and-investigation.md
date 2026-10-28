# Phase 1 — REA setup and authorized investigation

**Status:** REA MCP verified in Cursor (`user-rea`, ready); local git linked and pushed  
**Remote repo:** https://github.com/mauropn/MS.3DMaker.git  
**Date:** 2026-10-28

## 1. What was configured

| Item | Result |
|------|--------|
| Node.js | **v24.22.1** (meets `rea-agents@6.8.0` engines) |
| REA package | `rea-agents@6.8.0` via `npx` |
| Cursor MCP | Updated `C:\Users\mauro\.cursor\mcp.json` — REA server added; **Stripe MCP preserved** |
| Backups | `mcp.json.rea.backup` (REA) and `mcp.json.ms3d.pre-rea.bak` (MS3DMaker) |
| Skill | `C:\Users\mauro\.agents\skills\reverse-engineer-anything` |
| Setup doctor | `doctor.healthy: true` for Cursor client scope |
| Optional engines | Ghidra / IDA / JADX / Kuna / Hopper — **not** installed (optional later) |

MCP command registered (conceptually):

```text
cmd.exe /d /c npx -y rea-agents@6.8.0 mcp
```

## 2. MCP verification (completed)

- Namespace: `user-rea`
- Status: `ready`
- Tool catalog discoverable (200+ tools, e.g. `binary_overview`, `analyze_function`, `open_binary`, …)
- Live call check: `binary_overview` responded correctly with `target_unavailable` (no app open yet) — proves MCP connectivity
- Optional analysis engines (Ghidra/IDA/JADX/Hopper): still not installed (acceptable for now)

## 3. Authorized investigation policy (MS3DMaker)

MS3DMaker is an **independent** open-source CAD application. Fusion 360 may inform *product understanding* (workflows, public API concepts), never as a source of proprietary implementation to copy.

### Allowed

- Official Autodesk documentation and public Fusion 360 API / extension docs  
- Documented user workflows and publicly described capabilities  
- Public technical discussions and research  
- Open-source CAD references (especially FreeCAD): architecture, module boundaries, persistence patterns — used as **inspiration**, not paste-and-rename  
- REA against **targets you are legally allowed to inspect** (e.g. your own builds, open-source apps, Windows Calculator demos as in REA docs)

### Not allowed

- Decompiling Fusion 360 proprietary binaries to recover protected internals  
- Bypassing technical protections or license checks  
- Extracting proprietary assets, schemas, or internal algorithms for derivative implementation  
- Modifying the installed Fusion 360 product  
- Treating REA output from Fusion as a blueprint for MS3DMaker source

### Primary open reference

- https://github.com/FreeCAD/FreeCAD  
- https://freecad.github.io/DevelopersHandbook/  
- https://help.autodesk.com/cloudhelp/ENU/Fusion-360-API/

## 4. What REA is (and is not)

REA gives the coding agent tools to inspect software behavior. It is **not** a CAD kernel and will not auto-build MS3DMaker. Use it when a specific, authorized investigation question benefits from runtime/static inspection.

## 5. Next steps after Cursor restart

1. Verify REA MCP tools are visible to the agent.  
2. Optionally connect the local workspace to `mauropn/MS.3DMaker` (git init / remote / push — founder approval).  
3. Phase 2 — i18n architecture ADR.  
4. Phase 3 — license ADR + governance docs on the existing GitHub repo.  
5. Phase 4 — first CAD prototype (Qt + OCCT viewport).
