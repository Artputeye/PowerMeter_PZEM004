const wsUrl = 'ws://' + location.host + '/ws';
let socket = null, reconnectTimer = null, historyData = null, historyRange = 'hourly';
const $ = id => document.getElementById(id);
const num = value => { const n = Number(value); return Number.isFinite(n) ? n : null; };
const fmt = (value, digits = 2) => { const n = num(value); return n === null ? '--' : n.toFixed(digits); };
const set = (id, value) => { const node = $(id); if (node) node.textContent = value; };

function setConnection(online) {
  set('connectionText', online ? 'Connected' : 'Disconnected'); set('wsText', online ? 'Connected' : 'Disconnected');
  $('connectionDot')?.classList.toggle('online', online); $('connectionDot')?.classList.toggle('offline', !online);
  $('wsStatus')?.classList.toggle('online', online); $('wsStatus')?.classList.toggle('offline', !online);
}

function updatePowerDirection(direction, valid) {
  const badge = $('flowDirectionBadge'), title = $('flowTitle'), arrow = $('flowArrow'), text = $('flowDirectionText'), hint = $('flowDirectionHint');
  const line1 = $('flowLine1'), line2 = $('flowLine2');
  if (!badge) return;
  badge.classList.remove('forward', 'reverse', 'idle', 'unknown');
  badge.classList.add(valid ? direction : 'unknown');
  line1?.classList.remove('flow-unknown'); line2?.classList.remove('flow-unknown');
  if (!valid || direction === 'unknown') {
    if (title) title.textContent = 'Power Flow'; if (arrow) arrow.textContent = '↔'; if (text) text.textContent = 'NO DATA';
    if (hint) hint.textContent = 'Waiting for power direction'; line1?.classList.add('flow-unknown'); line2?.classList.add('flow-unknown'); return;
  }
  if (direction === 'forward') {
    if (title) title.textContent = 'Grid → Load'; if (arrow) arrow.textContent = '→'; if (text) text.textContent = 'GRID → LOAD';
    if (hint) hint.textContent = 'Power entering the load';
  } else if (direction === 'reverse') {
    if (title) title.textContent = 'Load → Grid'; if (arrow) arrow.textContent = '←'; if (text) text.textContent = 'LOAD → GRID';
    if (hint) hint.textContent = 'Reverse power / export';
  } else {
    if (title) title.textContent = 'No Power Flow'; if (arrow) arrow.textContent = '—'; if (text) text.textContent = 'IDLE';
    if (hint) hint.textContent = 'Power near zero';
  }
}

function updateTelemetry(d) {
  const valid = !!d.valid, power = num(d.power), voltage = num(d.voltage), current = num(d.current), pf = num(d.pf);
  const direction = power === null ? 'unknown' : (power < -1 ? 'reverse' : (power > 1 ? 'forward' : 'idle'));
  set('voltage', fmt(voltage, 1)); set('current', fmt(current, 2)); set('power', fmt(power, 1)); set('energyTotal', fmt(d.energy, 3));
  set('frequency', fmt(d.frequency, 1)); set('pf', fmt(pf, 3));
  set('flowPower', fmt(Math.abs(power), 0)); set('flowLoad', fmt(Math.abs(power), 0));
  set('flowCurrent', current === null ? '-- A' : current.toFixed(2) + ' A'); set('powerMain', fmt(power, 1)); set('loadGaugePower', fmt(Math.abs(power), 0));
  set('voltageStat', fmt(voltage, 1)); set('currentStat', fmt(current, 2)); set('pfStat', fmt(pf, 3));
  set('insightPower', fmt(power, 1)); set('insightVoltage', fmt(voltage, 1)); set('insightCurrent', fmt(current, 2)); set('valid', valid ? 'OK' : 'NO DATA');
  updatePowerDirection(direction, valid);
  ['voltageCard', 'currentCard', 'powerCard', 'frequencyCard', 'pfCard', 'statusCard'].forEach(id => { const c = $(id); if (c) { c.classList.toggle('offline', !valid); c.classList.toggle('normal', valid); } });
  $('telemetryStatus')?.classList.toggle('online', valid); $('telemetryStatus')?.classList.toggle('offline', !valid); set('telemetryText', valid ? 'Live data' : 'No data');
  set('systemStatusTitle', valid ? 'PZEM telemetry available' : 'Waiting for telemetry'); set('systemStatusBadge', valid ? 'LIVE DATA' : 'NO DATA');
  $('systemStatusBadge')?.classList.toggle('online', valid); $('systemStatusBadge')?.classList.toggle('offline', !valid);
  const circumference = 263.89, maxGauge = 4200, pct = power === null ? 0 : Math.min(100, Math.max(0, Math.abs(power) / maxGauge * 100)), circle = $('loadGaugeCircle');
  if (circle) { circle.style.strokeDashoffset = String(circumference - circumference * pct / 100); circle.classList.toggle('offline', !valid); }
  set('lastUpdate', new Date().toLocaleTimeString('en-GB', { hour: '2-digit', minute: '2-digit', second: '2-digit', hourCycle: 'h23' }));
}

function connect() {
  if (socket && (socket.readyState === WebSocket.OPEN || socket.readyState === WebSocket.CONNECTING)) return;
  socket = new WebSocket(wsUrl); socket.onopen = () => setConnection(true);
  socket.onmessage = event => { try { updateTelemetry(JSON.parse(event.data)); } catch (error) { console.error('Telemetry error', error); } };
  socket.onerror = () => socket.close(); socket.onclose = () => { setConnection(false); clearTimeout(reconnectTimer); reconnectTimer = setTimeout(connect, 2000); };
}

