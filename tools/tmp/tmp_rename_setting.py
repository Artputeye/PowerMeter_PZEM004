from pathlib import Path
root = Path(r"D:\@Project\PowerMeter_PZEM004")
old = root / "data" / "expense.html"
new = root / "data" / "setting.html"
old.rename(new)
for p in root.joinpath("data").glob("*"):
    if p.is_file():
        s = p.read_text(encoding="utf-8")
        if "expense.html" in s:
            p.write_text(s.replace("expense.html", "setting.html"), encoding="utf-8")
print("renamed and references updated")
