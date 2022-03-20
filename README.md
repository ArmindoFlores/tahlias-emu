# EMU1.0
This is the repository for the code used to solve X-MAS CTF 2020's EMU1.0 challenge. All this code was written during the challenge and has been posteriorly ported to this repository.

## Architecture Description
The description of the computer architecture being emulated is found in the PDF document inlcuded (`Tahlias_EMU_1.0_Manual.pdf`), which was provided during the challenge.

## Usage
To try out the emulator, you can run `make` and then the generated executable, following its intstructions.
To start running the program after the command, type `run`. You can add breakpoints at specific instructions with `break` and clear them with `clear`, or step through instructions one by one using `step`. Examining the stack can be done using `st`. Dump the memory with `memdump` and toggle debug mode with `debug`.
There are a few available ROMs in `roms/`. You can also write new ROMs, and for that you might want to use the assembler script (example assembly code exists in `assembly/`).
