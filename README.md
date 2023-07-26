# CustomSpatialAudioFeatures
Hi! This is a wip repo with some custom spatial audio systems that I am developing in Unreal Engine. 

Here is a basic overview of what I have right now: 
1. Custom emitters cast three lines towards the listener.
2. The information from these three queries is used to calculate the intensity of the filtering.
3. I am caching the listener position so we can stop the line tracing if the listener is not moving.
4. I also check the falloff distance of the emitter and only activate the obstruction once the listener is in the falloff distance. However, this can cause some issues that I will be addressing soon.
5. An obstruction manager where all the audio emitters are registered.

Things coming in the future: 
1. A ring buffer that cycles through all the emitters so the load can be distributed over frames.
2. Better improved check for the falloff distance, so the filtering starts and the value it should be at instead of interpolating to it.

Here is a short (old) video of obstruction working in place: https://youtu.be/DIZFPNkLBeo 
