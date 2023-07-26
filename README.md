# CustomSpatialAudioFeatures
Hi! This is a wip repo with some custom spatial audio systems that I am developing in Unreal Engine. 

Here is a basic overview of what I have right now: 
1. Custom emitters that cast three lines towards the listener
2. The information from these three queries is used to calculate the intensity of the filtering
3. I am caching the listener position so we can stop the line tracing if the listener is not moving
4. An obstruction manager where all the audio emitters are registered

Things coming in the future: 
1. A ring buffer that cycles through all the emitters so the load can be distributed over frames 

Here is a short (old) video of obstruction working in place: https://youtu.be/DIZFPNkLBeo 
