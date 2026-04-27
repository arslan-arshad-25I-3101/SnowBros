# Copilot Instructions

## General Guidelines
- User prefers simpler C++ for this second-semester OOP project and wants to avoid advanced STL headers/features like <algorithm> and <optional> where possible.
- User cannot use `std::vector` in this project and wants vector usage removed.
- User wants code to be written at a first-year Software Engineering level: simple, functional, and not expert-level/over-engineered.
- User prefers first-year level C++ and wants simpler code over advanced patterns.
- Use `using namespace std;` and `using namespace sf;` to avoid repeated `std::`/`sf::` qualifiers.
- User has not studied `constexpr` yet and prefers using `const` instead.
- User prefers faster, concise responses with minimal back-and-forth.
- User wants learning-first help: explain each change, avoid silent autonomous edits, and include clear comments in code so they understand what is happening.

## Project-Specific Rules
- User requested scaffolding-only changes for enemy architecture tasks: create/move header and cpp files without implementing code.
- When the user gives a specific sprite path/coordinates, only apply that requested change and avoid unrelated gameplay/asset fallback modifications.