# Rubberneck

-	SIMD
-	with LNF

-	VUMeter (fun!)

-	HPF-LPF (disabled if at 0 or nyquist)

-	measurements:
  -	RMS (is there a standard for this?)
  -	Peak (in dB but also the actual value measured, can be negative too)
  -	True peak?
  -	LUFS?
  -	button to Clear measurements
  -	Display sample rate
  -	Mono vs Stereo mode
  -	Display block size (tally of which ones and how often); histogram?
  -	Current DC offset

-	protectYourEars built into this
  -	with a button to turn it off
  -	with warning light in the UI (green = OK, orange = clipping, red = killed the sound)
-	mono output (+ sides only mode)  [all | mids | sides]
  - see StereoOMG for styling of this control / custom control

- -6 dB and -12 dB buttons (0.5 and 0.25) to change the gain parameter

- bypass button styling
  - also make it go down by one pixel, don't make font smaller
  - also tiny "drop shadow"? (just draw rounded rect with alpha)
- button styling
