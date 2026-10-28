# Release process

**Automatic publishing of releases or binaries is disabled** until the founder explicitly approves a release workflow.

## Current policy

1. Tagging and GitHub Releases are **manual** and require founder approval ([GOVERNANCE.md](../GOVERNANCE.md)).  
2. CI may build and test on pull requests and `main`; it must **not** upload installers, attach release assets, or push tags.  
3. Changelog entries land in [CHANGELOG.md](../CHANGELOG.md) before a tagged release.  
4. Dual license and third-party notices ship with every source release (`LICENSE*`, `NOTICE`, `THIRD_PARTY_NOTICES.md`).

## Manual checklist (when a release is approved)

- [ ] Version bump in `CMakeLists.txt` / app about string if applicable  
- [ ] `CHANGELOG.md` section for the version  
- [ ] Clean `main`; CI green  
- [ ] Annotated tag `vX.Y.Z` created by founder (or delegate)  
- [ ] Optional: GitHub Release notes pasted from changelog — **no unsigned auto-upload**  
- [ ] Announce in Discussions if used

## Secrets

Do not put signing keys or tokens in workflow YAML. Prefer GitHub Environments with required reviewers if publishing is enabled later.
