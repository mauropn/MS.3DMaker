from pathlib import Path
import xml.etree.ElementTree as ET

xml_path = Path(r"C:\Users\mauro\AppData\Local\Temp\Updates.xml")
root = ET.fromstring(xml_path.read_text(encoding="utf-8", errors="replace"))

targets = {
    "qt.qt6.683.win64_msvc2022_64",
    "qt.qt6.683.addons.qttools",
    "qt.qt6.683.addons.qttools.win64_msvc2022_64",
}

print("=== matching packages ===")
for pkg in root.findall("PackageUpdate"):
    name = pkg.findtext("Name") or ""
    if "debug" in name:
        continue
    if "qttools" in name and "msvc2022_64" in name:
        print(name)
    if name in targets or name == "qt.qt6.683.win64_msvc2022_64":
        print("PKG", name)
        print("VER", pkg.findtext("Version"))
        print("DEP", pkg.findtext("Dependencies"))
        archives = (pkg.findtext("DownloadableArchives") or "").strip()
        print("ARCHIVES", archives)
        print("---")

print("=== all non-debug msvc2022_64 base-ish ===")
for pkg in root.findall("PackageUpdate"):
    name = pkg.findtext("Name") or ""
    if name.endswith(".win64_msvc2022_64") and "debug" not in name and "addons" not in name:
        print(name, "|", (pkg.findtext("DownloadableArchives") or "")[:120])
