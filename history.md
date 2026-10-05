ทำงานที่ D:\@Project\PowerMeter_PZEM004
ปรับปรุงหน้า dashboard.html ตาม skill.md
ลองปรับสี hader และ nav เป็นสีขาวใส โทนสว่าง
ตอนนี้ Header + Navigation เป็นโทน:
🤍 ขาวสว่าง
โปร่งใสแบบ soft glass
backdrop-filter: blur(14px)
Border บาง ๆ
Shadow เบามาก
ตัวหนังสือเป็น dark gray
Active nav เป็นพื้นขาวเด่นขึ้นเล็กน้อย
ยังคง #454746 ไว้เป็น accent เล็ก ๆ ใน icon/องค์ประกอบ
โดยยังไม่แตะ HTML/JS หรือระบบ WebSocket/API ครับ

แก้ไข class="brand-mark" ให้ไปใช้ icon ใน floder data/pic/transmission-tower และออกแบบตาม design.md

ปรับใช้ header และ nav ให้ไปใช้กับหน้า network.html,setting.html,ota.html,console.html 
header และ nav ของหน้าทั้งหมดให้อ้างอิง dashboard.css ที่เดียว

ใส่ nav bar ให้ไปใช้กับหน้า network.html,setting.html,ota.html,console.html nav ของหน้าทั้งหมดให้อ้างอิง dashboard.css ที่เดียว

Nav bar เวลาเปลี่ยนหน้าจาก dashboard ไปยัง network แล้ว hilight มันไม่เปลี่ยนตาม

modern-ui.css navigation.js
 
style ENERGY HISTORY และ POWER HISTORY ให้ใช้แบบเดียวกับ hybrid inverter

