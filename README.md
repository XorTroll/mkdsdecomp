# Mario Kart DS (EU) reverse-engineering

- [Mario Kart DS (EU) reverse-engineering](#mario-kart-ds-eu-reverse-engineering)
  - [Save editor](#save-editor)
  - [ASM-hacking](#asm-hacking)
  - [Command-line utility](#command-line-utility)
  - [Credits](#credits)

This repository contains my reverse-engineering work of the *European version* of **Mario Kart DS** (game code `AMCP`): ASM-hacking, better understanding of yet not fully documented DS/MKDS formats, additional tools (like a save editor) and hopefully more:

[![YouTube](https://img.youtube.com/vi/bEItGRcRmDU/0.jpg)](https://www.youtube.com/watch?v=bEItGRcRmDU)

> YouTube video showcasing an ASM-hack injection of a custom scene with custom dialogs

<img src="tools/save-editor/screenshots/3.png" alt="drawing" width="75%"/>

<img src="tools/save-editor/screenshots/5.png" alt="drawing" width="75%"/>

> Screenshots of the [online save editor](https://xortroll.github.io/mkds-re/)

## Save editor

Reversing [save-data formats and encryption/decryption](tools/common) allowed me to create an almost fully-featured [online Mario Kart DS save editor](https://xortroll.github.io/mkds-re/):

<img src="tools/save-editor/screenshots/0.png" alt="drawing"/>
<img src="tools/save-editor/screenshots/1.png" alt="drawing"/>
<img src="tools/save-editor/screenshots/2.png" alt="drawing"/>
<img src="tools/save-editor/screenshots/3.png" alt="drawing"/>
<img src="tools/save-editor/screenshots/4.png" alt="drawing"/>
<img src="tools/save-editor/screenshots/5.png" alt="drawing"/>

## ASM-hacking

Check out the [ASM-hacking examples](asmhack-examples) for more detailed information on how to inject custom code in MKDS, using generated [headers](re-export/include) and [symbols](re-export/mkds-eu-decomp-symbols.x) of my reverse-engineering work on this game.

## Command-line utility

For practical purposes (to make my life easier) I made a simple command-line tool, [mkdsutil](tools/mkdsutil), in order to work with several MKDS formats in a more straightforward way (better than staring into a hex editor).

It currently has the following features:

- Decrypt and/or encrypt save data

- Print info/fields of decrypted save data

You will need [CMake](https://cmake.org/) in order to compile it, without any external dependencies other than standard C++.

If you wish to make your own tools or extend these ones, you may be interested in [the C++ code](tools/common) covering various MKDS formats and utils.

## Credits

- [GBATEK](https://problemkaputt.de/gbatek.htm) for its great (although sometimes limited/outdated) DS docs

- [mkds-asm](https://github.com/rocoloco321/mkds-asm) containing some basic reference MKDS RE work

- [Existing MKDS decomp headers](https://github.com/HaroohiePals/MKDS-decomp-headers) for being a great reference for so many internal structs and types

- [Super Mario Wiki](https://www.mariowiki.com/Mario_Kart_DS) for allowing me to understand some basic gameplay aspects without having to spend hours playing or watching gameplay footage :P

- [Pokémon Diamond decompilation](https://github.com/pret/pokediamond) as a helpful reference for already (partially) understood DS SDK code

- [simontime's MKDS time trial code encoder](https://github.com/simontime/MKDSTTEncoder)

- ['Hacking Nintendo DS' EnHacklopedia page](https://doc.kodewerx.org/hacking_nds.html) as the only (and great) specifications I found regarding DS cheat codes

- [NCPatcher](https://github.com/TheGameratorT/NCPatcher) as a great tool for simplifying the process of making DS ASM hacks

- [Custom Mario Kart Wiiki](https://wiki.tockdom.com/wiki/Friend_Code) for providing me some understanding of WFC-related fields and data
