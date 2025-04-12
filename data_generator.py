import random
import os

numbers = list(range(1_000_000))

random.shuffle(numbers)

with open("1m_data.txt", "w") as file:
    for number in numbers:
        file.write(f"{number} ")