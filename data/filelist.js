document.addEventListener("DOMContentLoaded", () => {
  const tableBody = document.querySelector("#fileTable tbody");
  const fileCount = document.querySelector("#fileCount");
  const refreshButton = document.querySelector("#refreshButton");

  function loadFileList() {
    refreshButton.disabled = true;
    refreshButton.textContent = "LOADING";

    fetch("/list?dir=/")
      .then(response => {
        if (!response.ok) throw new Error(`HTTP ${response.status}`);
        return response.json();
      })
      .then(renderFileList)
      .catch(error => {
        console.error("Error loading file list:", error);
        fileCount.textContent = "-- files";
        renderMessage("Failed to load file system data.", true);
      })
      .finally(() => {
        refreshButton.disabled = false;
        refreshButton.textContent = "REFRESH";
      });
  }

  function renderFileList(files) {
    tableBody.innerHTML = "";
    fileCount.textContent = `${files.length} ${files.length === 1 ? "file" : "files"}`;

    if (!files.length) {
      renderMessage("No files found in directory.");
      return;
    }

    files.forEach(file => {
      const row = document.createElement("tr");
      row.appendChild(createTypeCell(file.type));
      row.appendChild(createNameCell(file.name));
      row.appendChild(createActionsCell(file));
      tableBody.appendChild(row);
    });
  }

  function renderMessage(message, isError = false) {
    tableBody.innerHTML = "";
    const row = document.createElement("tr");
    const cell = document.createElement("td");
    cell.colSpan = 3;
    cell.className = `empty-message${isError ? " error-message" : ""}`;
    cell.textContent = message;
    row.appendChild(cell);
    tableBody.appendChild(row);
  }

  function createTypeCell(type) {
    const cell = document.createElement("td");
    const badge = document.createElement("span");
    badge.className = `file-badge ${type === "dir" ? "dir" : "file"}`;
    badge.textContent = type === "dir" ? "DIR" : "FILE";
    cell.appendChild(badge);
    return cell;
  }

  function createNameCell(name) {
    const cell = document.createElement("td");
    cell.className = "file-name";
    cell.textContent = name;
    return cell;
  }

  function createActionsCell(file) {
    const cell = document.createElement("td");
    cell.className = "actions-cell";

    if (file.type === "file") {
      const size = document.createElement("span");
      size.className = "file-size";
      size.textContent = formatBytes(Number(file.size) || 0);

      const deleteButton = document.createElement("button");
      deleteButton.className = "delete-button";
      deleteButton.type = "button";
      deleteButton.textContent = "DELETE";
      deleteButton.addEventListener("click", () => deleteFile(file.name));

      cell.append(size, deleteButton);
    } else {
      cell.textContent = "-";
      cell.style.color = "var(--muted)";
    }

    return cell;
  }

  function formatBytes(bytes) {
    if (bytes < 1024) return `${bytes} B`;
    if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
    return `${(bytes / (1024 * 1024)).toFixed(1)} MB`;
  }

  function deleteFile(path) {
    if (!confirm(`Delete ${path}?`)) return;

    fetch(`/delete?file=${encodeURIComponent(path)}`)
      .then(response => {
        if (!response.ok) {
          return response.text().then(text => {
            throw new Error(text || "Delete failed");
          });
        }
        loadFileList();
      })
      .catch(error => {
        console.error("Delete error:", error);
        alert(`Delete failed: ${error.message}`);
      });
  }

  refreshButton.addEventListener("click", loadFileList);
  loadFileList();
});
