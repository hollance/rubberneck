# Rubberneck

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
