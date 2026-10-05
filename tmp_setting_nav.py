from pathlib import Path
root = Path(r"D:\@Project\PowerMeter_PZEM004\data")
for name in ["dashboard.html","setting.html","network.html","info.html","ota.html"]:
    p = root / name
    if p.exists():
        s = p.read_text(encoding="utf-8")
        s = s.replace(">Expense<", ">Setting<")
        p.write_text(s, encoding="utf-8")
print("navigation label updated")
