# The Creation and Solving of Mazes

## Abstract

This work focuses on creating dynamic mazes, i.e., the mazes that can change over
time. Dynamic mazes can be represented using cellular automata, but this is an
area that has not been extensively researched. Existing maze generators that use
cellular automata and matrices are limited and do not produce reliable results. This
work proposes a new method of representing cellular automata using graphs and
also offers new combinations of rules and graphs that lead to better results. The
mazes generated using these methods are more interesting from the perspective
of solvability, and some of the generating combinations have been experimentally
verified as reliable and fast in this work.

## Usage

### Clone

Clone this repository to your local machine using the following command:

    git clone --recursive https://github.com/SpeekeR99/BP_2022_2023_Zappe.git

### Build

Project is built using CMake. To build the project, run the following commands:

#### Windows

    mkdir build
    cd build
    cmake ../
    cmake --build . --config Release

#### Linux

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ../
    make

---

# Tvorba a řešení bludišť

## Abstrakt

Tato práce se zaměřuje na vytváření dynamických bludišť, tj. bludišť která se
mohou měnit v čase. Dynamická bludiště je možné reprezentovat pomocí
buněčných automatů, jedná se ale o nepříliš prozkoumané téma. Dosavadní
generátory bludišť, které využívají buněčné automaty a matice, jsou omezené a
nepřinášejí spolehlivé výsledky. Tato práce navrhuje nový způsob reprezentace
buněčných automatů pomocí grafů a nabízí nové kombinace pravidel a grafů, které
vedou k lepším výsledkům. Takto vygenerovaná bludiště jsou zajímavější z pohledu
řešení a některé z generujících kombinací navíc byly v práci ověřeny jako
spolehlivé a rychlé.

## Použití

### Klonování

Na svůj počítač si naklonujte tento repozitář pomocí následujícího příkazu:

    git clone --recursive https://github.com/SpeekeR99/BP_2022_2023_Zappe.git

### Překlad

Projekt je sestaven pomocí CMake. Pro přeložení projektu spusťte následující příkazy:

#### Windows

    mkdir build
    cd build
    cmake ../
    cmake --build . --config Release

#### Linux

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ../
    make