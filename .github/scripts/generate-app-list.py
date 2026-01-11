#!/usr/bin/env python3

import os
import re
import subprocess
import sys
import json

print("Generating app list...")

# List of excluded CubeIDE projects/apps (can be overridden via APPS_EXCLUDED CI variable)
excluded_apps_str = os.environ.get('APPS_EXCLUDED', '')
excluded_apps = []
for line in excluded_apps_str.split('\n'):
  for item in line.split(','):
    if item.strip():
      excluded_apps.append(item.strip())

# User-provided discovery pattern:
#   find Apps/*/Software/ -type d ( -name "*App*" -o -name "*Apps*" ) -exec find {} -type d -maxdepth 1 -mindepth 1 \; 2>/dev/null
cmd = "find Apps/*/Software/ -type d \\(" \
      " -name '*App*' -o -name '*Apps*' \\)" \
      " -exec find {} -type d -maxdepth 1 -mindepth 1 \\; 2>/dev/null"

out = subprocess.check_output(cmd, shell=True, text=True)
candidates = [ln.strip() for ln in out.splitlines() if ln.strip()]

# Keep only CubeIDE projects; ignore TouchGFX-GUI entries from the discovery output.
cubeide_projects = []
for p in candidates:
  if p.endswith("CubeIDE") or p.endswith("CubeIDE/"):
    cubeide_projects.append(p.rstrip("/") + "/")

# Exclude specified apps/projects (substring match)
cubeide_projects = [p for p in cubeide_projects if not any(ex in p for ex in excluded_apps)]

apps = {}
for p in cubeide_projects:
  parts = p.split("/")
  if len(parts) < 2 or parts[0] != "Apps":
    continue
  app = parts[1]
  apps.setdefault(app, set()).add(p)

if not apps:
  print("ERROR: No *CubeIDE app projects found by discovery command", file=sys.stderr)
  print("Discovery output:")
  for ln in candidates:
    print("  " + ln)
  sys.exit(2)

# Output as JSON for GitHub Actions
apps_json = json.dumps(list(apps.keys()))
print(f"apps={apps_json}", file=open(os.environ.get('GITHUB_OUTPUT', '/dev/stdout'), 'a'))