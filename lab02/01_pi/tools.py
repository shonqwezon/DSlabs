import pandas as pd
import matplotlib.pyplot as plt


def main():
    data = pd.read_csv("output/results.csv")

    times = data.groupby(
        ["mode", "n", "threads"]
    )["seconds"].median()

    fig, axes = plt.subplots(1, 3, figsize=(16, 5))

    for points in sorted(data["n"].unique()):
        sequential = times.loc[("seq", points, 1)]
        parallel = times.loc[("omp", points)].sort_index()

        threads = parallel.index.to_numpy()
        seconds = parallel.to_numpy()

        speedup = sequential / seconds
        efficiency = speedup / threads

        label = f"N = {points:,}"

        axes[0].plot(threads, seconds, "o-", label=label)
        axes[1].plot(threads, speedup, "o-", label=label)
        axes[2].plot(threads, efficiency, "o-", label=label)

    titles = [
        "Время выполнения",
        "Ускорение",
        "Эффективность",
    ]

    labels = [
        "Время, с",
        "T последовательное / T параллельное",
        "Ускорение / число потоков",
    ]

    for ax, title, label in zip(axes, titles, labels):
        ax.set_title(title)
        ax.set_xlabel("Число потоков")
        ax.set_ylabel(label)
        ax.grid()
        ax.legend()

    plt.tight_layout()
    plt.savefig("output/performance.png", dpi=150)
    plt.show()


if __name__ == "__main__":
    main()
