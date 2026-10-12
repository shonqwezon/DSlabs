import math

# ===== НАСТРОЙКИ =====

FIELD_WIDTH = 100.0       # Размер поля по X
FIELD_HEIGHT = 100.0      # Размер поля по Y

CLUSTERS_X = 5            # Количество кластеров по горизонтали
CLUSTERS_Y = 5            # Количество кластеров по вертикали
BODIES_PER_CLUSTER = 20    # Количество тел в каждом кластере

MASS = 1.0e7              # Масса каждого тела
CLUSTER_RADIUS = 0.15     # Радиус расположения тел вокруг центра
VELOCITY = 0.005          # Начальная скорость тел

OUTPUT_FILE = "input.txt"

# ======================


def generate_bodies():
    bodies = []

    cluster_count = CLUSTERS_X * CLUSTERS_Y
    total_bodies = cluster_count * BODIES_PER_CLUSTER

    # Равномерно размещаем центры кластеров внутри поля
    step_x = FIELD_WIDTH / CLUSTERS_X
    step_y = FIELD_HEIGHT / CLUSTERS_Y

    for cy in range(CLUSTERS_Y):
        for cx in range(CLUSTERS_X):
            center_x = -FIELD_WIDTH / 2 + step_x * (cx + 0.5)
            center_y = -FIELD_HEIGHT / 2 + step_y * (cy + 0.5)

            for j in range(BODIES_PER_CLUSTER):
                angle = 2 * math.pi * j / BODIES_PER_CLUSTER

                x = center_x + CLUSTER_RADIUS * math.cos(angle)
                y = center_y + CLUSTER_RADIUS * math.sin(angle)

                vx = -VELOCITY * math.sin(angle)
                vy = VELOCITY * math.cos(angle)

                bodies.append(
                    f"{MASS:.6e} {x:.6f} {y:.6f} "
                    f"{vx:.6f} {vy:.6f}"
                )

    with open(OUTPUT_FILE, "w") as f:
        f.write(f"{total_bodies}\n")
        f.write("\n".join(bodies) + "\n")

    print(f"Файл: {OUTPUT_FILE}")
    print(f"Размер поля: {FIELD_WIDTH} x {FIELD_HEIGHT}")
    print(f"Кластеров: {cluster_count}")
    print(f"Тел в кластере: {BODIES_PER_CLUSTER}")
    print(f"Всего тел: {total_bodies}")


if __name__ == "__main__":
    generate_bodies()
