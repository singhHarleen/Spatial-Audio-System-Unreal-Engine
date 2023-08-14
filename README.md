# CustomSpatialAudioFeatures
Hi! This is a wip repo with some custom spatial audio systems that I am developing in Unreal Engine. 

Here is a basic overview: 
1. Custom emitters cast five lines displaced vertically and horizontally from the listener.
2. The information from these five queries is used to calculate the intensity of the filtering.
3. Optimisation: The system checks the falloff distance of the emitter and only activates the obstruction once the listener is in the falloff distance
4. Optimisation: I am making use of a circular buffer to cycle through the emitters so the processing load can be spread across frames

Here is a short (old) video of obstruction working in place: https://youtu.be/DIZFPNkLBeo 
