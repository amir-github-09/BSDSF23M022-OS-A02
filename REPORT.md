
## Feature 1: Project Setup & Initial Build

### Student
Roll No: BSDSF23M022
Repository: https://github.com/amir-github-09/BSDSF23M022-OS-A02

### Steps completed

1. Created GitHub repo and initialized with README.md.
2. Cloned the instructor starter `ls-v1.0.0` and copied `src/`.
3. Create the Makefile in root
4. Created directories: bin, obj, man.
5. Ran `make` and confirmed `./bin/ls` executes.
6. Committed and pushed initial project state.

### Commands used (exact)
- git clone https://github.com/amir-github-09/BSDSF23M022-OS-A02.git
- mkdir -p bin obj man
- touch bin/.gitkeep obj/.gitkeep man/.gitkeep
- touch REPORT.md
- make
- make clean
- ./bin/ls
- git add .
- git commit -m "Feature1: Initial project setup with starter code"
- git push origin main

### Verification
- `make` created the binary `bin/ls`.
- Running `./bin/ls` listed files correctly.
