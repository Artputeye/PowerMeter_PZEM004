from pathlib import Path
p = Path(r"D:\@Project\PowerMeter_PZEM004\data\dashboard.css")
s = p.read_text(encoding="utf-8")
old = """.top-navigation {
  display: flex;
  align-self: flex-end;
  gap: 2px;
  margin: 0 auto -14px;
  padding: 0 9px;
  border-radius: 13px 13px 0 0;
  background: linear-gradient(135deg, #0d6efd, #2196f3);
  box-shadow: 0 4px 18px rgba(13,110,253,.08);
  max-width: calc(100% - 32px);
}

.top-navigation-link {
  display: flex;
  align-items: center;
  gap: 7px;
  padding: 15px 16px;
  color: #42617e;
  font-size: 11px;
  font-weight: 700;
  text-decoration: none;
  white-space: nowrap;
}

.top-navigation-link span {
  font-size: 15px;
}

.top-navigation-link:hover {
  color: #0d6efd;
  background: rgba(13,110,253,.08);
}

.top-navigation-link.active {
  color: #fff;
  background: linear-gradient(135deg,#1976d2,#2196f3);
  box-shadow: 0 4px 10px rgba(13,110,253,.18);
}"""
new = """.top-navigation {
  display: flex;
  align-self: center;
  gap: 3px;
  margin: 0 auto;
  padding: 4px;
  border: 1px solid rgba(13, 110, 253, .12);
  border-radius: 14px;
  background: rgba(13, 110, 253, .06);
  box-shadow: 0 4px 16px rgba(13, 110, 253, .07);
  max-width: calc(100% - 32px);
}

.top-navigation-link {
  display: flex;
  align-items: center;
  gap: 5px;
  padding: 8px 12px;
  border-radius: 10px;
  color: #42617e;
  font-size: 11px;
  font-weight: 700;
  text-decoration: none;
  white-space: nowrap;
  transition: color .18s ease, background .18s ease, box-shadow .18s ease;
}

.top-navigation-link span {
  font-size: 14px;
}

.top-navigation-link:hover {
  color: #0d6efd;
  background: rgba(13, 110, 253, .10);
}

.top-navigation-link.active {
  color: #fff;
  background: linear-gradient(135deg, #1976d2, #2196f3);
  box-shadow: 0 3px 9px rgba(13, 110, 253, .16);
}"""
assert old in s
p.write_text(s.replace(old, new), encoding="utf-8")
print("updated")
