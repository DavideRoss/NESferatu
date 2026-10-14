# NESferatu

## Core
- [x] Implement 16 pixel mode for sprites
- [x] APU
- [] Save states
- [] Remove memory as separate entity and integrate into the CPU
- [] Use `NESFERATU_THROW` everywhere
- [] Tidy up stacktrace usage on throw
- [] Implement proper reset option (soft and hard reset)
- [] Gate every debug function under debug define
- [] Debug: CPU/PPU/APU do not expose internal registers, but return a state object
- [] Proper logging: system is responsible

## App
- [X] Proper frame timing
- [] No hardcoded ROM
- [] IMGui integration
    - [] Load ROM from file
    - [] Execution commands: start, stop, reset, step
    - [] Volatite save states
    - [] Export/import save states to file

## Release
- [x] Compilation setup for Windows
- [] Setup CI build system to release

## Bugs
- [] When dragging the SDL window, the emulator should pause, but it catches up on release.