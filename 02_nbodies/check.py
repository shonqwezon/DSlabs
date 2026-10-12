import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# ===== НАСТРОЙКИ =====

CSV_FILE = "output/data.csv"
G = 6.67430e-11

# Названия столбцов в CSV
TIME_COL = "time"
BODY_COL = "id"
MASS_COL = "mass"
X_COL = "x"
Y_COL = "y"
VX_COL = "vx"
VY_COL = "vy"

# =====================


df = pd.read_csv(CSV_FILE)

required = [
    TIME_COL, BODY_COL, MASS_COL,
    X_COL, Y_COL, VX_COL, VY_COL
]

missing = [col for col in required if col not in df.columns]
if missing:
    raise ValueError(
        f"В CSV отсутствуют столбцы: {missing}\n"
        f"Доступные столбцы: {list(df.columns)}"
    )

times = []
momentum_x = []
momentum_y = []
momentum_abs = []
kinetic_energy = []
potential_energy = []
total_energy = []

for time, frame in df.groupby(TIME_COL, sort=True):
    m = frame[MASS_COL].to_numpy(dtype=float)
    x = frame[X_COL].to_numpy(dtype=float)
    y = frame[Y_COL].to_numpy(dtype=float)
    vx = frame[VX_COL].to_numpy(dtype=float)
    vy = frame[VY_COL].to_numpy(dtype=float)

    # Полный импульс
    px = np.sum(m * vx)
    py = np.sum(m * vy)
    p_abs = np.hypot(px, py)

    # Кинетическая энергия
    ek = 0.5 * np.sum(m * (vx**2 + vy**2))

    # Потенциальная энергия гравитационного взаимодействия
    dx = x[:, None] - x[None, :]
    dy = y[:, None] - y[None, :]
    r2 = dx**2 + dy**2

    i, j = np.triu_indices(len(m), k=1)
    distances = np.sqrt(r2[i, j])

    if np.any(distances == 0):
        raise ValueError(
            f"Обнаружены совпадающие координаты тел при t={time}"
        )

    ep = -G * np.sum(m[i] * m[j] / distances)

    times.append(time)
    momentum_x.append(px)
    momentum_y.append(py)
    momentum_abs.append(p_abs)
    kinetic_energy.append(ek)
    potential_energy.append(ep)
    total_energy.append(ek + ep)


# ===== ГРАФИКИ =====

fig, axes = plt.subplots(2, 1, figsize=(11, 8), sharex=True)

axes[0].plot(times, momentum_abs, label="|P|")
axes[0].plot(times, momentum_x, label="Px", alpha=0.7)
axes[0].plot(times, momentum_y, label="Py", alpha=0.7)
axes[0].set_ylabel("Импульс")
axes[0].set_title("Полный импульс системы")
axes[0].grid(True)
axes[0].legend()

axes[1].plot(times, kinetic_energy, label="Кинетическая энергия")
axes[1].plot(times, potential_energy, label="Потенциальная энергия")
axes[1].plot(times, total_energy, label="Полная энергия", linewidth=2)
axes[1].set_xlabel("Время")
axes[1].set_ylabel("Энергия")
axes[1].set_title("Энергия системы")
axes[1].grid(True)
axes[1].legend()

plt.tight_layout()
plt.savefig("momentum_energy.png", dpi=150)
plt.show()
