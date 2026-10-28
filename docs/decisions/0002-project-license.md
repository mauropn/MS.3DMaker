# ADR 0002 — Project application license

**Status:** Proposed (awaiting founder decision)  
**Date:** 2026-10-28  
**Deciders:** Project founder ([mauropn](https://github.com/mauropn))  

## Problem

MS3DMaker needs a public application license before treating the project as a settled open-source product. The choice must be compatible with **Qt** (open-source modules, typically LGPL) and **OCCT** (LGPL-family), and must match founder-led distribution goals.

Important: the application license does **not** override dependency licenses. Binary distributions must still meet Qt/OCCT obligations (dynamic linking, notices, offer of object files / LGPL compliance as applicable).

## Options

### MIT

- **Pros:** Maximum reuse; simple; friendly to proprietary and open downstream projects.
- **Cons:** No express patent grant; recipients can relicense modifications under almost any terms.
- **With Qt/OCCT:** Compatible if LGPL obligations for those libraries are still met when shipping binaries.

### Apache-2.0 (recommended)

- **Pros:** Permissive like MIT; **explicit patent grant**; clear NOTICE/attribution practices; widely used in industry and foundations.
- **Cons:** Slightly more text than MIT; requires preserving NOTICE files where used.
- **With Qt/OCCT:** Compatible under the same LGPL compliance rules for dependencies.

### LGPL-3.0

- **Pros:** Aligns closely with Qt/OCCT spirit; allows linking from differently licensed apps if LGPL terms for *this* library are met.
- **Cons:** MS3DMaker is an **application**, not primarily a library; LGPL’s linking model is a poorer fit for a desktop CAD executable and plugins story; more complex for contributors.
- **With Qt/OCCT:** Compatible, but does not remove the need to comply with Qt/OCCT themselves.

### GPL-3.0

- **Pros:** Strong copyleft; ensures derivative works of the combined program stay Free under GPL terms.
- **Cons:** Forces strong copyleft on the whole distributed work; can deter commercial contributors/plugins that cannot use GPL; still does not erase Qt/OCCT LGPL duties.
- **With Qt/OCCT:** Generally compatible (LGPL code can combine into a GPL work), but the **application** becomes GPL, which is a major policy choice.

## Recommendation

Adopt **Apache-2.0** for MS3DMaker application source, plus:

- `NOTICE` (and/or `THIRD_PARTY_NOTICES`) listing Qt, OCCT, and other dependencies
- Documented binary redistribution checklist for LGPL compliance
- Keep dependency licenses unchanged

Rationale: permissive growth and community contributions, patent clarity, and no conflict with remaining founder control via governance (this ADR does not force copyleft).

## Founder decision required

Please choose one:

1. **Apache-2.0** (recommended)  
2. **MIT**  
3. **LGPL-3.0**  
4. **GPL-3.0**  
5. Defer LICENSE file (keep status Proposed)

Until a choice is confirmed, **do not** treat a repository `LICENSE` file as final.

## Testing / compliance follow-up (after lock)

- Add `LICENSE` and `NOTICE`
- Mention license in `README.md` and GitHub repo metadata
- CI later: license header policy (optional), dependency inventory

## Reversibility

Changing from permissive (MIT/Apache) to GPL later is hard for recipients of older releases. Changing GPL → permissive is also disruptive. Prefer locking before wide external contribution.
