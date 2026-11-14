import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import gc


def main():
    filename = "output/data.csv"
    if len(sys.argv) > 1:
        filename = sys.argv[1]

    print(f"Reading data from {filename}...")
    try:
        data = np.loadtxt(filename, delimiter=",", ndmin=2)
    except Exception as e:
        print(f"Error reading {filename}: {e}")
        return

    if data.size == 0:
        print("Data is empty!")
        return

    times = data[:, 0]
    coords = data[:, 1:]
    n_points = coords.shape[1] // 2
    X = coords[:, 0::2]
    Y = coords[:, 1::2]

    print(f"Loaded {len(times)} frames for {n_points} bodies.")

    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_aspect("equal")
    ax.grid(True, linestyle="--", alpha=0.6)

    # --- Фиксированные оси, центр в 0.0 ---
    LIMIT = 110.0
    ax.set_xlim(-LIMIT, LIMIT)
    ax.set_ylim(-LIMIT, LIMIT)
    ax.axhline(0, color="gray", linewidth=0.5, alpha=0.5)
    ax.axvline(0, color="gray", linewidth=0.5, alpha=0.5)
    ax.plot(0, 0, "k+", markersize=10, zorder=1)

    colors = plt.cm.jet(np.linspace(0, 1, n_points))
    scatter = ax.scatter(X[0], Y[0], s=50, c=colors, zorder=5)
    lines = [
        ax.plot([], [], "-", color=colors[i], alpha=0.5)[0] for i in range(n_points)
    ]
    time_text = ax.text(0.02, 0.95, "", transform=ax.transAxes, fontsize=12)

    # --- Настройки визуализации ---
    dt = 0.001  # шаг симуляции (как в твоей программе)
    fps = 50  # кадров в секунду
    interval = int(1000 / fps)  # 20 мс
    speedup = 1000  # во сколько раз быстрее реального времени

    steps_per_frame = max(1, round(speedup * (interval / 1000) / dt))
    frames_to_skip = steps_per_frame

    # tail_length теперь тоже логичнее задавать в секундах симуляции,
    # а не в кадрах — иначе при разном dt хвост будет разной длины.
    tail_seconds = 0.5
    tail_length = max(1, int(round(tail_seconds / dt / frames_to_skip)))
    closer = {"timer": None}

    def update(frame_idx):
        i = frame_idx * frames_to_skip
        if i >= len(times):
            i = len(times) - 1

        scatter.set_offsets(np.c_[X[i], Y[i]])

        start_idx = max(0, i - tail_length * frames_to_skip)
        for p in range(n_points):
            lines[p].set_data(X[start_idx : i + 1, p], Y[start_idx : i + 1, p])

        time_text.set_text(f"Time: {times[i]:.3g}")

        if frame_idx == num_frames - 1:
            closer["timer"] = fig.canvas.new_timer(interval=300)
            closer["timer"].add_callback(lambda: plt.close(fig))
            closer["timer"].start()

        return [scatter, time_text] + lines

    num_frames = max(1, len(times) // frames_to_skip)

    ani = animation.FuncAnimation(
        fig, update, frames=num_frames, interval=interval, blit=False, repeat=False
    )

    plt.title("N-Body Simulation (fixed axes)")
    plt.show()

    print("Closing and cleaning up...")
    plt.close("all")
    del X, Y, times, data, scatter, lines, ani, fig, ax
    gc.collect()
    print("Done.")


if __name__ == "__main__":
    main()
