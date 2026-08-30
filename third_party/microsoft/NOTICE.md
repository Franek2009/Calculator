# Microsoft Runtime Components Notice

The Windows package may contain unmodified Microsoft redistributable runtime
components selected and copied by Qt's deployment tooling. These can include:

- `D3Dcompiler_47.dll`, the Microsoft Direct3D shader compiler runtime;
- a Microsoft Visual C++ runtime redistributable payload required by the
  Release build.

These components are Microsoft software, not part of Calculator, Qt, or Mesa,
and are not licensed under the Calculator project's MIT License. They remain
subject to the applicable Microsoft redistribution and license terms. No
corresponding source-code distribution applies to these proprietary runtime
components.

Microsoft redistribution information:
https://learn.microsoft.com/en-us/windows/win32/directx-sdk--august-2009-
https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files
