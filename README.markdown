# Rubberneck

While developing plug-ins, I usually put Rubberneck at the end of the chain for monitoring levels and protecting my eardrums against mistakes that lead to deafening feedback. It also provides some basic utility functions such as flipping polarity.

![Screenshot of Rubberneck](Rubberneck.jpg)

## How to build

Requires JUCE 8.

## TODO

The code isn't great or efficient. Forgive me, it's just a simple development tool. :wink:

- Improve mono support. Right now in Logic it shows up as Mono->Stereo mode, which works wrong... there is one input channel and two output channels, but right now this only renders to left and zeros out right.

-	Other measurements:
    -	True peak?
    -	LUFS?
    -	Block size: tally of which ones and how often / histogram?

## Credits & license

Copyright (C) 2023-2024 M.I. Hollemans

This program is free software: you can redistribute it and/or modify it under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

JUCE is copyright © Raw Material Software.

The font is [Victor Mono](https://rubjo.github.io/victor-mono/) by Rune Bjørnerås. Licensed under the SIL Open Font License, Version 1.1.
