# Suggested Commands

- Build: `pio run` from the project root.
- Upload firmware: `pio run -t upload`.
- Monitor serial output: `pio device monitor` (115200 baud configured by project).
- Build/upload filesystem when `data/` is populated: `pio run -t buildfs` / `pio run -t uploadfs`.
- Windows shell: use `dir`/PowerShell equivalents rather than assuming Unix utilities.