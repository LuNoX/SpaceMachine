#!/usr/bin/env python3
"""Run clangd-tidy, with cmake compatible output, over the passed files.

clang-tidy and clangd behave differently when it comes to header includes.
clangd has better detection for transitive includes. So in addition to running
clang-tidy during cmake builds, this script runs clangD-tidy over the project.
"""

import re
import subprocess
import sys

"""Mapping between clangd and cmake severity levels.

Clangd-tidy outputs diagnositcs with severities different to the cmake ones.
In order for cmake to report them accurately, translate accordingly.
"""
_SEVERITIES = {
    # clangd-tidy: cmake
    "Error": "error",
    "Warning": "warning",
    "Information": "info",
    "Hint": "remark",
    "Note": "note",
}


class CaptureGroupNames:
    """Capture group names for the clangd-tidy diagnostic regex."""

    location = "location"
    severity = "severity"
    message = "message"


"""Regex matching clangd-tidy diagnostics

filename:line:column: Severity: message"""
_DIAGNOSTIC = re.compile(
    "^"
    rf"(?P<{CaptureGroupNames.location}>.+:\d+:\d+): "
    f"(?P<{CaptureGroupNames.severity}>{'|'.join(_SEVERITIES.keys())}): "
    f"(?P<{CaptureGroupNames.message}>.*)"
    "$",
    re.MULTILINE,
)


def _clangd_tidy_to_cmake(diagnostic: re.Match) -> str:
    """Convert clangd-tidy diagnostic to cmake format.

    Rewrites clangd-tidy severity with its cmake severity."""
    location = diagnostic[CaptureGroupNames.location]
    severity = _SEVERITIES[diagnostic[CaptureGroupNames.severity]]
    message = diagnostic[CaptureGroupNames.message]
    return f"{location}: {severity}: {message}"


def main() -> int:
    """Run clangd-tidy with the passed cli arguments,
    then rewrite the output to cmake format.
    """
    completed = subprocess.run(
        ["clangd-tidy", *sys.argv[1:]],
        capture_output=True,
        encoding="utf-8",
        errors="replace",
    )

    diagnostics = _DIAGNOSTIC.sub(_clangd_tidy_to_cmake, completed.stdout)

    sys.stdout.write(diagnostics)
    sys.stderr.write(completed.stderr)
    return completed.returncode


if __name__ == "__main__":
    sys.exit(main())