function drawLineChart(canvasId, values, unit, colorMode) {
  const canvas = $(canvasId); if (!canvas) return; const wrap = canvas.parentElement, width = wrap.clientWidth, height = wrap.clientHeight, ratio = window.devicePixelRatio || 1;
  canvas.width = Math.max(1, Math.floor(width * ratio)); canvas.height = Math.max(1, Math.floor(height * ratio)); canvas.style.width = width + 'px'; canvas.style.height = height + 'px';
  const ctx = canvas.getContext('2d'); ctx.setTransform(ratio, 0, 0, ratio, 0, 0); ctx.clearRect(0, 0, width, height);
  const clean = values.map(num), available = clean.filter(v => v !== null); if (!available.length) return;
  const maxValue = Math.max(1, ...available), pad = { left: 48, right: 14, top: 18, bottom: 28 }, plotW = width - pad.left - pad.right, plotH = height - pad.top - pad.bottom;
  ctx.strokeStyle = '#dfe9e7'; ctx.lineWidth = 1; ctx.fillStyle = '#789195'; ctx.font = '10px DM Sans, sans-serif'; ctx.textAlign = 'right';
  for (let i = 0; i <= 4; i++) { const y = pad.top + plotH - plotH * i / 4; ctx.beginPath(); ctx.moveTo(pad.left, y); ctx.lineTo(width - pad.right, y); ctx.stroke(); ctx.fillText((maxValue * i / 4).toFixed(maxValue < 10 ? 1 : 0), pad.left - 8, y + 3); }
  ctx.textAlign = 'center'; const step = clean.length > 1 ? plotW / (clean.length - 1) : plotW;
  clean.forEach((value, index) => { if (value === null) return; if (index === 0 || index === clean.length - 1 || index % Math.max(1, Math.floor(clean.length / 6)) === 0) ctx.fillText(colorMode === 'power' ? '-' + (clean.length - index) + 'm' : String(index + 1), pad.left + index * step, height - 8); });
  ctx.beginPath(); let started = false; clean.forEach((value, index) => { if (value === null) { started = false; return; } const x = pad.left + index * step, y = pad.top + plotH - (value / maxValue) * plotH; if (!started) { ctx.moveTo(x, y); started = true; } else ctx.lineTo(x, y); });
  ctx.strokeStyle = colorMode === 'power' ? '#4d99df' : '#23b99a'; ctx.lineWidth = 2; ctx.stroke(); if (colorMode === 'power') set('powerPeak', Math.max(...available).toFixed(0));
}
function historyLabels(count, range) { const now = new Date(); return Array.from({ length: count }, (_, i) => { const back = count - i - 1, d = new Date(now); if (range === 'hourly') { d.setHours(d.getHours() - back, 0, 0, 0); return d.toLocaleTimeString('en-GB', { hour: '2-digit', hourCycle: 'h23' }); } if (range === 'daily') { d.setDate(d.getDate() - back); return d.toLocaleDateString('en-US', { month: 'short', day: 'numeric' }); } d.setDate(1); d.setMonth(d.getMonth() - back); return d.toLocaleDateString('en-US', { month: 'short' }); }); }
function renderEnergyHistory() { if (!historyData) return; const values = historyData[historyRange] || [], total = values.reduce((s, v) => s + (num(v) || 0), 0); set('historyTotal', total.toFixed(2)); set('energyToday', (historyData.daily?.[historyData.daily.length - 1] ?? 0).toFixed(2)); set('energyMonth', (historyData.monthly?.[historyData.monthly.length - 1] ?? 0).toFixed(2)); set('summaryToday', (historyData.daily?.[historyData.daily.length - 1] ?? 0).toFixed(2)); set('summaryMonth', (historyData.monthly?.[historyData.monthly.length - 1] ?? 0).toFixed(2)); drawLineChart('energyChart', values, 'kWh', 'energy'); $('energyChartEmpty').style.display = values.some(v => num(v) !== null && num(v) > 0) ? 'none' : 'block'; }
function renderPowerHistory() { if (!historyData) return; const values = historyData.power || []; drawLineChart('powerChart', values, 'W', 'power'); $('powerChartEmpty').style.display = values.some(v => num(v) !== null && num(v) > 0) ? 'none' : 'block'; }
async function fetchExpense() { try { const r = await fetch('/api/expense?t=' + Date.now(), { cache: 'no-store' }); if (!r.ok) throw new Error('HTTP ' + r.status); const d = await r.json(); set('estimatedBill', num(d.estimatedBill) === null ? '--' : num(d.estimatedBill).toFixed(2)); } catch (e) { set('estimatedBill', '--'); console.error('Expense error', e); } } 
async function fetchHistory() { try { const response = await fetch('/api/history?t=' + Date.now(), { cache: 'no-store' }); if (!response.ok) throw new Error('HTTP ' + response.status); historyData = await response.json(); set('historyStatus', 'Saved energy + 1 min power samples'); renderEnergyHistory(); renderPowerHistory(); } catch (error) { set('historyStatus', 'History unavailable'); console.error(error); } }
function setupHistoryControls() { document.querySelectorAll('[data-range]').forEach(button => button.addEventListener('click', () => { historyRange = button.dataset.range; document.querySelectorAll('[data-range]').forEach(b => b.classList.toggle('active', b === button)); renderEnergyHistory(); })); }
window.addEventListener('load', () => { set('currentDate', new Date().toLocaleDateString('en-GB')); setupHistoryControls(); connect(); fetchHistory(); fetchExpense(); setInterval(fetchHistory, 15000); setInterval(fetchExpense, 15000); });
window.addEventListener('resize', () => { renderEnergyHistory(); renderPowerHistory(); });
