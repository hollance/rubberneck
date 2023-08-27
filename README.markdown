# Rubberneck

-	SIMD

-	VUMeter (fun!)

-	measurements:
  -	RMS (is there a standard for this?)
  -	Peak (in dB but also the actual value measured, can be negative too)
  -	True peak?
  -	LUFS?
  -	button to Clear measurements
  -	Current DC offset
  -	Display sample rate
  -	Channels in: N out: M
  -	Display block size (tally of which ones and how often); histogram?

-	protectYourEars built into this
  -	with a button to turn it off
  -	with warning light in the UI (green = OK, orange = clipping, red = killed the sound)
    - put this on top of the analysis section; Clear button to clear it

-	HPF-LPF (disabled if at 0 or nyquist)
  - TODO: if the string ends in "k" or "kHz", then value *= 1000.0f;
  - Cytomic SVF
