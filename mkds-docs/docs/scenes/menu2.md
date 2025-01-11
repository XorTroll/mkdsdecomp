# `Menu2` scene

This scene appears to be unused.

Loading this scene (using ASM-hacks, for instance) works fine without any crashes, but causes an endless black-screen loop (see below).

This scene perhaps served as a transition scene between `Menu` and `Race` scenes at some early development stage, performing the logic necessary to start the ongoing race/battle/mission/etc, meanwhile displaying a black screen.

> This logic was probably moved to `Menu` scene altogether (TODO: verify this claim)

## Handlers

Note that the "scene context" argument passed to these handler functions is never used in all of them, hence the scene never exits by pointing to a new scene to jump to in said context, causing and endless loop of doing nothing (running the "update" and "vblank" handlers, but never getting to run the "finalize" handler which contains any meaningful logic).

### Initialization

Only the display is configured. Unlike other scenes, no object allocation/scene-unique logic is performed.

### Update

This is an empty function.

### Vblank

This is another empty function.

### Finalization

Interestingly, this is the only place where nontrivial logic happens.

The scene checks what kind the ongoing "next race" will be.

If it's a battle or a mission run, sets fields appropriately in order to display the battle intro (for battles) or directly jump to the race (for missions), as is the usual behaviour.

If it's the award race, it performs some configuration of racers 4 to 8. Note that this check is performed by looking the course ID and checking if it's the `Award` course.

If it's the credits (checking against `StaffRoll`/`StaffRollTrue` courses) it also seems to perform some racer configuration for the credits scene.

It finishes by setting (properly setting) the cup ID value in the "next race" config, after computing it from the course ID.
