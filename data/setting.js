document.addEventListener("DOMContentLoaded", () => {
  fetch('/expense.json')
    .then(res => res.json())
    .then(data => {
      Object.keys(data).forEach(id => {
        const input = document.getElementById(id);
        if (input && (input.type === "number" || input.type === "time")) input.value = data[id];
      });
      updateTierLabelsUI();
    })
    .catch(err => console.error("[RESTORE ERROR] Fetching expense settings:", err));
});

async function postJSON(url, payload) {
  console.log("[POST REQUEST]", url, payload);
  try {
    const res = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload)
    });
    const responseText = await res.text();
    console.log("[RESPONSE]", res.status, responseText);
    return responseText;
  } catch (err) {
    console.error("[FETCH ERROR]", url, err);
    return "";
  }
}

// --------------------------------------------------------------------------
// Expense settings handler
// --------------------------------------------------------------------------
async function saveExpenseSettings() {
  const expenseData = {
    UnitCost1: Number(document.getElementById("UnitCost1")?.value || 200),
    PriceCost1: Number(document.getElementById("PriceCost1")?.value || 3.0000),
    UnitCost2: Number(document.getElementById("UnitCost2")?.value || 400),
    PriceCost2: Number(document.getElementById("PriceCost2")?.value || 4.1584),
    PriceCost3: Number(document.getElementById("PriceCost3")?.value || 4.3583),
    UnitSolar: Number(document.getElementById("UnitSolar")?.value || 450),
    ft: Number(document.getElementById("ft")?.value || 0.3972),
    ServiceFee: Number(document.getElementById("ServiceFee")?.value || 38.22),
    VatRate: Number(document.getElementById("VatRate")?.value || 7),
    DailyResetTime: document.getElementById("DailyResetTime")?.value || "00:00",
    MonthlyResetDay: Number(document.getElementById("MonthlyResetDay")?.value || 1)
  };

  console.log("[ACTION] Saving Expense Settings:", expenseData);
  const response = await postJSON('/expense.json', expenseData);
  try {
    if (JSON.parse(response).status === "success") {
      alert("Expense settings saved successfully!");
      return;
    }
  } catch (err) {
    console.error("[SAVE ERROR] Expense settings:", err);
  }
  alert("Unable to save expense settings.");
}

/**
 * Calculate progressive electricity cost.
 */
function calculateProgressiveGridCost(totalUnits, cfg = {}) {
  const tier1Limit = parseFloat(cfg.UnitCost1 || 200);
  const tier1Price = parseFloat(cfg.PriceCost1 || 3.0000);
  const tier2Limit = parseFloat(cfg.UnitCost2 || 400);
  const tier2Price = parseFloat(cfg.PriceCost2 || 4.1584);
  const tier3Price = parseFloat(cfg.PriceCost3 || 4.3583);

  let cost = 0;
  if (totalUnits <= 0) return 0;

  const tier1Units = Math.min(totalUnits, tier1Limit);
  cost += tier1Units * tier1Price;

  if (totalUnits > tier1Limit) {
    const tier2Units = Math.min(totalUnits - tier1Limit, tier2Limit - tier1Limit);
    cost += tier2Units * tier2Price;
  }

  if (totalUnits > tier2Limit) {
    const tier3Units = totalUnits - tier2Limit;
    cost += tier3Units * tier3Price;
  }

  return cost;
}

function updateTierLabelsUI() {
  const tier1Val = parseInt(document.getElementById("UnitCost1")?.value || document.getElementById("UnitCost1")?.placeholder || 200, 10);
  const tier2Val = parseInt(document.getElementById("UnitCost2")?.value || document.getElementById("UnitCost2")?.placeholder || 400, 10);

  const t1RangeText = document.getElementById("tier1RangeText");
  const t2StartText = document.getElementById("tier2StartText");
  const t2EndText = document.getElementById("tier2EndText");
  const t3StartText = document.getElementById("tier3StartText");

  if (t1RangeText) t1RangeText.textContent = tier1Val;
  if (t2StartText) t2StartText.textContent = tier1Val + 1;
  if (t2EndText) t2EndText.textContent = tier2Val;
  if (t3StartText) t3StartText.textContent = tier2Val;
}

function submitAllSettings() {
  const data = {};
  const expenseView = document.getElementById("expenseSettingsView");
  document.querySelectorAll('input[type="checkbox"]').forEach(el => el.id && !expenseView?.contains(el) && (data[el.id] = el.checked ? "1" : "0"));
  document.querySelectorAll('input[type="number"]').forEach(el => el.id && !expenseView?.contains(el) && (data[el.id] = el.value));
  document.querySelectorAll('select').forEach(el => el.id && !expenseView?.contains(el) && (data[el.id] = el.value));

  console.log("[SYNCING] Saving full state to /setting.json...");
  postJSON('/setting.json', data);
}
