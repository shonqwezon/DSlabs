import sys

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.stats import qmc, norm


def generate(generator, n, seed):
    if n <= 0 or seed <= 0:
        raise ValueError("Число испытаний и seed должны быть положительными")

    if generator == "mcg59":
        modulus = 2 ** 59
        multiplier = 13 ** 13
        state = (2 * seed + 1) % modulus
        u = np.empty(n)

        for i in range(n):
            state = (multiplier * state) % modulus
            u[i] = state / modulus

    elif generator == "sobol":
        power = 0
        size = 1
        while size < n:
            size *= 2
            power += 1
        if 2 ** power != n:
            raise ValueError("Для Соболя выбери число испытаний 2^m")
        sampler = qmc.Sobol(d=1, scramble=True, seed=seed)
        u = sampler.random_base2(power).ravel()

    else:
        raise ValueError("Генератор должен быть mcg59 или sobol")

    u = np.clip(u, np.finfo(float).eps, 1 - np.finfo(float).eps)
    z = norm.ppf(u)
    np.savetxt("output/numbers.txt", z, header=str(n), comments="", fmt="%.17g")


def plot():
    data = pd.read_csv("output/results.csv")
    times = data.groupby(["generator", "n", "mode", "threads"])["seconds"].median()
    names = {"mcg59": "MCG59", "sobol": "Соболь"}

    for generator in ["mcg59", "sobol"]:
        fig, axes = plt.subplots(1, 3, figsize=(16, 5))
        sizes = sorted(data.loc[data["generator"] == generator, "n"].unique())
        for n in sizes:
            sequential = times.loc[(generator, n, "seq", 1)]
            parallel = times.loc[(generator, n, "omp")].sort_index()
            threads = parallel.index.to_numpy()
            seconds = parallel.to_numpy()
            speedup = sequential / seconds
            efficiency = speedup / threads
            for ax, values in zip(axes, [seconds, speedup, efficiency]):
                ax.plot(threads, values, "o-", label=f"Испытаний: {n:,}")

        for ax, title, ylabel in zip(
            axes,
            ["Время расчёта выплат", "Ускорение", "Эффективность"],
            ["Время, с", "Последовательное время / параллельное", "Ускорение / число потоков"],
        ):
            ax.set_title(title)
            ax.set_xlabel("Число потоков")
            ax.set_ylabel(ylabel)
            ax.grid()
            ax.legend(fontsize=8)
        fig.suptitle(f"{names[generator]}: без генерации и чтения данных")
        fig.tight_layout()
        fig.savefig(f"output/performance_{generator}.png", dpi=150)

    fig, axes = plt.subplots(1, 2, figsize=(13, 5))
    sequential = data[data["mode"] == "seq"]
    for generator in ["mcg59", "sobol"]:
        grouped = sequential[sequential["generator"] == generator].groupby("n")
        errors = grouped["error"].apply(lambda x: np.sqrt(np.mean(x ** 2)))
        prices = grouped["price"].mean()
        axes[0].loglog(errors.index, errors, "o-", label=names[generator])
        axes[1].semilogx(prices.index, prices, "o-", label=names[generator])

    exact = sequential["exact"].iloc[0]
    axes[1].axhline(exact, linestyle="--", color="black", label="Блэк–Шоулс")
    axes[0].set_title("Погрешность относительно аналитической цены")
    axes[0].set_ylabel("Корень из среднего квадрата ошибки")
    axes[1].set_title("Средняя оценка цены опциона")
    axes[1].set_ylabel("Цена, денежные единицы")
    for ax in axes:
        ax.set_xlabel("Число испытаний")
        ax.grid()
        ax.legend()
    fig.tight_layout()
    fig.savefig("output/accuracy.png", dpi=150)
    print(f"Цена Блэка–Шоулса: {exact:.10f}")
    plt.show()


if __name__ == "__main__":
    if sys.argv[1] == "generate":
        generate(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]))
    elif sys.argv[1] == "plot":
        plot()
