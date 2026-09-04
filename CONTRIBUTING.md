# Contributing to Smart Vehicle Fare System

Thanks for your interest in improving this project. This is a small, single-file C++ console application built for the BACSE104 (Structured and Object-Oriented Programming) course at VIT, and contributions are welcome from anyone who wants to extend or clean it up further.

## Getting Started

1. Fork the repository and clone your fork locally.
2. Make sure you have a C++17-capable compiler (e.g. `g++ 9+`).
3. Build the project:
   ```bash
   g++ -std=c++17 -Wall -Wextra -o smart_vehicle_fare_system smart_vehicle_fare_system.cpp
   ```
4. Run it:
   ```bash
   ./smart_vehicle_fare_system
   ```

## Project Structure

- `smart_vehicle_fare_system.cpp` — the entire application: exception classes, the `Vehicle` hierarchy, `Customer`, `FareCalculator`, `FileHandler`, `ReceiptGenerator`, `AdminPanel`, and `main()`.
- `trip_history.txt` — generated at runtime; stores a log of completed trips. Not tracked in version control.

## How to Contribute

1. **Open an issue first** for anything beyond a trivial fix (typos, formatting) so the change can be discussed before you invest time in it.
2. **Create a feature branch** off `main`, named descriptively, e.g. `fix/negative-distance-check` or `feature/add-scooter-vehicle`.
3. **Keep changes focused.** One pull request should address one concern (a bug fix, a new vehicle type, a refactor) rather than bundling unrelated changes.
4. **Follow the existing style:**
   - 4-space indentation, no tabs.
   - `PascalCase` for class names, `camelCase` for functions and variables.
   - Prefer `const` correctness and pass-by-reference for non-trivial parameters.
   - New vehicle types should inherit from `Vehicle` and implement `calculateFare()` and `displayDetails()`.
   - Use the existing custom exception types (`InvalidDistanceException`, `InvalidInputException`, `InvalidCouponException`) for validation errors, or add a new one following the same pattern if needed.
5. **Test your changes** by compiling with warnings enabled and manually exercising the affected menu options before opening a pull request:
   ```bash
   g++ -std=c++17 -Wall -Wextra -o smart_vehicle_fare_system smart_vehicle_fare_system.cpp
   ```
6. **Write a clear commit message and PR description** explaining what changed and why.

## Reporting Bugs

When filing an issue, please include:
- Steps to reproduce (which menu options you selected, in order).
- What you expected to happen vs. what actually happened.
- Your compiler and OS, if the issue seems environment-specific.

## Code of Conduct

Be respectful and constructive in issues and pull request discussions. This project is meant to be a learning resource, so questions and beginner-friendly contributions are welcome.

## License

By contributing, you agree that your contributions may be used and redistributed under the same terms as the rest of the project.
