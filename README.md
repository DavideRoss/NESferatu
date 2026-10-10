# NESferatu

## Core
- [x] Implement 16 pixel mode for sprites
- [] APU
- [] Save states
- [] Remove memory as separate entity and integrate into the CPU
- [] Use `NESFERATU_THROW` everywhere
- [] Tidy up stacktrace usage on throw
- [] Implement proper reset option (soft and hard reset)
- [] Proper logging: system is responsible
- [] Gate every debug function under debug define

## App
- [] Proper frame timing
- [] No hardcoded ROM
- [] IMGui integration
    - [] Load ROM from file
    - [] Execution commands: start, stop, reset, step
    - [] Volatite save states
    - [] Export/import save states to file

## Release
- [] Setup CI build system to release