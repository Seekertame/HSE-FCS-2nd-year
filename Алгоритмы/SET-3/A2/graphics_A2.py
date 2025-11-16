import pandas as pd
import matplotlib.pyplot as plt

STEP = 20  # берём каждую 20-ю точку
# т.к. если брать каждую, то график слишком замыливыается.


def plot_for_array_type(df: pd.DataFrame, array_type: str):
    sub = df[df["array_type"] == array_type]

    base = sub[sub["algorithm"] == "merge"]
    hybrid = sub[sub["algorithm"] == "merge_insertion"]

    # Чистый MERGE SORT
    plt.figure(figsize=(8, 5))

    base_sorted = base.sort_values("size")
    base_sampled = base_sorted.iloc[::STEP]

    plt.plot(base_sampled["size"], base_sampled["time_ms"], label="merge")
    plt.xlabel("Размер массива")
    plt.ylabel("Время, мс")
    plt.title(f"MERGE SORT — тип массива: {array_type}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"merge_{array_type}.png", dpi=200)
    plt.close()

    # MERGE+INSERTION (по threshold)
    plt.figure(figsize=(8, 5))
    for threshold, grp in hybrid.groupby("threshold"):
        grp_sorted = grp.sort_values("size")
        grp_sampled = grp_sorted.iloc[::STEP]

        plt.plot(
            grp_sampled["size"],
            grp_sampled["time_ms"],
            label=f"threshold={threshold}",
        )

    plt.xlabel("Размер массива")
    plt.ylabel("Время, мс")
    plt.title(f"MERGE+INSERTION — тип массива: {array_type}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"merge_insertion_{array_type}.png", dpi=200)
    plt.close()

    # Сводный график: MERGE vs все MERGE+INSERTION
    plt.figure(figsize=(8, 5))

    # merge
    plt.plot(
        base_sampled["size"],
        base_sampled["time_ms"],
        label="merge",
        linewidth=2,
    )

    # гибриды с разными thresholds
    for threshold, grp in hybrid.groupby("threshold"):
        grp_sorted = grp.sort_values("size")
        grp_sampled = grp_sorted.iloc[::STEP]

        plt.plot(
            grp_sampled["size"],
            grp_sampled["time_ms"],
            label=f"hybrid {threshold}",
        )

    plt.xlabel("Размер массива")
    plt.ylabel("Время, мс")
    plt.title(f"Сравнение MERGE и MERGE+INSERTION — тип: {array_type}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"compare_{array_type}.png", dpi=200)
    plt.close()


def main():
    df = pd.read_csv("data_A2.csv")

    for t in ["random", "reversed", "almost_sorted"]:
        plot_for_array_type(df, t)

    print("Графики сохранены в PNG (merge_*, merge_insertion_*, compare_*).")


if __name__ == "__main__":
    main()
