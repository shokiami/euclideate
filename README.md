# Euclideate

A highly optimized algorithm for finding minimal step solutions to straight-edge and compass construction problems.

Optimizations include:
1. Iterative deepening depth-first search for constant memory.
2. Exact arithmetic and field degree constraints for speed.
3. Symmetry detection for pruning the search space.

Compiling:
```
make
```
Running:
```
./main
```

Example: Here is the minimal construction of a square inscribed in a circle.
<img width="100%" alt="Figure_1" src="https://github.com/user-attachments/assets/a96a074f-8801-459e-9823-6b3785413105"/>
