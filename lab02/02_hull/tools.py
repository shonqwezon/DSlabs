import sys

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def generate(n, kind, filename):
    rng = np.random.default_rng(42)
    if kind not in ["square", "circle"]:
        raise ValueError("Use square or circle")

    # Write in portions to avoid a second large array for the whole file.
    with open(filename, "w") as file:
        file.write(f"{n}\n")
        for begin in range(0, n, 100000):
            count = min(100000, n - begin)
            if kind == "square":
                points = rng.uniform(-1, 1, (count, 2))
            else:
                indices = np.arange(begin, begin + count)
                angles = 2 * np.pi * indices / n
                points = np.column_stack((np.cos(angles), np.sin(angles)))
            np.savetxt(file, points, fmt="%.17g")


def hull():
    points = np.loadtxt("output/points.txt", skiprows=1, ndmin=2)
    boundary = np.loadtxt("output/hull.txt", skiprows=1, ndmin=2)
    closed = np.vstack((boundary, boundary[0]))

    plt.figure(figsize=(7, 7))
    plt.scatter(points[:, 0], points[:, 1], s=5, label="Points")
    plt.plot(closed[:, 0], closed[:, 1], "r-", label="Convex hull")
    plt.scatter(boundary[:, 0], boundary[:, 1], s=20, color="red")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.axis("equal")
    plt.grid()
    plt.legend()
    plt.tight_layout()
    plt.savefig("output/hull.png", dpi=150)
    plt.show()


def plot():
    data = pd.read_csv("output/results.csv")
    times = data.groupby(["kind", "n", "mode", "threads"])["seconds"].median()

    for kind in data["kind"].unique():
        fig, axes = plt.subplots(1, 3, figsize=(16, 5))
        sizes = sorted(data.loc[data["kind"] == kind, "n"].unique())

        for n in sizes:
            sequential = times.loc[(kind, n, "seq", 1)]
            parallel = times.loc[(kind, n, "omp")].sort_index()
            threads = parallel.index.to_numpy()
            seconds = parallel.to_numpy()
            speedup = sequential / seconds
            efficiency = speedup / threads

            for ax, values in zip(axes, [seconds, speedup, efficiency]):
                ax.plot(threads, values, "o-", label=f"N={n:,}")

        for ax, title, label in zip(
            axes,
            ["Время выполнения", "Ускорение", "Эффективность"],
            [
                "Время, с",
                "T последовательное / T параллельное",
                "Ускорение / число потоков",
            ],
        ):
            ax.set_title(title)
            ax.set_xlabel("Потоки")
            ax.set_ylabel(label)
            ax.grid()
            ax.legend()

        fig.suptitle(kind)
        fig.tight_layout()
        fig.savefig(f"output/performance_{kind}.png", dpi=150)

    plt.show()


if __name__ == "__main__":
    if sys.argv[1] == "generate":
        generate(int(sys.argv[2]), sys.argv[3], sys.argv[4])
    elif sys.argv[1] == "hull":
        hull()
    elif sys.argv[1] == "plot":
        plot()
