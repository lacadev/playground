import matplotlib.pyplot as plt

from math import sqrt
from pprint import pprint
from random import randint, random, sample


class City:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    @staticmethod
    def distance(city1, city2):
        x_dis = abs(city1.x - city2.x)
        y_dis = abs(city1.y - city2.y)
        distance = sqrt(x_dis ** 2 + y_dis ** 2)
        return distance

    def __repr__(self):
        return f"({self.x}, {self.y})"


class Fitness:
    def __init__(self, route):
        self.route = route
        self._distance = 0
        self._fitness = 0.0

    def route_distance(self):
        # Compute distance if not stored and return it
        if self._distance == 0:
            path_distance = 0
            num_cities_in_route = len(self.route)
            for from_city, to_city in zip(self.route, self.route[1:]):
                path_distance += City.distance(from_city, to_city)
            path_distance += City.distance(self.route[-1], self.route[0])
            self._distance = path_distance
        return self._distance

    def route_fitness(self):
        if self._fitness == 0:
            # The closer fitness is to 1, the better
            # Could be 100 instead of 1, whatever
            self._fitness = 1 / float(self.route_distance())
        return self._fitness


# Function to create new initial individuals
def create_route(city_list):
    # Randomly mix cities to create a route
    return sample(city_list, len(city_list))


# Function to create initial population
def initial_population(population_size, city_list):
    return [create_route(city_list) for _ in range(population_size)]


# Determine fitness
def rank_routes(population):
    fitness_results = [
        (Fitness(route).route_fitness(), i) for i, route in enumerate(population)
    ]
    return sorted(fitness_results, reverse=True)


def selection(ranked_population, elite_size, num_random_groups):
    if elite_size + num_random_groups > len(ranked_population):
        raise Exception(
            "elite_size + num_random_groups cannot be bigger than ranked_population"
        )
    # Choose the elite
    selection_results = [i for _, i in ranked_population[:elite_size]]

    # Probability of having descendants
    # proportionally to the fitness
    fit_sum = sum(fitness for fitness, _ in ranked_population[elite_size:])
    fit_perc = [(fitness / fit_sum, i) for fitness, i in ranked_population[elite_size:]]
    # Select the best from random groups
    for i in range(num_random_groups):
        # Set random perc threshold between min and max perc of
        # individual not belonging to elite group
        min_perc = fit_perc[-1][0]
        max_perc = fit_perc[0][0]
        max_fit_thr = min_perc + random() * (max_perc - min_perc)
        under_thr = [
            (perc_index, pop_index)
            for perc_index, (perc, pop_index) in enumerate(fit_perc)
            if perc <= max_fit_thr
        ]
        if under_thr:
            # Add best of group if group has individuals
            best_under_thr = under_thr[0][1]
            perc_index = under_thr[0][0]
            selection_results.append(best_under_thr)
            del fit_perc[perc_index]

    return selection_results


def mating_pool(population, selected):
    return [population[i] for i in selected]


def breed(parent1, parent2):
    gene_a = int(random() * len(parent1))
    gene_b = int(random() * len(parent1))

    start_gene = min(gene_a, gene_b)
    end_gene = max(gene_a, gene_b)

    l = [gene for gene in parent1[start_gene:end_gene]]

    r = [gene for gene in parent2 if gene not in l]

    return l + r


def breed_population(mating_pool, elite_size):
    # 1337 all the way
    children = mating_pool[:elite_size]

    pool_len = len(mating_pool)
    # Mix the mate pool and create offspring
    # Do twice cause for 2 parents only 1 child is born, meh
    mixed_pool_1 = sample(mating_pool, pool_len)
    for i in range(pool_len - elite_size):
        child = breed(mixed_pool_1[i], mixed_pool_1[pool_len - 1 - i])
        children.append(child)
    mixed_pool_2 = sample(mating_pool, pool_len)
    for i in range(pool_len - elite_size):
        child = breed(mixed_pool_2[i], mixed_pool_2[pool_len - 1 - i])
        children.append(child)

    return children


def mutate(individual, mutation_rate):
    # Swap mutation. Normal mutation would delete or add cities and
    # we do not want that. We can only change the order
    num_genes = len(individual)
    for i in range(num_genes):
        if random() < mutation_rate:
            j = int(random() * num_genes)

            gene1 = individual[i]
            gene2 = individual[j]
            individual[i] = gene2
            individual[j] = gene1

    return individual


def mutate_population(population, mutation_rate):
    return [mutate(individual, mutation_rate) for individual in population]


def next_generation(current_generation, elite_size, num_random_groups, mutation_rate):
    ranked_population = rank_routes(current_generation)
    best_dist = 1 / ranked_population[0][0]
    selected = selection(ranked_population, elite_size, num_random_groups)
    pop_mating_pool = mating_pool(current_generation, selected)
    children = breed_population(pop_mating_pool, elite_size)
    next_gen = mutate_population(children, mutation_rate)
    return next_gen, best_dist


def genetic_algorithm(
    domain, pop_size, elite_size, num_random_groups, mutation_rate, num_generations
):
    population = initial_population(pop_size, domain)
    dist_progress = []

    for i in range(num_generations):
        if i % 1000 == 0:
            print(f"Generation: {i}/{num_generations}")
        population, dist = next_generation(
            population, elite_size, num_random_groups, mutation_rate
        )
        dist_progress.append(dist)
    print(f"Generation: {num_generations}/{num_generations}\n")

    final_best = min(
        [(Fitness(route).route_distance(), i) for i, route in enumerate(population)]
    )
    best_route = population[final_best[1]]
    best_route_dist = final_best[0]
    print(f"Initial best distance: {dist_progress[0]}\n")
    print(f"Final best distance: {best_route_dist}\n")

    plt.plot(dist_progress)
    plt.ylabel("Distance")
    plt.xlabel("Generation")
    plt.show()

    return best_route, best_route_dist


def main():
    max_dist = 200
    num_cities = 25
    pop_size = 200
    elite_size = 50
    num_random_groups = 50
    mutation_rate = 0.01
    num_generations = 2000

    city_list = [
        City(random() * max_dist, random() * max_dist) for _ in range(num_cities)
    ]
    print("Cities (x,y):")
    pprint(city_list)
    print()

    best_route, dist = genetic_algorithm(
        city_list,
        pop_size,
        elite_size,
        num_random_groups,
        mutation_rate,
        num_generations,
    )

    print("Best route (x,y):")
    pprint(best_route)


if __name__ == "__main__":
    main()
