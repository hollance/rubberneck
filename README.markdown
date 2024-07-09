# Rubberneck

While developing plug-ins, I usually put Rubberneck at the end of the chain for monitoring levels and protecting my ear drums against mistakes that lead to deafening feedback. It also provides some basic utility functions such as flipping polarity.

![Screenshot of Rubberneck](Rubberneck.jpg)

## How to build

JUCE 8

## TODO

-	SIMD
- faster drawing of VU meter

-	other measurements:
  -	True peak?
  -	LUFS?
  -	Block size: tally of which ones and how often / histogram?

- improve mono support
  - it want this to show up as Mono | Stereo in Logic, not Mono->Stereo
  - right now in Logic it shows up as Mono->Stereo mode, which works wrong... there is
    one input channel and two output channels, but right now this only renders to left
    and zeros out right

- do VUMeter as in Levels
- clean up the code

## Credits & license

Copyright (C) 2023-2024 M.I. Hollemans

This program is free software: you can redistribute it and/or modify it under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

JUCE is copyright © Raw Material Software.

The font is [Victor Mono](https://rubjo.github.io/victor-mono/) by Rune Bjørnerås. Licensed under the SIL Open Font License, Version 1.1.
